/** @file queue.c
 *
 * @brief Implementation of a queue data structure.
 */

 #include "queue.h"
 #include <stdlib.h>
 #include <stdint.h>
 
 /*************************************************************************
  * Constants and Macros
  *************************************************************************/
 
 /* Maximum number of items allowed in queue to prevent unbounded growth */
 #define MAX_QUEUE_SIZE (100u)
 
 /*************************************************************************
  * Private Data Structures
  *************************************************************************/
 
 /* Node structure representing each element in the queue */
 typedef struct node
 {
     struct node * next;    /* Points to next node in queue */
     void * p_value;        /* Stores the actual data */
 } node_t;
 
 /* Main queue structure with front and rear pointers for O(1) operations */
 struct queue 
 {
     node_t * p_front;      /* Points to first element */
     node_t * p_rear;       /* Points to last element */
     uint32_t size;         /* Number of elements currently in queue */
 };
 
 /*************************************************************************
  * Public Functions
  *************************************************************************/
 
 /*!
  * @brief Creates a new empty queue.
  *
  * Allocates and initializes a new queue structure with NULL pointers
  * and size set to 0.
  *
  * @return Pointer to newly created queue if successful, NULL if memory
  *         allocation fails
  */
 queue_t *
 queue_create(void)
 {
     return calloc(1, sizeof(queue_t));
 }
 
 /*!
  * @brief Destroys a queue and frees all associated memory.
  *
  * Frees all nodes in the queue and the queue structure itself.
  * Sets the pointer to the queue to NULL after freeing.
  *
  * @param[in,out] p_queue Double pointer to the queue to destroy
  * @return true on successful destroy, false on error
  */
 bool 
 queue_destroy(queue_t ** const p_queue)
 {
     if ((NULL == p_queue) || (NULL == *p_queue))
     {
         return false;
     }
 
     /* Traversing the linked list, freeing each node */
     node_t * p_current = (*p_queue)->p_front;
     while (NULL != p_current)
     {
         node_t * p_temp = p_current;  /* Save current node */
         p_current = p_current->next;  /* Move to next node */
         free(p_temp);                 /* Free saved node */
     }
 
     free(*p_queue);
     *p_queue = NULL;
     return true;
 }
 
 /*!
  * @brief Adds an item to the end of the queue.
  *
  * Creates a new node containing the item and adds it to the rear
  * of the queue. Updates front pointer if queue was empty.
  *
  * @param[in] p_queue Pointer to the queue
  * @param[in] p_item Pointer to the item to enqueue
  * @return true on success, false on failure (queue full or invalid params)
  */
 bool
 queue_enqueue(queue_t * const p_queue, void * const p_item)
 {
     /* Check for invalid input or queue at maximum capacity */
     if ((NULL == p_queue) || (NULL == p_item) || (p_queue->size >= MAX_QUEUE_SIZE))
     {
         return false;
     }
 
     /* Allocate and initialize new node */
     node_t * p_new_node = calloc(1, sizeof(node_t));
     if (NULL == p_new_node)
     {
         return false;
     }
 
     /* Set up the new node's data */
     p_new_node->p_value = p_item;
     p_new_node->next = NULL;
 
     /* If queue is empty, set both front and rear to new node */
     if (NULL == p_queue->p_rear)
     {
         p_queue->p_front = p_new_node;
         p_queue->p_rear = p_new_node;
     }
     /* If not, add to end and update rear pointer */
     else
     {
         p_queue->p_rear->next = p_new_node;
         p_queue->p_rear = p_new_node;
     }
 
     p_queue->size++;
     return true;
 }
 
 /*!
  * @brief Removes and returns the item at the front of the queue.
  *
  * Removes the front node of the queue and returns its value through
  * the pp_item parameter. Updates rear pointer if queue becomes empty.
  *
  * @param[in] p_queue Pointer to the queue
  * @param[out] pp_item Pointer to store the dequeued item
  * @return true on success, false on failure (queue empty or invalid params)
  */
 bool
 queue_dequeue(queue_t * const p_queue, void ** const pp_item)
 {
     if ((NULL == p_queue) || (NULL == pp_item) || (NULL == p_queue->p_front))
     {
         return false;
     }
 
     /* Save the front node and its value */
     node_t * p_node_to_remove = p_queue->p_front;
     *pp_item = p_node_to_remove->p_value;
 
     /* Update front pointer to next node */
     p_queue->p_front = p_queue->p_front->next;
     p_queue->size--;
 
     /* If queue is now empty, update rear pointer as well */
     if (NULL == p_queue->p_front)
     {
         p_queue->p_rear = NULL;
     }
 
     free(p_node_to_remove);
     return true;
 }
 
 /*!
  * @brief Returns the current number of items in the queue.
  *
  * @param[in] p_queue Pointer to the queue
  * @return Number of items in queue if valid, -1 if queue pointer is NULL
  */
 int32_t
 queue_size(queue_t * const p_queue)
 {
     return ((NULL != p_queue) ? ((int32_t)p_queue->size) : -1);
 }
 
 /*!
  * @brief Checks if the queue is empty.
  *
  * @param[in] p_queue Pointer to the queue
  * @return true if queue is empty or NULL, false otherwise
  */
 bool
 queue_is_empty(queue_t * const p_queue)
 {
     /* Queue is empty if NULL or size is 0 */
     return ((NULL == p_queue) || (0u == p_queue->size));
 }
 
 /*** end of file ***/