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
#ifndef __POINT_LIST_H__
#define __POINT_LIST_H__

#include <dlink.h>
#include <point.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef dlist_t point_list_t;

#define point_list_inc_ref(p) ((p)->reference++)
#define point_list_dec_ref(p) ((p)->reference--)
#define point_list_get_ref(p) ((p)->reference)
#define point_list_inc_count(p) ((p)->count++)
#define point_list_dec_count(p) ((p)->count--)
#define point_list_get_count(p) ((p)->count)
#define point_list_new() (point_list_t *)dlist_new()
  void point_list_copy(point_list_t *copyer, point_list_t *copee);
  point_list_t *point_list_new_and_copy(point_list_t *list);
  void point_list_push(point_t *p, point_list_t *list);
  point_t *point_list_pop(point_list_t *list);
  void point_list_insert(point_t *p, point_list_t *list);
  point_t *point_list_extract(point_list_t *list);
  point_t *point_list_glimpse(int i, point_list_t *list);
  point_t *point_list_pick(int i, point_list_t *list);
  void point_list_put(point_t *p, int i, point_list_t *list);
  int point_list_query(point_t *p, point_list_t *list);
  void point_list_delete(point_list_t *list);
  void point_list_destroy(point_list_t *list);

#ifdef __cplusplus
}
#endif

#endif /*  __POINT_LIST_H__ */