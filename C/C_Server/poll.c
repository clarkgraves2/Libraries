/** @file poll.c
 *
 * @brief Implementation of poll-based event management for network servers.
 *
 * This module implements a straightforward event notification system
 * using the standard poll API. It provides functions to monitor multiple
 * file descriptors for I/O events.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <errno.h>
 #include <pthread.h>
 #include <poll.h>
 
 #include "poll.h"
 #include "syslog.h"
 
 /*************************************************************************
  * Private Data Structures
  *************************************************************************/
 
 /* Structure to store callback and user data for each file descriptor */
 typedef struct {
     int fd;                     /* File descriptor */
     poll_callback_t callback;   /* Callback function */
     void *user_data;            /* User data to pass to callback */
     bool in_use;                /* Whether this slot is in use */
 } fd_data_t;
 
 /*************************************************************************
  * Static Variables
  *************************************************************************/
 
 /* Array of poll structures for poll() */
 static struct pollfd g_poll_fds[POLL_MAX_FDS];
 
 /* Array to store callback and user data for each file descriptor */
 static fd_data_t g_fd_data[POLL_MAX_FDS];
 
 /* Number of file descriptors currently being monitored */
 static int g_fd_count = 0;
 
 /* Mutex for thread-safety */
 static pthread_mutex_t g_poll_mutex = PTHREAD_MUTEX_INITIALIZER;
 
 /* Flag indicating whether the poll subsystem is running */
 static volatile bool g_b_running = false;
 
 /* Flag indicating whether poll has been initialized */
 static volatile bool g_b_initialized = false;
 
 /*************************************************************************
  * Static Function Prototypes
  *************************************************************************/
 
 /**
  * @brief Find the index for a given file descriptor in the arrays
  *
  * @param[in] fd File descriptor to find
  *
  * @return Index in arrays, or -1 if not found
  */
 static int find_fd_index(int fd);
 
 /*************************************************************************
  * Public Functions
  *************************************************************************/
 
 int 
 poll_init(void)
 {
     pthread_mutex_lock(&g_poll_mutex);
 
     /* Check if already initialized */
     if (g_b_initialized) {
         pthread_mutex_unlock(&g_poll_mutex);
         return POLL_SUCCESS;
     }
 
     /* Initialize arrays */
     memset(g_poll_fds, 0, sizeof(g_poll_fds));
     memset(g_fd_data, 0, sizeof(g_fd_data));
     g_fd_count = 0;
 
     /* Mark all slots as unused */
     for (int i = 0; i < POLL_MAX_FDS; i++) {
         g_poll_fds[i].fd = -1;  /* -1 indicates unused slot */
         g_fd_data[i].in_use = false;
     }
 
     g_b_initialized = true;
     
     pthread_mutex_unlock(&g_poll_mutex);
     syslog_write(INFO, SYSLOG_DEST_NONE, "Poll subsystem initialized");
     
     return POLL_SUCCESS;
 }
 
 int 
 poll_shutdown(void)
 {
     pthread_mutex_lock(&g_poll_mutex);
 
     if (!g_b_initialized) {
         pthread_mutex_unlock(&g_poll_mutex);
         return POLL_SUCCESS;
     }
 
     /* Stop the event loop if running */
     g_b_running = false;
 
     /* Reset all data */
     for (int i = 0; i < POLL_MAX_FDS; i++) {
         g_poll_fds[i].fd = -1;
         g_fd_data[i].in_use = false;
     }
 
     g_fd_count = 0;
     g_b_initialized = false;
 
     pthread_mutex_unlock(&g_poll_mutex);
     syslog_write(INFO, SYSLOG_DEST_NONE, "Poll subsystem shutdown");
     
     return POLL_SUCCESS;
 }
 
 int 
 poll_add(int fd, uint16_t events, poll_callback_t callback, void *user_data)
 {
     if (fd < 0 || !callback) {
         return POLL_ERROR_PARAM;
     }
 
     pthread_mutex_lock(&g_poll_mutex);
 
     if (!g_b_initialized) {
         pthread_mutex_unlock(&g_poll_mutex);
         return POLL_ERROR_PARAM;
     }
 
     /* Check if we've reached maximum capacity */
     if (g_fd_count >= POLL_MAX_FDS) {
         pthread_mutex_unlock(&g_poll_mutex);
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                     "Maximum number of monitored FDs reached");
         return POLL_ERROR_FULL;
     }
 
     /* Check if the fd is already monitored */
     int idx = find_fd_index(fd);
     if (idx >= 0) {
         pthread_mutex_unlock(&g_poll_mutex);
         syslog_write(WARNING, SYSLOG_DEST_NONE, 
                     "FD %d is already being monitored", fd);
         return POLL_ERROR_PARAM;
     }
 
     /* Find an empty slot */
     for (idx = 0; idx < POLL_MAX_FDS; idx++) {
         if (g_poll_fds[idx].fd == -1 && !g_fd_data[idx].in_use) {
             break;
         }
     }
 
     /* This shouldn't happen if g_fd_count < POLL_MAX_FDS */
     if (idx >= POLL_MAX_FDS) {
         pthread_mutex_unlock(&g_poll_mutex);
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                     "No empty slots available despite g_fd_count < POLL_MAX_FDS");
         return POLL_ERROR_FULL;
     }
 
     /* Setup the poll structure */
     g_poll_fds[idx].fd = fd;
     g_poll_fds[idx].events = events;
     g_poll_fds[idx].revents = 0;
 
     /* Store callback and user data */
     g_fd_data[idx].fd = fd;
     g_fd_data[idx].callback = callback;
     g_fd_data[idx].user_data = user_data;
     g_fd_data[idx].in_use = true;
 
     g_fd_count++;
 
     pthread_mutex_unlock(&g_poll_mutex);
     syslog_write(DEBUG, SYSLOG_DEST_NONE, 
                 "Added fd %d to poll monitoring", fd);
                 
     return POLL_SUCCESS;
 }
 
 int 
 poll_modify(int fd, uint16_t events)
 {
     if (fd < 0) {
         return POLL_ERROR_PARAM;
     }
 
     pthread_mutex_lock(&g_poll_mutex);
 
     if (!g_b_initialized) {
         pthread_mutex_unlock(&g_poll_mutex);
         return POLL_ERROR_PARAM;
     }
 
     /* Find the fd index */
     int idx = find_fd_index(fd);
     if (idx < 0) {
         pthread_mutex_unlock(&g_poll_mutex);
         return POLL_ERROR_NOT_FOUND;
     }
 
     /* Update the events */
     g_poll_fds[idx].events = events;
 
     pthread_mutex_unlock(&g_poll_mutex);
     syslog_write(DEBUG, SYSLOG_DEST_NONE, 
                 "Modified events for fd %d in poll", fd);
                 
     return POLL_SUCCESS;
 }
 
 int 
 poll_remove(int fd)
 {
     if (fd < 0) {
         return POLL_ERROR_PARAM;
     }
 
     pthread_mutex_lock(&g_poll_mutex);
 
     if (!g_b_initialized) {
         pthread_mutex_unlock(&g_poll_mutex);
         return POLL_ERROR_PARAM;
     }
 
     /* Find the fd index */
     int idx = find_fd_index(fd);
     if (idx < 0) {
         pthread_mutex_unlock(&g_poll_mutex);
         return POLL_ERROR_NOT_FOUND;
     }
 
     /* Mark the slot as unused */
     g_poll_fds[idx].fd = -1;
     g_poll_fds[idx].events = 0;
     g_poll_fds[idx].revents = 0;
     g_fd_data[idx].in_use = false;
 
     g_fd_count--;
 
     pthread_mutex_unlock(&g_poll_mutex);
     syslog_write(DEBUG, SYSLOG_DEST_NONE, 
                 "Removed fd %d from poll monitoring", fd);
                 
     return POLL_SUCCESS;
 }
 
 int 
 poll_process_events(int timeout_ms, int *event_count)
 {
     int num_events;
 
     pthread_mutex_lock(&g_poll_mutex);
 
     if (!g_b_initialized) {
         pthread_mutex_unlock(&g_poll_mutex);
         return POLL_ERROR_PARAM;
     }
 
     /* Nothing to do if no fds are monitored */
     if (g_fd_count == 0) {
         pthread_mutex_unlock(&g_poll_mutex);
         if (event_count) {
             *event_count = 0;
         }
         return POLL_SUCCESS;
     }
 
     /* Make a copy of the poll structures for thread safety */
     struct pollfd poll_fds_copy[POLL_MAX_FDS];
     fd_data_t fd_data_copy[POLL_MAX_FDS];
     int fd_count_copy = g_fd_count;
 
     memcpy(poll_fds_copy, g_poll_fds, sizeof(g_poll_fds));
     memcpy(fd_data_copy, g_fd_data, sizeof(g_fd_data));
 
     pthread_mutex_unlock(&g_poll_mutex);
 
     /* Call poll() */
     num_events = poll(poll_fds_copy, POLL_MAX_FDS, timeout_ms);
 
     /* Handle errors */
     if (num_events < 0) {
         if (errno == EINTR) {
             /* Interrupted by signal, not an error */
             if (event_count) {
                 *event_count = 0;
             }
             return POLL_SUCCESS;
         }
         
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                     "poll() failed: %s", strerror(errno));
         return POLL_ERROR_POLL;
     }
 
     /* Process events */
     int processed_events = 0;
     for (int i = 0; i < POLL_MAX_FDS && processed_events < num_events; i++) {
         /* Skip unused slots and slots with no events */
         if (poll_fds_copy[i].fd == -1 || poll_fds_copy[i].revents == 0) {
             continue;
         }
 
         /* Find the corresponding callback data */
         if (fd_data_copy[i].in_use && 
             fd_data_copy[i].fd == poll_fds_copy[i].fd && 
             fd_data_copy[i].callback) {
             
             /* Call the callback */
             fd_data_copy[i].callback(
                 poll_fds_copy[i].fd, 
                 poll_fds_copy[i].revents, 
                 fd_data_copy[i].user_data
             );
             
             processed_events++;
         }
     }
 
     if (event_count) {
         *event_count = processed_events;
     }
 
     return POLL_SUCCESS;
 }
 
 int 
 poll_run(int timeout_ms)
 {
     if (!g_b_initialized) {
         return POLL_ERROR_PARAM;
     }
 
     /* Set running flag */
     g_b_running = true;
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "Poll event loop starting");
     
     /* Main event loop */
     while (g_b_running) {
         int rc = poll_process_events(timeout_ms, NULL);
         if (rc != POLL_SUCCESS) {
             syslog_write(ERROR, SYSLOG_DEST_NONE, 
                         "Error in poll event loop: %d", rc);
             g_b_running = false;
             return rc;
         }
     }
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "Poll event loop stopped");
     return POLL_SUCCESS;
 }
 
 void 
 poll_stop(void)
 {
     g_b_running = false;
     syslog_write(INFO, SYSLOG_DEST_NONE, "Poll event loop stop requested");
 }
 
 bool 
 poll_is_running(void)
 {
     return g_b_running;
 }
 
 /*************************************************************************
  * Static Functions
  *************************************************************************/
 
 static int 
 find_fd_index(int fd)
 {
     for (int i = 0; i < POLL_MAX_FDS; i++) {
         if (g_poll_fds[i].fd == fd && g_fd_data[i].in_use) {
             return i;
         }
     }
     return -1;
 }
 
 /*** end of file ***/