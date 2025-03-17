/** @file linked_list.h
 *
 * @brief A simple linked list implementation following BARR-C coding standard.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #ifndef LINKED_LIST_H
 #define LINKED_LIST_H
 
 #include <stdint.h>
 #include <stdbool.h>
 
 /**
  * @brief Structure representing a node in the linked list.
  */
 typedef struct list_node
 {
     void *p_data;                 /* Pointer to the data stored in the node */
     struct list_node *p_next;     /* Pointer to the next node in the list */
 } list_node_t;
 
 /**
  * @brief Structure representing a linked list.
  */
 typedef struct
 {
     list_node_t *p_head;         /* Pointer to the first node in the list */
     list_node_t *p_tail;         /* Pointer to the last node in the list */
     uint32_t     size;           /* Number of nodes in the list */
 } linked_list_t;
 
 /**
  * @brief Initialize a linked list.
  *
  * @param[in,out] p_list Pointer to the linked list to initialize.
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool linked_list_init(linked_list_t *p_list);
 
 /**
  * @brief Add a new node to the end of the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] p_data Pointer to the data to be stored in the new node.
  *
  * @return true if the node was added successfully, false otherwise.
  */
 bool linked_list_append(linked_list_t *p_list, void *p_data);
 
 /**
  * @brief Add a new node to the beginning of the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] p_data Pointer to the data to be stored in the new node.
  *
  * @return true if the node was added successfully, false otherwise.
  */
 bool linked_list_prepend(linked_list_t *p_list, void *p_data);
 
 /**
  * @brief Insert a new node at the specified position in the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] p_data Pointer to the data to be stored in the new node.
  * @param[in] position Position at which to insert the new node (0-based).
  *
  * @return true if the node was inserted successfully, false otherwise.
  */
 bool linked_list_insert_at(linked_list_t *p_list, void *p_data, uint32_t position);
 
 /**
  * @brief Remove the first node from the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  *
  * @return Pointer to the data stored in the removed node, or NULL if the list is empty.
  */
 void *linked_list_remove_first(linked_list_t *p_list);
 
 /**
  * @brief Remove the last node from the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  *
  * @return Pointer to the data stored in the removed node, or NULL if the list is empty.
  */
 void *linked_list_remove_last(linked_list_t *p_list);
 
 /**
  * @brief Remove the node at the specified position from the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] position Position of the node to remove (0-based).
  *
  * @return Pointer to the data stored in the removed node, or NULL if the position is invalid.
  */
 void *linked_list_remove_at(linked_list_t *p_list, uint32_t position);
 
 /**
  * @brief Get the data stored at the specified position in the linked list.
  *
  * @param[in] p_list Pointer to the linked list.
  * @param[in] position Position of the node to get (0-based).
  *
  * @return Pointer to the data at the specified position, or NULL if the position is invalid.
  */
 void *linked_list_get_at(const linked_list_t *p_list, uint32_t position);
 
 /**
  * @brief Get the size of the linked list.
  *
  * @param[in] p_list Pointer to the linked list.
  *
  * @return Number of nodes in the linked list.
  */
 uint32_t linked_list_size(const linked_list_t *p_list);
 
 /**
  * @brief Check if the linked list is empty.
  *
  * @param[in] p_list Pointer to the linked list.
  *
  * @return true if the linked list is empty, false otherwise.
  */
 bool linked_list_is_empty(const linked_list_t *p_list);
 
 /**
  * @brief Clear the linked list, removing all nodes.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each node.
  */
 void linked_list_clear(linked_list_t *p_list, bool b_free_data);
 
 /**
  * @brief Destroy the linked list, freeing all memory associated with it.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each node.
  */
 void linked_list_destroy(linked_list_t *p_list, bool b_free_data);
 
 #endif /* LINKED_LIST_H */
 /*** end of file ***/