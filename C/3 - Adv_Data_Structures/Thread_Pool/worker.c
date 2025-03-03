/** @file worker.c
 *
 * @brief Implementation of worker thread management and operations.
 *
 * This module implements the workers that communicate with the bartender server,
 * handle the protocol messaging, and display the song lyrics.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include "worker.h"
 #include "server_message.h"
 #include "syslog.h"
 #include "initialize.h"
 #include "cleanup.h"
 
 /*************************************************************************
 * Private Data Structures
 *************************************************************************/
 
 /**
  * @brief Data structure for worker thread
  */
 typedef struct thread_worker_data
 {
     char              prefix[PREFIX_BUFFER_SIZE];  
     char              suffix[SUFFIX_BUFFER_SIZE]; 
     pthread_mutex_t * client_turn_lock;
     int               socket_fd;
     const char *      dictionary;
 } thread_worker_data_t;
 
 /*************************************************************************
 * Static Function Prototypes
 *************************************************************************/

/**
 * @brief Process a message received from the bartender
 *
 * @param[in] data Worker thread data
 * @param[in] message_buffer Buffer containing the message to process
 * @param[out] decoded_buffer Buffer for decoded image data
 * @return true if processing should continue, false if we should stop receiving
 */
static bool process_bartender_message(thread_worker_data_t* data, 
    const char* message_buffer,
    char* decoded_buffer);

/**
* @brief Receive and process messages from the bartender
*
* @param[in] data Worker thread data
* @return true on successful completion, false on error
*/
static bool handle_bartender_responses(thread_worker_data_t* data);

/**
* @brief Worker thread function that handles bartender communication
*
* @param[in] arg Pointer to thread_worker_data structure
* @return NULL in all cases
*/
static void* thread_worker(void* arg);
 
 /*************************************************************************
 * Public Functions
 *************************************************************************/
 
 bool create_workers(thread_pool_t* thread_pool,
                   char prefixes[][PREFIX_BUFFER_SIZE],
                   char* suffix,
                   int prefix_count,
                   pthread_mutex_t* client_turn_lock,
                   int socket_fd,
                   const char* dictionary)
 {
     // Validate input parameters
     if (NULL == thread_pool || NULL == prefixes || NULL == suffix ||
         prefix_count <= 0 || NULL == client_turn_lock || socket_fd < 0 ||
         NULL == dictionary)
     {
         syslog_write(ERROR, "Invalid parameters for create_workers");
         return false;
     }
 
     // Allocate worker data array
     thread_worker_data_t* worker_data = calloc(prefix_count, sizeof(thread_worker_data_t));
     if (NULL == worker_data)
     {
         syslog_write(CRITICAL, "Failed to allocate worker data");
         return false;
     }
 
     if (!cleanup_add_void(worker_data_cleanup_wrapper, worker_data, 3))
     {
         syslog_write(ERROR, "Failed to add worker data cleanup");
         free(worker_data);
         return false;
     }
 
     // Initialize and submit worker threads
     for (int idx = 0; idx < prefix_count; idx++)
     {
         strncpy(worker_data[idx].prefix, prefixes[idx], sizeof(worker_data[idx].prefix) - 1);
         strncpy(worker_data[idx].suffix, suffix, sizeof(worker_data[idx].suffix) - 1);
         worker_data[idx].client_turn_lock = client_turn_lock;
         worker_data[idx].socket_fd = socket_fd;
         worker_data[idx].dictionary = dictionary;
 
         thread_job_t job = {.job_fn = thread_worker, .p_arg = &worker_data[idx]};
 
         if (thread_pool_submit(thread_pool, &job) != 0)
         {
             syslog_write(ERROR, "Failed to submit thread job for worker %d", idx);
             return false;
         }
     }
 
     syslog_write(INFO, "All worker threads created successfully");
     return true;
 }
 
 void worker_data_cleanup_wrapper(void* arg)
 {
     free(arg);
 }
 
 /*************************************************************************
* Private Functions
*************************************************************************/

/**
 * @brief Process a message received from the bartender
 *
 * @param[in] data Worker thread data
 * @param[in] message_buffer Buffer containing the message to process
 * @param[out] decoded_buffer Buffer for decoded image data
 * @return true if processing should continue, false if we should stop receiving
 */
static bool process_bartender_message(thread_worker_data_t* data, 
    const char* message_buffer,
    char* decoded_buffer)
{
// Check for END message
if (strncmp(message_buffer, "END", 3) == 0)
{
return false;
}

// Check for IMAGE message
if (strncmp(message_buffer, "IMAGE ", IMAGE_PREFIX_LEN) == 0)
{
memset(decoded_buffer, 0, MAX_MESSAGE_SIZE);
if (decode_server_message(message_buffer,
data->dictionary,
decoded_buffer,
MAX_MESSAGE_SIZE)
== 0)
{
puts(data->prefix);
puts(decoded_buffer);
puts(data->suffix);
}
else
{
syslog_write(ERROR, "Failed to decode server message");
}
}

return true;
}

/**
* @brief Receive and process messages from the bartender
*
* @param[in] data Worker thread data
* @return true on successful completion, false on error
*/
static bool handle_bartender_responses(thread_worker_data_t* data)
{
char message_buffer[MAX_MESSAGE_SIZE];
char decoded_buffer[MAX_MESSAGE_SIZE];

while (!is_bar_closed())
{
memset(message_buffer, 0, sizeof(message_buffer));
ssize_t bytes_received = recv(data->socket_fd,
   message_buffer,
   sizeof(message_buffer) - 1,
   0);

if (bytes_received <= 0 || is_bar_closed())
{
return false;
}

message_buffer[bytes_received] = '\0';

// Process the message, exit loop if it returns false
if (!process_bartender_message(data, message_buffer, decoded_buffer))
{
return true;
}
}

return false;
}

/**
* @brief Worker thread function that handles bartender communication
*
* @param[in] arg Pointer to thread_worker_data structure
* @return NULL in all cases
*/
static void* thread_worker(void* arg)
{
thread_worker_data_t* data = (thread_worker_data_t*)arg;
char message_buffer[MAX_MESSAGE_SIZE];

while (!is_bar_closed())
{
// Try to get the lock, but check for bar closed
if (pthread_mutex_lock(data->client_turn_lock) != 0)
{
syslog_write(ERROR, "Failed to acquire mutex");
break;
}

// Check again after getting lock
if (is_bar_closed())
{
pthread_mutex_unlock(data->client_turn_lock);
break;
}

// Prepare and send TAKE_ONE_DOWN message
snprintf(message_buffer,
sizeof(message_buffer),
"TAKE_ONE_DOWN %d\n",
get_window_width());

ssize_t bytes_sent = send(
data->socket_fd, message_buffer, strlen(message_buffer), 0);

if (bytes_sent < 0)
{
syslog_write(ERROR, "Failed to send TAKE_ONE_DOWN message");
pthread_mutex_unlock(data->client_turn_lock);

if (is_bar_closed())
{
break;
}
continue;
}

// Handle responses from bartender until END received
handle_bartender_responses(data);

pthread_mutex_unlock(data->client_turn_lock);

if (is_bar_closed())
{
break;
}

usleep(THREAD_SLEEP_USEC);
}

return NULL;
}