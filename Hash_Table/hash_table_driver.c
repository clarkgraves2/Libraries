#include "hash_table.h"
#include <check.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//START_TEST ( )
{

}
END_TEST

// Define test suite and add test cases
//
Suite *
hash_table_suite (void)
{
    Suite * s;
    TCase * tc_core;

    //s       = suite_create (" ");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, insert_function);
  

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
