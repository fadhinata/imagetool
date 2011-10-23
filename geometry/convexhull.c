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
#include <math.h>
#include <malloc.h>
#include <assert.h>

#include <buffering/dlink.h>
#include <geometry/point_list.h>
#include <geometry/polygon.h>
#include <geometry/convexhull.h>

extern void keyhit(void);
extern void putline(int x1, int y1, int x2, int y2, int r, int g, int b);
extern void circle(int x, int y, int radius, int r, int g, int b);

/* http://www.cs.princeton.edu/~ah/alg_anim/version2/ConvexHull.html */
/*
 * Jarvis' March
 * This is perhaps the most simple-minded algorithm for the convex hull,
 * and yet in some cases it can be very fast. The basic idea is as follows: 
 * 1. Start at some extreme point, which is guaranteed to be on the hull. 
 * 2. At each step, test each of the points,
 *    and find the one which makes the largest right-hand turn.
 *    That point has to be the next one on the hull. 
 */
 /* some problem is occurred due to co-linear pixels,
  * so it need to be check the maximum distance within same minimum angle
  * comment : if break statement in the Right chain growth routine is converted
  * from "if (ycoord[imin] < ycoord[i-1]) break;"
  * to "if (ycoord[imin] <= ycoord[i-1]) break;"
  * , then some problem is removed.
  * However, if an point exits 
  * such that is resemble to "ycoord" of the extreme point
  * and has the minimum angle
  * then the convex hull that we want don't be created.
  */
static int jarvismarch(polygon_t *convexhull, point_list_t *points)
{
  real_t ang, dist, ymin;
  real_t amin, dmax, dx, dy;
  dlink_t *a, *b, *c;
  point_t *p, *q;

  assert(convexhull);
  assert(points);
  assert(points->count >= 3);

  // preparing the polygon, and
  // find the point with minimum value of y-coordinate
  for (c = NULL, a = points->tail->next; a != points->head; a = a->next) {
    p = (point_t *)a->object;
    p->reference++, b = dlink_new(), b->object = (void *)p;
    dlist_insert(b, (dlist_t *)convexhull);
    if ((c == NULL) || (p->y < ymin)) { c = b; ymin = p->y; }
  }
  dlink_cutoff(c);
  convexhull->count--;
  dlist_push(c, convexhull);
  point_dump((point_t *)c->object);
  //printf("n: %d\n", convexhull->count);

  // scan right chain
  for (a = convexhull->tail->next; a->next != convexhull->head; a = a->next) {
    p = (point_t *)a->object;
    for (c = NULL, b = a->next; b != convexhull->head; b = b->next) {
      q = (point_t *)b->object;
      dx = q->x - p->x;
      dy = q->y - p->y;
      ang = arctan2r(dy, dx);
      dist = sqrt(sqr(dx) + sqr(dy));
      // find another vertex with min-angle and max-distance
      if ((c == NULL) || (ang < amin)) {
	c = b;
	amin = ang;
	dmax = dist;
      } else if ((ang == amin) && (dist > dmax)) {
	// if the pixel is co-linear with previewous pixel with minimum angle
	c = b;
	dmax = dist;
      }
    }
    q = (point_t *)c->object;
    // right chain complete ?
    if (q->y < p->y) break;
    // swapping (coordinations and mark)
    dlink_cutoff(c);
    dlink_append(c, a);
    point_dump((point_t *)c->object);
  }
  //printf("n: %d\n", convexhull->count);

  // scan left chain
  for (; a->next != convexhull->head; a = a->next) {
    p = (point_t *)a->object;
    for (c = NULL, b = a->next; b != convexhull->head; b = b->next) {
      q = (point_t *)b->object;
      dx = p->x - q->x;
      dy = p->y - q->y;
      ang = arctan2r(dy, dx);
      dist = sqrt(sqr(dx) + sqr(dy));
      if ((c == NULL) || (ang < amin)) {
	c = b;
	amin = ang;
	dmax = dist;
      } else if ((ang == amin) && (dist > dmax)) {
	c = b;
	dmax = dist;
      }
    }
    b = convexhull->tail->next;
    q = (point_t *)b->object;
    dx = p->x - q->x;
    dy = p->y - q->y;
    ang = arctan2r(dy, dx);
    // convexhull complete ?
    if (ang < amin) break;
    // swapping
    dlink_cutoff(c);
    dlink_append(c, a);
    point_dump((point_t *)c->object);
  }
  //printf("n: %d\n", convexhull->count);

  while (a->next != convexhull->head) {
    b = a->next;
    dlink_cutoff(b);
    convexhull->count--;
    //((point_t *)b->object)->reference--;
    point_destroy((point_t *)b->object);
    dlink_destroy(b);
  }

  return convexhull->count;
}

/* Graham's Scan
 * Given a set of points on the plane, Graham's scan computes their convex hull.
 * The algorithm works in three phases: 
 * 1. Find an extreme point.
 *    This point will be the pivot, is guaranteed to be on the hull,
 *    and is chosen to be the point with largest y coordinate. 
 * 2. Sort the points in order of increasing angle about the pivot.
 *    We end up with a star-shaped polygon (one in which one special point,
 *    in this case the pivot, can "see" the whole polygon). 
 * 3. Build the hull, by marching around the star-shaped poly, adding edges
 *    when we make a left turn, and back-tracking when we make a right turn. 
 */
