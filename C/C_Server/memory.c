#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

#define INITIAL_CAPACITY (16)

struct tracked_ptr 
{
    void *ptr;
    size_t size;
    mem_category_t type;
};

struct memory_tracker 
{
    struct tracked_ptr *tracked_ptrs;
    size_t count;
    size_t capacity;
};

memory_tracker_t *
memory_create_tracker(void) 
{
    memory_tracker_t *tracker = calloc(1, sizeof(memory_tracker_t));
    if (NULL == tracker) 
    {
        // Log Error
        return NULL;
    }

    tracker->tracked_ptrs = calloc(INITIAL_CAPACITY, sizeof(struct tracked_ptr));
    
    if (NULL == tracker->tracked_ptrs) 
    {
        // Log Error
        free(tracker);
        return NULL;
    }

    tracker->count = 0;
    tracker->capacity = INITIAL_CAPACITY;
    return tracker;
}

void 
memory_destroy_tracker(memory_tracker_t **tracker) 
{
    if (NULL == tracker || NULL == *tracker) 
    {
        return;
    }

    memory_free_all(*tracker, NULL, 0);

    free((*tracker)->tracked_ptrs);
    (*tracker)->tracked_ptrs = NULL;

    free(*tracker);
    *tracker = NULL;
}

void *
memory_calloc(memory_tracker_t *tracker, void **ptr, size_t count, size_t size, mem_category_t category) 
{
    if (size > 0 && count > SIZE_MAX / size) 
    {
        return NULL;
    }

    void *temp = calloc(count, size);
    if (NULL == temp) 
    {
        // Log Error
        return NULL;
    }

    *ptr = temp;

    if (tracker->count >= tracker->capacity) 
    {
        size_t new_capacity = tracker->capacity * 2;
        struct tracked_ptr *new_ptr_array = realloc(tracker->tracked_ptrs, new_capacity * sizeof(struct tracked_ptr));
        if (NULL == new_ptr_array) 
        {
            free(temp);
            *ptr = NULL;
            return NULL;
        }
        tracker->tracked_ptrs = new_ptr_array;
        tracker->capacity = new_capacity;
    }

    tracker->tracked_ptrs[tracker->count].ptr = temp;
    tracker->tracked_ptrs[tracker->count].size = count * size;
    tracker->tracked_ptrs[tracker->count].type = category;
    tracker->count++;

    return temp;
}

bool 
memory_free(memory_tracker_t *tracker, void **ptr) 
{
    if (NULL == tracker || NULL == ptr || NULL == *ptr) 
    {
        // Log Error
        return false;
    }

    for (size_t idx = 0; idx < tracker->count; idx++) 
    {
        if (tracker->tracked_ptrs[idx].ptr == *ptr) 
        {
            free(*ptr);
            *ptr = NULL;

            tracker->tracked_ptrs[idx].ptr = NULL;
            tracker->tracked_ptrs[idx].size = 0;
            tracker->tracked_ptrs[idx].type = MEM_DEFAULT;

            if (idx < tracker->count - 1) 
            {
                tracker->tracked_ptrs[idx] = tracker->tracked_ptrs[tracker->count - 1];
            }

            tracker->count--;
            return true;
        }
    }

    // Log Warning
    return false;
}

void 
memory_free_all(memory_tracker_t *tracker, void ***ptrs, size_t num_ptrs) 
{
    if (NULL == tracker || NULL == tracker->tracked_ptrs) 
    {
        return;
    }

    for (size_t idx = 0; idx < tracker->count; idx++)
    {
        free(tracker->tracked_ptrs[idx].ptr);
        tracker->tracked_ptrs[idx].ptr = NULL;
        tracker->tracked_ptrs[idx].size = 0;
        tracker->tracked_ptrs[idx].type = MEM_DEFAULT;
    }

    tracker->count = 0;

    if (ptrs != NULL) 
    {
        for (size_t i = 0; i < num_ptrs; i++) 
        {
            if (ptrs[i] != NULL) 
            {
                *(ptrs[i]) = NULL;
            }
        }
    }
}

void 
memory_free_category(memory_tracker_t *tracker, mem_category_t category, void ***ptrs, size_t num_ptrs) 
{
    if (NULL == tracker || NULL == tracker->tracked_ptrs) 
    {
        return;
    }

    if (category < MEM_DEFAULT || category >= MEM_MAX_CATEGORY) 
    {
        return;
    }

    size_t tracker_idx = 0;

    while (tracker_idx < tracker->count) 
    {
        if (tracker->tracked_ptrs[tracker_idx].type == category) 
        {
            free(tracker->tracked_ptrs[tracker_idx].ptr);
            tracker->tracked_ptrs[tracker_idx].ptr = NULL;

            tracker->count--;
            if (tracker_idx < tracker->count) 
            {
                tracker->tracked_ptrs[tracker_idx] = tracker->tracked_ptrs[tracker->count];
            }
        } 
        else 
        {
            tracker_idx++;
        }
    }

    if (ptrs != NULL) 
    {
        for (size_t i = 0; i < num_ptrs; i++) 
        {
            if (ptrs[i] != NULL) 
            {
                
                *(ptrs[i]) = NULL;
            }
        }
    }
}
