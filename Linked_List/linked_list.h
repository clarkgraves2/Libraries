#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <sys/types.h>

/**
 * @brief Opaque linked-list type.
 */
typedef struct llist llist_t;
typedef struct node node_t;

/**
 * @brief Creates the wrapper struct for a linked-list.
 * @param void Nothing is needed to pass to create list.
 * @return Valid pointer on success, anything else failure.
 */
llist_t * create_list(void);

/**
 * @brief Tracks size of the linked list
 * @param llist Passes in pointer to linked list
 * @return Size of list on success, -1 on failure.
 */
ssize_t list_size(llist_t * llist);

/**
 * @brief Creates a node for the link-list.
 * @param data Void pointer to data type held by list
 * @return Valid pointer on success, NULL on failure.
 */
node_t * create_node(void * data);

/**
 * @brief Inserts a node with value to front of list
 * @param data void pointer to data of node inserted
 * @return Returns pointer of inserted node on success,
 * NULL on failure.
 */
node_t * insert_front (llist_t * list, void * data);

/**
 * @brief Inserts a node with value to end of list
 * @param data void pointer to data of node inserted
 * @return Returns pointer of inserted node on success,
 * NULL on failure.
 */
node_t * insert_back (llist_t * list, void * data);

void insert_at(llist_t **, llist_t *, void * data);

void delete_at(llist_t **, void * data);

void delete_list(llist_t **);

ssize_t list_size(llist_t * llist);

void print(llist_t *);

/**
 * @brief Retrieves the data from a node for testing
 * @param node Passes in pointer to a node
 * @return data of a node on success, NULL on failure.
 */
void * get_node_data(node_t *node);

/**
 * @brief Retrieves the next point from a node for testing
 * @param node Passes in pointer to a node
 * @return pointer node on success, NULL on failure.
 */
node_t * get_next_ptr(node_t *node);

#endif
