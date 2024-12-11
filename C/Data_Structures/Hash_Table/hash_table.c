#include "hash_table.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct ht_node
{
    char *            key;
    void *            object;
    struct ht_node *  next;
} ht_node_t;

typedef struct hash_table
{
    size_t            size;
    size_t            count;
    double            load_factor;
    double            shrink_factor;
    double            grow_factor;
    double            shrink_threshold;
    size_t            (*hash)(const char* key, size_t key_length);
    ht_node_t **      elements;
} hash_table_t;

#define DEFAULT_INITIAL_SIZE 16
#define DEFAULT_LOAD_FACTOR 0.75
#define DEFAULT_SHRINK_FACTOR 0.25
#define DEFAULT_GROW_FACTOR 2.0
#define DEFAULT_SHRINK_THRESHOLD 0.5

hash_table_t * 
hash_table_create(size_t initial_size, double load_factor, double shrink_factor, 
                  double grow_factor, double shrink_threshold, 
                  size_t (*hash_func)(const char* key, size_t key_length))
{
    if (0 == initial_size)
    {
        initial_size = DEFAULT_INITIAL_SIZE;
    }

    if (load_factor <= 0 || load_factor >= 1) 
    {
        load_factor = DEFAULT_LOAD_FACTOR;
    }

    if (shrink_factor <= 0 || shrink_factor >= load_factor)
    {
        shrink_factor = DEFAULT_SHRINK_FACTOR;
    }

    if (grow_factor <= 1)
    {
        grow_factor = DEFAULT_GROW_FACTOR;
    }

    if (shrink_threshold <= 0 || shrink_threshold >= 1)
    {
        shrink_threshold = DEFAULT_SHRINK_THRESHOLD;
    }

    if (NULL == hash_func)
    {
        return NULL;
    }

    hash_table_t * table = (hash_table_t*)malloc(sizeof(hash_table_t));
    
    if (NULL == table) 
    {
        return NULL;  
    }

    table->elements = (ht_node_t**)calloc(initial_size, sizeof(ht_node_t*));
    
    if (NULL == table->elements) 
    {
        free(table);
        return NULL;  
    }

    table->size = initial_size;
    table->count = 0;
    table->load_factor = load_factor;
    table->shrink_factor = shrink_factor;
    table->grow_factor = grow_factor;
    table->shrink_threshold = shrink_threshold;
    table->hash = hash_func;

    return table;
}

void 
hash_table_destroy(hash_table_t* table) 
{
    if (NULL == table) 
    {
        return;
    }
    
    for (size_t idx = 0; idx < table->size; idx++) 
    {
        ht_node_t* node = table->elements[idx];
        
        while (node != NULL) 
        {
            ht_node_t* next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
    }
    
    free(table->elements);
    free(table);
}

bool 
hash_table_insert(hash_table_t* table, const char* key, void* value) 
{
    if (table == NULL || key == NULL) 
    {
        return false;
    }
    
    void* existing_value;
    if (hash_table_lookup(table, key, &existing_value)) 
    {
        return hash_table_update(table, key, value);
    }
    
    if (!hash_table_check_resize(table)) 
    {
        return false;
    }
    
    size_t key_length = strlen(key);
    size_t index = table->hash(key, key_length) % table->size;
    ht_node_t* new_node = (ht_node_t*)malloc(sizeof(ht_node_t));
    if (new_node == NULL) 
    {
        return false;
    }
    
    new_node->key = strdup(key);
    if (new_node->key == NULL)
    {
        free(new_node);
        return false;
    }
    new_node->object = value;
    new_node->next = table->elements[index];
    table->elements[index] = new_node;
    table->count++;
    
    return true;
}

bool 
hash_table_lookup(const hash_table_t* table, const char* key, void** value) 
{
    if (NULL == table || NULL == key || NULL == value) 
    {
        return false;
    }

    size_t key_length = strlen(key);
    size_t index = table->hash(key, key_length) % table->size;
    ht_node_t* node = table->elements[index];

    while (node != NULL) 
    {
        if (strcmp(node->key, key) == 0) 
        {
            *value = node->object;
            return true;
        }
        node = node->next;
    }

    return false;
}

bool 
hash_table_remove(hash_table_t* table, const char* key) 
{
    if (table == NULL || key == NULL) 
    {
        return false;
    }
    
    void* value;
    if (!hash_table_lookup(table, key, &value)) 
    {
        return false;
    }
    
    size_t key_length = strlen(key);
    size_t index = table->hash(key, key_length) % table->size;
    ht_node_t* node = table->elements[index];
    ht_node_t* prev = NULL;
    
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) 
        {
            if (prev == NULL) 
            {
                table->elements[index] = node->next;
            } 
            else 
            {
                prev->next = node->next;
            }

            free(node->key);
            free(node);
            table->count--;
            hash_table_check_resize(table);
            return true;
        }
        prev = node;
        node = node->next;
    }
    
    return false;
}

