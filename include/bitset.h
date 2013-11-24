/*
 * Copyright (c) 2013 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 *
 * BitSet: a bitset
 */

#ifndef _LIBAKIO_BITSET_H_
#define _LIBAKIO_BITSET_H_

typedef struct bitset bitset;

/**
 * 创建 length 个 bit 的 Bitset
 */
bitset *bitset_new(unsigned long length);

/**
 * 释放 Bitset
 */
void bitset_free(bitset *set);

/**
 * 得到第 n 位的值
 *
 * Return value: 0, 1
 */
int bitset_get(bitset *set, unsigned long n);

/**
 * 将第 n 位设置为 bit 值，只能是0或1,其它值不会对 bitset 做任何改变
 *
 * Return value: n 位原值
 */
int bitset_set(bitset *set, unsigned long n, int bit);

/**
 * 将全部位置为 1
 */
void bitset_setall(bitset *set);

/**
 * 将全部位清零
 */
void bitset_clearall(bitset *set);

/**
 * 返回 bitset 位长度
 */
unsigned long bitset_length(bitset *set);

/**
 * 返回 bitset 中为1的位数量
 */
unsigned long bitset_count(bitset *set);


#endif 