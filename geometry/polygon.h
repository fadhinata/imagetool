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
#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <common.h>
#include <buffering/dlink.h>
#include <geometry/point.h>
#include <geometry/point_list.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef point_list_t polygon_t;

#define polygon_new() (polygon_t *)point_list_new()
#define polygon_inc_ref(p) ((p)->reference++)
#define polygon_dec_ref(p) ((p)->reference--)
#define polygon_get_ref(p) ((p)->reference)
#define polygon_get_count(polygon) ((polygon)->count)
#define polygon_copy(copyer, copee) point_list_copy((point_list_t *)(copyer), (point_list_t *)(copee))
#define polygon_new_and_copy(polygon) (polygon_t *)point_list_new_and_copy((point_list_t *)(polygon))
#define polygon_push(point, polygon) point_list_push(point, (point_list_t *)(polygon))
#define polygon_pop(polygon) point_list_pop((point_list_t *)(polygon))
#define polygon_insert(point, polygon) point_list_insert(point, (point_list_t *)(polygon))
#define polygon_extract(polygon) point_list_extract((point_list_t *)(polygon))
#define polygon_glimpse(i, polygon) point_list_glimpse(i, (point_list_t *)(polygon))
#define polygon_pick(i, polygon) point_list_pick(i, (point_list_t *)(polygon))
#define polygon_exchange(i, j, polygon) point_list_exchange(i, j, (point_list_t *)(polygon))
#define polygon_put(point, i, polygon) point_list_put(point, i, (point_list_t *)(polygon))
#define polygon_query(p, polygon) point_list_query(p, (point_list_t *)(polygon))
#define polygon_delete(polygon) point_list_delete((point_list_t *)polygon)
#define polygon_destroy(polygon) point_list_destroy((point_list_t *)(polygon))

  int is_in_polygon(int x, int y, polygon_t *polygon);
  int point_in_polygon(point_t *p, polygon_t *polygon);

#ifdef __cplusplus
}
#endif

#endif /* __POLYGON_H__ */
