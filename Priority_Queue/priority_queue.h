#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

typedef struct pqueue pqueue_t;

pqueue_t * pqueue_create(void);

void pqueue_destroy(pqueue_t **pp_pqueue);

bool pqueue_insert(pqueue_t * p_pqueue, int weight, void * data);

void *pqueue_extract(pqueue_t *p_pqueue, int *p_weight);

void *pqueue_peek(pqueue_t *p_pqueue);

bool pqueue_is_empty(pqueue_t *p_pqueue);

int pqueue_size(pqueue_t * p_pqueue);

#endif