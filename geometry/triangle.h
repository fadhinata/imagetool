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
#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <common.h>
#include <geometry/point.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    int reference;
    point_t *a, *b, *c;
  } triangle_t;

#define triangle_get_a(p) ((p)->a)
#define triangle_get_b(p) ((p)->b)
#define triangle_get_c(p) ((p)->c)
#define triangle_inc_ref(p) ((p)->reference++)
#define triangle_dec_ref(p) ((p)->reference--)
#define triangle_get_ref(p) ((p)->reference)

  triangle_t *triangle_new(void);
  void triangle_assign(triangle_t *triangle, point_t *a, point_t *b, point_t *c);
  triangle_t *triangle_new_and_assign(point_t *a, point_t *b, point_t *c);
  void triangle_copy(triangle_t *copyer, triangle_t *copee);
  triangle_t *triangle_new_and_copy(triangle_t *copee);
  int triangle_cmp(triangle_t *x, triangle_t *y);
  void triangle_dump(triangle_t *tri);
  real_t triangle_area(triangle_t *tri);
  real_t triangle_circumradius(triangle_t *tri);
  real_t triangle_circumcircle(point_t *circumcenter, triangle_t *tri);
  real_t triangle_inradius(triangle_t *tri);
  real_t triangle_incircle(point_t *incenter, triangle_t *tri);
  real_t triangle_a_exradius(triangle_t *tri);
  real_t triangle_a_excircle(point_t *excenter, triangle_t *tri);
  real_t triangle_b_exradius(triangle_t *tri);
  real_t triangle_b_excircle(point_t *excenter, triangle_t *tri);
  real_t triangle_c_exradius(triangle_t *tri);
  real_t triangle_c_excircle(point_t *excenter, triangle_t *tri);
  void triangle_delete(triangle_t *tri);
  void triangle_destroy(triangle_t *triangle);

#ifdef __cplusplus
}
#endif

#endif /* __TRIANGLE_H__ */
