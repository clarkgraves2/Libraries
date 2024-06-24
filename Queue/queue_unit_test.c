#include "queue.h"
#include <check.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

START_TEST (test_create_queue)
{
    queue_t * test_queue = create_queue();

    // Checks for sucessful queue creation
    //
    ck_assert_ptr_nonnull (test_queue);
    ck_assert_int_eq (queue_size (test_queue), 0);
    ck_assert (queue_is_empty (test_queue));

    destroy_queue (&test_queue);
    ck_assert_ptr_null (test_queue);
}
END_TEST

START_TEST (test_destroy_queue)
{
    // Create a queue
    //
    queue_t * test_queue = create_queue();
    ck_assert_ptr_nonnull (test_queue);

    // Add some elements to the queue
    //
    int * value1 = calloc (1, sizeof (int));
    int * value2 = calloc (1, sizeof (int));
    *value1      = 10;
    *value2      = 20;
    queue_enqueue (test_queue, value1);
    queue_enqueue (test_queue, value2);

    // Destroy the queue
    //
    destroy_queue (&test_queue);

    // Check if the queue pointer is now NULL
    //
    ck_assert_ptr_null (test_queue);

    // Test destroying a NULL queue (should not crash)
    //
    destroy_queue (NULL);

    // Test destroying a queue with NULL pointer (should not crash)
    //
    queue_t * null_queue = NULL;
    destroy_queue (&null_queue);
}
END_TEST

START_TEST (test_queue_enqueue)
{
    queue_t * queue = create_queue();
    ck_assert_ptr_nonnull (queue);

    // Test 1: Enqueue to an empty queue
    //
    int * value1 = calloc (1, sizeof (int));
    *value1      = 10;
    int result   = queue_enqueue (queue, value1);
    ck_assert_int_eq (result, 0);
    ck_assert_int_eq (queue_size (queue), 1);

    // Test 2: Enqueue to a non-empty queue
    //
    int * value2 = calloc (1, sizeof (int));
    *value2      = 20;
    result       = queue_enqueue (queue, value2);
    ck_assert_int_eq (result, 0);
    ck_assert_int_eq (queue_size (queue), 2);

    // Test 3: Enqueue with NULL queue (should fail)
    //
    result = queue_enqueue (NULL, value1);
    ck_assert_int_eq (result, -1);

    // Test 4: Enqueue NULL item (should fail)
    //
    result = queue_enqueue (queue, NULL);
    ck_assert_int_eq (result, -1);

    // Test 5: Verify FIFO order
    //
    void * dequeued_value;
    queue_dequeue (queue, &dequeued_value);
    ck_assert_ptr_eq (dequeued_value, value1);
    queue_dequeue (queue, &dequeued_value);
    ck_assert_ptr_eq (dequeued_value, value2);

    free (value1);
    free (value2);
    destroy_queue (&queue);
}
END_TEST

START_TEST (test_queue_dequeue)
{
    queue_t * queue = create_queue();
    ck_assert_ptr_nonnull (queue);

    // Test 1: Dequeue from empty queue
    //
    void * item   = NULL;
    int    result = queue_dequeue (queue, &item);
    ck_assert_int_eq (result, -1);
    ck_assert_ptr_null (item);

    // Test 2: Enqueue and dequeue one item
    //
    int * value1 = calloc (1, sizeof (int));
    *value1      = 10;
    queue_enqueue (queue, value1);
    result = queue_dequeue (queue, &item);
    ck_assert_int_eq (result, 0);
    ck_assert_ptr_eq (item, value1);
    ck_assert_int_eq (*(int *) item, 10);
    ck_assert_int_eq (queue_size (queue), 0);
    ck_assert (queue_is_empty (queue));

    // Test 3: Enqueue multiple items and dequeue
    //
    int * value2 = calloc (1, sizeof (int));
    int * value3 = calloc (1, sizeof (int));
    *value2      = 20;
    *value3      = 30;
    queue_enqueue (queue, value2);
    queue_enqueue (queue, value3);

    result = queue_dequeue (queue, &item);
    ck_assert_int_eq (result, 0);
    ck_assert_ptr_eq (item, value2);
    ck_assert_int_eq (*(int *) item, 20);
    ck_assert_int_eq (queue_size (queue), 1);

    result = queue_dequeue (queue, &item);
    ck_assert_int_eq (result, 0);
    ck_assert_ptr_eq (item, value3);
    ck_assert_int_eq (*(int *) item, 30);
    ck_assert_int_eq (queue_size (queue), 0);
    ck_assert (queue_is_empty (queue));

    // Test 4: Dequeue with NULL queue
    //
    result = queue_dequeue (NULL, &item);
    ck_assert_int_eq (result, -1);

    // Test 5: Dequeue with NULL item pointer
    //
    queue_enqueue (queue, value1);
    result = queue_dequeue (queue, NULL);
    ck_assert_int_eq (result, -1);

    destroy_queue (&queue);
}
END_TEST

START_TEST (test_queue_size)
{
    queue_t * queue = create_queue();
    ck_assert_ptr_nonnull (queue);

    // Test 1: Size of empty queue
    //
    ck_assert_int_eq (queue_size (queue), 0);

    // Test 2: Size after enqueuing one item
    //
    int value1 = 10;
    queue_enqueue (queue, &value1);
    ck_assert_int_eq (queue_size (queue), 1);

    // Test 3: Size after enqueuing multiple items
    //
    int value2 = 20, value3 = 30;
    queue_enqueue (queue, &value2);
    queue_enqueue (queue, &value3);
    ck_assert_int_eq (queue_size (queue), 3);

    // Test 4: Size after dequeuing
    //
    void * item;
    queue_dequeue (queue, &item);
    ck_assert_int_eq (queue_size (queue), 2);

    // Test 5: Size after dequeuing all items
    //
    queue_dequeue (queue, &item);
    queue_dequeue (queue, &item);
    ck_assert_int_eq (queue_size (queue), 0);

    destroy_queue (&queue);
}
END_TEST

