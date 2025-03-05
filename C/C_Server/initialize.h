/** @file initialize.h
 *
 * @brief Interface for server initialization and cleanup.
 *
 * This module provides functions to initialize and cleanup all server
 * subsystems, abstracting the details of initialization sequence and
 * resource management from the main application logic.
 */

 #ifndef INITIALIZE_H
 #define INITIALIZE_H
 
 #include <stdbool.h>
 
 /*************************************************************************
  * Type Definitions
  *************************************************************************/
 
 /**
  * @brief Server configuration structure
  */
 typedef struct server_config 
 {
     int port;                /* Server listening port */
     int backlog;             /* Listen backlog size */
     int num_worker_threads;  /* Number of worker threads in thread pool */
     const char *log_file;    /* Path to log file */
     const char *db_path;     /* Path to database file (NULL for default) */
 } server_config_t;
 
 /*************************************************************************
  * Constants
  *************************************************************************/
 
 /* Default server configuration */
 #define SERVER_DEFAULT_PORT             8080
 #define SERVER_DEFAULT_BACKLOG          10
 #define SERVER_DEFAULT_WORKER_THREADS   4
 #define SERVER_DEFAULT_LOG_FILE         "server.log"
 #define SERVER_DEFAULT_DB_PATH          NULL
 
 /*************************************************************************
  * Function Prototypes
  *************************************************************************/
 
 /**
  * @brief Initialize all server subsystems
  *
  * Initializes logging, signal handlers, cleanup system, user database,
  * thread pool, and network related components based on the provided
  * configuration.
  *
  * @param[in] config Server configuration
  *
  * @return true if initialization successful, false on any error
  */
 bool server_initialize(const server_config_t *config);
 
 /**
  * @brief Create a server configuration with default values
  *
  * @return Default server configuration
  */
 server_config_t server_config_default(void);
 
 /**
  * @brief Get the server socket file descriptor
  *
  * @return Server socket file descriptor, or -1 if not initialized
  */
 int server_get_socket(void);
 
 /**
  * @brief Clean up all server resources
  *
  * This function is automatically registered with the cleanup system
  * during initialization, but can also be called directly if needed.
  *
  * @return 0 on success
  */
 int server_cleanup(void *arg);
 
 /**
  * @brief Handle shutdown signals
  *
  * Signal handler callback for SIGINT, SIGTERM, etc.
  */
 void server_handle_shutdown(void);
 
 /**
  * @brief Check if the server is running
  *
  * @return true if the server is running, false otherwise
  */
 bool server_is_running(void);
 
 #endif /* INITIALIZE_H */
 
 /*** end of file ***/