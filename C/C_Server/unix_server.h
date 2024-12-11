/** @file unix_server.h
 *
 * @brief Unix domain socket server interface.
 *
 * @par
 * This header defines the public interface for the Unix domain socket server.
 * 
 * COPYRIGHT NOTICE: (c) 2024 Your Company. All rights reserved.
 */

#ifndef UNIX_SERVER_H
#define UNIX_SERVER_H

#include <stdint.h>
#include <stddef.h>

/* Constants */
#define UNIX_SERVER_MAX_MSG_LEN     100u
#define UNIX_SERVER_LISTEN_BACKLOG    5
#define UNIX_SERVER_SOCK_PATH     "/tmp/unix_server.sock"

/* Error codes */
typedef enum
{
    UNIX_SERVER_SUCCESS = 0,
    UNIX_SERVER_ERR_SOCKET = -1,
    UNIX_SERVER_ERR_BIND = -2,
    UNIX_SERVER_ERR_LISTEN = -3,
    UNIX_SERVER_ERR_SIGNAL = -4,
    UNIX_SERVER_ERR_READ = -5,
    UNIX_SERVER_ERR_WRITE = -6
} unix_server_error_t;

/* Public function prototypes */
int32_t unix_server_init(void);
void unix_server_shutdown(void);

#endif /* UNIX_SERVER_H */