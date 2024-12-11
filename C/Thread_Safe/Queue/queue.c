/** @file queue.c
 *
 * @brief Implementation of thread-safe FIFO queue for inter-thread communication.
 *
 * @details
 * This implementation provides a thread-safe queue with the following features:
 *   - Mutex-protected access to queue data
 *   - Condition variables for blocking operations
 *   - Dynamic node allocation for queue elements
 *   - Automatic resource cleanup on destruction
 *
 * Key implementation details:
 *   - Uses a linked-list structure for O(1) enqueue/dequeue operations
 *   - Implements blocking wait on full/empty conditions
 *   - Provides proper cleanup of resources in error cases
 *   - Handles thread synchronization for multiple producers/consumers
 *
 * NOTE: Implementation assumptions:
 *   - Maximum queue size is compile-time constant
 *   - Memory allocation failures must be handled by caller
 *   - Caller responsible for managing memory of stored items
 *
 * WARNING: Important considerations:
 *   - Not safe for use in interrupt context
 *   - Blocking operations may deadlock if used incorrectly
 *   - Memory allocation occurs during enqueue operations
 *   - Queue must be explicitly destroyed to free resources
 *
 * Example internal node structure:
 * @code
 *     struct node {
 *         struct node *p_next;    // Next node in queue
 *         void *p_value;          // Stored value pointer
 *     };
 * @endcode
 *
 * @par
 * Change History:
 * 1.0.0 (2024-01-15) - Initial implementation
 *   - Basic queue operations implemented
 *   - Thread safety mechanisms added
 *   - Unit tests completed
 */

#include "queue.h"
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Node structure for queue elements
 */
struct queue_node
{
    struct queue_node *p_next;  
    void *p_data;             
};

/**
 * @brief Main queue structure
 */
struct queue 
{
    node_t *p_head;            /* Pointer to first node */
    node_t *p_tail;            /* Pointer to last node */
    uint32_t current_size;     /* Current number of elements */
    pthread_mutex_t mutex;     /* Queue access mutex */
    pthread_cond_t not_empty;  /* Signaled when queue becomes not empty */
    pthread_cond_t not_full;   /* Signaled when queue becomes not full */
    bool b_initialized;        /* Initialization status flag */
};

/* Static function prototypes */
static queue_err_t lock_queue(queue_t *p_queue);
static queue_err_t unlock_queue(queue_t *p_queue);
static bool validate_queue(const queue_t *p_queue);

queue_t *
queue_create(void)
{
    queue_t *p_queue = calloc(1, sizeof(queue_t));
    
    if (NULL == p_queue)
    {
        return NULL;
    }

    if (0 != pthread_mutex_init(&p_queue->mutex, NULL))
    {
        free(p_queue);
        return NULL;
    }

    if (0 != pthread_cond_init(&p_queue->not_empty, NULL))
    {
        pthread_mutex_destroy(&p_queue->mutex);
        free(p_queue);
        return NULL;
    }

    if (0 != pthread_cond_init(&p_queue->not_full, NULL))
    {
        pthread_mutex_destroy(&p_queue->mutex);
        pthread_cond_destroy(&p_queue->not_empty);
        free(p_queue);
        return NULL;
    }

    p_queue->p_head = NULL;
    p_queue->p_tail = NULL;
    p_queue->current_size = 0u;
    p_queue->b_initialized = true;

    return p_queue;
}

void
queue_destroy(queue_t **pp_queue)
{
    if ((NULL == pp_queue) || (NULL == *pp_queue))
    {
        return;
    }

    queue_t *p_queue = *pp_queue;

    if (p_queue->b_initialized)
    {
        /* Lock queue before cleanup */
        if (QUEUE_ACT_SUCCESS != lock_queue(p_queue))
        {
            return;
        }

        /* Free all nodes */
        node_t *p_current = p_queue->p_head;
        while (NULL != p_current)
        {
            node_t *p_temp = p_current;
            p_current = p_current->p_next;
            free(p_temp);
        }

        /* Release lock and destroy synchronization objects */
        unlock_queue(p_queue);
        pthread_mutex_destroy(&p_queue->mutex);
        pthread_cond_destroy(&p_queue->not_empty);
        pthread_cond_destroy(&p_queue->not_full);
    }

    /* Free queue structure and clear caller's pointer */
    free(p_queue);
    *pp_queue = NULL;
}

queue_err_t
queue_enqueue(queue_t *p_queue, void *p_item)
{
    queue_err_t result;

    if ((NULL == p_queue) || (NULL == p_item))
    {
        return QUEUE_ERR_NULL_PTR;
    }

    if (!validate_queue(p_queue))
    {
        return QUEUE_ERR_GENERIC;
    }

    if (QUEUE_ACT_SUCCESS != (result = lock_queue(p_queue)))
    {
        return result;
    }

    /* Wait while queue is full */
    while (p_queue->current_size >= QUEUE_MAX_SIZE)
    {
        if (0 != pthread_cond_wait(&p_queue->not_full, &p_queue->mutex))
        {
            unlock_queue(p_queue);
            return QUEUE_ERR_INTERRUPT;
        }
    }

    /* Create new node */
    node_t *p_new_node = calloc(1, sizeof(node_t));
    if (NULL == p_new_node)
    {
        unlock_queue(p_queue);
        return QUEUE_ERR_NO_MEM;
    }

    /* Initialize new node */
    p_new_node->p_data = p_item;
    p_new_node->p_next = NULL;

    /* Add to queue */
    if (NULL == p_queue->p_tail)
    {
        p_queue->p_head = p_new_node;
        p_queue->p_tail = p_new_node;
    }
    else
    {
        p_queue->p_tail->p_next = p_new_node;
        p_queue->p_tail = p_new_node;
    }

    p_queue->current_size++;

    /* Signal waiting threads */
    pthread_cond_signal(&p_queue->not_empty);
    unlock_queue(p_queue);

    return QUEUE_ACT_SUCCESS;
}