START_TEST (test_queue_is_empty)
{
    queue_t * queue = create_queue();
    ck_assert_ptr_nonnull (queue);

    // Test 1: Empty queue
    //
    ck_assert (queue_is_empty (queue));

    // Test 2: Non-empty queue
    //
    int value = 10;
    queue_enqueue (queue, &value);
    ck_assert (!queue_is_empty (queue));

    // Test 3: Queue becomes empty after dequeue
    //
    void * item;
    queue_dequeue (queue, &item);
    ck_assert (queue_is_empty (queue));

    // Test 4: NULL queue
    //
    ck_assert (queue_is_empty (NULL));

    destroy_queue (&queue);
}
END_TEST

// Helper function for queue_foreach test
static void
increment_int (void * item)
{
    (*(int *) item)++;
}

START_TEST (test_queue_peek)
{

    queue_t * queue  = create_queue();
    int       value1 = 10, value2 = 20;
    void *    peeked_item;

    // Test peek on empty queue
    ck_assert_int_eq (queue_peek (queue, &peeked_item), -1);

    queue_enqueue (queue, &value1);
    queue_enqueue (queue, &value2);

    // Test peek on non-empty queue
    ck_assert_int_eq (queue_peek (queue, &peeked_item), 0);
    ck_assert_int_eq (*(int *) peeked_item, 10);

    // Ensure peek doesn't remove the item
    ck_assert_int_eq (queue_size (queue), 2);

    destroy_queue (&queue);
}
END_TEST

START_TEST (test_queue_clear)
{
    queue_t * queue  = create_queue();
    int       value1 = 10, value2 = 20;

    queue_enqueue (queue, &value1);
    queue_enqueue (queue, &value2);

    queue_clear (queue);

    ck_assert_int_eq (queue_size (queue), 0);
    ck_assert (queue_is_empty (queue));

    destroy_queue (&queue);
}
END_TEST

START_TEST (test_queue_copy)
{
    queue_t * queue  = create_queue();
    int       value1 = 10, value2 = 20;
    queue_enqueue (queue, &value1);
    queue_enqueue (queue, &value2);

    queue_t * copied_queue = queue_copy (queue);

    ck_assert_int_eq (queue_size (queue), queue_size (copied_queue));

    void *item1, *item2;
    queue_dequeue (queue, &item1);
    queue_dequeue (copied_queue, &item2);
    ck_assert_int_eq (*(int *) item1, *(int *) item2);

    queue_dequeue (queue, &item1);
    queue_dequeue (copied_queue, &item2);
    ck_assert_int_eq (*(int *) item1, *(int *) item2);

    destroy_queue (&queue);
    destroy_queue (&copied_queue);
}
END_TEST

START_TEST (test_queue_for_each)
{
    queue_t * queue  = create_queue();
    int       value1 = 10, value2 = 20, value3 = 30;
    queue_enqueue (queue, &value1);
    queue_enqueue (queue, &value2);
    queue_enqueue (queue, &value3);

    queue_for_each (queue, increment_int);

    void * item;
    queue_dequeue (queue, &item);
    ck_assert_int_eq (*(int *) item, 11);
    queue_dequeue (queue, &item);
    ck_assert_int_eq (*(int *) item, 21);
    queue_dequeue (queue, &item);
    ck_assert_int_eq (*(int *) item, 31);

    destroy_queue (&queue);
}
END_TEST

START_TEST (test_queue_is_full)
{
    queue_t * queue = create_queue();
    int       value = 10;

    // Assuming MAX_QUEUE_SIZE is 100
    for (int i = 0; i < 99; i++)
    {
        queue_enqueue (queue, &value);
        ck_assert (!queue_is_full (queue));
    }

    queue_enqueue (queue, &value);
    ck_assert (queue_is_full (queue));

    void * item;
    queue_dequeue (queue, &item);
    ck_assert (!queue_is_full (queue));

    destroy_queue (&queue);
}
END_TEST

// Define test suite and add test cases
//
Suite *
queue_suite (void)
{
    Suite * s;
    TCase * tc_core;

    s       = suite_create ("Queue");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_create_queue);
    tcase_add_test (tc_core, test_destroy_queue);
    tcase_add_test (tc_core, test_queue_enqueue);
    tcase_add_test (tc_core, test_queue_dequeue);
    tcase_add_test (tc_core, test_queue_size);
    tcase_add_test (tc_core, test_queue_is_empty);
    tcase_add_test (tc_core, test_queue_peek);
    tcase_add_test (tc_core, test_queue_clear);
    tcase_add_test (tc_core, test_queue_copy);
    tcase_add_test (tc_core, test_queue_for_each);
    tcase_add_test (tc_core, test_queue_is_full);

    suite_add_tcase (s, tc_core);

    return s;
}

int
main (void)
{
    int       number_failed;
    Suite *   s;
    SRunner * sr;

    s  = queue_suite();
    sr = srunner_create (s);

    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
} /* main() */

/*** end of file ***/
