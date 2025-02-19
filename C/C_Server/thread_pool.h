/** @file thread_pool.h
 *
 * @brief Thread pool implementation for managing worker threads and job queues.
 *
 * This module provides a thread pool implementation that manages a fixed number
 * of worker threads. Jobs can be submitted to the pool and will be executed by
 * the next available worker thread. The implementation is thread-safe and uses
 * condition variables for efficient thread synchronization.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2024 Your Company Name. All rights reserved.
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdbool.h>

/**
 * @brief Opaque type representing a thread pool instance
 */
typedef struct thread_pool thread_pool_t;

/**
 * @brief Structure representing a job to be executed by the thread pool
 */
typedef struct thread_job
{
    void * (*job_fn)(void * p_arg);  /**< Function pointer to the job to execute */
    void * p_arg;                    /**< Argument to pass to the job function */
} thread_job_t;

/**
 * @brief Initialize a new thread pool with the specified number of worker threads
 *
 * @param[in] num_threads Number of worker threads to create (must be > 0)
 * @return Pointer to the created thread pool or NULL if initialization failed
 */
thread_pool_t * thread_pool_initialize(int num_threads);

/**
 * @brief Submit a new job to be executed by the thread pool
 *
 * @param[in] p_pool Pointer to the thread pool
 * @param[in] p_job Pointer to the job structure containing the function and argument
 * @return 0 on success, negative error code on failure
 */
int thread_pool_submit(thread_pool_t * const p_pool, thread_job_t * const p_job);

/**
 * @brief Shutdown the thread pool and wait for all worker threads to complete
 *
 * This function will wait for all queued jobs to complete before shutting down
 * the worker threads.
 *
 * @param[in] p_pool Pointer to the thread pool
 * @return 0 on success, negative error code on failure
 */
int thread_pool_shutdown(thread_pool_t * const p_pool);

/**
 * @brief Free resources associated with a thread pool
 *
 * The pool should be shut down before calling this function.
 *
 * @param[in] p_pool Pointer to the thread pool
 */
void thread_pool_destroy(thread_pool_t * const p_pool);

/**
 * @brief Get the number of jobs currently queued in the thread pool
 *
 * @param[in] p_pool Pointer to the thread pool
 * @return Number of queued jobs or negative error code on failure
 */
int thread_pool_get_queue_size(thread_pool_t * const p_pool);

/**
 * @brief Check if the thread pool is currently running
 *
 * @param[in] p_pool Pointer to the thread pool
 * @return true if running, false if shutting down or invalid pool pointer
 */
bool thread_pool_is_running(thread_pool_t * const p_pool);

#endif /* THREAD_POOL_H */