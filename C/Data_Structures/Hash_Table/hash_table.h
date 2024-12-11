#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <pthread.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>


#define HASH_TABLE_DEFAULT_SIZE       16u
#define HASH_TABLE_DEFAULT_LOAD       0.75f
#define HASH_TABLE_DEFAULT_SHRINK     0.25f
#define HASH_TABLE_DEFAULT_GROW       2.0f
#define HASH_TABLE_DEFAULT_SHRINK_RATE 0.5f

/*!
 * @brief Error codes specific to hash table operations
 */
typedef enum hash_table_error
{
    HASH_TABLE_SUCCESS = 0,
    HASH_TABLE_ERROR_NULL_ARG,
    HASH_TABLE_ERROR_BAD_PARAMETER,
    HASH_TABLE_ERROR_NO_MEMORY,
    HASH_TABLE_ERROR_LOCK_FAIL,
    HASH_TABLE_ERROR_UNKNOWN
} hash_table_error_t;

/*!
 * @brief Node structure for hash table entries
 */
typedef struct hash_node
{
    char *              p_key;
    void *              p_object;
    struct hash_node *  p_next;
} hash_node_t;

/*!
 * @brief Main hash table structure with thread safety support
 */
typedef struct hash_table
{
    size_t             size;
    size_t             count;
    float              load_max;      
    float              shrink_min;    
    float              grow_rate;     
    float              shrink_rate;   
    uint32_t           (*p_hash_fn)(const char *p_key, uint32_t key_len);
    hash_node_t **     p_elements;    
    pthread_rwlock_t   rw_lock;       
} hash_table_t;

/*!
 * @brief Creates and initializes a new thread-safe hash table
 *
 * @param[out] pp_table Pointer to location to store created hash table
 * @param[in] size Initial size of the hash table
 * @param[in] load_limit Maximum entries before growth (0-1)
 * @param[in] shrink_limit Minimum entries before shrink (0-1)
 * @param[in] grow_rt Growth rate when resizing
 * @param[in] shrink_rt Shrink rate when resizing
 * @param[in] hash_fn Pointer to hash function
 * @param[out] p_error_msg Buffer to store error message
 * @param[in] error_msg_len Length of error message buffer
 * 
 * @return Error code indicating success or failure type
 */
hash_table_error_t 
hash_table_create(hash_table_t **pp_table,
                  uint32_t size, 
                  float load_limit,
                  float shrink_limit, 
                  float grow_rt,
                  float shrink_rt,
                  uint32_t (*hash_fn)(const char *p_key, uint32_t len),
                  char *p_error_msg,
                  size_t error_msg_len);

/*!
 * @brief Destroys a hash table and frees all associated memory
 *
 * @param[in,out] pp_table Pointer to hash table pointer to destroy
 * @param[out] p_error_msg Buffer to store error message
 * @param[in] error_msg_len Length of error message buffer
 * 
 * @return Error code indicating success or failure type
 * 
 * @note Sets the input pointer to NULL after destruction
 */
hash_table_error_t 
hash_table_destroy(hash_table_t **pp_table,
                   char *p_error_msg,
                   size_t error_msg_len);

#endif /* HASH_TABLE_H */