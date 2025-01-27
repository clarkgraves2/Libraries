/** @file syslog.c
 *
 * @brief Implementation of thread-safe system logging facility.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025 Your Company. All rights reserved.
 */

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "syslog.h"

/*************************************************************************
* Constants and macros
*************************************************************************/

#define SYSLOG_TIMESTAMP_SIZE    32  // Buffer size for timestamp string (includes safety margin)
#define SYSLOG_TIMESTAMP_FORMAT  "%Y-%m-%d %H:%M:%S"
#define SYSLOG_HEADER_FORMAT     "[%s][%s] %s\n"

static const char* const SYSLOG_TYPE_STRINGS[] = 
{
    "INFO",
    "WARNING",
    "ERROR", 
    "DEBUG",
    "CRITICAL"
};

/*************************************************************************
* Private variables 
*************************************************************************/

static FILE* g_p_logfile_handle = NULL;
static bool g_b_initialized = false;
static pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;

/*************************************************************************
* Private function prototypes
*************************************************************************/

static bool syslog_is_valid(void);
static bool syslog_write_timestamp(const char* p_prefix);

/*************************************************************************
* Public functions
*************************************************************************/

bool 
syslog_init(const char* p_file_path)
{
    pthread_mutex_lock(&g_log_mutex);

    if (g_b_initialized || (NULL == p_file_path))
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }

    g_p_logfile_handle = fopen(p_file_path, "a");
    if (NULL == g_p_logfile_handle)
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }

    if (!syslog_write_timestamp("--- Log Started at "))
    {
        fclose(g_p_logfile_handle);
        g_p_logfile_handle = NULL;
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }

    g_b_initialized = true;
    pthread_mutex_unlock(&g_log_mutex);
    return true;
}

bool 
syslog_write(syslog_type_t type, const char* p_message)
{
    char timestamp[SYSLOG_TIMESTAMP_SIZE];
    struct tm time_info;
    time_t now;
    
    if ((NULL == p_message) || (type > SYSLOG_CRITICAL))
    {
        return false;
    }
    
    pthread_mutex_lock(&g_log_mutex);

    if (!syslog_is_valid())
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }
    
    now = time(NULL);
    if (NULL == localtime_r(&now, &time_info))
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }
    
    if (0 == strftime(timestamp, sizeof(timestamp), 
                     SYSLOG_TIMESTAMP_FORMAT, &time_info))
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }
    
    if (fprintf(g_p_logfile_handle, SYSLOG_HEADER_FORMAT,
               timestamp, SYSLOG_TYPE_STRINGS[type], p_message) < 0)
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }
    
    if (0 != fflush(g_p_logfile_handle))
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }
    
    pthread_mutex_unlock(&g_log_mutex);
    return true;
}

bool 
syslog_shutdown(void)
{
    pthread_mutex_lock(&g_log_mutex);

    if (!syslog_is_valid())
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }
    
    if (!syslog_write_timestamp("--- Log Ended at "))
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }
    
    if (0 != fclose(g_p_logfile_handle))
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }
    
    g_p_logfile_handle = NULL;
    g_b_initialized = false;
    
    pthread_mutex_unlock(&g_log_mutex);
    return true;
}

/*************************************************************************
* Private functions
*************************************************************************/

static bool
syslog_is_valid(void)
{
    return ((NULL != g_p_logfile_handle) && g_b_initialized);
}

static bool
syslog_write_timestamp(const char* p_prefix)
{
    time_t now;
    char time_str[SYSLOG_TIMESTAMP_SIZE];
    struct tm time_info;
    
    if (NULL == p_prefix)
    {
        return false;
    }
    
    now = time(NULL);
    if (NULL == localtime_r(&now, &time_info))
    {
        return false;
    }
    
    if (NULL == asctime_r(&time_info, time_str))
    {
        return false;
    }
    
    if (fprintf(g_p_logfile_handle, "%s%s", p_prefix, time_str) < 0)
    {
        return false;
    }
    
    if (0 != fflush(g_p_logfile_handle))
    {
        return false;
    }
    
    return true;
}

/*** end of file ***/
