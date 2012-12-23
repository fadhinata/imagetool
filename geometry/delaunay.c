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
#include <assert.h>

#include <geometry/delaunay.h>
#include <geometry/point.h>
#include <geometry/point_list.h>
#include <geometry/line.h>
#include <geometry/line_list.h>
#include <geometry/triangle.h>
#include <geometry/triangle_list.h>

static delaunay_net_t *delaunay_net_new(void)
{
  delaunay_net_t *net;

  net = (delaunay_net_t *)dlist_new();

  return net;
}

static void delaunay_net_set_center(delaunay_net_t *net, point_t *coord)
{
  assert(net);
  assert(coord);

  point_inc_ref(coord);
  net->spare = (void *)coord;
}

static void delaunay_net_destroy(delaunay_net_t *net)
{
  dlink_t *link;
  delaunay_net_t *nbr;

  assert(net);

  while (dlist_get_count((dlist_t *)net) > 0) {
    link = dlist_pop((dlist_t *)net);
    nbr = (delaunay_net_t *)link->object;
    dlink_destroy(link);

    for (link = nbr->tail->next; link != nbr->head; link = link->next) {
      if (net == (delaunay_net_t *)link->object) {
	dlink_cutoff(link); dlist_dec_count((dlist_t *)nbr);
	dlist_dec_ref((dlist_t *)nbr);
	dlink_destroy(link);
	break;
      }
    }
    dlist_dec_ref((dlist_t *)net);
  }

  if (dlist_get_ref((dlist_t *)net) <= 0) {
    point_destroy((point_t *)net->spare);
    dlist_destroy((dlist_t *)net);
  } else {
    dlist_dec_ref((dlist_t *)net);
  }
}

delaunay_net_t *delaunay_net_get_neighbor(int index, delaunay_net_t *net)
{
  dlink_t *link;

  assert(net);

  link = dlist_glimpse(index, (dlist_t *)net);

  return (delaunay_net_t *)link->object;
}

static void delaunay_net_connect(delaunay_net_t *a, delaunay_net_t *b)
{
  dlink_t *link;

  assert(a);
  assert(b);

  dlist_inc_ref((dlist_t *)b);
  link = dlink_new(), link->object = (void *)b;
  dlist_insert(link, (dlist_t *)a);

  dlist_inc_ref((dlist_t *)a);
  link = dlink_new(), link->object = (void *)a;
  dlist_insert(link, (dlist_t *)b);
}

/*
static void delaunay_net_delete_connection(delaunay_net_t *net)
{
  dlink_t *link;
  delaunay_net_t *nbr;

  assert(net);

  while (dlist_get_count((dlist_t *)net) > 0) {
    link = dlist_pop((dlist_t *)net);
    nbr = (delaunay_net_t *)link->object;
    dlink_destroy(link);
    for (link = nbr->tail->next; link != nbr->head; link = link->next) {
      if (net == (delaunay_net_t *)link->object) {
	dlink_cutoff(link);
	dlist_dec_count((dlist_t *)nbr);
	dlist_dec_ref((dlist_t *)net);
	dlink_destroy(link);
	break;
      }
    }
    dlist_dec_ref((dlist_t *)nbr);
  }
}
*/

delaunay_net_t *delaunay_list_get_net(int inet, delaunay_list_t *list)
{
  dlink_t *link;

  assert(list);

  link = dlist_glimpse(inet, (dlist_t *)list);

  return (delaunay_net_t *)link->object;
}

/*
point_t *delaunay_list_get_coordinate(int inet, delaunay_list_t *list)
{
  delaunay_net_t *net;

  assert(list);

  net = delaunay_list_get_net(inet, list);

  return delaunay_net_get_center(net);
  //  return delaunay_net_get_coordinate(net);
}
*/

static void delaunay_list_insert(delaunay_net_t *net, delaunay_list_t *list)
{
  dlink_t *link;

  assert(net);
  assert(list);

  delaunay_net_inc_ref(net);
  link = dlink_new();
  link->object = (void *)net;
  dlist_insert(link, (dlist_t *)list);
}

