#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>
#include "Queue.h"

struct node
{
    struct node *next;
    void *       value;
};

struct queue
{
    node_t *front;
    node_t *rear;
    ssize_t size;
};

queue_t *
queue_create (void)
{
    return calloc(1, sizeof(queue_t));
}

int queue_enqueue(queue_t * queue, void * item)
{
    if (NULL == queue || NULL == item)
    {
        return -1;
    }
   
    node_t * node_to_enqueue = calloc(1, sizeof(node_t));
   
    if (NULL == node_to_enqueue)
    {
        return -1;
    }
    
    node_to_enqueue->value = item;
    node_to_enqueue->next = queue->front;
    queue->front = node_to_enqueue;
    (queue->size)++;
    
    return 0;
}
 
void * queue_dequeue(queue_t * queue)
{
    if (NULL == queue)
    {
        return NULL;
    }

    if (0 == queue->size)
    {
        return NULL;
    }

    node_t * current = queue->front;
    node_t * node_to_dequeue = queue->rear;

    if (queue->front == queue->rear)
    {
        void * dequeued_value = node_to_dequeue->value;
        free(node_to_dequeue);
        node_to_dequeue = NULL;
        (queue->size)--;
        return dequeued_value; 
    }

    while (current->next != queue->rear)
    {
        current = current->next;
    }

    queue->rear = current;
    current = NULL;
    void * dequeued_value = node_to_dequeue->value;
    free(node_to_dequeue);
    node_to_dequeue = NULL;
    (queue->size)--;
    return dequeued_value;    
}

int queue_size(queue_t * queue)
{
    return queue->size;
}


