#ifndef BT_H
#define BT_H

#include <stdbool.h>
#include <stddef.h>

typedef struct node {
    void * value;
    struct node * left;
    struct node * right;
} node_t;

typedef struct bt {
    node_t * root;
} bt_t;

node_t * create_node (void * value);

void destroy_node (node_t * node);

bt_t * create_new_tree (void);

void destroy_tree (bt_t ** tree);

int insert_left (node_t * parent, void * value);

int insert_right (node_t * parent, void * value);

node_t * get_root (bt_t * tree);

node_t * get_left_child (node_t * node);

node_t * get_right_child (node_t * node);

void * get_data (node_t * node);

int set_data (node_t * node, void * value);

bool is_empty (bt_t * tree);

size_t size_recursive (node_t * node);

size_t height_recursive (node_t * node);

size_t size (bt_t * tree);

size_t height (bt_t * tree);

void traverse_pre_order (node_t * node, void (*visit) (void *));

void traverse_in_order (node_t * node, void (*visit) (void *));

void traverse_post_order (node_t * node, void (*visit) (void *));

void traverse_level_order (bt_t * tree, void (*visit) (void *));

node_t * find_node_recursive (node_t * node, void *   value,
                                  int (*compare) (const void *, const void *));

node_t * find_node (bt_t * tree,
           void * value,
           int (*compare) (const void *, const void *));

node_t * delete_node_recursive (node_t * node,
                    void *   value,
                    int (*compare) (const void *, const void *));

int delete_node (bt_t * tree, void * value, int (*compare) (const void *, 
             const void *));

node_t * copy_node_recursive (node_t * node);

bt_t * copy_tree (bt_t * tree);

int is_balanced_recursive (node_t * node, int * height);

bool is_balanced (bt_t * tree);

node_t * lca_recursive (node_t * node, void *   value1,
            void *   value2, int (*compare) (const void *, const void *));

node_t * lowest_common_ancestor (bt_t * tree, void * value1, void * value2, int (*compare) (const void *, const void *));

#endif
