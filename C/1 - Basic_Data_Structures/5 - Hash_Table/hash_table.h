/** @file hash_table.h
 *
 * @brief A hash table implementation following BARR-C coding standard.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #ifndef HASH_TABLE_H
 #define HASH_TABLE_H
 
 #include <stdint.h>
 #include <stdbool.h>
 
 /**
  * @brief Structure representing a hash table entry.
  */
 typedef struct hash_entry
 {
     void                *p_key;          /* Pointer to the key */
     void                *p_value;        /* Pointer to the value */
     struct hash_entry   *p_next;         /* Pointer to the next entry in case of collision */
 } hash_entry_t;
 
 /**
  * @brief Structure representing a hash table.
  */
 typedef struct
 {
     hash_entry_t    **pp_buckets;       /* Array of pointers to hash entry buckets */
     uint32_t          size;             /* Number of entries in the hash table */
     uint32_t          capacity;         /* Number of buckets in the hash table */
     float             load_factor;      /* Maximum ratio of size to capacity before resizing */
     
     /** @brief Function pointer to hash function */
     uint32_t        (*hash_function)(const void *p_key, uint32_t capacity);
     
     /** @brief Function pointer to key comparison function */
     bool            (*key_equals)(const void *p_key1, const void *p_key2);
     
     /** @brief Function pointer to key copy function */
     void*           (*key_copy)(const void *p_key);
     
     /** @brief Function pointer to key free function */
     void            (*key_free)(void *p_key);
 } hash_table_t;
 
 /**
  * @brief Initialize a hash table.
  *
  * @details This function initializes a hash table with the specified capacity and load factor.
  *          The user must provide callback functions for key operations:
  *          - hash_function: Computes a hash code for a key
  *          - key_equals: Checks if two keys are equal
  *          - key_copy (optional): Creates a copy of a key
  *          - key_free (optional): Frees resources associated with a key
  *
  *          For simple key types like integers, key_copy and key_free can be NULL.
  *          For complex keys like strings, these functions should be provided to
  *          properly manage memory.
  *
  * @param[in,out] p_table Pointer to the hash table to initialize.
  * @param[in] initial_capacity Initial capacity of the hash table.
  * @param[in] load_factor Maximum ratio of size to capacity before resizing.
  * @param[in] hash_function Function to calculate hash code for a key.
  * @param[in] key_equals Function to check if two keys are equal.
  * @param[in] key_copy Function to create a copy of a key (can be NULL for simple keys).
  * @param[in] key_free Function to free a key (can be NULL for simple keys).
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool hash_table_init(hash_table_t *p_table, 
                     uint32_t initial_capacity,
                     float load_factor,
                     uint32_t (*hash_function)(const void *p_key, uint32_t capacity),
                     bool (*key_equals)(const void *p_key1, const void *p_key2),
                     void* (*key_copy)(const void *p_key),
                     void (*key_free)(void *p_key));
 
 /**
  * @brief Put a key-value pair in the hash table.
  *
  * @param[in,out] p_table Pointer to the hash table.
  * @param[in] p_key Pointer to the key.
  * @param[in] p_value Pointer to the value.
  *
  * @return Pointer to the old value if key already exists, NULL otherwise.
  */
 void *hash_table_put(hash_table_t *p_table, const void *p_key, void *p_value);
 
 /**
  * @brief Get the value associated with a key from the hash table.
  *
  * @param[in] p_table Pointer to the hash table.
  * @param[in] p_key Pointer to the key.
  *
  * @return Pointer to the value associated with the key, or NULL if the key is not found.
  */
 void *hash_table_get(const hash_table_t *p_table, const void *p_key);
 
 /**
  * @brief Remove a key-value pair from the hash table.
  *
  * @param[in,out] p_table Pointer to the hash table.
  * @param[in] p_key Pointer to the key.
  *
  * @return Pointer to the value that was removed, or NULL if the key was not found.
  */
 void *hash_table_remove(hash_table_t *p_table, const void *p_key);
 
 /**
  * @brief Check if the hash table contains a key.
  *
  * @param[in] p_table Pointer to the hash table.
  * @param[in] p_key Pointer to the key.
  *
  * @return true if the hash table contains the key, false otherwise.
  */
 bool hash_table_contains_key(const hash_table_t *p_table, const void *p_key);
 
 /**
  * @brief Get the size of the hash table.
  *
  * @param[in] p_table Pointer to the hash table.
  *
  * @return Number of entries in the hash table.
  */
 uint32_t hash_table_size(const hash_table_t *p_table);
 
 /**
  * @brief Check if the hash table is empty.
  *
  * @param[in] p_table Pointer to the hash table.
  *
  * @return true if the hash table is empty, false otherwise.
  */
 bool hash_table_is_empty(const hash_table_t *p_table);
 
 /**
  * @brief Clear the hash table, removing all entries.
  *
  * @param[in,out] p_table Pointer to the hash table.
  * @param[in] b_free_values Flag indicating whether to free the values pointed to by each entry.
  */
 void hash_table_clear(hash_table_t *p_table, bool b_free_values);
 
 /**
  * @brief Destroy the hash table, freeing all memory associated with it.
  *
  * @param[in,out] p_table Pointer to the hash table.
  * @param[in] b_free_values Flag indicating whether to free the values pointed to by each entry.
  */
 void hash_table_destroy(hash_table_t *p_table, bool b_free_values);
 
 /*
  * NOTE: This basic implementation does not include pre-built callback functions for 
  * specific key types. When using this hash table, you must provide your own callback
  * functions based on your key type:
  */
 
 #endif /* HASH_TABLE_H */
 /*** end of file ***/