static int grahamscan(polygon_t *convexhull, point_list_t *points)
{
  int i;
  real_t dx, dy, ymin;
  real_t *ang;
  point_t *p, *q;
  point_t *v1, *v2, *v3;
  dlink_t *a, *b, *c, *temp;
	
  assert(convexhull);
  assert(points);
  assert(points->count >= 3);
	
  ang = (real_t *)malloc(points->count * sizeof(*ang));
  assert(ang);

  // find an extreme point
  // preparing the polygon, and
  // find the point with minimum value of y-coordinate
  for (c = NULL, a = points->tail->next; a != points->head; a = a->next) {
    p = (point_t *)a->object;
    p->reference++, b = dlink_new(), b->object = (void *)p;
    dlist_insert(b, (dlist_t *)convexhull);
    if ((c == NULL) || (p->y < ymin)) { c = b; ymin = p->y; }
  }
  dlink_cutoff(c), convexhull->count--;
  dlist_push(c, convexhull);

  // sort the points in order of increasing angle about the pivot.
  p = (point_t *)c->object; i = 0;
  //point_dump(p);
  for (a = c->next; a != convexhull->head; a = a->next) {
    q = (point_t *)a->object;
    dx = q->x - p->x;
    dy = q->y - p->y;
    ang[i] = arctan2r(dy, dx);
    a->spare = (void *)&ang[i];
    i++;
    //point_dump(q);
    //printf("ang: %lf\n", ang[i-1]);
  }
  for (a = c->next; a->next != convexhull->head; a = a->next) {
    for (b = a->next; b != convexhull->head; b = b->next) {
      if (*((real_t *)b->spare) < *((real_t *)a->spare)) {
	dlink_exchange(a, b);
	temp = a, a = b, b = temp;
      }
    }
  }
  //point_dump((point_t *)c->object);

  v1 = point_new();
  v2 = point_new();
  v3 = point_new();
  c = convexhull->tail->next->next->next;
  while (c != convexhull->head) {
    a = c->prev->prev;
    b = c->prev;
    point_subtract(v1, (point_t *)a->object, (point_t *)b->object);
    point_subtract(v2, (point_t *)c->object, (point_t *)b->object);
    point_xproduct(v3, v1, v2);
    // convex ?
    if (v3->z < 0) {
      c = c->next;
    } else {
      dlink_cutoff(b);
      convexhull->count--;
      //((point_t *)b->object)->reference--
      point_destroy((point_t *)b->object);
      dlink_destroy(b);
    }
  }
  for (a = convexhull->tail->next; a != convexhull->head; a = a->next) {
    a->spare = NULL;
  }
  point_destroy(v3);
  point_destroy(v2);
  point_destroy(v1);

  free(ang);

  return convexhull->count;
}

/*
 * Quick-Hull
 * Here's an algorithm that deserves its name.
 * It's a fast way to compute the convex hull of a set of points on the plane.
 * It shares a few similarities with its namesake, quick-sort: 
 * - it is recursive.
 * - each recursive step partitions data into several groups.
 *
 * The partitioning step does all the work. The basic idea is as follows:
 * 1. We are given a some points, 
 *    and line segment AB which we know is a chord of the convex hull. 
 * 2. Among the given points, find the one which is farthest from AB.
 *    Let's call this point C.
 * 3. The points inside the triangle ABC cannot be on the hull.
 *    Put them in set s0.
 * 4. Put the points which lie outside edge AC in set s1,
 * and points outside edge BC in set s2.
 *
 * Once the partitioning is done, we recursively invoke quick-hull on sets s1 and s2.
 * The algorithm works fast on random sets of points 
 * because step 3 of the partition typically discards a large fraction of the points. 
 */
