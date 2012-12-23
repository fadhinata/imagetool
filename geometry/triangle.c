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
#include <math.h>
#include <string.h>
#include <assert.h>

#include <common.h>
#include <geometry/point.h>
#include <geometry/triangle.h>

triangle_t *triangle_new(void)
{
  triangle_t *triangle;

  triangle = malloc(sizeof(triangle_t));
  assert(triangle);

  memset(triangle, 0, sizeof(triangle_t));

  return triangle;
}

void triangle_set_a(triangle_t *tri, point_t *a)
{
  assert(tri);
  assert(a);

  if (tri->a) point_destroy(tri->a);
  point_inc_ref(a);
  tri->a = a;
}

void triangle_set_b(triangle_t *tri, point_t *b)
{
  assert(tri);
  assert(b);

  if (tri->b) point_destroy(tri->b);
  point_inc_ref(b);
  tri->b = b;
}

void triangle_set_c(triangle_t *tri, point_t *c)
{
  assert(tri);
  assert(c);

  if (tri->c) point_destroy(tri->c);
  point_inc_ref(c);
  tri->c = c;
}

void triangle_set(triangle_t *triangle, point_t *a, point_t *b, point_t *c)
{
  assert(triangle);
  assert(a);
  assert(b);
  assert(c);

  if (triangle->a) point_destroy(triangle->a);
  if (triangle->b) point_destroy(triangle->b);
  if (triangle->c) point_destroy(triangle->c);

  point_inc_ref(a);
  point_inc_ref(b);
  point_inc_ref(c);

  triangle->a = a;
  triangle->b = b;
  triangle->c = c;
}

triangle_t *triangle_new_and_set(point_t *a, point_t *b, point_t *c)
{
  triangle_t *triangle;

  assert(a);
  assert(b);
  assert(c);

  triangle = triangle_new();
  triangle_set(triangle, a, b, c);

  return triangle;
}

void triangle_copy(triangle_t *copyer, triangle_t *copee)
{
  assert(copyer);
  assert(copee);

  point_copy(copyer->a, copee->a);
  point_copy(copyer->b, copee->b);
  point_copy(copyer->c, copee->c);
}

triangle_t *triangle_new_and_copy(triangle_t *copee)
{
  triangle_t *tri;
  point_t *p;

  assert(copee);

  tri = triangle_new();

  p = point_new_and_copy(copee->a);
  //point_inc_ref(p);
  tri->a = p;

  p = point_new_and_copy(copee->b);
  //point_inc_ref(p);
  tri->b = p;

  p = point_new_and_copy(copee->c);
  //point_inc_ref(p);
  tri->c = p;

  return tri;
}

int triangle_cmp(triangle_t *x, triangle_t *y)
{
  assert(x);
  assert(y);

  if (point_cmp(x->a, y->a) == 0) {
    if (point_cmp(x->b, y->b) == 0) {
      if (point_cmp(x->c, y->c) == 0) return 0;
    } else if (point_cmp(x->b, y->c) == 0) {
      if (point_cmp(x->c, y->b) == 0) return 0;
    }
  } else if (point_cmp(x->a, y->b) == 0) {
    if (point_cmp(x->b, y->a) == 0) {
      if (point_cmp(x->c, y->c) == 0) return 0;
    } else if (point_cmp(x->b, y->c) == 0) {
      if (point_cmp(x->c, y->a) == 0) return 0;
    }
  } else if (point_cmp(x->a, y->c) == 0) {
    if (point_cmp(x->b, y->a) == 0) {
      if (point_cmp(x->c, y->b) == 0) return 0;
    } else if (point_cmp(x->b, y->b) == 0) {
      if (point_cmp(x->c, y->a) == 0) return 0;
    }
  }
  return -1;
}

void triangle_dump(triangle_t *tri)
{
  assert(tri);
  printf("triangle: a(%.2lf,%.2lf,%.2lf) b(%.2lf,%.2lf,%.2lf) c(%.2lf,%.2lf,%.2lf)\n", tri->a->x, tri->a->y, tri->a->z, tri->b->x, tri->b->y, tri->b->z, tri->c->x, tri->c->y, tri->c->z);
}

/* By Heron's formula, the area of the triangle
   sqrt(s*(s-a)*(s-b)*(s-c)), s: (a+b+c)/2 */
real_t triangle_compute_area(triangle_t *tri)
{
  real_t s, a, b, c;

  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;

  return sqrt(s * (s - a) * (s - b) * (s - c));
}

real_t triangle_compute_circumradius(triangle_t *tri)
{
  real_t s, a, b, c, area;

  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;

  area = sqrt(s * (s - a) * (s - b) * (s - c));
  assert(!(abs(area) < REAL_EPSILON));

  return ((a * b * c) / (4 * area));
}

/* tri : given triangle
 * circumcenter : the center of circumscribed-circle
 * return value : the radius of circumscribed-circle */
