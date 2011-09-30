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
#ifndef __DELAUNAY_H__
#define __DELAUNAY_H__

#include <dlink.h>
#include <point.h>
#include <bitmap.h>
#include <point_list.h>
#include <triangle.h>
#include <triangle_list.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef dlist_t delaunay_list_t;
  typedef dlist_t delaunay_net_t;

#define delaunay_net_dec_ref(net) dlist_dec_ref((dlist_t *)net)
#define delaunay_net_inc_ref(net) dlist_inc_ref((dlist_t *)net)
#define delaunay_net_get_ref(net) dlist_get_ref((dlist_t *)net)
#define delaunay_net_get_count_of_neighbors(net) dlist_get_count((dlist_t *)net)
#define delaunay_net_get_coordinate(net) (point_t *)((net)->spare)
#define delaunay_net_pop(net) dlist_pop((dlist_t *)net)
  delaunay_net_t *delaunay_net_get_neighbor(int index, delaunay_net_t *net);

#define delaunay_list_new() (delaunay_list_t *)dlist_new()
#define delaunay_list_dec_ref(list) dlist_dec_ref((dlist_t *)list)
#define delaunay_list_inc_ref(list) dlist_inc_ref((dlist_t *)list)
#define delaunay_list_get_ref(list) dlist_get_ref((dlist_t *)list)
#define delaunay_list_get_count_of_networks(list) dlist_get_count((dlist_t *)list)
  delaunay_net_t *delaunay_list_get_net(int inet, delaunay_list_t *list);
  point_t *delaunay_list_get_coordinate(int inet, delaunay_list_t *list);
  void delaunay_list_delete(delaunay_list_t *list);
  void delaunay_list_destroy(delaunay_list_t *list);
  void delaunay_list_create_2d_network(delaunay_list_t *list, point_list_t *points);
  void delaunay_list_hook_triangle(triangle_list_t *tri, int inet, delaunay_list_t *list);
#define delaunay_list_get_count_of_triangles(list) (dlist_get_count((dlist_t *)list->head->object))
#define delaunay_list_get_triangle_list(list) ((triangle_list_t *)list->head->object)
#define delaunay_list_get_triangle(i, list) triangle_list_glimpse(i, (triangle_list_t *)list->head->object)

#ifdef __cplusplus
}
#endif

#endif /* __DELAUNAY_H__ */
