#include "bst.h"
#include <check.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bst_t *
create_new_tree (void)
{
    bst_t * new_bst = calloc (1, sizeof (bst_t));

    if (NULL == new_bst)
    {
        return NULL;
    }

    new_bst->root = NULL;
    return new_bst;
}

node_t *
create_new_node (int data)
{
    node_t * new_node = calloc (1, sizeof (node_t));

    if (NULL == new_node)
    {
        return NULL;
    }

    new_node->data  = data;
    new_node->left  = NULL;
    new_node->right = NULL;
    return new_node;
}

// Insert a node into the BST recursively
//
node_t *
insert_node_recursive (node_t * root, int data)
{
    if (root == NULL)
    {
        return create_new_node (data);
    }
    if (data <= root->data)
    {
        root->left = insert_node_recursive (root->left, data);
    }
    else
    {
        root->right = insert_node_recursive (root->right, data);
    }
    return root;
}

// Function to insert a node into the BST
//
void
insert_node (bst_t * bst, int data)
{
    if (NULL == bst)
    {
        return;
    }
    bst->root = insert_node_recursive (bst->root, data);
}

// Search for a node
//
node_t *
search (node_t * root, int key)
{
    if (NULL == root || key == root->data)
    {
        return root;
    }

    if (key < root->data)
    {
        return search (root->left, key);
    }
    return search (root->right, key);
}

// Find the minimum value node
//
node_t *
find_min (node_t * node)
{
    node_t * current = node;

    while (current && current->left != NULL)
    {
        current = current->left;
    }
    return current;
}

// Find the maximum value node
//
node_t *
find_max (node_t * node)
{
    node_t * current = node;

    while (current && current->right != NULL)
    {
        current = current->right;
    }
    return current;
}

// Delete a Node from the Tree
//
node_t *
delete_node (node_t * root, int data)
{
    if (NULL == root)
    {
        return root;
    }
    if (data < root->data)
    {
        root->left = delete_node (root->left, data);
    }
    else if (data > root->data)
    {
        root->right = delete_node (root->right, data);
    }
    else
    {
        if (root->left == NULL)
        {
            node_t * temp = root->right;
            free (root);
            return temp;
        }
        else if (root->right == NULL)
        {
            node_t * temp = root->left;
            free (root);
            return temp;
        }

        node_t * temp = find_min (root->right);
        root->data    = temp->data;
        root->right   = delete_node (root->right, temp->data);
    }
    return root;
}

// In Order Traversal of Tree with Print Function
//
void
in_order_traversal (node_t * root)
{
    if (root != NULL)
    {
        in_order_traversal (root->left);
        printf ("%d ", root->data);
        in_order_traversal (root->right);
    }
}

// Calculates the height of the tree
//
int
height (node_t * root)
{
    if (root == NULL)
    {
        return 0;
    }
    else
    {
        int left_height  = height (root->left);
        int right_height = height (root->right);
        if (left_height > right_height)
        {
            return left_height + 1;
        }
        else
        {
            return right_height + 1;
        }
    }
}

// Print nodes at the current level
void
printCurrentLevel (node_t * root, int level)
{
    if (root == NULL)
    {
        return;
    }
    if (level == 1)
    {
        printf ("%d ", root->data);
    }
    else if (level > 1)
    {
        printCurrentLevel (root->left, level - 1);
        printCurrentLevel (root->right, level - 1);
    }
}

// Level-order traversal
void
levelOrderTraversal (node_t * root)
{
    int hgt = height (root);
    for (int idx = 1; idx <= hgt; idx++)
    {
        printCurrentLevel (root, idx);
    }
}