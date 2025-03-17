/** @file queue.h
 *
 * @brief Interface for a minimal FIFO queue data structure.
 *
 * This module provides essential queue operations needed for thread pool
 * job management. The implementation is designed to be thread-safe when used 
 * with external synchronization.
 *
 */

 #ifndef QUEUE_H
 #define QUEUE_H

 #include <stdbool.h>
 
 /*************************************************************************
 * Type Definitions
 *************************************************************************/
 
 // @brief Opaque queue structure 
 typedef struct queue queue_t;
 
 // @brief Node structure for queue elements 
 typedef struct node node_t;
 
 /*************************************************************************
 * Function Declarations 
 *************************************************************************/
 
 /** 
  * @brief Creates a new empty queue.
  *
  * @return Pointer to newly created queue or NULL if allocation fails
  */
 queue_t * 
 queue_create(void);
 
 /** 
  * @brief Destroys a queue and frees all associated memory.
  *
  * @param[in,out] p_queue Double pointer to queue. Set to NULL after freeing.
  * @return true on sucessful destroy, false on error.
  */
 bool 
 queue_destroy(queue_t ** p_queue);
 
 /** 
  * @brief Adds an item to the back of the queue.
  *
  * @param[in,out] p_queue The queue to add to
  * @param[in] p_item The item to add
  *
  * @return true on success, false on failure
  */
 bool 
 queue_enqueue(queue_t *p_queue, void *p_item);
 
 /** 
  * @brief Removes and returns the item at front of queue.
  *
  * @param[in,out] p_queue The queue to remove from 
  * @param[out] pp_item Where to store the dequeued item
  *
  * @return true on success, false on failure
  */
 bool 
 queue_dequeue(queue_t * p_queue, void ** pp_item);
 
 /**
  * @brief Gets the current size of the queue.
  *
  * @param[in] p_queue The queue to check
  *
  * @return Current queue size, or -1 if queue is NULL
  */
 int 
 queue_size(queue_t *p_queue);
 
 /**
  * @brief Checks if queue is empty.
  *
  * @param[in] p_queue The queue to check
  *
  * @return true if empty or NULL, false otherwise
  */
 bool 
 queue_is_empty(queue_t *p_queue);
 
 #endif /* QUEUE_H */
 
 /*** end of file ***/
