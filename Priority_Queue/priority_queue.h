#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct pqueue pqueue_t;

pqueue_t * 
pqueue_create(int (*compare)(const void*, const void*));

void
pqueue_destroy (pqueue_t * p_pqueue);

bool
pqueue_insert (pqueue_t * p_pqueue, int priority, void * data);

void *
pqueue_extract (pqueue_t * pqueue);

void *
pqueue_peek (const pqueue_t * p_pqueue);

bool
pqueue_is_empty (const pqueue_t * p_pqueue);

size_t
pqueue_size (const pqueue_t * p_pqueue);

bool 
pqueue_contains(const pqueue_t* pqueue, const void* item);

bool 
pqueue_change_priority(pqueue_t* pqueue, void* item, int new_priority);

#endif