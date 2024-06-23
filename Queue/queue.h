/**
 * @file queue.h
 * @brief Header file for a queue library.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <sys/types.h>
#include <stdbool.h>

/**
 * @brief Opaque linked-list type.
 */
typedef struct queue queue_t;
typedef struct node node_t;

/**
 * @brief Creates a queue.
 * @return A pointer to the newly created queue, or NULL if memory
 * allocation fails.
 */
queue_t * create_queue();

/**
 * @brief Destroys a queue and frees all associated memory.
 * @param queue A double pointer to the queue to destroy.
 * The pointer will be set to NULL after the queue is
 * destroyed.
 */
void destroy_queue (queue_t **queue);

int queue_enqueue (queue_t * queue, void * item);

int queue_dequeue(queue_t *queue, void **item);

int queue_size (queue_t * queue);

bool queue_is_empty(queue_t * queue); 

#endif /* QUEUE_H */
