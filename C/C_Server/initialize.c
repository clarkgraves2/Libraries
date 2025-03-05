/** @file initialize.c
 *
 * @brief Implementation of server initialization and cleanup.
 *
 * This module implements the functions to initialize and clean up
 * all server subsystems, providing a clean modular approach to 
 * server lifecycle management.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include <signal.h>
 #include <unistd.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <errno.h>
 
 #include "initialize.h"
 #include "user_db.h"
 #include "syslog.h"
 #include "signal_handler.h"
 #include "cleanup.h"
 #include "poll.h"
 #include "thread_pool.h"
 
 /*************************************************************************
  * Constants and Macros
  *************************************************************************/
 
 /* Cleanup priority values - higher numbers execute first */
 #define CLEANUP_PRIORITY_POLL        100
 #define CLEANUP_PRIORITY_THREAD_POOL  90
 #define CLEANUP_PRIORITY_SERVER_SOCK  80
 #define CLEANUP_PRIORITY_USER_DB      70
 #define CLEANUP_PRIORITY_LOGGING      10
 
 /*************************************************************************
  * Static Variables
  *************************************************************************/
 
 /* Main server socket */
 static int g_server_socket = -1;
 
 /* Thread pool for handling client requests */
 static thread_pool_t *g_thread_pool = NULL;
 
 /* Flag to indicate if the server is running */
 static volatile bool g_b_running = false;
 
 /* Default server configuration */
 static server_config_t g_config = {
     .port = SERVER_DEFAULT_PORT,
     .backlog = SERVER_DEFAULT_BACKLOG,
     .num_worker_threads = SERVER_DEFAULT_WORKER_THREADS,
     .log_file = SERVER_DEFAULT_LOG_FILE,
     .db_path = SERVER_DEFAULT_DB_PATH
 };
 
 /*************************************************************************
  * Static Function Prototypes
  *************************************************************************/
 
 /**
  * @brief Create and bind the server socket
  *
  * @param[in] port    Port number to bind to
  * @param[in] backlog Listen backlog size
  *
  * @return Socket file descriptor if successful, -1 on error
  */
 static int create_server_socket(int port, int backlog);
 
 /**
  * @brief Initialize logging system
  *
  * @param[in] log_file Path to log file
  *
  * @return true if successful, false otherwise
  */
 static bool init_logging(const char *log_file);
 
 /**
  * @brief Initialize signal handlers
  *
  * @return true if successful, false otherwise
  */
 static bool init_signal_handlers(void);
 
 /**
  * @brief Initialize cleanup system
  *
  * @return true if successful, false otherwise
  */
 static bool init_cleanup(void);
 
 /**
  * @brief Initialize user database
  *
  * @param[in] db_path Path to database file (can be NULL for default)
  *
  * @return true if successful, false otherwise
  */
 static bool init_user_db(const char *db_path);
 
 /**
  * @brief Initialize thread pool
  *
  * @param[in] num_threads Number of worker threads
  *
  * @return true if successful, false otherwise
  */
 static bool init_thread_pool(int num_threads);
 
 /**
  * @brief Initialize poll subsystem
  *
  * @return true if successful, false otherwise
  */
 static bool init_poll(void);
 
 /**
  * @brief Cleanup function for the poll subsystem
  *
  * @param[in] arg Ignored
  * @return 0 on success
  */
 static int cleanup_poll_subsystem(void *arg);
 
 /**
  * @brief Cleanup function for the thread pool
  *
  * @param[in] arg Ignored
  * @return 0 on success
  */
 static int cleanup_thread_pool(void *arg);
 
 /**
  * @brief Cleanup function for the server socket
  *
  * @param[in] arg Ignored
  * @return 0 on success
  */
 static int cleanup_server_socket(void *arg);
 
 /**
  * @brief Cleanup function for the user database
  *
  * @param[in] arg Ignored
  * @return 0 on success
  */
 static int cleanup_user_database(void *arg);
 
 /**
  * @brief Cleanup function for the logging system
  *
  * @param[in] arg Ignored
  * @return 0 on success
  */
 static int cleanup_logging_system(void *arg);
 
 /*************************************************************************
  * Public Functions
  *************************************************************************/
 
 server_config_t server_config_default(void)
 {
     return g_config;
 }
 
 bool server_initialize(const server_config_t *config)
 {
     bool init_success = false;
 
     /* Store configuration for later use */
     if (config != NULL) {
         g_config = *config;
     }
 
     /* Initialize all subsystems in sequence */
     if (!init_logging(g_config.log_file)) {
         fprintf(stderr, "Failed to initialize logging system\n");
         return false;
     }
 
     syslog_write(INFO, SYSLOG_DEST_NONE, "Server starting...");
 
     if (!init_cleanup()) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to initialize cleanup system");
         syslog_shutdown();
         return false;
     }
 
     /* Register cleanup function for logging (executed last) */
     if (!cleanup_add_int(cleanup_logging_system, NULL, CLEANUP_PRIORITY_LOGGING)) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to register logging cleanup function");
         syslog_shutdown();
         return false;
     }
 
     /* Initialize signal handlers */
     if (!init_signal_handlers()) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to initialize signal handlers");
         cleanup_execute();
         return false;
     }
 
     /* Initialize user database */
     if (!init_user_db(g_config.db_path)) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to initialize user database");
         cleanup_execute();
         return false;
     }
 
     /* Register cleanup function for user database */
     if (!cleanup_add_int(cleanup_user_database, NULL, CLEANUP_PRIORITY_USER_DB)) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to register user database cleanup function");
         user_db_cleanup();
         cleanup_execute();
         return false;
     }
 
     /* Initialize thread pool */
     if (!init_thread_pool(g_config.num_worker_threads)) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to initialize thread pool");
         cleanup_execute();
         return false;
     }
 
     /* Register cleanup function for thread pool */
     if (!cleanup_add_int(cleanup_thread_pool, NULL, CLEANUP_PRIORITY_THREAD_POOL)) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to register thread pool cleanup function");
         cleanup_execute();
         return false;
     }
 
     /* Create server socket */
     g_server_socket = create_server_socket(g_config.port, g_config.backlog);
     if (g_server_socket < 0) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to create server socket");
         cleanup_execute();
         return false;
     }
 
     /* Register cleanup function for server socket */
     if (!cleanup_add_int(cleanup_server_socket, NULL, CLEANUP_PRIORITY_SERVER_SOCK)) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to register server socket cleanup function");
         close(g_server_socket);
         g_server_socket = -1;
         cleanup_execute();
         return false;
     }
 
     /* Initialize poll subsystem */
     if (!init_poll()) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to initialize poll subsystem");
         cleanup_execute();
         return false;
     }
 
     /* Register cleanup function for poll subsystem */
     if (!cleanup_add_int(cleanup_poll_subsystem, NULL, CLEANUP_PRIORITY_POLL)) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to register poll cleanup function");
         poll_shutdown();
         cleanup_execute();
         return false;
     }
 
     /* Server is now initialized and running */
     g_b_running = true;
 
     syslog_write(INFO, SYSLOG_DEST_NONE, 
                 "Server initialized successfully, listening on port %d", g_config.port);
 
     return true;
 }
 
 int server_get_socket(void)
 {
     return g_server_socket;
 }
 
 int server_cleanup(void *arg)
 {
     (void)arg;  /* Unused parameter */
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "Performing server cleanup");
     
     g_b_running = false;
     
     /* Execute all registered cleanup functions */
     cleanup_execute();
     
     return 0;
 }
 
 void server_handle_shutdown(void)
 {
     syslog_write(INFO, SYSLOG_DEST_NONE, "Received shutdown signal");
     g_b_running = false;
     poll_stop();
 }
 
 bool server_is_running(void)
 {
     return g_b_running;
 }
 
 /*************************************************************************
  * Static Functions
  *************************************************************************/
 
 static int create_server_socket(int port, int backlog)
 {
     int server_fd;
     int opt = 1;
     struct sockaddr_in addr;
     
     /* Create socket */
     server_fd = socket(AF_INET, SOCK_STREAM, 0);
     if (server_fd < 0) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Socket creation failed: %s", strerror(errno));
         return -1;
     }
     
     /* Set socket options */
     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to set socket options: %s", strerror(errno));
         close(server_fd);
         return -1;
     }
     
     /* Configure server address */
     memset(&addr, 0, sizeof(addr));
     addr.sin_family = AF_INET;
     addr.sin_addr.s_addr = INADDR_ANY;
     addr.sin_port = htons(port);
     
     /* Bind socket to address */
     if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Socket bind failed: %s", strerror(errno));
         close(server_fd);
         return -1;
     }
     
     /* Listen for connections */
     if (listen(server_fd, backlog) < 0) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Listen failed: %s", strerror(errno));
         close(server_fd);
         return -1;
     }
     
     return server_fd;
 }
 
 static bool init_logging(const char *log_file)
 {
     syslog_config_t log_config = {
         .destinations = SYSLOG_DEST_STDOUT | SYSLOG_DEST_FILE,
         .file_path = log_file,
         .min_level = INFO
     };
 
     return syslog_init(&log_config);
 }
 
 static bool init_signal_handlers(void)
 {
     signal_config_t sig_configs[] = {
         {SIGINT, server_handle_shutdown},
         {SIGTERM, server_handle_shutdown}
     };
     
     return sig_handler_init(sig_configs, sizeof(sig_configs) / sizeof(sig_configs[0]));
 }
 
 static bool init_cleanup(void)
 {
     return cleanup_initialize();
 }
 
 static bool init_user_db(const char *db_path)
 {
     return (user_db_init(db_path) == USER_DB_SUCCESS);
 }
 
 static bool init_thread_pool(int num_threads)
 {
     if (num_threads <= 0) {
         num_threads = SERVER_DEFAULT_WORKER_THREADS;
     }
     
     g_thread_pool = thread_pool_initialize(num_threads);
     return (g_thread_pool != NULL);
 }
 
 static bool init_poll(void)
 {
     return (poll_init() == POLL_SUCCESS);
 }
 
 static int cleanup_poll_subsystem(void *arg)
 {
     (void)arg; /* Unused parameter */
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "Cleaning up poll subsystem");
     poll_shutdown();
     
     return 0;
 }
 
 static int cleanup_thread_pool(void *arg)
 {
     (void)arg; /* Unused parameter */
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "Cleaning up thread pool");
     if (g_thread_pool != NULL) {
         thread_pool_shutdown(g_thread_pool);
         thread_pool_destroy(g_thread_pool);
         g_thread_pool = NULL;
     }
     
     return 0;
 }
 
 static int cleanup_server_socket(void *arg)
 {
     (void)arg; /* Unused parameter */
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "Closing server socket");
     if (g_server_socket >= 0) {
         close(g_server_socket);
         g_server_socket = -1;
     }
     
     return 0;
 }
 
 static int cleanup_user_database(void *arg)
 {
     (void)arg; /* Unused parameter */
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "Cleaning up user database");
     user_db_cleanup();
     
     return 0;
 }
 
 static int cleanup_logging_system(void *arg)
 {
     (void)arg; /* Unused parameter */
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "Shutting down logging system");
     syslog_shutdown();
     
     return 0;
 }
 
 /*** end of file ***/