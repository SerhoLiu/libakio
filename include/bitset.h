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

bitset *bitset_new(unsigned long length);

void bitset_free(bitset *set);

int bitset_get(bitset *set, unsigned long n);

int bitset_set(bitset *set, unsigned long n, int bit);

void bitset_setall(bitset *set);

void bitset_clearall(bitset *set);

unsigned long bitset_length(bitset *set);

unsigned long bitset_count(bitset *set);


#endif 