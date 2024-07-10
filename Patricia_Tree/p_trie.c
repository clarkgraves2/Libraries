#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p_trie.h"

struct pat_node
{
    char *             key;
    struct pat_node ** children;
    bool               str_complete;
    int                num_of_children;
};

struct patricia_tree
{
    pat_node_t * root;
    int          size;
};

patricia_tree_t *
create_patricia_tree (void)
{
    patricia_tree_t * tree
        = (patricia_tree_t *) calloc (1, sizeof (patricia_tree_t));
    if (tree == NULL)
    {
        return NULL;
    }

    tree->root = (pat_node_t *) calloc (1, sizeof (pat_node_t));
    if (tree->root == NULL)
    {
        free (tree);
        return NULL;
    }

    tree->root->key             = NULL;
    tree->root->children        = NULL;
    tree->root->str_complete    = false;
    tree->root->num_of_children = 0;

    tree->size                  = 0;
    return tree;
}

bool
insert_node (const char * string, patricia_tree_t * tree)
{
    if (tree == NULL || string == NULL)
    {
        return false;
    }

    
    if (trie_contains(tree, string)) 
    {
        return false; 
    }
    pat_node_t * current_node  = tree->root;
    const char * remaining_key = string;

    while (*remaining_key)
    {

        pat_node_t * child
            = find_child_with_prefix (current_node, remaining_key);

        if (child == NULL)
        {
            pat_node_t * new_node
                = (pat_node_t *) calloc (1, sizeof (pat_node_t));

            if (new_node == NULL)
            {
                return false;
            }

            new_node->key             = strdup (remaining_key);
            new_node->children        = NULL;
            new_node->str_complete    = true;
            new_node->num_of_children = 0;

            current_node->num_of_children++;

            current_node->children = (pat_node_t **) realloc (
                current_node->children,
                current_node->num_of_children * sizeof (pat_node_t *));

            if (current_node->children == NULL)
            {
                free (new_node->key);
                free (new_node);
                return false; //
            }
            current_node->children[current_node->num_of_children - 1]
                = new_node;
            tree->size++;
            return true;
        }

        remaining_key += strlen (child->key);
        current_node = child;
    }

    current_node->str_complete = true;
    return true;
}

pat_node_t *
find_child_with_prefix (pat_node_t * node, const char * prefix)
{
    for (int i = 0; i < node->num_of_children; i++)
    {
        if (strncmp (
                node->children[i]->key, prefix, strlen (node->children[i]->key))
            == 0)
        {
            return node->children[i];
        }
    }
    return NULL;
}

void print_patricia_tree(pat_node_t *node, char *prefix) {
    if (node == NULL) {
        return;
    }

    if (node->str_complete) {
        printf("%s%s\n", prefix, node->key);
    }

    for (int i = 0; i < node->num_of_children; i++) {
        char *new_prefix = malloc(strlen(prefix) + strlen(node->key) + 1);
        if (new_prefix == NULL) {
            // Handle memory allocation failure
            fprintf(stderr, "Error: Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        sprintf(new_prefix, "%s%s", prefix, node->key);
        print_patricia_tree(node->children[i], new_prefix);
        free(new_prefix); // Free allocated memory after use
    }
}

bool trie_contains(patricia_tree_t *tree, const char *string) {
    if (tree == NULL || string == NULL) {
        return false;
    }

    pat_node_t *current_node = tree->root;
    const char *remaining_key = string;

    while (*remaining_key) {
        pat_node_t *child = find_child_with_prefix(current_node, remaining_key);
        if (child == NULL || strncmp(child->key, remaining_key, strlen(child->key)) != 0) {
            return false; 
        }

       
        remaining_key += strlen(child->key);
        current_node = child;
    }

    return current_node->str_complete;
}
