/** @file syslog.h
 *
 * @brief A thread-safe system logging facility that writes timestamped entries to a file.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025 Your Company. All rights reserved.
 */

#ifndef SYSLOG_H
#define SYSLOG_H

#include <stdbool.h>

/*************************************************************************
* Constants and type definitions
*************************************************************************/

/**
 * @brief Supported log entry types
 */
typedef enum 
{
    SYSLOG_INFO,
    SYSLOG_WARNING, 
    SYSLOG_ERROR,
    SYSLOG_DEBUG,
    SYSLOG_CRITICAL
} syslog_type_t;

/*************************************************************************
* Public function declarations
*************************************************************************/

/*!
 * @brief Initializes the system's logging functionality.
 *
 * This function must be called before any other syslog functions.
 * Calling this function multiple times without an intervening shutdown
 * will result in an error.
 *
 * @param[in] p_file_path  Path to the log file that should be created or appended to.
 *
 * @return true if the log file was successfully opened and initialized, 
 *         false if there was an error or if p_file_path was NULL.
 */
bool syslog_init(const char* p_file_path);

/*!
 * @brief Writes a log entry with type classification, message, and timestamp.
 *
 * This function is thread-safe and can be called from multiple contexts.
 *
 * @param[in] type      Type of log message (INFO, CRITICAL, etc.)
 * @param[in] p_message Text describing the log event (must not be NULL)
 *
 * @return true if the message was successfully written to the log,
 *         false if the log file isn't open or p_message was NULL.
 */
bool syslog_write(syslog_type_t type, const char* p_message);

/*!
 * @brief Shuts down the system logger.
 *
 * Writes a final timestamp to the log, closes the file, and cleans up resources.
 *
 * @return true if the log file was successfully closed,
 *         false if the log file wasn't open.
 */
bool syslog_shutdown(void);

#endif /* SYSLOG_H */

/*** end of file ***/