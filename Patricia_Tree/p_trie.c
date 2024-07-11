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

pat_node_t * create_node(const char *key, pat_node_t *parent, bool str_complete) {
    // Allocate memory for the new node
    pat_node_t *new_node = calloc(1, sizeof(pat_node_t));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory and copy the key
    new_node->key = malloc(strlen(key) + 1);
    if (new_node->key == NULL) {
        fprintf(stderr, "Memory allocation for key failed\n");
        free(new_node);
        exit(EXIT_FAILURE);
    }
    strcpy(new_node->key, key);

    // Allocate or reallocate memory for the parent's children array
    parent->children = realloc(parent->children, (parent->num_of_children + 1) * sizeof(pat_node_t *));
    if (parent->children == NULL) {
        fprintf(stderr, "Memory allocation for children array failed\n");
        free(new_node->key);
        free(new_node);
        exit(EXIT_FAILURE);
    }

    // Add the new node to the parent's children array
    parent->children[parent->num_of_children] = new_node;
    parent->num_of_children++;

    // Initialize the new node
    new_node->parent = parent;
    new_node->children = NULL; 
    new_node->str_complete = str_complete;
    new_node->num_of_children = 0;

    return new_node;
}

// Function to insert a new node into the Patricia Tree
bool insert_node(const char *string, patricia_tree_t *tree) 
{
    if (tree == NULL || string == NULL) {
        return false;
    }

    pat_node_t * current_node = tree->root;
    const char *insert_string = string;
    
    if (NULL == current_node->children)
    {
        create_node(string, current_node, true);
        current_node->num_of_children++;
        return true;
    }


    for (int i = 0; i < current_node->num_of_children; i++) 
    {
        pat_node_t *child = current_node->children[i];

        int diff_index = index_of_difference(insert_string, child->key);

        if (diff_index == -1)
        {
            return false;
        }
        
        if (diff_index == 0)
        {
            continue;;
        }
        else
        {
        int input_string_length = strlen(insert_string);
        int node_key_length = strlen(child->key);

        if (node_key_length < insert_string)
        {
        insert_rem_chars_as_node(diff_index, insert_string, child);
        }
        else
        {
        swap_nodes(insert_string, child, current_node);
        }

        return true;
        }
    }

    return false;
}

int index_of_difference(const char * string, const char * node_key)
{
   int index = 0;

    // Compare each character of both strings until a difference is found
    while (string[index] != '\0' && node_key[index] != '\0') 
    {
        if (string[index] != node_key[index]) {
            return index;
        }
        index++;
    }

    // Check if the strings are of different lengths
    if (string[index] != node_key[index]) {
        return index;
    }

    // Return -1 if no difference is found
    return -1;
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


