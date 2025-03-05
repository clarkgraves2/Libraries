/** @file server_main.c
 *
 * @brief Main entry point for the server application.
 *
 * This file serves as the entry point for the server application,
 * initializing all required subsystems through the initialize module
 * and setting up the main event loop for handling client connections.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include <unistd.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <errno.h>
 
 #include "initialize.h"
 #include "poll.h"
 #include "thread_pool.h"
 #include "syslog.h"
 #include "cleanup.h"
 
 /*************************************************************************
  * Constants and Macros
  *************************************************************************/
 
 #define POLL_TIMEOUT_MS     (1000)
 
 /*************************************************************************
  * Static Function Prototypes
  *************************************************************************/
 
 /**
  * @brief Handle new client connections
  *
  * @param fd        Server socket file descriptor
  * @param events    Poll events that triggered this callback
  * @param user_data User data associated with the socket (unused)
  */
 static void handle_new_connection(int fd, uint16_t events, void *user_data);
 
 /**
  * @brief Thread pool job function to handle a client request
  *
  * @param p_arg Pointer to client socket file descriptor
  */
 static void handle_client_request(void *p_arg);
 
 /*************************************************************************
  * Main Function
  *************************************************************************/
 
 int main(void)
 {
     /* Set up server with default configuration */
     server_config_t config = server_config_default();
     
     /* Initialize all server subsystems */
     if (!server_initialize(&config)) 
     {
         fprintf(stderr, "Failed to initialize server\n");
         return EXIT_FAILURE;
     }
 
     /* Get server socket */
     int server_socket = server_get_socket();
     
     /* Add server socket to poll */
     if (poll_add(server_socket, POLL_EVENT_READ, handle_new_connection, NULL) != POLL_SUCCESS) 
     {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to add server socket to poll");
         cleanup_execute();
         return EXIT_FAILURE;
     }
 
     /* Main server loop */
     while (server_is_running()) 
     {
         if (poll_process_events(POLL_TIMEOUT_MS, NULL) != POLL_SUCCESS) 
         {
             syslog_write(ERROR, SYSLOG_DEST_NONE, "Error processing poll events");
             break;
         }
     }
 
     syslog_write(INFO, SYSLOG_DEST_NONE, "Server shutting down...");
     
     /* Execute cleanup */
     cleanup_execute();
     
     return EXIT_SUCCESS;
 }
 
 /*************************************************************************
  * Static Function Implementations
  *************************************************************************/
 
 /**
  * @brief Handle new client connections
  */
 static void handle_new_connection(int fd, uint16_t events, void *user_data)
 {
     struct sockaddr_in client_addr;
     socklen_t client_addr_len = sizeof(client_addr);
     int client_fd;
     
     /* Check for read event */
     if (!(events & POLL_EVENT_READ)) 
     {
         /* This shouldn't happen as we only register for read events */
         syslog_write(WARNING, SYSLOG_DEST_NONE, 
                     "Server socket received unexpected event: %d", events);
         return;
     }
     
     /* Accept incoming connection */
     client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
     if (client_fd < 0) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to accept connection: %s", strerror(errno));
         return;
     }
     
     char client_ip[INET_ADDRSTRLEN];
     inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
     syslog_write(INFO, SYSLOG_DEST_NONE, "New connection from %s:%d", client_ip, ntohs(client_addr.sin_port));
     
     /* Allocate memory for the client socket file descriptor */
     int *p_client_fd = malloc(sizeof(int));
     if (p_client_fd == NULL) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to allocate memory for client connection");
         close(client_fd);
         return;
     }
     
     *p_client_fd = client_fd;
     
     /* Create a job for the thread pool */
     thread_job_t job = {
         .job_fn = handle_client_request,
         .p_arg = p_client_fd
     };
     
     /* Submit job to the thread pool */
     if (thread_pool_submit(NULL, &job) != 0) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Failed to submit job to thread pool");
         free(p_client_fd);
         close(client_fd);
     }
 }
 
 /**
  * @brief Thread pool job function to handle a client request
  */
 static void handle_client_request(void *p_arg)
 {
     int client_fd = *((int *)p_arg);
     char buffer[1024] = {0};
     ssize_t bytes_read;
     
     /* Free the allocated memory for the file descriptor */
     free(p_arg);
     
     /* Read client request */
     bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
     if (bytes_read <= 0) {
         if (bytes_read < 0) {
             syslog_write(ERROR, SYSLOG_DEST_NONE, "Error reading from client: %s", strerror(errno));
         }
         close(client_fd);
         return;
     }
     
     buffer[bytes_read] = '\0';
     syslog_write(INFO, SYSLOG_DEST_NONE, "Received from client: %s", buffer);
     
     /* Process the request (this is where you would add your application logic) */
     /* ... */
     
     /* For this example, just echo the request back */
     if (send(client_fd, buffer, bytes_read, 0) != bytes_read) {
         syslog_write(ERROR, SYSLOG_DEST_NONE, "Error sending response to client: %s", strerror(errno));
     }
     
     /* Close the connection */
     close(client_fd);
 }