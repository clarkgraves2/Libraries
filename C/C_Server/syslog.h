/** @file syslog.h
 *
 * @brief Thread-safe multi-destination logging system
 *
 * This module implements a thread-safe logging system that supports
 * multiple output destinations including file, stdout, stderr,
 * and system logging facilities. It ensures thread safety through
 * mutex synchronization.
 *
 */

 #ifndef SYSLOG_H
 #define SYSLOG_H
 
 #include <stdbool.h>
 #include <stddef.h>
 
 /**
  * Log message severity types
  */
 typedef enum {
     INFO = 0,    /**< Informational message */
     WARNING,     /**< Warning condition */
     ERROR,       /**< Error condition */
     DEBUG,       /**< Debug-level message */
     CRITICAL     /**< Critical condition */
 } syslog_type_t;
 
 /**
  * Log output destinations
  */
 typedef enum {
     SYSLOG_DEST_NONE    = 0x00,  // No logging (disabled)
     SYSLOG_DEST_FILE    = 0x01,  // Log to file
     SYSLOG_DEST_STDOUT  = 0x02,  // Log to standard output 
     SYSLOG_DEST_STDERR  = 0x04,  // Log to standard error 
     SYSLOG_DEST_SYSLOG  = 0x08,  // Log to system logger
     SYSLOG_DEST_ALL     = 0x0F   // Log to all destinations
 } syslog_dest_t;
 
 /**
  * Logger configuration
  */
 typedef struct {
     syslog_dest_t destinations;   /**< Bitwise OR of destination flags */
     const char* file_path;        /**< Path for file logging (if enabled) */
     syslog_type_t min_level;      /**< Minimum level to log (inclusive) */
 } syslog_config_t;
 
 /**
  * @brief Initialize the system logging facility
  *
  * Opens the specified log destinations based on the configuration.
  * Must be called before any other syslog functions.
  *
  * @param[in] config Configuration parameters for the logger
  *
  * @return true if initialization successful
  * @retval false if already initialized, configuration invalid, or any destination fails
  */
 bool syslog_init(const syslog_config_t* config);
 
 /**
  * @brief Writes a formatted log message with timestamp and type
  *
  * Takes a message type, destination, format string and variable arguments.
  * Formats and writes a timestamped log entry to specified destination(s).
  * Thread-safe via mutex protection.
  *
  * @param[in] type Log message type (INFO, ERROR, etc)
  * @param[in] dest Destination(s) for this specific message (use SYSLOG_DEST_NONE to use global config)
  * @param[in] p_format Printf-style format string
  * @param[in] ... Variable arguments for format string
  *
  * @return true if write successful to at least one destination
  * @retval false if format string is NULL, message type invalid, logger not initialized
  */
 bool syslog_write(syslog_type_t type, syslog_dest_t dest, const char* p_format, ...);
 
 /**
  * @brief Updates the log configuration at runtime
  *
  * Allows changing minimum log level, destinations, and other parameters
  * without restarting the logging system.
  *
  * @param[in] config New configuration parameters
  *
  * @return true if reconfiguration successful
  * @retval false if config is NULL or invalid
  */
 bool syslog_reconfigure(const syslog_config_t* config);
 
 /**
  * @brief Shuts down the logging system
  *
  * Writes a final timestamp entry and closes all open destinations.
  * Cleans up logging resources.
  *
  * @return true if shutdown successful
  * @retval false if logger not initialized
  */
 bool syslog_shutdown(void);
 
 #endif /* SYSLOG_H */