static int quickhull_grouping(dlist_t *group)
{
  dlink_t *p, *a, *b, *c, *next;
  dlist_t *s1, *s2;
  point_t *v1, *v2, *v3;
  real_t area;

  assert(group);

  if (group->count <= 2) return group->count;

  v1 = point_new();
  v2 = point_new();
  v3 = point_new();

  // find the point with maximum parallelogram's area
  a = dlist_pop(group);
  b = dlist_extract(group);
  c = NULL;
  point_subtract(v2, (point_t *)(b->object), (point_t *)(a->object));
  for (p = group->tail->next; p != group->head; p = p->next) {
    point_subtract(v1, (point_t *)(p->object), (point_t *)(a->object));
    point_xproduct(v3, v1, v2);
    if ((c == NULL) || (v3->z > area)) { c = p; area = v3->z; }
  }
  dlink_cutoff(c); group->count--;

  // s1 grouping
  s1 = dlist_new();
  dlist_insert(a, s1);
  point_subtract(v2, (point_t *)(c->object), (point_t *)(a->object));
  for (p = group->tail->next; p != group->head;) {
    point_subtract(v1, (point_t *)(p->object), (point_t *)(a->object));
    point_xproduct(v3, v1, v2);
    if (v3->z > 0) {
      next = p->next;
      dlink_cutoff(p); group->count--;
      dlist_insert(p, s1);
      p = next;
    } else p = p->next;
  }
  dlist_insert(c, s1);
  quickhull_grouping(s1);
  assert(c == s1->head->prev);

  // s2 grouping and pop out others
  c = dlist_extract(s1);
  s2 = dlist_new();
  dlist_insert(c, s2);
  point_subtract(v2, (point_t *)b->object, (point_t *)c->object);
  for (p = group->tail->next; p != group->head;) {
    point_subtract(v1, (point_t *)p->object, (point_t *)c->object);
    point_xproduct(v3, v1, v2);
    if (v3->z > 0) {
      next = p->next;
      dlink_cutoff(p); group->count--;
      dlist_insert(p, s2);
      p = next;
    } else {
      next = p->next;
      dlink_cutoff(p); group->count--;
      //((point_t *)p->object)->reference--;
      point_destroy((point_t *)p->object);
      dlink_destroy(p);
      p = next;
    }
  }
  dlist_insert(b, s2);
  quickhull_grouping(s2);
  assert(b == s2->head->prev);

  assert(group->count == 0);

  // merge s1 and s2 into group
  while (s1->count) {
    p = dlist_pop(s1);
    dlist_insert(p, group);
  }

  while (s2->count) {
    p = dlist_pop(s2);
    dlist_insert(p, group);
  }

  dlist_destroy(s2);
  dlist_destroy(s1);

  point_destroy(v3);
  point_destroy(v2);
  point_destroy(v1);

  return group->count;
}

static int quickhull(polygon_t *convexhull, point_list_t *points)
{
  real_t ymin, ymax, y;
  point_t *p, *v1, *v2, *v3;
  dlink_t *a, *b, *c, *next;
  dlist_t *right, *left;

  assert(convexhull);
  assert(points);

  // allocate the structure element of convex hull
  for (a = points->tail->next; a != points->head; a = a->next) {
    p = (point_t *)a->object;
    p->reference++;
    b = dlink_new();
    b->object = (void *)p;
    dlist_insert(b, convexhull);
  }

  // find the extreme points along y-axis
  a = NULL;
  b = NULL;
  for (c = convexhull->tail->next; c != convexhull->head; c = c->next) {
    y = ((point_t *)(c->object))->y;
    if ((a == NULL) || (y < ymin)) { a = c; ymin = y; }
    if ((b == NULL) || (y > ymax)) { b = c; ymax = y; }
  }
  dlink_cutoff(a); convexhull->count--;
  dlink_cutoff(b); convexhull->count--;
  //point_dump((point_t *)a->object);
  //point_dump((point_t *)b->object);

  v1 = point_new();
  v2 = point_new();
  v3 = point_new();

  //printf("for right section\n");
  right = dlist_new();
  dlist_insert(a, right);
  point_subtract(v2, (point_t *)b->object, (point_t *)a->object);
  for (c = convexhull->tail->next; c != convexhull->head;) {
    //point_dump((point_t *)c->object);
    point_subtract(v1, (point_t *)c->object, (point_t *)a->object);
    point_xproduct(v3, v1, v2);
    if (v3->z > 0) {
      next = c->next;
      dlink_cutoff(c); convexhull->count--;
      dlist_insert(c, right);
      //printf(" ");
      //point_dump((point_t *)c->object);
      c = next;
    } else c = c->next;
  }
  dlist_insert(b, right);
  quickhull_grouping(right);

  //printf("for left section\n");
  a = dlist_pop(right);
  b = dlist_extract(right);
  //point_dump((point_t *)a->object);
  //point_dump((point_t *)b->object);

  left = dlist_new();
  dlist_insert(b, left);
  point_subtract(v2, (point_t *)a->object, (point_t *)b->object);
  for (c = convexhull->tail->next; c != convexhull->head;) {
    point_subtract(v1, (point_t *)c->object, (point_t *)b->object);
    point_xproduct(v3, v1, v2);
    if (v3->z > 0) {
      next = c->next;
      dlink_cutoff(c); convexhull->count--;
      dlist_insert(c, left);
      //point_dump((point_t *)c->object);
      c = next;
    } else {
      next = c->next;
      dlink_cutoff(c); convexhull->count--;
      point_destroy((point_t *)c->object);
      dlink_destroy(c);
      c = next;
    }
  }
  dlist_insert(a, left);
  quickhull_grouping(left);

  a = dlist_extract(left);
  b = dlist_pop(left);

  dlist_insert(a, convexhull);
  while (right->count) {
    c = dlist_pop(right);
    dlist_insert(c, convexhull);
  }
  dlist_insert(b, convexhull);
  while (left->count) {
    c = dlist_pop(left);
    dlist_insert(c, convexhull);
  }

  dlist_destroy(left);
  dlist_destroy(right);

  point_destroy(v3);
  point_destroy(v2);
  point_destroy(v1);

  return convexhull->count;
}

