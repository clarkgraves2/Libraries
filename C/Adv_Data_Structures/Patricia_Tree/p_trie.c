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
patricia_tree_t *
create_patricia_tree(void) {
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

pat_node_t * create_node(char *key, pat_node_t *parent)
{
    pat_node_t *new_node = calloc(1, sizeof(pat_node_t));
    
    if (new_node == NULL) 
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    new_node->key = string_duplicate(key);
    
    if (new_node->key == NULL) 
    {
        fprintf(stderr, "Memory allocation for key failed\n");
        free(new_node);
        return NULL;
    }

    new_node->parent = parent;
    new_node->children = NULL;
    new_node->str_complete = false;
    new_node->num_of_children = 0;

    if (parent != NULL) {
        parent->children = realloc(parent->children, (parent->num_of_children + 1) * sizeof(pat_node_t *));
        if (parent->children == NULL) 
        {
            fprintf(stderr, "Memory allocation for children array failed\n");
            free(new_node->key);
            free(new_node);
            return NULL;
        }

        parent->children[parent->num_of_children] = new_node;
        parent->num_of_children++;
    }

    return new_node;
}

bool insert_node(const char *string, pat_node_t *root) 
{
    char *working_string = string_duplicate(string);
    if (working_string == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return false;
    }

    bool result = insert_node_helper(working_string, root);
    
    free(working_string);
    return result;
}

bool insert_node_helper(char *string, pat_node_t *current_node) 
{
    size_t string_len = strlen(string);
    
    // If the current node is empty, just insert the string here
    if (current_node->key == NULL) {
        current_node->key = string_duplicate(string);
        current_node->str_complete = true;
        return true;
    }

    size_t common_prefix = index_of_difference(string, current_node->key);
    size_t current_len = strlen(current_node->key);

    // If the strings are identical
    if (common_prefix == (size_t)-1) {
        current_node->str_complete = true;
        return true;
    }

    // If we need to split the current node
    if (common_prefix < current_len) {
        pat_node_t *new_child = create_node(current_node->key + common_prefix, NULL);
        new_child->str_complete = current_node->str_complete;
        new_child->children = current_node->children;
        new_child->num_of_children = current_node->num_of_children;

        for (int i = 0; i < new_child->num_of_children; i++) {
            new_child->children[i]->parent = new_child;
        }

        current_node->key[common_prefix] = '\0';
        current_node->children = malloc(sizeof(pat_node_t*) * 2);  // Allocate space for 2 children
        current_node->children[0] = new_child;
        current_node->num_of_children = 1;
        current_node->str_complete = false;

        new_child->parent = current_node;

        // If the new string is longer, add it as a second child
        if (string_len > common_prefix) {
            pat_node_t *new_string_child = create_node(string + common_prefix, current_node);
            new_string_child->str_complete = true;
            current_node->children[1] = new_string_child;
            current_node->num_of_children = 2;
        } else {
            current_node->str_complete = true;
        }

        return true;
    }

    // If the current node is a prefix of the string
    if (common_prefix == current_len) {
        // If there's more to insert
        if (string_len > current_len) {
            // Check if there's a child that matches the next character
            for (int i = 0; i < current_node->num_of_children; i++) {
                if (current_node->children[i]->key[0] == string[current_len]) {
                    return insert_node_helper(string + current_len, current_node->children[i]);
                }
            }
            // If no matching child found, create a new one
            pat_node_t *new_child = create_node(string + current_len, current_node);
            new_child->str_complete = true;
            current_node->children = realloc(current_node->children, (current_node->num_of_children + 1) * sizeof(pat_node_t*));
            current_node->children[current_node->num_of_children] = new_child;
            current_node->num_of_children++;
        } else {
            // The string is exactly the current node
            current_node->str_complete = true;
        }
        return true;
    }

    // This case should never happen if the function is called correctly
    return false;
}

char* string_duplicate(const char* str) {
    size_t len = strlen(str) + 1;
    char* dup = malloc(len);
    if (dup != NULL) {
        memcpy(dup, str, len);
    }
    return dup;
}

char * remaining_string_from_index(const char *string, size_t diff_index) {
    if (string == NULL || diff_index >= strlen(string)) {
        return NULL;
    }

    size_t new_length = strlen(string) - diff_index;
    char *new_string = malloc(new_length + 1);
    if (new_string == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    strcpy(new_string, string + diff_index);
    return new_string;
}

char* create_new_string_from_index(char* string, size_t diff_index) {
    if (string == NULL || diff_index >= strlen(string)) {
        // Return an empty string if the input is invalid
        char * empty_string = malloc(1);
        if (empty_string == NULL) {
            // Handle memory allocation failure
            return NULL;
        }
        empty_string[0] = '\0';
        return empty_string;
    }

    int new_length = strlen(string) - diff_index;
    char* new_string = malloc((new_length + 1) * sizeof(char));
    if (new_string == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    strcpy(new_string, string + diff_index);
    return new_string;
}

void truncate_string(char* string, int new_length) {
    if (string == NULL || new_length < 0) {
        // Invalid input, do nothing
        return;
    }

    int original_length = strlen(string);
    if (new_length >= original_length) {
        // If the new length is greater than or equal to the original length,
        // do nothing since the string is already shorter than or equal to the desired length
        return;
    }

    // Truncate the string by placing a null terminator at the new length
    string[new_length] = '\0';
}

void prefix_breakdown(pat_node_t * current_node, size_t diff_index)
{
    char * substring = create_new_string_from_index(current_node->key, diff_index);
    pat_node_t * new_child = create_node(substring, current_node);
    free(substring);

    // Move all children to the new child
    new_child->children = current_node->children;
    new_child->num_of_children = current_node->num_of_children;
    for (int i = 0; i < new_child->num_of_children; i++) {
        new_child->children[i]->parent = new_child;
    }

    // Update the current node
    current_node->children = malloc(sizeof(pat_node_t*));
    current_node->children[0] = new_child;
    current_node->num_of_children = 1;
    truncate_string(current_node->key, diff_index);
    current_node->str_complete = false;
}

size_t index_of_difference(const char *string1, const char *string2) {
    size_t index = 0;
    while (string1[index] != '\0' && string2[index] != '\0') {
        if (string1[index] != string2[index]) {
            return index;
        }
        index++;
    }

    if (string1[index] != string2[index]) {
        return index;
    }

    return (size_t)-1;  // Special value to indicate strings are identical
}

// Enhanced print function
void print_patricia_tree(struct pat_node *node, int level) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    if (node->key) {
        printf("|-- %s", node->key);
        if (node->str_complete) {
            printf(" [complete]");
        }
        printf("\n");
    }

    for (int i = 0; i < node->num_of_children; i++) {
        print_patricia_tree(node->children[i], level + 1);
    }
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
    // Create a new Patricia tree
    patricia_tree_t *tree = create_patricia_tree();
    if (tree == NULL) {
        fprintf(stderr, "Failed to create Patricia tree\n");
        return 1;
    }

    // Test strings
    const char *test_strings[] = {
        "picket",
        "pickle",
        "predate",
        "preview",
        "pickles",
        "pick",
        "pickling",
        "picture"
    };
    
    int num_strings = sizeof(test_strings) / sizeof(test_strings[0]);

    // Insert strings into the tree
    for (int i = 0; i < num_strings; i++) {
        printf("Inserting: %s\n", test_strings[i]);
        if (!insert_node(test_strings[i], tree->root)) {
            fprintf(stderr, "Failed to insert: %s\n", test_strings[i]);
        }
        printf("\nCurrent tree structure:\n");
        print_patricia_tree(tree->root, 0);
        printf("\n");
    }

    // Print the final tree structure
    printf("\nFinal Patricia Tree Structure:\n");
    print_patricia_tree(tree->root, 0);

    // Free the tree
    free_patricia_tree(tree->root);
    free(tree);

    return 0;
}

