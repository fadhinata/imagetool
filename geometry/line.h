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
#ifndef __LINE_H__
#define __LINE_H__

#include <limits.h>
#include <common.h>
#include <geometry/point.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef enum {
    LINE_BY_ENDPOINTS,
    LINE_BY_ENDPOINT_DIRECTION, 
    LINE_BY_POSITION_DIRECTION
  } line_type_t;

  typedef struct {
    int reference;
    line_type_t form_type;
    point_t *a;
    point_t *b;
  } line_t;

#define line_get_a(p) ((p)->a)
#define line_get_b(p) ((p)->b)
#define line_inc_ref(p) ((p)->reference++)
#define line_get_ref(p) ((p)->reference)
#define line_dec_ref(p) ((p)->reference = (p)->reference > 0 ? (p)->reference-1 : (p)->reference)

  //#define line_set(p, a, b) line_assign(p, a, b)

  line_t *line_new(void);
  //void line_assign(line_t *line, point_t *a, point_t *b);
  //line_t *line_new_and_assign(point_t *a, point_t *b);
  void line_set_endpoints(line_t *line, point_t *a, point_t *b);
  void line_set_endpoint_direction(line_t *line, point_t *e, point_t *v);
  void line_set_position_direction(line_t *line, point_t *p, point_t *v);
  void line_copy(line_t *copyer, line_t *copee);
  line_t *line_new_and_copy(line_t *copee);
  real_t line_get_length(line_t *line);
  /*
  real_t line_get_slope(line_t *line);
  real_t line_get_x_intercept(line_t *line);
  real_t line_get_y_intercept(line_t *line);
  */
  void line_read_normal_direction(point_t *dir, line_t *line);
  real_t line_read_shortest_point(point_t *p, line_t *line);
  void line_move(line_t *line, real_t dx, real_t dy, real_t dz);
  real_t line_read_shortest_point_from(point_t *p, line_t *line, point_t *q);
  int line_cmp(line_t *x, line_t *y);
  void line_dump(line_t *line);
  void line_clear(line_t *line);
  void line_destroy(line_t *line);
  bool point_in_line(point_t *p, line_t *line);
  int compute_intersection_point_of_lines(point_t *p, line_t *l, line_t *m);

#ifdef __cplusplus
}
#endif

#endif /* __LINE_H__ */
