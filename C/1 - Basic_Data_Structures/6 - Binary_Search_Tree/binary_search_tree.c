/** @file binary_search_tree.c
 *
 * @brief Implementation of binary search tree functions.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #include <stdlib.h>
 #include "binary_search_tree.h"
 
 /*!
  * @brief Create a new node for the binary search tree.
  *
  * @param[in] p_data Pointer to the data to be stored in the new node.
  *
  * @return Pointer to the newly created node, or NULL if memory allocation failed.
  */
 static bst_node_t *
 create_node(void *p_data)
 {
     bst_node_t *p_node = (bst_node_t *)malloc(sizeof(bst_node_t));
     
     if (NULL == p_node)
     {
         return (NULL);
     }
     
     p_node->p_data = p_data;
     p_node->p_left = NULL;
     p_node->p_right = NULL;
     p_node->p_parent = NULL;
     
     return (p_node);
 }
 
 /*!
  * @brief Find the node containing the specified data.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  * @param[in] p_data Pointer to the data to search for.
  *
  * @return Pointer to the node containing the data, or NULL if not found.
  */
 static bst_node_t *
 find_node(const bst_t *p_tree, const void *p_data)
 {
     if ((NULL == p_tree) || (NULL == p_tree->p_root) || (NULL == p_data))
     {
         return (NULL);
     }
     
     bst_node_t *p_current = p_tree->p_root;
     int32_t compare_result;
     
     while (NULL != p_current)
     {
         compare_result = p_tree->compare_fn(p_data, p_current->p_data);
         
         if (0 == compare_result)
         {
             /* Found the node */
             return (p_current);
         }
         else if (compare_result < 0)
         {
             /* Data is smaller, go left */
             p_current = p_current->p_left;
         }
         else
         {
             /* Data is larger, go right */
             p_current = p_current->p_right;
         }
     }
     
     /* Node not found */
     return (NULL);
 }
 
 /*!
  * @brief Find the node with the minimum value in a subtree.
  *
  * @param[in] p_node Root of the subtree to search.
  *
  * @return Pointer to the node with the minimum value.
  */
 static bst_node_t *
 find_min_node(bst_node_t *p_node)
 {
     if (NULL == p_node)
     {
         return (NULL);
     }
     
     /* Traverse left until reaching the leftmost node */
     bst_node_t *p_current = p_node;
     
     while (NULL != p_current->p_left)
     {
         p_current = p_current->p_left;
     }
     
     return (p_current);
 }
 
 /*!
  * @brief Find the node with the maximum value in a subtree.
  *
  * @param[in] p_node Root of the subtree to search.
  *
  * @return Pointer to the node with the maximum value.
  */
 static bst_node_t *
 find_max_node(bst_node_t *p_node)
 {
     if (NULL == p_node)
     {
         return (NULL);
     }
     
     /* Traverse right until reaching the rightmost node */
     bst_node_t *p_current = p_node;
     
     while (NULL != p_current->p_right)
     {
         p_current = p_current->p_right;
     }
     
     return (p_current);
 }
 
 /*!
  * @brief Helper function for inorder traversal of the binary search tree.
  *
  * @param[in] p_node Current node being visited.
  * @param[in] callback Function called for each node during traversal.
  * @param[in,out] p_context Optional context pointer passed to the callback.
  */
 static void
 inorder_traversal_helper(const bst_node_t *p_node, void (*callback)(void *p_data, void *p_context), void *p_context)
 {
     if (NULL == p_node)
     {
         return;
     }
     
     /* Visit left subtree */
     inorder_traversal_helper(p_node->p_left, callback, p_context);
     
     /* Visit current node */
     callback(p_node->p_data, p_context);
     
     /* Visit right subtree */
     inorder_traversal_helper(p_node->p_right, callback, p_context);
 }
 
 /*!
  * @brief Helper function for preorder traversal of the binary search tree.
  *
  * @param[in] p_node Current node being visited.
  * @param[in] callback Function called for each node during traversal.
  * @param[in,out] p_context Optional context pointer passed to the callback.
  */
 static void
 preorder_traversal_helper(const bst_node_t *p_node, void (*callback)(void *p_data, void *p_context), void *p_context)
 {
     if (NULL == p_node)
     {
         return;
     }
     
     /* Visit current node */
     callback(p_node->p_data, p_context);
     
     /* Visit left subtree */
     preorder_traversal_helper(p_node->p_left, callback, p_context);
     
     /* Visit right subtree */
     preorder_traversal_helper(p_node->p_right, callback, p_context);
 }
 
 /*!
  * @brief Helper function for postorder traversal of the binary search tree.
  *
  * @param[in] p_node Current node being visited.
  * @param[in] callback Function called for each node during traversal.
  * @param[in,out] p_context Optional context pointer passed to the callback.
  */
 static void
 postorder_traversal_helper(const bst_node_t *p_node, void (*callback)(void *p_data, void *p_context), void *p_context)
 {
     if (NULL == p_node)
     {
         return;
     }
     
     /* Visit left subtree */
     postorder_traversal_helper(p_node->p_left, callback, p_context);
     
     /* Visit right subtree */
     postorder_traversal_helper(p_node->p_right, callback, p_context);
     
     /* Visit current node */
     callback(p_node->p_data, p_context);
 }
 
 /*!
  * @brief Recursively free all nodes in a subtree.
  *
  * @param[in] p_node Root of the subtree to free.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each node.
  */
 static void
 free_subtree(bst_node_t *p_node, bool b_free_data)
 {
     if (NULL == p_node)
     {
         return;
     }
     
     /* Recursively free left and right subtrees */
     free_subtree(p_node->p_left, b_free_data);
     free_subtree(p_node->p_right, b_free_data);
     
     /* Free the data if requested and it exists */
     if ((b_free_data) && (NULL != p_node->p_data))
     {
         free(p_node->p_data);
     }
     
     /* Free the node */
     free(p_node);
 }
 
 /*!
  * @brief Remove a node with no children from the binary search tree.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] p_node Node to be removed.
  */
 static void
 remove_leaf_node(bst_t *p_tree, bst_node_t *p_node)
 {
     if (p_node == p_tree->p_root)
     {
         /* Root is the only node */
         p_tree->p_root = NULL;
     }
     else
     {
         /* Update parent pointer */
         if (p_node->p_parent->p_left == p_node)
         {
             p_node->p_parent->p_left = NULL;
         }
         else
         {
             p_node->p_parent->p_right = NULL;
         }
     }
     
     free(p_node);
 }
 
 /*!
  * @brief Remove a node with only a right child from the binary search tree.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] p_node Node to be removed.
  */
 static void
 remove_node_with_right_child(bst_t *p_tree, bst_node_t *p_node)
 {
     if (p_node == p_tree->p_root)
     {
         /* Update root */
         p_tree->p_root = p_node->p_right;
     }
     else
     {
         /* Update parent pointer */
         if (p_node->p_parent->p_left == p_node)
         {
             p_node->p_parent->p_left = p_node->p_right;
         }
         else
         {
             p_node->p_parent->p_right = p_node->p_right;
         }
     }
     
     /* Update child's parent pointer */
     p_node->p_right->p_parent = p_node->p_parent;
     
     free(p_node);
 }
 
 /*!
  * @brief Remove a node with only a left child from the binary search tree.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] p_node Node to be removed.
  */
 static void
 remove_node_with_left_child(bst_t *p_tree, bst_node_t *p_node)
 {
     if (p_node == p_tree->p_root)
     {
         /* Update root */
         p_tree->p_root = p_node->p_left;
     }
     else
     {
         /* Update parent pointer */
         if (p_node->p_parent->p_left == p_node)
         {
             p_node->p_parent->p_left = p_node->p_left;
         }
         else
         {
             p_node->p_parent->p_right = p_node->p_left;
         }
     }
     
     /* Update child's parent pointer */
     p_node->p_left->p_parent = p_node->p_parent;
     
     free(p_node);
 }
 
 /*!
  * @brief Initialize a binary search tree.
  *
  * @param[in,out] p_tree Pointer to the binary search tree to initialize.
  * @param[in] compare_fn Function used to compare nodes.
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool
 bst_init(bst_t *p_tree, bst_compare_func_t compare_fn)
 {
     if ((NULL == p_tree) || (NULL == compare_fn))
     {
         return (false);
     }
     
     p_tree->p_root = NULL;
     p_tree->size = 0;
     p_tree->compare_fn = compare_fn;
     
     return (true);
 }
 
 /*!
  * @brief Insert a new node into the binary search tree.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] p_data Pointer to the data to be stored in the new node.
  *
  * @return true if the node was inserted successfully, false otherwise.
  */
 bool
 bst_insert(bst_t *p_tree, void *p_data)
 {
     if ((NULL == p_tree) || (NULL == p_data))
     {
         return (false);
     }
     
     /* Create a new node */
     bst_node_t *p_new_node = create_node(p_data);
     
     if (NULL == p_new_node)
     {
         return (false);
     }
     
     /* If the tree is empty, make the new node the root */
     if (NULL == p_tree->p_root)
     {
         p_tree->p_root = p_new_node;
         p_tree->size = 1;
         return (true);
     }
     
     /* Find the insertion point */
     bst_node_t *p_current = p_tree->p_root;
     bst_node_t *p_parent = NULL;
     int32_t compare_result;
     bool b_go_left = false;
     
     while (NULL != p_current)
     {
         p_parent = p_current;
         compare_result = p_tree->compare_fn(p_data, p_current->p_data);
         
         if (0 == compare_result)
         {
             /* Duplicate data, free the new node and fail */
             free(p_new_node);
             return (false);
         }
         else if (compare_result < 0)
         {
             /* Data is smaller, go left */
             p_current = p_current->p_left;
             b_go_left = true;
         }
         else
         {
             /* Data is larger, go right */
             p_current = p_current->p_right;
             b_go_left = false;
         }
     }
     
     /* Insert the new node */
     p_new_node->p_parent = p_parent;
     
     if (b_go_left)
     {
         p_parent->p_left = p_new_node;
     }
     else
     {
         p_parent->p_right = p_new_node;
     }
     
     p_tree->size++;
     
     return (true);
 }
 
 /*!
  * @brief Search for a node in the binary search tree.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  * @param[in] p_data Pointer to the data to search for.
  *
  * @return Pointer to the found data, or NULL if not found.
  */
 void *
 bst_search(const bst_t *p_tree, const void *p_data)
 {
     bst_node_t *p_node = find_node(p_tree, p_data);
     
     if (NULL != p_node)
     {
         return (p_node->p_data);
     }
     
     return (NULL);
 }
 
 /*!
  * @brief Remove a node from the binary search tree.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] p_data Pointer to the data to be removed.
  *
  * @return Pointer to the removed data, or NULL if not found.
  */
 void *
 bst_remove(bst_t *p_tree, const void *p_data)
 {
     if ((NULL == p_tree) || (NULL == p_tree->p_root) || (NULL == p_data))
     {
         return (NULL);
     }
     
     /* Find the node to remove */
     bst_node_t *p_node = find_node(p_tree, p_data);
     
     if (NULL == p_node)
     {
         return (NULL);
     }
     
     void *p_removed_data = p_node->p_data;
     
     /* Case 1: Node has no children */
     if ((NULL == p_node->p_left) && (NULL == p_node->p_right))
     {
         remove_leaf_node(p_tree, p_node);
     }
     /* Case 2: Node has only a right child */
     else if (NULL == p_node->p_left)
     {
         remove_node_with_right_child(p_tree, p_node);
     }
     /* Case 3: Node has only a left child */
     else if (NULL == p_node->p_right)
     {
         remove_node_with_left_child(p_tree, p_node);
     }
     /* Case 4: Node has both left and right children */
     else
     {
         /* Find the successor (smallest node in right subtree) */
         bst_node_t *p_successor = find_min_node(p_node->p_right);
         
         /* Copy successor data to current node */
         void *p_successor_data = p_successor->p_data;
         
         /* Recursively remove the successor node */
         bst_remove(p_tree, p_successor->p_data);
         
         /* Replace the node's data with the successor's data */
         p_node->p_data = p_successor_data;
         
         /* We preserve the original data to return it */
     }
     
     p_tree->size--;
     
     return (p_removed_data);
 }
 
 /*!
  * @brief Get the minimum value in the binary search tree.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  *
  * @return Pointer to the minimum data in the tree, or NULL if the tree is empty.
  */
 void *
 bst_find_min(const bst_t *p_tree)
 {
     if ((NULL == p_tree) || (NULL == p_tree->p_root))
     {
         return (NULL);
     }
     
     bst_node_t *p_min_node = find_min_node(p_tree->p_root);
     
     return (p_min_node->p_data);
 }
 
 /*!
  * @brief Get the maximum value in the binary search tree.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  *
  * @return Pointer to the maximum data in the tree, or NULL if the tree is empty.
  */
 void *
 bst_find_max(const bst_t *p_tree)
 {
     if ((NULL == p_tree) || (NULL == p_tree->p_root))
     {
         return (NULL);
     }
     
     bst_node_t *p_max_node = find_max_node(p_tree->p_root);
     
     return (p_max_node->p_data);
 }
 
 /*!
  * @brief Get the size of the binary search tree.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  *
  * @return Number of nodes in the binary search tree.
  */
 uint32_t
 bst_size(const bst_t *p_tree)
 {
     if (NULL == p_tree)
     {
         return (0);
     }
     
     return (p_tree->size);
 }
 
 /*!
  * @brief Check if the binary search tree is empty.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  *
  * @return true if the binary search tree is empty, false otherwise.
  */
 bool
 bst_is_empty(const bst_t *p_tree)
 {
     if (NULL == p_tree)
     {
         return (true);
     }
     
     return (0 == p_tree->size);
 }
 
 /*!
  * @brief Inorder traversal of the binary search tree with callback function.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  * @param[in] callback Function called for each node during traversal.
  * @param[in,out] p_context Optional context pointer passed to the callback.
  */
 void
 bst_inorder_traversal(const bst_t *p_tree, void (*callback)(void *p_data, void *p_context), void *p_context)
 {
     if ((NULL == p_tree) || (NULL == p_tree->p_root) || (NULL == callback))
     {
         return;
     }
     
     inorder_traversal_helper(p_tree->p_root, callback, p_context);
 }
 
 /*!
  * @brief Preorder traversal of the binary search tree with callback function.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  * @param[in] callback Function called for each node during traversal.
  * @param[in,out] p_context Optional context pointer passed to the callback.
  */
 void
 bst_preorder_traversal(const bst_t *p_tree, void (*callback)(void *p_data, void *p_context), void *p_context)
 {
     if ((NULL == p_tree) || (NULL == p_tree->p_root) || (NULL == callback))
     {
         return;
     }
     
     preorder_traversal_helper(p_tree->p_root, callback, p_context);
 }
 
 /*!
  * @brief Postorder traversal of the binary search tree with callback function.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  * @param[in] callback Function called for each node during traversal.
  * @param[in,out] p_context Optional context pointer passed to the callback.
  */
 void
 bst_postorder_traversal(const bst_t *p_tree, void (*callback)(void *p_data, void *p_context), void *p_context)
 {
     if ((NULL == p_tree) || (NULL == p_tree->p_root) || (NULL == callback))
     {
         return;
     }
     
     postorder_traversal_helper(p_tree->p_root, callback, p_context);
 }
 
 /*!
  * @brief Clear the binary search tree, removing all nodes.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each node.
  */
 void
 bst_clear(bst_t *p_tree, bool b_free_data)
 {
     if ((NULL == p_tree) || (NULL == p_tree->p_root))
     {
         return;
     }
     
     /* Free all nodes in the tree */
     free_subtree(p_tree->p_root, b_free_data);
     
     /* Reset the tree structure */
     p_tree->p_root = NULL;
     p_tree->size = 0;
 }
 
 /*!
  * @brief Destroy the binary search tree, freeing all memory associated with it.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each node.
  */
 void
 bst_destroy(bst_t *p_tree, bool b_free_data)
 {
     if (NULL == p_tree)
     {
         return;
     }
     
     /* Clear all nodes and their data if requested */
     bst_clear(p_tree, b_free_data);
 }
 /*** end of file ***/