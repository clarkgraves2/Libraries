/** @file stack.c
 *
 * @brief Implementation of stack functions.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #include <stdlib.h>
 #include "stack.h"
 
 /*!
  * @brief Initialize a stack.
  *
  * @param[in,out] p_stack Pointer to the stack to initialize.
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool
 stack_init(stack_t *p_stack)
 {
     if (NULL == p_stack)
     {
         return false;
     }
 
     p_stack->p_top = NULL;
     p_stack->size = 0;
 
     return true;
 }
 
 /*!
  * @brief Push a new element onto the stack.
  *
  * @param[in,out] p_stack Pointer to the stack.
  * @param[in] p_data Pointer to the data to be stored in the new element.
  *
  * @return true if the element was pushed successfully, false otherwise.
  */
 bool
 stack_push(stack_t *p_stack, void *p_data)
 {
     if (NULL == p_stack)
     {
         return false;
     }
     
     /* Create a new stack element */
     stack_element_t *p_element = (stack_element_t *)malloc(sizeof(stack_element_t));
     
     if (NULL == p_element)
     {
         /* Memory allocation failed */
         return false;
     }
     
     /* Initialize and push the new element onto the stack */
     p_element->p_data = p_data;
     p_element->p_next = p_stack->p_top;
     p_stack->p_top = p_element;
     p_stack->size++;
     
     return true;
 }
 
 /*!
  * @brief Pop an element from the stack.
  *
  * @param[in,out] p_stack Pointer to the stack.
  *
  * @return Pointer to the data stored in the popped element, or NULL if the stack is empty.
  */
 void *
 stack_pop(stack_t *p_stack)
 {
     if ((NULL == p_stack) || (NULL == p_stack->p_top))
     {
         return NULL;
     }
     
     /* Get the top element and its data */
     stack_element_t *p_element = p_stack->p_top;
     void *p_data = p_element->p_data;
     
     /* Update the stack top pointer */
     p_stack->p_top = p_element->p_next;
     p_stack->size--;
     
     /* Free the popped element */
     free(p_element);
     
     return p_data;
 }
 
 /*!
  * @brief Peek at the top element of the stack without removing it.
  *
  * @param[in] p_stack Pointer to the stack.
  *
  * @return Pointer to the data stored in the top element, or NULL if the stack is empty.
  */
 void *
 stack_peek(const stack_t *p_stack)
 {
     if ((NULL == p_stack) || (NULL == p_stack->p_top))
     {
         return NULL;
     }
     
     /* Return the data from the top element without removing it */
     return p_stack->p_top->p_data;
 }
 
 /*!
  * @brief Get the size of the stack.
  *
  * @param[in] p_stack Pointer to the stack.
  *
  * @return Number of elements in the stack.
  */
 uint32_t
 stack_size(const stack_t *p_stack)
 {
     if (NULL == p_stack)
     {
         return 0;
     }
     
     return p_stack->size;
 }
 
 /*!
  * @brief Check if the stack is empty.
  *
  * @param[in] p_stack Pointer to the stack.
  *
  * @return true if the stack is empty, false otherwise.
  */
 bool
 stack_is_empty(const stack_t *p_stack)
 {
     if (NULL == p_stack)
     {
         return true;
     }
     
     return (0 == p_stack->size);
 }
 
 /*!
  * @brief Clear the stack, removing all elements.
  *
  * @param[in,out] p_stack Pointer to the stack.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void
 stack_clear(stack_t *p_stack, bool b_free_data)
 {
     if ((NULL == p_stack) || (NULL == p_stack->p_top))
     {
         return;
     }
     
     /* Traverse the stack, freeing each element and optionally its data */
     stack_element_t *p_current = p_stack->p_top;
     stack_element_t *p_next = NULL;
     
     while (NULL != p_current)
     {
         p_next = p_current->p_next;
         
         /* Free the data if requested and it exists */
         if ((b_free_data) && (NULL != p_current->p_data))
         {
             free(p_current->p_data);
         }
         
         /* Free the element */
         free(p_current);
         p_current = p_next;
     }
     
     /* Reset the stack structure */
     p_stack->p_top = NULL;
     p_stack->size = 0;
 }
 
 /*!
  * @brief Destroy the stack, freeing all memory associated with it.
  *
  * @param[in,out] p_stack Pointer to the stack.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void
 stack_destroy(stack_t *p_stack, bool b_free_data)
 {
     if (NULL == p_stack)
     {
         return;
     }
     
     /* Clear all elements and their data if requested */
     stack_clear(p_stack, b_free_data);
 }
 /*** end of file ***/