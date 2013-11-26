/*
 * Copyright (c) 2013 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 *
 * SkipList: a skiplist
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

typedef struct  skiplist skiplist;

skiplist *skiplist_new(compare_key *compare);

void skiplist_free(skiplist *list);

#endif 