/** @file linked_list.c
 *
 * @brief Implementation of linked list functions.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #include <stdlib.h>
 #include "linked_list.h"
 
 /*!
  * @brief Initialize a linked list.
  *
  * @param[in,out] p_list Pointer to the linked list to initialize.
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool
 linked_list_init(linked_list_t *p_list)
 {
     if (NULL == p_list)
     {
         return false;
     }
 
     p_list->p_head = NULL;
     p_list->p_tail = NULL;
     p_list->size = 0;
 
     return true;
 }
 
 /*!
  * @brief Create a new node with the given data.
  *
  * @param[in] p_data Pointer to the data to be stored in the new node.
  *
  * @return Pointer to the newly created node, or NULL if memory allocation failed.
  */
 static list_node_t *
 create_node(void *p_data)
 {
     /* Cast from void* to list_node_t* is safe as we're allocating exactly 
      * the size needed for the structure */
     list_node_t *p_node = (list_node_t *)malloc(sizeof(list_node_t));
     
     if (NULL == p_node)
     {
         return NULL;
     }
     
     p_node->p_data = p_data;
     p_node->p_next = NULL;
     
     return p_node;
 }
 
 /*!
  * @brief Add a new node to the end of the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] p_data Pointer to the data to be stored in the new node.
  *
  * @return true if the node was added successfully, false otherwise.
  */
 bool
 linked_list_append(linked_list_t *p_list, void *p_data)
 {
     if (NULL == p_list)
     {
         return false;
     }
     
     list_node_t *p_node = create_node(p_data);
     
     if (NULL == p_node)
     {
         return false;
     }
     
     /* Add the new node to the list */
     if (NULL == p_list->p_head)
     {
         /* List is empty, set both head and tail to the new node */
         p_list->p_head = p_node;
         p_list->p_tail = p_node;
     }
     else
     {
         /* Append to the end of the list */
         p_list->p_tail->p_next = p_node;
         p_list->p_tail = p_node;
     }
     
     p_list->size++;
     
     return true;
 }
 
 /*!
  * @brief Add a new node to the beginning of the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] p_data Pointer to the data to be stored in the new node.
  *
  * @return true if the node was added successfully, false otherwise.
  */
 bool
 linked_list_prepend(linked_list_t *p_list, void *p_data)
 {
     if (NULL == p_list)
     {
         return false;
     }
     
     list_node_t *p_node = create_node(p_data);
     
     if (NULL == p_node)
     {
         return false;
     }
     
     /* Add the new node to the list */
     if (NULL == p_list->p_head)
     {
         /* List is empty, set both head and tail to the new node */
         p_list->p_head = p_node;
         p_list->p_tail = p_node;
     }
     else
     {
         /* Prepend to the beginning of the list */
         p_node->p_next = p_list->p_head;
         p_list->p_head = p_node;
     }
     
     p_list->size++;
     
     return true;
 }
 
 /*!
  * @brief Insert a new node at the specified position in the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] p_data Pointer to the data to be stored in the new node.
  * @param[in] position Position at which to insert the new node (0-based).
  *
  * @return true if the node was inserted successfully, false otherwise.
  */
 bool
 linked_list_insert_at(linked_list_t *p_list, void *p_data, uint32_t position)
 {
     if (NULL == p_list)
     {
         return false;
     }
     
     /* Handle special cases with existing functions */
     if (0 == position)
     {
         return linked_list_prepend(p_list, p_data);
     }
     
     if (p_list->size == position)
     {
         return linked_list_append(p_list, p_data);
     }
     
     /* Check if position is valid */
     if (position > p_list->size)
     {
         return false;
     }
     
     list_node_t *p_node = create_node(p_data);
     
     if (NULL == p_node)
     {
         return false;
     }
     
     /* Find the node before the position */
     list_node_t *p_current = p_list->p_head;
     for (uint32_t idx = 0; idx < position - 1; idx++)
     {
         p_current = p_current->p_next;
     }
     
     /* Insert the new node between p_current and p_current->p_next */
     p_node->p_next = p_current->p_next;
     p_current->p_next = p_node;
     
     p_list->size++;
     
     return true;
 }
 
 /*!
  * @brief Remove the first node from the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  *
  * @return Pointer to the data stored in the removed node, or NULL if the list is empty.
  */
 void *
 linked_list_remove_first(linked_list_t *p_list)
 {
     if ((NULL == p_list) || (NULL == p_list->p_head))
     {
         return NULL;
     }
     
     /* Save the head node and its data */
     list_node_t *p_node = p_list->p_head;
     void *p_data = p_node->p_data;
     
     /* Update the head pointer */
     p_list->p_head = p_node->p_next;
     
     /* If the list is now empty, update the tail pointer */
     if (NULL == p_list->p_head)
     {
         p_list->p_tail = NULL;
     }
     
     /* Free the node and update the size */
     free(p_node);
     p_list->size--;
     
     return p_data;
 }
 
 /*!
  * @brief Remove the last node from the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  *
  * @return Pointer to the data stored in the removed node, or NULL if the list is empty.
  */
 void *
 linked_list_remove_last(linked_list_t *p_list)
 {
     if ((NULL == p_list) || (NULL == p_list->p_tail))
     {
         return NULL;
     }
     
     /* If there's only one node, remove the first node */
     if (p_list->p_head == p_list->p_tail)
     {
         return linked_list_remove_first(p_list);
     }
     
     /* Find the second-to-last node */
     list_node_t *p_current = p_list->p_head;
     while (p_current->p_next != p_list->p_tail)
     {
         p_current = p_current->p_next;
     }
     
     /* Remove the last node */
     list_node_t *p_node = p_list->p_tail;
     void *p_data = p_node->p_data;
     
     /* Update the tail pointer and terminate the list */
     p_current->p_next = NULL;
     p_list->p_tail = p_current;
     
     /* Free the node and update the size */
     free(p_node);
     p_list->size--;
     
     return p_data;
 }
 
 /*!
  * @brief Remove the node at the specified position from the linked list.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] position Position of the node to remove (0-based).
  *
  * @return Pointer to the data stored in the removed node, or NULL if the position is invalid.
  */
 void *
 linked_list_remove_at(linked_list_t *p_list, uint32_t position)
 {
     if ((NULL == p_list) || (NULL == p_list->p_head))
     {
         return NULL;
     }
     
     /* Check if position is valid */
     if (position >= p_list->size)
     {
         return NULL;
     }
     
     /* Handle special cases with existing functions */
     if (0 == position)
     {
         return linked_list_remove_first(p_list);
     }
     
     if (position == p_list->size - 1)
     {
         return linked_list_remove_last(p_list);
     }
     
     /* Find the node before the one to remove */
     list_node_t *p_current = p_list->p_head;
     for (uint32_t idx = 0; idx < position - 1; idx++)
     {
         p_current = p_current->p_next;
     }
     
     /* Remove the node at position */
     list_node_t *p_node = p_current->p_next;
     void *p_data = p_node->p_data;
     
     /* Update the link and free the node */
     p_current->p_next = p_node->p_next;
     
     free(p_node);
     p_list->size--;
     
     return p_data;
 }
 
 /*!
  * @brief Get the data stored at the specified position in the linked list.
  *
  * @param[in] p_list Pointer to the linked list.
  * @param[in] position Position of the node to get (0-based).
  *
  * @return Pointer to the data at the specified position, or NULL if the position is invalid.
  */
 void *
 linked_list_get_at(const linked_list_t *p_list, uint32_t position)
 {
     if ((NULL == p_list) || (NULL == p_list->p_head))
     {
         return NULL;
     }
     
     /* Check if position is valid */
     if (position >= p_list->size)
     {
         return NULL;
     }
     
     /* Find the node at the specified position */
     list_node_t *p_current = p_list->p_head;
     for (uint32_t idx = 0; idx < position; idx++)
     {
         p_current = p_current->p_next;
     }
     
     return p_current->p_data;
 }
 
 /*!
  * @brief Get the size of the linked list.
  *
  * @param[in] p_list Pointer to the linked list.
  *
  * @return Number of nodes in the linked list.
  */
 uint32_t
 linked_list_size(const linked_list_t *p_list)
 {
     if (NULL == p_list)
     {
         return 0;
     }
     
     return p_list->size;
 }
 
 /*!
  * @brief Check if the linked list is empty.
  *
  * @param[in] p_list Pointer to the linked list.
  *
  * @return true if the linked list is empty, false otherwise.
  */
 bool
 linked_list_is_empty(const linked_list_t *p_list)
 {
     if (NULL == p_list)
     {
         return true;
     }
     
     return (0 == p_list->size);
 }
 
 /*!
  * @brief Clear the linked list, removing all nodes.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each node.
  */
 void
 linked_list_clear(linked_list_t *p_list, bool b_free_data)
 {
     if ((NULL == p_list) || (NULL == p_list->p_head))
     {
         return;
     }
     
     /* Traverse the list, freeing each node and optionally its data */
     list_node_t *p_current = p_list->p_head;
     list_node_t *p_next = NULL;
     
     while (NULL != p_current)
     {
         p_next = p_current->p_next;
         
         /* Free the data if requested and it exists */
         if ((b_free_data) && (NULL != p_current->p_data))
         {
             free(p_current->p_data);
         }
         
         /* Free the node */
         free(p_current);
         p_current = p_next;
     }
     
     /* Reset the list structure */
     p_list->p_head = NULL;
     p_list->p_tail = NULL;
     p_list->size = 0;
 }
 
 /*!
  * @brief Destroy the linked list, freeing all memory associated with it.
  *
  * @param[in,out] p_list Pointer to the linked list.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each node.
  */
 void
 linked_list_destroy(linked_list_t *p_list, bool b_free_data)
 {
     if (NULL == p_list)
     {
         return;
     }
     
     /* Clear all nodes and their data if requested */
     linked_list_clear(p_list, b_free_data);
 }
 /*** end of file ***/