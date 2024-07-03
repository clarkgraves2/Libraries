#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"
 
typedef struct node 
{
    char * key;
    size_t keylength;
    void * value;
    struct node * next;
}node;

typedef struct hash_table
{
    uint32_t size;
    hash_function * hash;
    node ** elements;
}hash_table;
 
hash_table_t * create_hash_table(uint32_t size, hash_function * hash_func)
{
    hash_table_t * hash_table = malloc(sizeof(hash_table_t));

    if (NULL == hash_table)
    {
        return NULL;
    }

    hash_table->size = size;
    hash_table->hash = hash_func;
    hash_table->elements = calloc(hash_table->size, sizeof(node_t));

    if (NULL == hash_table->elements)
    {
        free(hash_table);
        hash_table = NULL;
        return NULL;
    }

    return hash_table;
}