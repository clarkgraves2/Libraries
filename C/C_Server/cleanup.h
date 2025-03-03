/** @file cleanup.h
 *
 * @brief Interface for managing module cleanup functions for resources.
 *
 * This provides the interface to register cleanup functions by thier type 
 * and be able to specify the order of the cleanup needed if an error occurs
 * or to shutdown and cleanup gracefully.
 * 
 */

 #ifndef CLEANUP_H
 #define CLEANUP_H
 
  /*************************************************************************
  * Includes
  *************************************************************************/
 
 #include <stdbool.h>
 
 /*************************************************************************
 * Opaque Data Structure Declaration
 *************************************************************************/
 
 typedef struct cleanup_entry cleanup_entry_t;
 
 /*************************************************************************
 * Public Functions
 *************************************************************************/
 
 /**
  * @brief Initialize the cleanup system
  * @return true if successful, false otherwise
  */
 bool cleanup_initialize(void);
 
 /**
  * @brief Add void function taking void* argument with order number
  * @param[in] func Function pointer
  * @param[in] arg Argument to pass to function
  * @param[in] order Cleanup order (higher numbers execute first)
  * @return true if successful
  */
 bool cleanup_add_void(void (*func)(void*), void *arg, int order);
 
 /**
  * @brief Add bool function taking no arguments with order number
  * @param[in] func Function pointer
  * @param[in] order Cleanup order (higher numbers execute first)
  * @return true if successful
  */
 bool cleanup_add_bool(bool (*func)(void), int order);
 
 /**
  * @brief Add int function taking void* argument with order number
  * @param[in] func Function pointer
  * @param[in] arg Argument to pass to function
  * @param[in] order Cleanup order (higher numbers execute first)
  * @return true if successful
  */
 bool cleanup_add_int(int (*func)(void*), void *arg, int order);
 
 /**
  * @brief Execute all cleanup functions in order
  */
 void cleanup_execute(void);
 
 #endif /* CLEANUP_H */
 
 /*** end of file ***/