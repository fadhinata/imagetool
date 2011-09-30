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
#ifndef __DLINK_H__
#define __DLINK_H__

#ifdef __cplusplus
extern "C" {
#endif

  struct dlink {
    void *object;
    void *spare;
    struct dlink *prev;
    struct dlink *next;
  };

  typedef struct dlink dlink_t;

  dlink_t *dlink_new(void);
  void dlink_prepend(dlink_t *obj, dlink_t *at);
  void dlink_append(dlink_t *obj, dlink_t *at);
  void dlink_cutoff(dlink_t *obj);
  void dlink_substitute(dlink_t *dummy, dlink_t *at);
  void dlink_exchange(dlink_t *a, dlink_t *b);
  void dlink_destroy(dlink_t *dlink);

  typedef struct {
    int reference;
    int count;
    void *spare;
    dlink_t *head;
    dlink_t *tail;
  } dlist_t;

#define dlist_inc_ref(p) ((p)->reference++)
#define dlist_dec_ref(p) ((p)->reference--)
#define dlist_get_ref(p) ((p)->reference)
#define dlist_inc_count(p) ((p)->count++)
#define dlist_dec_count(p) ((p)->count--)
#define dlist_get_count(p) ((p)->count)
  dlist_t *dlist_new(void);
  void dlist_push(dlink_t *link, dlist_t *list);
  dlink_t *dlist_pop(dlist_t *list);
  void dlist_insert(dlink_t *link, dlist_t *list);
  dlink_t *dlist_extract(dlist_t *list);
  dlink_t *dlist_glimpse(int index, dlist_t *list);
  dlink_t *dlist_pick(int index, dlist_t *list);
  void dlist_put(dlink_t *link, int i, dlist_t *list);
  void dlist_exchange(int i, int j, dlist_t *list);
  void dlist_destroy(dlist_t *list);

#ifdef __cplusplus
}
#endif

#endif /* __DLINK_H__ */
