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

/**
 * LinkList 中 Value 的释放函数
 */
typedef void free_value(void *value);

struct list_node {
    struct list_node *prev, *next;
    void *value;
};

struct linklist {
    struct list_node *head, *tail;
    unsigned long len;
    free_value *free_func;
};

typedef struct list_node list_node;
typedef struct linklist linklist;


#define LINKLIST_LENGTH(l) ((l)->len)

#define LINKLIST_HEAD(l) ((l)->head)

#define LINKLIST_TAIL(l) ((l)->tail)

#define LINKLIST_PREV_NODE(n) ((n)->prev)

#define LINKLIST_NEXT_NODE(n) ((n)->next)

#define LINKLIST_NODE_VALUE(n) ((n)->value)


linklist *linklist_new(free_value *func);

void linklist_free(linklist *list);

/**
 * 创建一个包含 Value 的结点并添加到头部
 */ 
linklist *linklist_add_head(linklist *list, void *value);

/**
 * 创建一个包含 Value 的结点并添加到尾部
 */ 
linklist *linklist_add_tail(linklist *list, void *value);

/**
 * 创建一个包含 Value 的结点并根据 after 指示插入到 node 前或后
 */
linklist *linklist_insert_node(linklist *list, list_node *node,
                               void *value, int after);


/**
 * 将结点 src 按照 after 指示移动到 dst 前或者后
 */
linklist *linklist_move_node(linklist *list, list_node *src,
                             list_node *dst, int after);

void linklist_delete_node(linklist *list, list_node *node);


#endif 