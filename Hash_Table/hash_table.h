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

void destroy_hash_table(hash_table_t * table_to_destroy);

bool hash_table_insert(hash_table_t * hash_table, const char * key, void * object);

void * hash_table_lookup(hash_table_t * table_to_lookup, const char * key);

void * hash_table_delete(hash_table_t * hash_table, const char * key);

void print_hash_table(hash_table_t * table_to_print);

bool hash_table_update(hash_table_t * hash_table, const char * key, void * new_object);

bool hash_table_resize(hash_table_t * hash_table, uint32_t new_size);

void hash_table_clear(hash_table_t * hash_table);

float hash_table_get_load_factor(hash_table_t * hash_table);

char ** hash_table_get_keys(hash_table_t * hash_table, int * key_count);



#endif
