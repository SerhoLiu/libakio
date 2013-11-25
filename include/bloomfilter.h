/*
 * Copyright (c) 2013 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 *
 * BloomFilter: a bloomfilter
 */

#ifndef _LIBAKIO_BLOOMFILTER_H_
#define _LIBAKIO_BLOOMFILTER_H_

typedef struct bloomfilter bloomfilter;

/**
 * 创建 Bloom Filter
 *
 * @capacity 需要加入的集合元素个数，一旦创建后，不允许加入大于此数的元素
 * @erate 预估的错误率，即检查 K 个元素后发生误检的比例
 */
bloomfilter *bloomfilter_new(unsigned long capacity, float erate);

void bloomfilter_free(bloomfilter *filter);

/**
 * 向 Bloom Filter 中加入检测元素
 *
 * Return value: 0 错误，包括已经加入的元素超过 capacity; 1 成功
 */
int bloomfilter_add(bloomfilter *filter, const char *key);

/**
 * 检测 key 是否在集合中
 *
 * Return value: 0 不在; 1 存在
 */
int bloomfilter_check(bloomfilter *filter, const char *key);


#endif 