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

#define GET_ADDR_ERR    (-1)
#define GET_SOCK_ERR    (-1)
#define SETSOCKOPT_ERR  (-1)
#define BIND_ERR        (-1)
#define LISTEN_ERR      (-1)

#define PORT_STR        "80"
#define BACKLOG_SIZE    (10)

static void * determine_ip_type(struct sockaddr *sas);
static int setup_listen_socket(void);

int main(void)
{
    int socket_fd;
    int new_fd;
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    char client_ip[INET6_ADDRSTRLEN];

    // Setup main server socket
    socket_fd = setup_listen_socket();
    if (socket_fd < 0)
    {
        fprintf(stderr, "Failed to set up listening socket\n");
        return 1;
    }

    // Main accept loop
    for(;;)
    {
        addr_size = sizeof client_addr;
        new_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size);
        
        if (new_fd == -1) 
        {
            perror("accept");
            continue;
        }

        // Get the client's IP address
        inet_ntop(client_addr.ss_family,
            determine_ip_type((struct sockaddr *)&client_addr),
            client_ip, sizeof client_ip);
        printf("server: got connection from %s\n", client_ip);

        // Handle the connection (example: echo server)
        // For production code, you might want to use fork() or threads
        // to handle multiple clients simultaneously
        
        // Example: Send a welcome message
        if (send(new_fd, "Hello, world!", 13, 0) == -1)
        {
            perror("send");
        }   
        close(new_fd);
    }

    return 0;
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
    
    // Initialize the hints structure for getaddrinfo and
    // clears out the structure in case it was used previously
    memset(&hints, 0, sizeof(hints));
    // IPv4 or IPv6 
    hints.ai_family = AF_UNSPEC;    
    // Declaring TCP Protocol
    hints.ai_socktype = SOCK_STREAM; 

    hints.ai_flags = AI_PASSIVE;     

    // Get address info for the local address we'll bind to.
    get_addr_rv = getaddrinfo(NULL, PORT_STR, &hints, &server_info);
    if (get_addr_rv != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(get_addr_rv));
        return GET_ADDR_ERR;
    }

    // Loop through getaddrinfo results and bind to the address first we can.
    for (cur_addrinfo_ptr = server_info; cur_addrinfo_ptr != NULL; cur_addrinfo_ptr = cur_addrinfo_ptr->ai_next)
    {
        // Create the socket
        socket_fd = socket(cur_addrinfo_ptr->ai_family, cur_addrinfo_ptr->ai_socktype, cur_addrinfo_ptr->ai_protocol);
        if (GET_SOCK_ERR == socket_fd)
        {
            perror("server: socket");
            continue;
        }

        // Set socket options (SO_REUSEADDR to avoid "address already in use" errors) 
        setsockopt_rv = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (setsockopt_rv == SETSOCKOPT_ERR)
        {
            perror("server: setsockopt");
            close(socket_fd);
            freeaddrinfo(server_info);
            return SETSOCKOPT_ERR;
        }

        /* Bind socket to the port */
        bind_rv = bind(socket_fd, cur_addrinfo_ptr->ai_addr, cur_addrinfo_ptr->ai_addrlen);
        if (bind_rv == BIND_ERR)
        {
            perror("server: bind");
            close(socket_fd);
            continue;
        }

        break; 
    }

    // Free the structure since we got the info we needed.
    freeaddrinfo(server_info); 

    // Bind check
    if (cur_addrinfo_ptr == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        return BIND_ERR;
    }

    // Start listening for incoming connections
    listen_rv = listen(socket_fd, BACKLOG_SIZE);
    if (listen_rv == LISTEN_ERR)
    {
        perror("server: listen");
        close(socket_fd);
        return LISTEN_ERR;
    }

    printf("server: waiting for connections on port %s...\n", PORT_STR);
    
    return socket_fd;
}