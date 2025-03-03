/** @file worker.h
 *
 * @brief Interface for worker thread management and operations.
 *
 * This module provides functionality for creating, managing, and executing
 * worker threads that communicate with the bartender server.
 */

 #ifndef WORKER_H
 #define WORKER_H
 
 #include <pthread.h>
 #include <stdbool.h>
 #include "../include/thread_pool.h"
 
 /*************************************************************************
 * Constants and macros
 *************************************************************************/
 
 #define THREAD_SLEEP_USEC   (100000)
 #define IMAGE_PREFIX_LEN    (6)
 #define MAX_MESSAGE_SIZE    (4096)
 #define PREFIX_BUFFER_SIZE  (128)
 #define SUFFIX_BUFFER_SIZE  (128)

 /*************************************************************************
 * Type Definitions
 *************************************************************************/
 
 /**
  * @brief Creates and submits worker threads to handle customer operations
  *
  * @param[in] thread_pool       Initialized thread pool
  * @param[in] prefixes          Array of decoded color prefix strings
  * @param[in] suffix            Terminal reset suffix string
  * @param[in] prefix_count      Number of prefixes/threads to create
  * @param[in] client_turn_lock  Mutex for synchronizing client communication
  * @param[in] socket_fd         Socket for server communication
  * @param[in] dictionary        Dictionary for decoding messages
  *
  * @return true if all workers created successfully, false otherwise
  */
 bool create_workers(thread_pool_t* thread_pool,
                    char prefixes[][PREFIX_BUFFER_SIZE],  // Size should match PREFIX_BUFFER_SIZE
                    char* suffix,
                    int prefix_count,
                    pthread_mutex_t* client_turn_lock,
                    int socket_fd,
                    const char* dictionary);
 
 /**
  * @brief Cleanup function wrapper for thread worker data
  *
  * @param[in] arg Pointer to worker data to free
  */
 void worker_data_cleanup_wrapper(void* arg);
 
 #endif /* WORKER_H */