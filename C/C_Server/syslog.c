/** @file syslog.c
 *
 * @brief Implementation of thread-safe system logging
 *
 * This module implements a thread-safe logging system that writes
 * timestamped entries to a file. It supports multiple message types
 * and ensures thread safety through mutex synchronization.
 *
 */

 #include "syslog.h"
 #include <pthread.h>
 #include <stdarg.h>
 #include <stdbool.h>
 #include <stdio.h>
 #include <string.h>
 #include <time.h>
 
 /*************************************************************************
  * Constants and Macros
  *************************************************************************/
 
 // Maximum size of formatted message buffer
 #define MESSAGE_BUFFER (1024)
 
 // Size of timestamp string buffer 
 #define SYSLOG_TIMESTAMP_SIZE (32)
 
 // Format string for timestamp
 #define SYSLOG_TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
 
 // Format string for log entry header 
 #define SYSLOG_HEADER_FORMAT "[%s][%s] %s\n"
 
 // Log message types 
 static const char * const SYSLOG_TYPE_STRINGS[] = 
 {
     "INFO",     
     "WARNING",  
     "ERROR",    
     "DEBUG",    
     "CRITICAL"  
 };
 
 /*************************************************************************
  * Static Variables
  *************************************************************************/
 
 static FILE * g_p_logfile_handle = NULL;
 
 static bool g_b_initialized = false;
 
 static pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;
 
 /*************************************************************************
  * Private Function Prototypes
  *************************************************************************/
 
 /** @brief Validates logger state
  *
  *  Checks if logger is properly initialized with valid file handle.
  *
  *  @return True if logger valid and initialized
  */
 static bool
 syslog_is_valid(void);
 
 /** @brief Writes timestamp with prefix to log
  *
  *  Helper function to write formatted timestamp entries.
  *
  *  @param[in]  p_prefix  String to prepend to timestamp
  *
  *  @return True if write successful
  *  @retval false if write fails or prefix is NULL
  */
 static bool
 syslog_write_timestamp(const char * p_prefix);
 
 /*************************************************************************
  * Function Definitions
  *************************************************************************/
 
 /*!
  * @brief Initializes the system logging facility
  *
  * Opens the specified log file for appending and writes an initial timestamp.
  * Must be called before any other syslog functions.
  *
  * @param[in] p_file_path Path to log file to open/create
  * 
  * @return true if initialization successful
  * @retval false if already initialized, file_path is NULL, file open fails
  */
bool
syslog_init(const char * const p_file_path)
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

    // Initial timestamp 
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

/*!
 * @brief Writes a formatted log message with timestamp and type
 *
 * Takes a message type, format string and variable arguments.
 * Formats and writes a timestamped log entry to the log file.
 * Thread-safe via mutex protection.
 *
 * @param[in] type Log message type (INFO, ERROR, etc)
 * @param[in] p_format Printf-style format string
 * @param[in] ... Variable arguments for format string
 * 
 * @return true if write successful
 * @retval false if format string is NULL, message type invalid, logger not initialized,
 *         time calculation fails, or write operation fails
 */
bool
syslog_write(syslog_type_t type, const char * p_format, ...)
{
    char formatted_message[MESSAGE_BUFFER];
    va_list args;
    struct tm time_info;
    time_t now;
    char timestamp[SYSLOG_TIMESTAMP_SIZE];

    if ((NULL == p_format) || (type > CRITICAL))
    {
        return false;
    }

    // Format message with variable arguments 
    va_start(args, p_format);
    if (vsnprintf(formatted_message, sizeof(formatted_message), p_format, args) < 0)
    {
        va_end(args);
        return false;
    }
    va_end(args);

    pthread_mutex_lock(&g_log_mutex);

    // Check logger state 
    if (!syslog_is_valid())
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }

    // Get current time for log 
    now = time(NULL);
    if (NULL == localtime_r(&now, &time_info))
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }

    // Format timestamp 
    if (0 == strftime(timestamp, sizeof(timestamp), SYSLOG_TIMESTAMP_FORMAT, &time_info))
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }

    // Write log entry 
    if (fprintf(g_p_logfile_handle, SYSLOG_HEADER_FORMAT,
            timestamp, SYSLOG_TYPE_STRINGS[type], formatted_message) < 0)
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }

    // Flush to ensure writing to disk
    if (0 != fflush(g_p_logfile_handle))
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }

    pthread_mutex_unlock(&g_log_mutex);
    return true;
}

/*!
 * @brief Shuts down the logging system
 *
 * Writes a final timestamp entry and closes the log file.
 * Cleans up logging resources.
 *
 * @return true if shutdown successful
 * @retval false if logger not initialized, final timestamp write fails,
 *         or file close operation fails
 */
bool
syslog_shutdown(void)
{
    pthread_mutex_lock(&g_log_mutex);

    // Checking logger state 
    if (!syslog_is_valid())
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }

    // Writing final timestamp 
    if (!syslog_write_timestamp("--- Log Ended at "))
    {
        pthread_mutex_unlock(&g_log_mutex);
        return false;
    }

    // Close log file
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
 * Private Function Definitions
 *************************************************************************/

/*!
 * @brief Validates the logger state
 *
 * Checks if the logger is properly initialized with a valid file handle.
 *
 * @return true if logger valid and initialized
 * @retval false if file handle is NULL or logger not initialized
 */
static bool
syslog_is_valid(void)
{
    return ((NULL != g_p_logfile_handle) && g_b_initialized);
}

/*!
 * @brief Writes a timestamped entry to the log
 *
 * Helper function to write formatted timestamp entries.
 * Used for start/end log markers.
 *
 * @param[in] p_prefix String to prepend to timestamp
 * 
 * @return true if write successful
 * @retval false if prefix is NULL, time calculation fails, or write operation fails
 */
static bool
syslog_write_timestamp(const char * const p_prefix)
{
    time_t now;
    char time_str[SYSLOG_TIMESTAMP_SIZE];
    struct tm time_info;

    if (NULL == p_prefix)
    {
        return false;
    }

    // Get current time for timestamp 
    now = time(NULL);
    if (NULL == localtime_r(&now, &time_info))
    {
        return false;
    }

    // Format time string 
    if (NULL == asctime_r(&time_info, time_str))
    {
        return false;
    }

    // Write timestamp entry 
    if (fprintf(g_p_logfile_handle, "%s%s", p_prefix, time_str) < 0)
    {
        return false;
    }

    // Ensuring we're writing to disk
    if (0 != fflush(g_p_logfile_handle))
    {
        return false;
    }

    return true;
}

/*** end of file ***/