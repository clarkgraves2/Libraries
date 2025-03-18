/** @file binary_search_tree.h
 *
 * @brief A binary search tree implementation following BARR-C coding standard.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #ifndef BINARY_SEARCH_TREE_H
 #define BINARY_SEARCH_TREE_H
 
 #include <stdint.h>
 #include <stdbool.h>
 
 /**
  * @brief Typedef for the comparison function used by the binary search tree.
  * 
  * @param[in] p_data1 Pointer to the first data item to compare.
  * @param[in] p_data2 Pointer to the second data item to compare.
  * 
  * @return Negative value if p_data1 < p_data2, 0 if equal, positive if p_data1 > p_data2.
  */
 typedef int32_t (*bst_compare_func_t)(const void *p_data1, const void *p_data2);
 
 /**
  * @brief Structure representing a node in the binary search tree.
  */
 typedef struct bst_node
 {
     void               *p_data;     /* Pointer to the data stored in the node */
     struct bst_node    *p_left;     /* Pointer to the left child node */
     struct bst_node    *p_right;    /* Pointer to the right child node */
     struct bst_node    *p_parent;   /* Pointer to the parent node */
 } bst_node_t;
 
 /**
  * @brief Structure representing a binary search tree.
  */
 typedef struct
 {
     bst_node_t         *p_root;      /* Pointer to the root node of the tree */
     uint32_t            size;        /* Number of nodes in the tree */
     bst_compare_func_t  compare_fn;  /* Function used to compare nodes */
 } bst_t;
 
 /**
  * @brief Initialize a binary search tree.
  *
  * @param[in,out] p_tree Pointer to the binary search tree to initialize.
  * @param[in] compare_fn Function used to compare nodes.
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool
 bst_init(bst_t *p_tree, bst_compare_func_t compare_fn);
 
 /**
  * @brief Insert a new node into the binary search tree.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] p_data Pointer to the data to be stored in the new node.
  *
  * @return true if the node was inserted successfully, false otherwise.
  */
 bool
 bst_insert(bst_t *p_tree, void *p_data);
 
 /**
  * @brief Search for a node in the binary search tree.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  * @param[in] p_data Pointer to the data to search for.
  *
  * @return Pointer to the found data, or NULL if not found.
  */
 void *
 bst_search(const bst_t *p_tree, const void *p_data);
 
 /**
  * @brief Remove a node from the binary search tree.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] p_data Pointer to the data to be removed.
  *
  * @return Pointer to the removed data, or NULL if not found.
  */
 void *
 bst_remove(bst_t *p_tree, const void *p_data);
 
 /**
  * @brief Get the minimum value in the binary search tree.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  *
  * @return Pointer to the minimum data in the tree, or NULL if the tree is empty.
  */
 void *
 bst_find_min(const bst_t *p_tree);
 
 /**
  * @brief Get the maximum value in the binary search tree.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  *
  * @return Pointer to the maximum data in the tree, or NULL if the tree is empty.
  */
 void *
 bst_find_max(const bst_t *p_tree);
 
 /**
  * @brief Get the size of the binary search tree.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  *
  * @return Number of nodes in the binary search tree.
  */
 uint32_t
 bst_size(const bst_t *p_tree);
 
 /**
  * @brief Check if the binary search tree is empty.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  *
  * @return true if the binary search tree is empty, false otherwise.
  */
 bool
 bst_is_empty(const bst_t *p_tree);
 
 /**
  * @brief Inorder traversal of the binary search tree with callback function.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  * @param[in] callback Function called for each node during traversal.
  * @param[in,out] p_context Optional context pointer passed to the callback.
  */
 void
 bst_inorder_traversal(const bst_t *p_tree, void (*callback)(void *p_data, void *p_context), void *p_context);
 
 /**
  * @brief Preorder traversal of the binary search tree with callback function.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  * @param[in] callback Function called for each node during traversal.
  * @param[in,out] p_context Optional context pointer passed to the callback.
  */
 void
 bst_preorder_traversal(const bst_t *p_tree, void (*callback)(void *p_data, void *p_context), void *p_context);
 
 /**
  * @brief Postorder traversal of the binary search tree with callback function.
  *
  * @param[in] p_tree Pointer to the binary search tree.
  * @param[in] callback Function called for each node during traversal.
  * @param[in,out] p_context Optional context pointer passed to the callback.
  */
 void
 bst_postorder_traversal(const bst_t *p_tree, void (*callback)(void *p_data, void *p_context), void *p_context);
 
 /**
  * @brief Clear the binary search tree, removing all nodes.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each node.
  */
 void
 bst_clear(bst_t *p_tree, bool b_free_data);
 
 /**
  * @brief Destroy the binary search tree, freeing all memory associated with it.
  *
  * @param[in,out] p_tree Pointer to the binary search tree.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each node.
  */
 void
 bst_destroy(bst_t *p_tree, bool b_free_data);
 
 #endif /* BINARY_SEARCH_TREE_H */
 /*** end of file ***/