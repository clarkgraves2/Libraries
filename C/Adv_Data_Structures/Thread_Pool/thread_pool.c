/** @file thread_pool.c
 *
 * @brief Implementation of thread pool functionality for worker thread management.
 *
 * This module implements a thread pool that manages a fixed number of worker
 * threads. Jobs can be submitted to the pool and will be executed by the next
 * available worker thread. The implementation is thread-safe and uses condition
 * variables for efficient thread synchronization.
 */

 #include "thread_pool.h"
 #include "queue.h"
 #include <pthread.h>
 #include <stdlib.h>
 
 /*************************************************************************
 * Constants and Macros
 *************************************************************************/
 
 #define THREAD_POOL_SUCCESS        (0)
 #define THREAD_POOL_ERROR_PARAM    (-1)
 #define THREAD_POOL_ERROR_MEMORY   (-2)
 #define THREAD_POOL_ERROR_QUEUE    (-3)
 #define THREAD_POOL_ERROR_THREAD   (-4)
 #define THREAD_POOL_STATE_RUNNING  (1)
 #define THREAD_POOL_STATE_STOPPING (0)
 
 /*************************************************************************
 * Private Data Structures
 *************************************************************************/
 
 /**
  * @brief Internal structure representing the thread pool.
  *
  * Contains all the data needed to manage the thread pool including the worker
  * threads, job queue, synchronization primitives, and state information.
  */
 struct thread_pool
 {
     int             num_threads;    // Number of worker threads 
     int             state;          // Current pool state (running/stopping) 
     queue_t *       p_queue;        // Queue of pending jobs 
     pthread_t *     p_threads;      // Array of worker thread handles 
     pthread_mutex_t pool_lock;      // Mutex for thread synchronization 
     pthread_cond_t  signal;         // Condition variable for worker notification 
 };
 
 /*************************************************************************
 * Static Function Prototypes
 *************************************************************************/
 
 /**
  * @brief Worker thread function that processes jobs from the queue.
  *
  * This function runs in each worker thread. It waits for jobs to be available
  * in the queue, dequeues them, and executes them.
  *
  * @param[in] p_arg Pointer to the thread pool structure
  * @return NULL in all cases
  */
 static void * thread_pool_worker(void * p_arg);
 
 /*************************************************************************
 * Public Functions
 *************************************************************************/
 
 /*!
  * @brief Submits a new job to be executed by the thread pool.
  *
  * This function adds a new job to the pool's job queue. The job will be
  * executed by the next available worker thread.
  *
  * @param[in] p_pool Pointer to the thread pool
  * @param[in] p_job  Pointer to the job structure containing function and args
  *
  * @return THREAD_POOL_SUCCESS on success, negative error code on failure
  */
 int 
 thread_pool_submit(thread_pool_t * const p_pool, thread_job_t * const p_job)
 {
     // Validate input parameters
     if ((NULL == p_pool) || (NULL == p_job) || (NULL == p_job->job_fn))
     {
         return THREAD_POOL_ERROR_PARAM;
     }
 
     // Allocate memory for job copy
     thread_job_t * p_new_job = malloc(sizeof(thread_job_t));
     if (NULL == p_new_job)
     {
         return THREAD_POOL_ERROR_MEMORY;
     }
 
     // Copy job data
     *p_new_job = *p_job;
 
     pthread_mutex_lock(&p_pool->pool_lock);
 
     // Check if pool is still running
     if (THREAD_POOL_STATE_RUNNING != p_pool->state)
     {
         pthread_mutex_unlock(&p_pool->pool_lock);
         free(p_new_job);
         return THREAD_POOL_ERROR_QUEUE;
     }
 
     // Add job to queue
     int result = queue_enqueue(p_pool->p_queue, p_new_job);
     if (0 != result)
     {
         pthread_mutex_unlock(&p_pool->pool_lock);
         free(p_new_job);
         return THREAD_POOL_ERROR_QUEUE;
     }
 
     // Signal waiting worker threads
     pthread_cond_signal(&p_pool->signal);
     pthread_mutex_unlock(&p_pool->pool_lock);
 
     return THREAD_POOL_SUCCESS;
 }
 
 /*!
  * @brief Initializes a new thread pool with specified number of worker threads.
  *
  * Creates and initializes a thread pool with the given number of worker threads.
  * Sets up the job queue and synchronization primitives.
  *
  * @param[in] num_threads Number of worker threads to create (must be > 0)
  *
  * @return Pointer to initialized thread pool or NULL on failure
  */
 thread_pool_t *
 thread_pool_initialize(int num_threads)
 {
     if (num_threads <= 0)
     {
         return NULL;
     }
 
     // Allocate thread pool structure
     thread_pool_t * p_pool = calloc(1, sizeof(thread_pool_t));
     if (NULL == p_pool)
     {
         return NULL;
     }
 
     // Initialize basic members
     p_pool->num_threads = num_threads;
     p_pool->state = THREAD_POOL_STATE_RUNNING;
     
     // Create job queue
     p_pool->p_queue = queue_create();
     if (NULL == p_pool->p_queue)
     {
         free(p_pool);
         return NULL;
     }
 
     // Initialize mutex
     if (0 != pthread_mutex_init(&p_pool->pool_lock, NULL))
     {
         queue_destroy(&p_pool->p_queue);
         free(p_pool);
         return NULL;
     }
 
     // Initialize condition variable
     if (0 != pthread_cond_init(&p_pool->signal, NULL))
     {
         pthread_mutex_destroy(&p_pool->pool_lock);
         queue_destroy(&p_pool->p_queue);
         free(p_pool);
         return NULL;
     }
 
     // Allocate thread handles array
     p_pool->p_threads = calloc(num_threads, sizeof(pthread_t));
     if (NULL == p_pool->p_threads)
     {
         pthread_cond_destroy(&p_pool->signal);
         pthread_mutex_destroy(&p_pool->pool_lock);
         queue_destroy(&p_pool->p_queue);
         free(p_pool);
         return NULL;
     }
 
     // Create worker threads
     for (int idx = 0; idx < num_threads; idx++)
     {
         if (0 != pthread_create(&p_pool->p_threads[idx], NULL, 
                               thread_pool_worker, p_pool))
         {
             // Thread creation failed - clean up
             p_pool->state = THREAD_POOL_STATE_STOPPING;
             pthread_cond_broadcast(&p_pool->signal);
 
             // Wait for already created threads
             for (int j = 0; j < idx; j++)
             {
                 pthread_join(p_pool->p_threads[j], NULL);
             }
 
             // Free resources
             free(p_pool->p_threads);
             pthread_cond_destroy(&p_pool->signal);
             pthread_mutex_destroy(&p_pool->pool_lock);
             queue_destroy(&p_pool->p_queue);
             free(p_pool);
             return NULL;
         }
     }
 
     return p_pool;
 }
 
 /*!
  * @brief Shuts down the thread pool and waits for all worker threads to complete.
  *
  * Initiates an orderly shutdown of the thread pool. Sets the pool state to
  * stopping and waits for all worker threads to complete their current jobs.
  *
  * @param[in] p_pool Pointer to the thread pool
  *
  * @return THREAD_POOL_SUCCESS on success, negative error code on failure
  */
 int
 thread_pool_shutdown(thread_pool_t * const p_pool)
 {
     if (NULL == p_pool)
     {
         return THREAD_POOL_ERROR_PARAM;
     }
 
     pthread_mutex_lock(&p_pool->pool_lock);
     p_pool->state = THREAD_POOL_STATE_STOPPING;
     pthread_cond_broadcast(&p_pool->signal);
     pthread_mutex_unlock(&p_pool->pool_lock);
 
     // Wait for all threads to complete
     for (int idx = 0; idx < p_pool->num_threads; idx++)
     {
         pthread_join(p_pool->p_threads[idx], NULL);
     }
 
     return THREAD_POOL_SUCCESS;
 }
 
 /*!
  * @brief Frees resources associated with a thread pool.
  *
  * Cleans up all resources allocated by the thread pool including the job queue,
  * synchronization primitives, and thread handles. The pool should be shut down
  * before calling this function.
  *
  * @param[in] p_pool Pointer to the thread pool to destroy
  */
 void
 thread_pool_destroy(thread_pool_t * const p_pool)
 {
     if (NULL == p_pool)
     {
         return;
     }
 
     // Free any remaining jobs in queue
     thread_job_t * p_job = NULL;
     while (0 == queue_dequeue(p_pool->p_queue, (void **)&p_job))
     {
         if (NULL != p_job)
         {
             free(p_job);
         }
     }
 
     // Free all resources
     free(p_pool->p_threads);
     pthread_cond_destroy(&p_pool->signal);
     pthread_mutex_destroy(&p_pool->pool_lock);
     queue_destroy(&p_pool->p_queue);
     free(p_pool);
 }
 
 /*!
  * @brief Gets the number of jobs currently queued in the thread pool.
  *
  * Returns the current number of pending jobs in the pool's job queue.
  *
  * @param[in] p_pool Pointer to the thread pool
  *
  * @return Number of queued jobs or negative error code on failure
  */
 int
 thread_pool_get_queue_size(thread_pool_t * const p_pool)
 {
     if (NULL == p_pool)
     {
         return THREAD_POOL_ERROR_PARAM;
     }
 
     pthread_mutex_lock(&p_pool->pool_lock);
     int size = queue_size(p_pool->p_queue);
     pthread_mutex_unlock(&p_pool->pool_lock);
 
     return size;
 }
 
 /*!
  * @brief Checks if the thread pool is currently running.
  *
  * Returns the current operational state of the thread pool.
  *
  * @param[in] p_pool Pointer to the thread pool
  *
  * @return true if running, false if shutting down or invalid pool pointer
  */
 bool
 thread_pool_is_running(thread_pool_t * const p_pool)
 {
     if (NULL == p_pool)
     {
         return false;
     }
 
     pthread_mutex_lock(&p_pool->pool_lock);
     bool running = (THREAD_POOL_STATE_RUNNING == p_pool->state);
     pthread_mutex_unlock(&p_pool->pool_lock);
 
     return running;
 }
 
 /*************************************************************************
 * Private Functions
 *************************************************************************/
 
 /*!
  * @brief Worker thread function that processes jobs from the queue.
  *
  * Main function for worker threads. Continuously monitors the job queue and
  * executes jobs when available. Exits when pool is stopping and queue is empty.
  *
  * @param[in] p_arg Pointer to the thread pool structure
  *
  * @return NULL in all cases
  */
 static void *
 thread_pool_worker(void * const p_arg)
 {
     thread_pool_t * const p_pool = (thread_pool_t *)p_arg;
 
     if (NULL == p_pool)
     {
         return NULL;
     }
 
     for (;;)
     {
         pthread_mutex_lock(&p_pool->pool_lock);
 
         // Wait while pool is running but no jobs available
         while ((THREAD_POOL_STATE_RUNNING == p_pool->state) && 
                (queue_is_empty(p_pool->p_queue)))
         {
             pthread_cond_wait(&p_pool->signal, &p_pool->pool_lock);
         }
 
         // Exit if pool is stopping and no more jobs
         if ((THREAD_POOL_STATE_RUNNING != p_pool->state) && 
             (queue_is_empty(p_pool->p_queue)))
         {
             pthread_mutex_unlock(&p_pool->pool_lock);
             break;
         }
 
         // Get next job from queue
         thread_job_t * p_job = NULL;
         int result = queue_dequeue(p_pool->p_queue, (void **)&p_job);
 
         pthread_mutex_unlock(&p_pool->pool_lock);
 
         // Execute job if dequeue successful
         if ((0 == result) && (NULL != p_job))
         {
             if (NULL != p_job->job_fn)
             {
                 p_job->job_fn(p_job->p_arg);
             }
             free(p_job);
         }
     }
 
     return NULL;
 }
 
 /*** end of file ***/