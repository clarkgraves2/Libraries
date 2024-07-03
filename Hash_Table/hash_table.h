#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Opaque hash table type.
 */
typedef struct hash_table hash_table_t;
typedef struct node node_t;
typedef uint64_t hash_function (const char *, size_t);

hash_table_t * create_hash_table(uint32_t size, hash_function * hash_func);


#endif
