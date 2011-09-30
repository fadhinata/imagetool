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
#ifndef __LINE_LIST_H__
#define __LINE_LIST_H__

#include <common.h>
#include <dlink.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef dlist_t line_list_t;

#define line_list_new() (line_list_t *)dlist_new()
#define line_list_inc_ref(p) ((p)->reference++)
#define line_list_dec_ref(p) ((p)->reference--)
#define line_list_get_ref(p) ((p)->reference)
#define line_list_get_count(list) ((list)->count)
  void line_list_copy(line_list_t *copyer, line_list_t *copee);
  line_list_t *line_list_new_and_copy(line_list_t *copee);
  void line_list_push(line_t *line, line_list_t *list);
  line_t *line_list_pop(line_list_t *list);
  void line_list_insert(line_t *line, line_list_t *list);
  line_t *line_list_extract(line_list_t *list);
  line_t *line_list_glimpse(int i, line_list_t *list);
  line_t *line_list_pick(int i, line_list_t *list);
  void line_list_put(line_t *line, int i, line_list_t *list);
#define line_list_exchange(i, j, list) dlist_exchange(i, j, (dlist_t *)(list))
  int line_list_query(line_t *line, line_list_t *list);
  void line_list_delete(line_list_t *list);
  void line_list_destroy(line_list_t *list);

#ifdef __cplusplus
}
#endif

#endif /* __LINE_LIST_H__ */
