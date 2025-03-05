/** @file cleanup.c
 *
 * @brief Implementation of module cleanup function tracker with numeric
 * ordering.
 *
 * @details
 * Implements a thread-safe cleanup system that manages registration and 
 * execution of cleanup functions in a specified order. The system supports
 * three types of cleanup functions:
 *   - Void functions taking void* arguments
 *   - Bool functions taking no arguments
 *   - Int functions taking void* arguments
 *
 * Thread safe through mutex locks. Cleanup functions
 * are executed in descending order of their assigned priority.
 * 
 */

/*************************************************************************
* Includes
*************************************************************************/
#include "../include/cleanup.h"
#include "../include/syslog.h"
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

/*************************************************************************
* Constants and Macros
*************************************************************************/

#define MAX_CLEANUP_FNS (32)

/*************************************************************************
* Private Typedefs and Enumerations
*************************************************************************/

/** Types of cleanup functions supported by the system */
typedef enum 
{
    CLEANUP_VOID,    // Function returns void, void* arg
    CLEANUP_BOOL,    // Function returns bool, no args
    CLEANUP_INT      // Function returns int,  void* arg
} cleanup_type_t;

/*************************************************************************
* Private Data Structures
*************************************************************************/

struct cleanup_entry
{
    cleanup_type_t type;    
    int            order;   
    
    // Union of possible function pointer types 
    union
    {
        void (*void_func) (void *);   
        bool (*bool_func) (void);     
        int  (*int_func)  (void *);   
    } func;
    
    void * arg;    // Argument passed to function if needed
};

/*************************************************************************
* Static Variables
*************************************************************************/

// Array of registered cleanup functions
static cleanup_entry_t g_cleanup_fns[MAX_CLEANUP_FNS];

// Number of registered cleanup functions 
static int g_cleanup_count = 0;

// Mutex for thread-safety
static pthread_mutex_t g_cleanup_mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialization status
static bool g_b_initialized = false;

/*************************************************************************
* Static Function Prototypes
*************************************************************************/

static bool cleanup_add_internal(cleanup_entry_t entry);
static void sort_cleanup_entries(void);

/*************************************************************************
* Public Functions
*************************************************************************/

/*!
 * @brief Initializes the cleanup system
 *
 * @return true if initialization successful, false if already initialized
 */
bool
cleanup_initialize(void)
{
    pthread_mutex_lock(&g_cleanup_mutex);

    // Check if already initialized
    if (g_b_initialized)
    {
        pthread_mutex_unlock(&g_cleanup_mutex);
        return false;
    }
    // Reset count of registered cleanup functions
    g_cleanup_count = 0;   
    // Mark system as initialized                 
    g_b_initialized = true;                   

    pthread_mutex_unlock(&g_cleanup_mutex);
    return true;
}

/*!
 * @brief Registers a void function for cleanup
 *
 * @param[in] func   Function pointer to register
 * @param[in] arg    Argument to pass to function
 * @param[in] order  Execution priority (higher numbers execute first)
 *
 * @return true if registration successful, false otherwise
 */
bool
cleanup_add_void(void (*func)(void*), void* arg, int order)
{
    if (NULL == func)
    {
        return false;
    }

    cleanup_entry_t entry =              
    {
        .type = CLEANUP_VOID,              // Set type to function
        .func.void_func = func,            // Function pointer
        .arg = arg,                        // Function argument
        .order = order                     // Execution order
    };

    return cleanup_add_internal(entry);
}

/*!
 * @brief Registers a bool function for cleanup
 *
 * @param[in] func   Function pointer to register
 * @param[in] order  Execution priority (higher numbers execute first)
 *
 * @return true if registration successful, false otherwise
 */
bool
cleanup_add_bool(bool (*func)(void), int order)
{
    if (NULL == func)
    {
        return false;
    }

    cleanup_entry_t entry = 
    {
        .type = CLEANUP_BOOL,
        .func.bool_func = func,
        .arg = NULL,
        .order = order  
    };

    return cleanup_add_internal(entry);
}

