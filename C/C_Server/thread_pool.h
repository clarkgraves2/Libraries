#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "queue.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct thread_pool thread_pool_t;
typedef struct thread_job thread_job_t;

thread_pool_t * thread_pool_initialize(int num_of_threads);
thread_job_t * thread_pool_job(void * (*job_function));

#endif THREAD_POOL_H