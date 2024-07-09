#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

typedef struct pqueue pqueue_t;

pqueue_t * pqueue_create(void);

// If del_f is not NULL, del_f() will be called on
// each stored element that remains.
void pqueue_destroy(pqueue_t **pp_pqueue);

bool pqueue_insert(pqueue_t * p_pqueue, int weight, void * data);

// if p_weight is not NULL, pqueue_extract will store the weight
// of the extracted data
void *pqueue_extract(pqueue_t *p_pqueue, int *p_weight);

// Maybe the same interface as extract?
void *pqueue_peek(pqueue_t *p_pqueue);

bool pqueue_is_empty(pqueue_t *p_pqueue);

int pqueue_size(pqueue_t * p_pqueue);

// In .c file
// static void bubble_up(pqueue_t * p_pqueue, int index);
// static void bubble_down(pqueue_t * p_pqueue, int index);
// static void resize(pqueue_t * p_pqueue);

#endif