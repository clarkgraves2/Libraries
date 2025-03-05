/** @file poll.h
 *
 * @brief Interface for poll-based event management for network servers.
 *
 * This module provides a simpler event notification mechanism for handling
 * multiple socket connections. It abstracts the standard poll API to provide
 * a clean interface for adding, removing, and monitoring socket events.
 */

 #ifndef POLL_H
 #define POLL_H
 
 #include <stdint.h>
 #include <stdbool.h>
 #include <poll.h>
 
 /*************************************************************************
  * Constants and Macros
  *************************************************************************/
 
 /* Maximum number of file descriptors to monitor */
 #define POLL_MAX_FDS           (64)
 
 /* Default timeout for poll in milliseconds */
 #define POLL_DEFAULT_TIMEOUT   (1000)
 
 /* Error codes */
 #define POLL_SUCCESS           (0)
 #define POLL_ERROR_PARAM       (-1)
 #define POLL_ERROR_FULL        (-2)
 #define POLL_ERROR_NOT_FOUND   (-3)
 #define POLL_ERROR_POLL        (-4)
 
 /*************************************************************************
  * Type Definitions
  *************************************************************************/
 
 /* Event types (matching poll event flags for clarity) */
 typedef enum {
     POLL_EVENT_READ    = POLLIN,     /* Socket readable event */
     POLL_EVENT_WRITE   = POLLOUT,    /* Socket writable event */
     POLL_EVENT_ERROR   = POLLERR,    /* Error condition on socket */
     POLL_EVENT_HUP     = POLLHUP,    /* Hang up event (socket disconnected) */
     POLL_EVENT_NVAL    = POLLNVAL    /* Invalid request */
 } poll_event_type_t;
 
 /* Callback function for event handling */
 typedef void (*poll_callback_t)(int fd, uint16_t events, void *user_data);
 
 /*************************************************************************
  * Function Prototypes
  *************************************************************************/
 
 /**
  * @brief Initialize the poll subsystem
  *
  * Sets up the internal structures needed for event monitoring.
  *
  * @return POLL_SUCCESS on success, negative error code on failure
  */
 int poll_init(void);
 
 /**
  * @brief Shutdown the poll subsystem
  *
  * Releases all resources associated with the poll instance.
  *
  * @return POLL_SUCCESS on success, negative error code on failure
  */
 int poll_shutdown(void);
 
 /**
  * @brief Add a file descriptor to the poll monitoring set
  *
  * @param[in] fd          File descriptor to monitor
  * @param[in] events      Bit mask of events to monitor (POLL_EVENT_*)
  * @param[in] callback    Function to call when events occur
  * @param[in] user_data   User data to pass to the callback function
  *
  * @return POLL_SUCCESS on success, negative error code on failure
  */
 int poll_add(int fd, uint16_t events, poll_callback_t callback, void *user_data);
 
 /**
  * @brief Modify the events being monitored for a file descriptor
  *
  * @param[in] fd          File descriptor to modify
  * @param[in] events      New bit mask of events to monitor
  *
  * @return POLL_SUCCESS on success, negative error code on failure
  */
 int poll_modify(int fd, uint16_t events);
 
 /**
  * @brief Remove a file descriptor from the poll monitoring set
  *
  * @param[in] fd  File descriptor to remove
  *
  * @return POLL_SUCCESS on success, negative error code on failure
  */
 int poll_remove(int fd);
 
 /**
  * @brief Process pending events
  *
  * Waits for events on the monitored file descriptors and dispatches callbacks.
  * This function will block for at most the specified timeout period.
  *
  * @param[in] timeout_ms  Maximum time to wait in milliseconds, or -1 for no timeout
  * @param[out] event_count Number of events processed (can be NULL)
  *
  * @return POLL_SUCCESS on success, negative error code on failure
  */
 int poll_process_events(int timeout_ms, int *event_count);
 
 /**
  * @brief Enter the main event loop
  *
  * Continuously processes events until a shutdown is requested.
  * This function blocks until shutdown.
  *
  * @param[in] timeout_ms  Maximum time to wait in milliseconds between iterations
  *
  * @return POLL_SUCCESS on clean shutdown, negative error code on failure
  */
 int poll_run(int timeout_ms);
 
 /**
  * @brief Request the event loop to stop
  *
  * Signals the event loop to exit cleanly after the current iteration.
  */
 void poll_stop(void);
 
 /**
  * @brief Check if poll is running
  *
  * @return true if the poll subsystem is initialized and running
  */
 bool poll_is_running(void);
 
 #endif /* POLL_H */
 
 /*** end of file ***/