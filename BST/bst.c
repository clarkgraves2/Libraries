#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>
#include "bst.h"

struct node
{
  int data;  
  node * left;
  node * right;
};

struct bst
{
   node * root;
};

bst_t * create_new_tree(void)
{
  bst_t * new_bst = calloc(1, sizeof(bst);
  bst->root = NULL;
  return new_bst;
}


node_t * create_new_node(node_t * root, int data)
{
  
}
