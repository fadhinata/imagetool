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
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <assert.h>

#include <buffering/dlink.h>
#include <geometry/point_list.h>
#include <geometry/polygon.h>
#include <geometry/convexhull.h>

/*
extern void keyhit(void);
extern void putline(int x1, int y1, int x2, int y2, int r, int g, int b);
extern void circle(int x, int y, int radius, int r, int g, int b);
*/

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
static int jarvismarch(polygon_t *chull, point_list_t *points)
{
  real_t ang, dist, ymin;
  real_t amin, dmax, dx, dy;
  dlink_t *x, *y, *ax, *bx;
  point_t *p, *q;

  assert(chull);
  assert(points);
  assert(point_list_get_count(points) >= 3);

  // Preparing the polygon, and
  // Find the point with minimum value of y-coordinate
  for (ax = NULL, x = points->tail->next; x != points->head; x = x->next) {
    p = (point_t *)x->object;
    y = dlink_new();
    point_inc_ref(p);
    y->object = (void *)p;
    dlist_insert(y, chull);
    if (ax == NULL || point_get_y(p) < ymin) {
      ax = y;
      ymin = point_get_y(p);
    }
  }
  dlink_cutoff(ax);
  dlist_dec_count(chull);
  dlist_push(ax, chull);
  point_dump((point_t *)ax->object);

  // Scan right chain
  for (ax = chull->tail->next; ax->next != chull->head; ax = ax->next) {
    p = (point_t *)ax->object;
    for (bx = NULL, x = ax->next; x != chull->head; x = x->next) {
      q = (point_t *)x->object;
      dx = point_get_x(q) - point_get_x(p);
      dy = point_get_y(q) - point_get_y(p);
      ang = arctan2r(dy, dx);
      dist = sqrt(sqr(dx) + sqr(dy));
      // Find another vertex with min-angle and max-distance
      if (bx == NULL || ang < amin) {
	bx = x;
	amin = ang;
	dmax = dist;
      } else if (ang == amin && dist > dmax) {
	bx = x;
	dmax = dist;
      }
    }
    q = (point_t *)bx->object;

    // Right chain complete ?
    if (point_get_y(q) < point_get_y(p))
      break;

    // Swapping (coordinations and mark)
    dlink_cutoff(bx);
    dlink_append(bx, ax);
    point_dump((point_t *)bx->object);
  }
  //printf("n: %d\n", convexhull->count);

  // Scan left chain
  for (; ax->next != chull->head; ax = ax->next) {
    p = (point_t *)ax->object;
    for (bx = NULL, x = ax->next; x != chull->head; x = x->next) {
      q = (point_t *)x->object;
      dx = point_get_x(p) - point_get_x(q);
      dy = point_get_y(p) - point_get_y(q);
      ang = arctan2r(dy, dx);
      dist = sqrt(sqr(dx) + sqr(dy));
      if (bx == NULL || ang < amin) {
	bx = x;
	amin = ang;
	dmax = dist;
      } else if (ang == amin && dist > dmax) {
	bx = x;
	dmax = dist;
      }
    }

    y = chull->tail->next;
    q = (point_t *)y->object;
    dx = point_get_x(p) - point_get_x(q);
    dy = point_get_y(p) - point_get_y(q);
    ang = arctan2r(dy, dx);
    // Convexhull complete ?
    if (ang < amin) break;

    // Swapping
    dlink_cutoff(bx);
    dlink_append(bx, ax);
    point_dump((point_t *)bx->object);
  }
  //printf("n: %d\n", convexhull->count);

  while (ax->next != chull->head) {
    x = ax->next;
    dlink_cutoff(x);
    dlist_dec_count(chull);
    point_destroy((point_t *)x->object);
    dlink_destroy(x);
  }

  return dlist_get_count(chull);
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
static int grahamscan(polygon_t *chull, point_list_t *points)
{
  int i;
  real_t dx, dy, ymin;
  real_t *ang;
  point_t *p, *q;
  point_t *v1, *v2, *v3;
  dlink_t *ax, *bx, *cx, *x, *y, *tmp;
	
  assert(chull);
  assert(points);
  assert(point_list_get_count(points) >= 3);
	
  ang = (real_t *)malloc(point_list_get_count(points) * sizeof(real_t));
  assert(ang);

  // Find an extreme point
  // Preparing the polygon, and
  // Find the point with minimum value of y-coordinate
  for (ax = NULL, x = points->tail->next; x != points->head; x = x->next) {
    p = (point_t *)x->object;
    y = dlink_new();
    point_inc_ref(p);
    y->object = (void *)p;
    dlist_insert(y, chull);
    if (ax == NULL || point_get_y(p) < ymin) {
      ax = y;
      ymin = point_get_y(p);
    }
  }
  dlink_cutoff(ax);
  dlist_dec_count(chull);
  dlist_push(ax, chull);

  // Sort the points in order of increasing angle about the pivot.
  p = (point_t *)ax->object;
  //point_dump(p);
  for (i = 0, x = ax->next; x != chull->head; x = x->next) {
    q = (point_t *)x->object;
    dx = point_get_x(q) - point_get_x(p);
    dy = point_get_y(q) - point_get_y(p);
    ang[i] = arctan2r(dy, dx);
    x->spare = (void *)&(ang[i]);
    i++;
    //point_dump(q);
    //printf("ang: %lf\n", ang[i-1]);
  }

  for (x = ax->next; x->next != chull->head; x = x->next) {
    for (y = x->next; y != chull->head; y = y->next) {
      if (*((real_t *)y->spare) < *((real_t *)x->spare)) {
	dlink_exchange(x, y);
	tmp = x, x = y, y = tmp;
      }
    }
  }
  //point_dump((point_t *)c->object);

  v1 = point_new();
  v2 = point_new();
  v3 = point_new();

  cx = chull->tail->next->next->next;
  while (cx != chull->head) {
    bx = cx->prev;
    ax = bx->prev;

    point_subtract(v1, (point_t *)ax->object, (point_t *)bx->object);
    point_subtract(v2, (point_t *)cx->object, (point_t *)bx->object);
    point_xproduct(v3, v1, v2);

    // Convex ?
    if (point_get_z(v3) < 0) {
      cx = cx->next;
    } else {
      dlink_cutoff(bx);
      dlist_dec_count(chull);
      point_destroy((point_t *)bx->object);
      dlink_destroy(bx);
    }
  }

  for (x = chull->tail->next; x != chull->head; x = x->next)
    x->spare = NULL;

  point_destroy(v3);
  point_destroy(v2);
  point_destroy(v1);

  free(ang);

  return dlist_get_count(chull);
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
  dlink_t *x, *ax, *bx, *cx, *next;
  dlist_t *s1, *s2;
  point_t *v1, *v2, *v3;
  real_t area;

  assert(group);

  if (dlist_get_count(group) <= 2)
    return dlist_get_count(group);

  v1 = point_new();
  v2 = point_new();
  v3 = point_new();

  // Find the point with maximum parallelogram's area
  ax = dlist_pop(group);
  bx = dlist_extract(group);
  cx = NULL;

  point_subtract(v2, (point_t *)(bx->object), (point_t *)(ax->object));
  for (x = group->tail->next; x != group->head; x = x->next) {
    point_subtract(v1, (point_t *)(x->object), (point_t *)(ax->object));
    point_xproduct(v3, v1, v2);
    if (cx == NULL || point_get_z(v3) > area) {
      cx = x;
      area = point_get_z(v3);
    }
  }
  dlink_cutoff(cx);
  dlist_dec_count(group);

  // S1 grouping
  s1 = dlist_new();
  dlist_insert(ax, s1);
  point_subtract(v2, (point_t *)(cx->object), (point_t *)(ax->object));
  for (x = group->tail->next; x != group->head; ) {
    point_subtract(v1, (point_t *)(x->object), (point_t *)(ax->object));
    point_xproduct(v3, v1, v2);
    if (point_get_z(v3) > 0) {
      next = x->next;
      dlink_cutoff(x);
      dlist_dec_count(group);
      dlist_insert(x, s1);
      x = next;
    } else x = x->next;
  }
  dlist_insert(cx, s1);
  quickhull_grouping(s1);
  assert(cx == s1->head->prev);

  // S2 grouping and pop out others
  cx = dlist_extract(s1);
  s2 = dlist_new();
  dlist_insert(cx, s2);
  point_subtract(v2, (point_t *)bx->object, (point_t *)cx->object);
  for (x = group->tail->next; x != group->head;) {
    point_subtract(v1, (point_t *)x->object, (point_t *)cx->object);
    point_xproduct(v3, v1, v2);
    if (point_get_z(v3) > 0) {
      next = x->next;
      dlink_cutoff(x);
      dlist_dec_count(group);
      dlist_insert(x, s2);
      x = next;
    } else {
      next = x->next;
      dlink_cutoff(x);
      dlist_dec_count(group);
      point_destroy((point_t *)x->object);
      dlink_destroy(x);
      x = next;
    }
  }
  dlist_insert(bx, s2);
  quickhull_grouping(s2);
  assert(bx == s2->head->prev);

  assert(dlist_get_count(group) == 0);
  //assert(group->count == 0);

  // Merge s1 and s2 into group
  while (dlist_get_count(s1) > 0) {
    x = dlist_pop(s1);
    dlist_insert(x, group);
  }

  while (dlist_get_count(s2) > 0) {
    x = dlist_pop(s2);
    dlist_insert(x, group);
  }

  dlist_destroy(s2);
  dlist_destroy(s1);

  point_destroy(v3);
  point_destroy(v2);
  point_destroy(v1);

  return dlist_get_count(group);
}

static int quickhull(polygon_t *chull, point_list_t *points)
{
  real_t ymin, ymax, yval;
  point_t *p, *v1, *v2, *v3;
  dlink_t *ax, *bx, *x, *y, *next;
  dlist_t *right_group, *left_group;

  assert(chull);
  assert(points);

  // Allocate the structure element of convex hull
  for (x = points->tail->next; x != points->head; x = x->next) {
    p = (point_t *)x->object;
    point_inc_ref(p);
    y = dlink_new();
    y->object = (void *)p;
    dlist_insert(y, chull);
  }

  // find the extreme points along y-axis
  ax = NULL;
  bx = NULL;
  for (x = chull->tail->next; x != chull->head; x = x->next) {
    yval = point_get_y((point_t *)(x->object));
    if (ax == NULL || yval < ymin) {
      ax = x;
      ymin = yval;
    }
    if (bx == NULL || yval > ymax) {
      bx = x;
      ymax = yval;
    }
  }
  dlink_cutoff(ax);
  dlist_dec_count(chull);
  dlink_cutoff(bx);
  dlist_dec_count(chull);
  //point_dump((point_t *)ax->object);
  //point_dump((point_t *)bx->object);

  v1 = point_new();
  v2 = point_new();
  v3 = point_new();

  //printf("for right section\n");
  right_group= dlist_new();
  dlist_insert(ax, right_group);
  point_subtract(v2, (point_t *)bx->object, (point_t *)ax->object);
  for (x = chull->tail->next; x != chull->head;) {
    //point_dump((point_t *)x->object);
    point_subtract(v1, (point_t *)x->object, (point_t *)ax->object);
    point_xproduct(v3, v1, v2);
    if (point_get_z(v3) > 0) {
      next = x->next;
      dlink_cutoff(x);
      dlist_dec_count(chull);
      dlist_insert(x, right_group);
      //printf(" ");
      //point_dump((point_t *)x->object);
      x = next;
    } else x = x->next;
  }
  dlist_insert(bx, right_group);
  quickhull_grouping(right_group);

  //printf("for left section\n");
  ax = dlist_pop(right_group);
  bx = dlist_extract(right_group);
  //point_dump((point_t *)ax->object);
  //point_dump((point_t *)bx->object);

  left_group = dlist_new();
  dlist_insert(bx, left_group);
  point_subtract(v2, (point_t *)ax->object, (point_t *)bx->object);
  for (x = chull->tail->next; x != chull->head; ) {
    point_subtract(v1, (point_t *)x->object, (point_t *)bx->object);
    point_xproduct(v3, v1, v2);
    if (point_get_z(v3) > 0) {
      next = x->next;
      dlink_cutoff(x);
      dlist_dec_count(chull);
      dlist_insert(x, left_group);
      //point_dump((point_t *)x->object);
      x = next;
    } else {
      next = x->next;
      dlink_cutoff(x);
      dlist_dec_count(chull);
      point_destroy((point_t *)x->object);
      dlink_destroy(x);
      x = next;
    }
  }
  dlist_insert(ax, left_group);
  quickhull_grouping(left_group);

  ax = dlist_extract(left_group);
  bx = dlist_pop(left_group);

  dlist_insert(ax, chull);
  while (dlist_get_count(right_group) > 0) {
    x = dlist_pop(right_group);
    dlist_insert(x, chull);
  }
  dlist_insert(bx, chull);

  while (dlist_get_count(left_group) > 0) {
    x = dlist_pop(left_group);
    dlist_insert(x, chull);
  }

  dlist_destroy(left_group);
  dlist_destroy(right_group);

  point_destroy(v3);
  point_destroy(v2);
  point_destroy(v1);

  return dlist_get_count(chull);
}

int convexhulling(polygon_t *chull, point_list_t *points, convexhull_method_t way)
{
  int cnt;

  assert(chull);
  assert(points);
  assert(point_list_get_count(points));

  switch (way) {
  case JAVIS_MARCH: cnt = jarvismarch(chull, points); break;
  case GRAHAM_SCAN: cnt = grahamscan(chull, points); break;
  case QUICK_HULL: cnt = quickhull(chull, points); break;
  default: abort(); break;
  }

  return cnt;
}

static real_t distance_from_point_to_line(point_t *p, line_t *line)
{
  real_t len, dist;
  //point_t *q, *r;
  point_t a, b;

  assert(p);
  assert(line);

  switch (line->form_type) {
  case LINE_BY_ENDPOINTS:
    point_copy(&a, line_get_a(line));
    point_copy(&b, line_get_b(line));
    break;
  case LINE_BY_ENDPOINT_DIRECTION:
    point_copy(&a, line_get_a(line));
    point_add(&b, line_get_a(line), line_get_b(line));
    break;
  case LINE_BY_POSITION_DIRECTION:
    point_copy(&a, line_get_a(line));
    point_add(&b, line_get_a(line), line_get_b(line));
    break;
  default:
    abort();
    break;
  }

  len = sqrt(sqr(point_get_x(&b) - point_get_x(&a)) +
	     sqr(point_get_y(&b) - point_get_y(&a)));

  dist = abs((point_get_x(&b) - point_get_x(&a)) *
	     (point_get_y(&a) - point_get_y(p)) -
	     (point_get_x(&a) - point_get_x(p)) *
	     (point_get_y(&b) - point_get_y(&a)));

  return dist / len;
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
real_t convexhull_compute_diameter(line_t *diameter, polygon_t *chull)
{
  int i;
  real_t *ang, dist, temp, rotate;
  real_t alpha, beta, da, db, xmin, xmax;
  dlink_t *ax, *bx, *cx, *x, *lx, *rx;
  point_t *a, *b, *p, *q;

  assert(diameter);
  assert(chull);
  assert(polygon_get_count(chull) >= 3);

  ang = (real_t *)malloc(polygon_get_count(chull) * sizeof(real_t));
  assert(ang);

  // Find the angle between verteces
  i = 0;
  ax = chull->head->prev;
  bx = chull->tail->next;
  do {
    a = (point_t *)ax->object;
    b = (point_t *)bx->object;
    ang[i] = arctan2r(point_get_y(b) - point_get_y(a),
		      point_get_x(b) - point_get_x(a));
    ang[i] = ang[i] > M_PI ? ang[i] - M_PI : ang[i];
    ax->spare = (void *)&ang[i];
    printf("(%lf,%lf) -> (%lf,%lf) < %lf deg\n",
	   point_get_x(a), point_get_y(a),
	   point_get_x(b), point_get_y(b),
	   ang[i] * 180 / M_PI);
    i++;
    ax = bx;
    bx = bx->next;
  } while (bx != chull->head);
  printf("Initialized the angle between verteces\n");

  // Find the most left and right point
  lx = NULL;
  rx = NULL;
  for (x = chull->tail->next; x != chull->head; x = x->next) {
    p = (point_t *)x->object;
    if (lx == NULL || point_get_x(p) < xmin) {
      lx = x;
      xmin = point_get_x(p);
    }
    if (rx == NULL || point_get_x(p) > xmax) {
      rx = x;
      xmax = point_get_x(p);
    }
  }
  printf("Founded the leftmost and rightmost points\n");

  ax = lx;
  bx = rx;
  dist = 0;
  rotate = M_PI_2;

  do {
    a = (point_t *)ax->object;
    b = (point_t *)bx->object;

    printf("a: (%lf,%lf) -> b: (%lf,%lf)\n",
	   point_get_x(a), point_get_y(a),
	   point_get_x(b), point_get_y(b));

    temp = get_distance_of_p2p(a, b);
    if (temp > dist) {
      dist = temp;
      p = a;
      q = b;
    }

    alpha = *((real_t *)ax->spare);
    beta = *((real_t *)bx->spare);

    if (alpha >= rotate) da = alpha - rotate;
    else da = M_PI + alpha - rotate;

    if (beta >= rotate) db = beta - rotate;
    else db = M_PI + beta - rotate;

    if (da < db) {
      rotate = alpha;
      if (ax->next == chull->head) ax = chull->tail->next;
      else ax = ax->next;
    } else {
      rotate = beta;
      if (bx->next == chull->head) bx = chull->tail->next;
      else bx = bx->next;
    }
  } while (!((ax == rx && bx == lx) || (bx == rx && ax == lx)));

  for (x = chull->tail->next; x != chull->head; x = x->next)
    x->spare = NULL;

  line_set_endpoints(diameter, p, q);

  free(ang);

  return dist;
}

real_t convexhull_compute_weak_diameter(line_t *diameter, polygon_t *chull)
{
  real_t low, high, dist;
  real_t temp, temp1, temp2, temp3, temp4;
  dlink_t *ax, *bx, *ax_prev, *bx_next;
  point_t *a, *b, p, *u, *v, *r;

  assert(diameter);
  assert(chull);

  r = point_new();
  u = NULL;
  v = NULL;
  dist = 0;

  for (ax = chull->tail->next; ax->next != chull->head; ax = ax->next) {
    if (ax->prev == chull->tail) ax_prev = chull->head->prev;
    else ax_prev = ax->prev;
    a = (point_t *)ax->object;

    for (bx = ax->next; bx != chull->head; bx = bx->next) {
      if (bx->next == chull->head) bx_next = chull->tail->next;
      else bx_next = bx->next;
      b = (point_t *)bx->object;

      point_subtract(&p, b, a);
      low = point_dotproduct(a, &p);
      high = point_dotproduct(b, &p);

      if (low > high) {
	temp = low;
	low = high;
	high = temp;
      }

      temp1 = point_dotproduct((point_t *)ax->next->object, &p);
      temp2 = point_dotproduct((point_t *)ax_prev->object, &p);
      temp3 = point_dotproduct((point_t *)bx_next->object, &p);
      temp4 = point_dotproduct((point_t *)bx->prev->object, &p);

      if ((low <= temp1 && temp1 <= high) &&
	  (low <= temp2 && temp2 <= high) &&
	  (low <= temp3 && temp3 <= high) &&
	  (low <= temp4 && temp4 <= high)) {
	temp = get_distance_of_p2p(a, b);
	/*
	circle(p->x, p->y, 10, 255, 0, 0);
	circle(q->x, q->y, 10, 255, 0, 0);
	keyhit();
	circle(p->x, p->y, 10, 0, 0, 0);
	circle(q->x, q->y, 10, 0, 0, 0);
	*/
	if (temp > dist) {
	  u = a; v = b;
	  dist = temp;
	}
      }
    }
  }

  line_set_endpoints(diameter, u, v);

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
real_t convexhull_compute_min_width(line_t *width, polygon_t *chull)
{
  int i;
  real_t *ang, dist, value, rotate;
  real_t da, db, alpha, beta, xmin, xmax;
  dlink_t *ax, *ax_next;
  dlink_t *bx, *bx_next;
  dlink_t *x, *lx, *rx;
  point_t *a, *b, *p, *q;
  line_t *line;

  assert(width);
  assert(chull);
  assert(polygon_get_count(chull) >= 3);

  ang = (real_t *)malloc(polygon_get_count(chull) * sizeof(real_t));
  assert(ang);

  // Find the angle between verteces
  i = 0;
  ax = chull->head->prev;
  bx = chull->tail->next;
  do {
    a = (point_t *)ax->object;
    b = (point_t *)bx->object;

    ang[i] = arctan2r(point_get_y(b) - point_get_y(a),
		      point_get_x(b) - point_get_x(a));
    ang[i] = ang[i] > M_PI ? ang[i] - M_PI : ang[i];

    ax->spare = (void *)&ang[i];
    printf("(%lf,%lf) -> (%lf,%lf) < %lf deg\n",
	   point_get_x(a), point_get_y(a),
	   point_get_x(b), point_get_y(b),
	   ang[i] * 180 / M_PI);

    i++;
    ax = bx;
    bx = bx->next;
  } while (bx != chull->head);
  printf("Initialized the angle between verteces\n");

  // Find the most left and right point
  lx = NULL;
  rx = NULL;
  for (x = chull->tail->next; x != chull->head; x = x->next) {
    p = (point_t *)x->object;
    if (lx == NULL || point_get_x(p) < xmin) {
      lx = x;
      xmin = point_get_x(p);
    }
    if (rx == NULL || point_get_x(p) > xmax) {
      rx = x;
      xmax = point_get_x(p);
    }
  }
  printf("Founded the leftmost and rightmost points\n");

  ax = lx;
  bx = rx;
  dist = 0.0;
  p = NULL;
  q = NULL;
  rotate = M_PI_2; // pi / 2;
  line = line_new();

  do {
    a = (point_t *)ax->object;
    b = (point_t *)bx->object;
    printf("(%lf,%lf) -> (%lf,%lf)\n",
	   point_get_x(a), point_get_y(a),
	   point_get_x(b), point_get_y(b));

    value = get_distance_of_p2p(a, b);
    if (p == NULL || q == NULL || value < dist) {
      p = a;
      q = b;
      dist = value;
    }

    alpha = *((real_t *)ax->spare);
    beta = *((real_t *)bx->spare);

    printf("rotate: %lf, alpha: %lf, beta: %lf\n",
	   rotate * 180 / M_PI, alpha * 180 / M_PI, beta * 180 / M_PI);
    /*
    circle(((point_t *)a->object)->x, ((point_t *)a->object)->y, 10, 255, 0, 0);
    circle(((point_t *)b->object)->x, ((point_t *)b->object)->y, 10, 255, 0, 0);
    keyhit();
    circle(((point_t *)a->object)->x, ((point_t *)a->object)->y, 10, 0, 0, 0);
    circle(((point_t *)b->object)->x, ((point_t *)b->object)->y, 10, 0, 0, 0);
    */

    // Make alpha and beta in the range of acute angle
    if (alpha >= rotate) da = alpha - rotate;
    else da = M_PI + alpha - rotate;

    if (beta >= rotate) db = beta - rotate;
    else db = M_PI + beta - rotate;

    if (da < db) {
      rotate = alpha;
      if (ax->next == chull->head) ax_next = chull->tail->next;
      else ax_next -= ax->next;

      line_set_endpoints(line, a, (point_t *)ax_next->object);

      value = distance_from_point_to_line(b, line);
      if (p == NULL || q == NULL || value < dist) {
	p = a;
	q = b;
	dist = value;
      }
      line_clear(line);
      ax = ax_next;
    } else {
      rotate = beta;
      if (bx->next == chull->head) bx_next = chull->tail->next;
      else bx_next = bx->next;
      
      line_set_endpoints(line, b, (point_t *)bx_next->object);
      
      value = distance_from_point_to_line(a, line);
      if (p == NULL || q == NULL || value < dist) {
	p = b;
	q = a;
	dist = value;
      }
      line_clear(line);
      bx = bx_next;
    }
  } while (!((ax == rx && bx == lx) || (bx == rx && ax == lx)));

  for (x = chull->tail->next; x != chull->head; x = x->next)
    x->spare = NULL;

  line_set_endpoints(width, p, q);
  //line_assign(width, p, q);

  free(ang);

  return dist;
}

static void convexhull_delete_circumference_rectangles(dlist_t *circumrects)
{
  dlink_t *a;

  assert(circumrects);

  while (circumrects->count > 0) {
    a = dlist_extract(circumrects);
    polygon_destroy((polygon_t *)a->object);
    dlink_destroy(a);
  }
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
static void convexhull_create_circumference_rectangles(dlist_t *circumrects, polygon_t *chull)
{
  int i, irotate;
  real_t *angle, rectangle[4], delta;
  real_t rotate, dmin, slope, yslope, ortho, yortho, temp;
  real_t xmin, xmax, ymin, ymax, area;
  point_t *p, *q, *r, *s;
  dlink_t *left, *right, *top, *bottom;
  dlink_t *cross_vertex[4], *start, *pivot, *pivot_next;
  dlink_t *ax, *bx;
  polygon_t *rect;

  assert(circumrects);
  assert(chull);
  assert(polygon_get_count(chull) >= 3);

  // Finding angle of edge of convexhull
  angle = (real_t *)malloc(polygon_get_count(chull) * sizeof(real_t));
  assert(angle);

  i = 0;
  ax = chull->head->prev;
  bx = chull->tail->next;
  do {
    p = (point_t *)ax->object;
    q = (point_t *)bx->object;
    angle[i] = arctan2r(point_get_y(q) - point_get_y(p),
			point_get_x(q) - point_get_x(p));
    //angle[i] = angle[i] > M_PI ? angle[i]-M_PI : angle[i];
    ax->spare = (void *)&angle[i];
    printf("(%lf,%lf)->(%lf,%lf):%lf\n",
	   point_get_x(p), point_get_y(p),
	   point_get_x(q), point_get_y(q),
	   angle[i] * 180 / M_PI);
    i++;
    ax = bx;
    bx = bx->next;
  } while (bx != chull->head);
  printf("Initialized the angle between verteces\n");
  
  // Finding four extreme points
  left = NULL;
  right = NULL;
  top = NULL;
  bottom = NULL;
  for (ax = chull->tail->next; ax != chull->head; ax = ax->next) {
    p = (point_t *)ax->object;
    if (left == NULL || point_get_x(p) < xmin) {
      left = ax;
      xmin = point_get_x(p);
    }
    if (right == NULL || point_get_x(p) > xmax) {
      right = ax;
      xmax = point_get_x(p);
    }
    if (bottom == NULL || point_get_y(p) < ymin) {
      bottom = ax;
      ymin = point_get_y(p);
    }
    if (top == NULL || point_get_y(p) > ymax) {
      top = ax;
      ymax = point_get_y(p);
    }
  }
  cross_vertex[0] = bottom;
  cross_vertex[1] = right;
  cross_vertex[2] = top;
  cross_vertex[3] = left;
  area = (xmax - xmin) * (ymax - ymin);

  // Forming the starting rectangle from four extreme points
  rect = polygon_new();
  polygon_insert(point_new_and_set(xmin, ymin, 0), rect);
  polygon_insert(point_new_and_set(xmax, ymin, 0), rect);
  polygon_insert(point_new_and_set(xmax, ymax, 0), rect);
  polygon_insert(point_new_and_set(xmin, ymax, 0), rect);

  // Angle of four edge of the starting rectangle
  rectangle[0] = 0;
  rectangle[1] = M_PI_2;
  rectangle[2] = M_PI;
  rectangle[3] = M_PI + M_PI_2;
  for (i = 0, ax = rect->tail->next; ax != rect->head; ax = ax->next) {
    ax->spare = (void *)&rectangle[i];
    i++;
  }

  ax = dlink_new();
  ax->object = (void *)polygon_new_and_copy(rect);
  dlist_insert(ax, circumrects);

  // Difference between angle of edge of rectangle and edge of convexhull
  start = NULL;
  for (i = 0, ax = rect->tail->next; ax != rect->head; ax = ax->next) {
    delta = (*((real_t *)cross_vertex[i]->spare) - *((real_t *)ax->spare) + 2 * M_PI);
    if (delta >= 2 * M_PI) delta -= 2 * M_PI;
    if ((start == NULL) || (delta < dmin)) {
      start = cross_vertex[i];
      dmin = delta;
      irotate = i;
    }
    i++;
  }

  rotate = 0;
  pivot = start;
  do {
    if (pivot->next == chull->head) pivot_next = chull->tail->next;
    else pivot_next = pivot->next;

    p = (point_t *)pivot->object;
    q = (point_t *)pivot_next->object;

    if (point_get_x(p) == point_get_x(q) || point_get_y(p) == point_get_y(q)) {
      // For 0 or 90 or 180 degree
      // Nothing
    } else {
      // For others
      // Update rectangluar by pivot edge for rotation

      // Finding parameters of extending line between p and q
      /////////////////////////////////////////////////////////
      slope = (point_get_y(q) - point_get_y(p)) /
	(point_get_x(q) - point_get_x(p));
      yslope = point_get_y(q) - slope * point_get_x(q);

      r = (point_t *)cross_vertex[(irotate + 1) % 4]->object;
      ortho = -1 / slope;
      yortho = point_get_y(r) - ortho * point_get_x(r);
      
      s = polygon_glimpse(irotate, rect);
      point_put_x(-1 * (yslope - yortho) / (slope - ortho), s);
      point_put_y(slope * point_get_x(s) + yslope, s);

      /////////////////////////////////////////////////////////
      temp = slope;
      slope = ortho;
      ortho = temp;
      yslope = yortho;
      r = (point_t *)cross_vertex[(irotate + 2) % 4]->object;
      yortho = point_get_y(r) - ortho * point_get_x(r);

      s = polygon_glimpse((irotate + 1) % 4, rect);
      point_put_x(-1 * (yslope - yortho) / (slope - ortho), s);
      point_put_y(slope * point_get_x(s) + yslope, s);

      /////////////////////////////////////////////////////////
      temp = slope;
      slope = ortho;
      ortho = temp;
      yslope = yortho;
      r = (point_t *)cross_vertex[(irotate + 3) % 4]->object;
      yortho = point_get_y(r) - ortho * point_get_x(r);

      s = polygon_glimpse((irotate + 2) % 4, rect);
      point_put_x(-1 * (yslope - yortho) / (slope - ortho), s);
      point_put_y(slope * point_get_x(s) + yslope, s);

      /////////////////////////////////////////////////////////
      temp = slope;
      slope = ortho;
      ortho = temp;
      yslope = yortho;
      r = (point_t *)cross_vertex[irotate]->object;
      yortho = point_get_y(r) - ortho * point_get_x(r);

      s = polygon_glimpse((irotate + 3) % 4, rect);
      point_put_x(-1 * (yslope - yortho) / (slope - ortho), s);
      point_put_y(slope * point_get_x(s) + yslope, s);

      /////////////////////////////////////////////////////////
      rotate += dmin;
      for (ax = rect->tail->next; ax != rect->head; ax = ax->next) {
	*((real_t *)ax->spare) += dmin;
	if (*((real_t *)ax->spare) >= 2 * M_PI)
	  *((real_t *)ax->spare) -= 2 * M_PI;
      }
      p = polygon_glimpse(0, rect);
      q = polygon_glimpse(1, rect);
      r = polygon_glimpse(2, rect);
      s = polygon_glimpse(3, rect);

      ax = dlink_new();
      ax->object = (void *)polygon_new_and_copy(rect);
      dlist_insert(ax, circumrects);

      temp = get_distance_of_p2p(p, q) * get_distance_of_p2p(q, r);
      if (temp < area)
	area = temp;
    }

    // Update crossing verteces between rectangle and convexhull
    cross_vertex[irotate] = pivot_next;

    pivot = NULL;
    for (i = 0, ax = rect->tail->next; ax != rect->head; ax = ax->next) {
      delta = (*((real_t *)cross_vertex[i]->spare) - *((real_t *)ax->spare) + 2 * M_PI);
      if (delta >= 2 * M_PI) delta -= 2 * M_PI;
      if (pivot == NULL || delta < dmin) {
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
  } while (pivot != start);

  point_destroy(polygon_pop(rect));
  point_destroy(polygon_pop(rect));
  point_destroy(polygon_pop(rect));
  point_destroy(polygon_pop(rect));
  polygon_destroy(rect);

  free(angle);
}

real_t convexhull_compute_min_area_rectangle(polygon_t *minrect, polygon_t *chull)
{
  real_t area, temp;
  dlist_t *circumrects;
  polygon_t *rect;
  point_t *p, *q, *r;
  dlink_t *a, *b;

  assert(minrect);
  //assert(minrect->count == 4);
  assert(chull);
  assert(chull->count >= 3);

  circumrects = dlist_new();
  convexhull_create_circumference_rectangles(circumrects, chull);

  b = NULL;
  for (a = circumrects->tail->next; a != circumrects->head; a = a->next) {
    rect = (polygon_t *)a->object;
    p = polygon_glimpse(0, rect);
    q = polygon_glimpse(1, rect);
    r = polygon_glimpse(2, rect);
    temp = get_distance_of_p2p(p, q) * get_distance_of_p2p(q, r);
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
  /*
  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);
  polygon_copy(minrect, (polygon_t *)b->object);
  */
  polygon_insert(polygon_glimpse(0, (polygon_t *)b->object), minrect);
  polygon_insert(polygon_glimpse(1, (polygon_t *)b->object), minrect);
  polygon_insert(polygon_glimpse(2, (polygon_t *)b->object), minrect);
  polygon_insert(polygon_glimpse(3, (polygon_t *)b->object), minrect);

  convexhull_delete_circumference_rectangles(circumrects);
  dlist_destroy(circumrects);

  return area;
}

real_t convexhull_compute_min_perimeter_rectangle(polygon_t *minrect, polygon_t *chull)
{
  real_t perimeter, temp;
  dlist_t *circumrects;
  polygon_t *rect;
  point_t *p, *q, *r;
  dlink_t *a, *b;

  assert(minrect);
  //  assert(minrect->count == 4);
  assert(chull);
  assert(chull->count >= 3);

  circumrects = dlist_new();
  convexhull_create_circumference_rectangles(circumrects, chull);

  b = NULL;
  for (a = circumrects->tail->next; a != circumrects->head; a = a->next) {
    rect = (polygon_t *)a->object;
    p = polygon_glimpse(0, rect);
    q = polygon_glimpse(1, rect);
    r = polygon_glimpse(2, rect);
    temp = 2 * (get_distance_of_p2p(p, q) + get_distance_of_p2p(q, r));
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
  /*
  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);
  polygon_insert(point_new(), minrect);
  polygon_copy(minrect, (polygon_t *)b->object);
  */

  polygon_insert(polygon_glimpse(0, (polygon_t *)b->object), minrect);
  polygon_insert(polygon_glimpse(1, (polygon_t *)b->object), minrect);
  polygon_insert(polygon_glimpse(2, (polygon_t *)b->object), minrect);
  polygon_insert(polygon_glimpse(3, (polygon_t *)b->object), minrect);

  convexhull_delete_circumference_rectangles(circumrects);
  dlist_destroy(circumrects);

  return perimeter;
}
