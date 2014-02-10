#include <stdio.h>
#include "linklist.h"
#include "minunit.h"

linklist *list;

int test1 = 1;
int test2 = 2;
int test3 = 3;


char *test_linklist_new()
{
    list = linklist_new(NULL);
    mu_assert(list != NULL, "skiplist new failed!");
    return NULL;
}

char *test_linklist_add_head()
{
    linklist_add_head(list, &test1);
    mu_assert(*(int *)LINKLIST_NODE_VALUE(LINKLIST_HEAD(list)) == test1,
              "linklist add head failed");
    return NULL;
}

char *test_linklist_add_tail()
{
    linklist_add_tail(list, &test3);
    mu_assert(*(int *)LINKLIST_NODE_VALUE(LINKLIST_TAIL(list)) == test3,
              "linklist add tail failed");
    return NULL;
}

char *test_linklist_insert_node()
{
    linklist_insert_node(list, LINKLIST_HEAD(list), &test2, 1);
    int v = *(int *)LINKLIST_NODE_VALUE(LINKLIST_NEXT_NODE(LINKLIST_HEAD(list)));
    mu_assert(v == test2, "linklist insert node failed");
    return NULL;
}

char *test_linklist_move_node()
{
    linklist_move_node(list, LINKLIST_TAIL(list), LINKLIST_HEAD(list), 0);
    int h = *(int *)LINKLIST_NODE_VALUE(LINKLIST_HEAD(list));
    int t = *(int *)LINKLIST_NODE_VALUE(LINKLIST_TAIL(list));
    mu_assert((h == test3) && (t == test2), "linklist move node failed");
    return NULL;
}

char *all_tests() {
    mu_suite_start();
    mu_run_test(test_linklist_new);
    mu_run_test(test_linklist_add_head);
    mu_run_test(test_linklist_add_tail);
    mu_run_test(test_linklist_insert_node);
    mu_run_test(test_linklist_move_node);
    linklist_free(list);
    return NULL;
 }
  
RUN_TESTS(all_tests);