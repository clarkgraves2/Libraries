#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Opaque hash table type.
 */
typedef struct hash_table hash_table_t;
typedef struct ht_node ht_node_t;

typedef uint64_t hash_function (const char *, size_t);

hash_table_t * create_hash_table(uint32_t size, hash_function * hash_func);

void hash_table_destroy(hash_table_t* table); 

bool hash_table_insert(hash_table_t * hash_table, const char * key, void * object);

bool hash_table_lookup(const hash_table_t* table, const char* key, void** value);

bool hash_table_remove(hash_table_t* table, const char* key); 

void print_hash_table(hash_table_t * table_to_print);

bool hash_table_update(hash_table_t * hash_table, const char * key, void * new_object);

bool hash_table_resize(hash_table_t* table, size_t new_size);

void hash_table_clear(hash_table_t * hash_table);

double hash_table_get_load_factor(const hash_table_t* table); 

char** hash_table_get_keys(const hash_table_t* hash_table, size_t* key_count); 

#endif