/*
 * Copyright (c) 2013 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 *
 * HashTable，采用开放地址解决 Hash 冲突,  Hash 函数
 * 和冲突解决方法参考自 Python mapobject 的实现
 * 
 * 初始化大小是 512，当负载因子达到 2/3 时进行 resize 操作，
 * 每次增加一倍
 */

#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0
#define HASHMAP_INIT_SIZE (1 << 9)

typedef struct {
    void *key;                      
    void *value;
    int isdel;                    
} map_entry;

struct hashmap {
    unsigned long size;
    unsigned long use;
    compare_key *compare;
    map_entry *entrys;
};

static unsigned long string_hash(const char *str);
static void hashmap_resize(hashmap *map);


hashmap *hashmap_new(unsigned long size, compare_key *compare)
{
    hashmap *map;
    
    map = (hashmap *)malloc(sizeof(hashmap));
    if (map == NULL) {
        return NULL;
    }

    if (size == 0) {
        size = HASHMAP_INIT_SIZE;
    }
    map->size = size;
    map->use = 0;
    map->compare = compare;

    map->entrys = (map_entry *)malloc(map->size * sizeof(map_entry));
    if (map->entrys == NULL) {
        free(map);
        return NULL;
    }
    memset(map->entrys, 0, map->size * sizeof(map_entry)); 
    return map;   
}


void hashmap_free(hashmap *map)
{
    if (map == NULL) return;
    free(map->entrys);      /* free all entry */
    free(map);              /* free map node */
}


int hashmap_put(hashmap *map, const void *key, const void *value)
{
    map_entry *entry;

    if (map == NULL || key == NULL) {
        return 0;
    }

    /* 当 use/size >= 2/3 时，扩大表 */
    if (map->use * 3 >= map->size * 2) {
        hashmap_resize(map);
        printf("i'm resize.....now size %ld\n", map->size);
    }

    unsigned long hash = string_hash(key);
    unsigned long index = hash;
    unsigned long perturb = hash;
    
    while (1) {
        index = index & (map->size - 1);
        entry = &(map->entrys[index]);
        
        if ((entry->key != NULL) && (!map->compare(entry->key, key))) {
            entry->value = value;
            if (entry->isdel == TRUE) {
                entry->isdel = FALSE;
            }
            return 1;
        }

        if (entry->key == NULL) {
            entry->key = key;
            entry->value = value;
            entry->isdel = FALSE;
            map->use++;
            return 1;
        }

        perturb >>= 5;
        index = (index << 2) + index + perturb + 1;
    }
}


void *hashmap_get(hashmap *map, const void *key)
{
    map_entry *entry;

    if (map == NULL || key == NULL) {
        return NULL;
    }

    unsigned long hash = string_hash(key);
    unsigned long index = hash;
    unsigned long perturb = hash;

    while (1) {
        index = index & (map->size - 1);
        entry = &(map->entrys[index]);
        if (entry->key == NULL && entry->isdel != TRUE) {
            return NULL;
        }

        /* key 相同，查找成功 */
        if ((entry->key != NULL) && (!map->compare(entry->key, key))) {
            return entry->value;
        }

        perturb >>= 5;
        index = (index << 2) + index + perturb + 1;
    }
}


void *hashmap_delete(hashmap *map, const void *key)
{
    map_entry *entry;

    if (map == NULL || key == NULL) {
        return NULL;
    }

    unsigned long hash = string_hash(key);
    unsigned long index = hash;
    unsigned long perturb = hash;

    while (1) {
        index = index & (map->size - 1);
        entry = &(map->entrys[index]);
        
        if (entry->key == NULL && entry->isdel == TRUE) {
            return NULL;
        }

        /* key 相同，查找成功 */
        if ((entry->key != NULL) && (!map->compare(entry->key, key))) {
            entry->key = NULL;
            entry->isdel = TRUE;
            return entry->value;
        }

        perturb >>= 5;
        index = (index << 2) + index + perturb + 1;
    } 
}

static void hashmap_resize(hashmap *map)
{
    unsigned long oldsize, newsize, olduse, i;
    map_entry *entry, *temp;
    hashmap *newmap;

    oldsize = map->size;
    olduse = map->use;

    /* 这里没有检测 newsize 是否溢出，因为 unsigned long 长度和指针一样 */
    newsize = oldsize << 1;

    newmap = hashmap_new(newsize, map->compare);
    if (newmap == NULL) {
        return;
    }

    for (i = 0; (olduse > 0) && (i < oldsize); i++) {
        entry = &(map->entrys[i]);
        if (entry->key) {
            if (hashmap_put(newmap, entry->key, entry->value)) {
                olduse--;
            } 
        }
    }
    
    /* 如果 olduse == 0, 则重新散列成功，交换 hashtable */
    if (!olduse) {
        temp = map->entrys;
        map->entrys = newmap->entrys;
        map->size = newsize;
        newmap->entrys = temp;
        newmap->size = oldsize;
    }
    hashmap_free(newmap);
}


/* 这个 Hash 函数来自于 Python */
static unsigned long string_hash(const char *str) {
    unsigned long x;
    unsigned long len = 0;
 
    x = *str << 7;
    while (*str) {
        x = (1000003 * x) ^ *str++;
        len++;
    }
    x ^= len;
    return x;
}
