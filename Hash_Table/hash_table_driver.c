#include "hash_table.h"
#include <check.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



START_TEST (create_hash_table)
{
    int size = 20;
    hash_function * test_function;
    hash_table_t * hash_table = create_hash_table(size, test_function);
}
END_TEST

Suite *
hash_table_suite (void)
{
    Suite * s;
    TCase * tc_core;

    s       = suite_create ("hash_table.c");

    tc_core = tcase_create ("create_hash_table()");

    tcase_add_test (tc_core, create_hash_table);
  
    suite_add_tcase (s, tc_core);

    return s;
}

int
main (void)
{
    int       number_failed;
    Suite *   s;
    SRunner * sr;

    s  =  hash_table_suite();
    sr = srunner_create (s);

    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
} /* main() */

/*** end of file ***/
