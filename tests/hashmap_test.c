#include <stdio.h>
#include "hashmap.h"
#include "minunit.h"

static char *KEY = "testkey";
static char *VALUE = "testvalue";
static hashmap *map;

int compare(const char *desc, const char *src)
{
    return strcmp(desc, src);
}

void map_func(const char *key, void *value, const void *other)
{
    printf("%s: %s\n", key, (char *)value);
}

char *test_new_hashmap()
{
    map = hashmap_new(0, compare);
    mu_assert(map != NULL, "new hashmap failed!");
    return NULL;
}

char *test_hashmap_put()
{
    debug("#### the put Key: %s and Value: %s", KEY, VALUE);
    int r = hashmap_put(map, KEY, VALUE);
    mu_assert(r == 1, "put hashmap failed!" );
    return NULL;
}

char *test_hashmap_get()
{
    char *value = NULL;
    debug("### the get Key %s", KEY);
    value = hashmap_get(map, KEY);
    mu_assert(value != NULL, "get from hashmap failed!");
    debug("### the get Value: %s", value);
    return NULL;
}

char *test_hashmap_map()
{
    int r = hashmap_map(map, map_func, NULL);
    mu_assert(r == 1, "hashmap map failed");
    return NULL;
}

char *test_hashmap_delete()
{
    char *value = NULL;
    debug("### the delete Key: %s", KEY);
    value = hashmap_delete(map, KEY);
    mu_assert(value != NULL, "delete from hashmap failed!");
    debug("### the get Value: %s", value);
    value = hashmap_get(map, KEY);
    mu_assert(value == NULL, "delete from hashmap failed!");
    return NULL;
}

char *test_free_hashmap()
{
    void *p = (void *)map;
    hashmap_free(map);
    mu_assert(map == p, "delete hashmap failed!")
    return NULL;
}

char *all_tests() {
    mu_suite_start();
    mu_run_test(test_new_hashmap);
    mu_run_test(test_hashmap_put);
    mu_run_test(test_hashmap_get);
    mu_run_test(test_hashmap_map);
    mu_run_test(test_hashmap_delete);
    mu_run_test(test_free_hashmap);
  
    return NULL;
 }
  
RUN_TESTS(all_tests);