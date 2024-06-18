
#ifndef BST_H
#define BST_H

typedef struct node {
    int data;
    struct node * left;
    struct node * right;
} node_t;

typedef struct bst {
    node_t * root;
} bst_t;

bst_t * create_new_tree();

node_t * create_new_node(int data);

node_t * insert_node_recursive(node_t *root, int data);

void insert_node (bst_t * bst, int data);

node_t * search (node_t * root, int key);

node_t * find_min(node_t* node);

node_t * find_max(node_t* node);

void print_in_order(node_t *root); 

#endif