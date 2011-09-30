/*
 Copyright 2011 Hoyoung Yi.

 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, please visit www.gnu.org.
*/
#ifndef __TRIANGLE_LIST_H__
#define __TRIANGLE_LIST_H__

#include <common.h>
#include <dlink.h>
#include <triangle.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef dlist_t triangle_list_t;

#define triangle_list_new() (triangle_list_t *)dlist_new()
#define triangle_list_inc_ref(p) ((p)->reference++)
#define triangle_list_dec_ref(p) ((p)->reference--)
#define triangle_list_get_ref(p) ((p)->reference)
#define triangle_list_get_count(list) ((list)->count)
  void triangle_list_copy(triangle_list_t *copyer, triangle_list_t *copee);
  triangle_list_t *triangle_list_new_and_copy(triangle_list_t *copee);
  void triangle_list_push(triangle_t *triangle, triangle_list_t *list);
  triangle_t *triangle_list_pop(triangle_list_t *list);
  void triangle_list_insert(triangle_t *triangle, triangle_list_t *list);
  triangle_t *triangle_list_extract(triangle_list_t *list);
  triangle_t *triangle_list_glimpse(int i, triangle_list_t *list);
  triangle_t *triangle_list_pick(int i, triangle_list_t *list);
  void triangle_list_put(triangle_t *tri, int i, triangle_list_t *list);
#define triangle_list_exchange(i, j, list) dlist_exchange(i, j, (dlist_t *)(list))
  int triangle_list_query(triangle_t *tri, triangle_list_t *list);
  void triangle_list_delete(triangle_list_t *list);
  void triangle_list_destroy(triangle_list_t *list);

#ifdef __cplusplus
}
#endif

#endif /* __TRIANGLE_LIST_H__ */