int convexhull_compute(polygon_t *convexhull, point_list_t *points, convexhull_method_t way)
{
  int ret;

  assert(convexhull);
  assert(points);
  assert(points->count >= 3);

  switch (way) {
  case JAVIS_MARCH:
    ret = jarvismarch(convexhull, points);
    break;
  case GRAHAM_SCAN:
    ret = grahamscan(convexhull, points);
    break;
  case QUICK_HULL:
    ret = quickhull(convexhull, points);
    break;
  }
  return ret;
}

static real_t distance_from_point_to_line(point_t *p, line_t *line)
{
  real_t dist;
  point_t *q, *r;

  assert(p);
  assert(line);
  q = line->a;
  r = line->b;
  dist = (abs((r->x - q->x) * (q->y - p->y) - (q->x - p->x) * (r->y - q->y)) /
	  (sqrt(sqr(r->x - q->x) + sqr(r->y - q->y))));
  return dist;
}

/* http://valis.cs.uiuc.edu/~sariel/research/CG/applets/bounding_rectangle/main.html */
/*
 * First, we define "antipodal points" to be two points
 * (that belong to the convex hull)
 * thru which we can draw two parallel lines
 * which don't intersect the polygon (beside those two point).
 * It is clear that the most distant dots will be antipodal.
 * Now, we'll check all of the antipodal pairs of points ( O(n) ).
 * For each pair we'll find the distance between the two points,
 * and so we'll find the maximum distance.
 * We go through the pairs in the following way:
 * 1. The first pair will consist of the leftmost and the rightmost points.
 *    We draw two vertical lines thru them.
 * 2. Now, at each step, we rotate the parallel lines in the minimal angle
 *    which changes the pair of antipodal points that are defined by the lines.
 * 3. We finish when we complete a whole round.
 */
real_t convexhull_diameter(line_t *diameter, polygon_t *convexhull)
{
  int i;
  real_t *ang, dist, temp, rotate;
  real_t alpha, beta, da, db, xmin, xmax;
  dlink_t *a, *b, *c, *left, *right;
  point_t *p, *q;

  assert(diameter);
  assert(convexhull);
  assert(convexhull->count >= 3);

  ang = (real_t *)malloc(convexhull->count * sizeof(*ang));

  // find the angle between verteces
  i = 0;
  a = convexhull->head->prev;
  b = convexhull->tail->next;
  do {
    p = (point_t *)a->object;
    q = (point_t *)b->object;
    ang[i] = arctan2r(q->y - p->y, q->x - p->x);
    ang[i] = ang[i] > M_PI ? ang[i]-M_PI : ang[i];
    a->spare = (void *)&ang[i];
    printf("(%lf,%lf)->(%lf,%lf):%lf\n", p->x, p->y, q->x, q->y, ang[i] * 180 / M_PI);
    i++;
    a = b;
    b = b->next;
  } while (b != convexhull->head);
  printf("Initialized the angle between verteces\n");

  // find the most left and right point
  left = NULL;
  right = NULL;
  for (c = convexhull->tail->next; c != convexhull->head; c = c->next) {
    p = (point_t *)c->object;
    if ((left == NULL) || (p->x < xmin)) { left = c; xmin = p->x; }
    if ((right == NULL) || (p->x > xmax)) { right = c; xmax = p->x; }
  }
  printf("Founded the leftmost and rightmost points\n");

  a = left; b = right; dist = 0; p = NULL; q = NULL;
  rotate = M_PI/2;

  do {
    printf("a: (%lf,%lf) -> b: (%lf,%lf)\n", ((point_t *)a->object)->x, ((point_t *)a->object)->y, ((point_t *)b->object)->x, ((point_t *)b->object)->y);

    temp = distance_between_points((point_t *)a->object, (point_t *)b->object);
    if (temp > dist) {
      p = (point_t *)a->object;
      q = (point_t *)b->object;
      dist = temp;
    }

    alpha = *((real_t *)a->spare);
    beta = *((real_t *)b->spare);
    /*
    printf("rotate: %lf, alpha: %lf, beta: %lf\n", rotate*180/M_PI, alpha*180/M_PI, beta*180/M_PI);

    circle(((point_t *)a->object)->x, ((point_t *)a->object)->y, 10, 255, 0, 0);
    circle(((point_t *)b->object)->x, ((point_t *)b->object)->y, 10, 255, 0, 0);
    keyhit();
    circle(((point_t *)a->object)->x, ((point_t *)a->object)->y, 10, 0, 0, 0);
    circle(((point_t *)b->object)->x, ((point_t *)b->object)->y, 10, 0, 0, 0);
    */
    if (alpha >= rotate) da = alpha - rotate;
    else da = M_PI + alpha - rotate;
    if (beta >= rotate) db = beta - rotate;
    else db = M_PI + beta - rotate;

    if (da < db) {
      rotate = alpha;
      if (a->next == convexhull->head) a = convexhull->tail->next;
      else a = a->next;
    } else {
      rotate = beta;
      if (b->next == convexhull->head) b = convexhull->tail->next;
      else b = b->next;
    }
  } while (!(((a == right) && (b == left)) || ((b == right) && (a == left))));

  for (c = convexhull->tail->next; c != convexhull->head; c = c->next) {
    c->spare = NULL;
  }

  line_assign(diameter, p, q);

  free(ang);

  return dist;
}

