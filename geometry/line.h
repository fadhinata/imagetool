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

#include <common.h>
#include <point.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    int reference;
    point_t *a;
    point_t *b;
  } line_t;

#define line_get_a(p) ((p)->a)
#define line_get_b(p) ((p)->b)
#define line_inc_ref(p) ((p)->reference++)
#define line_dec_ref(p) ((p)->reference--)
#define line_get_ref(p) ((p)->reference)

  line_t *line_new(void);
  void line_assign(line_t *line, point_t *a, point_t *b);
  line_t *line_new_and_assign(point_t *a, point_t *b);
  void line_copy(line_t *copyer, line_t *copee);
  line_t *line_new_and_copy(line_t *copee);
  real_t line_length(line_t *line);
  int line_cmp(line_t *x, line_t *y);
  void line_dump(line_t *line);
  void line_delete(line_t *line);
  void line_destroy(line_t *line);
  int intersect_point_between_lines(point_t *p, line_t *l, line_t *m);

#ifdef __cplusplus
}
#endif

#endif /* __LINE_H__ */
