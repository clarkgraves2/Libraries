#include "thread_pool.h"

// defines the parameters for the thread pool
#define MAX_THREADS 4
#define QUEUE_SIZE 10

// Struct responsible for the function and it's args
// to be executed by the thread.
typedef struct {
    void (*function)(void*);
    void *arg;
} task_t;

// Thread pool struct 
typedef struct {
    task_t queue[QUEUE_SIZE];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} threadpool_t;

void threadpool_init(threadpool_t *pool) {
    pool->front = pool->rear = pool->count = 0;
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);
}

void threadpool_add_task(threadpool_t *pool, void (*function)(void *), void *arg) {
    pthread_mutex_lock(&pool->mutex);

    if (pool->count == QUEUE_SIZE) {
        printf("Queue is full\n");
        pthread_mutex_unlock(&pool->mutex);
        return;
    }

    task_t task = { function, arg };
    pool->queue[pool->rear] = task;
    pool->rear = (pool->rear + 1) % QUEUE_SIZE;
    pool->count++;

    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->mutex);
}

void *threadpool_worker(void *arg) {
    threadpool_t *pool = (threadpool_t *)arg;

    while (1) {
        pthread_mutex_lock(&pool->mutex);

        while (pool->count == 0) {
            pthread_cond_wait(&pool->cond, &pool->mutex);
        }

        task_t task = pool->queue[pool->front];
        pool->front = (pool->front + 1) % QUEUE_SIZE;
        pool->count--;

        pthread_mutex_unlock(&pool->mutex);

        task.function(task.arg);
    }

    return NULL;
}
