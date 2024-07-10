#include "priority_queue.h"
#include <stdio.h>

#define INITIAL_QUEUE_SIZE 8
#define ROOT_INDEX 0

typedef struct pqueue
{
    int *priority;    // Array of Priorities of items
    void **item;       // Array of pointers
    int size_index;        // Current size of the heap 
    int capacity;    // Maximum capacity of the heap
}pqueue_t;

pqueue_t * pqueue_create(void)
{

    pqueue_t * base_p_queue = calloc(1, sizeof(pqueue_t));

    if (NULL == base_p_queue)
    {
        return NULL;
    }
    base_p_queue->size_index = 0;
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

    free(pqueue->priority);
    pqueue->priority = NULL;

    free(pqueue->item);
    pqueue->item = NULL;

    free(pqueue);
    *pp_pqueue = NULL;
}

bool pqueue_insert(pqueue_t * p_pqueue, int priority, void * data)
{
    if (NULL == p_pqueue)
    {
        return false;
    }

    if (p_pqueue->size_index == p_pqueue->capacity)
    {
        if (!pqueue_resize(p_pqueue))
        {
            return false;
        }
    }

    p_pqueue->item[p_pqueue->size_index] = data;
    p_pqueue->priority[p_pqueue->size_index] = priority; 
    pqueue_heapify_up(p_pqueue, p_pqueue->size_index);
    p_pqueue->size_index++;

    return true; 
}

void *pqueue_extract(pqueue_t *pqueue)
{
    if (pqueue->size_index == 0)
    {
        return NULL; 
    }

  
    void *root_item = pqueue->item[0];
    int root_priority = pqueue->priority[0];

   
    pqueue->item[0] = pqueue->item[pqueue->size_index - 1];
    pqueue->priority[0] = pqueue->priority[pqueue->size_index - 1];

    pqueue->size_index--;

    pqueue_heapify_down(pqueue, 0);

    return root_item;
}

void *pqueue_peek(pqueue_t *p_pqueue)
{
    if (p_pqueue == NULL || p_pqueue->size_index == 0)
    {
        return NULL; 
    }
    return p_pqueue->item[0];
}

bool pqueue_is_empty(pqueue_t *p_pqueue)
{
    if (NULL == p_pqueue || 0 ==  p_pqueue->size_index)
    {
        return true;
    }
    return false;
}

int pqueue_size(pqueue_t * p_pqueue)
{
    if (NULL == p_pqueue)
    {
        return 0;
    }
    return p_pqueue->size_index;
}

static void pqueue_heapify_up(pqueue_t *pqueue, int current_index)
{
    while (current_index != ROOT_INDEX)
    {
        int parent_index = (current_index - 1) / 2;
        if (pqueue->priority[current_index] >= pqueue->priority[parent_index])
        {
            break;
        }

        void *temp_item = pqueue->item[current_index];
        pqueue->item[current_index] = pqueue->item[parent_index];
        pqueue->item[parent_index] = temp_item;

        int temp_priority = pqueue->priority[current_index];
        pqueue->priority[current_index] = pqueue->priority[parent_index];
        pqueue->priority[parent_index] = temp_priority;

        current_index = parent_index;
    }
}

static void pqueue_heapify_down(pqueue_t *pqueue, int index)
{
    while (index < pqueue->size_index)
    {
        int left_child = 2 * index + 1;
        int right_child = 2 * index + 2;
        int smallest = index;

        if (left_child < pqueue->size_index && pqueue->priority[left_child] < pqueue->priority[smallest])
        {
            smallest = left_child;
        }

        if (right_child < pqueue->size_index && pqueue->priority[right_child] < pqueue->priority[smallest])
        {
            smallest = right_child;
        }

        if (smallest == index)
        {
            break;
        }

        void *temp_item = pqueue->item[index];
        pqueue->item[index] = pqueue->item[smallest];
        pqueue->item[smallest] = temp_item;

        int temp_priority = pqueue->priority[index];
        pqueue->priority[index] = pqueue->priority[smallest];
        pqueue->priority[smallest] = temp_priority;

        index = smallest;
    }
}


 
static bool pqueue_resize(pqueue_t *pqueue)
{
    int new_capacity = pqueue->capacity * 2;
    void **new_items = realloc(pqueue->item, new_capacity * sizeof(void *));
    if (new_items == NULL)
    {
        return false; 
    }

    int **new_priorities = realloc(pqueue->priority, new_capacity * sizeof(int *));
    if (new_priorities == NULL)
    {
        free(new_items);
        return false;
    }

    pqueue->item = new_items;
    pqueue->priority = new_priorities;
    pqueue->capacity = new_capacity;

    return true;
}