real_t convexhull_weak_diameter(line_t *diameter, polygon_t *convexhull)
{
  real_t low, high, dist;
  real_t temp, temp1, temp2, temp3, temp4;
  dlink_t *a, *b;
  dlink_t *prev_a, *next_b;
  point_t *p, *q, *r, *u, *v;

  assert(diameter);
  assert(convexhull);

  r = point_new();
  u = NULL;
  v = NULL;
  dist = 0;
  for (a = convexhull->tail->next; a->next != convexhull->head; a = a->next) {
    if (a->prev == convexhull->tail) prev_a = convexhull->head->prev;
    else prev_a = a->prev;
    p = (point_t *)a->object;
    for (b = a->next; b != convexhull->head; b = b->next) {
      if (b->next == convexhull->head) next_b = convexhull->tail->next;
      else next_b = b->next;
      q = (point_t *)b->object;
      point_subtract(r, q, p);
      low = point_dotproduct(p, r);
      high = point_dotproduct(q, r);
      if (low > high) { temp = low; low = high; high = temp; }
      temp1 = point_dotproduct((point_t *)a->next->object, r);
      temp2 = point_dotproduct((point_t *)prev_a->object, r);
      temp3 = point_dotproduct((point_t *)next_b->object, r);
      temp4 = point_dotproduct((point_t *)b->prev->object, r);
      if (((low <= temp1) && (temp1 <= high)) &&
	  ((low <= temp2) && (temp2 <= high)) &&
	  ((low <= temp3) && (temp3 <= high)) &&
	  ((low <= temp4) && (temp4 <= high))) {
	temp = distance_between_points(p, q);
	/*
	circle(p->x, p->y, 10, 255, 0, 0);
	circle(q->x, q->y, 10, 255, 0, 0);
	keyhit();
	circle(p->x, p->y, 10, 0, 0, 0);
	circle(q->x, q->y, 10, 0, 0, 0);
	*/
	if (temp > dist) {
	  u = p; v = q;
	  dist = temp;
	}
      }
    }
  }
  line_assign(diameter, u, v);
  //line_list_destroy(line_list);
  point_destroy(r);
  return dist;
}

/* 
 * Minimum Width
 * The minimum width is the distance between the closest parallel lines
 * between which all of the point are placed.
 * To find the minimal width,
 * we'll go through a sequence of pairs of parallel lines the hold all of the points between them.
 * In each pair, one of the lines must coincide with a polygon edge.
 * The other line will be the most distant line that still touches the polygon
 * (There are at most n pairs). 
 * 1. For each pair, we'll find the distance between the two lines,
 *    and the width is the minimal distance.
 * 2. We'll find the pairs in the following way:
 * 3. The first pair will consist of two vertical lines that
 *    go thru the most left and the most right points.
 * 4. Then, for each pair, (exactly like in the diameter)
 *    we rotate the two parallel lines in the angle that
 *    move one line to lie on a new edge. 
 */
