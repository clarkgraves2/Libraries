#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>
#include "bst.h"

struct node
{
  int data;  
  node_t * left;
  node_t * right;
};

struct bst
{
   node_t * root;
};

bst_t * 
create_new_tree(void)
{
  bst_t * new_bst = calloc(1, sizeof(bst_t));
  new_bst->root = NULL;
  return new_bst;
}

node_t * 
create_new_node(int data) 
{
    node_t* new_node = calloc(1,sizeof(node_t));
    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

node_t * 
insert_node(bst_t * bst, int data) 
{
    if (NULL == bst->root)
    {
      create_new_node(data);
    }
    else if (data <= bst->root->data)
    {
      bst->root->left = insert_node(bst->root-)
    }
    return bst->root;
}