queue_err_t
queue_dequeue(queue_t *p_queue, void **pp_item)
{
    queue_err_t result;

    if ((NULL == p_queue) || (NULL == pp_item))
    {
        return QUEUE_ERR_NULL_PTR;
    }

    if (!validate_queue(p_queue))
    {
        return QUEUE_ERR_GENERIC;
    }

    if (QUEUE_ACT_SUCCESS != (result = lock_queue(p_queue)))
    {
        return result;
    }

    /* Wait while queue is empty */
    while (0u == p_queue->current_size)
    {
        if (0 != pthread_cond_wait(&p_queue->not_empty, &p_queue->mutex))
        {
            unlock_queue(p_queue);
            return QUEUE_ERR_INTERRUPT;
        }
    }

    /* Remove from front of queue */
    node_t *p_node = p_queue->p_head;
    *pp_item = p_node->p_data;

    p_queue->p_head = p_queue->p_head->p_next;
    p_queue->current_size--;

    if (NULL == p_queue->p_head)
    {
        p_queue->p_tail = NULL;
    }

    free(p_node);

    pthread_cond_signal(&p_queue->not_full);
    unlock_queue(p_queue);

    return QUEUE_ACT_SUCCESS;
}

int32_t
queue_size(queue_t *p_queue)
{
    queue_err_t result;
    int32_t size;

    if (NULL == p_queue)
    {
        return QUEUE_ERR_NULL_PTR;
    }

    if (QUEUE_ACT_SUCCESS != (result = lock_queue(p_queue)))
    {
        return -1;
    }

    size = (int32_t)p_queue->current_size;
    unlock_queue(p_queue);

    return size;
}

bool
queue_is_empty(queue_t *p_queue)
{
    queue_err_t result;
    bool b_is_empty;

    if (NULL == p_queue)
    {
        return true;
    }

    if (QUEUE_ACT_SUCCESS != (result = lock_queue(p_queue)))
    {
        return true;
    }

    b_is_empty = (0u == p_queue->current_size);
    unlock_queue(p_queue);

    return b_is_empty;
}

queue_err_t
queue_peek(queue_t *p_queue, void **pp_item)
{
    queue_err_t result;

    if ((NULL == p_queue) || (NULL == pp_item))
    {
        return QUEUE_ERR_NULL_PTR;
    }

    if (!validate_queue(p_queue))
    {
        return QUEUE_ERR_GENERIC;
    }

    if (QUEUE_ACT_SUCCESS != (result = lock_queue(p_queue)))
    {
        return result;
    }

    if (NULL == p_queue->p_head)
    {
        unlock_queue(p_queue);
        return QUEUE_ERR_EMPTY;
    }

    *pp_item = p_queue->p_head->p_data;
    unlock_queue(p_queue);

    return QUEUE_ACT_SUCCESS;
}

void
queue_clear(queue_t *p_queue)
{
    if (NULL == p_queue)
    {
        return;
    }

    if (QUEUE_ACT_SUCCESS != lock_queue(p_queue))
    {
        return;
    }

    while (NULL != p_queue->p_head)
    {
        node_t *p_temp = p_queue->p_head;
        p_queue->p_head = p_queue->p_head->p_next;
        free(p_temp);
    }

    p_queue->p_tail = NULL;
    p_queue->current_size = 0u;

    pthread_cond_broadcast(&p_queue->not_full);
    unlock_queue(p_queue);
}

bool
queue_is_full(queue_t *p_queue)
{
    return ((NULL != p_queue) && 
            (p_queue->current_size >= QUEUE_MAX_SIZE));
}

/**
 * @brief Validates queue structure integrity
 *
 * @param[in] p_queue Queue to validate
 *
 * @return true if valid, false otherwise
 */
static bool
validate_queue(const queue_t *p_queue)
{
    if (!p_queue->b_initialized)
    {
        return false;
    }

    if ((NULL == p_queue->p_head) != (NULL == p_queue->p_tail))
    {
        return false;
    }

    if ((0u == p_queue->current_size) != (NULL == p_queue->p_head))
    {
        return false;
    }

    return true;
}

/**
 * @brief Acquires queue mutex
 *
 * @param[in] p_queue Queue to lock
 *
 * @return QUEUE_ACT_SUCCESS on success, error code on failure
 */
static queue_err_t
lock_queue(queue_t *p_queue)
{
    if (0 != pthread_mutex_lock(&p_queue->mutex))
    {
        return QUEUE_ERR_LOCKED;
    }

    return QUEUE_ACT_SUCCESS;
}