real_t convexhull_min_width(line_t *width, polygon_t *convexhull)
{
  int i;
  real_t *ang, dist, temp, rotate;
  real_t da, db, alpha, beta, xmin, xmax;
  dlink_t *a, *next_a;
  dlink_t *b, *next_b;
  dlink_t *c, *left, *right;
  point_t *p, *q;
  line_t *line;

  assert(width);
  assert(convexhull);
  assert(convexhull->count >= 3);

  ang = (real_t *)malloc(convexhull->count * sizeof(*ang));

  // find the angle between verteces
  i = 0;
  a = convexhull->head->prev;
  b = convexhull->tail->next;
  do {
    p = (point_t *)a->object;
    q = (point_t *)b->object;
    ang[i] = arctan2r(q->y - p->y, q->x - p->x);
    ang[i] = ang[i] > M_PI ? ang[i]-M_PI : ang[i];
    a->spare = (void *)&ang[i];
    printf("(%lf,%lf)->(%lf,%lf):%lf\n", p->x, p->y, q->x, q->y, ang[i] * 180 / M_PI);
    i++;
    a = b;
    b = b->next;
  } while (b != convexhull->head);
  printf("Initialized the angle between verteces\n");

  // find the most left and right point
  left = NULL;
  right = NULL;
  for (c = convexhull->tail->next; c != convexhull->head; c = c->next) {
    p = (point_t *)c->object;
    if ((left == NULL) || (p->x < xmin)) { left = c; xmin = p->x; }
    if ((right == NULL) || (p->x > xmax)) { right = c; xmax = p->x; }
  }
  printf("Founded the leftmost and rightmost points\n");

  a = left; b = right; dist = 0; p = NULL; q = NULL;
  rotate = M_PI / 2;
  do {
    printf("a: (%lf,%lf) -> b: (%lf,%lf)\n", ((point_t *)a->object)->x, ((point_t *)a->object)->y, ((point_t *)b->object)->x, ((point_t *)b->object)->y);

    temp = distance_between_points((point_t *)a->object, (point_t *)b->object);
    if ((p == NULL) || (q == NULL) || (temp < dist)) {
      p = (point_t *)a->object;
      q = (point_t *)b->object;
      dist = temp;
    }

    alpha = *((real_t *)a->spare);
    beta = *((real_t *)b->spare);

    printf("rotate: %lf, alpha: %lf, beta: %lf\n", rotate*180/M_PI, alpha*180/M_PI, beta*180/M_PI);
    /*
    circle(((point_t *)a->object)->x, ((point_t *)a->object)->y, 10, 255, 0, 0);
    circle(((point_t *)b->object)->x, ((point_t *)b->object)->y, 10, 255, 0, 0);
    keyhit();
    circle(((point_t *)a->object)->x, ((point_t *)a->object)->y, 10, 0, 0, 0);
    circle(((point_t *)b->object)->x, ((point_t *)b->object)->y, 10, 0, 0, 0);
    */
    if (alpha >= rotate) da = alpha - rotate;
    else da = M_PI + alpha - rotate;
    if (beta >= rotate) db = beta - rotate;
    else db = M_PI + beta - rotate;

    if (da < db) {
      rotate = alpha;
      if (a->next == convexhull->head) next_a = convexhull->tail->next;
      else next_a = a->next;
      line = line_new_and_assign((point_t *)a->object, (point_t *)next_a->object);
      temp = distance_from_point_to_line((point_t *)b->object, line);
      if ((p == NULL) || (q == NULL) || (temp < dist)) {
	p = (point_t *)a->object; q = (point_t *)b->object;
	dist = temp;
      }
      line_destroy(line);
      a = next_a;
    } else {
      rotate = beta;
      if (b->next == convexhull->head) next_b = convexhull->tail->next;
      else next_b = b->next;
      line = line_new_and_assign((point_t *)b->object, (point_t *)next_b->object);
      temp = distance_from_point_to_line((point_t *)a->object, line);
      if ((p == NULL) || (q == NULL) || (temp < dist)) {
	p = (point_t *)b->object; q = (point_t *)a->object;
	dist = temp;
      }
      line_destroy(line);
      b = next_b;
    }
  } while (!(((a == right) && (b == left)) || ((b == right) && (a == left))));

  for (c = convexhull->tail->next; c != convexhull->head; c = c->next) {
    c->spare = NULL;
  }

  line_assign(width, p, q);

  free(ang);

  return dist;
}

/*
 * By computing a formula, deriving and finding the minimum,
 * one can prove that
 * the minimum rects will always have at least one side that lies on a polygon edge. 
 * In order to find the minimum area or circumference blocking rectangule,
 * we'll examine a sequence of such rectangles.
 * The basic idea is to start with a bounding rectangle and to rotate it at each step,
 * so we'll change only one point from the polygon's points that lie on the rect.
 * We do that by calculating the angles for each point
 * between the rect side that lies on it and the next polygon edge (clockwise).
 * We rotate the rect in the minimal angle we've calculated.
 * The first rect will lie on the most right, most left, most up and most down points. 
 * For each rect we'll calculate the area and the circumference. This way we can find the minimum.
 */
