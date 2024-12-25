/** @file error.c
 *
 * @brief [Single sentence describing the implementation]
 *
 * @details
 * [Detailed implementation overview:
 *   - Key algorithms used
 *   - Important data structures
 *   - Critical behaviors]
 *
 * NOTE: Implementation assumes:
 *   - [Assumption 1]
 *   - [Assumption 2]
 *
 * WARNING: Important considerations:
 *   - [Warning 1]
 *   - [Warning 2]
 *
 * Change History:
 * 1.0.0 (YYYY-MM-DD) - Initial implementation
 *   - [Change note 1]
 *   - [Change note 2]
 */

#define _POSIX_C_SOURCE 200112L

#include "../include/error.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool 
error_handler_init()
{

}

void 
error_handler_log(const char * msg, bool h_option)
{
    char err_buff[256] = {0};
    strerror_r(errno, err_buff, sizeof(err_buff));
    
    if (true == h_option) 
    {
        fprintf(stdout, "%s: %s\n", msg, err_buff);
    } 
    else
    {
        fprintf(stderr, "%s: %s\n", msg, err_buff);
    }
}
