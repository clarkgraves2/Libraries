#include "queue.h"
#include <check.h>
#include <fnmatch.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE_SIZE 100

struct q_node
{
    struct q_node * next;
    void *        value;
};

struct queue
{
    q_node_t * front;
    q_node_t * rear;
    ssize_t  size;
};

queue_t *
create_queue (void)
{
    return calloc (1, sizeof (queue_t));
}

void
destroy_queue (queue_t ** queue)
{
    if (queue == NULL || *queue == NULL)
    {
        return;
    }

    // Free all nodes in the queue
    //
    struct q_node * current = (*queue)->front;

    while (current != NULL)
    {
        struct q_node * temp = current;
        current            = current->next;
        free (temp);
    }

    // Free the queue structure itself
    //
    free (*queue);
    *queue = NULL;
}

int
queue_enqueue (queue_t * queue, void * item)
{
    if (NULL == queue || NULL == item || queue_is_full (queue))
    {
        return -1;
    }

    q_node_t * node_to_enqueue = calloc (1, sizeof (q_node_t));

    if (NULL == node_to_enqueue)
    {
        return -1;
    }

    node_to_enqueue->value = item;
    node_to_enqueue->next  = NULL;

    if (queue->rear == NULL)
    {
        queue->front = node_to_enqueue;
        queue->rear  = node_to_enqueue;
    }
    else
    {
        queue->rear->next = node_to_enqueue;
        queue->rear       = node_to_enqueue;
    }

    (queue->size)++;

    return 0;
}

int
queue_dequeue (queue_t * queue, void ** item)
{
    if (NULL == queue || NULL == item || queue->front == NULL)
    {
        return -1;
    }

    q_node_t * node_to_dequeue = queue->front;
    *item                    = node_to_dequeue->value;

    queue->front             = queue->front->next;
    (queue->size)--;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free (node_to_dequeue);
    return 0;
}

int
queue_size (queue_t * queue)
{
    return (queue != NULL) ? queue->size : -1;
}

bool
queue_is_empty (queue_t * queue)
{
    return (NULL == queue || queue->size == 0);
}

int
queue_peek (queue_t * queue, void ** item)
{
    if (queue == NULL || item == NULL || queue->front == NULL)
    {
        return -1;
    }
    *item = queue->front->value;
    return 0;
}

void
queue_clear (queue_t * queue)
{
    if (queue == NULL)
        return;

    while (!queue_is_empty (queue))
    {
        void * item;
        queue_dequeue (queue, &item);
        // We don't free(item) here
    }

    // Ensure the queue is in a consistent empty state
    queue->front = NULL;
    queue->rear  = NULL;
    queue->size  = 0;
}

queue_t *
queue_copy (queue_t * queue)
{
    if (queue == NULL)
        return NULL;
    queue_t * new_queue = create_queue();
    if (new_queue == NULL)
        return NULL;

    q_node_t * current = queue->front;
    while (current != NULL)
    {
        // We don't know the size of the item, so we'll just copy the pointer
        if (queue_enqueue (new_queue, current->value) != 0)
        {
            destroy_queue (&new_queue);
            return NULL;
        }
        current = current->next;
    }
    return new_queue;
}

void
queue_for_each (queue_t * queue, void (*func) (void *))
{
    if (queue == NULL || func == NULL)
        return;
    q_node_t * current = queue->front;
    while (current != NULL)
    {
        func (current->value);
        current = current->next;
    }
}

bool
queue_is_full (queue_t * queue)
{
    return (queue != NULL && queue->size >= MAX_QUEUE_SIZE);
}
