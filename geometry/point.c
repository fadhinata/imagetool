#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include <common.h>
#include <geometry/point.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

real_t arctan2r(real_t dy, real_t dx)
{
  real_t ang = atan2r(dy, dx);
  return ang < 0.0 ? ang + 2 * M_PI : ang;
}

point_t *point_new(void)
{
  point_t *point;

  point = (point_t *)malloc(sizeof(point_t));
  assert(point);

  memset(point, 0, sizeof(point_t));

  return point;
}

/*
void point_assign(point_t *point, real_t x, real_t y, real_t z)
{
  assert(point);

  point->x = x;
  point->y = y;
  point->z = z;
}
*/

point_t *point_new_and_set(real_t x, real_t y, real_t z)
{
  point_t *point;

  point = point_new();
  point_assign(point, x, y, z);

  return point;
}

void point_copy(point_t *copyer, point_t *copee)
{
  assert(copyer);
  assert(copee);

  copyer->x = copee->x;
  copyer->y = copee->y;
  copyer->z = copee->z;
}

int point_cmp(point_t *a, point_t *b)
{
  assert(a);
  assert(b);

  if ((abs(a->x - b->x) < REAL_EPSILON) &&
      (abs(a->y - b->y) < REAL_EPSILON) &&
      (abs(a->z - b->z) < REAL_EPSILON))
    return 0;

  return -1;
}

point_t *point_new_and_copy(point_t *copee)
{
  point_t *copyer;

  assert(copee);

  copyer = point_new();
  point_copy(copyer, copee);

  return copyer;
}

void point_dump(point_t *point)
{
  assert(point);

#ifdef REAL_TYPE_AS_DOUBLE
  printf("point: (%.2lf,%.2lf,%.2lf)\n", point->x, point->y, point->z);
#elif REAL_TYPE_AS_FLOAT
  printf("point: (%.2f,%.2f,%.2f)\n", point->x, point->y, point->z);
#else
  # error "Undefined real_t!: data-type"
#endif
}

void point_destroy(point_t *point)
{
  assert(point);

  if (point->reference > 0) point->reference--;
  else free(point);
}

void point_move(point_t *p, real_t dx, real_t dy, real_t dz)
{
  assert(p);

  p->x += dx;
  p->y += dy;
  p->z += dz;
}

void point_add(point_t *c, point_t *a, point_t *b)
{
  assert(c);
  assert(a);
  assert(b);

  c->x = a->x + b->x;
  c->y = a->y + b->y;
  c->z = a->z + b->z;
}

void point_subtract(point_t *c, point_t *a, point_t *b)
{
  assert(c);
  assert(a);
  assert(b);

  c->x = a->x - b->x;
  c->y = a->y - b->y;
  c->z = a->z - b->z;
}

real_t point_get_distance(point_t *point)
{
  real_t dist;

  assert(point);

  dist = sqrt(sqr(point->x) + sqr(point->y) + sqr(point->z));

  return dist;
}

real_t point_dotproduct(point_t *a, point_t *b)
{
  assert(a);
  assert(b);

  return (a->x * b->x + a->y * b->y + a->z * b->z);
}

void point_xproduct(point_t *c, point_t *a, point_t *b)
{
  assert(c);
  assert(a);
  assert(b);

  c->x = (a->y * b->z) - (a->z * b->y);
  c->y = (a->z * b->x) - (a->x * b->z);
  c->z = (a->x * b->y) - (a->y * b->x);
}

real_t get_area_of_parallelogram(point_t *a, point_t *o, point_t *b)
{
  real_t area;
  point_t *p, *q, *r;

  assert(a);
  assert(o);
  assert(b);

  p = point_new();
  q = point_new();
  r = point_new();

  point_subtract(p, a, o);
  point_subtract(q, b, o);
  point_xproduct(r, p, q);

  area = point_get_distance(r);

  point_destroy(r);
  point_destroy(q);
  point_destroy(p);

  return area;
}

/*
real_t parallelogram_area_of_3points(point_t *a, point_t *o, point_t *b)
{
  real_t area;
  point_t *p, *q, *r;

  assert(a);
  assert(o);
  assert(b);

  p = point_new();
  q = point_new();
  r = point_new();

  point_subtract(p, a, o);
  point_subtract(q, b, o);
  point_xproduct(r, p, q);

  area = point_get_distance(r);

  point_destroy(r);
  point_destroy(q);
  point_destroy(p);

  return area;
}
*/

real_t get_distance_of_p2p(point_t *a, point_t *b)
{
  real_t dist;
  point_t *ab;

  assert(a);
  assert(b);

  ab = point_new();
  point_subtract(ab, b, a);
  dist = point_get_distance(ab);
  point_destroy(ab);

  return dist;
}

/*
real_t distance_between_points(point_t *a, point_t *b)
{
  real_t dist;
  point_t *ab;

  assert(a);
  assert(b);

  ab = point_new();
  point_subtract(ab, b, a);
  dist = point_get_distance(ab);
  point_destroy(ab);

  return dist;
}
*/

/* By Heron's formula, the area of the triangle
   sqrt(s*(s-a)*(s-b)*(s-c)), s: (a+b+c)/2 */
real_t get_area_of_three_points(point_t *A, point_t *B, point_t *C)
{
  real_t a, b, c, s;

  assert(A);
  assert(B);
  assert(C);

  a = get_distance_of_p2p(B, C);
  b = get_distance_of_p2p(C, A);
  c = get_distance_of_p2p(A, B);
  s = (a + b + c) / 2;

  return sqrt(s * (s - a) * (s - b) * (s - c));
}

#if 0
// return value: [0, pi]
real_t angle_between_points(point_t *a, point_t *b)
{
  real_t rad;

  assert(a);
  assert(b);

  rad = acosr(point_dotproduct(a, b) / (point_get_distance(a) * point_get_distance(b)));

  return rad;
}

real_t angle_among_points(point_t *a, point_t *o, point_t *b)
{
  real_t rad;
  point_t *p, *q;

  assert(a);
  assert(o);
  assert(b);

  p = point_new();
  q = point_new();

  point_subtract(p, a, o);
  point_subtract(q, b, o);

  rad = angle_between_points(p, q);

  point_destroy(q);
  point_destroy(p);

  return rad;
}
#endif