static void convexhull_create_circumference_rectangles(dlist_t *circumrects, polygon_t *convexhull)
{
  int i, irotate;
  real_t *angle, rectangle[4], delta;
  real_t rotate, dmin, slope, yslope, ortho, yortho, temp;
  real_t xmin, xmax, ymin, ymax, area;
  point_t *p, *q, *r, *s;
  dlink_t *left, *right, *top, *bottom;
  dlink_t *cross_vertex[4], *start, *pivot, *pivot_next;
  dlink_t *a, *b;
  polygon_t *rect;

  assert(circumrects);
  assert(convexhull);
  assert(convexhull->count >= 4);

  // Finding angle of edge of convexhull
  angle = (real_t *)malloc(convexhull->count * sizeof(*angle));

  i = 0;
  a = convexhull->head->prev; b = convexhull->tail->next;
  do {
    p = (point_t *)a->object;
    q = (point_t *)b->object;
    angle[i] = arctan2r(q->y - p->y, q->x - p->x);
    //angle[i] = angle[i] > M_PI ? angle[i]-M_PI : angle[i];
    a->spare = (void *)&angle[i];
    printf("(%lf,%lf)->(%lf,%lf):%lf\n", p->x, p->y, q->x, q->y, angle[i] * 180 / M_PI);
    i++;
    a = b;
    b = b->next;
  } while (b != convexhull->head);
  printf("Initialized the angle between verteces\n");
  
  // Finding four extreme points
  left = NULL; right = NULL; top = NULL; bottom = NULL;
  for (a = convexhull->tail->next; a != convexhull->head; a = a->next) {
    p = (point_t *)a->object;
    if ((left == NULL) || (p->x < xmin)) { left = a; xmin = p->x; }
    if ((right == NULL) || (p->x > xmax)) { right = a; xmax = p->x; }
    if ((bottom == NULL) || (p->y < ymin)) { bottom = a; ymin = p->y; }
    if ((top == NULL) || (p->y > ymax)) { top = a; ymax = p->y; }
  }
  cross_vertex[0] = bottom;
  cross_vertex[1] = right;
  cross_vertex[2] = top;
  cross_vertex[3] = left;

  area = (xmax - xmin) * (ymax - ymin);

  // Forming the starting rectangle from four extreme points
  rect = polygon_new();
  polygon_insert(point_new_and_assign(xmin, ymin, 0), rect);
  polygon_insert(point_new_and_assign(xmax, ymin, 0), rect);
  polygon_insert(point_new_and_assign(xmax, ymax, 0), rect);
  polygon_insert(point_new_and_assign(xmin, ymax, 0), rect);

  // Angle of four edge of the starting rectangle
  rectangle[0] = 0;
  rectangle[1] = M_PI / 2;
  rectangle[2] = M_PI;
  rectangle[3] = 3 * M_PI / 2;
  for (i = 0, a = rect->tail->next; a != rect->head; a = a->next) {
    a->spare = (void *)&rectangle[i];
    i++;
  }

  a = dlink_new();
  a->object = (void *)polygon_new_and_copy(rect);
  dlist_insert(a, circumrects);
  /*
  putline(xmin, ymin, xmax, ymin, 255, 0, 0);
  putline(xmax, ymin, xmax, ymax, 255, 0, 0);
  putline(xmax, ymax, xmin, ymax, 255, 0, 0);
  putline(xmin, ymax, xmin, ymin, 255, 0, 0);
  */
  // Difference between angle of edge of rectangle and edge of convexhull
  start = NULL;
  for (i = 0, a = rect->tail->next; a != rect->head; a = a->next) {
    delta = (*((real_t *)cross_vertex[i]->spare) - *((real_t *)a->spare) + 2 * M_PI);
    if (delta >= 2 * M_PI) delta -= 2 * M_PI;
    if ((start == NULL) || (delta < dmin)) {
      start = cross_vertex[i];
      dmin = delta;
      irotate = i;
    }
    i++;
  }
  /*
  circle(((point_t *)start->object)->x, ((point_t *)start->object)->y, 10, 255, 0, 0);
  keyhit();
  circle(((point_t *)start->object)->x, ((point_t *)start->object)->y, 10, 0, 0, 0);
  putline(xmin, ymin, xmax, ymin, 0, 0, 0);
  putline(xmax, ymin, xmax, ymax, 0, 0, 0);
  putline(xmax, ymax, xmin, ymax, 0, 0, 0);
  putline(xmin, ymax, xmin, ymin, 0, 0, 0);
  */
  rotate = 0;
  pivot = start;
  do {
    if (pivot->next == convexhull->head) pivot_next = convexhull->tail->next;
    else pivot_next = pivot->next;

    p = (point_t *)pivot->object;
    q = (point_t *)pivot_next->object;

    if ((p->x == q->x) || (p->y == q->y)) { // for 0 or 90 or 180 degree
      // nothing
    } else { // for others
      // update rectangluar by pivot edge for rotation
      slope = (q->y - p->y) / (q->x - p->x);
      yslope = q->y - slope * q->x;

      r = (point_t *)cross_vertex[(irotate + 1) % 4]->object;
      ortho = -1 / slope;
      yortho = r->y - ortho * r->x;

      s = polygon_glimpse(irotate, rect);
      s->x = -1 * (yslope - yortho) / (slope - ortho);
      s->y = slope * s->x + yslope;

      temp = slope; slope = ortho; ortho = temp;
      yslope = yortho;
      r = (point_t *)cross_vertex[(irotate + 2) % 4]->object;
      yortho = r->y - ortho * r->x;

      s = polygon_glimpse((irotate + 1) % 4, rect);
      s->x = -1 * (yslope - yortho) / (slope - ortho);
      s->y = slope * s->x + yslope;

      temp = slope; slope = ortho; ortho = temp;
      yslope = yortho;
      r = (point_t *)cross_vertex[(irotate + 3) % 4]->object;
      yortho = r->y - ortho * r->x;

      s = polygon_glimpse((irotate + 2) % 4, rect);
      s->x = -1 * (yslope - yortho) / (slope - ortho);
      s->y = slope * s->x + yslope;
      
      temp = slope; slope = ortho; ortho = temp;
      yslope = yortho;
      r = (point_t *)cross_vertex[irotate]->object;
      yortho = r->y - ortho * r->x;

      s = polygon_glimpse((irotate + 3) % 4, rect);
      s->x = -1 * (yslope - yortho) / (slope - ortho);
      s->y = slope * s->x + yslope;

      rotate += dmin;
      for (a = rect->tail->next; a != rect->head; a = a->next) {
	*((real_t *)a->spare) += dmin;
	if (*((real_t *)a->spare) >= 2 * M_PI)
	  *((real_t *)a->spare) -= 2 * M_PI;
      }
      p = polygon_glimpse(0, rect);
      q = polygon_glimpse(1, rect);
      r = polygon_glimpse(2, rect);
      s = polygon_glimpse(3, rect);

      a = dlink_new();
      a->object = (void *)polygon_new_and_copy(rect);
      dlist_insert(a, circumrects);

      temp = distance_between_points(p, q) * distance_between_points(q, r);
      if (temp < area) area = temp;
    }

    // update crossing verteces between rectangle and convexhull
    cross_vertex[irotate] = pivot_next;

    pivot = NULL;
    for (i = 0, a = rect->tail->next; a != rect->head; a = a->next) {
      delta = (*((real_t *)cross_vertex[i]->spare) - *((real_t *)a->spare) + 2 * M_PI);
      if (delta >= 2 * M_PI) delta -= 2 * M_PI;
      if ((pivot == NULL) || (delta < dmin)) {
	pivot = cross_vertex[i];
	dmin = delta;
	irotate = i;
      }
      i++;
    }

    p = polygon_glimpse(0, rect);
    q = polygon_glimpse(1, rect);
    r = polygon_glimpse(2, rect);
    s = polygon_glimpse(3, rect);
    /*
    putline(p->x, p->y, q->x, q->y, 255, 0, 0);
    putline(q->x, q->y, r->x, r->y, 255, 0, 0);
    putline(r->x, r->y, s->x, s->y, 255, 0, 0);
    putline(s->x, s->y, p->x, p->y, 255, 0, 0);

    circle(((point_t *)pivot->object)->x, ((point_t *)pivot->object)->y, 10, 255, 0, 0);
    keyhit();

    circle(((point_t *)pivot->object)->x, ((point_t *)pivot->object)->y, 10, 0, 0, 0);
    putline(p->x, p->y, q->x, q->y, 0, 0, 0);
    putline(q->x, q->y, r->x, r->y, 0, 0, 0);
    putline(r->x, r->y, s->x, s->y, 0, 0, 0);
    putline(s->x, s->y, p->x, p->y, 0, 0, 0);
    */
  } while (pivot != start);

  point_destroy(polygon_pop(rect));
  point_destroy(polygon_pop(rect));
  point_destroy(polygon_pop(rect));
  point_destroy(polygon_pop(rect));
  polygon_destroy(rect);

  free(angle);
}

