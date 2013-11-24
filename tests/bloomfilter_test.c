#include <stdio.h>
#include "bloomfilter.h"
#include "minunit.h"

bloomfilter *filter;

static char *t1 = "serho";
static char *t2 = "wahaha";

char *test_bloomfilter_new()
{
    filter = bloomfilter_new(100000, 0.00001);
    mu_assert(filter != NULL, "bloomfilter new failed!");
    return NULL;
}

char *test_bloomfilter_add()
{
    int r = bloomfilter_add(filter, t1);
    mu_assert(r == 1, "bloomfilter add failed!");
    return NULL;
}

/* 这个测试不太测试 */
char *test_bloomfilter_check()
{
    int r;
    r = bloomfilter_check(filter, t1);
    mu_assert(r == 1, "bloomfilter check failed!");
    r = bloomfilter_check(filter, t2);
    mu_assert(r == 0, "bloomfilter check failed!");
    return NULL;
}

char *all_tests() {
    mu_suite_start();
    mu_run_test(test_bloomfilter_new);
    mu_run_test(test_bloomfilter_add);
    mu_run_test(test_bloomfilter_check);
  
    return NULL;
 }
  
RUN_TESTS(all_tests);