#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p_trie.h"

// Define structures
struct pat_node {
    char *key;
    struct pat_node *parent;
    struct pat_node **children;
    bool str_complete;
    int num_of_children;
};

struct patricia_tree {
    pat_node_t *root;
    int size;
};

// Function to create a new Patricia Tree
patricia_tree_t *create_patricia_tree(void) {
    patricia_tree_t *tree = (patricia_tree_t *)calloc(1, sizeof(patricia_tree_t));
    if (tree == NULL) {
        return NULL;
    }

    tree->root = (pat_node_t *)calloc(1, sizeof(pat_node_t));
    if (tree->root == NULL) {
        free(tree);
        return NULL;
    }

    tree->root->key = NULL;
    tree->root->parent = NULL;
    tree->root->children = NULL;
    tree->root->str_complete = false;
    tree->root->num_of_children = 0;
    tree->size = 0;
    return tree;
}

// Function to insert a new node into the Patricia Tree
bool insert_node(const char *string, patricia_tree_t *tree) {
    if (tree == NULL || string == NULL) {
        return false;
    }

    pat_node_t *current_node = tree->root;
    const char *remaining_key = string;

    while (*remaining_key)
    {

    }
        
}

int index_of_difference(const char * string, const * node_key)
{
    
}

// Function to print the Patricia Tree
void print_patricia_tree(pat_node_t *node, const char *prefix) {
    if (node == NULL) {
        return;
    }

    char *new_prefix = malloc(strlen(prefix) + (node->key ? strlen(node->key) : 0) + 1);
    if (new_prefix == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    sprintf(new_prefix, "%s%s", prefix, node->key ? node->key : "");

    if (node->str_complete) {
        printf("%s\n", new_prefix);
    }

    for (int i = 0; i < node->num_of_children; i++) {
        print_patricia_tree(node->children[i], new_prefix);
    }

    free(new_prefix);
}

void free_patricia_tree(pat_node_t *node) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < node->num_of_children; i++) {
        free_patricia_tree(node->children[i]);
    }

    free(node->key);
    free(node->children);
    free(node);
}

int main() {
    // Create a new Patricia Trie
    patricia_tree_t *trie = create_patricia_tree();
    if (trie == NULL) {
        fprintf(stderr, "Failed to create Patricia Trie\n");
        return 1;
    }

    // Insert some strings into the trie
    insert_node("apple", trie);
    insert_node("banana", trie);
    insert_node("app", trie);
    insert_node("orange", trie);
    insert_node("pickle", trie);
    insert_node("pickles", trie);
    insert_node("pic", trie);
    insert_node("picket", trie);
    insert_node("pick", trie);

    // Print the contents of the trie
    printf("Contents of the Patricia Trie:\n");
    print_patricia_tree(trie->root, "");

    // Example of checking if a string exists in the trie
    const char *search_str = "pic";
    if (trie_contains(trie, search_str)) {
        printf("'%s' exists in the trie.\n", search_str);
    } else {
        printf("'%s' does not exist in the trie.\n", search_str);
    }

    // Clean up: free memory used by the trie
    free_patricia_tree(trie->root);
    free(trie);

    return 0;
}


