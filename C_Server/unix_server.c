#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


#define MSG_LEN 100
#define SOCK_PATH "/tmp/unix_server.sock"

static volatile sig_atomic_t keep_running = 1;

void cleanup_and_exit(int base_socket, char *msg_buff, size_t msg_len, char *str, size_t str_len);

int 
main(void)
{
    // Set's up base socket
    int base_socket = 0;
    int socket_com = 0;
    int len = 0;
    int error_num;
    char msg_buff[MSG_LEN];
    char str[100] = {0};

    struct sockaddr_un remote, local = 
    {
            .sun_family = AF_UNIX,
    };
    
    if ((base_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        error_num = strerror_r(errno, msg_buff, MSG_LEN);
        if (error_num == 0) 
        {
        printf("Error message: %s\n", msg_buff);
        } 
        else 
        {
        printf("Error occurred while retrieving error message\n");
        }
    }

    // Binds address to socket
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    
    if (bind(base_socket, (struct sockaddr *)&local, len) == -1) 
    {
        error_num = strerror_r(errno, msg_buff, MSG_LEN);
        switch (errno) 
        {
    
            // Network Errors
            case EACCES:
                return "Address is protected and user is not superuser";
            
            case EADDRINUSE:
                return "Address is already in use or all ephemeral ports are occupied";
            
            case EBADF:
                return "Invalid file descriptor";
            
            case EINVAL:
                return "Socket already bound or invalid address/length";
            
            case ENOTSOCK:
                return "File descriptor does not refer to a socket";
            
            // Unix domain specific errors
            case EADDRNOTAVAIL:
                return "Nonexistent interface or non-local address requested";
            
            case EFAULT:
                return "Address points outside accessible address space";
            
            case ELOOP:
                return "Too many symbolic links encountered";
            
            case ENAMETOOLONG:
                return "Address is too long";
            
            case ENOENT:
                return "Component in directory prefix doesn't exist";
            
            case ENOMEM:
                return "Insufficient kernel memory";
            
            case ENOTDIR:
                return "Path prefix component is not a directory";
            
            case EROFS:
                return "Socket inode would reside on read-only filesystem";
            
            default:
                return "Unknown error occurred";
        }
    }

    // Sets the Base Socket to listen
    if (listen(base_socket, 5) == -1)
    {
        error_num = strerror_r(errno, msg_buff, MSG_LEN);
        switch (errno) 
        {
            case EADDRINUSE:
                printf("Error: Address already in use - another socket is listening on this port\n");
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
                printf("Unknown error occurred\n");
                break;
        }
    }

    for (;;)
    {

    }

    if (close (base_socket) == -1)
    {
        error_num = strerror_r(errno, msg_buff, MSG_LEN);

        switch (errno) 
        {
            case EBADF:
                fprintf(stderr, "Error: File descriptor is not valid\n");
                break;
            case EINTR:
                fprintf(stderr, "Error: Close operation interrupted by signal\n");
                break;
            case EIO:
                fprintf(stderr, "Error: I/O error occurred\n");
                break;
            default:
                fprintf(stderr, "Unknown error occurred\n");
        }
    }
    cleanup_and_exit(base_socket, msg_buff, sizeof(msg_buff), str, sizeof(str));
}

void 
cleanup_and_exit(int base_socket, char *msg_buff, size_t msg_len,\
                      char *str, size_t str_len)
{
    // Close socket if still open
    if (base_socket > 0) {
        close(base_socket);
    }
    
    // Remove socket file
    unlink(SOCK_PATH);
    
    // Zero out any sensitive data in buffers
    explicit_bzero(msg_buff, MSG_LEN);
    explicit_bzero(str, sizeof(str));
    
    _Exit(EXIT_SUCCESS);
}

static void 
sig_handler(int sig) 
{
    keep_running = 0;
}
