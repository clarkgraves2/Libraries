#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

typedef struct pqueue pqueue_t;

pqueue_t *
pqueue_create (int (*compare) (int, int));

void
pqueue_destroy (pqueue_t ** pp_pqueue);

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

static int
default_compare (int a, int b);

#endif