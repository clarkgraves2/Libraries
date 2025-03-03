#include <check.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p_trie.h"




START_TEST (test_create_new_tree)
{
        patricia_tree_t *trie = create_patricia_tree();
    if (trie == NULL) {
        fprintf(stderr, "Failed to create Patricia Trie\n");
        return 1;
    }
    pat_node_t * test_node = trie;
    // Insert some strings into the trie
    insert_node("apple", trie);
    insert_node("banana", trie);
    insert_node("app", trie);
    insert_node("orange", trie);

    // Print the contents of the trie
    printf("Contents of the Patricia Trie:\n");
    print_patricia_tree(test_node, "");

    // Example of checking if a string exists in the trie
    const char *search_str = "apple";
    if (trie_contains(trie, search_str)) {
        printf("\'%s\' exists in the trie.\n", search_str);
    } else {
        printf("\'%s\' does not exist in the trie.\n", search_str);
    }

    // Clean up: free memory used by the trie
    // (Note: You may want to implement a function to recursively free nodes)
    free(test_node);
    free(trie);
}
END_TEST




// Define test suite and add test cases
//
Suite *
bst_suite (void)
{
    Suite * s;
    TCase * tc_core;

    s       = suite_create ("Patricia Tree");

    tc_core = tcase_create ("Core");

    tcase_add_test (tc_core, test_create_new_tree);


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
