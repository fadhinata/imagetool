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

typedef struct {
  int reference; // reference counter
  real_t x, y, z;
} point_t;

#ifdef __cplusplus
extern "C" {
#endif

  real_t arctan2r(real_t dy, real_t dx);

#define point_get_x(p) ((p)->x)
#define point_get_y(p) ((p)->y)
#define point_get_z(p) ((p)->z)
#define point_put_x(i, p) ((p)->x = i)
#define point_put_y(j, p) ((p)->y = j)
#define point_put_z(k, p) ((p)->z = k)
#define point_set(p, i, j, k) ((p)->x = i, (p)->y = j, (p)->z = k)
#define point_get_ref(p) ((p)->reference)
#define point_inc_ref(p) ((p)->reference++)
#define point_dec_ref(p) ((p)->reference = (p)->reference > 0 ? (p)->reference-1 : (p)->reference)

  point_t *point_new(void);
  //void point_assign(point_t *point, real_t x, real_t y, real_t z);
#define point_assign(p, x, y, z) ((p)->x = x, (p)->y = y, (p)->z = z)
  point_t *point_new_and_set(real_t x, real_t y, real_t z);
  void point_copy(point_t *copyer, point_t *copee);
  int point_cmp(point_t *a, point_t *b);
  point_t *point_new_and_copy(point_t *copee);
  void point_dump(point_t *point);
  void point_destroy(point_t *point);
  void point_move(point_t *p, real_t dx, real_t dy, real_t dz);
  void point_add(point_t *c, point_t *a, point_t *b);
  void point_subtract(point_t *c, point_t *a, point_t *b);
  real_t point_get_distance(point_t *point);
  real_t point_dotproduct(point_t *a, point_t *b);
  void point_xproduct(point_t *c, point_t *a, point_t *b);
  real_t get_area_of_parallelogram(point_t *a, point_t *o, point_t *b);
  real_t get_distance_of_p2p(point_t *a, point_t *b);
  real_t get_area_of_three_points(point_t *A, point_t *B, point_t *C);
  //real_t parallelogram_area_of_3points(point_t *a, point_t *org, point_t *b);
  //real_t area_of_3_points(point_t *A, point_t *B, point_t *C);
  //real_t distance_between_points(point_t *a, point_t *b);
  //real_t angle_between_points(point_t *a, point_t *b);
  //real_t angle_among_points(point_t *a, point_t *b, point_t *c);
  //real_t area_of_3_points(point_t *A, point_t *B, point_t *C);

#ifdef __cplusplus
}
#endif

#endif /* __POINT_H__ */
