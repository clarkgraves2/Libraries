
#ifndef BST_H
#define BST_H

typedef struct bst bst_t;
typedef struct node node_t;

bst_t * create_new_tree(void);

node_t * create_new_node(int data);

node_t * insert_node(bst_t * bst, int data); 

#endif