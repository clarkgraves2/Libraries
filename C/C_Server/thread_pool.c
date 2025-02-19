/** @file thread_pool.c
 *
 * @brief Implementation of the thread pool interface.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2024 Your Company Name. All rights reserved.
 */

#include <pthread.h>
#include <stdlib.h>
#include "thread_pool.h"
#include "queue.h"

#define THREAD_POOL_SUCCESS       (0)
#define THREAD_POOL_ERROR_PARAM   (-1)
#define THREAD_POOL_ERROR_MEMORY  (-2)
#define THREAD_POOL_ERROR_QUEUE   (-3)
#define THREAD_POOL_ERROR_THREAD  (-4)

#define THREAD_POOL_STATE_RUNNING  (1)
#define THREAD_POOL_STATE_STOPPING (0)

/**
 * @brief Internal structure representing the thread pool
 */
struct thread_pool
{
    int num_threads;             
    int state;                   
    queue_t *p_queue;            
    pthread_t *p_threads;        
    pthread_mutex_t pool_lock;   
    pthread_cond_t signal;       
};

static void *thread_pool_worker(void *p_arg);

int 
thread_pool_submit(thread_pool_t * const p_pool, thread_job_t * const p_job)
{
    if ((NULL == p_pool) || (NULL == p_job) || (NULL == p_job->job_fn))
    {
        return THREAD_POOL_ERROR_PARAM;
    }

 
    thread_job_t *p_new_job = malloc(sizeof(thread_job_t));
    if (NULL == p_new_job)
    {
        return THREAD_POOL_ERROR_MEMORY;
    }
    
    *p_new_job = *p_job;

    pthread_mutex_lock(&p_pool->pool_lock);
    

    if (THREAD_POOL_STATE_RUNNING != p_pool->state)
    {
        pthread_mutex_unlock(&p_pool->pool_lock);
        free(p_new_job);
        return THREAD_POOL_ERROR_QUEUE;
    }
    
 
    int result = queue_enqueue(p_pool->p_queue, p_new_job);
    if (0 != result)
    {
        pthread_mutex_unlock(&p_pool->pool_lock);
        free(p_new_job);
        return THREAD_POOL_ERROR_QUEUE;
    }
    
  
    pthread_cond_signal(&p_pool->signal);
    pthread_mutex_unlock(&p_pool->pool_lock);
    
    return THREAD_POOL_SUCCESS;
}

thread_pool_t *
thread_pool_initialize(int num_threads)
{
    if (num_threads <= 0)
    {
        return NULL;
    }

 
    thread_pool_t *p_pool = calloc(1, sizeof(thread_pool_t));
    if (NULL == p_pool)
    {
        return NULL;
    }

    p_pool->num_threads = num_threads;
    p_pool->state = THREAD_POOL_STATE_RUNNING;
    p_pool->p_queue = queue_create();
    if (NULL == p_pool->p_queue)
    {
        free(p_pool);
        return NULL;
    }

  
    if (0 != pthread_mutex_init(&p_pool->pool_lock, NULL))
    {
        queue_destroy(p_pool->p_queue);
        free(p_pool);
        return NULL;
    }

    if (0 != pthread_cond_init(&p_pool->signal, NULL))
    {
        pthread_mutex_destroy(&p_pool->pool_lock);
        queue_destroy(p_pool->p_queue);
        free(p_pool);
        return NULL;
    }

 
    p_pool->p_threads = calloc(num_threads, sizeof(pthread_t));
    if (NULL == p_pool->p_threads)
    {
        pthread_cond_destroy(&p_pool->signal);
        pthread_mutex_destroy(&p_pool->pool_lock);
        queue_destroy(p_pool->p_queue);
        free(p_pool);
        return NULL;
    }

 
    for (int idx = 0; idx < num_threads; idx++)
    {
        if (0 != pthread_create(&p_pool->p_threads[idx], NULL, 
                              thread_pool_worker, p_pool))
        {

            p_pool->state = THREAD_POOL_STATE_STOPPING;
            pthread_cond_broadcast(&p_pool->signal);
            
            for (int j = 0; j < idx; j++)
            {
                pthread_join(p_pool->p_threads[j], NULL);
            }
            
 
            free(p_pool->p_threads);
            pthread_cond_destroy(&p_pool->signal);
            pthread_mutex_destroy(&p_pool->pool_lock);
            queue_destroy(p_pool->p_queue);
            free(p_pool);
            return NULL;
        }
    }

    return p_pool;
}

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

    for (int idx = 0; idx < p_pool->num_threads; idx++)
    {
        pthread_join(p_pool->p_threads[idx], NULL);
    }

    return THREAD_POOL_SUCCESS;
}

void 
thread_pool_destroy(thread_pool_t * const p_pool)
{
    if (NULL == p_pool)
    {
        return;
    }

    thread_job_t *p_job = NULL;
    while (0 == queue_dequeue(p_pool->p_queue, (void **)&p_job))
    {
        if (NULL != p_job)
        {
            free(p_job);
        }
    }

    free(p_pool->p_threads);
    pthread_cond_destroy(&p_pool->signal);
    pthread_mutex_destroy(&p_pool->pool_lock);
    queue_destroy(p_pool->p_queue);
    free(p_pool);
}

int 
thread_pool_get_queue_size(thread_pool_t * const p_pool)
{
    if (NULL == p_pool)
    {
        return THREAD_POOL_ERROR_PARAM;
    }

    pthread_mutex_lock(&p_pool->pool_lock);
    int size = queue_get_size(p_pool->p_queue);
    pthread_mutex_unlock(&p_pool->pool_lock);

    return size;
}

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

/**
 * @brief Worker thread function that processes jobs from the queue
 *
 * @param[in] p_arg Pointer to the thread pool
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

        while ((THREAD_POOL_STATE_RUNNING == p_pool->state) && 
               (queue_is_empty(p_pool->p_queue)))
        {
            pthread_cond_wait(&p_pool->signal, &p_pool->pool_lock);
        }

        if ((THREAD_POOL_STATE_RUNNING != p_pool->state) && 
            (queue_is_empty(p_pool->p_queue)))
        {
            pthread_mutex_unlock(&p_pool->pool_lock);
            break;
        }

        thread_job_t *p_job = NULL;
        int result = queue_dequeue(p_pool->p_queue, (void **)&p_job);
        
        pthread_mutex_unlock(&p_pool->pool_lock);

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