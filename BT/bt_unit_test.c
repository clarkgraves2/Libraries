#include "bt.h" // Assuming this is your header file
#include <check.h>
#include <stdlib.h>

// Helper function for comparisons
static int
int_compare (const void * a, const void * b)
{
    return *(int *) a - *(int *) b;
}

// Helper function for traversals
static void
int_print (void * data)
{
    printf ("%d ", *(int *) data);
}

START_TEST (test_create_new_tree)
{
    bt_t * tree = create_new_tree();
    ck_assert_ptr_nonnull (tree);
    ck_assert_ptr_null (tree->root);
    destroy_tree (&tree);
}
END_TEST

START_TEST (test_destroy_tree)
{
    bt_t * tree  = create_new_tree();
    int    value = 10;
    tree->root   = create_node (&value);
    destroy_tree (&tree);
    ck_assert_ptr_null (tree);
}
END_TEST

START_TEST (test_insert_left)
{
    bt_t * tree     = create_new_tree();
    int    root_val = 10, left_val = 5;
    tree->root = create_node (&root_val);

    ck_assert_int_eq (insert_left (tree->root, &left_val), 0);
    ck_assert_ptr_nonnull (tree->root->left);
    ck_assert_int_eq (*(int *) tree->root->left->value, 5);

    // Try inserting left again (should fail)
    ck_assert_int_eq (insert_left (tree->root, &left_val), -1);

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_insert_right)
{
    bt_t * tree     = create_new_tree();
    int    root_val = 10, right_val = 15;
    tree->root = create_node (&root_val);

    ck_assert_int_eq (insert_right (tree->root, &right_val), 0);
    ck_assert_ptr_nonnull (tree->root->right);
    ck_assert_int_eq (*(int *) tree->root->right->value, 15);

    // Try inserting right again (should fail)
    ck_assert_int_eq (insert_right (tree->root, &right_val), -1);

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_get_root)
{
    bt_t * tree  = create_new_tree();
    int    value = 10;
    tree->root   = create_node (&value);

    ck_assert_ptr_eq (get_root (tree), tree->root);
    ck_assert_int_eq(*(int*)get_root(tree)->value), 10);

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_get_left_child)
{
    bt_t * tree     = create_new_tree();
    int    root_val = 10, left_val = 5;
    tree->root = create_node (&root_val);
    insert_left (tree->root, &left_val);

    ck_assert_ptr_eq (get_left_child (tree->root), tree->root->left);
    ck_assert_int_eq (*(int *) get_left_child (tree->root)->value, 5);

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_get_right_child)
{
    bt_t * tree     = create_new_tree();
    int    root_val = 10, right_val = 15;
    tree->root = create_node (&root_val);
    insert_right (tree->root, &right_val);

    ck_assert_ptr_eq (get_right_child (tree->root), tree->root->right);
    ck_assert_int_eq (*(int *) get_right_child (tree->root)->value, 15);

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_get_data)
{
    bt_t * tree  = create_new_tree();
    int    value = 10;
    tree->root   = create_node (&value);

    ck_assert_ptr_eq (get_data (tree->root), &value);
    ck_assert_int_eq (*(int *) get_data (tree->root), 10);

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_set_data)
{
    bt_t * tree      = create_new_tree();
    int    old_value = 10, new_value = 20;
    tree->root = create_node (&old_value);

    ck_assert_int_eq (set_data (tree->root, &new_value), 0);
    ck_assert_int_eq (*(int *) tree->root->value, 20);

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_is_empty)
{
    bt_t * tree = create_new_tree();
    ck_assert (is_empty (tree));

    int value  = 10;
    tree->root = create_node (&value);
    ck_assert (!is_empty (tree));

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_size)
{
    bt_t * tree = create_new_tree();
    ck_assert_int_eq (size (tree), 0);

    int values[] = {10, 5, 15};
    tree->root   = create_node (&values[0]);
    insert_left (tree->root, &values[1]);
    insert_right (tree->root, &values[2]);

    ck_assert_int_eq (size (tree), 3);

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_height)
{
    bt_t * tree = create_new_tree();
    ck_assert_int_eq (height (tree), 0);

    int values[] = {10, 5, 15, 3};
    tree->root   = create_node (&values[0]);
    insert_left (tree->root, &values[1]);
    insert_right (tree->root, &values[2]);
    insert_left (tree->root->left, &values[3]);

    ck_assert_int_eq (height (tree), 3);

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_traversals)
{
    bt_t * tree     = create_new_tree();
    int    values[] = {10, 5, 15, 3, 7};
    tree->root      = create_node (&values[0]);
    insert_left (tree->root, &values[1]);
    insert_right (tree->root, &values[2]);
    insert_left (tree->root->left, &values[3]);
    insert_right (tree->root->left, &values[4]);

    // Note: These tests will print to stdout, you'll need to visually verify
    // correctness
    printf ("Preorder: ");
    traverse_preorder (tree->root, int_print);
    printf ("\nInorder: ");
    traverse_inorder (tree->root, int_print);
    printf ("\nPostorder: ");
    traverse_postorder (tree->root, int_print);
    printf ("\nLevel-order: ");
    traverse_levelorder (tree, int_print);
    printf ("\n");

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_find_node)
{
    bt_t * tree     = create_new_tree();
    int    values[] = {10, 5, 15, 3, 7};
    tree->root      = create_node (&values[0]);
    insert_left (tree->root, &values[1]);
    insert_right (tree->root, &values[2]);
    insert_left (tree->root->left, &values[3]);
    insert_right (tree->root->left, &values[4]);

    int      search_val = 7;
    node_t * found      = find_node (tree, &search_val, int_compare);
    ck_assert_ptr_nonnull (found);
    ck_assert_int_eq (*(int *) found->value, 7);

    search_val = 100;
    found      = find_node (tree, &search_val, int_compare);
    ck_assert_ptr_null (found);

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_delete_node)
{
    bt_t * tree     = create_new_tree();
    int    values[] = {10, 5, 15, 3, 7};
    tree->root      = create_node (&values[0]);
    insert_left (tree->root, &values[1]);
    insert_right (tree->root, &values[2]);
    insert_left (tree->root->left, &values[3]);
    insert_right (tree->root->left, &values[4]);

    int del_val = 5;
    ck_assert_int_eq (delete_node (tree, &del_val, int_compare), 0);
    ck_assert_ptr_null (find_node (tree, &del_val, int_compare));

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_copy_tree)
{
    bt_t * tree     = create_new_tree();
    int    values[] = {10, 5, 15};
    tree->root      = create_node (&values[0]);
    insert_left (tree->root, &values[1]);
    insert_right (tree->root, &values[2]);

    bt_t * copy = copy_tree (tree);
    ck_assert_ptr_nonnull (copy);
    ck_assert_int_eq (size (tree), size (copy));
    ck_assert_int_eq (*(int *) copy->root->value, 10);
    ck_assert_int_eq (*(int *) copy->root->left->value, 5);
    ck_assert_int_eq (*(int *) copy->root->right->value, 15);

    destroy_tree (&tree);
    destroy_tree (&copy);
}
END_TEST

