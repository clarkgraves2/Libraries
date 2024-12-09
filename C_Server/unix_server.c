/** @file unix_server.c
 *
 * @brief Unix domain socket server implementation.
 *
 * @par
 * This server creates a Unix domain socket, handles client connections,
 * and implements graceful shutdown on signal reception.
 *
 * COPYRIGHT NOTICE: (c) 2024 Your Company. All rights reserved.
 */

#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdarg.h>
#include "unix_server.h"

/* Constants */
#define MAX_ERROR_MSG_LEN 256

/* Global Variables */
static volatile sig_atomic_t g_keep_running = 1;

/* Function Prototypes */
static void unix_server_signal_handler(int32_t sig, siginfo_t *info, void *context);
static void unix_server_cleanup(int32_t socket_fd, uint8_t *msg_buffer, 
                              size_t msg_size, uint8_t *data_buffer,
                              size_t data_size);
static unix_server_error_t unix_server_setup_signals(uint8_t *error_buffer, 
                                                   size_t buffer_size);
static unix_server_error_t unix_server_create_socket(int32_t *socket_fd,
                                                   uint8_t *error_buffer, 
                                                   size_t buffer_size);
static unix_server_error_t unix_server_handle_bind_error(int32_t error_code, 
                                                       uint8_t *error_buffer, 
                                                       size_t buffer_size);
static unix_server_error_t unix_server_handle_listen_error(int32_t error_code,
                                                         uint8_t *error_buffer, 
                                                         size_t buffer_size);
static void log_error(const char *format, ...);
static void log_info(const char *format, ...);
static unix_server_error_t handle_client_connection(int32_t client_fd,
                                                  uint8_t *msg_buffer,
                                                  size_t buffer_size);

