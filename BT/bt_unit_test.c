#include "bt.h"
#include <check.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bt.h"

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

// Helper function to insert nodes easily
//
void
insert_nodes (bst_t * bst, int values[], int size)
{
    for (int i = 0; i < size; i++)
    {
        insert_node (bst, values[i]);
    }
}

// Helper function to open stdout to a known state and redirect it to buffer
//
void
setup_redirect_stdout ()
{
    freopen ("/dev/null", "a", stdout);
    setvbuf (stdout, buffer, _IOFBF, BUFFER_SIZE);
}

// Helper function to flush the buffer and reset stdout to terminal
//
void
teardown_redirect_stdout ()
{
    fflush (stdout);
    freopen ("/dev/tty", "a", stdout);
}

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

START_TEST (test_insert_node)
{
    bst_t * bst = create_new_tree();

    insert_node (bst, 50);
    ck_assert_ptr_ne (bst->root, NULL);
    ck_assert_int_eq (bst->root->data, 50);

    insert_node (bst, 30);
    ck_assert_ptr_ne (bst->root->left, NULL);
    ck_assert_int_eq (bst->root->left->data, 30);

    insert_node (bst, 70);
    ck_assert_ptr_ne (bst->root->right, NULL);
    ck_assert_int_eq (bst->root->right->data, 70);

    insert_node (bst, 20);
    ck_assert_ptr_ne (bst->root->left->left, NULL);
    ck_assert_int_eq (bst->root->left->left->data, 20);

    insert_node (bst, 40);
    ck_assert_ptr_ne (bst->root->left->right, NULL);
    ck_assert_int_eq (bst->root->left->right->data, 40);

    // Free allocated memory (not necessary for this simple test, but good
    // practice)
    free (bst->root->left->left);
    free (bst->root->left->right);
    free (bst->root->left);
    free (bst->root->right);
    free (bst->root);
    free (bst);
}
END_TEST

START_TEST (test_insert_node_recursive)
{
    bst_t * bst = create_new_tree();

    // Insert first node
    //
    bst->root   = insert_node_recursive (bst->root, 50);
    ck_assert_ptr_ne (bst->root, NULL);
    ck_assert_int_eq (bst->root->data, 50);
    ;

    // Insert a smaller value
    //
    insert_node_recursive (bst->root, 30);
    ck_assert_ptr_ne (bst->root->left, NULL);
    ck_assert_int_eq (bst->root->left->data, 30);

    // Insert a larger value
    //
    insert_node_recursive (bst->root, 70);
    ck_assert_ptr_ne (bst->root->right, NULL);
    ck_assert_int_eq (bst->root->right->data, 70);

    // Insert another smaller value
    //
    insert_node_recursive (bst->root, 20);
    ck_assert_ptr_ne (bst->root->left->left, NULL);
    ck_assert_int_eq (bst->root->left->left->data, 20);

    // Insert another larger value
    //
    insert_node_recursive (bst->root, 40);
    ck_assert_ptr_ne (bst->root->left->right, NULL);
    ck_assert_int_eq (bst->root->left->right->data, 40);

    // Free allocated memory (not necessary for this simple test, but good
    // practice)
    //
    free (bst->root->left->left);
    free (bst->root->left->right);
    free (bst->root->left);
    free (bst->root->right);
    free (bst->root);
    free (bst);
}
END_TEST

START_TEST (test_search)
{
    bst_t * bst = create_new_tree();

    insert_node (bst, 50);
    ck_assert_ptr_ne (bst->root, NULL);
    ck_assert_int_eq (bst->root->data, 50);

    insert_node (bst, 30);
    ck_assert_ptr_ne (bst->root->left, NULL);
    ck_assert_int_eq (bst->root->left->data, 30);

    insert_node (bst, 70);
    ck_assert_ptr_ne (bst->root->right, NULL);
    ck_assert_int_eq (bst->root->right->data, 70);

    insert_node (bst, 20);
    ck_assert_ptr_ne (bst->root->left->left, NULL);
    ck_assert_int_eq (bst->root->left->left->data, 20);

    insert_node (bst, 40);
    ck_assert_ptr_ne (bst->root->left->right, NULL);
    ck_assert_int_eq (bst->root->left->right->data, 40);

    node_t * search_result;

    search_result = search (bst->root, 50);
    ck_assert_ptr_ne (search_result, NULL);
    ck_assert_int_eq (search_result->data, 50);

    search_result = search (bst->root, 30);
    ck_assert_ptr_ne (search_result, NULL);
    ck_assert_int_eq (search_result->data, 30);

    search_result = search (bst->root, 70);
    ck_assert_ptr_ne (search_result, NULL);
    ck_assert_int_eq (search_result->data, 70);

    search_result = search (bst->root, 20);
    ck_assert_ptr_ne (search_result, NULL);
    ck_assert_int_eq (search_result->data, 20);

    search_result = search (bst->root, 40);
    ck_assert_ptr_ne (search_result, NULL);
    ck_assert_int_eq (search_result->data, 40);
}
END_TEST

