#include "bt.h"
#include <stdbool.h>
#include <stdlib.h>

// Assuming you've defined these structures:
typedef struct node
{
    void *        value;
    struct node * left;
    struct node * right;
} node_t;

typedef struct bt
{
    node_t * root;
} bt_t;

// Helper function to create a new node
static node_t *
create_node (void * value)
{
    node_t * node = malloc (sizeof (node_t));
    if (node)
    {
        node->value = value;
        node->left  = NULL;
        node->right = NULL;
    }
    return node;
}

bt_t *
create_new_tree (void)
{
    bt_t * tree = malloc (sizeof (bt_t));
    if (tree)
    {
        tree->root = NULL;
    }
    return tree;
}

// Helper function for recursively destroying nodes
static void
destroy_node (node_t * node)
{
    if (node)
    {
        destroy_node (node->left);
        destroy_node (node->right);
        free (node);
    }
}

void
destroy_tree (bt_t ** tree)
{
    if (tree && *tree)
    {
        destroy_node ((*tree)->root);
        free (*tree);
        *tree = NULL;
    }
}

int
insert_left (node_t * parent, void * value)
{
    if (!parent)
        return -1;
    if (parent->left)
        return -1; // Left child already exists
    parent->left = create_node (value);
    return parent->left ? 0 : -1;
}

int
insert_right (node_t * parent, void * value)
{
    if (!parent)
        return -1;
    if (parent->right)
        return -1; // Right child already exists
    parent->right = create_node (value);
    return parent->right ? 0 : -1;
}

node_t *
get_root (bt_t * tree)
{
    return tree ? tree->root : NULL;
}

node_t *
get_left_child (node_t * node)
{
    return node ? node->left : NULL;
}

node_t *
get_right_child (node_t * node)
{
    return node ? node->right : NULL;
}

void *
get_data (node_t * node)
{
    return node ? node->value : NULL;
}

int
set_data (node_t * node, void * value)
{
    if (!node)
        return -1;
    node->value = value;
    return 0;
}

bool
is_empty (bt_t * tree)
{
    return tree ? (tree->root == NULL) : true;
}

// Helper function for size
static size_t
size_recursive (node_t * node)
{
    if (!node)
        return 0;
    return 1 + size_recursive (node->left) + size_recursive (node->right);
}

size_t
size (bt_t * tree)
{
    return tree ? size_recursive (tree->root) : 0;
}

// Helper function for height
static size_t
height_recursive (node_t * node)
{
    if (!node)
        return 0;
    size_t left_height  = height_recursive (node->left);
    size_t right_height = height_recursive (node->right);
    return 1 + (left_height > right_height ? left_height : right_height);
}

size_t
height (bt_t * tree)
{
    return tree ? height_recursive (tree->root) : 0;
}

// Traversal functions
void
traverse_pre_order (node_t * node, void (*visit) (void *))
{
    if (node)
    {
        visit (node->value);
        traverse_pre_order (node->left, visit);
        traverse_pre_order (node->right, visit);
    }
}

void
traverse_in_order (node_t * node, void (*visit) (void *))
{
    if (node)
    {
        traverse_in_order (node->left, visit);
        visit (node->value);
        traverse_in_order (node->right, visit);
    }
}

void
traverse_post_order (node_t * node, void (*visit) (void *))
{
    if (node)
    {
        traverse_post_order (node->left, visit);
        traverse_post_order (node->right, visit);
        visit (node->value);
    }
}

// For level-order traversal, we'll need a queue
// Assuming you have a queue implementation, otherwise you'd need to implement
// one
#include "queue.h"

void
traverse_level_order (bt_t * tree, void (*visit) (void *))
{
    if (!tree || !tree->root)
        return;

    queue_t * q = create_queue();
    queue_enqueue (q, tree->root);

    while (!queue_is_empty (q))
    {
        node_t * node;
        queue_dequeue (q, (void **) &node);
        visit (node->value);
        if (node->left)
            queue_enqueue (q, node->left);
        if (node->right)
            queue_enqueue (q, node->right);
    }

    destroy_queue (&q);
}

