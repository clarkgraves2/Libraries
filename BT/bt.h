#ifndef BT_H
#define BT_H

typedef struct node {
    int data;
    struct node * left;
    struct node * right;
} node_t;

typedef struct bt {
    node_t * root;
} bt_t;

bt_t * create_new_tree (void);

node_t * create_new_node (int data);



#endif
