#include "bst.h"
#include <check.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

START_TEST (test_create_new_tree)
{
    bst_t * bst = create_new_tree();

    // Check if bst is not NULL (allocation success)
    ck_assert_ptr_ne (bst, NULL);

    // Check if the root of the BST is NULL (initialization)
    ck_assert_ptr_eq (bst->root, NULL);

    // Free the allocated memory
    free (bst);
}
END_TEST

START_TEST (test_create_new_node)
{
    node_t * node = create_new_node (1);

    // Check if node is not NULL (allocation success)
    //
    ck_assert_ptr_ne (node, NULL);

    // Check if the node fields initialized correctly
    //
    ck_assert_ptr_eq (node->left, NULL);
    ck_assert_ptr_eq (node->right, NULL);
    ck_assert_int_eq (node->data, 1);

    // Free the allocated memory
    //
    free (node);
}
END_TEST

START_TEST (test_insert_node_recursive)
{
    bst_t * tree_to_test = create_new_tree();
    node_t * node_to_test = insert_node_recursive (tree_to_test, 7);

    // Check if node is not NULL (allocation success)
    //
    ck_assert_ptr_ne (tree_to_test, NULL);
    ck_assert_ptr_ne (node_to_test, NULL);

    // Check if the node fields initialized correctly
    //
    ck_assert_ptr_eq (node_to_test->left, NULL);
    ck_assert_ptr_eq (node_to_test->right, NULL);
    ck_assert_int_eq (node_to_test->data, 7);

    // Free the allocated memory
    //
    free (tree_to_test);
    free (node_to_test);
}
END_TEST

// Define test suite and add test cases
Suite *
bst_suite (void)
{
    Suite * s;
    TCase * tc_core;

    s       = suite_create ("BST");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_create_new_tree);
    tcase_add_test (tc_core, test_create_new_node);
    tcase_add_test (tc_core, test_insert_node_recursive);

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
