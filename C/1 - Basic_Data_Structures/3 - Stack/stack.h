/** @file stack.h
 *
 * @brief A stack implementation following BARR-C coding standard.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2025. All rights reserved.
 */

 #ifndef STACK_H
 #define STACK_H
 
 #include <stdint.h>
 #include <stdbool.h>
 
 /**
  * @brief Structure representing a stack element.
  */
 typedef struct stack_element
 {
     void *p_data;                         /* Pointer to the data stored in the element */
     struct stack_element *p_next;         /* Pointer to the next element in the stack */
 } stack_element_t;
 
 /**
  * @brief Structure representing a stack.
  */
 typedef struct
 {
     stack_element_t *p_top;               /* Pointer to the top element of the stack */
     uint32_t         size;                /* Number of elements in the stack */
 } stack_t;
 
 /**
  * @brief Initialize a stack.
  *
  * @param[in,out] p_stack Pointer to the stack to initialize.
  * 
  * @return true if initialization was successful, false otherwise.
  */
 bool stack_init(stack_t *p_stack);
 
 /**
  * @brief Push a new element onto the stack.
  *
  * @param[in,out] p_stack Pointer to the stack.
  * @param[in] p_data Pointer to the data to be stored in the new element.
  *
  * @return true if the element was pushed successfully, false otherwise.
  */
 bool stack_push(stack_t *p_stack, void *p_data);
 
 /**
  * @brief Pop an element from the stack.
  *
  * @param[in,out] p_stack Pointer to the stack.
  *
  * @return Pointer to the data stored in the popped element, or NULL if the stack is empty.
  */
 void *stack_pop(stack_t *p_stack);
 
 /**
  * @brief Peek at the top element of the stack without removing it.
  *
  * @param[in] p_stack Pointer to the stack.
  *
  * @return Pointer to the data stored in the top element, or NULL if the stack is empty.
  */
 void *stack_peek(const stack_t *p_stack);
 
 /**
  * @brief Get the size of the stack.
  *
  * @param[in] p_stack Pointer to the stack.
  *
  * @return Number of elements in the stack.
  */
 uint32_t stack_size(const stack_t *p_stack);
 
 /**
  * @brief Check if the stack is empty.
  *
  * @param[in] p_stack Pointer to the stack.
  *
  * @return true if the stack is empty, false otherwise.
  */
 bool stack_is_empty(const stack_t *p_stack);
 
 /**
  * @brief Clear the stack, removing all elements.
  *
  * @param[in,out] p_stack Pointer to the stack.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void stack_clear(stack_t *p_stack, bool b_free_data);
 
 /**
  * @brief Destroy the stack, freeing all memory associated with it.
  *
  * @param[in,out] p_stack Pointer to the stack.
  * @param[in] b_free_data Flag indicating whether to free the data pointed to by each element.
  */
 void stack_destroy(stack_t *p_stack, bool b_free_data);
 
 #endif /* STACK_H */
 /*** end of file ***/