real_t convexhull_create_min_area_rectangle(polygon_t *minrect, polygon_t *convexhull)
{
  real_t area, temp;
  dlist_t *circumrects;
  polygon_t *rect;
  point_t *p, *q, *r;
  dlink_t *a, *b;

  assert(minrect);
  //assert(minrect->count == 4);
  assert(convexhull);
  assert(convexhull->count >= 4);

  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);

  circumrects = dlist_new();

  convexhull_create_circumference_rectangles(circumrects, convexhull);

  b = NULL;
  for (a = circumrects->tail->next; a != circumrects->head; a = a->next) {
    rect = (polygon_t *)a->object;
    p = polygon_glimpse(0, rect);
    q = polygon_glimpse(1, rect);
    r = polygon_glimpse(2, rect);
    temp = distance_between_points(p, q) * distance_between_points(q, r);
    if ((b == NULL) || (temp < area)) {
      b = a;
      area = temp;
    }
  }
  /*
  dlink_cutoff(b);
  circumrects->count--;
  minrect = (polygon_t *)b->object;
  minrect->reference--;
  dlink_destroy(b);
  */

  polygon_copy(minrect, (polygon_t *)b->object);

  while (circumrects->count) {
    a = dlist_pop(circumrects);
    rect = (polygon_t *)a->object;
    while (rect->count) {
      p = polygon_pop(rect);
      point_destroy(p);
    }
    dlink_destroy(a);
  }
  dlist_destroy(circumrects);

  return area;
}

real_t convexhull_create_min_perimeter_rectangle(polygon_t *minrect, polygon_t *convexhull)
{
  real_t perimeter, temp;
  dlist_t *circumrects;
  polygon_t *rect;
  point_t *p, *q, *r;
  dlink_t *a, *b;

  assert(minrect);
  //  assert(minrect->count == 4);
  assert(convexhull);
  assert(convexhull->count >= 4);

  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);

  circumrects = dlist_new();

  convexhull_create_circumference_rectangles(circumrects, convexhull);

  b = NULL;
  for (a = circumrects->tail->next; a != circumrects->head; a = a->next) {
    rect = (polygon_t *)a->object;
    p = polygon_glimpse(0, rect);
    q = polygon_glimpse(1, rect);
    r = polygon_glimpse(2, rect);
    temp = 2 * (distance_between_points(p, q) + distance_between_points(q, r));
    if ((b == NULL) || (temp < perimeter)) {
      b = a;
      perimeter = temp;
    }
  }
  /*
  dlink_cutoff(b);
  circumrects->count--;
  minrect = (polygon_t *)b->object;
  minrect->reference--;
  dlink_destroy(b);
  */
  polygon_copy(minrect, (polygon_t *)b->object);

  while (circumrects->count) {
    a = dlist_pop(circumrects);
    rect = (polygon_t *)a->object;
    while (rect->count) {
      p = polygon_pop(rect);
      point_destroy(p);
    }
    dlink_destroy(a);
  }

  dlist_destroy(circumrects);

  return perimeter;
}
