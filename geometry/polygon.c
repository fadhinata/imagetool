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
#include <dlink.h>
#include <polygon.h>
#include <point.h>

int is_in_polygon(int x, int y, polygon_t *polygon)
{
  int i, inside = 0;
  point_t *old, *new;
  point_t *left, *right;

  assert(polygon);
  assert(polygon->count >= 3);

  old = polygon_glimpse(polygon->count - 1, polygon);
  for (i = 0; i < polygon->count; i++) {
    new = polygon_glimpse(i, polygon);
    if (new->x > old->x) {
      left = old; right = new;
    } else {
      left = new; right = old;
    }
    if (((new->x < x) == (x <= old->x)) &&  /* edge "open" at left end */
	((y - left->y) * (right->x - left->x) < (right->y - left->y) * (x - left->x))) {
      inside = !inside;
    }
    old = new;
  }
  return inside;
}

int point_in_polygon(point_t *p, polygon_t *polygon)
{
  int i, inside = 0;
  point_t *old, *new;
  point_t *left, *right;

  assert(p);
  assert(polygon);
  assert(polygon->count >= 3);

  old = polygon_glimpse(polygon->count - 1, polygon);
  for (i = 0; i < polygon->count; i++) {
    new = polygon_glimpse(i, polygon);
    if (new->x > old->x) {
      left = old; right = new;
    } else {
      left = new; right = old;
    }
    if (((new->x < p->x) == (p->x <= old->x)) &&  /* edge "open" at left end */
	((p->y - left->y) * (right->x - left->x) < (right->y - left->y) * (p->x - left->x))) {
      inside = !inside;
    }
    old = new;
  }
  return inside;
}
