#include "hash_table.h"
#include <stdlib.h>
#include <string.h>


#define HASH_TABLE_DEFAULT_SIZE       16u
#define HASH_TABLE_DEFAULT_LOAD       0.75f
#define HASH_TABLE_DEFAULT_SHRINK     0.25f
#define HASH_TABLE_DEFAULT_GROW       2.0f
#define HASH_TABLE_DEFAULT_SHRINK_RATE 0.5f

hash_table_error_t 
hash_table_create(hash_table_t **pp_table,
                  uint32_t size, 
                  float load_limit,
                  float shrink_limit, 
                  float grow_rt,
                  float shrink_rt,
                  uint32_t (*hash_fn)(const char *p_key, uint32_t len),
                  char *p_error_msg,
                  size_t error_msg_len)
{
    // Validate input parameters
    if ((NULL == pp_table) || (NULL == hash_fn) || (NULL == p_error_msg))
    {
        if (NULL != p_error_msg)
        {
            strncpy(p_error_msg, "NULL argument provided", error_msg_len - 1);
            p_error_msg[error_msg_len - 1] = '\0';
        }
        return HASH_TABLE_ERROR_NULL_ARG;
    }

    *pp_table = NULL;  // Initialize output parameter

    // Apply default size if none specified
    if (0 == size)
    {
        size = HASH_TABLE_DEFAULT_SIZE;
    }

    // Validate load factor limits
    if ((load_limit <= 0.0f) || (load_limit >= 1.0f))
    {
        load_limit = HASH_TABLE_DEFAULT_LOAD;
    }

    // Validate shrink factor limits 
    if ((shrink_limit <= 0.0f) || (shrink_limit >= load_limit))
    {
        shrink_limit = HASH_TABLE_DEFAULT_SHRINK;
    }

    // Validate growth rate
    if (grow_rt <= 1.0f)
    {
        grow_rt = HASH_TABLE_DEFAULT_GROW;
    }

    // Validate shrink rate
    if ((shrink_rt <= 0.0f) || (shrink_rt >= 1.0f))
    {
        shrink_rt = HASH_TABLE_DEFAULT_SHRINK_RATE;
    }

    // Allocate table structure
    hash_table_t *p_table = calloc(1, sizeof(hash_table_t));
    
    if (NULL == p_table)
    {
        strerror_r(errno, p_error_msg, error_msg_len);
        return HASH_TABLE_ERROR_NO_MEMORY;
    }

    // Allocate bucket array
    p_table->p_elements = calloc(size, sizeof(hash_node_t *));
    
    if (NULL == p_table->p_elements)
    {
        free(p_table);
        strerror_r(errno, p_error_msg, error_msg_len);
        return HASH_TABLE_ERROR_NO_MEMORY;
    }

    // Initialize thread safety lock
    int lock_status = pthread_rwlock_init(&p_table->rw_lock, NULL);
    
    if (0 != lock_status)
    {
        free(p_table->p_elements);
        free(p_table);
        strerror_r(lock_status, p_error_msg, error_msg_len);
        return HASH_TABLE_ERROR_LOCK_FAIL;
    }

    // Initialize table parameters
    p_table->size = size;
    p_table->count = 0;
    p_table->load_max = load_limit;
    p_table->shrink_min = shrink_limit;
    p_table->grow_rate = grow_rt;
    p_table->shrink_rate = shrink_rt;
    p_table->p_hash_fn = hash_fn;

    *pp_table = p_table;
    strncpy(p_error_msg, "Success", error_msg_len - 1);
    p_error_msg[error_msg_len - 1] = '\0';
    
    return HASH_TABLE_SUCCESS;
}

hash_table_error_t 
hash_table_destroy(hash_table_t **pp_table,
                   char *p_error_msg,
                   size_t error_msg_len)
{
    // Validate input parameters
    if ((NULL == pp_table) || (NULL == p_error_msg))
    {
        if (NULL != p_error_msg)
        {
            strncpy(p_error_msg, "NULL argument provided", error_msg_len - 1);
            p_error_msg[error_msg_len - 1] = '\0';
        }
        return HASH_TABLE_ERROR_NULL_ARG;
    }

    hash_table_t *p_table = *pp_table;
    
    if (NULL == p_table)
    {
        strncpy(p_error_msg, "NULL table pointer", error_msg_len - 1);
        p_error_msg[error_msg_len - 1] = '\0';
        return HASH_TABLE_ERROR_NULL_ARG;
    }

    // Acquire write lock before destruction
    int lock_status = pthread_rwlock_wrlock(&p_table->rw_lock);
    if (0 != lock_status)
    {
        strerror_r(lock_status, p_error_msg, error_msg_len);
        return HASH_TABLE_ERROR_LOCK_FAIL;
    }
    
    // Free all nodes in each bucket
    for (size_t idx = 0; idx < p_table->size; idx++) 
    {
        hash_node_t *p_node = p_table->p_elements[idx];
        
        while (NULL != p_node) 
        {
            hash_node_t *p_next = p_node->p_next;
            
            // Free the node's key and the node itself
            free(p_node->p_key);
            free(p_node);
            
            p_node = p_next;
        }
        p_table->p_elements[idx] = NULL;
    }
    
    // Release and destroy the lock
    pthread_rwlock_unlock(&p_table->rw_lock);
    
    lock_status = pthread_rwlock_destroy(&p_table->rw_lock);
    if (0 != lock_status)
    {
        // Note: Memory has been freed but lock destruction failed
        strerror_r(lock_status, p_error_msg, error_msg_len);
        return HASH_TABLE_ERROR_LOCK_FAIL;
    }
    
    // Free the elements array and table structure
    free(p_table->p_elements);
    free(p_table);
    
    // Set the caller's pointer to NULL
    *pp_table = NULL;
    
    strncpy(p_error_msg, "Success", error_msg_len - 1);
    p_error_msg[error_msg_len - 1] = '\0';
    
    return HASH_TABLE_SUCCESS;
}