void 
print_hash_table(const hash_table_t* table) 
{
    if (table == NULL) 
    {
        printf("Hash table is NULL\n");
        return;
    }

    printf("Hash Table Contents:\n");
    printf("Size: %zu, Count: %zu, Load Factor: %.2f\n", 
           table->size, table->count, (double)table->count / table->size);

    for (size_t i = 0; i < table->size; i++) {
        printf("[%zu]", i);
        ht_node_t* node = table->elements[i];
        if (node == NULL) {
            printf(" ---\n");
        } else {
            while (node != NULL) {
                printf(" -> (%s: %p)", node->key, node->object);
                node = node->next;
            }
            printf("\n");
        }
    }
    printf("End of Hash Table\n");
}

bool 
hash_table_update(hash_table_t* table, const char* key, void* new_value)
{
    if (table == NULL || key == NULL) {
        return false;
    }

    void* current_value;
    if (hash_table_lookup(table, key, &current_value)) {
        size_t key_length = strlen(key);
        size_t index = table->hash(key, key_length) % table->size;
        ht_node_t* node = table->elements[index];
        while (node != NULL) {
            if (strcmp(node->key, key) == 0) {
                node->object = new_value;
                return true;
            }
            node = node->next;
        }
    }

    return hash_table_insert(table, key, new_value);
}

bool 
hash_table_resize(hash_table_t* table, size_t new_size)
{   
    if (new_size < DEFAULT_INITIAL_SIZE || new_size < table->count) 
    {
        new_size = (DEFAULT_INITIAL_SIZE > table->count) ? DEFAULT_INITIAL_SIZE : table->count;
    }

    ht_node_t** new_elements = (ht_node_t**)calloc(new_size, sizeof(ht_node_t*));
    if (NULL == new_elements) 
    {
        return false;
    }
    
    for (size_t idx = 0; idx < table->size; idx++) 
    {
        ht_node_t* node = table->elements[idx];
        while (node != NULL) 
        {
            ht_node_t* next = node->next;
            size_t key_length = strlen(node->key);
            size_t new_index = table->hash(node->key, key_length) % new_size;
            node->next = new_elements[new_index];
            new_elements[new_index] = node;
            node = next;
        }
    }
    
    free(table->elements);
    table->elements = new_elements;
    table->size = new_size;
    
    return true;
}

bool 
hash_table_check_resize(hash_table_t* table)
{
    double current_load = (double)table->count / table->size;

    if (current_load > table->load_factor) 
    {
        return hash_table_resize(table, table->size * table->grow_factor);
    } 
    
    if (current_load < table->shrink_factor && table->size > DEFAULT_INITIAL_SIZE * table->shrink_threshold) 
    {
        return hash_table_resize(table, table->size / table->grow_factor);
    }

    return true; 
}

void 
hash_table_clear(hash_table_t* table)
{
    if (NULL == table) 
    {
        return;
    }
    
    for (size_t idx = 0; idx < table->size; idx++) 
    {
        ht_node_t* node = table->elements[idx];
        while (node != NULL) {
            ht_node_t* next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
        table->elements[idx] = NULL;
    }
    
    table->count = 0;
}

char** 
hash_table_get_keys(const hash_table_t* hash_table, size_t* key_count) 
{
    if (NULL == hash_table || NULL == key_count) 
    {
        return NULL;
    }

    *key_count = 0;
    char** keys = NULL;

    for (size_t idx = 0; idx < hash_table->size; idx++) 
    {
        ht_node_t* tmp = hash_table->elements[idx];

        while (tmp != NULL) 
        {
            char** new_keys = realloc(keys, (*key_count + 1) * sizeof(char*));
         
            if (NULL == new_keys) 
            {
               
                for (size_t jdx = 0; jdx < *key_count; jdx++) 
                {
                    free(keys[jdx]);
                }
                free(keys);
                return NULL;
            }

            keys = new_keys;
            keys[*key_count] = strdup(tmp->key);

            if (NULL == keys[*key_count]) 
            {
                for (size_t kdx = 0; kdx < *key_count; kdx++) 
                {
                    free(keys[kdx]);
                }
                free(keys);
                return NULL;
            }
            (*key_count)++;
            tmp = tmp->next;
        }
    }

    return keys;
}

void 
hash_table_free_keys(char** keys, size_t key_count) 
{
    if (NULL == keys) 
    {
        return;
    }

    for (size_t idx = 0; idx < key_count; idx++) 
    {
        free(keys[idx]);
    }
    free(keys);
}

size_t 
hash_table_get_count(const hash_table_t* table) 
{
    return table ? table->count : 0;
}

double 
hash_table_get_load_factor(const hash_table_t* table) 
{
    return table ? (double)table->count / table->size : 0.0;
}

size_t djb2_hash(const char* key, size_t key_length)
{
    size_t hash = 5381;
    for (size_t i = 0; i < key_length; i++) {
        hash = ((hash << 5) + hash) + key[i];
    }
    return hash;
}