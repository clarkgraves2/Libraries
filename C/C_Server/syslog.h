/** @file syslog.h
 *
 * @brief Thread-safe system logging interface
 * 
 * This module provides a thread-safe logging facility for writing timestamped
 * log entries to a file. The logger supports multiple message types and 
 * ensures thread safety through mutex synchronization.
 */

 #ifndef SYSLOG_H
 #define SYSLOG_H
 
 /*************************************************************************
 * Includes
 *************************************************************************/

 #include <stdbool.h>
 
 /*************************************************************************
 * Type Definitions 
 *************************************************************************/
 
 // Log message types for message categorization
 typedef enum 
 {
     INFO,      
     WARNING,   
     ERROR,     
     DEBUG,     
     CRITICAL   
 } syslog_type_t;
 
 /*************************************************************************
 * Function Declarations
 *************************************************************************/
 
 /** @brief Initializes the logging system
  *
  * Must be called before any other syslog functions. Creates or opens the
  * log file for appending and writes an initial timestamp entry.
  *
  * @param[in] p_file_path Path to log file to create/append
  * @return true if initialization successful, false otherwise
  */
 bool syslog_init(const char *p_file_path);
 
 /** @brief Writes a formatted message to the log
  *
  * Writes a timestamped, typed log entry. Supports printf-style formatting.
  * Thread-safe - can be called from multiple threads.
  *
  * @param[in] type Type of log message (INFO, ERROR, etc.)
  * @param[in] p_format Printf-style format string
  * @param[in] ... Variable arguments for format string
  * @return true if write successful, false otherwise
  */
 bool syslog_write(syslog_type_t type, const char *p_format, ...);
 
 /** @brief Shuts down the logging system
  *
  * Writes final timestamp, closes log file and cleans up resources.
  * Must be called to properly close the log file.
  *
  * @return true if shutdown successful, false otherwise
  */
 bool syslog_shutdown(void);
 
 #endif /* SYSLOG_H */

 /*** end of file ***/