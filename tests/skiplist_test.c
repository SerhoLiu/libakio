#include <stdio.h>
#include "skiplist.h"
#include "minunit.h"

skiplist *list;

const char test1[20] = "serho";
const char test2[20] = "yours";
static int value = 1;

int compare(const char *desc, const char *src)
{
    return strcmp(desc, src);
}

char *test_skiplist_new()
{
    list = skiplist_new(compare);
    mu_assert(list != NULL, "skiplist new failed!");
    return NULL;
}

char *test_skiplist_insert()
{
    int r;
    r = skiplist_insert(list, test1, &value);
    mu_assert(r == 1, "skiplist insert failed!");

    r = skiplist_insert(list, test2, &value);
    mu_assert(r == 1, "skiplist insert failed!");

    r = skiplist_insert(list, test1, NULL);
    mu_assert(r == 0, "skiplist insert failed!");

    return NULL;
}

char *test_skiplist_length()
{
    unsigned long l = skiplist_length(list);
    mu_assert(l == 2, "skiplist length failed!")
    return NULL;
}

char *test_skiplist_search()
{
    int *v;
    v = skiplist_search(list, test1);
    mu_assert(v == &value, "skiplist search failed!");

    v = skiplist_search(list, "wahaha");
    mu_assert(v == NULL, "skiplist search failed!");

    return NULL;
}

char *test_skiplist_delete()
{
    int *v;
    v = skiplist_delete(list, test2);
    mu_assert(v == &value, "skiplist delete failed!");

    unsigned long l = skiplist_length(list);
    mu_assert(l == 1, "skiplist delete failed!")

    v = skiplist_search(list, test2);
    mu_assert(v == NULL, "skiplist delete failed!");

    return NULL;
}

char *all_tests() {
    mu_suite_start();
    mu_run_test(test_skiplist_new);
    mu_run_test(test_skiplist_insert);
    mu_run_test(test_skiplist_length);
    mu_run_test(test_skiplist_search);
    mu_run_test(test_skiplist_delete);
    skiplist_free(list);
    return NULL;
 }
  
RUN_TESTS(all_tests);