START_TEST (test_find_min)
{
    bst_t * bst = create_new_tree();

    insert_node (bst, 50);
    ck_assert_ptr_ne (bst->root, NULL);
    ck_assert_int_eq (bst->root->data, 50);

    insert_node (bst, 30);
    ck_assert_ptr_ne (bst->root->left, NULL);
    ck_assert_int_eq (bst->root->left->data, 30);

    insert_node (bst, 70);
    ck_assert_ptr_ne (bst->root->right, NULL);
    ck_assert_int_eq (bst->root->right->data, 70);

    insert_node (bst, 20);
    ck_assert_ptr_ne (bst->root->left->left, NULL);
    ck_assert_int_eq (bst->root->left->left->data, 20);

    insert_node (bst, 40);
    ck_assert_ptr_ne (bst->root->left->right, NULL);
    ck_assert_int_eq (bst->root->left->right->data, 40);

    node_t * find_min_result;

    find_min_result = find_min (bst->root);
    ck_assert_ptr_ne (find_min_result, NULL);
    ck_assert_int_eq (find_min_result->data, 20);
}
END_TEST

START_TEST (test_find_max)
{
    bst_t * bst = create_new_tree();

    insert_node (bst, 50);
    ck_assert_ptr_ne (bst->root, NULL);
    ck_assert_int_eq (bst->root->data, 50);

    insert_node (bst, 30);
    ck_assert_ptr_ne (bst->root->left, NULL);
    ck_assert_int_eq (bst->root->left->data, 30);

    insert_node (bst, 70);
    ck_assert_ptr_ne (bst->root->right, NULL);
    ck_assert_int_eq (bst->root->right->data, 70);

    insert_node (bst, 20);
    ck_assert_ptr_ne (bst->root->left->left, NULL);
    ck_assert_int_eq (bst->root->left->left->data, 20);

    insert_node (bst, 40);
    ck_assert_ptr_ne (bst->root->left->right, NULL);
    ck_assert_int_eq (bst->root->left->right->data, 40);

    node_t * find_max_result;

    find_max_result = find_max (bst->root);
    ck_assert_ptr_ne (find_max_result, NULL);
    ck_assert_int_eq (find_max_result->data, 70);

    // printf ("Expected Output: 70\n");
    // printf ("Max Result: %d\n", find_max_result->data);
}
END_TEST

START_TEST (test_delete_node)
{
    bst_t * bst      = create_new_tree();
    int     values[] = {50, 30, 70, 20, 40, 60, 80};
    insert_nodes (bst, values, sizeof (values) / sizeof (values[0]));

    /* Initial tree structure
     *        50
     *       /  \
     *     30    70
     *    /  \  /  \
     *   20  40 60  80
     */

    // Delete a node with no children (leaf node)
    //
    bst->root = delete_node (bst->root, 20);

    /* Expected tree structure
     *        50
     *       /  \
     *     30    70
     *      \   /  \
     *      40 60  80
     */

    // Node 20 should be deleted
    //
    ck_assert_ptr_eq (search (bst->root, 20), NULL);

    // Delete a node with one child
    //
    bst->root = delete_node (bst->root, 30);

    /* Expected tree structure
     *        50
     *       /  \
     *     40    70
     *           /  \
     *         60   80
     */

    // Node 30 should be deleted
    //
    ck_assert_ptr_eq (search (bst->root, 30), NULL);

    // Delete a node with two children
    //
    bst->root = delete_node (bst->root, 50);

    /* Expected tree structure
     *         60
     *       /  \
     *     40    70
     *             \
     *              80
     */

    // Node 50 should be deleted
    //
    ck_assert_ptr_eq (search (bst->root, 50), NULL);

    // Verify the new root is 60
    //
    ck_assert_ptr_ne (bst->root, NULL);
    ck_assert_int_eq (bst->root->data, 60);

    // Verify in-order traversal
    //  
    // printf ("In-order traversal after deletions: \n");
    // printf("Expected output: 40 60 70 80\n");
    // printf ("Output: ");
    // in_order_traversal(bst->root);
    // printf ("\n");
}
END_TEST

START_TEST (test_in_order_traversal)
{

    bst_t * bst      = create_new_tree();
    int     values[] = {50, 30, 70, 20, 40, 60, 80};
    
    insert_nodes (bst, values, sizeof (values) / sizeof (values[0]));

    // Expected output
    //
    const char * expected_output = "20 30 40 50 60 70 80 ";

    // Capture in-order traversal output
    //
    setup_redirect_stdout();
    in_order_traversal (bst->root);
    teardown_redirect_stdout();

    // Check if the captured output matches the expected output
    //
    ck_assert_str_eq (buffer, expected_output);

    // Clean up
    //
    free (bst->root);
    free (bst);
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

    tcase_add_test (tc_core, test_create_new_tree);
    tcase_add_test (tc_core, test_create_new_node);
    tcase_add_test (tc_core, test_insert_node);
    tcase_add_test (tc_core, test_insert_node_recursive);
    tcase_add_test (tc_core, test_search);
    tcase_add_test (tc_core, test_find_min);
    tcase_add_test (tc_core, test_find_max);
    tcase_add_test (tc_core, test_delete_node);
    tcase_add_test (tc_core, test_in_order_traversal);

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
