/** @file heap.h
 *
 * @brief A binary heap implementation following BARR-C coding standard.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #ifndef HEAP_H
 #define HEAP_H
 
 #include <stdint.h>
 #include <stdbool.h>
 
 /**
  * @brief Typedef for the comparison function used by the heap.
  * 
  * @param[in] p_data1 Pointer to the first data item to compare.
  * @param[in] p_data2 Pointer to the second data item to compare.
  * 
  * @return Negative value if p_data1 < p_data2, 0 if equal, positive if p_data1 > p_data2.
  *         For a min-heap, if p_data1 < p_data2, it should return a negative value.
  *         For a max-heap, if p_data1 > p_data2, it should return a negative value.
  */
 typedef int32_t (*heap_compare_func_t)(const void *p_data1, const void *p_data2);
 
 /**
  * @brief Structure representing a heap.
  */
 typedef struct
 {
     void               **pp_data;       /* Array of pointers to data elements */
     uint32_t             capacity;      /* Current capacity of the array */
     uint32_t             size;          /* Number of elements in the array */
     float                growth_factor; /* Factor by which to grow the array when needed */
     heap_compare_func_t  compare_fn;    /* Function used to compare elements */
 } heap_t;
 
 /**
  * @brief Initialize a heap.
  *
  * @param[in,out] p_heap Pointer to the heap to initialize.
  * @param[in] initial_capacity Initial capacity of the heap.
  * @param[in] growth_factor Factor by which to grow the heap when needed (1.5f or 2.0f recommended).
  * @param[in] compare_fn Function used to compare elements.
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool heap_init(heap_t *p_heap, uint32_t initial_capacity, float growth_factor, heap_compare_func_t compare_fn);
 
 /**
  * @brief Insert a new element into the heap.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] p_data Pointer to the data to be stored in the heap.
  *
  * @return true if the element was inserted successfully, false otherwise.
  */
 bool heap_insert(heap_t *p_heap, void *p_data);
 
 /**
  * @brief Remove and return the top element from the heap.
  *
  * @param[in,out] p_heap Pointer to the heap.
  *
  * @return Pointer to the data stored at the top of the heap, or NULL if the heap is empty.
  */
 void *heap_extract_top(heap_t *p_heap);
 
 /**
  * @brief Get the top element of the heap without removing it.
  *
  * @param[in] p_heap Pointer to the heap.
  *
  * @return Pointer to the data stored at the top of the heap, or NULL if the heap is empty.
  */
 void *heap_peek_top(const heap_t *p_heap);
 
 /**
  * @brief Get the size of the heap.
  *
  * @param[in] p_heap Pointer to the heap.
  *
  * @return Number of elements in the heap.
  */
 uint32_t heap_size(const heap_t *p_heap);
 
 /**
  * @brief Get the capacity of the heap.
  *
  * @param[in] p_heap Pointer to the heap.
  *
  * @return Current capacity of the heap.
  */
 uint32_t heap_capacity(const heap_t *p_heap);
 
 /**
  * @brief Check if the heap is empty.
  *
  * @param[in] p_heap Pointer to the heap.
  *
  * @return true if the heap is empty, false otherwise.
  */
 bool heap_is_empty(const heap_t *p_heap);
 
 /**
  * @brief Ensure the heap has at least the specified capacity.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] min_capacity Minimum capacity required.
  *
  * @return true if the capacity was ensured successfully, false otherwise.
  */
 bool heap_ensure_capacity(heap_t *p_heap, uint32_t min_capacity);
 
 /**
  * @brief Decrease key of an element in the heap. The new value must be smaller 
  * than the current value for a min-heap, or larger for a max-heap.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] idx Index of the element to update.
  * @param[in] p_new_data Pointer to the new data.
  *
  * @return true if the key was decreased successfully, false otherwise.
  */
 bool heap_decrease_key(heap_t *p_heap, uint32_t idx, void *p_new_data);
 
 /**
  * @brief Remove an element at the specified index from the heap.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] idx Index of the element to remove.
  *
  * @return Pointer to the removed data, or NULL if the index is invalid.
  */
 void *heap_remove_at(heap_t *p_heap, uint32_t idx);
 
 /**
  * @brief Clear the heap, removing all elements.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void heap_clear(heap_t *p_heap, bool b_free_data);
 
 /**
  * @brief Destroy the heap, freeing all memory associated with it.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void heap_destroy(heap_t *p_heap, bool b_free_data);
 
 #endif /* HEAP_H */
 /*** end of file ***/