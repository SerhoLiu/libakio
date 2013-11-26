/*
 * Copyright (c) 2013 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 */

#include "skiplist.h"
#include <stdlib.h>

typedef struct skiplist_node {
    const char *key;
    void *value;

    struct skiplist_level {
        struct skiplist_node *forward;
        unsigned long span;
    } level[1];

} skiplist_node;

struct skiplist {
    int level;
    unsigned long length;
    compare_key *compare;

    struct skiplist_node *header, *tail;
};


static skiplist_node *_create_node(int level, const char *key, void *value)
{
    skiplist_node *node;
    node = (skiplist_node *)malloc(sizeof(*node) + level*sizeof(struct skiplist_level));
    if (node == NULL) return NULL;

    node->key = key;
    node->value = value;

    return node;
}

skiplist *skiplist_new(compare_key *compare)
{
    skiplist *list;
    list = (skiplist *)malloc(sizeof(*list));
    if (list == NULL) return NULL;

    list->level = 1;
    list->length = 0;
    list->compare = compare;

    /* 初始化头节点 */
    list->header = _create_node(SKIPLIST_MAXLEVEL, NULL, NULL);
    int i;
    for (i = 0; i < SKIPLIST_MAXLEVEL; i++) {
        list->header->level[i].forward = NULL;
        list->header->level[i].span = 0;
    }
    list->tail = NULL;

    return list;
}

void skiplist_free(skiplist *list)
{
    skiplist_node *node, *next;
    node = list->header->level[0].forward;
    
    free(list->header);

    while(node) {
        next = node->level[0].forward;
        free(node);
        node = next;
    }
    free(list);
}
