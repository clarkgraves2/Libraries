/** @file dynamic_array.h
 *
 * @brief A dynamic array implementation following BARR-C coding standard.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #ifndef DYNAMIC_ARRAY_H
 #define DYNAMIC_ARRAY_H
 
 #include <stdint.h>
 #include <stdbool.h>
 
 /**
  * @brief Structure representing a dynamic array.
  */
 typedef struct
 {
     void     **pp_data;       /* Array of pointers to data elements */
     uint32_t   capacity;      /* Current capacity of the array */
     uint32_t   size;          /* Number of elements in the array */
     float      growth_factor; /* Factor by which to grow the array when needed */
 } dynamic_array_t;
 
 /**
  * @brief Initialize a dynamic array.
  *
  * @param[in,out] p_array Pointer to the dynamic array to initialize.
  * @param[in] initial_capacity Initial capacity of the array.
  * @param[in] growth_factor Factor by which to grow the array when needed (1.5f or 2.0f recommended).
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool dynamic_array_init(dynamic_array_t * const p_array, uint32_t initial_capacity, float growth_factor);
 
 /**
  * @brief Add a new element to the end of the dynamic array.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] p_data Pointer to the data to be stored in the array.
  *
  * @return true if the element was added successfully, false otherwise.
  */
 bool dynamic_array_add(dynamic_array_t * const p_array, void * const p_data);
 
 /**
  * @brief Insert a new element at the specified position in the dynamic array.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] p_data Pointer to the data to be stored in the array.
  * @param[in] position Position at which to insert the new element (0-based).
  *
  * @return true if the element was inserted successfully, false otherwise.
  */
 bool dynamic_array_insert_at(dynamic_array_t * const p_array, void * const p_data, uint32_t position);
 
 /**
  * @brief Remove the element at the specified position from the dynamic array.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] position Position of the element to remove (0-based).
  *
  * @return Pointer to the data stored at the specified position, or NULL if the position is invalid.
  */
 void *dynamic_array_remove_at(dynamic_array_t * const p_array, uint32_t position);
 
 /**
  * @brief Get the element at the specified position in the dynamic array.
  *
  * @param[in] p_array Pointer to the dynamic array.
  * @param[in] position Position of the element to get (0-based).
  *
  * @return Pointer to the data at the specified position, or NULL if the position is invalid.
  */
 void *dynamic_array_get_at(dynamic_array_t const * const p_array, uint32_t position);
 
 /**
  * @brief Set the element at the specified position in the dynamic array.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] position Position of the element to set (0-based).
  * @param[in] p_data Pointer to the new data to be stored.
  *
  * @return Pointer to the old data at the specified position, or NULL if the position is invalid.
  */
 void *dynamic_array_set_at(dynamic_array_t * const p_array, uint32_t position, void * const p_data);
 
 /**
  * @brief Get the size of the dynamic array.
  *
  * @param[in] p_array Pointer to the dynamic array.
  *
  * @return Number of elements in the dynamic array.
  */
 uint32_t dynamic_array_size(dynamic_array_t const * const p_array);
 
 /**
  * @brief Get the capacity of the dynamic array.
  *
  * @param[in] p_array Pointer to the dynamic array.
  *
  * @return Current capacity of the dynamic array.
  */
 uint32_t dynamic_array_capacity(dynamic_array_t const * const p_array);
 
 /**
  * @brief Check if the dynamic array is empty.
  *
  * @param[in] p_array Pointer to the dynamic array.
  *
  * @return true if the dynamic array is empty, false otherwise.
  */
 bool dynamic_array_is_empty(dynamic_array_t const * const p_array);
 
 /**
  * @brief Ensure the dynamic array has at least the specified capacity.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] min_capacity Minimum capacity required.
  *
  * @return true if the capacity was ensured successfully, false otherwise.
  */
 bool dynamic_array_ensure_capacity(dynamic_array_t * const p_array, uint32_t min_capacity);
 
 /**
  * @brief Trim the capacity of the dynamic array to match its size.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  *
  * @return true if the array was trimmed successfully, false otherwise.
  */
 bool dynamic_array_trim_to_size(dynamic_array_t * const p_array);
 
 /**
  * @brief Clear the dynamic array, removing all elements.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void dynamic_array_clear(dynamic_array_t * const p_array, bool b_free_data);
 
 /**
  * @brief Destroy the dynamic array, freeing all memory associated with it.
  *
  * @param[in,out] p_array Pointer to the dynamic array.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void dynamic_array_destroy(dynamic_array_t * const p_array, bool b_free_data);
 
 #endif /* DYNAMIC_ARRAY_H */
 /*** end of file ***/