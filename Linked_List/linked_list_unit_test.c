#include "linked_list.h"
#include <check.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



START_TEST (test_create_list)
{
    llist_t * test_list = create_list();

    ck_assert_ptr_nonnull(test_list);
    ck_assert_int_eq(list_size(test_list), 0);

}
END_TEST

START_TEST (test_list_size)
{
    llist_t * test_list = create_list();
    ck_assert_ptr_nonnull(test_list);

    int sample_data = 42;
    node_t * inserted_node = insert_front(test_list, &sample_data); 
    ck_assert_ptr_nonnull(inserted_node);
    ck_assert_int_eq(list_size(test_list), 1);
}
END_TEST

START_TEST(test_create_node)
{
    int sample_data = 42;
    node_t *test_node = create_node(&sample_data);

    ck_assert_ptr_nonnull(test_node);
    ck_assert_int_eq(*(int *)get_node_data(test_node), 42);
    ck_assert_ptr_null(get_next_ptr(test_node));

    free(test_node);
}
END_TEST

START_TEST(test_insert_front)
{
    int sample_data = 42;
    int sample_data2 = 8;

    llist_t * test_list = create_list();

    node_t * inserted_node = insert_front(test_list, &sample_data); 
    ck_assert_ptr_nonnull(inserted_node);
    ck_assert_int_eq(list_size(test_list), 1);
    ck_assert_int_eq(*(int *)get_node_data(inserted_node), 42);
    ck_assert_ptr_eq(get_next_ptr(inserted_node), NULL);

    node_t * inserted_node2 = insert_front(test_list, &sample_data2);
    ck_assert_ptr_nonnull(inserted_node2);
    ck_assert_int_eq(list_size(test_list), 2);
    ck_assert_int_eq(*(int *)get_node_data(inserted_node2), 8);
    ck_assert_ptr_eq(get_next_ptr(inserted_node2), inserted_node);
    
    free(inserted_node);
    free(inserted_node2);
    free(test_list);

    inserted_node = NULL;
    inserted_node2 = NULL;
    test_list = NULL;
}
END_TEST

START_TEST(test_insert_back)
{
    int sample_data = 42;
    int sample_data2 = 8;

    llist_t * test_list = create_list();

    node_t * inserted_node = insert_back(test_list, &sample_data); 
    ck_assert_ptr_nonnull(inserted_node);
    ck_assert_int_eq(list_size(test_list), 1);
    ck_assert_int_eq(*(int *)get_node_data(inserted_node), 42);
    ck_assert_ptr_eq(get_next_ptr(inserted_node), NULL);

    node_t * inserted_node2 = insert_back(test_list, &sample_data2);
    ck_assert_ptr_nonnull(inserted_node2);
    ck_assert_int_eq(list_size(test_list), 2);
    ck_assert_int_eq(*(int *)get_node_data(inserted_node2), 8);
    ck_assert_ptr_eq(get_next_ptr(inserted_node), NULL);
    
    free(inserted_node);
    free(inserted_node2);
    free(test_list);

    inserted_node = NULL;
    inserted_node2 = NULL;
    test_list = NULL;
}
END_TEST

START_TEST(test_insert_at)
{
    int sample_data = 42;
    int sample_data2 = 8;
    int sample_data3 = 27;

    llist_t * test_list = create_list();

    node_t * inserted_node = insert_at(test_list, 0, &sample_data); 
    ck_assert_ptr_nonnull(inserted_node);
    ck_assert_int_eq(list_size(test_list), 1);
    ck_assert_int_eq(*(int *)get_node_data(inserted_node), 42);
    ck_assert_ptr_eq(get_next_ptr(inserted_node), NULL);

    node_t * inserted_node2 = insert_at(test_list, 1, &sample_data2);
    ck_assert_ptr_nonnull(inserted_node2);
    ck_assert_int_eq(list_size(test_list), 2);
    ck_assert_int_eq(*(int *)get_node_data(inserted_node2), 8);
    ck_assert_ptr_eq(get_next_ptr(inserted_node2), NULL);

    node_t * inserted_node3 = insert_at(test_list, 1, &sample_data3);
    ck_assert_ptr_nonnull(inserted_node3);
    ck_assert_int_eq(list_size(test_list), 3);
    ck_assert_int_eq(*(int *)get_node_data(inserted_node3), 27);
    ck_assert_ptr_eq(get_next_ptr(inserted_node3), inserted_node2);
    ck_assert_ptr_eq(get_next_ptr(inserted_node), inserted_node3);
    
    free(inserted_node);
    free(inserted_node2);
    free(inserted_node3);
    free(test_list);

    inserted_node = NULL;
    inserted_node2 = NULL;
    inserted_node3 = NULL;
    test_list = NULL;
}
END_TEST

// Define test suite and add test cases
//
Suite *
linked_list_suite (void)
{
    Suite * s;
    TCase * tc_core;

    s       = suite_create ("Linked List");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_create_list);
    tcase_add_test (tc_core, test_list_size);
    tcase_add_test (tc_core, test_create_node);
    tcase_add_test (tc_core, test_insert_front);
    tcase_add_test (tc_core, test_insert_back);
    tcase_add_test (tc_core, test_insert_at);


    suite_add_tcase (s, tc_core);

    return s;
}

int
main (void)
{
    int       number_failed;
    Suite *   s;
    SRunner * sr;

    s  =  linked_list_suite();
    sr = srunner_create (s);

    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
} /* main() */

/*** end of file ***/
