/** @file tcp_server.c
 *
 * @brief A simple TCP server with proper signal handling and logging.
 *
 * This module implements a TCP server that properly handles signals
 * and uses a structured logging system. It creates a socket to accept
 * incoming connections and demonstrates proper resource cleanup.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <errno.h>
 #include <string.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <sys/wait.h>
 #include <signal.h>
 #include <stdbool.h>
 
 #include "signal_handler.h"
 #include "syslog.h"
 
 /*************************************************************************
  * Constants and Macros
  *************************************************************************/
 
 #define GET_ADDR_ERR    (-1)
 #define GET_SOCK_ERR    (-1)
 #define SETSOCKOPT_ERR  (-1)
 #define BIND_ERR        (-1)
 #define LISTEN_ERR      (-1)
 #define BACKLOG_SIZE    (10)
 #define PORT_STR        "80"
 #define SERVER_MSG      "Hello, world!"
 
 /*************************************************************************
  * Static Variables
  *************************************************************************/
 
 static int g_socket_fd = -1;
 static volatile bool g_b_running = true;
 
 /*************************************************************************
  * Static Function Prototypes
  *************************************************************************/
 
 static void *determine_ip_type(struct sockaddr *sas);
 static int  setup_listen_socket(void);
 static void cleanup_resources(void);
 static void handle_client_connection(int client_fd, struct sockaddr_storage *p_client_addr);
 static void sigint_handler(void);
 static void sigterm_handler(void);
 
 /*************************************************************************
  * Static Functions
  *************************************************************************/
 
 /**
  * Handles SIGINT signal (Ctrl+C)
  */
 static void
 sigint_handler(void)
 {
     syslog_write(INFO, SYSLOG_DEST_NONE, "SIGINT received, shutting down...");
     g_b_running = false;
 }
 
 /**
  * Handles SIGTERM signal
  */
 static void
 sigterm_handler(void)
 {
     syslog_write(INFO, SYSLOG_DEST_NONE, "SIGTERM received, shutting down...");
     g_b_running = false;
 }
 
 /**
  * Cleans up resources before exiting
  */
 static void
 cleanup_resources(void)
 {
     if (g_socket_fd >= 0)
     {
         syslog_write(INFO, SYSLOG_DEST_NONE, "Closing server socket");
         close(g_socket_fd);
         g_socket_fd = -1;
     }
     
     syslog_shutdown();
 }
 
 /**
  * Function to get sockaddr, IPv4 or IPv6
  *
  * @param[in] sas Socket address structure
  * @return    Pointer to the appropriate address structure
  */
 static void *
 determine_ip_type(struct sockaddr *sas)
 {
     if (sas->sa_family == AF_INET)
     {
         return &(((struct sockaddr_in*)sas)->sin_addr);
     }
     return &(((struct sockaddr_in6*)sas)->sin6_addr);
 }
 
 /**
  * Handles an individual client connection
  *
  * @param[in] client_fd     Client socket file descriptor
  * @param[in] p_client_addr Pointer to client address structure
  */
 static void
 handle_client_connection(int client_fd, struct sockaddr_storage *p_client_addr)
 {
     char client_ip[INET6_ADDRSTRLEN];
     
     // Get the client's IP address
     inet_ntop(p_client_addr->ss_family,
         determine_ip_type((struct sockaddr *)p_client_addr),
         client_ip, sizeof(client_ip));
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "Connection from %s", client_ip);
     
     // Send a welcome message
     if (send(client_fd, SERVER_MSG, strlen(SERVER_MSG), 0) == -1)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to send data: %s", strerror(errno));
     }
     
     // Close the connection
     close(client_fd);
 }
 
 /**
  * Sets up the listening socket
  *
  * @return Socket file descriptor or -1 on error
  */
 static int 
 setup_listen_socket(void)
 {
     int             socket_fd;
     int             yes = 1;
     int             get_addr_rv;
     int             setsockopt_rv;
     int             bind_rv;
     int             listen_rv;
     struct addrinfo hints;
     struct addrinfo *server_info;
     struct addrinfo *cur_addrinfo_ptr;
     
     // Initialize the hints structure for getaddrinfo
     memset(&hints, 0, sizeof(hints));
     hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6 
     hints.ai_socktype = SOCK_STREAM; // TCP Protocol
     hints.ai_flags = AI_PASSIVE;     // Use local IP
 
     // Get address info for the local address we'll bind to
     get_addr_rv = getaddrinfo(NULL, PORT_STR, &hints, &server_info);
     if (get_addr_rv != 0)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                     "getaddrinfo error: %s", gai_strerror(get_addr_rv));
         return GET_ADDR_ERR;
     }
 
     // Loop through getaddrinfo results and bind to the first we can
     for (cur_addrinfo_ptr = server_info; cur_addrinfo_ptr != NULL; 
          cur_addrinfo_ptr = cur_addrinfo_ptr->ai_next)
     {
         // Create the socket
         socket_fd = socket(cur_addrinfo_ptr->ai_family, 
                            cur_addrinfo_ptr->ai_socktype, 
                            cur_addrinfo_ptr->ai_protocol);
         if (GET_SOCK_ERR == socket_fd)
         {
             syslog_write(WARNING, SYSLOG_DEST_NONE, 
                         "socket creation failed: %s", strerror(errno));
             continue;
         }
 
         // Set socket options (SO_REUSEADDR to avoid "address already in use" errors) 
         setsockopt_rv = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
         if (setsockopt_rv == SETSOCKOPT_ERR)
         {
             syslog_write(ERROR, SYSLOG_DEST_NONE, 
                         "setsockopt failed: %s", strerror(errno));
             close(socket_fd);
             freeaddrinfo(server_info);
             return SETSOCKOPT_ERR;
         }
 
         // Bind socket to the port
         bind_rv = bind(socket_fd, cur_addrinfo_ptr->ai_addr, cur_addrinfo_ptr->ai_addrlen);
         if (bind_rv == BIND_ERR)
         {
             syslog_write(WARNING, SYSLOG_DEST_NONE, 
                         "bind failed: %s", strerror(errno));
             close(socket_fd);
             continue;
         }
        
        // No error bind sucessful.
        break; 
     }
 
     // Free the structure since we got the info we needed
     freeaddrinfo(server_info); 
 
     // Bind check
     if (cur_addrinfo_ptr == NULL)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to bind to any address");
         return BIND_ERR;
     }
 
     // Start listening for incoming connections
     listen_rv = listen(socket_fd, BACKLOG_SIZE);
     if (listen_rv == LISTEN_ERR)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, 
                     "listen failed: %s", strerror(errno));
         close(socket_fd);
         return LISTEN_ERR;
     }
 
     syslog_write(INFO, SYSLOG_DEST_NONE, "Server waiting for connections on port %s", PORT_STR);
     
     return socket_fd;
 }
 
 /*************************************************************************
  * Public Functions
  *************************************************************************/
 
 /**
  * Main entry point for the TCP server application
  */
 int 
 main(void)
 {
     int new_fd;
     struct sockaddr_storage client_addr;
     socklen_t addr_size;
     
     // Initialize the logging system
     syslog_config_t log_config = {
         .destinations = SYSLOG_DEST_STDOUT | SYSLOG_DEST_FILE,
         .file_path = "tcp_server.log",
         .min_level = INFO
     };
     
     if (!syslog_init(&log_config))
     {
         fprintf(stderr, "Failed to initialize logging system\n");
         return 1;
     }
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "TCP Server starting up");
     
     // Register signal handlers
     signal_config_t sig_configs[] = {
         {SIGINT, sigint_handler},
         {SIGTERM, sigterm_handler}
     };
     
     if (!sig_handler_init(sig_configs, sizeof(sig_configs) / sizeof(sig_configs[0])))
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to initialize signal handlers");
         syslog_shutdown();
         return 1;
     }
     
     // Setup main server socket
     g_socket_fd = setup_listen_socket();
     if (g_socket_fd < 0)
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to set up listening socket");
         syslog_shutdown();
         return 1;
     }
     
     // Register cleanup handler to run at exit
     atexit(cleanup_resources);
     
     // Main accept loop
     while (g_b_running)
     {
         addr_size = sizeof(client_addr);
         new_fd = accept(g_socket_fd, (struct sockaddr *)&client_addr, &addr_size);
         
         if (new_fd == -1)
         {
             if (errno == EINTR)
             {
                 // Interrupted by signal, check if we should continue running
                 if (!g_b_running)
                 {
                     break;
                 }
                 continue;
             }
             
             syslog_write(ERROR, SYSLOG_DEST_NONE, 
                         "accept failed: %s", strerror(errno));
             continue;
         }
         
         // Handle the client connection
         handle_client_connection(new_fd, &client_addr);
     }
     
     syslog_write(INFO, SYSLOG_DEST_NONE, "TCP Server shutting down gracefully");
     
     return 0;
 }