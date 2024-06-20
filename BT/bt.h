#ifndef BT_H
#define BT_H

typedef struct node {
    int data;
    struct node * left;
    struct node * right;
} node_t;

typedef struct bst {
    node_t * root;
} bst_t;

bst_t * create_new_tree();

void * free_tree();

node_t * create_new_node(int data);

void insert_left (bst_t * bst, int data);

void insert_right (bst_t * bst, int data);

node_t * search (node_t * root, int key);

node_t * find_min(node_t* node);

node_t * find_max(node_t* node);

node_t * delete_node (node_t * root, int data);

void in_order_traversal(node_t* root);

void inorderTraversal(node_t *root);

void postorderTraversal(node_t *root);

#endif