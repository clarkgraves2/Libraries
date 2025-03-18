/** @file hash_table.c
 *
 * @brief Implementation of hash table functions.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #include <stdlib.h>
 #include <string.h>
 #include "hash_table.h"
 
 /*!
  * @brief Create a new hash entry.
  *
  * @param[in] p_table Pointer to the hash table.
  * @param[in] p_key Pointer to the key.
  * @param[in] p_value Pointer to the value.
  *
  * @return Pointer to the newly created hash entry, or NULL if memory allocation failed.
  */
 static hash_entry_t *
 create_entry(const hash_table_t *p_table, const void *p_key, void *p_value)
 {
     hash_entry_t *p_entry = (hash_entry_t *)malloc(sizeof(hash_entry_t));
     
     if (NULL == p_entry)
     {
         return NULL;
     }
     
     /* Copy the key if a key copy function is provided */
     if (NULL != p_table->key_copy)
     {
         p_entry->p_key = p_table->key_copy(p_key);
         if (NULL == p_entry->p_key)
         {
             free(p_entry);
             return NULL;
         }
     }
     else
     {
         /* Use key pointer directly if no copy function */
         p_entry->p_key = (void *)p_key;
     }
     
     p_entry->p_value = p_value;
     p_entry->p_next = NULL;
     
     return p_entry;
 }
 
 /*!
  * @brief Free a hash entry.
  *
  * @param[in] p_table Pointer to the hash table.
  * @param[in] p_entry Pointer to the hash entry to free.
  * @param[in] b_free_value Flag indicating whether to free the value.
  */
 static void
 free_entry(const hash_table_t *p_table, hash_entry_t *p_entry, bool b_free_value)
 {
     if (NULL == p_entry)
     {
         return;
     }
     
     /* Free the key if a key free function is provided */
     if ((NULL != p_table->key_free) && (NULL != p_entry->p_key))
     {
         p_table->key_free(p_entry->p_key);
     }
     
     /* Free the value if requested */
     if ((b_free_value) && (NULL != p_entry->p_value))
     {
         free(p_entry->p_value);
     }
     
     free(p_entry);
 }
 
 /*!
  * @brief Rehash the entries during resize operation.
  *
  * @param[in] pp_old_buckets Pointer to the old buckets array.
  * @param[in] old_capacity Capacity of the old buckets array.
  * @param[in,out] pp_new_buckets Pointer to the new buckets array.
  * @param[in] new_capacity Capacity of the new buckets array.
  * @param[in] hash_function Hash function to use for calculating bucket indices.
  */
 static void
 rehash_entries(hash_entry_t **pp_old_buckets, 
                uint32_t old_capacity,
                hash_entry_t **pp_new_buckets,
                uint32_t new_capacity,
                uint32_t (*hash_function)(const void *, uint32_t))
 {
     /* Rehash all entries into the new buckets */
     for (uint32_t idx = 0; idx < old_capacity; idx++)
     {
         hash_entry_t *p_entry = pp_old_buckets[idx];
         
         while (NULL != p_entry)
         {
             /* Save the next entry before we modify the current one */
             hash_entry_t *p_next = p_entry->p_next;
             
             /* Calculate the new bucket index */
             uint32_t bucket_idx = hash_function(p_entry->p_key, new_capacity);
             
             /* Add the entry to the new bucket */
             p_entry->p_next = pp_new_buckets[bucket_idx];
             pp_new_buckets[bucket_idx] = p_entry;
             
             /* Move to the next entry in the old bucket */
             p_entry = p_next;
         }
     }
 }
 
 /*!
  * @brief Resize the hash table to the new capacity.
  *
  * @param[in,out] p_table Pointer to the hash table.
  * @param[in] new_capacity New capacity for the hash table.
  *
  * @return true if the resize was successful, false otherwise.
  */
 static bool
 resize_table(hash_table_t *p_table, uint32_t new_capacity)
 {
     bool result = false;
     
     if ((NULL == p_table) || (NULL == p_table->pp_buckets) || (new_capacity < p_table->size))
     {
         return result;
     }
     
     /* Allocate a new array of buckets */
     hash_entry_t **pp_new_buckets = (hash_entry_t **)calloc(new_capacity, sizeof(hash_entry_t *));
     
     if (NULL == pp_new_buckets)
     {
         return result;
     }
     
     /* Rehash all entries into the new buckets */
     rehash_entries(p_table->pp_buckets, 
                    p_table->capacity,
                    pp_new_buckets,
                    new_capacity,
                    p_table->hash_function);
     
     /* Free the old buckets array and update the hash table */
     free(p_table->pp_buckets);
     p_table->pp_buckets = pp_new_buckets;
     p_table->capacity = new_capacity;
     result = true;
     
     return result;
 }
 
 /*!
  * @brief Process an existing key during put operation.
  *
  * @param[in,out] p_entry Pointer to the entry with matching key.
  * @param[in] p_value New value to store.
  *
  * @return Pointer to the old value.
  */
 static void *
 process_existing_key(hash_entry_t *p_entry, void *p_value)
 {
     void *p_old_value = p_entry->p_value;
     p_entry->p_value = p_value;
     return p_old_value;
 }
 
 /*!
  * @brief Initialize a hash table.
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
 bool
 hash_table_init(hash_table_t *p_table, 
                uint32_t initial_capacity,
                float load_factor,
                uint32_t (*hash_function)(const void *p_key, uint32_t capacity),
                bool (*key_equals)(const void *p_key1, const void *p_key2),
                void* (*key_copy)(const void *p_key),
                void (*key_free)(void *p_key))
 {
     bool result = false;
     
     if ((NULL == p_table) || (NULL == hash_function) || (NULL == key_equals))
     {
         return result;
     }
     
     /* Validate input parameters */
     if (0 == initial_capacity)
     {
         initial_capacity = 16; /* Default initial capacity */
     }
     
     if (load_factor <= 0.0f || load_factor > 1.0f)
     {
         load_factor = 0.75f; /* Default load factor */
     }
     
     /* Allocate memory for the buckets array */
     p_table->pp_buckets = (hash_entry_t **)calloc(initial_capacity, sizeof(hash_entry_t *));
     
     if (NULL != p_table->pp_buckets)
     {
         /* Initialize the hash table properties */
         p_table->capacity = initial_capacity;
         p_table->size = 0;
         p_table->load_factor = load_factor;
         p_table->hash_function = hash_function;
         p_table->key_equals = key_equals;
         p_table->key_copy = key_copy;
         p_table->key_free = key_free;
         result = true;
     }
     
     return result;
 }
 
 /*!
  * @brief Put a key-value pair in the hash table.
  *
  * @param[in,out] p_table Pointer to the hash table.
  * @param[in] p_key Pointer to the key.
  * @param[in] p_value Pointer to the value.
  *
  * @return Pointer to the old value if key already exists, NULL otherwise.
  */
 void *
 hash_table_put(hash_table_t *p_table, const void *p_key, void *p_value)
 {
     void *p_old_value = NULL;
     
     if ((NULL == p_table) || (NULL == p_key) || (NULL == p_table->pp_buckets))
     {
         return p_old_value;
     }
     
     /* Check if we need to resize the table */
     if (p_table->size >= (uint32_t)(p_table->capacity * p_table->load_factor))
     {
         uint32_t new_capacity = p_table->capacity * 2;
         
         /* Resize operation might fail, but we can still continue with the current capacity */
         resize_table(p_table, new_capacity);
     }
     
     /* Calculate the bucket index */
     uint32_t bucket_idx = p_table->hash_function(p_key, p_table->capacity);
     
     /* Check if the key already exists */
     hash_entry_t *p_entry = p_table->pp_buckets[bucket_idx];
     hash_entry_t *p_prev = NULL;
     
     while (NULL != p_entry)
     {
         if (p_table->key_equals(p_key, p_entry->p_key))
         {
             /* Key already exists, update the value */
             return process_existing_key(p_entry, p_value);
         }
         
         p_prev = p_entry;
         p_entry = p_entry->p_next;
     }
     
     /* Key doesn't exist, create a new entry */
     p_entry = create_entry(p_table, p_key, p_value);
     
     if (NULL == p_entry)
     {
         return p_old_value;
     }
     
     /* Add the entry to the bucket */
     if (NULL == p_prev)
     {
         p_table->pp_buckets[bucket_idx] = p_entry;
     }
     else
     {
         p_prev->p_next = p_entry;
     }
     
     p_table->size++;
     
     return p_old_value;
 }
 
 /*!
  * @brief Get the value associated with a key from the hash table.
  *
  * @param[in] p_table Pointer to the hash table.
  * @param[in] p_key Pointer to the key.
  *
  * @return Pointer to the value associated with the key, or NULL if the key is not found.
  */
 void *
 hash_table_get(const hash_table_t *p_table, const void *p_key)
 {
     void *p_value = NULL;
     
     if ((NULL == p_table) || (NULL == p_key) || (NULL == p_table->pp_buckets))
     {
         return p_value;
     }
     
     /* Calculate the bucket index */
     uint32_t bucket_idx = p_table->hash_function(p_key, p_table->capacity);
     
     /* Search for the key in the bucket */
     hash_entry_t *p_entry = p_table->pp_buckets[bucket_idx];
     
     while (NULL != p_entry)
     {
         if (p_table->key_equals(p_key, p_entry->p_key))
         {
             /* Key found, return the value */
             p_value = p_entry->p_value;
             break;
         }
         
         p_entry = p_entry->p_next;
     }
     
     return p_value;
 }
 
 /*!
  * @brief Remove a key-value pair from the hash table.
  *
  * @param[in,out] p_table Pointer to the hash table.
  * @param[in] p_key Pointer to the key.
  *
  * @return Pointer to the value that was removed, or NULL if the key was not found.
  */
 void *
 hash_table_remove(hash_table_t *p_table, const void *p_key)
 {
     void *p_value = NULL;
     
     if ((NULL == p_table) || (NULL == p_key) || (NULL == p_table->pp_buckets))
     {
         return p_value;
     }
     
     /* Calculate the bucket index */
     uint32_t bucket_idx = p_table->hash_function(p_key, p_table->capacity);
     
     /* Search for the key in the bucket */
     hash_entry_t *p_entry = p_table->pp_buckets[bucket_idx];
     hash_entry_t *p_prev = NULL;
     
     while (NULL != p_entry)
     {
         if (p_table->key_equals(p_key, p_entry->p_key))
         {
             /* Key found, remove the entry */
             if (NULL == p_prev)
             {
                 p_table->pp_buckets[bucket_idx] = p_entry->p_next;
             }
             else
             {
                 p_prev->p_next = p_entry->p_next;
             }
             
             /* Save the value to return */
             p_value = p_entry->p_value;
             
             /* Free the entry but not its value */
             free_entry(p_table, p_entry, false);
             
             p_table->size--;
             break;
         }
         
         p_prev = p_entry;
         p_entry = p_entry->p_next;
     }
     
     return p_value;
 }
 
 /*!
  * @brief Check if the hash table contains a key.
  *
  * @param[in] p_table Pointer to the hash table.
  * @param[in] p_key Pointer to the key.
  *
  * @return true if the hash table contains the key, false otherwise.
  */
 bool
 hash_table_contains_key(const hash_table_t *p_table, const void *p_key)
 {
     if ((NULL == p_table) || (NULL == p_key) || (NULL == p_table->pp_buckets))
     {
         return false;
     }
     
     /* Calculate the bucket index */
     uint32_t bucket_idx = p_table->hash_function(p_key, p_table->capacity);
     
     /* Search for the key in the bucket */
     hash_entry_t *p_entry = p_table->pp_buckets[bucket_idx];
     
     while (NULL != p_entry)
     {
         if (p_table->key_equals(p_key, p_entry->p_key))
         {
             /* Key found */
             return true;
         }
         
         p_entry = p_entry->p_next;
     }
     
     return false;
 }
 
 /*!
  * @brief Get the size of the hash table.
  *
  * @param[in] p_table Pointer to the hash table.
  *
  * @return Number of entries in the hash table.
  */
 uint32_t
 hash_table_size(const hash_table_t *p_table)
 {
     if (NULL == p_table)
     {
         return 0;
     }
     
     return p_table->size;
 }
 
 /*!
  * @brief Check if the hash table is empty.
  *
  * @param[in] p_table Pointer to the hash table.
  *
  * @return true if the hash table is empty, false otherwise.
  */
 bool
 hash_table_is_empty(const hash_table_t *p_table)
 {
     if (NULL == p_table)
     {
         return true;
     }
     
     return (0 == p_table->size);
 }
 
 /*!
  * @brief Clear the hash table, removing all entries.
  *
  * @param[in,out] p_table Pointer to the hash table.
  * @param[in] b_free_values Flag indicating whether to free the values pointed to by each entry.
  */
 void
 hash_table_clear(hash_table_t *p_table, bool b_free_values)
 {
     if ((NULL == p_table) || (NULL == p_table->pp_buckets))
     {
         return;
     }
     
     /* Free all entries in each bucket */
     for (uint32_t idx = 0; idx < p_table->capacity; idx++)
     {
         hash_entry_t *p_entry = p_table->pp_buckets[idx];
         
         while (NULL != p_entry)
         {
             hash_entry_t *p_next = p_entry->p_next;
             
             /* Free the entry and optionally its value */
             free_entry(p_table, p_entry, b_free_values);
             
             p_entry = p_next;
         }
         
         p_table->pp_buckets[idx] = NULL;
     }
     
     p_table->size = 0;
 }
 
 /*!
  * @brief Destroy the hash table, freeing all memory associated with it.
  *
  * @param[in,out] p_table Pointer to the hash table.
  * @param[in] b_free_values Flag indicating whether to free the values pointed to by each entry.
  */
 void
 hash_table_destroy(hash_table_t *p_table, bool b_free_values)
 {
     if (NULL == p_table)
     {
         return;
     }
     
     /* Clear all entries */
     hash_table_clear(p_table, b_free_values);
     
     /* Free the buckets array */
     if (NULL != p_table->pp_buckets)
     {
         free(p_table->pp_buckets);
         p_table->pp_buckets = NULL;
     }
     
     /* Reset all properties */
     p_table->capacity = 0;
     p_table->size = 0;
     p_table->load_factor = 0.0f;
     p_table->hash_function = NULL;
     p_table->key_equals = NULL;
     p_table->key_copy = NULL;
     p_table->key_free = NULL;
 }
 
 /*!
  * @brief Standard hash function for string keys.
  *
  * @param[in] p_key Pointer to the string key (char *).
  * @param[in] capacity Hash table capacity.
  *
  * @return Hash code for the string key.
  */
 uint32_t
 hash_table_string_hash(const void *p_key, uint32_t capacity)
 {
     /* Cast to char* is safe here as we expect a string key for this hash function */
     const char *p_str = (const char *)p_key;
     uint32_t hash = 0;
     
     if (NULL == p_str)
     {
         return 0;
     }
     
     /* Simple djb2 hash algorithm */
     while (*p_str)
     {
         hash = ((hash << 5) + hash) + (uint32_t)(*p_str++);
     }
     
     return hash % capacity;
 }
 
 /*!
  * @brief Standard key equality function for string keys.
  *
  * @param[in] p_key1 Pointer to the first string key (char *).
  * @param[in] p_key2 Pointer to the second string key (char *).
  *
  * @return true if the string keys are equal, false otherwise.
  */
 bool
 hash_table_string_equals(const void *p_key1, const void *p_key2)
 {
     /* Casts to char* are safe as we expect string keys for this equality function */
     const char *p_str1 = (const char *)p_key1;
     const char *p_str2 = (const char *)p_key2;
     
     if ((NULL == p_str1) && (NULL == p_str2))
     {
         return true;
     }
     
     if ((NULL == p_str1) || (NULL == p_str2))
     {
         return false;
     }
     
     return (0 == strcmp(p_str1, p_str2));
 }
 
 /*!
  * @brief Standard key copy function for string keys.
  *
  * @param[in] p_key Pointer to the string key (char *) to copy.
  *
  * @return Pointer to the new copy of the string key.
  */
 void *
 hash_table_string_copy(const void *p_key)
 {
     /* Cast to char* is safe as we expect a string key for this copy function */
     const char *p_str = (const char *)p_key;
     char *p_copy = NULL;
     
     if (NULL == p_str)
     {
         return NULL;
     }
     
     /* Allocate memory for the copy and duplicate the string */
     p_copy = (char *)malloc(strlen(p_str) + 1);
     
     if (NULL != p_copy)
     {
         strcpy(p_copy, p_str);
     }
     
     return p_copy;
 }
 
 /*!
  * @brief Standard key free function for string keys.
  *
  * @param[in] p_key Pointer to the string key (char *) to free.
  */
 void
 hash_table_string_free(void *p_key)
 {
     if (NULL != p_key)
     {
         free(p_key);
     }
 }
 
 /*!
  * @brief Standard hash function for integer keys.
  *
  * @param[in] p_key Pointer to the integer key (int *).
  * @param[in] capacity Hash table capacity.
  *
  * @return Hash code for the integer key.
  */
 uint32_t
 hash_table_int_hash(const void *p_key, uint32_t capacity)
 {
     /* Cast to int* is safe here as we expect an integer key for this hash function */
     const int *p_int = (const int *)p_key;
     
     if (NULL == p_int)
     {
         return 0;
     }
     
     /* Simple integer hash */
     return (uint32_t)(*p_int % capacity);
 }
 
 /*!
  * @brief Standard key equality function for integer keys.
  *
  * @param[in] p_key1 Pointer to the first integer key (int *).
  * @param[in] p_key2 Pointer to the second integer key (int *).
  *
  * @return true if the integer keys are equal, false otherwise.
  */
 bool
 hash_table_int_equals(const void *p_key1, const void *p_key2)
 {
     /* Casts to int* are safe as we expect integer keys for this equality function */
     const int *p_int1 = (const int *)p_key1;
     const int *p_int2 = (const int *)p_key2;
     
     if ((NULL == p_int1) && (NULL == p_int2))
     {
         return true;
     }
     
     if ((NULL == p_int1) || (NULL == p_int2))
     {
         return false;
     }
     
     return (*p_int1 == *p_int2);
 }
 
 /*!
  * @brief Standard key copy function for integer keys.
  *
  * @param[in] p_key Pointer to the integer key (int *) to copy.
  *
  * @return Pointer to the new copy of the integer key.
  */
 void *
 hash_table_int_copy(const void *p_key)
 {
     /* Cast to int* is safe as we expect an integer key for this copy function */
     const int *p_int = (const int *)p_key;
     int *p_copy = NULL;
     
     if (NULL == p_int)
     {
         return NULL;
     }
     
     /* Allocate memory for the copy and duplicate the integer */
     p_copy = (int *)malloc(sizeof(int));
     
     if (NULL != p_copy)
     {
         *p_copy = *p_int;
     }
     
     return p_copy;
 }
 
 /*!
  * @brief Standard key free function for integer keys.
  *
  * @param[in] p_key Pointer to the integer key (int *) to free.
  */
 void
 hash_table_int_free(void *p_key)
 {
     if (NULL != p_key)
     {
         free(p_key);
     }
 }
 /*** end of file ***/