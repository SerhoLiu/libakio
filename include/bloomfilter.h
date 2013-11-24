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

bloomfilter *bloomfilter_new(unsigned long capacity, float erate);

void bloomfilter_free(bloomfilter *filter);

int bloomfilter_add(bloomfilter *filter, const char *key);

int bloomfilter_check(bloomfilter *filter, const char *key);


#endif 