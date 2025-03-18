/** @file dynamic_array.c
 *
 * @brief Implementation of dynamic array functions.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #include <stdlib.h>
 #include <string.h>
 #include "dynamic_array.h"
 
 /*!
  * @brief Resize the dynamic array to the new capacity.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] new_capacity New capacity for the array.
  *
  * @return true if the resize was successful, false otherwise.
  */
 static bool
 resize_array(dynamic_array_t * const p_array, uint32_t new_capacity)
 {
     bool result = false;
     
     if ((NULL == p_array) || (NULL == p_array->pp_data) || (new_capacity < p_array->size))
     {
         return result;
     }
     
     /* Allocate memory for the new array */
     void **pp_new_data = (void **)realloc(p_array->pp_data, new_capacity * sizeof(void *));
     
     if (NULL != pp_new_data)
     {
         /* Update the array with the new capacity */
         p_array->pp_data = pp_new_data;
         p_array->capacity = new_capacity;
         result = true;
     }
     
     return result;
 }
 
 /*!
  * @brief Initialize a dynamic array.
  *
  * @param[in,out] p_array Pointer to the dynamic array to initialize.
  * @param[in] initial_capacity Initial capacity of the array.
  * @param[in] growth_factor Factor by which to grow the array when needed.
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool
 dynamic_array_init(dynamic_array_t * const p_array, uint32_t initial_capacity, float growth_factor)
 {
     bool result = false;
     
     if (NULL == p_array)
     {
         return result;
     }
     
     /* Validate input parameters */
     if (0 == initial_capacity)
     {
         initial_capacity = 10; /* Default initial capacity */
     }
     
     if (growth_factor < 1.1f)
     {
         growth_factor = 1.5f; /* Default growth factor */
     }
     
     /* Allocate memory for the data array */
     p_array->pp_data = (void **)malloc(initial_capacity * sizeof(void *));
     
     if (NULL != p_array->pp_data)
     {
         /* Initialize the array properties */
         p_array->capacity = initial_capacity;
         p_array->size = 0;
         p_array->growth_factor = growth_factor;
         result = true;
     }
     
     return result;
 }
 
 /*!
  * @brief Add a new element to the end of the dynamic array.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] p_data Pointer to the data to be stored in the array.
  *
  * @return true if the element was added successfully, false otherwise.
  */
 bool
 dynamic_array_add(dynamic_array_t * const p_array, void * const p_data)
 {
     bool result = false;
     
     if (NULL == p_array)
     {
         return result;
     }
     
     /* Check if we need to resize the array */
     if (p_array->size >= p_array->capacity)
     {
         uint32_t new_capacity = (uint32_t)(p_array->capacity * p_array->growth_factor);
         
         /* Ensure we grow by at least 1 */
         if (new_capacity <= p_array->capacity)
         {
             new_capacity = p_array->capacity + 1;
         }
         
         if (!resize_array(p_array, new_capacity))
         {
             return result;
         }
     }
     
     /* Add the new element to the end of the array */
     p_array->pp_data[p_array->size] = p_data;
     p_array->size++;
     result = true;
     
     return result;
 }
 
 /*!
  * @brief Insert a new element at the specified position in the dynamic array.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] p_data Pointer to the data to be stored in the array.
  * @param[in] position Position at which to insert the new element (0-based).
  *
  * @return true if the element was inserted successfully, false otherwise.
  */
 bool
 dynamic_array_insert_at(dynamic_array_t * const p_array, void * const p_data, uint32_t position)
 {
     bool result = false;
     
     if (NULL == p_array)
     {
         return result;
     }
     
     /* Handle special case: append to the end */
     if (position == p_array->size)
     {
         return dynamic_array_add(p_array, p_data);
     }
     
     /* Check if position is valid */
     if (position > p_array->size)
     {
         return result;
     }
     
     /* Check if we need to resize the array */
     if (p_array->size >= p_array->capacity)
     {
         uint32_t new_capacity = (uint32_t)(p_array->capacity * p_array->growth_factor);
         
         /* Ensure we grow by at least 1 */
         if (new_capacity <= p_array->capacity)
         {
             new_capacity = p_array->capacity + 1;
         }
         
         if (!resize_array(p_array, new_capacity))
         {
             return result;
         }
     }
     
     /* Shift elements to make room for the new element */
     if (position < p_array->size)
     {
         memmove(&(p_array->pp_data[position + 1]), 
                 &(p_array->pp_data[position]), 
                 (p_array->size - position) * sizeof(void *));
     }
     
     /* Insert the new element */
     p_array->pp_data[position] = p_data;
     p_array->size++;
     result = true;
     
     return result;
 }
 
 /*!
  * @brief Remove the element at the specified position from the dynamic array.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] position Position of the element to remove (0-based).
  *
  * @return Pointer to the data stored at the specified position, or NULL if the position is invalid.
  */
 void *
 dynamic_array_remove_at(dynamic_array_t * const p_array, uint32_t position)
 {
     void *p_data = NULL;
     
     if ((NULL == p_array) || (position >= p_array->size))
     {
         return p_data;
     }
     
     /* Save the data to return */
     p_data = p_array->pp_data[position];
     
     /* Shift elements to fill the gap */
     if (position < (p_array->size - 1))
     {
         memmove(&(p_array->pp_data[position]), 
                 &(p_array->pp_data[position + 1]), 
                 (p_array->size - position - 1) * sizeof(void *));
     }
     
     p_array->size--;
     
     return p_data;
 }
 
 /*!
  * @brief Get the element at the specified position in the dynamic array.
  *
  * @param[in] p_array Pointer to the dynamic array.
  * @param[in] position Position of the element to get (0-based).
  *
  * @return Pointer to the data at the specified position, or NULL if the position is invalid.
  */
 void *
 dynamic_array_get_at(dynamic_array_t const * const p_array, uint32_t position)
 {
     void *p_data = NULL;
     
     if ((NULL == p_array) || (position >= p_array->size))
     {
         return p_data;
     }
     
     p_data = p_array->pp_data[position];
     
     return p_data;
 }
 
 /*!
  * @brief Set the element at the specified position in the dynamic array.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] position Position of the element to set (0-based).
  * @param[in] p_data Pointer to the new data to be stored.
  *
  * @return Pointer to the old data at the specified position, or NULL if the position is invalid.
  */
 void *
 dynamic_array_set_at(dynamic_array_t * const p_array, uint32_t position, void * const p_data)
 {
     void *p_old_data = NULL;
     
     if ((NULL == p_array) || (position >= p_array->size))
     {
         return p_old_data;
     }
     
     /* Save the old data to return */
     p_old_data = p_array->pp_data[position];
     
     /* Set the new data */
     p_array->pp_data[position] = p_data;
     
     return p_old_data;
 }
 
 /*!
  * @brief Get the size of the dynamic array.
  *
  * @param[in] p_array Pointer to the dynamic array.
  *
  * @return Number of elements in the dynamic array.
  */
 uint32_t
 dynamic_array_size(dynamic_array_t const * const p_array)
 {
     uint32_t size = 0;
     
     if (NULL != p_array)
     {
         size = p_array->size;
     }
     
     return size;
 }
 
 /*!
  * @brief Get the capacity of the dynamic array.
  *
  * @param[in] p_array Pointer to the dynamic array.
  *
  * @return Current capacity of the dynamic array.
  */
 uint32_t
 dynamic_array_capacity(dynamic_array_t const * const p_array)
 {
     uint32_t capacity = 0;
     
     if (NULL != p_array)
     {
         capacity = p_array->capacity;
     }
     
     return capacity;
 }
 
 /*!
  * @brief Check if the dynamic array is empty.
  *
  * @param[in] p_array Pointer to the dynamic array.
  *
  * @return true if the dynamic array is empty, false otherwise.
  */
 bool
 dynamic_array_is_empty(dynamic_array_t const * const p_array)
 {
     bool b_is_empty = true;
     
     if (NULL != p_array)
     {
         b_is_empty = (0 == p_array->size);
     }
     
     return b_is_empty;
 }
 
 /*!
  * @brief Ensure the dynamic array has at least the specified capacity.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] min_capacity Minimum capacity required.
  *
  * @return true if the capacity was ensured successfully, false otherwise.
  */
 bool
 dynamic_array_ensure_capacity(dynamic_array_t * const p_array, uint32_t min_capacity)
 {
     bool result = false;
     
     if (NULL == p_array)
     {
         return result;
     }
     
     if (p_array->capacity >= min_capacity)
     {
         /* Capacity is already sufficient */
         result = true;
     }
     else
     {
         result = resize_array(p_array, min_capacity);
     }
     
     return result;
 }
 
 /*!
  * @brief Trim the capacity of the dynamic array to match its size.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  *
  * @return true if the array was trimmed successfully, false otherwise.
  */
 bool
 dynamic_array_trim_to_size(dynamic_array_t * const p_array)
 {
     bool result = false;
     
     if ((NULL == p_array) || (NULL == p_array->pp_data))
     {
         return result;
     }
     
     if (0 == p_array->size)
     {
         /* Special case: empty array, free memory but keep a minimal capacity */
         void **pp_new_data = (void **)realloc(p_array->pp_data, sizeof(void *));
         
         if (NULL != pp_new_data)
         {
             p_array->pp_data = pp_new_data;
             p_array->capacity = 1;
             result = true;
         }
     }
     else if (p_array->size < p_array->capacity)
     {
         result = resize_array(p_array, p_array->size);
     }
     else
     {
         /* No need to trim */
         result = true;
     }
     
     return result;
 }
 
 /*!
  * @brief Clear the dynamic array, removing all elements.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void
 dynamic_array_clear(dynamic_array_t * const p_array, bool b_free_data)
 {
     if ((NULL == p_array) || (NULL == p_array->pp_data))
     {
         return;
     }
     
     if (b_free_data)
     {
         /* Free each element's data if requested */
         for (uint32_t idx = 0; idx < p_array->size; idx++)
         {
             if (NULL != p_array->pp_data[idx])
             {
                 free(p_array->pp_data[idx]);
                 p_array->pp_data[idx] = NULL;
             }
         }
     }
     
     /* Reset the size, but keep the capacity */
     p_array->size = 0;
 }
 
 /*!
  * @brief Destroy the dynamic array, freeing all memory associated with it.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void
 dynamic_array_destroy(dynamic_array_t * const p_array, bool b_free_data)
 {
     if (NULL == p_array)
     {
         return;
     }
     
     if (NULL != p_array->pp_data)
     {
         if (b_free_data)
         {
             /* Free each element's data if requested */
             for (uint32_t idx = 0; idx < p_array->size; idx++)
             {
                 if (NULL != p_array->pp_data[idx])
                 {
                     free(p_array->pp_data[idx]);
                 }
             }
         }
         
         /* Free the array itself */
         free(p_array->pp_data);
         p_array->pp_data = NULL;
     }
     
     /* Reset all properties */
     p_array->capacity = 0;
     p_array->size = 0;
     p_array->growth_factor = 0.0f;
 }
 /*** end of file ***/