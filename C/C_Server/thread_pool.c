
#include "thread_pool.h"
#include "queue.h"
#include <pthread.h>
#include <stdbool.h>

#define THREAD_POOL_RUNNING (1)
#define DEQUEUE_ERROR (-1)

struct thread_pool
{
    int num_threads;
    bool state;
    queue_t * queue;
    pthread_t * pool;
    pthread_mutex_t thread_pool_lock;
    pthread_cond_t signal;
};

struct thread_job
{
    void * (*thread_job)(void * arg);
    void * arg;
};

void * thread_pool_job(void * arg);

thread_pool_t *
thread_pool_initialize(int num_threads)
{
    if (0 >= num_threads)
    {
        // Log Error
        return NULL;
    }

    thread_pool_t * thread_pool = calloc(1, sizeof(thread_pool_t));
    if (NULL == thread_pool)
    {
        // Log Error
        free(thread_pool);
        thread_pool = NULL;
        return NULL;
    }
    
    thread_pool->num_threads = num_threads;
    thread_pool->state = THREAD_POOL_RUNNING;
    thread_pool->pool = calloc(1, sizeof(pthread_t[num_threads]));

    for (int idx = 0; idx < num_threads; idx++)
    {
        pthread_create(&thread_pool->pool[idx], NULL, thread_pool_job, NULL);
    }
    
    thread_pool->queue = queue_create();

    int lock_result = pthread_mutex_init(&thread_pool->thread_pool_lock, NULL);
    int cond_result = pthread_cond_init(&thread_pool->signal, NULL);

    return thread_pool;
}

void * 
thread_pool_job(void * arg)
{
    thread_pool_t * thread_pool = (thread_pool_t *)arg;
    
    if (NULL == thread_pool) 
    {
        // Insert Error Log
        return NULL;
    }
    
    pthread_mutex_lock(&thread_pool->thread_pool_lock);
    
    while (THREAD_POOL_RUNNING == thread_pool->state)
    {
        while (queue_is_empty(thread_pool->queue) && 
               THREAD_POOL_RUNNING == thread_pool->state) 
        {
            pthread_cond_wait(&thread_pool->signal, &thread_pool->thread_pool_lock);
        }

        if (THREAD_POOL_RUNNING != thread_pool->state) 
        {
            break;
        }

        void * item = NULL;
        int dequeue_result = queue_dequeue(thread_pool->queue, &item);
        
        if (dequeue_result == -1) 
        {
            // Log error 
            pthread_mutex_unlock(&thread_pool->thread_pool_lock);
            continue;
        }
        
        thread_job_t *job = (thread_job_t *)item;
        
        pthread_mutex_unlock(&thread_pool->thread_pool_lock);
        
        if (job != NULL) 
        {
            if (job->thread_job != NULL) 
            {
                job->thread_job(job->arg);
            }
        
            free(job);
            job == NULL;
        }
        
        pthread_mutex_lock(&thread_pool->thread_pool_lock);
    }
    
    pthread_mutex_unlock(&thread_pool->thread_pool_lock);
    return NULL;
}
