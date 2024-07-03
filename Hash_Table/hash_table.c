#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hash_table.h"
 
typedef struct node 
{
    char * key;
    void * object;
    struct node * next;
}node;

typedef struct hash_table
{
    uint32_t size;
    hash_function * hash;
    node ** elements;
}hash_table;

static size_t hash_table_index(hash_table_t * hash_table, const char * key)
{
    size_t result = (hash_table->hash(key,strlen(key)) % (hash_table->size));
    return result;
}
 
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

void destroy_hash_table(hash_table_t * table_to_destroy)
{
    free(table_to_destroy->elements);
    free(table_to_destroy);
}

bool hash_table_insert(hash_table_t * hash_table, const char * key, void * object)
{
    if (NULL == key || NULL == object || NULL == hash_table)
    {
        return false;
    }

    size_t index = hash_table_index(hash_table, key);

    if (hash_table_lookup(hash_table, key)!= NULL)
    {
        return false;
    }

    node_t * entry = malloc(sizeof(*entry));
    entry->object = object;
    entry->key = malloc(strlen((key)+1));
    strcpy(entry->key, key);

    entry->next = hash_table->elements[index];
    hash_table->elements[index] = entry;
    return true;
}

void * hash_table_lookup(hash_table_t * table_to_lookup, const char * key)
{
    if (NULL == key || NULL == table_to_lookup)
    {
        return false;
    }
    
    size_t index = hash_table_index(table_to_lookup, key);
    node_t * tmp = table_to_lookup->elements[index];

    while(tmp != NULL && strcmp(tmp->key, key) != 0)
    {
        tmp = tmp->next;
    }
    if (NULL == tmp)
    {
        return NULL;
    }
    return tmp->object;
}

void * hash_table_delete(hash_table_t * hash_table, const char * key)
{
     if (NULL == key || NULL == hash_table)
    {
        return false;
    }
    
    size_t index = hash_table_index(hash_table, key);
    node_t * tmp = hash_table->elements[index];
    node_t * prev = NULL;
    while(tmp != NULL && strcmp(tmp->key, key) != 0)
    {
        prev = tmp;
        tmp = tmp->next;
    }
    if (NULL == tmp)
    {
        return NULL;
    }
    if (NULL == prev)
    {
        hash_table->elements[index] = tmp->next;
    }
    else
    {
        prev->next = tmp->next;
    
    }
    
    void * result = tmp->object;
    free(tmp);
    return result;
}

void print_hash_table(hash_table_t * table_to_print)
{
    printf("Start Table\n");
    for (uint32_t idx = 0; idx < table_to_print->size; idx++)
    {
        if (NULL == table_to_print->elements[idx])
        {
            printf("\t%i\t---\n",idx);
        }
        else
        {
            printf("\t%i\t\n",idx);
            node_t * tmp = table_to_print->elements[idx];
            while(NULL != tmp)
            {
                printf("\"%s\"(%p) - ", tmp->key, tmp->object);
                tmp = tmp->next;
            }
            printf("\n");
        }
    }
    printf("End Table\n");
}