/** @file syslog.c
 *
 * @brief Implementation of thread-safe multi-destination logging
 *
 * This module implements a thread-safe logging system that writes
 * timestamped entries to multiple destinations. It supports multiple 
 * message types and ensures thread safety through mutex synchronization.
 *
 */

 #include "syslog.h"
 #include <pthread.h>
 #include <stdarg.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include <unistd.h>
 #include <syslog.h>
 
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
 static const char* const SYSLOG_TYPE_STRINGS[] = 
 {
     "INFO",     
     "WARNING",  
     "ERROR",    
     "DEBUG",    
     "CRITICAL"  
 };
 
 // Syslog priority mapping
 static const int SYSLOG_PRIORITY_MAP[] = 
 {
     LOG_INFO,     // INFO
     LOG_WARNING,  // WARNING
     LOG_ERR,      // ERROR
     LOG_DEBUG,    // DEBUG
     LOG_CRIT      // CRITICAL
 };
 
 /*************************************************************************
  * Static Variables
  *************************************************************************/
 
 static FILE* g_p_logfile_handle = NULL;
 
 static bool g_b_initialized = false;
 
 static pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;
 
 static syslog_config_t g_config = {
     .destinations = SYSLOG_DEST_NONE,
     .file_path = NULL,
     .min_level = INFO
 };
 
 /*************************************************************************
  * Private Function Prototypes
  *************************************************************************/
 
 /**
  * @brief Validates logger state
  *
  * Checks if logger is properly initialized.
  *
  * @return True if logger valid and initialized
  */
 static bool
 syslog_is_valid(void);
 
 /**
  * @brief Writes timestamp with prefix to log
  *
  * Helper function to write formatted timestamp entries.
  *
  * @param[in]  p_prefix  String to prepend to timestamp
  *
  * @return True if write successful
  * @retval false if write fails or prefix is NULL
  */
 static bool
 syslog_write_timestamp(const char* p_prefix);
 
 /**
  * @brief Formats a log message with timestamp and type
  *
  * @param[out] p_buffer     Buffer to store formatted message
  * @param[in]  buffer_size  Size of buffer
  * @param[in]  type         Log message type
  * @param[in]  p_message    Message content
  *
  * @return True if formatting successful
  */
 static bool
 syslog_format_message(char* p_buffer, size_t buffer_size, syslog_type_t type, 
                       const char* p_message);
 
 /**
  * @brief Write to system logger
  *
  * @param[in] type       Log message type
  * @param[in] p_message  Message content
  *
  * @return True if write successful
  */
 static bool
 syslog_write_to_syslog(syslog_type_t type, const char* p_message);
 
 /**
  * @brief Write to file
  *
  * @param[in] p_formatted_msg  Formatted message to write
  *
  * @return True if write successful
  */
 static bool
 syslog_write_to_file(const char* p_formatted_msg);
 
 /**
  * @brief Write to stdout
  *
  * @param[in] type           Log message type
  * @param[in] p_message      Message content
  * @param[in] p_formatted    Pre-formatted message (if available)
  *
  * @return True if write successful
  */
 static bool
 syslog_write_to_stdout(syslog_type_t type, const char* p_message, const char* p_formatted);
 
 /**
  * @brief Write to stderr
  *
  * @param[in] type           Log message type
  * @param[in] p_message      Message content
  * @param[in] p_formatted    Pre-formatted message (if available)
  *
  * @return True if write successful
  */
 static bool
 syslog_write_to_stderr(syslog_type_t type, const char* p_message, const char* p_formatted);
 
 /*************************************************************************
  * Function Definitions
  *************************************************************************/
 
 bool
 syslog_init(const syslog_config_t* config)
 {
     pthread_mutex_lock(&g_log_mutex);
 
     if (g_b_initialized || (NULL == config))
     {
         pthread_mutex_unlock(&g_log_mutex);
         return false;
     }
 
     // Store configuration
     memcpy(&g_config, config, sizeof(syslog_config_t));
 
     // Initialize file logging if needed
     if ((g_config.destinations & SYSLOG_DEST_FILE) && g_config.file_path)
     {
         g_p_logfile_handle = fopen(g_config.file_path, "a");
         if (NULL == g_p_logfile_handle)
         {
             // Failed to open log file
             g_config.destinations &= ~SYSLOG_DEST_FILE;
         }
         else
         {
             // Initial timestamp in file
             syslog_write_timestamp("--- Log Started at ");
         }
     }
 
     // Initialize system logger if needed
     if (g_config.destinations & SYSLOG_DEST_SYSLOG)
     {
         // Open connection to system logger
         openlog("application", LOG_PID, LOG_USER);
     }
 
     g_b_initialized = true;
     pthread_mutex_unlock(&g_log_mutex);
     return true;
 }
 
 bool
 syslog_write(syslog_type_t type, syslog_dest_t dest, const char* p_format, ...)
 {
     char formatted_message[MESSAGE_BUFFER];
     char display_message[MESSAGE_BUFFER];
     va_list args;
     bool result = false;
     syslog_dest_t active_destinations;
 
     // Skip messages below minimum level
     if ((NULL == p_format) || (type > CRITICAL) || 
         !g_b_initialized || (type < g_config.min_level))
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
 
     // Format the message with timestamp and type
     if (!syslog_format_message(display_message, sizeof(display_message), 
                                type, formatted_message))
     {
         pthread_mutex_unlock(&g_log_mutex);
         return false;
     }
 
     // Determine which destinations to use
     if (dest == SYSLOG_DEST_NONE) {
         // Use global configuration if no specific destination provided
         active_destinations = g_config.destinations;
     } else {
         // Use the specified destination(s)
         active_destinations = dest;
     }
 
     // Write to each enabled destination
     if (active_destinations & SYSLOG_DEST_FILE)
     {
         result |= syslog_write_to_file(display_message);
     }
 
     if (active_destinations & SYSLOG_DEST_STDOUT)
     {
         result |= syslog_write_to_stdout(type, formatted_message, display_message);
     }
 
     if (active_destinations & SYSLOG_DEST_STDERR)
     {
         result |= syslog_write_to_stderr(type, formatted_message, display_message);
     }
 
     if (active_destinations & SYSLOG_DEST_SYSLOG)
     {
         result |= syslog_write_to_syslog(type, formatted_message);
     }
 
     pthread_mutex_unlock(&g_log_mutex);
     return result;
 }
 
 bool
 syslog_reconfigure(const syslog_config_t* config)
 {
     if (NULL == config)
     {
         return false;
     }
 
     pthread_mutex_lock(&g_log_mutex);
 
     // Check if file destination is being added or changed
     if ((config->destinations & SYSLOG_DEST_FILE) && 
         (!(g_config.destinations & SYSLOG_DEST_FILE) || 
          strcmp(g_config.file_path, config->file_path) != 0))
     {
         // Close previous file if open
         if (g_p_logfile_handle != NULL)
         {
             syslog_write_timestamp("--- Log Ended at ");
             fclose(g_p_logfile_handle);
             g_p_logfile_handle = NULL;
         }
 
         // Open new file
         if (config->file_path)
         {
             g_p_logfile_handle = fopen(config->file_path, "a");
             if (g_p_logfile_handle != NULL)
             {
                 syslog_write_timestamp("--- Log Started at ");
             }
         }
     }
     // File destination is being removed
     else if (!(config->destinations & SYSLOG_DEST_FILE) && 
              (g_config.destinations & SYSLOG_DEST_FILE))
     {
         if (g_p_logfile_handle != NULL)
         {
             syslog_write_timestamp("--- Log Ended at ");
             fclose(g_p_logfile_handle);
             g_p_logfile_handle = NULL;
         }
     }
 
     // Check if syslog destination is being added
     if ((config->destinations & SYSLOG_DEST_SYSLOG) && 
         !(g_config.destinations & SYSLOG_DEST_SYSLOG))
     {
         openlog("application", LOG_PID, LOG_USER);
     }
     // Syslog destination is being removed
     else if (!(config->destinations & SYSLOG_DEST_SYSLOG) && 
              (g_config.destinations & SYSLOG_DEST_SYSLOG))
     {
         closelog();
     }
 
     // Update configuration
     g_config.destinations = config->destinations;
     g_config.min_level = config->min_level;
     
     if (config->file_path && g_p_logfile_handle != NULL) {
         g_config.file_path = config->file_path;
     }
 
     pthread_mutex_unlock(&g_log_mutex);
     return true;
 }
 
 bool
 syslog_shutdown(void)
 {
     pthread_mutex_lock(&g_log_mutex);
 
     if (!g_b_initialized)
     {
         pthread_mutex_unlock(&g_log_mutex);
         return false;
     }
 
     // Close file if open
     if ((g_config.destinations & SYSLOG_DEST_FILE) && g_p_logfile_handle != NULL)
     {
         syslog_write_timestamp("--- Log Ended at ");
         fclose(g_p_logfile_handle);
         g_p_logfile_handle = NULL;
     }
 
     // Close system logger if open
     if (g_config.destinations & SYSLOG_DEST_SYSLOG)
     {
         closelog();
     }
 
     g_b_initialized = false;
     g_config.destinations = SYSLOG_DEST_NONE;
 
     pthread_mutex_unlock(&g_log_mutex);
     return true;
 }
 
 /*************************************************************************
  * Private Function Definitions
  *************************************************************************/
 
 static bool
 syslog_is_valid(void)
 {
     return g_b_initialized;
 }
 
 static bool
 syslog_write_timestamp(const char* const p_prefix)
 {
     time_t now;
     char time_str[SYSLOG_TIMESTAMP_SIZE];
     struct tm time_info;
 
     if (NULL == p_prefix || NULL == g_p_logfile_handle)
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
 
 static bool
 syslog_format_message(char* p_buffer, size_t buffer_size, syslog_type_t type, 
                       const char* p_message)
 {
     struct tm time_info;
     time_t now;
     char timestamp[SYSLOG_TIMESTAMP_SIZE];
     int written = 0;
 
     if (NULL == p_buffer || buffer_size == 0 || NULL == p_message || type > CRITICAL)
     {
         return false;
     }
 
     // Get current time
     now = time(NULL);
     if (NULL == localtime_r(&now, &time_info))
     {
         return false;
     }
 
     // Format timestamp
     if (0 == strftime(timestamp, sizeof(timestamp), SYSLOG_TIMESTAMP_FORMAT, &time_info))
     {
         return false;
     }
 
     // Format message with timestamp and type
     written = snprintf(p_buffer, buffer_size, "[%s][%s] %s\n",
             timestamp, SYSLOG_TYPE_STRINGS[type], p_message);
 
     return (written > 0 && (size_t)written < buffer_size);
 }
 
 static bool
 syslog_write_to_syslog(syslog_type_t type, const char* p_message)
 {
     if (NULL == p_message || type > CRITICAL)
     {
         return false;
     }
 
     syslog(SYSLOG_PRIORITY_MAP[type], "%s", p_message);
     return true;
 }
 
 static bool
 syslog_write_to_file(const char* p_formatted_msg)
 {
     if (NULL == p_formatted_msg || NULL == g_p_logfile_handle)
     {
         return false;
     }
 
     if (fprintf(g_p_logfile_handle, "%s", p_formatted_msg) < 0)
     {
         return false;
     }
 
     if (0 != fflush(g_p_logfile_handle))
     {
         return false;
     }
 
     return true;
 }
 
 static bool
 syslog_write_to_stdout(syslog_type_t type, const char* p_message, const char* p_formatted)
 {
     char local_buffer[MESSAGE_BUFFER];
 
     if (NULL == p_message || type > CRITICAL)
     {
         return false;
     }
 
     // Use pre-formatted message if available, otherwise format locally
     if (p_formatted)
     {
         if (fprintf(stdout, "%s", p_formatted) < 0)
         {
             return false;
         }
     }
     else
     {
         if (!syslog_format_message(local_buffer, sizeof(local_buffer), type, p_message))
         {
             return false;
         }
 
         if (fprintf(stdout, "%s", local_buffer) < 0)
         {
             return false;
         }
     }
 
     fflush(stdout);
     return true;
 }
 
 static bool
 syslog_write_to_stderr(syslog_type_t type, const char* p_message, const char* p_formatted)
 {
     char local_buffer[MESSAGE_BUFFER];
 
     if (NULL == p_message || type > CRITICAL)
     {
         return false;
     }
 
     // Use pre-formatted message if available, otherwise format locally
     if (p_formatted)
     {
         if (fprintf(stderr, "%s", p_formatted) < 0)
         {
             return false;
         }
     }
     else
     {
         if (!syslog_format_message(local_buffer, sizeof(local_buffer), type, p_message))
         {
             return false;
         }
 
         if (fprintf(stderr, "%s", local_buffer) < 0)
         {
             return false;
         }
     }
 
     fflush(stderr);
     return true;
 }