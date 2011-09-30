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
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include <common.h>
#include <triangle.h>
#include <triangle_list.h>

void triangle_list_copy(triangle_list_t *copyer, triangle_list_t *copee)
{
  dlink_t *a, *b;

  assert(copyer);
  assert(copee);
  assert(triangle_list_get_count(copyer) == triangle_list_get_count(copee));

  a = copyer->tail->next;
  for (b = copee->tail->next; b != copee->head; b = b->next) {
    triangle_copy((triangle_t *)a->object, (triangle_t *)b->object);
    a = a->next;
  }
}

triangle_list_t *triangle_list_new_and_copy(triangle_list_t *copee)
{
  dlink_t *a, *b;
  triangle_list_t *list;
  triangle_t *tri;

  assert(copee);

  list = triangle_list_new();
  for (b = copee->tail->next; b != copee->head; b = b->next) {
    a = dlink_new();
    tri = triangle_new_and_copy((triangle_t *)b->object);
    triangle_inc_ref(tri);
    a->object = (void *)tri;
    dlist_insert(a, list);
  }

  return list;
}

void triangle_list_push(triangle_t *triangle, triangle_list_t *list)
{
  dlink_t *link;

  assert(triangle);
  assert(list);

  triangle_inc_ref(triangle);
  link = dlink_new(), link->object = (void *)triangle;
  dlist_push(link, list);
}

triangle_t *triangle_list_pop(triangle_list_t *list)
{
  dlink_t *link;
  triangle_t *tri;

  assert(list);

  link = dlist_pop((dlist_t *)list);
  tri = (triangle_t *)link->object;
  triangle_dec_ref(tri);
  dlink_destroy(link);

  return tri;
}

void triangle_list_insert(triangle_t *triangle, triangle_list_t *list)
{
  dlink_t *link;

  assert(triangle);
  assert(list);

  triangle_inc_ref(triangle);
  link = dlink_new(), link->object = (void *)triangle;
  dlist_insert(link, list);
}

triangle_t *triangle_list_extract(triangle_list_t *list)
{
  dlink_t *link;
  triangle_t *tri;

  assert(list);

  link = dlist_extract((dlist_t *)list);
  tri = (triangle_t *)link->object;
  triangle_dec_ref(tri);
  dlink_destroy(link);

  return tri;
}

triangle_t *triangle_list_glimpse(int i, triangle_list_t *list)
{
  dlink_t *link;
  triangle_t *triangle;

  assert(list);
  assert(i >= 0 && i < triangle_list_get_count(list));

  link = dlist_glimpse(i, (dlist_t *)list);
  triangle = (triangle_t *)link->object;

  return triangle;
}

triangle_t *triangle_list_pick(int i, triangle_list_t *list)
{
  dlink_t *link;
  triangle_t *tri;

  assert(list);
  assert(i >= 0 && i < triangle_list_get_count(list));

  link = dlist_pick(i, (dlist_t *)list);
  tri = (triangle_t *)link->object;
  triangle_dec_ref(tri);
  dlink_destroy(link);

  return tri;
}

void triangle_list_put(triangle_t *tri, int i, triangle_list_t *list)
{
  dlink_t *link;

  assert(tri);
  assert(list);
  assert(i >= 0 && i < triangle_list_get_count(list));

  triangle_inc_ref(tri);
  link = dlink_new();
  link->object = (void *)tri;
  dlist_put(link, i, list);
}

int triangle_list_query(triangle_t *tri, triangle_list_t *list)
{
  int i;
  dlink_t *x;

  assert(tri);
  assert(list);

  for (i = 0, x = list->tail->next; x != list->head; x = x->next) {
    if (triangle_cmp(tri, (triangle_t *)x->object) == 0) return i;
    i++;
  }
  return -1;
}

void triangle_list_delete(triangle_list_t *list)
{
  dlink_t *link;
  triangle_t *tri;

  assert(list);

  for (link = list->tail->next; link != list->head; link = link->next) {
    tri = (triangle_t *)link->object;
    triangle_dec_ref(tri);
  }
}

void triangle_list_destroy(triangle_list_t *list)
{
  dlink_t *link;

  assert(list);

  if (triangle_list_get_ref(list) <= 0) {
    while (triangle_list_get_count(list) > 0) {
      link = dlist_pop((dlist_t *)list);
      triangle_destroy((triangle_t *)link->object);
      dlink_destroy(link);
    }
    dlist_destroy((dlist_t *)list);
  } else {
    triangle_dec_ref(list);
  }
}
