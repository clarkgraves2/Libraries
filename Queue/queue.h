/**
 * @file llist.h
 * @brief Header file for a linked-list library.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <sys/types.h>

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
queue_t *queue_create();

/**
 * @brief Destroys a queue and frees all associated memory.
 * @param list A double pointer to the queue to destroy.
 * The pointer will be set to NULL after the queue is
 * destroyed.
 */
void queue_destroy(queue_t **list);

/**
 * @brief Appends a string to the end of the linked-list.
 * @param list A pointer to the linked-list.
 * @param string The string to append.
 * @return 0 on success, -1 on failure (e.g., memory allocation failure).
 */
int llist_append(llist_t *list, char * string);

/**
 * @brief Adds a string at a specific index in the linked-list.
 * @param list A pointer to the linked-list.
 * @param index The index at which to add the string (0-based).
 * @param string The string to add.
 * @return 0 on success, -1 on failure (e.g., invalid index or memory allocation
 * failure).
 */
int llist_add_at(llist_t *list, size_t index, char * string);

/**
 * @brief Changes the string at a specific index in the linked-list.
 * @param list A pointer to the linked-list.
 * @param index The index of the string to change (0-based).
 * @param string The new string.
 * @return 0 on success, -1 on failure (e.g., invalid index).
 */
int llist_change_at(llist_t *list, size_t index, char * string);

/**
 * @brief Finds the first occurrence of a string in the linked-list.
 * @param list A pointer to the linked-list.
 * @param string The string to find.
 * @return The index of the first occurrence of the string (0-based), or -1 if
 * the string is not found.
 */
ssize_t llist_find(llist_t *list, char * string);

/**
 * @brief Removes the first occurrence of a string from the linked-list.
 * @param list A pointer to the linked-list.
 * @param string The string to remove.
 * @return 0 on success, -1 on failure (e.g., string not found).
 */
int llist_remove(llist_t *list, char * string);

/**
 * @brief Removes the string at a specific index from the linked-list.
 * @param list A pointer to the linked-list.
 * @param index The index of the string to remove (0-based).
 * @return 0 on success, -1 on failure (e.g., invalid index).
 */
int llist_remove_at(llist_t *list, size_t index);

/**
 * @brief Gets the size of the linked-list.
 * @param list A pointer to the linked-list.
 * @return The number of elements in the linked-list, or -1 on error (e.g.,
 * invalid list pointer).
 */
ssize_t llist_size(llist_t *list);

/**
 * @brief Checks if the linked-list is empty.
 * @param list A pointer to the linked-list.
 * @return 1 if the linked-list is empty, 0 if it is not empty, or -1 on error
 * (e.g., invalid list pointer).
 */
ssize_t llist_is_empty(llist_t *list);

void print_list(llist_t *list);

void free_nodes_in_list(llist_t * list);

void count_words(char * string, llist_t * list);

void print_word_count(llist_t * list);

#endif /* LLIST_H */
