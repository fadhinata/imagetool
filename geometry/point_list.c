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
#include <assert.h>

#include <geometry/point.h>
#include <buffering/dlink.h>
#include <geometry/point_list.h>

void point_list_copy(point_list_t *copyer, point_list_t *copee)
{
  dlink_t *a, *b;

  assert(copyer);
  assert(copee);
  assert(point_list_get_count(copyer) == point_list_get_count(copee));

  a = copyer->tail->next;
  for (b = copee->tail->next; b != copee->head; b = b->next) {
    point_copy((point_t *)a->object, (point_t *)b->object);
    a = a->next;
  }
}

point_list_t *point_list_new_and_copy(point_list_t *list)
{
  dlink_t *a, *b;
  point_list_t *ret;
  point_t *p;

  assert(list);

  ret = point_list_new();

  for (a = list->tail->next; a != list->head; a = a->next) {
    p = point_new_and_copy((point_t *)a->object);
    point_inc_ref(p);
    b = dlink_new();
    b->object = (void *)p;
    dlist_insert(b, (dlist_t *)ret);
  }

  return ret;
}

void point_list_push(point_t *p, point_list_t *list)
{
  dlink_t *link;

  assert(p);
  assert(list);

  point_inc_ref(p);
  link = dlink_new();
  link->object = (void *)p;
  dlist_push(link, list);
}

point_t *point_list_pop(point_list_t *list)
{
  dlink_t *link;
  point_t *p;

  assert(list);

  link = dlist_pop((dlist_t *)list);
  p = (point_t *)link->object;
  point_dec_ref(p);
  dlink_destroy(link);

  return p;
}

void point_list_insert(point_t *p, point_list_t *list)
{
  dlink_t *link;

  assert(p);
  assert(list);

  point_inc_ref(p);
  link = dlink_new();
  link->object = (void *)p;
  dlist_insert(link, list);
}

point_t *point_list_extract(point_list_t *list)
{
  dlink_t *link;
  point_t *p;

  assert(list);

  link = dlist_extract((dlist_t *)list);
  p = (point_t *)link->object;
  point_dec_ref(p);
  dlink_destroy(link);

  return p;
}

point_t *point_list_glimpse(int i, point_list_t *list)
{
  dlink_t *link;
  point_t *p;

  assert(list);
  assert(0 <= i && i < point_list_get_count(list));

  link = dlist_glimpse(i, (dlist_t *)list);
  p = (point_t *)link->object;

  return p;
}

point_t *point_list_pick(int i, point_list_t *list)
{
  dlink_t *link;
  point_t *p;

  assert(list);
  assert(0 <= i && i < point_list_get_count(list));

  link = dlist_pick(i, (dlist_t *)list);
  p = (point_t *)link->object;
  point_dec_ref(p);
  dlink_destroy(link);

  return p;
}

void point_list_put(point_t *p, int i, point_list_t *list)
{
  dlink_t *link;

  assert(p);
  assert(list);
  assert(0 <= i && i < point_list_get_count(list));

  point_inc_ref(p);
  link = dlink_new();
  link->object = (void *)p;
  dlist_put(link, i, list);
}

int point_list_query(point_t *p, point_list_t *list)
{
  int i;
  dlink_t *x;

  assert(p);
  assert(list);

  i = 0;
  for (x = list->tail->next; x != list->head; x = x->next) {
    if (point_cmp(p, (point_t *)x->object) == 0)
      return i;
    i++;
  }

  return -1;
}

void point_list_delete(point_list_t *list)
{
  dlink_t *link;
  point_t *p;

  assert(list);

  for (link = list->tail->next; link != list->head; link = link->next) {
    p = (point_t *)link->object;
    point_dec_ref(p);
  }
}

void point_list_destroy(point_list_t *list)
{
  dlink_t *link;

  assert(list);

  if (point_list_get_ref(list) <= 0) {
    while (point_list_get_count(list) > 0) {
      link = dlist_pop((dlist_t *)list);
      point_destroy((point_t *)link->object);
      dlink_destroy(link);
    }
    dlist_destroy((dlist_t *)list);
  } else {
    point_list_dec_ref(list);
  }
}

