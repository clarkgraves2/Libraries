#include "priority_queue.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_QUEUE_SIZE 8
#define ROOT_INDEX         0

struct pqueue
{
    int *   priorities;        // Array of priorities of items
    void ** items;             // Array of pointers to items
    size_t  size;              // Current size of the heap
    size_t  capacity;          // Maximum capacity of the heap
    int (*compare) (int, int); // Comparison function for priorities
};

// Create a new priority queue
pqueue_t *
pqueue_create (int (*compare) (int, int))
{
    pqueue_t * queue = calloc (1, sizeof (pqueue_t));
    if (NULL == queue)
    {
        return NULL;
    }

    queue->priorities = calloc (INITIAL_QUEUE_SIZE, sizeof (int));
    queue->items      = calloc (INITIAL_QUEUE_SIZE, sizeof (void *));

    if (NULL == queue->priorities || NULL == queue->items)
    {
        pqueue_destroy (&queue);
        return NULL;
    }

    queue->size     = 0;
    queue->capacity = INITIAL_QUEUE_SIZE;
    queue->compare = compare ? compare : (int (*) (int, int)) & default_compare;

    return queue;
}

// Default comparison function (min-heap)
static int
default_compare (int a, int b)
{
    return a - b;
}

// Destroy the priority queue and free all associated memory
void
pqueue_destroy (pqueue_t ** pp_pqueue)
{
    if (NULL == pp_pqueue || NULL == *pp_pqueue)
    {
        return;
    }

    pqueue_t * pqueue = *pp_pqueue;
    free (pqueue->priorities);
    free (pqueue->items);
    free (pqueue);
    *pp_pqueue = NULL;
}

// Insert an item into the priority queue
bool
pqueue_insert (pqueue_t * p_pqueue, int priority, void * data)
{
    if (NULL == p_pqueue)
    {
        return false;
    }

    if (p_pqueue->size == p_pqueue->capacity)
    {
        if (!pqueue_resize (p_pqueue))
        {
            return false;
        }
    }

    p_pqueue->items[p_pqueue->size]      = data;
    p_pqueue->priorities[p_pqueue->size] = priority;
    pqueue_heapify_up (p_pqueue, p_pqueue->size);
    p_pqueue->size++;

    return true;
}

// Extract the highest priority item from the queue
void *
pqueue_extract (pqueue_t * pqueue)
{
    if (NULL == pqueue || 0 == pqueue->size)
    {
        return NULL;
    }

    void * root_item = pqueue->items[0];
    pqueue->size--;

    if (pqueue->size > 0)
    {
        pqueue->items[0]      = pqueue->items[pqueue->size];
        pqueue->priorities[0] = pqueue->priorities[pqueue->size];
        pqueue_heapify_down (pqueue, 0);
    }

    return root_item;
}

// Peek at the highest priority item without removing it
void *
pqueue_peek (const pqueue_t * p_pqueue)
{
    if (NULL == p_pqueue || 0 == p_pqueue->size)
    {
        return NULL;
    }
    return p_pqueue->items[0];
}

// Check if the priority queue is empty
bool
pqueue_is_empty (const pqueue_t * p_pqueue)
{
    return (NULL == p_pqueue || 0 == p_pqueue->size);
}

// Get the current size of the priority queue
size_t
pqueue_size (const pqueue_t * p_pqueue)
{
    return (NULL == p_pqueue) ? 0 : p_pqueue->size;
}

// Heapify up operation
static void
pqueue_heapify_up (pqueue_t * pqueue, size_t current_index)
{
    while (current_index > 0)
    {
        size_t parent_index = (current_index - 1) / 2;
        if (pqueue->compare (pqueue->priorities[current_index],
                             pqueue->priorities[parent_index])
            >= 0)
        {
            break;
        }

        // Swap items and priorities
        void * temp_item                  = pqueue->items[current_index];
        pqueue->items[current_index]      = pqueue->items[parent_index];
        pqueue->items[parent_index]       = temp_item;

        int temp_priority                 = pqueue->priorities[current_index];
        pqueue->priorities[current_index] = pqueue->priorities[parent_index];
        pqueue->priorities[parent_index]  = temp_priority;

        current_index                     = parent_index;
    }
}

// Heapify down operation
static void
pqueue_heapify_down (pqueue_t * pqueue, size_t index)
{
    while (index < pqueue->size)
    {
        size_t left_child  = 2 * index + 1;
        size_t right_child = 2 * index + 2;
        size_t smallest    = index;

        if (left_child < pqueue->size
            && pqueue->compare (pqueue->priorities[left_child],
                                pqueue->priorities[smallest])
                   < 0)
        {
            smallest = left_child;
        }

        if (right_child < pqueue->size
            && pqueue->compare (pqueue->priorities[right_child],
                                pqueue->priorities[smallest])
                   < 0)
        {
            smallest = right_child;
        }

        if (smallest == index)
        {
            break;
        }

        // Swap items and priorities
        void * temp_item             = pqueue->items[index];
        pqueue->items[index]         = pqueue->items[smallest];
        pqueue->items[smallest]      = temp_item;

        int temp_priority            = pqueue->priorities[index];
        pqueue->priorities[index]    = pqueue->priorities[smallest];
        pqueue->priorities[smallest] = temp_priority;

        index                        = smallest;
    }
}

// Resize the priority queue
static bool
pqueue_resize (pqueue_t * pqueue)
{
    size_t  new_capacity = pqueue->capacity * 2;
    void ** new_items = realloc (pqueue->items, new_capacity * sizeof (void *));
    int *   new_priorities
        = realloc (pqueue->priorities, new_capacity * sizeof (int));

    if (NULL == new_items || NULL == new_priorities)
    {
        free (new_items);
        free (new_priorities);
        return false;
    }

    pqueue->items      = new_items;
    pqueue->priorities = new_priorities;
    pqueue->capacity   = new_capacity;

    return true;
}