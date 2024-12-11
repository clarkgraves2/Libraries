#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"

// Test fixture setup and teardown
static hash_table_t *p_test_table;
static char error_msg[256];

static void 
setup(void)
{
    p_test_table = NULL;
    memset(error_msg, 0, sizeof(error_msg));
}

static void 
teardown(void)
{
    if (NULL != p_test_table)
    {
        // Will implement hash_table_destroy later
        // hash_table_destroy(p_test_table);
        p_test_table = NULL;
    }
}

// Sample hash function for testing
static uint32_t
test_hash_fn(const char *p_key, uint32_t key_len)
{
    uint32_t hash = 0;
    for (uint32_t i = 0; i < key_len; i++)
    {
        hash += (uint32_t)p_key[i];
    }
    return hash;
}

// Test cases
START_TEST(test_create_null_params)
{
    hash_table_error_t result;
    
    // Test NULL table pointer
    result = hash_table_create(NULL, 16, 0.75f, 0.25f, 2.0f, 0.5f, 
                             test_hash_fn, error_msg, sizeof(error_msg));
    ck_assert_int_eq(result, HASH_TABLE_ERROR_NULL_ARG);
    
    // Test NULL hash function
    result = hash_table_create(&p_test_table, 16, 0.75f, 0.25f, 2.0f, 0.5f, 
                             NULL, error_msg, sizeof(error_msg));
    ck_assert_int_eq(result, HASH_TABLE_ERROR_NULL_ARG);
    ck_assert_ptr_null(p_test_table);
    
    // Test NULL error message buffer
    result = hash_table_create(&p_test_table, 16, 0.75f, 0.25f, 2.0f, 0.5f, 
                             test_hash_fn, NULL, sizeof(error_msg));
    ck_assert_int_eq(result, HASH_TABLE_ERROR_NULL_ARG);
    ck_assert_ptr_null(p_test_table);
}
END_TEST

START_TEST(test_create_valid_params)
{
    hash_table_error_t result;
    
    result = hash_table_create(&p_test_table, 16, 0.75f, 0.25f, 2.0f, 0.5f,
                             test_hash_fn, error_msg, sizeof(error_msg));
    
    ck_assert_int_eq(result, HASH_TABLE_SUCCESS);
    ck_assert_ptr_nonnull(p_test_table);
    ck_assert_uint_eq(p_test_table->size, 16);
    ck_assert_float_eq(p_test_table->load_max, 0.75f);
    ck_assert_float_eq(p_test_table->shrink_min, 0.25f);
    ck_assert_float_eq(p_test_table->grow_rate, 2.0f);
    ck_assert_float_eq(p_test_table->shrink_rate, 0.5f);
    ck_assert_ptr_eq(p_test_table->p_hash_fn, test_hash_fn);
    ck_assert_uint_eq(p_test_table->count, 0);
    ck_assert_ptr_nonnull(p_test_table->p_elements);
}
END_TEST

START_TEST(test_create_default_values)
{
    hash_table_error_t result;
    
    // Test with 0 size (should use default)
    result = hash_table_create(&p_test_table, 0, 0.75f, 0.25f, 2.0f, 0.5f,
                             test_hash_fn, error_msg, sizeof(error_msg));
    
    ck_assert_int_eq(result, HASH_TABLE_SUCCESS);
    ck_assert_ptr_nonnull(p_test_table);
    ck_assert_uint_eq(p_test_table->size, HASH_TABLE_DEFAULT_SIZE);
    
    // Cleanup for next test
    // hash_table_destroy(p_test_table);
    p_test_table = NULL;
    
    // Test with invalid load factors (should use defaults)
    result = hash_table_create(&p_test_table, 16, -1.0f, -1.0f, 0.5f, 2.0f,
                             test_hash_fn, error_msg, sizeof(error_msg));
    
    ck_assert_int_eq(result, HASH_TABLE_SUCCESS);
    ck_assert_ptr_nonnull(p_test_table);
    ck_assert_float_eq(p_test_table->load_max, HASH_TABLE_DEFAULT_LOAD);
    ck_assert_float_eq(p_test_table->shrink_min, HASH_TABLE_DEFAULT_SHRINK);
    ck_assert_float_eq(p_test_table->grow_rate, HASH_TABLE_DEFAULT_GROW);
    ck_assert_float_eq(p_test_table->shrink_rate, HASH_TABLE_DEFAULT_SHRINK_RATE);
}
END_TEST

Suite *
hash_table_create_suite(void)
{
    Suite *p_suite;
    TCase *p_core;

    p_suite = suite_create("hash_table_create");
    p_core = tcase_create("Core");

    // Add setup and teardown
    tcase_add_checked_fixture(p_core, setup, teardown);

    // Add test cases
    tcase_add_test(p_core, test_create_null_params);
    tcase_add_test(p_core, test_create_valid_params);
    tcase_add_test(p_core, test_create_default_values);

    suite_add_tcase(p_suite, p_core);

    return p_suite;
}

int 
main(void)
{
    int failed;
    Suite *p_suite;
    SRunner *p_runner;

    p_suite = hash_table_create_suite();
    p_runner = srunner_create(p_suite);

    srunner_run_all(p_runner, CK_NORMAL);
    failed = srunner_ntests_failed(p_runner);
    srunner_free(p_runner);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
