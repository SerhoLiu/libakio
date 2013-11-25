/*
 * Copyright (c) 2013 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 */

#include "bloomfilter.h"
#include "bitset.h"
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

struct bloomfilter {
    float erate;
    int k;
    unsigned long capacity;
    unsigned long nbits;
    unsigned long count;
    bitset *set;
};

static uint32_t _bloom_hash(const char *data);

bloomfilter *bloomfilter_new(unsigned long capacity, float erate)
{
    if (erate <= 0 || erate >= 1 ) {
        return NULL;
    }

    bloomfilter *filter;
    filter = (bloomfilter *)malloc(sizeof(*filter));
    if (filter == NULL) {
        return NULL;
    }
    filter->capacity = capacity;
    filter->erate = erate;
    filter->count = 0;

    /* 这里通过 f = 0.5^k 大约算出 k 的取值 */
    filter->k = ceilf(log2f(1.0 / erate));
    if (filter->k < 1) filter->k = 1;
    if (filter->k > 30) filter->k = 30;

    /* 通过 M/N = ln2 * k 计算 M */
    filter->nbits = capacity * ceilf((filter->k / 0.69));

    /* 通常根据上面计算得出 M/N 和 K 后，需要重新计算准确的 f */
    
    filter->set = bitset_new(filter->nbits);

    return filter;
}

void bloomfilter_free(bloomfilter *filter)
{
    if (filter == NULL) return;
    bitset_free(filter->set);
    free(filter);
}

int bloomfilter_add(bloomfilter *filter, const char *key)
{
    if (filter == NULL || key == NULL) {
        return 0;
    }

    if (filter->count > filter->capacity) {
        return 0;
    }

    uint32_t h = _bloom_hash(key);
    uint32_t delta = (h >> 17) | (h << 15);
    uint32_t bitpos;
    int i;
    for (i = 0; i < filter->k; i++) {
        bitpos = h % filter->nbits;
        bitset_set(filter->set, bitpos, 1);
        h += delta;
    }
    filter->count++;

    return 1;
}

int bloomfilter_check(bloomfilter *filter, const char *key)
{
    if (filter == NULL || key == NULL) {
        return 0;
    }

    uint32_t h = _bloom_hash(key);
    uint32_t delta = (h >> 17) | (h << 15);
    uint32_t bitpos;
    int i;
    for (i = 0; i < filter->k; i++) {
        bitpos = h % filter->nbits;
        if (bitset_get(filter->set, bitpos) != 1) {
            return 0;
        }
        h += delta;
    }

    return 1;
}

static uint32_t murmurhash2(const void * key, size_t len, uint32_t seed)
{
    /* 
     * 'm' and 'r' are mixing constants generated offline.
     *  They're not really 'magic', they just happen to work well.
     */
    uint32_t m = 0x5bd1e995;
    uint32_t r = 24;
   
    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;
   
    /* Mix 4 bytes at a time into the hash */
    const unsigned char * data = (const unsigned char *)key;
   
    while(len >= 4) {
        uint32_t k = *(uint32_t *)data;
       
        k *= m;
        k ^= k >> r;
        k *= m;
       
        h *= m;
        h ^= k;
       
        data += 4;
        len -= 4;
    }
   
    /* Handle the last few bytes of the input array */
    switch(len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
                h *= m;
    };
   
    /**
     * Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated.
     */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
   
    return h;
}

static uint32_t _bloom_hash(const char *data) {
    size_t len = strlen(data);
    return murmurhash2(data, len,  0xbc9f1d34);
}
