#include <stdio.h>
#include "hashmap.h"
#include "minunit.h"

unsigned long SIZE = 100;
char *KEY = "testkey";
numdict *dict;

char *test_new_numdict()
{
    dict = numdict_new();
    mu_assert(dict != NULL, "new numdict failed!");
    return NULL;
}

char *test_numdict_put()
{
    long VLONG = 128;
    debug("#### the put &VLONG: %p", &VLONG);
    int r = numdict_put(dict, KEY, &VLONG);
    mu_assert(r == 1, "put numdict failed!" );
    return NULL;
}

char *test_numdict_get()
{
    long *vlong=NULL;
    debug("### the get vlong %p", vlong);
    vlong = numdict_get(dict, KEY);
    mu_assert(vlong != NULL, "get from numdict failed!");
    debug("### the get vlong %p", vlong);
    mu_assert((*vlong) == 128, "get from numdict failed!");
    return NULL;
}

char *test_delete_numdict()
{
    void *p = (void *)dict;
    numdict_delete(dict);
    mu_assert(dict == p, "delete numdict failed!")
    return NULL;
}

char *all_tests() {
    mu_suite_start();
    mu_run_test(test_new_numdict);
    mu_run_test(test_numdict_put);
    mu_run_test(test_numdict_get);
    mu_run_test(test_delete_numdict);
  
    return NULL;
 }
  
RUN_TESTS(all_tests);