/*
static delaunay_net_t *delaunay_list_extract(delaunay_list_t *list)
{
  delaunay_net_t *net;
  dlink_t *link;

  assert(list);

  link = dlist_extract((dlist_t *)list);
  net = (delaunay_net_t *)link->object;
  dlink_destroy(link);
  delaunay_net_dec_ref(net);

  return net;
}

static void delaunay_list_push(delaunay_net_t *net, delaunay_list_t *list)
{
  dlink_t *link;

  assert(net);
  assert(list);

  delaunay_net_inc_ref(net);
  link = dlink_new();
  link->object = (void *)net;
  dlist_push(link, (dlist_t *)list);
}
*/

static delaunay_net_t *delaunay_list_pop(delaunay_list_t *list)
{
  delaunay_net_t *net;
  dlink_t *link;

  assert(list);

  link = dlist_pop((dlist_t *)list);
  net = (delaunay_net_t *)link->object;
  dlink_destroy(link);
  delaunay_net_dec_ref(net);

  return net;
}

#define delaunay_list_exchange(i, j, list) dlist_exchange(i, j, (dlist_t *)list)

void delaunay_list_destroy(delaunay_list_t *list)
{
  delaunay_net_t *net;
  triangle_list_t *triangles;

  assert(list);

  if (delaunay_list_get_ref(list) <= 0) {
    while (delaunay_list_get_count(list) > 0) {
      net = delaunay_list_pop(list);
      delaunay_net_destroy(net);
    }

    triangles = (triangle_list_t *)list->head->object;
    triangle_list_isolate(triangles);
    triangle_list_destroy(triangles);
    list->head->object = NULL;

    dlist_destroy((dlist_t *)list);
  } else {
    delaunay_list_dec_ref(list);
  }
}

