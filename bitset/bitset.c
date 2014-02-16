/*
 * Copyright (c) 2013 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 */

#include "bitset.h"
#include <stdlib.h>
#include <string.h>

struct bitset {
    unsigned long length;
    unsigned char *bytes;
};

bitset *bitset_new(unsigned long length)
{
    bitset *set;

    set = (bitset *)malloc(sizeof(*set));
    if (set == NULL) {
        return NULL;
    }

    unsigned long nbyte = (length + 7) / 8;
    set->bytes = (unsigned char *)malloc(nbyte * sizeof(unsigned char));
    if (set->bytes == NULL) {
        free(set);
        return NULL;
    }
    memset(set->bytes, 0, nbyte * sizeof(unsigned char));
    set->length = length;

    return set;
}

void bitset_free(bitset *set)
{
    if (set == NULL) {
        return;
    }
    free(set->bytes);
    free(set);
}

int bitset_get(bitset *set, unsigned long n)
{
    if (set == NULL || n > set->length) {
        return 0;
    }

    return (set->bytes[n/8] >> (n % 8) & 1);
}

int bitset_set(bitset *set, unsigned long n, int bit) 
{
    if (set == NULL || n > set->length || (bit != 0 && bit != 1)) {
        return 0;
    }

    int prev = (set->bytes[n/8] >> (n % 8) & 1);
    
    if (bit == 1) {
        set->bytes[n/8] |= (1 << (n % 8));
    } else {
        set->bytes[n/8] &= ~(1 << (n % 8));
    }

    return prev;
}

void bitset_setall(bitset *set)
{
    if (set == NULL) {
        return;
    }
    unsigned long nbytes = ((set->length + 7) / 8);
    static unsigned char lsbmask[] = {
        0x7F, 0x3F, 0x1F, 0x0F,
        0x07, 0x03, 0x01
    };
    memset(set->bytes, 0xFF, nbytes);

    int mbits = nbytes * 8 - set->length;
    if (mbits) {
        set->bytes[nbytes-1] &= lsbmask[mbits-1]; 
    }
}

void bitset_clearall(bitset *set)
{
    if (set == NULL) {
        return;
    }
    unsigned long nbytes = ((set->length + 7) / 8);
    memset(set->bytes, 0, nbytes);
}

unsigned long bitset_length(bitset *set)
{
    if (set == NULL) {
        return 0;
    }

    return set->length;
}

unsigned long bitset_count(bitset *set)
{
    
    if (set == NULL) {
        return 0;
    }

    unsigned long length = 0, nbytes, i;
    unsigned char c;
    static char count[] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
    
    nbytes = (set->length + 7) / 8;
    for (i = 0; i <= nbytes; i++) {
        c = set->bytes[i];
        length += count[c&0xF] + count[c>>4];
    }

    return length;
}
