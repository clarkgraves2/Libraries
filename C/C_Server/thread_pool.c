
#include "thread_pool.h"
#include "queue.h"
#include <pthread.h>
#include <stdbool.h>

#define THREAD_POOL_RUNNING (1)

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
    while (THREAD_POOL_RUNNING == thread_pool->state)
    {
        pthread_mutex_lock(&thread_pool->thread_pool_lock);
        
        
        
        
        pthread_mutex_unlock(&thread_pool->thread_pool_lock);
    }
}