real_t triangle_compute_circumcircle(point_t *circumcenter, triangle_t *tri)
{
  real_t area, a, b, c, s;
  real_t alpha, beta, gamma;
  point_t *d1, *d2;

  assert(circumcenter);
  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;
  area = sqrt(s * (s - a) * (s - b) * (s - c));

  d1 = point_new();
  d2 = point_new();

  point_subtract(d1, tri->a, tri->b);
  point_subtract(d2, tri->a, tri->c);
  alpha = sqr(a) * point_dotproduct(d1, d2) / (2 * sqr(2*area));
  
  point_subtract(d1, tri->b, tri->a);
  point_subtract(d2, tri->b, tri->c);
  beta = sqr(b) * point_dotproduct(d1, d2) / (2 * sqr(2*area));

  point_subtract(d1, tri->c, tri->a);
  point_subtract(d2, tri->c, tri->b);
  gamma = sqr(c) * point_dotproduct(d1, d2) / (2 * sqr(2*area));

  point_destroy(d2);
  point_destroy(d1);

  circumcenter->x = alpha * tri->a->x + beta * tri->b->x + gamma * tri->c->x;
  circumcenter->y = alpha * tri->a->y + beta * tri->b->y + gamma * tri->c->y;
  circumcenter->z = alpha * tri->a->z + beta * tri->b->z + gamma * tri->c->z;

  return (a * b * c) / (4 * area);
}

// s : semiperimeter
// 2 * s : perimeter
real_t triangle_compute_inradius(triangle_t *tri)
{
  real_t s, a, b, c;

  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;

  return sqrt(((s - a) * (s - b) * (s - c)) / s);
}

real_t triangle_compute_incircle(point_t *incenter, triangle_t *tri)
{
  real_t s, a, b, c;

  assert(incenter);
  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;

  incenter->x = (a * tri->a->x + b * tri->b->x + c * tri->c->x) / (2 * s);
  incenter->y = (a * tri->a->y + b * tri->b->y + c * tri->c->y) / (2 * s);
  incenter->z = (a * tri->a->z + b * tri->b->z + c * tri->c->z) / (2 * s);

  return sqrt(((s - a) * (s - b) * (s - c)) / s);
}

real_t triangle_compute_a_exradius(triangle_t *tri)
{
  real_t s, a, b, c;

  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;

  return sqrt((s * (s - b) * (s - c)) / (s - a));
}

real_t triangle_compute_a_excircle(point_t *excenter, triangle_t *tri)
{
  real_t s, a, b, c;

  assert(excenter);
  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;

  excenter->x = (-a * tri->a->x + b * tri->b->x + c * tri->c->x) / (-a + b + c);
  excenter->y = (-a * tri->a->y + b * tri->b->y + c * tri->c->y) / (-a + b + c);
  excenter->z = (-a * tri->a->z + b * tri->b->z + c * tri->c->z) / (-a + b + c);

  return sqrt((s * (s - b) * (s - c)) / (s - a));
}

real_t triangle_compute_b_exradius(triangle_t *tri)
{
  real_t s, a, b, c;

  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;

  return sqrt((s * (s - a) * (s - c)) / (s - b));
}

real_t triangle_compute_b_excircle(point_t *excenter, triangle_t *tri)
{
  real_t s, a, b, c;

  assert(excenter);
  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;

  excenter->x = (a * tri->a->x - b * tri->b->x + c * tri->c->x) / (a - b + c);
  excenter->y = (a * tri->a->y - b * tri->b->y + c * tri->c->y) / (a - b + c);
  excenter->z = (a * tri->a->z - b * tri->b->z + c * tri->c->z) / (a - b + c);

  return sqrt((s * (s - a) * (s - c)) / (s - b));
}

real_t triangle_compute_c_exradius(triangle_t *tri)
{
  real_t s, a, b, c;

  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;

  return sqrt((s * (s - a) * (s - b)) / (s - c));
}

real_t triangle_compute_c_excircle(point_t *excenter, triangle_t *tri)
{
  real_t s, a, b, c;

  assert(excenter);
  assert(tri);

  a = get_distance_of_p2p(tri->b, tri->c);
  b = get_distance_of_p2p(tri->c, tri->a);
  c = get_distance_of_p2p(tri->a, tri->b);
  s = (a + b + c) / 2;

  excenter->x = (a * tri->a->x + b * tri->b->x - c * tri->c->x) / (a + b - c);
  excenter->y = (a * tri->a->y + b * tri->b->y - c * tri->c->y) / (a + b - c);
  excenter->z = (a * tri->a->z + b * tri->b->z - c * tri->c->z) / (a + b - c);

  return sqrt((s * (s - a) * (s - b)) / (s - c));
}

void triangle_clear(triangle_t *tri)
{
  assert(tri);

  if (tri->a) {
    point_destroy(tri->a);
    tri->a = NULL;
  }

  if (tri->b) {
    point_destroy(tri->b);
    tri->b = NULL;
  }

  if (tri->c) {
    point_destroy(tri->c);
    tri->c = NULL;
  }
}

void triangle_destroy(triangle_t *tri)
{
  assert(tri);

  if (triangle_get_ref(tri) <= 0) {
    if (tri->a) point_destroy(tri->a);
    if (tri->b) point_destroy(tri->b);
    if (tri->c) point_destroy(tri->c);
    free(tri);
  } else {
    tri->reference--;
  }
}
