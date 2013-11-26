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

    struct skiplist_node *forward[1];
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
    node = (skiplist_node *)malloc(sizeof(*node) + level*sizeof(struct skiplist_node));
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
        list->header->forward[i] = NULL;
    }
    list->tail = NULL;

    return list;
}

void skiplist_free(skiplist *list)
{
    if (list == NULL) return;

    skiplist_node *node, *next;
    node = list->header->forward[0];
    
    free(list->header);

    while(node) {
        next = node->forward[0];
        free(node);
        node = next;
    }
    free(list);
}

/**
 * 返回一个介于 1 和 ZSKIPLIST_MAXLEVEL 之间的随机值，作为节点的层数。
 *
 * 根据幂次定律(power law)，数值越大，函数生成它的几率就越小
 */
static int _random_level()
{
    int level = 1;
    while ((random()&0xFFFF) < (SKIPLIST_P * 0xFFFF))
        level += 1;
    return (level < SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
}
