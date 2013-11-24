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

typedef struct hashmap hashmap;

typedef int compare_key(const char *desc, const char *src);

typedef void hashmap_map_func(const char *key, void **value, const void *other);

hashmap *hashmap_new(unsigned long size, compare_key *compare);

void hashmap_free(hashmap *map);

int hashmap_put(hashmap *map, const char *key, void *value);

void *hashmap_get(const hashmap *map, const char *key);

void *hashmap_delete(hashmap *map, const char *key);

int hashmap_map(const hashmap *map, hashmap_map_func *func, const void *other);

#endif 