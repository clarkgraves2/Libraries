/** @file signal_handler.c
 *
 * @brief Implementation of signal handling subsystem.
 *
 * This module implements a flexible signal handling system that allows
 * registration of callback functions for specific signals. It provides
 * a centralized way to manage signal handlers while maintaining safety
 * and organization.
 */

 #include <string.h>
 #include "../include/signal_handler.h"
 #include "../include/syslog.h"
 
 /*************************************************************************
 * Constants and Macros
 *************************************************************************/

 #define MAX_SIGNALS (32)
 
 /*************************************************************************
 * Static Variables
 *************************************************************************/
 
 static signal_callback_t g_callbacks[MAX_SIGNALS] = {0};
 
 /*************************************************************************
 * Static Function Prototypes
 *************************************************************************/
 
 /**
  * @brief Central signal handler function that executes callback
  *
  * @param[in] signo Signal number that was received
  */
 static void signal_handler(int signo);
 
 /*************************************************************************
 * Static Functions
 *************************************************************************/
 
 /*!
 * @brief Central signal handling function that dispatches to registered callbacks
 *
 * When a signal is received, this function looks up the corresponding callback
 * in the global callback array and executes it if one exists.
 *
 * @param[in] signo Signal number that was received
 */
 static void
 signal_handler(int signo)
 {
     if (signo >= 0 && signo < MAX_SIGNALS && g_callbacks[signo])
     {
         g_callbacks[signo]();
     }
 }
 
 /*************************************************************************
 * Public Functions
 *************************************************************************/
 
 /*!
 * @brief Initializes the signal handling subsystem with provided configurations
 *
 * For each signal configuration provided:
 * 1. Validates the signal number is within acceptable range
 * 2. Stores the callback in the global callback array
 * 3. Registers the central signal handler for that signal
 *
 * @param[in] configs      Array of signal configurations mapping signals to callbacks
 * @param[in] config_count Number of configurations in the array
 *
 * @return true if all handlers were successfully configured
 *         false if:
 *         - configs is NULL
 *         - config_count is 0 or exceeds MAX_SIGNALS
 *         - any signal number is invalid
 *         - sigaction() call fails for any signal
 *
 * @note This function must be called before any signals can be handled
 * @note Previous signal handlers will be overwritten
 */
 bool
 sig_handler_init(const signal_config_t *configs, size_t config_count)
 {
     if (!configs || config_count == 0 || config_count > MAX_SIGNALS)
     {
         return false;
     }
 
     // Initialize sigaction structure
     struct sigaction sig_act;
     memset(&sig_act, 0, sizeof(sig_act));
     sig_act.sa_handler = signal_handler;
     sigemptyset(&sig_act.sa_mask);
     sig_act.sa_flags = 0;
 
     // Set up handlers for each configured signal
     for (size_t i = 0; i < config_count; i++)
     {
         // Validate signal number
         if (configs[i].signal < 0 || configs[i].signal >= MAX_SIGNALS)
         {
             syslog_write(ERROR, "Invalid signal number: %d", configs[i].signal);
             return false;
         }
 
         // Store callback in global array
         g_callbacks[configs[i].signal] = configs[i].callback;
 
         // Register signal handler
         if (sigaction(configs[i].signal, &sig_act, NULL) == -1)
         {
             syslog_write(ERROR, 
                         "Failed to set up handler for signal %d",
                         configs[i].signal);
             return false;
         }
     }
 
     return true;
 }
 
 /*** end of file ***/