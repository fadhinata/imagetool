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
#include <string.h>

#include <dlink.h>

dlink_t *dlink_new(void)
{
  dlink_t *dlink;
  
  dlink = malloc(sizeof(dlink_t));
  assert(dlink);
  memset(dlink, 0, sizeof(dlink_t));

  return dlink;
}

void dlink_prepend(dlink_t *obj, dlink_t *at)
{
  assert(obj);
  assert(at);

  obj->prev = at->prev;
  obj->next = at;

  at->prev->next = obj;
  at->prev = obj;
}

void dlink_append(dlink_t *obj, dlink_t *at)
{
  assert(obj);
  assert(at);

  obj->prev = at;
  obj->next = at->next;

  at->next->prev = obj;
  at->next = obj;
}

void dlink_cutoff(dlink_t *obj)
{
  assert(obj);

  obj->prev->next = obj->next;
  obj->next->prev = obj->prev;
}

void dlink_substitute(dlink_t *dummy, dlink_t *at)
{
  assert(dummy);
  assert(at);

  at->prev->next = dummy;
  at->next->prev = dummy;
  dummy->prev = at->prev;
  dummy->next = at->next;
}

void dlink_exchange(dlink_t *a, dlink_t *b)
{
  dlink_t *dummy;

  assert(a);
  assert(b);

  dummy = dlink_new();

  dlink_substitute(dummy, a);
  dlink_substitute(a, b);
  dlink_substitute(b, dummy);
  
  dlink_destroy(dummy);
}

void dlink_destroy(dlink_t *dlink)
{
  assert(dlink);
  free(dlink);
}

dlist_t *dlist_new(void)
{
  dlist_t *dlist;

  dlist = malloc(sizeof(dlist_t)+2*sizeof(dlink_t));
  assert(dlist);

  memset(dlist, 0, sizeof(dlist_t)+2*sizeof(dlink_t));

  dlist->head = (dlink_t *)(dlist+1);
  dlist->tail = dlist->head+1;

  dlist->head->next = NULL;
  dlist->head->prev = dlist->tail;

  dlist->tail->next = dlist->head;
  dlist->tail->prev = NULL;

  return dlist;
}

void dlist_push(dlink_t *link, dlist_t *list)
{
  assert(list);
  assert(link);

  dlink_append(link, list->tail);
  //dlink_prepend(link, list->head);
  list->count++;
}

dlink_t *dlist_pop(dlist_t *list)
{
  dlink_t *link;
  assert(list);
  
  //link = list->head->prev;
  link = list->tail->next;
  dlink_cutoff(link);
  list->count--;

  return link;
}

void dlist_insert(dlink_t *link, dlist_t *list)
{
  assert(list);
  assert(link);

  dlink_prepend(link, list->head);
  list->count++;
}

dlink_t *dlist_extract(dlist_t *list)
{
  dlink_t *link;

  assert(list);

  if (list->count == 0) return NULL;

  //link = list->tail->next;
  link = list->head->prev;
  dlink_cutoff(link);
  list->count--;

  return link;
}

dlink_t *dlist_glimpse(int index, dlist_t *list)
{
  dlink_t *link;

  assert(list);
  assert(index >= 0 && index < list->count);

  link = list->tail->next;
  while (index--) link = link->next;

  return link;
}

dlink_t *dlist_pick(int index, dlist_t *list)
{
  dlink_t *link;

  assert(list);
  assert(index >= 0 && index < list->count);

  link = list->tail->next;
  while (index--) link = link->next;
  dlink_cutoff(link);
  list->count--;

  return link;
}

void dlist_put(dlink_t *link, int i, dlist_t *list)
{
  dlink_t *ilink;

  assert(link);
  assert(i >= 0 && i < list->count);

  ilink = list->tail->next;
  while (i--) ilink = ilink->next;
  dlink_prepend(link, ilink);
}

void dlist_exchange(int i, int j, dlist_t *list)
{
  dlink_t *ilink, *jlink;

  assert(list);
  assert(i >= 0 && i < list->count);
  assert(j >= 0 && j < list->count);

  ilink = list->tail->next;
  while (i--) ilink = ilink->next;

  jlink = list->tail->next;
  while (j--) jlink = jlink->next;

  dlink_exchange(ilink, jlink);
}

void dlist_destroy(dlist_t *list)
{
  dlink_t *link;

  assert(list);

  if (list->reference == 0) {
    while (list->count > 0) {
      link = dlist_extract(list);
      dlink_destroy(link);
    }
    free(list);
  } else list->reference--;
}

