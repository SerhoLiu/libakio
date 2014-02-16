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
    node = (skiplist_node *)malloc(
        sizeof(*node) + level*sizeof(struct skiplist_node));
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

unsigned long skiplist_length(const skiplist *list)
{
    if (list == NULL) return 0;
    return list->length;
}

/**
 * 返回一个介于 1 和 SKIPLIST_MAXLEVEL 之间的随机值，作为节点的层数。
 *
 * 根据幂次定律(power law)，数值越大，函数生成它的几率就越小
 */
static int _random_level()
{
    int level = 1;
    while ((random()&0xFFFF) < (SKIPLIST_P * 0xFFFF)) {
        level += 1;
    }
    return (level < SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
}

int skiplist_insert(skiplist *list, const char *key, void *value)
{
    if (list == NULL || key == NULL || value == NULL) {
        return 0;
    }

    /* update 中是将指向新插入节点各层的指针 */
    skiplist_node *update[SKIPLIST_MAXLEVEL], *x;

    x = list->header;

    /* 找到在需要新插入地方需要“打断”的前节点 */
    int i;
    for (i = list->level-1; i >= 0; i--) {
        while (x->forward[i] &&
               (list->compare(x->forward[i]->key, key) < 0)) {
            x = x->forward[i];
        }
        update[i] = x;
    }

    /* 这里需要判断一下 x 后面节点是否 key 相同，如果是，则更改 value */
    if (x->forward[0] &&
        (list->compare(x->forward[0]->key, key) == 0)) {
        x->forward[0]->value = value;
        return 1;
    }

    /**
     * 否则插入新的节点，新节点层数随机产生，如果新节点层数大于
     * 当前 SkipList 最大层数，则更新相应的数据和指针
     */
    int level = _random_level();
    if (level > list->level) {
        for (i = list->level; i < level; i++) {
            update[i] = list->header;
        }
        list->level = level;
    }

    /* 创建新的节点 */
    x = _create_node(level, key, value);
    for (i = 0; i < level; i++) {
        x->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = x;
    }
    list->length++;

    return 1;
}


void *skiplist_search(const skiplist *list, const char *key)
{
    if (list == NULL || key == NULL) {
        return NULL;
    }

    skiplist_node *x;
    x = list->header;

    int i;
    for (i = list->level-1; i >= 0; i--) {
        while (x->forward[i] &&
               (list->compare(x->forward[i]->key, key) < 0)) {
            x = x->forward[i];
        }
    }

    x = x->forward[0];
    if (x && (list->compare(x->key, key) == 0)) {
        return x->value;
    } else {
        return NULL;
    }
}

void *skiplist_delete(skiplist *list, const char *key)
{
    if (list == NULL || key == NULL) {
        return NULL;
    }

    /* update 中是将指向新插入节点各层的指针 */
    skiplist_node *update[SKIPLIST_MAXLEVEL], *x;
    x = list->header;

    int i;
    for (i = list->level-1; i >= 0; i--) {
        while (x->forward[i] &&
              (list->compare(x->forward[i]->key, key) < 0)) {
            x = x->forward[i];
        }
        update[i] = x;
    }

    x = x->forward[0];

    if (x && (list->compare(x->key, key) == 0)) {
        /* 更新 x 前置节点的指针 */
        for (i = 0; i < list->level; i++) {
            if (update[i]->forward[i] != x) break;
            update[i]->forward[i] = x->forward[i];
        }
        void *oldvalue = x->value;
        free(x);
        list->length--;

        /* 更新 SkipList 层数 */
        while (list->level > 1 &&
               list->header->forward[list->level-1] == NULL) {
            list->level--;
        }

        return oldvalue;
    }

    return NULL;
}