/*!
 * @brief Registers an int function for cleanup
 *
 * @param[in] func   Function pointer to register
 * @param[in] arg    Argument to pass to function
 * @param[in] order  Execution priority (higher numbers execute first)
 *
 * @return true if registration successful, false otherwise
 */
bool
cleanup_add_int(int (*func)(void*), void* arg, int order)
{
    if (NULL == func)
    {
        return false;
    }

    cleanup_entry_t entry = 
    {
        .type = CLEANUP_INT,
        .func.int_func = func,
        .arg = arg,
        .order = order    
    };

    return cleanup_add_internal(entry);
}

/*!
 * @brief Executes all registered cleanup functions in priority order
 *
 * Executes cleanup functions in descending order of priority. Higher
 * order numbers execute first. Failures are logged but do not stop
 * execution of remaining functions.
 */
void
cleanup_execute(void)
{
    pthread_mutex_lock(&g_cleanup_mutex);
    
    // Sort entries by priority order before executing the cleanup functions.
    sort_cleanup_entries();

    for (int idx = 0; idx < g_cleanup_count; idx++)
    {
        // Get most recent cleanup entry
        cleanup_entry_t* entry = &g_cleanup_fns[idx];
        
        // Initialize sucessful cleanup flag 
        bool cleanup_success = false;

        // Look at current entry's type
        switch (entry->type)
        {
            case CLEANUP_VOID:
                
                if (entry->func.void_func) 
                {   
                    syslog_write(INFO, "Executing void cleanup function with order %d", entry->order);
                    entry->func.void_func(entry->arg);
                    cleanup_success = true;
                }
                break;
                case CLEANUP_BOOL:
                if (entry->func.bool_func)
                {
                    syslog_write(INFO,
                               "Executing bool cleanup function with order %d",
                               entry->order);
                    entry->func.bool_func();
                    cleanup_success = true;
                }
                break;

            case CLEANUP_INT:
                if (entry->func.int_func)
                {
                    syslog_write(INFO,
                               "Executing int cleanup function with order %d",
                               entry->order);
                    entry->func.int_func(entry->arg);
                    cleanup_success = true;
                }
                break;
        }

        if (!cleanup_success)
        {
            syslog_write(ERROR, 
                        "Cleanup function with order %d failed", 
                        entry->order);
        }
    }

    pthread_mutex_unlock(&g_cleanup_mutex);
    syslog_write(INFO, "Cleanup process completed");
}

/*************************************************************************
* Private Functions
*************************************************************************/

/*!
 * @brief Internal helper function to add cleanup entries
 *
 * @param[in] entry  Cleanup entry to add to registry
 *
 * @return true if entry added successfully, false otherwise
 *
 * @note Assumes mutex is already locked by caller
 */
static bool
cleanup_add_internal(cleanup_entry_t entry)
{
    if (!g_b_initialized)
    {
        return false;
    }

    pthread_mutex_lock(&g_cleanup_mutex);

    // Check to see if the function registry is full
    if (g_cleanup_count >= MAX_CLEANUP_FNS)
    {
        pthread_mutex_unlock(&g_cleanup_mutex);
        return false;
    }

    // Add function and arg to registry array
    g_cleanup_fns[g_cleanup_count] = entry;
    g_cleanup_count++;

    pthread_mutex_unlock(&g_cleanup_mutex);
    return true;
}

/*!
 * @brief Sorts cleanup entries by order number
 *
 * Implements bubble sort to arrange entries in descending order
 * of priority (higher numbers first).
 *
 * @note Assumes mutex is already locked by caller
 */
static void
sort_cleanup_entries(void)
{
    for (int idx = 0; idx < g_cleanup_count - 1; idx++)
    {
        for (int jdx = 0; jdx < g_cleanup_count - idx - 1; jdx++)
        {
            if (g_cleanup_fns[jdx].order < g_cleanup_fns[jdx + 1].order)
            {
                cleanup_entry_t temp = g_cleanup_fns[jdx];
                g_cleanup_fns[jdx] = g_cleanup_fns[jdx + 1];
                g_cleanup_fns[jdx + 1] = temp;
            }
        }
    }
}

/*** end of file ***/