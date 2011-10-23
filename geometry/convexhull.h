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
#ifndef __CONVEXHULL_H__
#define __CONVEXHULL_H__

#include <common.h>
#include <geometry/line.h>
#include <geometry/point_list.h>
#include <geometry/polygon.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef enum {
    JAVIS_MARCH,
    GRAHAM_SCAN,
    QUICK_HULL
  } convexhull_method_t;

  int convexhull_compute(polygon_t *convexhull, point_list_t *points, convexhull_method_t way);
  real_t convexhull_diameter(line_t *diameter, polygon_t *convexhull);
  real_t convexhull_weak_diameter(line_t *diameter, polygon_t *convexhull);
  real_t convexhull_min_width(line_t *width, polygon_t *convexhull);
  //void convexhull_get_circumference_rectangles(dlist_t *circumrects, polygon_t *convexhull);
  real_t convexhull_create_min_area_rectangle(polygon_t *minrect, polygon_t *convexhull);
  real_t convexhull_create_min_perimeter_rectangle(polygon_t *minrect, polygon_t *convexhull);

#ifdef __cplusplus
}
#endif

#endif /* __CONVEXHULL_H__ */
