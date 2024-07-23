// linked_list.h
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>
#include <sys/types.h>

typedef struct llist_node llist_node_t;
typedef struct llist llist_t;

llist_t *llist_create(void);
void llist_destroy(llist_t **list);
ssize_t llist_size(const llist_t *list);
llist_node_t *llist_create_node(void *data);
llist_node_t *llist_insert_front(llist_t *list, void *data);
llist_node_t *llist_insert_back(llist_t *list, void *data);
llist_node_t *llist_insert_at(llist_t *list, size_t position, void *data);
int llist_delete_node(llist_t *list, const void *data);
void *llist_get_node_data(const llist_node_t *node);
llist_node_t *llist_get_next_ptr(const llist_node_t *node);

#endif // LINKED_LIST_H
