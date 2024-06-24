#ifndef BT_H
#define BT_H

typedef struct node {
    void * value;
    struct node * left;
    struct node * right;
} node_t;

typedef struct bt {
    node_t * root;
} bt_t;

bt_t * create_new_tree(void);
destroy_tree();
insert_left();
insert_right();
get_root();
get_left_child();
get_right_child();
get_data();
set_data();
is_empty();
size();
height();
traverse_preorder();
traverse_inorder();
traverse_postorder();
traverse_levelorder();
find_node();
delete_node();
copy_tree();
is_balanced();
lowest_common_ancestor();


#endif
