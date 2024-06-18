#include "bst.h"
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

// Function to print the BST in order
//
void
print_in_order (node_t * root)
{
    if (root != NULL)
    {
        print_in_order (root->left);
        printf ("%d ", root->data);
        print_in_order (root->right);
    }
}