int32_t
main(void)
{
    int32_t base_socket = -1;
    socklen_t socket_length = 0;
    uint8_t msg_buffer[UNIX_SERVER_MAX_MSG_LEN] = {0};
    uint8_t data_buffer[UNIX_SERVER_MAX_MSG_LEN] = {0};
    unix_server_error_t result = UNIX_SERVER_SUCCESS;
    
    /* Set up signal handling */
    result = unix_server_setup_signals(msg_buffer, sizeof(msg_buffer));
    if (result != UNIX_SERVER_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    /* Create and initialize socket */
    result = unix_server_create_socket(&base_socket, msg_buffer, sizeof(msg_buffer));
    if (result != UNIX_SERVER_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    /* Socket address structure */
    struct sockaddr_un addr_local;
    
    /* Initialize socket address structure */
    addr_local.sun_family = AF_UNIX;
    
    /* Use safer string operation for path */
    if (snprintf(addr_local.sun_path, sizeof(addr_local.sun_path), "%s", 
                UNIX_SERVER_SOCK_PATH) >= sizeof(addr_local.sun_path))
    {
        log_error("Socket path too long");
        unix_server_cleanup(base_socket, msg_buffer, sizeof(msg_buffer),
                          data_buffer, sizeof(data_buffer));
        return EXIT_FAILURE;
    }

    /* Set restrictive permissions before creating socket file */
    umask(077);
    
    /* Remove existing socket file */
    (void)unlink(addr_local.sun_path);
    
    socket_length = sizeof(struct sockaddr_un);
    
    /* Bind socket to address */
    if (bind(base_socket, (struct sockaddr *)&addr_local, socket_length) == -1) 
    {
        result = unix_server_handle_bind_error(errno, msg_buffer, sizeof(msg_buffer));
        unix_server_cleanup(base_socket, msg_buffer, sizeof(msg_buffer),
                          data_buffer, sizeof(data_buffer));
        return EXIT_FAILURE;
    }

    /* Set socket to listen state */
    if (listen(base_socket, UNIX_SERVER_LISTEN_BACKLOG) == -1)
    {
        result = unix_server_handle_listen_error(errno, msg_buffer, sizeof(msg_buffer));
        unix_server_cleanup(base_socket, msg_buffer, sizeof(msg_buffer),
                          data_buffer, sizeof(data_buffer));
        return EXIT_FAILURE;
    }

    log_info("Server started. Listening for connections...");

    /* Main server loop */
    while (g_keep_running)
    {
        /* Accept client connections */
        struct sockaddr_un client_addr;
        socklen_t client_len = sizeof(client_addr);
        int32_t client_fd;

        client_fd = accept(base_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1)
        {
            if (errno == EINTR)
            {
                continue;  /* Interrupted system call, try again */
            }
            log_error("Error accepting connection: %s", strerror(errno));
            continue;
        }

        /* Handle client connection */
        result = handle_client_connection(client_fd, msg_buffer, sizeof(msg_buffer));
        if (result != UNIX_SERVER_SUCCESS)
        {
            log_error("Error handling client connection");
        }

        close(client_fd);
    }

    log_info("Server shutting down...");
    unix_server_cleanup(base_socket, msg_buffer, sizeof(msg_buffer),
                       data_buffer, sizeof(data_buffer));
    return EXIT_SUCCESS;
}

/**
 * @brief Sets up signal handlers for graceful server termination
 *
 * @param[out] error_buffer Buffer for error messages
 * @param[in]  buffer_size Size of error buffer
 *
 * @return UNIX_SERVER_SUCCESS on success, error code on failure
 */
static unix_server_error_t
unix_server_setup_signals(uint8_t *error_buffer, size_t buffer_size)
{
    struct sigaction act = {0};
    
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = unix_server_signal_handler;
    
    if (sigemptyset(&act.sa_mask) == -1)
    {
        if (strerror_r(errno, (char *)error_buffer, buffer_size) == 0)
        {
            log_error("Failed to initialize signal mask: %s", error_buffer);
        }
        return UNIX_SERVER_ERR_SIGNAL;
    }
    
    if ((sigaction(SIGTERM, &act, NULL) == -1) || 
        (sigaction(SIGINT, &act, NULL) == -1))
    {
        if (strerror_r(errno, (char *)error_buffer, buffer_size) == 0)
        {
            log_error("Failed to install signal handlers: %s", error_buffer);
        }
        return UNIX_SERVER_ERR_SIGNAL;
    }

    return UNIX_SERVER_SUCCESS;
}

/**
 * @brief Signal handler for graceful server shutdown
 *
 * @param[in] sig Signal number
 * @param[in] info Signal information structure
 * @param[in] context Signal context
 */
static void 
unix_server_signal_handler(int32_t sig, siginfo_t *info, void *context)
{
    /* Mark parameters as used */
    (void)sig;
    (void)info;
    (void)context;
    
    g_keep_running = 0;
}

/**
 * @brief Logging function for error messages
 *
 * @param[in] format Printf-style format string
 * @param[in] ... Variable arguments
 */
static void
log_error(const char *format, ...)
{
    time_t now;
    char timestamp[26];
    va_list args;
    
    time(&now);
    ctime_r(&now, timestamp);
    timestamp[24] = '\0';  /* Remove newline */
    
    fprintf(stderr, "[%s] ERROR: ", timestamp);
    
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
}

/**
 * @brief Logging function for informational messages
 *
 * @param[in] format Printf-style format string
 * @param[in] ... Variable arguments
 */
static void
log_info(const char *format, ...)
{
    time_t now;
    char timestamp[26];
    va_list args;
    
    time(&now);
    ctime_r(&now, timestamp);
    timestamp[24] = '\0';  /* Remove newline */
    
    printf("[%s] INFO: ", timestamp);
    
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("\n");
}

/**
 * @brief Handle a client connection
 *
 * @param[in] client_fd Client socket file descriptor
 * @param[out] msg_buffer Buffer for messages
 * @param[in] buffer_size Size of message buffer
 *
 * @return UNIX_SERVER_SUCCESS on success, error code on failure
 */
static unix_server_error_t
handle_client_connection(int32_t client_fd, uint8_t *msg_buffer, size_t buffer_size)
{
    ssize_t bytes_read;
    
    /* Read client message */
    bytes_read = read(client_fd, msg_buffer, buffer_size - 1);
    if (bytes_read < 0)
    {
        log_error("Error reading from client: %s", strerror(errno));
        return UNIX_SERVER_ERR_READ;
    }
    
    msg_buffer[bytes_read] = '\0';
    log_info("Received message from client: %s", msg_buffer);
    
    /* Echo message back to client */
    if (write(client_fd, msg_buffer, bytes_read) != bytes_read)
    {
        log_error("Error writing to client: %s", strerror(errno));
        return UNIX_SERVER_ERR_WRITE;
    }
    
    return UNIX_SERVER_SUCCESS;
}

/**
 * @brief Cleanup server resources
 *
 * @param[in] socket_fd Socket file descriptor to close
 * @param[in,out] msg_buffer Message buffer to clear
 * @param[in] msg_size Size of message buffer
 * @param[in,out] data_buffer Data buffer to clear
 * @param[in] data_size Size of data buffer
 */
static void 
unix_server_cleanup(int32_t socket_fd, uint8_t *msg_buffer, 
                   size_t msg_size, uint8_t *data_buffer,
                   size_t data_size)
{    
    /* Close socket if open */
    if (socket_fd > 0) 
    {
        if (close(socket_fd) == -1)
        {
            if (strerror_r(errno, (char *)msg_buffer, msg_size) == 0)
            {
                printf("Warning: Failed to close socket: %s\n", msg_buffer);
            }
        }
    }
    
    /* Remove socket file */
    if (unlink(UNIX_SERVER_SOCK_PATH) == -1)
    {
        if (strerror_r(errno, (char *)msg_buffer, msg_size) == 0)
        {
            printf("Warning: Failed to remove socket file: %s\n", msg_buffer);
        }
    }
    
    /* Clear sensitive data */
    explicit_bzero(msg_buffer, msg_size);
    explicit_bzero(data_buffer, data_size);
}

/**
 * @brief Handle bind errors with detailed error messages
 *
 * @param[in] error_code The errno value
 * @param[out] error_buffer Buffer for error message
 * @param[in] buffer_size Size of error buffer
 * @return unix_server_error_t Error code
 */
static unix_server_error_t
unix_server_handle_bind_error(int32_t error_code, uint8_t *error_buffer, 
                            size_t buffer_size)
{
    /* Get error message for logging */
    if (strerror_r(error_code, (char *)error_buffer, buffer_size) != 0)
    {
        printf("Failed to get error details\n");
        return UNIX_SERVER_ERR_BIND;
    }
    
    switch (error_code) 
    {
        case EACCES:
            printf("Error: Address is protected and user is not superuser\n");
            break;
        
        case EADDRINUSE:
            printf("Error: Address is already in use\n");
            break;
        
        case EBADF:
            printf("Error: Invalid file descriptor\n");
            break;
        
        case EINVAL:
            printf("Error: Socket already bound or invalid address/length\n");
            break;
        
        case ENOTSOCK:
            printf("Error: File descriptor does not refer to a socket\n");
            break;
        
        case EADDRNOTAVAIL:
            printf("Error: Nonexistent interface or non-local address requested\n");
            break;
        
        default:
            printf("Unknown error occurred: %s\n", error_buffer);
            break;
    }
    
    return UNIX_SERVER_ERR_BIND;
}

/**
 * @brief Handle listen errors with detailed error messages
 *
 * @param[in] error_code The errno value
 * @param[out] error_buffer Buffer for error message
 * @param[in] buffer_size Size of error buffer
 * @return unix_server_error_t Error code
 */
static unix_server_error_t
unix_server_handle_listen_error(int32_t error_code, uint8_t *error_buffer, 
                              size_t buffer_size)
{
    /* Get error message for logging */
    if (strerror_r(error_code, (char *)error_buffer, buffer_size) != 0)
    {
        printf("Failed to get error details\n");
        return UNIX_SERVER_ERR_LISTEN;
    }
    
    switch (error_code) 
    {
        case EADDRINUSE:
            printf("Error: Address already in use\n");
            break;
            
        case EBADF:
            printf("Error: Invalid file descriptor\n");
            break;
            
        case ENOTSOCK:
            printf("Error: File descriptor does not refer to a socket\n");
            break;
            
        case EOPNOTSUPP:
            printf("Error: Socket does not support listen operation\n");
            break;
            
        default:
            printf("Unknown error occurred: %s\n", error_buffer);
            break;
    }
    
    return UNIX_SERVER_ERR_LISTEN;
}
