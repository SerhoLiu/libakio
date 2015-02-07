/**
 * \file timer.h
 *
 * \brief timer heap
 *
 * Copyright (c) 2015 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 *
 */

#ifndef _KZNET_TIMER_H_
#define _KZNET_TIMER_H_


struct timer_heap *timer_heap_create(int maxsize);

void timer_heap_delete(struct timer_heap *heap);

int get_shortest_msec(struct timer_heap *heap);

int process_time_events(struct event_loop *loop);

#endif