void delaunay_list_create_2d_network(delaunay_list_t *list, point_list_t *points)
{
  delaunay_net_t *an, *bn, *cn, *n;
  point_t *ao, *bo, *co, *o;
  int i, j;
  point_t *p, *q;
  dlink_t *ax, *bx, *cx;
  dlink_t *x, *y, *z;
  real_t da, db, dc, s;
  real_t value, vmin;
  real_t radius, area;
  point_t *v1, *v2, *v3, *org;
  triangle_list_t *tri_list;
  triangle_t *tri;
  line_list_t *line_list;
  line_t *line, *line1, *line2, *l, *m;

  assert(list);
  assert(points);

  // Allocate delaunay network list
  for (i = 0; i < point_list_get_count(points); i++) {
    p = point_list_glimpse(i, points);
    for (j = 0; j < i; j++) {
      q = point_list_glimpse(j, points);
      if (point_cmp(p, q) == 0)
	goto __skip__;
    }
    n = delaunay_net_new();
    delaunay_net_set_center(n, p);
    delaunay_list_insert(n, list);
  __skip__:;
  }

  // Starting point
  ax = list->tail->next;
  an = (delaunay_net_t *)(ax->object);
  ao = delaunay_net_get_center(an);

  // Find minimum distance between points 
  for (bn = NULL, x = list->tail->next->next; x != list->head; x = x->next) {
    n = (delaunay_net_t *)x->object;
    o = delaunay_net_get_center(n);
    value = get_distance_of_p2p(ao, o);
    if (bn == NULL || value < vmin) {
      bx = x;
      bn = n;
      vmin = value;
    }
  }
  bo = delaunay_net_get_center(bn);

  // Find minimum radius of circumscribed circle for triangle
  for (cn = NULL, x = list->tail->next->next; x != list->head; x = x->next) {
    if (x == bx) continue;
    n = (delaunay_net_t *)x->object;
    //if (n == bn) continue;
    o = delaunay_net_get_center(n);
    // Heron's formula to find the area formed by three points
    da = get_distance_of_p2p(ao, bo);
    db = get_distance_of_p2p(bo, o);
    dc = get_distance_of_p2p(o, ao);
    s = (da + db + dc) / 2;
    area = sqrt(s * (s - da) * (s - db) * (s - dc));
    // Unnecessary check that is zero area ?
    if (area < REAL_EPSILON) continue;
    // Calculate radius of circumscribed circle for triangle
    radius = (da * db * dc) / (4 * area);
    if (cn == NULL || radius < vmin) {
      cx = x;
      cn = n;
      vmin = radius;
    }
  }
  co = delaunay_net_get_center(cn);

  v1 = point_new();
  v2 = point_new();
  v3 = point_new();
  org = point_new();

  // Re-ordering as counterclockwise following a to b to c
  point_subtract(v1, bo, ao);
  point_subtract(v2, co, ao);
  point_xproduct(v3, v1, v2);
  if (point_get_z(v3) < 0.0) {
    // Swap 'bn' and 'cn'
    x = bx, bx = cx, cx = x;
    n = bn, bn = cn, cn = n;
    o = bo, bo = co, co = o;
  }
  //dlink_exchange(ax->next, bx);
  //dlink_exchange(bx->next, cx);

  // Connect delaunay network
  delaunay_net_connect(an, bn);
  delaunay_net_connect(bn, cn);
  delaunay_net_connect(cn, an);

  // Synthesize the starting triangle
  // And insert it to triangle list
  tri_list = triangle_list_new();
  tri = triangle_new_and_set(ao, bo, co);
  triangle_list_insert(tri, tri_list);

  // Synthesize three sides of the starting triangle
  line_list = line_list_new();
  line = line_new();
  line_set_endpoints(line, ao, bo);
  line_list_insert(line, line_list);
  line = line_new();
  line_set_endpoints(line, bo, co);
  line_list_insert(line, line_list);
  line = line_new();
  line_set_endpoints(line, co, ao);
  line_list_insert(line, line_list);

  // Main routine
  for (x = line_list->tail->next; x != line_list->head; x = x->next) {
    line = (line_t *)x->object;
    //line_dump(line);
    ao = line_get_a(line);
    co = line_get_b(line);

    for (y = list->tail->next; y != list->head; y = y->next) {
      bn = (delaunay_net_t *)y->object;
      bo = delaunay_net_get_center(n);
      if (point_cmp(bo, ao) == 0 || point_cmp(bo, co) == 0)
	continue;

      // Verify counter-clockwise rotation of the triangle formed by a, b, c
      point_subtract(v1, bo, ao);
      point_subtract(v2, co, ao);
      point_xproduct(v3, v1, v2);
      if (point_get_z(v3) < 0.0)
	continue;

      // Does any line crossing with line in the line list ?
      line1 = line_new();
      line_set_endpoints(line1, ao, bo);
      line2 = line_new();
      line_set_endpoints(line2, bo, co);

      for (z = line_list->tail->next; z != line_list->head; z = z->next) {
	l = (line_t *)z->object;
	// Check coincidance between end points of two lines
	if (point_cmp(line_get_a(l), line_get_a(line1)) == 0 ||
	    point_cmp(line_get_b(l), line_get_a(line1)) == 0 ||
	    point_cmp(line_get_a(l), line_get_b(line1)) == 0 ||
	    point_cmp(line_get_b(l), line_get_b(line1)) == 0 ||
	    point_cmp(line_get_a(l), line_get_a(line2)) == 0 ||
	    point_cmp(line_get_b(l), line_get_b(line2)) == 0 ||
	    point_cmp(line_get_a(l), line_get_a(line2)) == 0 ||
	    point_cmp(line_get_b(l), line_get_b(line2)) == 0)
	  continue;

	// Check crossing between lines
	if (compute_intersection_point_of_lines(org, l, line1) > 0 ||
	    compute_intersection_point_of_lines(org, l, line2) > 0) {
	  line_destroy(line1);
	  line1 = NULL;
	  line_destroy(line2);
	  line2 = NULL;
	  break;
	}
      }

      if (line1 == NULL && line2 == NULL) continue;

      // Does the triangle exist in the pool of delaunay triangles before ?
      tri = triangle_new_and_set(ao, bo, co);
      if (triangle_list_query(tri, tri_list) >= 0) {
	line_destroy(line1);
	line1 = NULL;
	line_destroy(line2);
	line2 = NULL;
	triangle_destroy(tri);
	tri = NULL;
	continue;
      }

      // Does the unique triangle form the delaunay triangle ?
      radius = triangle_compute_circumcircle(org, tri);
      for (z = list->tail->next; z != list->head; z = z->next) {
	n = (delaunay_net_t *)z->object;
	o = delaunay_net_get_center(n);
	if (point_cmp(o, ao) == 0 ||
	    point_cmp(o, bo) == 0 ||
	    point_cmp(o, co) == 0)
	  continue;
	value = get_distance_of_p2p(o, org);
	if (value < radius) {
	  line_destroy(line1);
	  line1 = NULL;
	  line_destroy(line2);
	  line2 = NULL;
	  triangle_destroy(tri);
	  tri = NULL;
	  break;
	}
      }

      if (tri) {
	/* put the trinangle into triangle's list, 
	 * establish connection between delaunay networks, and
	 * allocate and put two lines into line's list */
	triangle_list_insert(tri, tri_list);
	//triangle_dump(tri);
	//point_dump(org);
	//printf("radius: %lf\n", radius);
	//l = line_new_and_set(line->a, c);
	if (line_list_query(line1, line_list) < 0) {
	  line_list_insert(line1, line_list);
	  //line_dump(l);
	  for (z = list->tail->next; z != list->head; z = z->next) {
	    n = (delaunay_net_t *)z->object;
	    o = delaunay_net_get_center(n);
	    if (point_cmp(o, ao) == 0) {
	      delaunay_net_connect(bn, n);
	      break;
	    }
	  }
	} else {
	  line_destroy(line1);
	}

	//m = line_new_and_set(c, line->b);
	if (line_list_query(line2, line_list) < 0) {
	  line_list_insert(line2, line_list);
	  //line_dump(m);
	  for (z = list->tail->next; z != list->head; z = z->next) {
	    n = (delaunay_net_t *)z->object;
	    o = delaunay_net_get_center(n);
	    if (point_cmp(o, co) == 0) {
	      delaunay_net_connect(cn, n);
	      break;
	    }
	  }
	} else {
	  line_destroy(line2);
	}
	//keyhit();
	break;
      }
    }
  }

  list->head->object = (void *)tri_list;

  while (line_list_get_count(line_list) > 0) {
    l = line_list_pop(line_list);
    //line_dump(line);
    line_destroy(l);
  }
  line_list_destroy(line_list);

  point_destroy(v1);
  point_destroy(v2);
  point_destroy(v3);
  point_destroy(org);


#if 0

  if (v3->z > 0) { // counterclockwise following a to b to c ?
    n = bn, bn = cn, cn = n;
    o = bo, bo = co, co = o;
  }

  // connect delaunay network
  delaunay_net_connect(neta, netb);
  delaunay_net_connect(netb, netc);
  delaunay_net_connect(netc, neta);

  // Synthesize a triangle and insert it.
  //triangles = dlist_new();
  triangles = triangle_list_new();
  tri = triangle_new_and_set(a, b, c);
  triangle_list_insert(tri, triangles);
  //triangle_dump(tri);

  // Synthesize three line and insert it.
  lines = line_list_new();

  line = line_new();
  line_set_endpoints(line, a, b);
  line_list_insert(line, lines);

  line = line_new();
  line_set_endpoints(line, b, c);
  line_list_insert(line, lines);

  line = line_new();
  line_set_endpoints(line, b, c);
  line_list_insert(line, lines);

  // main routine
  for (x = lines->tail->next; x != lines->head; x = x->next) {
    line = (line_t *)x->object;
    //line_dump(line);
    /*
    if ((line->a->x == 308 && line->b->x == 308) ||
	(line->a->x == 308 && line->b->x == 333) ||
	(line->a->x == 333 && line->b->x == 358)) {
      putline(screen, line->a->x, line->a->y, line->b->x, line->b->y,
	      SDL_MapRGB(screen->format, 255, 0, 0));
    }
    */

    for (y = list->tail->next; y != list->head; y = y->next) {
      netc = (delaunay_net_t *)y->object;
      c = delaunay_net_get_center(netc);
      if (point_cmp(c, line->a) == 0 ||
	  point_cmp(c, line->b) == 0)
	continue;
      // counter-clockwise rotation of the triangle formed by a, b, and c
      point_subtract(v1, line->b, line->a);
      point_subtract(v2, c, line->a);
      point_xproduct(v3, v1, v2);
      if (v3->z < REAL_EPSILON) continue;
      // Does any line crossing with line in the line list ?
      l = line_new();
      line_set_endpoints(l, line->a, c);
      m = line_new();
      line_set_endpoints(m, c, line->b);
      /*
      if ((line->a->x == 308 && line->b->x == 308) ||
	  (line->a->x == 308 && line->b->x == 333) ||
	  (line->a->x == 333 && line->b->x == 358)) {
	putline(screen, l->a->x, l->a->y, l->b->x, l->b->y,
		SDL_MapRGB(screen->format, 0, 255, 0));
	putline(screen, m->a->x, m->a->y, m->b->x, m->b->y,
		SDL_MapRGB(screen->format, 0, 255, 0));
	SDL_Flip(screen);
	keyhit();
      }
      */

#if 1
      for (z = lines->tail->next; z != lines->head; z = z->next) {
	// Is there an intersecting ?
	if ((intersect_point_between_lines(v3, l, (line_t *)z->object) == 1) ||
	    (intersect_point_between_lines(v3, m, (line_t *)z->object) == 1)) {
	  /*
	  if ((line->a->x == 308 && line->b->x == 308) ||
	      (line->a->x == 308 && line->b->x == 333) ||
	      (line->a->x == 333 && line->b->x == 358)) {
	    printf("line crossing\n");
	    keyhit();
	    putline(screen, l->a->x, l->a->y, l->b->x, l->b->y,
		    SDL_MapRGB(screen->format, 0, 0, 0));
	    putline(screen, m->a->x, m->a->y, m->b->x, m->b->y,
		    SDL_MapRGB(screen->format, 0, 0, 0));
	    SDL_Flip(screen);
	  }
	  */

	  line_destroy(l); l = NULL;
	  line_destroy(m); m = NULL;
	  break;
	}
      }
      if (l == NULL && m == NULL) continue;
#endif

      // Does the triangle exist in the pool of delaunay triangles before ?
      tri = triangle_new_and_set(line->a, c, line->b);
      //if (is_triangle_in_list(tri, triangles)) {
      if (triangle_list_query(tri, triangles) >= 0) {
	/*
	if ((line->a->x == 308 && line->b->x == 308) ||
	    (line->a->x == 308 && line->b->x == 333) ||
	    (line->a->x == 333 && line->b->x == 358)) {
	  printf("already inserted\n");
	  keyhit();
	  putline(screen, l->a->x, l->a->y, l->b->x, l->b->y,
		  SDL_MapRGB(screen->format, 0, 0, 0));
	  putline(screen, m->a->x, m->a->y, m->b->x, m->b->y,
		  SDL_MapRGB(screen->format, 0, 0, 0));
	  SDL_Flip(screen);
	}
	*/
	if (line->a->x == 333 && line->b->x == 358)
	  printf("already contained\n");

	line_destroy(l);
	line_destroy(m);
	triangle_destroy(tri);
	continue;
      }

      // Does the unique triangle form the delaunay triangle ?
      radius = triangle_compute_circumcircle(org, tri);
      for (z = list->tail->next; z != list->head; z = z->next) {
	net = (delaunay_net_t *)z->object;
	o = delaunay_net_get_center(net);
	//o = delaunay_net_get_coordinate(net);
	if (point_cmp(o, line->a) == 0 ||
	    point_cmp(o, line->b) == 0 ||
	    point_cmp(o, c) == 0)
	  continue;
	val = get_distance_of_p2p(o, org);
	if ((val < radius) && (round(val * 100) < round(radius * 100))) {
	  //if (val < radius) {
	  /*
	  if ((line->a->x == 308 && line->b->x == 308) ||
	      (line->a->x == 308 && line->b->x == 333) ||
	      (line->a->x == 333 && line->b->x == 358)) {
	    printf("point containing\n");
	    putpixel(screen, o->x, o->y, SDL_MapRGB(screen->format, 255, 0, 0));
	    SDL_Flip(screen);
	    keyhit();
	    putline(screen, l->a->x, l->a->y, l->b->x, l->b->y,
		    SDL_MapRGB(screen->format, 0, 0, 0));
	    putline(screen, m->a->x, m->a->y, m->b->x, m->b->y,
		    SDL_MapRGB(screen->format, 0, 0, 0));
	    SDL_Flip(screen);
	  }
	  */

	  line_destroy(l);
	  line_destroy(m);
	  triangle_destroy(tri);
	  tri = NULL;
	  break;
	}
      }

      if (tri) {
	/* put the trinangle into triangle's list, 
	 * establish connection between delaunay networks, and
	 * allocate and put two lines into line's list */
	triangle_list_insert(tri, triangles);
	/*
	putline(screen, l->a->x, l->a->y, l->b->x, l->b->y, SDL_MapRGB(screen->format, 0, 0, 255));
	putline(screen, m->a->x, m->a->y, m->b->x, m->b->y, SDL_MapRGB(screen->format, 0, 0, 255));
	SDL_Flip(screen);
	//keyhit();
	*/
	//triangle_dump(tri);
	//point_dump(org);
	//printf("radius: %lf\n", radius);
	//l = line_new_and_set(line->a, c);
	if (line_list_query(l, lines) < 0) {
	  line_list_insert(l, lines);
	  //line_dump(l);
	  for (z = list->tail->next; z != list->head; z = z->next) {
	    net = (delaunay_net_t *)z->object;
	    o = delaunay_net_get_center(net);
	    //o = delaunay_net_get_coordinate(net);
	    if (point_cmp(o, line->a) == 0) {
	      delaunay_net_connect(netc, net);
	      break;
	    }
	  }
	} else line_destroy(l);
	//m = line_new_and_set(c, line->b);
	if (line_list_query(m, lines) < 0) {
	  line_list_insert(m, lines);
	  //line_dump(m);
	  for (z = list->tail->next; z != list->head; z = z->next) {
	    net = (delaunay_net_t *)z->object;
	    o = delaunay_net_get_center(net);
	    //o = delaunay_net_get_coordinate(net);
	    if (point_cmp(o, line->b) == 0) {
	      delaunay_net_connect(netc, net);
	      break;
	    }
	  }
	} else line_destroy(m);
	//keyhit();
	break;
      }
    }
  }

  // de-allocation lines and triangles
  /*
  while (triangles->count) {
    tri = triangle_list_pop(triangles);
    //triangle_dump(tri);
    triangle_destroy(tri);
  }
  triangle_list_destroy(triangles);
  */
  list->head->object = (void *)triangles;

  while (lines->count) {
    line = line_list_pop(lines);
    //line_dump(line);
    line_destroy(line);
  }
  line_list_destroy(lines);

  point_destroy(v1);
  point_destroy(v2);
  point_destroy(v3);
  point_destroy(org);
#endif

}

void delaunay_list_hook_triangle(triangle_list_t *triangles, int inet, delaunay_list_t *list)
{
  point_t *p;
  triangle_list_t *delaunay_triangles;
  triangle_t *tri;
  dlink_t *x;

  assert(list);
  assert(inet >= 0 && inet < list->count);

  p = delaunay_net_get_center(delaunay_list_get_net(inet, list));
  //p = delaunay_list_get_coordinate(inet, list);
  delaunay_triangles = (triangle_list_t *)list->head->object;
  for (x = delaunay_triangles->tail->next; x != delaunay_triangles->head; x = x->next) {
    tri = (triangle_t *)x->object;
    if (point_cmp(p, tri->a) == 0 ||
	point_cmp(p, tri->b) == 0 ||
	point_cmp(p, tri->c) == 0) {
      triangle_list_insert(tri, triangles);
    }
  }
}

