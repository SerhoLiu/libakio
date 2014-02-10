/*
 * Copyright (c) 2014 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 *
 * LinkList: a double link list
 */

#ifndef _LIBAKIO_LINKLIST_H_
#define _LIBAKIO_LINKLIST_H_

typedef void free_value(void *value);

typedef struct list_node list_node;
typedef struct linklist linklist;


#define linklist_length(l) ((l)->len)

#define linklist_head(l) ((l)->head)

#define linklist_tail(l) ((l)->tail)

#define linklist_prev_node(n) ((n)->prev)

#define linklist_next_node(n) ((n)->next)

#define linklist_node_value(n) ((n)->value)


linklist *linklist_new(free_value *func);

void linklist_free(linklist *list);

linklist *linklist_add_head(linklist *list, void *value);

linklist *linklist_add_tail(linklist *list, void *value);

linklist *linklist_insert_node(linklist *list, list_node *node,
                               void *value, int after);

void linklist_delete_node(linklist *list, list_node *node);


#endif 