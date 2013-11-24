#include <stdio.h>
#include "bitset.h"
#include "minunit.h"

static bitset *set;


char *test_bitset_new()
{
    set = bitset_new(100);
    mu_assert(set != NULL, "new hashmap failed!");
    return NULL;
}

char *test_bitset_set()
{
    debug("#### set 2 and 89 is 1");
    bitset_set(set, 2, 1);
    bitset_set(set, 89, 1);
    unsigned long count = bitset_count(set);
    mu_assert(count == 2, "bitset set failed!" );
    return NULL;
}

char *test_bitset_get()
{
    int i, j;
    debug("### get 89 and 78");
    i = bitset_get(set, 89);
    j = bitset_get(set, 78);
    mu_assert((i == 1 && j == 0), "bitset get failed!");
    return NULL;
}

char *test_bitset_count()
{
    unsigned long count = bitset_count(set);
    mu_assert(count == 2, "bitset count failed");
    return NULL;
}

char *test_bitset_clearall()
{
    bitset_clearall(set);
    unsigned long count = bitset_count(set);
    mu_assert(count == 0, "bitset clearall failed!");
    return NULL;
}

char *test_bitset_setall()
{
    bitset_setall(set);
    unsigned long count = bitset_count(set);
    printf("%ld\n", count);
    mu_assert(count == 100, "bitset setall failed!");
    return NULL;
}

char *test_bitset_length()
{
    unsigned long l = bitset_length(set);
    mu_assert(l == 100, "bitset length failed!");
    return NULL;
}


char *all_tests() {
    mu_suite_start();
    mu_run_test(test_bitset_new);
    mu_run_test(test_bitset_set);
    mu_run_test(test_bitset_get);
    mu_run_test(test_bitset_count);
    mu_run_test(test_bitset_clearall);
    mu_run_test(test_bitset_setall);
    mu_run_test(test_bitset_length);
  
    return NULL;
 }
  
RUN_TESTS(all_tests);