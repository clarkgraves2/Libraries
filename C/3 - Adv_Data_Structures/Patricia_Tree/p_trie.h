#ifndef PATRICIA_TREE_H
#define PATRICIA_TREE_H

typedef struct patricia_tree patricia_tree_t;
typedef struct pat_node pat_node_t;

patricia_tree_t * create_patricia_tree (void);

pat_node_t * create_node(char *key, pat_node_t *parent);

bool insert_node(const char * string,  pat_node_t * current_node);

bool insert_node_helper(char * string, pat_node_t * current_node);   

char * remaining_string_from_index(const char* string, size_t diff_index);

char* create_new_string_from_index(char* string, size_t diff_index);

void truncate_string(char* string, int new_length);

void prefix_breakdown(pat_node_t * current_node, size_t diff_index);

size_t index_of_difference(const char *string1, const char *string2);

void print_patricia_tree(pat_node_t *node, int level);

void free_patricia_tree(pat_node_t *node);

char* string_duplicate(const char* str);

#endif /* PATRICIA_TREE_H */
