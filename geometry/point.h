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
#ifndef __POINT_H__
#define __POINT_H__

#include <common.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    int reference; // reference counter
    real_t x, y, z;
  } point_t;

  real_t arctan2r(real_t dy, real_t dx);

#define point_get_x(p) ((p)->x)
#define point_get_y(p) ((p)->y)
#define point_get_z(p) ((p)->z)
#define point_get_ref(p) ((p)->reference)
#define point_inc_ref(p) ((p)->reference++)
#define point_dec_ref(p) ((p)->reference--)

  point_t *point_new(void);
  void point_assign(point_t *point, real_t x, real_t y, real_t z);
  point_t *point_new_and_assign(real_t x, real_t y, real_t z);
  void point_copy(point_t *copyer, point_t *copee);
  int point_cmp(point_t *a, point_t *b);
  point_t *point_new_and_copy(point_t *copee);
  void point_dump(point_t *point);
  void point_destroy(point_t *point);
  void point_add(point_t *c, point_t *a, point_t *b);
  void point_subtract(point_t *c, point_t *a, point_t *b);
  real_t point_distance(point_t *point);
  real_t point_dotproduct(point_t *a, point_t *b);
  void point_xproduct(point_t *c, point_t *a, point_t *b);

  real_t parallelogram_area_of_3points(point_t *a, point_t *org, point_t *b);
  real_t area_of_3_points(point_t *A, point_t *B, point_t *C);
  real_t distance_between_points(point_t *a, point_t *b);
  real_t angle_between_points(point_t *a, point_t *b);
  real_t angle_among_points(point_t *a, point_t *b, point_t *c);
  real_t area_of_3_points(point_t *A, point_t *B, point_t *C);

#ifdef __cplusplus
}
#endif

#endif /* __POINT_H__ */
