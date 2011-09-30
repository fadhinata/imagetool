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
#ifndef __VECTOR_LIST_H__
#define __VECTOR_LIST_H__

#include <dlink.h>
#include <vector.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef dlist_t vector_list_t;

#define vector_list_inc_ref(p) ((p)->reference++)
#define vector_list_dec_ref(p) ((p)->reference--)
#define vector_list_get_ref(p) ((p)->reference)
#define vector_list_inc_count(p) ((p)->count++)
#define vector_list_dec_count(p) ((p)->count--)
#define vector_list_get_count(p) ((p)->count)
#define vector_list_new() (vector_list_t *)dlist_new()
  void vector_list_copy(vector_list_t *copyer, vector_list_t *copee);
  vector_list_t *vector_list_new_and_copy(vector_list_t *list);
  void vector_list_push(vector_t *vec, vector_list_t *list);
  vector_t *vector_list_pop(vector_list_t *list);
  void vector_list_insert(vector_t *vec, vector_list_t *list);
  vector_t *vector_list_extract(vector_list_t *list);
  vector_t *vector_list_glimpse(int i, vector_list_t *list);
  vector_t *vector_list_pick(int i, vector_list_t *list);
  void vector_list_put(vector_t *vec, int i, vector_list_t *list);
  int vector_list_query(vector_t *p, vector_list_t *list);
  void vector_list_delete(vector_list_t *list);
  void vector_list_destroy(vector_list_t *list);

#ifdef __cplusplus
}
#endif

#endif /*  __VECTOR_LIST_H__ */
