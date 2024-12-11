/** @file queue.h
 *
 * @brief Thread-safe FIFO queue interface for inter-thread communication.
 *
 * @details
 * This module provides a generic thread-safe queue implementation that supports:
 *   - Thread-safe enqueue and dequeue operations
 *   - Blocking and non-blocking queue operations
 *   - Dynamic memory allocation for queue nodes
 *   - Condition variables for thread synchronization
 *
 * NOTE: This module assumes the following conditions are met:
 *   - POSIX threads (pthread) library is available
 *   - Dynamic memory allocation is permitted
 *   - System supports mutex and condition variables
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2024 Company Name. All rights reserved.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>

/* Constants */
#define QUEUE_MAX_SIZE 100u

/**
 * @brief Error codes for queue operations
 */
typedef enum
{
    QUEUE_ACT_SUCCESS   =  0,    /* Operation completed successfully */
    QUEUE_ERR_GENERIC   = -1,    /* Generic error occurred */
    QUEUE_ERR_FULL      = -2,    /* Queue is full */
    QUEUE_ERR_EMPTY     = -3,    /* Queue contains no elements */
    QUEUE_ERR_LOCKED    = -4,    /* Queue is currently locked by another thread */
    QUEUE_ERR_INTERRUPT = -5,    /* Operation was interrupted by a signal */
    QUEUE_ERR_NULL_PTR  = -6,    /* NULL pointer provided as argument */
    QUEUE_ERR_NO_MEM    = -7     /* Memory allocation failed */
} queue_err_t;

typedef struct queue_node node_t;
typedef struct queue queue_t;

/**
 * @brief Creates a queue.
 * 
 * @return Pointer to new queue / NULL if allocation fails.
 */
queue_t * queue_create(void);

/**
 * @brief Destroys a queue and frees all associated memory.
 * 
 * @param[in,out] pp_queue Double pointer to queue to destroy. Set to NULL after cleanup.
 */
void queue_destroy(queue_t **pp_queue);

/**
 * @brief Adds an item to the back of the queue.
 * 
 * @param[in] p_queue The queue to add to
 * @param[in] p_item The item to add
 * 
 * @return QUEUE_ACT_SUCCESS on success, error code on failure
 */
queue_err_t queue_enqueue(queue_t *p_queue, void *p_item);

/**
 * @brief Removes and returns the item at the front of the queue.
 * 
 * @param[in] p_queue The queue to remove from
 * @param[out] pp_item Pointer that will receive the dequeued item
 * 
 * @return QUEUE_ACT_SUCCESS on success, error code on failure
 */
queue_err_t queue_dequeue(queue_t *p_queue, void **pp_item);

/**
 * @brief Returns the current number of items in the queue.
 * 
 * @param[in] p_queue The queue to check
 * 
 * @return Current queue size, or -1 on error
 */
int32_t queue_size(queue_t *p_queue);

/**
 * @brief Checks if the queue is empty.
 * 
 * @param[in] p_queue The queue to check
 * 
 * @return true if queue is empty or invalid, false otherwise
 */
bool queue_is_empty(queue_t *p_queue);

/**
 * @brief Returns the item at the front of the queue without removing it.
 * 
 * @param[in] p_queue The queue to peek
 * @param[out] pp_item Pointer that will receive the item at front of queue
 * 
 * @return QUEUE_ACT_SUCCESS on success, error code on failure
 */
queue_err_t queue_peek(queue_t *p_queue, void **pp_item);

/**
 * @brief Removes all items from the queue.
 * 
 * @param[in] p_queue The queue to clear
 */
void queue_clear(queue_t *p_queue);

/**
 * @brief Returns whether the queue is at maximum capacity.
 * 
 * @param[in] p_queue The queue to check
 * 
 * @return true if queue is full or invalid, false otherwise
 */
bool queue_is_full(queue_t *p_queue);

#endif /* QUEUE_H */