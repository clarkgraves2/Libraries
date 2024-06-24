#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bt.h"

bt_t *
create_new_tree (void)
{
    bt_t * new_bt = calloc (1, sizeof (bt_t));

    if (NULL == new_bt)
    {
        return NULL;
    }

    new_bt->root = NULL;
    return new_bt;
}

void traverse_pre_order(bt_t * binary_tree)
{
    if (NULL == binary_tree || NULL == binary_tree->root)
    {
        return;
    }

    node_t * node_pointer = binary_tree->root;

    while(node_pointer)
    {
        printf("%d ", *(int *)(node_pointer->value));
        
        traverse_pre_order(node_pointer->left);

        traverse_pre_order(node_pointer->right);
    }
}

void traverse_in_order(bt_t * binary_tree)
{
    if (NULL == binary_tree || NULL == binary_tree->root)
    {
        return;
    }

    node_t * node_pointer = binary_tree->root;

    while (node_pointer)
    {
        traverse_in_order(node_pointer->left);

        printf("%d ", *(int *)(node_pointer->value));
        
        traverse_in_order(node_pointer->right);
    }
}

void traverse_post_order(bt_t * binary_tree)
{
    if (NULL == binary_tree || NULL == binary_tree->root)
    {
        return;
    }

    node_t * node_pointer = binary_tree->root;

    while (node_pointer)
    {
        traverse_in_order(node_pointer->left);
        
        traverse_in_order(node_pointer->right);

        printf("%d ", *(int *)(node_pointer->value));
    }
}