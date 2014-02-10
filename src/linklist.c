/*
 * Copyright (c) 2014 Serho Liu. All rights reserved.
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

    free_value *free_func;
};


linklist *linklist_new(free_value *func)
{
    struct linklist *list;

    list = malloc(sizeof(*list));
    if (list == NULL) return NULL;

    list->head = list->tail = NULL;
    list->len = 0;
    list->free_func = func;

    return list;
}


void linklist_free(linklist *list)
{
    if (list == NULL) return;
    
    unsigned long len;
    struct list_node *current, *next;

    current = list->head;
    len = list->len;
    while(len--) {
        next = current->next;
        if (list->free_func) {
            list->free_func(current->value);
        }
        free(current);
        current = next;
    }
    free(list);
}


linklist *linklist_add_head(linklist *list, void *value)
{
    if (list == NULL) return NULL;
    
    struct list_node *node = malloc(sizeof(*node));
    if (node == NULL) return NULL;

    node->value = value;

    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }

    list->len++;

    return list;
}


linklist *linklist_add_tail(linklist *list, void *value)
{
    if (list == NULL) return NULL;
    
    struct list_node *node = malloc(sizeof(*node));
    if (node == NULL) return NULL;

    node->value = value;

    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }

    list->len++;

    return list;
}


linklist *linklist_insert_node(linklist *list, list_node *node,
                               void *value, int after)
{
    
    if (list == NULL || node == NULL) return NULL;
    
    struct list_node *new_node = malloc(sizeof(*new_node));
    if (new_node == NULL) return NULL;

    new_node->value = value;

    if (after) {
        new_node->prev = node;
        new_node->next = node->next;
        if (list->tail == node) {
            list->tail = new_node;
        }
    } else {
        new_node->next = node;
        new_node->prev = node->prev;
        if (list->head == node) {
            list->head = new_node;
        }
    }

    if (new_node->prev != NULL) {
        new_node->prev->next = new_node;
    }
    if (new_node->next != NULL) {
        new_node->next->prev = new_node;
    }

    list->len++;

    return list;
}


void linklist_delete_node(linklist *list, list_node *node)
{
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
        
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }
    
    if (list->free_func) {
        list->free_func(node->value);
    }

    free(node);

    list->len--;
}
