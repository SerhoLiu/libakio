/*
 * Copyright (c) 2013 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 */

#include "linklist.h"
#include <stdlib.h>

struct list_node {

    struct list_node *prev, *next;

    void *value;
};

struct linklist {

    struct list_node *head, *tail;

    unsigned long len;

    void (*free_value)(void *ptr);
};
