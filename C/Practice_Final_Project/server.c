#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>

#define PORT (8080)
#define BUFFER_SIZE (1024)
#define MAX_CLIENTS (10)

// Global flag for server state
volatile sig_atomic_t running = 1;

// Signal handler function
void handle_signal(int sig) 
{
    running = 0;
    printf("\nSignal %d received. Server shutting down...\n", sig);
}

int main() 
{
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
    // Set up signal handlers
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);   // Handle Ctrl+C
    sigaction(SIGTERM, &sa, NULL);  // Handle termination request
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    
    // Set up address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_fd, 5) < 0) 
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d...\n", PORT);
    
    // Set up poll structure
    struct pollfd fds[MAX_CLIENTS + 1];
    int nfds = 1;
    
    // Initialize with server socket
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    
    // Initialize remaining slots
    for (int i = 1; i <= MAX_CLIENTS; i++) 
    {
        fds[i].fd = -1;
    }
    
    // Main loop - now using the global running flag
    while(running) 
    {
        // Wait for activity on one of the sockets (use timeout to check running flag)
        int activity = poll(fds, nfds, 1000); // 1 second timeout
        
        if (activity < 0 && errno != EINTR) 
        {
            perror("Poll error");
            break;
        }
        
        // Check for timeout or signal interruption
        if (activity == 0 || !running) 
        {
            continue;
        }
        
        // Check for new connection
        if (fds[0].revents & POLLIN) 
        {
            int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            if (new_socket < 0)
            {
                perror("Accept failed");
                continue;
            }
            
            printf("New connection, socket fd is %d\n", new_socket);
            
            // Add new socket to array of sockets
            int i;
            for (i = 1; i <= MAX_CLIENTS; i++) 
            {
                if (fds[i].fd == -1) 
                {
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;
                    break;
                }
            }
            
            if (i > nfds) nfds = i;
            
            if (i > MAX_CLIENTS) {
                printf("Too many connections\n");
                close(new_socket);
            }
        }
        
        // Check for client activity
        for (int i = 1; i <= MAX_CLIENTS; i++) {
            if (fds[i].fd != -1 && (fds[i].revents & POLLIN)) {
                // Handle data from client
                int valread = read(fds[i].fd, buffer, BUFFER_SIZE);
                
                if (valread <= 0) 
                {
                    // Client disconnected
                    printf("Client disconnected, fd %d\n", fds[i].fd);
                    close(fds[i].fd);
                    fds[i].fd = -1;
                } 
                else 
                {
                    buffer[valread] = '\0';
                    printf("Received from client %d: %s\n", i, buffer);
                    
                    // Send response back
                    char *response = "Hello from server";
                    send(fds[i].fd, response, strlen(response), 0);
                }
            }
        }
    }
    
    // Clean up - close all client sockets
    for (int i = 1; i <= MAX_CLIENTS; i++) {
        if (fds[i].fd != -1) {
            close(fds[i].fd);
        }
    }
    
    // Close the server socket
    close(server_fd);
    printf("Server shut down cleanly\n");
    
    return 0;
}