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

#include <dlink.h>
#include <line.h>
#include <line_list.h>

void line_list_copy(line_list_t *copyer, line_list_t *copee)
{
  dlink_t *a, *b;

  assert(copyer);
  assert(copee);
  assert(line_list_get_count(copyer) == line_list_get_count(copee));

  a = copyer->tail->next;
  for (b = copee->tail->next; b != copee->head; b = b->next) {
    line_copy((line_t *)a->object, (line_t *)b->object);
    a = a->next;
  }
}

line_list_t *line_list_new_and_copy(line_list_t *copee)
{
  dlink_t *a, *b;
  line_list_t *ret;
  line_t *p;

  assert(copee);

  ret = line_list_new();

  for (a = copee->tail->next; a != copee->head; a = a->next) {
    b = dlink_new();
    p = line_new_and_copy((line_t *)a->object);
    line_inc_ref(p);
    b->object = (void *)p;
    dlist_insert(b, (dlist_t *)ret);
  }

  return ret;
}

void line_list_push(line_t *line, line_list_t *list)
{
  dlink_t *link;

  assert(line);
  assert(list);

  line_inc_ref(line);
  link = dlink_new(), link->object = (void *)line;
  dlist_push(link, (dlist_t *)list);
}

line_t *line_list_pop(line_list_t *list)
{
  dlink_t *link;
  line_t *line;

  assert(list);

  link = dlist_pop((dlist_t *)list);
  line = (line_t *)link->object;
  line_dec_ref(line);
  dlink_destroy(link);

  return line;
}

void line_list_insert(line_t *line, line_list_t *list)
{
  dlink_t *link;

  assert(line);
  assert(list);

  line_inc_ref(line);
  link = dlink_new(), link->object = (void *)line;
  dlist_insert(link, (dlist_t *)list);
}

line_t *line_list_extract(line_list_t *list)
{
  dlink_t *link;
  line_t *line;

  assert(list);

  link = dlist_extract((dlist_t *)list);
  line = (line_t *)link->object;
  line_dec_ref(line);
  dlink_destroy(link);

  return line;
}

line_t *line_list_glimpse(int i, line_list_t *list)
{
  dlink_t *link;
  line_t *line;

  assert(list);
  assert(i >= 0 && i < line_list_get_count(list));

  link = dlist_glimpse(i, (dlist_t *)list);
  line = (line_t *)link->object;

  return line;
}

line_t *line_list_pick(int i, line_list_t *list)
{
  dlink_t *link;
  line_t *line;

  assert(list);
  assert(i >= 0 && i < line_list_get_count(list));

  link = dlist_pick(i, (dlist_t *)list);
  line = (line_t *)link->object;
  line_dec_ref(line);
  line->reference--;
  dlink_destroy(link);

  return line;
}

void line_list_put(line_t *line, int i, line_list_t *list)
{
  dlink_t *link;

  assert(line);
  assert(list);
  assert(i >= 0 && i < line_list_get_count(list));

  line_inc_ref(line);
  link = dlink_new(), link->object = (void *)line;
  dlist_put(link, i, list);
}

int line_list_query(line_t *line, line_list_t *list)
{
  int i;
  dlink_t *x;

  assert(line);
  assert(list);

  for (i = 0, x = list->tail->next; x != list->head; x = x->next) {
    if (line_cmp(line, (line_t *)x->object) == 0)
      return i;
    i++;
  }

  return -1;
}

void line_list_delete(line_list_t *list)
{
  dlink_t *link;
  line_t *line;

  assert(list);

  for (link = list->tail->next; link != list->head; link = link->next) {
    line = (line_t *)link->object;
    line_dec_ref(line);
  }
}

void line_list_destroy(line_list_t *list)
{
  dlink_t *link;

  assert(list);

  if (line_list_get_ref(list) <= 0) {
    while (line_list_get_count(list) > 0) {
      link = dlist_pop((dlist_t *)list);
      line_destroy((line_t *)link->object);
      dlink_destroy(link);
    }
    dlist_destroy((dlist_t *)list);
  } else {
    line_list_dec_ref(list);
  }
}

