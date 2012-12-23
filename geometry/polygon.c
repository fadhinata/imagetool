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
#include <buffering/dlink.h>
#include <geometry/polygon.h>
#include <geometry/point.h>

bool check_position_in_polygon_with_xy_profiling(int x, int y, polygon_t *polygon)
{
  int i;
  bool inside = false;
  point_t *old, *new;
  point_t *left, *right;

  assert(polygon);
  assert(polygon->count >= 3);

  old = polygon_glimpse(polygon->count - 1, polygon);
  for (i = 0; i < polygon->count; i++) {
    new = polygon_glimpse(i, polygon);
    if (point_get_x(new) > point_get_x(old)) {
      left = old; right = new;
    } else {
      left = new; right = old;
    }
    // edge "open" at left end
    if (((point_get_x(new) < x) == (x <= point_get_x(old))) &&
	((y - point_get_y(left)) * (point_get_x(right) - point_get_x(left)) < (point_get_y(right) - point_get_y(left)) * (x - point_get_x(left)))) {
      inside = !inside;
    }
    old = new;
  }

  return inside;
}
#if 0
bool is_in_polygon(int x, int y, polygon_t *polygon)
{
  int i;
  bool inside = false;
  point_t *old, *new;
  point_t *left, *right;

  assert(polygon);
  assert(polygon->count >= 3);

  old = polygon_glimpse(polygon->count - 1, polygon);
  for (i = 0; i < polygon->count; i++) {
    new = polygon_glimpse(i, polygon);
    if (point_get_x(new) > point_get_x(old)) {
      left = old; right = new;
    } else {
      left = new; right = old;
    }
    if (((point_get_x(new) < x) == (x <= point_get_x(old))) &&  /* edge "open" at left end */
	((y - point_get_y(left)) * point_get_x((right) - point_get_x(left)) < (point_get_y(right) - point_get_y(left)) * (x - point_get_x(left)))) {
      inside = !inside;
    }
    old = new;
  }

  return inside;
}
#endif

bool check_point_in_polygon_with_xy_profiling(point_t *p, polygon_t *polygon)
//int point_in_polygon(point_t *p, polygon_t *polygon)
{
  bool inside = false;
  int i;
  point_t *old, *new;
  point_t *left, *right;

  assert(p);
  assert(polygon);
  assert(polygon->count >= 3);

  old = polygon_glimpse(polygon->count - 1, polygon);
  for (i = 0; i < polygon->count; i++) {
    new = polygon_glimpse(i, polygon);
    if (point_get_x(new) > point_get_x(old)) {
      //if (new->x > old->x) {
      left = old; right = new;
    } else {
      left = new; right = old;
    }
    if (((point_get_x(new) < point_get_x(p)) == (point_get_x(p) <= point_get_x(old))) &&
	((point_get_y(p) - point_get_y(left)) * (point_get_x(right) - point_get_x(left)) < (point_get_y(right) - point_get_y(left)) * (point_get_x(p) - point_get_x(left)))) {
#if 0
    if (((new->x < p->x) == (p->x <= old->x)) &&  /* edge "open" at left end */
	((p->y - left->y) * (right->x - left->x) < (right->y - left->y) * (p->x - left->x))) {
#endif

      inside = !inside;
    }
    old = new;
  }

  return inside;
}
