/** @file heap.c
 *
 * @brief Implementation of heap functions.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #include <stdlib.h>
 #include <string.h>
 #include "heap.h"
 
 /*
  * Macros for calculating parent, left child, and right child indices
  * These macros are only used within this file
  */
 #define PARENT(i)      (((i) - 1) / 2)
 #define LEFT_CHILD(i)  (2 * (i) + 1)
 #define RIGHT_CHILD(i) (2 * (i) + 2)
 
 /*!
  * @brief Swap two elements in the heap.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] idx1 Index of the first element.
  * @param[in] idx2 Index of the second element.
  */
 static void
 swap_elements(heap_t *p_heap, uint32_t idx1, uint32_t idx2)
 {
     void *p_temp = p_heap->pp_data[idx1];
     p_heap->pp_data[idx1] = p_heap->pp_data[idx2];
     p_heap->pp_data[idx2] = p_temp;
 }
 
 /*!
  * @brief Heapify the subtree rooted at the given index.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] idx Index of the root of the subtree to heapify.
  */
 static void
 heapify_down(heap_t *p_heap, uint32_t idx)
 {
     uint32_t smallest = idx;
     uint32_t left = LEFT_CHILD(idx);
     uint32_t right = RIGHT_CHILD(idx);
     
     /* Find the smallest among idx, left, and right */
     if ((left < p_heap->size) && 
         (p_heap->compare_fn(p_heap->pp_data[left], p_heap->pp_data[smallest]) < 0))
     {
         smallest = left;
     }
     
     if ((right < p_heap->size) && 
         (p_heap->compare_fn(p_heap->pp_data[right], p_heap->pp_data[smallest]) < 0))
     {
         smallest = right;
     }
     
     /* If smallest is not idx, swap and recursively heapify */
     if (smallest != idx)
     {
         swap_elements(p_heap, idx, smallest);
         heapify_down(p_heap, smallest);
     }
 }
 
 /*!
  * @brief Maintain the heap property after inserting a new element.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] idx Index of the element to heapify up.
  */
 static void
 heapify_up(heap_t *p_heap, uint32_t idx)
 {
     /* If we are at the root or parent is smaller, we're done */
     if ((0 == idx) || 
         (p_heap->compare_fn(p_heap->pp_data[PARENT(idx)], p_heap->pp_data[idx]) <= 0))
     {
         return;
     }
     
     /* Swap with parent and continue upward */
     swap_elements(p_heap, idx, PARENT(idx));
     heapify_up(p_heap, PARENT(idx));
 }
 
 /*!
  * @brief Resize the heap to the new capacity.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] new_capacity New capacity for the heap.
  *
  * @return true if the resize was successful, false otherwise.
  */
 static bool
 resize_heap(heap_t *p_heap, uint32_t new_capacity)
 {
     bool result = false;
     
     if ((NULL == p_heap) || (NULL == p_heap->pp_data) || (new_capacity < p_heap->size))
     {
         return result;
     }
     
     /* Allocate memory for the new array */
     void **pp_new_data = (void **)realloc(p_heap->pp_data, new_capacity * sizeof(void *));
     
     if (NULL != pp_new_data)
     {
         /* Update the heap with the new capacity */
         p_heap->pp_data = pp_new_data;
         p_heap->capacity = new_capacity;
         result = true;
     }
     
     return result;
 }
 
 /*!
  * @brief Initialize a heap.
  *
  * @param[in,out] p_heap Pointer to the heap to initialize.
  * @param[in] initial_capacity Initial capacity of the heap.
  * @param[in] growth_factor Factor by which to grow the heap when needed.
  * @param[in] compare_fn Function used to compare elements.
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool
 heap_init(heap_t *p_heap, uint32_t initial_capacity, float growth_factor, heap_compare_func_t compare_fn)
 {
     bool result = false;
     
     if ((NULL == p_heap) || (NULL == compare_fn))
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
     p_heap->pp_data = (void **)malloc(initial_capacity * sizeof(void *));
     
     if (NULL != p_heap->pp_data)
     {
         /* Initialize the heap properties */
         p_heap->capacity = initial_capacity;
         p_heap->size = 0;
         p_heap->growth_factor = growth_factor;
         p_heap->compare_fn = compare_fn;
         result = true;
     }
     
     return result;
 }
 
 /*!
  * @brief Insert a new element into the heap.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] p_data Pointer to the data to be stored in the heap.
  *
  * @return true if the element was inserted successfully, false otherwise.
  */
 bool
 heap_insert(heap_t *p_heap, void *p_data)
 {
     bool result = false;
     
     if ((NULL == p_heap) || (NULL == p_heap->pp_data) || (NULL == p_data))
     {
         return result;
     }
     
     /* Check if we need to resize the heap */
     if (p_heap->size >= p_heap->capacity)
     {
         uint32_t new_capacity = (uint32_t)(p_heap->capacity * p_heap->growth_factor);
         
         /* Ensure we grow by at least 1 */
         if (new_capacity <= p_heap->capacity)
         {
             new_capacity = p_heap->capacity + 1;
         }
         
         if (!resize_heap(p_heap, new_capacity))
         {
             return result;
         }
     }
     
     /* Add the new element to the end of the heap */
     p_heap->pp_data[p_heap->size] = p_data;
     
     /* Maintain the heap property */
     heapify_up(p_heap, p_heap->size);
     
     p_heap->size++;
     result = true;
     
     return result;
 }
 
 /*!
  * @brief Remove and return the top element from the heap.
  *
  * @param[in,out] p_heap Pointer to the heap.
  *
  * @return Pointer to the data stored at the top of the heap, or NULL if the heap is empty.
  */
 void *
 heap_extract_top(heap_t *p_heap)
 {
     void *p_top_data = NULL;
     
     if ((NULL == p_heap) || (NULL == p_heap->pp_data) || (0 == p_heap->size))
     {
         return p_top_data;
     }
     
     /* Get the data from the top */
     p_top_data = p_heap->pp_data[0];
     
     /* Replace the top with the last element */
     p_heap->pp_data[0] = p_heap->pp_data[p_heap->size - 1];
     p_heap->size--;
     
     /* Maintain the heap property */
     if (p_heap->size > 0)
     {
         heapify_down(p_heap, 0);
     }
     
     return p_top_data;
 }
 
 /*!
  * @brief Get the top element of the heap without removing it.
  *
  * @param[in] p_heap Pointer to the heap.
  *
  * @return Pointer to the data stored at the top of the heap, or NULL if the heap is empty.
  */
 void *
 heap_peek_top(const heap_t *p_heap)
 {
     void *p_top_data = NULL;
     
     if ((NULL == p_heap) || (NULL == p_heap->pp_data) || (0 == p_heap->size))
     {
         return p_top_data;
     }
     
     p_top_data = p_heap->pp_data[0];
     
     return p_top_data;
 }
 
 /*!
  * @brief Get the size of the heap.
  *
  * @param[in] p_heap Pointer to the heap.
  *
  * @return Number of elements in the heap.
  */
 uint32_t
 heap_size(const heap_t *p_heap)
 {
     uint32_t size = 0;
     
     if (NULL != p_heap)
     {
         size = p_heap->size;
     }
     
     return size;
 }
 
 /*!
  * @brief Get the capacity of the heap.
  *
  * @param[in] p_heap Pointer to the heap.
  *
  * @return Current capacity of the heap.
  */
 uint32_t
 heap_capacity(const heap_t *p_heap)
 {
     uint32_t capacity = 0;
     
     if (NULL != p_heap)
     {
         capacity = p_heap->capacity;
     }
     
     return capacity;
 }
 
 /*!
  * @brief Check if the heap is empty.
  *
  * @param[in] p_heap Pointer to the heap.
  *
  * @return true if the heap is empty, false otherwise.
  */
 bool
 heap_is_empty(const heap_t *p_heap)
 {
     bool b_is_empty = true;
     
     if (NULL != p_heap)
     {
         b_is_empty = (0 == p_heap->size);
     }
     
     return b_is_empty;
 }
 
 /*!
  * @brief Ensure the heap has at least the specified capacity.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] min_capacity Minimum capacity required.
  *
  * @return true if the capacity was ensured successfully, false otherwise.
  */
 bool
 heap_ensure_capacity(heap_t *p_heap, uint32_t min_capacity)
 {
     bool result = false;
     
     if (NULL == p_heap)
     {
         return result;
     }
     
     if (p_heap->capacity >= min_capacity)
     {
         /* Capacity is already sufficient */
         result = true;
     }
     else
     {
         result = resize_heap(p_heap, min_capacity);
     }
     
     return result;
 }
 
 /*!
  * @brief Decrease key of an element in the heap. The new value must be smaller 
  * than the current value for a min-heap, or larger for a max-heap.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] idx Index of the element to update.
  * @param[in] p_new_data Pointer to the new data.
  *
  * @return true if the key was decreased successfully, false otherwise.
  */
 bool
 heap_decrease_key(heap_t *p_heap, uint32_t idx, void *p_new_data)
 {
     bool result = false;
     
     if ((NULL == p_heap) || (NULL == p_heap->pp_data) || (NULL == p_new_data) || 
         (idx >= p_heap->size))
     {
         return result;
     }
     
     /* Check if the new key is smaller than the current key (for min-heap) */
     if (p_heap->compare_fn(p_new_data, p_heap->pp_data[idx]) > 0)
     {
         return result;
     }
     
     /* Update the key */
     p_heap->pp_data[idx] = p_new_data;
     
     /* Maintain the heap property */
     heapify_up(p_heap, idx);
     
     result = true;
     
     return result;
 }
 
 /*!
  * @brief Remove an element at the specified index from the heap.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] idx Index of the element to remove.
  *
  * @return Pointer to the removed data, or NULL if the index is invalid.
  */
 void *
 heap_remove_at(heap_t *p_heap, uint32_t idx)
 {
     void *p_removed_data = NULL;
     
     if ((NULL == p_heap) || (NULL == p_heap->pp_data) || (idx >= p_heap->size))
     {
         return p_removed_data;
     }
     
     /* Save the data to be removed */
     p_removed_data = p_heap->pp_data[idx];
     
     /* Replace with the last element */
     p_heap->pp_data[idx] = p_heap->pp_data[p_heap->size - 1];
     p_heap->size--;
     
     /* Maintain the heap property if needed */
     if ((p_heap->size > 0) && (idx < p_heap->size))
     {
         /* Try heapify down first, then up if needed */
         heapify_down(p_heap, idx);
         
         /* If the element didn't move down, it might need to move up */
         if ((idx > 0) && 
             (p_heap->compare_fn(p_heap->pp_data[idx], p_heap->pp_data[PARENT(idx)]) < 0))
         {
             heapify_up(p_heap, idx);
         }
     }
     
     return p_removed_data;
 }
 
 /*!
  * @brief Clear the heap, removing all elements.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void
 heap_clear(heap_t *p_heap, bool b_free_data)
 {
     if ((NULL == p_heap) || (NULL == p_heap->pp_data))
     {
         return;
     }
     
     if (b_free_data)
     {
         /* Free each element's data if requested */
         for (uint32_t idx = 0; idx < p_heap->size; idx++)
         {
             if (NULL != p_heap->pp_data[idx])
             {
                 free(p_heap->pp_data[idx]);
                 p_heap->pp_data[idx] = NULL;
             }
         }
     }
     
     /* Reset the size, but keep the capacity */
     p_heap->size = 0;
 }
 
 /*!
  * @brief Destroy the heap, freeing all memory associated with it.
  *
  * @param[in,out] p_heap Pointer to the heap.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void
 heap_destroy(heap_t *p_heap, bool b_free_data)
 {
     if (NULL == p_heap)
     {
         return;
     }
     
     if (NULL != p_heap->pp_data)
     {
         if (b_free_data)
         {
             /* Free each element's data if requested */
             for (uint32_t idx = 0; idx < p_heap->size; idx++)
             {
                 if (NULL != p_heap->pp_data[idx])
                 {
                     free(p_heap->pp_data[idx]);
                 }
             }
         }
         
         /* Free the array itself */
         free(p_heap->pp_data);
         p_heap->pp_data = NULL;
     }
     
     /* Reset all properties */
     p_heap->capacity = 0;
     p_heap->size = 0;
     p_heap->growth_factor = 0.0f;
     p_heap->compare_fn = NULL;
 }
 /*** end of file ***/