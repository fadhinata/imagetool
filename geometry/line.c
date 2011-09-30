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
#include <line.h>
#include <point.h>

line_t *line_new(void)
{
  line_t *line;

  line = malloc(sizeof(line_t));
  assert(line);

  memset(line, 0, sizeof(line_t));

  return line;
}

void line_assign(line_t *line, point_t *a, point_t *b)
{
  assert(line);
  assert(a);
  assert(b);

  point_inc_ref(a);
  point_inc_ref(b);
  
  line->a = a;
  line->b = b;
}

line_t *line_new_and_assign(point_t *a, point_t *b)
{
  line_t *line;

  assert(a);
  assert(b);

  line = line_new();
  line_assign(line, a, b);

  return line;
}

void line_copy(line_t *copyer, line_t *copee)
{
  assert(copyer);
  assert(copee);

  point_copy(copyer->a, copee->a);
  point_copy(copyer->b, copee->b);
}

line_t *line_new_and_copy(line_t *copee)
{
  line_t *line;
  point_t *a, *b;

  assert(copee);

  line = line_new();
  a = point_new_and_copy(copee->a);
  b = point_new_and_copy(copee->b);
  line_assign(line, a, b);

  return line;
}

real_t line_length(line_t *line)
{
  assert(line);

  return sqrt(sqr(line->a->x - line->b->x) + sqr(line->a->y - line->b->y) + sqr(line->a->z - line->b->z));
}

int line_cmp(line_t *x, line_t *y)
{
  assert(x);
  assert(y);

  if (point_cmp(x->a, y->a) == 0) {
    if (point_cmp(x->b, y->b) == 0) return 0;
  } else if (point_cmp(x->a, y->b) == 0) {
    if (point_cmp(x->b, y->a) == 0) return 0;
  }
  return -1;
}

void line_dump(line_t *line)
{
  assert(line);

  printf("line: a(%.2lf,%.2lf,%.2lf) b(%.2lf,%.2lf,%.2lf)\n", line->a->x, line->a->y, line->a->z, line->b->x, line->b->y, line->b->z);
}

void line_delete(line_t *line)
{
  assert(line);

  if (line->a) point_dec_ref(line->a);
  if (line->b) point_dec_ref(line->b);
}

void line_destroy(line_t *line)
{
  assert(line);

  if (line_get_ref(line) <= 0) {
    point_destroy(line->a);
    point_destroy(line->b);
    free(line);
  } else {
    line_dec_ref(line);
  }
}

int intersect_point_between_lines(point_t *p, line_t *l, line_t *m)
{
  real_t denom, ua, ub;

  assert(p);
  assert(l);
  assert(m);

  // Pl = l->a + Ua * (l->b - l->a)
  // Pm = m->a + Ub * (m->b - m->a)
  // for Pl = Pm
  denom = (m->b->y - m->a->y) * (l->b->x - l->a->x) - (m->b->x - m->a->x) * (l->b->y - l->a->y);
  ua = (m->b->x - m->a->x) * (l->a->y - m->a->y) - (m->b->y - m->a->y) * (l->a->x - m->a->x);
  ub = (l->b->x - l->a->x) * (l->a->y - m->a->y) - (l->b->y - l->a->y) * (l->a->x - m->a->x);

  if (abs(denom) < REAL_EPSILON) {
    if (abs(ua) < REAL_EPSILON && abs(ub) < REAL_EPSILON)
      return 0; // COINCIDENT
    return -1; // PARALLEL
  }

  ua = ua / denom;
  ub = ub / denom;

  if (ua < 0 || ua > 1 || ub < 0  || ub > 1)
    return -2; // Not intersecting

  p->x = l->a->x + ua * (l->b->x - l->a->x);
  p->y = l->a->y + ua * (l->b->y - l->a->y);
  /*
  if (point_cmp(l->a, m->a) == 0 ||
      point_cmp(l->a, m->b) == 0 ||
      point_cmp(l->b, m->a) == 0 ||
      point_cmp(l->b, m->b) == 0) return 2;
  */
  if ((abs(ua) < REAL_EPSILON || abs(ua - 1.0) < REAL_EPSILON) ||
      (abs(ub) < REAL_EPSILON || abs(ub - 1.0) < REAL_EPSILON))
    return 2; // ENDPOINT

  return 1; // INTERSECTING
}
