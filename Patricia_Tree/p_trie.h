#ifndef PATRICIA_TREE_H
#define PATRICIA_TREE_H

typedef struct patricia_tree patricia_tree_t;
typedef struct pat_node pat_node_t;

patricia_tree_t * create_patricia_tree (void);

bool insert_node (const char * string, patricia_tree_t * tree);

pat_node_t *find_child_with_prefix(pat_node_t *node, const char *prefix);

void print_patricia_tree (pat_node_t * node, char * prefix);

bool trie_contains(patricia_tree_t *tree, const char *string);

#endif /* PATRICIA_TREE_H */
