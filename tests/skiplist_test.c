#include <stdio.h>
#include "skiplist.h"
#include "minunit.h"

skiplist *list;

char *test_skiplist_new()
{
    list = skiplist_new(NULL);
    mu_assert(list != NULL, "skiplist new failed!");
    return NULL;
}


char *all_tests() {
    mu_suite_start();
    mu_run_test(test_skiplist_new);
    
    skiplist_free(list);
    return NULL;
 }
  
RUN_TESTS(all_tests);