/** @file thread_pool.h
 *
 * @brief Interface for thread pool management and job execution.
 *
 * This module provides a thread pool implementation that manages concurrent 
 * worker threads and a job queue. Jobs can be submitted to the pool and 
 * will be executed by the next available worker thread. The implementation
 * is thread-safe and provides efficient thread synchronization.
 */

 #ifndef THREAD_POOL_H
 #define THREAD_POOL_H
 
 #include <stdbool.h>
 
 /*************************************************************************
 * Type Definitions
 *************************************************************************/
 
 // Opaque type representing a thread pool instance 
 typedef struct thread_pool thread_pool_t;
 
 // Structure representing a job to be executed by the pool 
 typedef struct thread_job
 {
     void * (*job_fn)(void * p_arg);  // Function pointer to job
     void * p_arg;                    // Job function argument 
 } thread_job_t;
 
 /*************************************************************************
 * Function Declarations
 *************************************************************************/
 
 /*!
  * @brief Creates and initializes a thread pool.
  *
  * @param[in] num_threads Number of worker threads to create
  *
  * @return Pointer to created thread pool if successful, NULL if failure
  */
 thread_pool_t * 
 thread_pool_initialize(int num_threads);
 
 /*!
  * @brief Submits a job to be executed by the thread pool.
  *
  * @param[in] p_pool Pointer to thread pool
  * @param[in] p_job Pointer to job structure
  *
  * @return 0 on success, negative error code on failure
  */
 int 
 thread_pool_submit(thread_pool_t * p_pool, thread_job_t * p_job);
 
 /*!
  * @brief Shuts down the thread pool and waits for completion.
  *
  * @param[in] p_pool Pointer to thread pool
  *
  * @return 0 on success, negative error code on failure
  */
 int 
 thread_pool_shutdown(thread_pool_t * p_pool);
 
 /*!
  * @brief Frees thread pool resources.
  *
  * @param[in] p_pool Pointer to thread pool
  */
 void 
 thread_pool_destroy(thread_pool_t * p_pool);
 
 /*!
  * @brief Gets current job queue size.
  *
  * @param[in] p_pool Pointer to thread pool
  *
  * @return Number of queued jobs or negative error code
  */
 int 
 thread_pool_get_queue_size(thread_pool_t * p_pool);
 
 /*!
  * @brief Checks if pool is running.
  *
  * @param[in] p_pool Pointer to thread pool
  *
  * @return true if running, false if stopping/invalid
  */
 bool 
 thread_pool_is_running(thread_pool_t * p_pool);
 
 #endif /* THREAD_POOL_H */

/*** end of file ***/