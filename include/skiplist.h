/*
 * Copyright (c) 2013 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 *
 * SkipList: a skiplist
 * more info see: 
 * http://www.cl.cam.ac.uk/teaching/0506/Algorithms/skiplists.pdf
 */

#ifndef _LIBAKIO_SKIPLIST_H_
#define _LIBAKIO_SKIPLIST_H_

#define SKIPLIST_MAXLEVEL 32
#define SKIPLIST_P 0.25

/**
 * 自定义比较两个 Key 值的函数
 *
 * Return value: = 0 相同，< 0 小于, > 0 大于
 */
typedef int compare_key(const char *desc, const char *src);

/**
 * SkipList: Key/Value 关联:
 * Key 是 const char *
 * Value 是 void *
 */
typedef struct  skiplist skiplist;

skiplist *skiplist_new(compare_key *compare);

void skiplist_free(skiplist *list);

/**
 * 插入 Key/Value，其中 key/value 均不能为 NULL
 *
 * Return value: 0 失败; 1 成功
 */
int skiplist_insert(skiplist *list, const char *key, void *value);

/**
 * 根据 Key 查找 Value
 *
 * Return value: 指向 Value 的指针，查找失败为 NULL
 */
void *skiplist_search(const skiplist *list, const char *key);

/**
 * 根据 Key 删除 Key/Value 关联
 *
 * Return value: 指向 Value 的指针，删除失败为 NULL
 */
void *skiplist_delete(skiplist *list, const char *key);

#endif 