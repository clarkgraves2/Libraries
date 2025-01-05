#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdbool.h>

typedef enum 
{
    MEM_DEFAULT,
    MEM_THREADS,
    MEM_QUEUE,
    MEM_MUTEX,
    MEM_BUFFER,
    MEM_MAX_CATEGORY
} mem_category_t;


typedef struct memory_tracker memory_tracker_t;
typedef struct tracked_ptr tracked_ptr_t;

memory_tracker_t * memory_create_tracker(void);

void memory_destroy_tracker(memory_tracker_t **tracker);

void * memory_calloc(memory_tracker_t *tracker, void **ptr, size_t count, size_t size, mem_category_t category);

bool memory_free(memory_tracker_t *tracker, void **ptr); 

void memory_free_all(memory_tracker_t *tracker, void ***ptrs, size_t num_ptrs);

void memory_free_category(memory_tracker_t *tracker, mem_category_t category, void ***ptrs, size_t num_ptrs);

#endif