#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>
#include "Queue.h"

struct node
{
  int data;  
  node * left;
  node * right;
};

struct bst
{
    node_t *front;
    node_t *rear;
    ssize_t size;
};
