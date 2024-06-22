#include <check.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"


START_TEST(test_create_queue)
{
    queue_t * test_queue = create_queue();
    
    ck_assert_ptr_nonnull(test_queue);
    ck_assert_int_eq(queue_size(test_queue), 0);
    ck_assert_true(queue_is_empty(test_queue));
    
    destroy_queue(&test_queue);
    ck_assert_ptr_null(test_queue);
}
END_TEST

START_TEST (test_destroy_queue)
{
    
}
END_TEST

START_TEST (test_queue_enqueue)
{
    
}
END_TEST

START_TEST (test_queue_dequeue)
{
    
}
END_TEST

START_TEST (test_queue_size)
{
    
}
END_TEST

// Define test suite and add test cases
//
Suite *
bst_suite (void)
{
    Suite * s;
    TCase * tc_core;

    s       = suite_create ("BST");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_create_queue);
    tcase_add_test (tc_core, test_destroy_queue);
    tcase_add_test (tc_core, test_queue_enqueue);
    tcase_add_test (tc_core, test_queue_dequeue);
    tcase_add_test (tc_core, test_queue_size);

    suite_add_tcase (s, tc_core);

    return s;
}

int
main (void)
{
    int       number_failed;
    Suite *   s;
    SRunner * sr;

    s  = bst_suite();
    sr = srunner_create (s);

    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
} /* main() */

/*** end of file ***/
