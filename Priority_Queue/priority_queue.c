#include "priority_queue.h"
#include <stdio.h>

#define INITIAL_QUEUE_SIZE 8

typedef struct pqueue
{
    int **priority;    // Array of Priorities of items
    void **item;       // Array of pointers
    int size;        // Current size of the heap
    int capacity;    // Maximum capacity of the heap
}pqueue_t;

pqueue_t * pqueue_create(void)
{

    pqueue_t * base_p_queue = calloc(1, sizeof(pqueue_t));

    if (NULL == base_p_queue)
    {
        return NULL;
    }
    base_p_queue->size = 0;
    base_p_queue->capacity = INITIAL_QUEUE_SIZE;

    base_p_queue->item = calloc(INITIAL_QUEUE_SIZE, sizeof(void *));
    
    if (NULL == base_p_queue->item)
    {
        free(base_p_queue);
        base_p_queue = NULL;
        return NULL;
    }
    
    base_p_queue->priority = calloc(INITIAL_QUEUE_SIZE, sizeof(int *));
    
    if (NULL == base_p_queue->priority)
    {
        free(base_p_queue->item);
        free(base_p_queue);
        return NULL;
    }

    return base_p_queue;
}

void pqueue_destroy(pqueue_t **pp_pqueue)
{
    if (NULL == pp_pqueue || NULL == *pp_pqueue)
    {
        return;
    }

    pqueue_t *pqueue = *pp_pqueue;

    for (int idx = 0; idx < pqueue->size; idx++)
    {
        free(pqueue->item[idx]);
        free(pqueue->priority[idx]);
        pqueue->item[idx] = NULL;
        pqueue->priority[idx] = NULL;
    }

     free(pqueue->priority);
    pqueue->priority = NULL;

    free(pqueue->item);
    pqueue->item = NULL;

    free(pqueue);
    *pp_pqueue = NULL;
}

// Insert Function: Implement a function to insert elements with priorities into the heap.
// Extract Function: Implement a function to extract the highest priority element.
// Heapify Functions: Implement heapify up and heapify down functions to maintain the heap property.
// Resize Function: Implement a function to resize the array when the heap is full.


bool pqueue_insert(pqueue_t * p_pqueue, int priority, void * data)
{
    if (NULL == p_pqueue)
    {
        return false;
    }


}


// if p_weight is not NULL, pqueue_extract will store the weight
// of the extracted data
void *pqueue_extract(pqueue_t *p_pqueue, int *p_weight);

// Maybe the same interface as extract?
void *pqueue_peek(pqueue_t *p_pqueue);

bool pqueue_is_empty(pqueue_t *p_pqueue);

int pqueue_size(pqueue_t * p_pqueue);