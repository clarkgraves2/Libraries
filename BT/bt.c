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

node_t *
insert_node (node_t * root , int value)
{
    if (NULL == root)
    {
        node_t * node_to_insert = calloc(1,sizeof(node_t));
        node_to_insert->data = value;
        node_to_insert->left = NULL;
        node_to_insert->right = NULL;
        return node_to_insert;
    }
    
}