START_TEST (test_is_balanced)
{
    bt_t * tree     = create_new_tree();
    int    values[] = {10, 5, 15, 3};
    tree->root      = create_node (&values[0]);
    insert_left (tree->root, &values[1]);
    insert_right (tree->root, &values[2]);

    ck_assert (is_balanced (tree));

    insert_left (tree->root->left, &values[3]);
    ck_assert (!is_balanced (tree));

    destroy_tree (&tree);
}
END_TEST

START_TEST (test_lowest_common_ancestor)
{
    bt_t * tree     = create_new_tree();
    int    values[] = {10, 5, 15, 3, 7};
    tree->root      = create_node (&values[0]);
    insert_left (tree->root, &values[1]);
    insert_right (tree->root, &values[2]);
    insert_left (tree->root->left, &values[3]);
    insert_right (tree->root->left, &values[4]);

    int      val1 = 3, val2 = 7;
    node_t * lca = lowest_common_ancestor (tree, &val1, &val2, int_compare);
    ck_assert_ptr_nonnull (lca);
    ck_assert_int_eq (*(int *) lca->value, 5);

    destroy_tree (&tree);
}
END_TEST

Suite *
binary_tree_suite (void)
{
    Suite * s;
    TCase * tc_core;

    s       = suite_create ("Binary Tree");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_create_new_tree);
    tcase_add_test (tc_core, test_destroy_tree);
    tcase_add_test (tc_core, test_insert_left);
    tcase_add_test (tc_core, test_insert_right);
    tcase_add_test (tc_core, test_get_root);
    tcase_add_test (tc_core, test_get_left_child);
    tcase_add_test (tc_core, test_get_right_child);
    tcase_add_test (tc_core, test_get_data);
    tcase_add_test (tc_core, test_set_data);
    tcase_add_test (tc_core, test_is_empty);
    tcase_add_test (tc_core, test_size);
    tcase_add_test (tc_core, test_height);
    tcase_add_test (tc_core, test_traversals);
    tcase_add_test (tc_core, test_find_node);
    tcase_add_test (tc_core, test_delete_node);
    tcase_add_test (tc_core, test_copy_tree);
    tcase_add_test (tc_core, test_is_balanced);
    tcase_add_test (tc_core, test_lowest_common_ancestor);

    suite_add_tcase (s, tc_core);

    return s;
}

int
main (void)
{
    int       number_failed;
    Suite *   s;
    SRunner * sr;

    s  = binary_tree_suite();
    sr = srunner_create (s);

    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}