// Helper function for find_node
static node_t *
find_node_recursive (node_t * node,
                  void *   value,
                  int (*compare) (const void *, const void *))
{
    if (!node)
        return NULL;
    if (compare (node->value, value) == 0)
        return node;
    node_t * left_result = find_node_recursive (node->left, value, compare);
    if (left_result)
        return left_result;
    return find_node_recursive (node->right, value, compare);
}

node_t *
find_node (bt_t * tree,
           void * value,
           int (*compare) (const void *, const void *))
{
    return tree ? find_node_helper (tree->root, value, compare) : NULL;
}

// Helper function for delete_node
static node_t *
delete_node_recursive (node_t * node,
                    void *   value,
                    int (*compare) (const void *, const void *))
{
    if (!node)
        return NULL;

    if (compare (node->value, value) == 0)
    {
        if (!node->left && !node->right)
        {
            free (node);
            return NULL;
        }
        if (!node->left)
        {
            node_t * temp = node->right;
            free (node);
            return temp;
        }
        if (!node->right)
        {
            node_t * temp = node->left;
            free (node);
            return temp;
        }
        // Node has two children, replace with in-order successor
        node_t * temp = node->right;
        while (temp->left)
            temp = temp->left;
        node->value = temp->value;
        node->right = delete_node_helper (node->right, temp->value, compare);
    }
    else
    {
        node->left  = delete_node_helper (node->left, value, compare);
        node->right = delete_node_helper (node->right, value, compare);
    }
    return node;
}

int
delete_node (bt_t * tree,
             void * value,
             int (*compare) (const void *, const void *))
{
    if (!tree)
        return -1;
    tree->root = delete_node_helper (tree->root, value, compare);
    return 0;
}

// Helper function for copy_tree
static node_t *
copy_node_recursive (node_t * node)
{
    if (!node)
        return NULL;
    node_t * new_node = create_node (node->value);
    if (new_node)
    {
        new_node->left  = copy_node_recursive (node->left);
        new_node->right = copy_node_recursive (node->right);
    }
    return new_node;
}

bt_t *
copy_tree (bt_t * tree)
{
    if (!tree)
        return NULL;
    bt_t * new_tree = create_new_tree();
    if (new_tree)
    {
        new_tree->root = copy_node_recursive (tree->root);
    }
    return new_tree;
}

// Helper function for is_balanced
static int
is_balanced_recursive (node_t * node, int * height)
{
    if (!node)
    {
        *height = 0;
        return 1;
    }

    int left_height, right_height;
    if (!is_balanced_recursive (node->left, &left_height))
        return 0;
    if (!is_balanced_recursive (node->right, &right_height))
        return 0;

    *height = 1 + (left_height > right_height ? left_height : right_height);

    return (abs (left_height - right_height) <= 1);
}

bool
is_balanced (bt_t * tree)
{
    if (!tree)
        return true;
    int height;
    return is_balanced_recursive (tree->root, &height);
}

// Helper function for lowest_common_ancestor
static node_t *
lca_recursive (node_t * node,
            void *   value1,
            void *   value2,
            int (*compare) (const void *, const void *))
{
    if (!node)
        return NULL;

    if (compare (node->value, value1) == 0
        || compare (node->value, value2) == 0)
        return node;

    node_t * left_lca  = lca_recursive (node->left, value1, value2, compare);
    node_t * right_lca = lca_recursive (node->right, value1, value2, compare);

    if (left_lca && right_lca)
        return node;

    return left_lca ? left_lca : right_lca;
}

node_t *
lowest_common_ancestor (bt_t * tree,
                        void * value1,
                        void * value2,
                        int (*compare) (const void *, const void *))
{
    if (!tree)
        return NULL;
    return lca_recursive (tree->root, value1, value2, compare);
}