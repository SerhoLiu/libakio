/*
 * Copyright (c) 2013 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 *
 * HashMap: a hashtable
 */

#ifndef _LIBAKIO_HASHMAP_H_
#define _LIBAKIO_HASHMAP_H_

typedef int compare_key(const void *desc, const void *src);

typedef struct hashmap hashmap;

hashmap *hashmap_new(unsigned long size, compare_key *compare);

void hashmap_free(hashmap *map);

int hashmap_put(hashmap *map, const void *key, const void *value);

void *hashmap_get(hashmap *map, const void *key);

void *hashmap_delete(hashmap *map, const void *key);

#endif 