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
//#include <SDL.h>

#include <geometry/delaunay.h>
#include <geometry/point.h>
#include <geometry/point_list.h>
#include <geometry/line.h>
#include <geometry/line_list.h>
#include <geometry/triangle.h>
#include <geometry/triangle_list.h>

/*
extern SDL_Surface *screen;
extern void keyhit(void);
extern void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
extern void putline(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 pixel);
*/

static delaunay_net_t *delaunay_net_new(point_t *coord)
{
  delaunay_net_t *net;

  net = (delaunay_net_t *)dlist_new();

  point_inc_ref(coord);
  net->spare = (void *)coord;

  return net;
}

static void delaunay_net_destroy(delaunay_net_t *net)
{
  assert(net);

  if (dlist_get_ref((dlist_t *)net) <= 0) {
    assert(dlist_get_count((dlist_t *)net) == 0);
    //point_dec_ref((point_t *)net->spare);
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

delaunay_net_t *delaunay_list_get_net(int inet, delaunay_list_t *list)
{
  dlink_t *link;

  assert(list);

  link = dlist_glimpse(inet, (dlist_t *)list);

  return (delaunay_net_t *)link->object;
}

point_t *delaunay_list_get_coordinate(int inet, delaunay_list_t *list)
{
  delaunay_net_t *net;

  assert(list);

  net = delaunay_list_get_net(inet, list);

  return delaunay_net_get_coordinate(net);
}

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

void delaunay_list_delete(delaunay_list_t *list)
{
  delaunay_net_t *net;
  triangle_list_t *triangles;

  assert(list);

  while (delaunay_list_get_count_of_networks(list) > 0) {
    net = delaunay_list_pop(list);
    delaunay_net_delete_connection(net);
    delaunay_net_destroy(net);
  }

  triangles = (triangle_list_t *)list->head->object;
  triangle_list_delete(triangles);
  triangle_list_destroy(triangles);
  list->head->object = NULL;
}

void delaunay_list_destroy(delaunay_list_t *list)
{
  assert(list);

  if (delaunay_list_get_ref(list) <= 0) {
    //assert(list->count == 0);
    assert(delaunay_list_get_count_of_networks(list) == 0);
    assert(list->head->object == NULL);
    dlist_destroy((dlist_t *)list);
  } else {
    delaunay_list_dec_ref(list);
  }
}

void delaunay_list_create_2d_network(delaunay_list_t *list, point_list_t *points)
{
  dlink_t *x, *y, *z;
  point_t *o, *a, *b, *c;
  point_t *p, *v1, *v2, *v3, *org;
  delaunay_net_t *neta, *netb, *netc, *net;
  triangle_list_t *triangles;
  triangle_t *tri;
  line_list_t *lines;
  line_t *line, *l, *m;
  real_t da, db, dc, s;
  real_t val, vmin;
  real_t radius, area;

  assert(list);
  assert(points);

  v1 = point_new();
  v2 = point_new();
  v3 = point_new();
  org = point_new();

  // allocate delaunay network list
  for (x = points->tail->next; x != points->head; x = x->next) {
    net = delaunay_net_new((point_t *)x->object);
    delaunay_list_insert(net, list);
  }

  // starting point
  neta = (delaunay_net_t *)(list->tail->next->object);
  //neta = delaunay_list_get_net(0, list);
  a = delaunay_net_get_coordinate(neta);

  // find minimum distance between points 
  for (netb = NULL, x = list->tail->next->next; x != list->head; x = x->next) {
    net = (delaunay_net_t *)x->object;
    p = delaunay_net_get_coordinate(net);
    val = distance_between_points(a, p);
    if (val < REAL_EPSILON) continue;
    if (netb == NULL || val < vmin) { netb = net; vmin = val; }
  }
  b = delaunay_net_get_coordinate(netb);

  // find minimum radius of circumscribed circle for triangle
  for (netc = NULL, x = list->tail->next->next; x != list->head; x = x->next) {
    net = (delaunay_net_t *)x->object;
    if (net == netb) continue;
    p = delaunay_net_get_coordinate(net);
    // Heron's formula to find the area formed by three points
    da = distance_between_points(a, b);
    db = distance_between_points(b, p);
    dc = distance_between_points(p, a);
    s = (da + db + dc) / 2;
    area = sqrt(s * (s - da) * (s - db) * (s - dc));
    if (area < REAL_EPSILON) continue;
    // calculate radius of circumscribed circle for triangle
    radius = (da * db * dc) / (4 * area);
    if (netc == NULL || radius < vmin) { netc = net; vmin = radius; }
  }
  c = delaunay_net_get_coordinate(netc);

  // re-ordering
  point_subtract(v1, b, a);
  point_subtract(v2, c, a);
  point_xproduct(v3, v1, v2);
  if (v3->z > 0) { // counterclockwise following a to b to c ?
    net = netb, netb = netc, netc = net;
    o = b, b = c, c = o;
  }

  // connect delaunay network
  delaunay_net_connect(neta, netb);
  delaunay_net_connect(netb, netc);
  delaunay_net_connect(netc, neta);

  // Synthesize a triangle and insert it.
  //triangles = dlist_new();
  triangles = triangle_list_new();
  tri = triangle_new_and_assign(a, b, c);
  triangle_list_insert(tri, triangles);
  //triangle_dump(tri);

  // Synthesize three line and insert it.
  lines = line_list_new();
  line = line_new_and_assign(a, b);
  line_list_insert(line, lines);
  line = line_new_and_assign(b, c);
  line_list_insert(line, lines);
  line = line_new_and_assign(c, a);
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
      c = delaunay_net_get_coordinate(netc);
      if (point_cmp(c, line->a) == 0 || point_cmp(c, line->b) == 0)
	continue;
      // counter-clockwise rotation of the triangle formed by a, b, and c
      point_subtract(v1, line->b, line->a);
      point_subtract(v2, c, line->a);
      point_xproduct(v3, v1, v2);
      if (v3->z < REAL_EPSILON) continue;
      // Does any line crossing with line in the line list ?
      l = line_new_and_assign(line->a, c);
      m = line_new_and_assign(c, line->b);
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
      tri = triangle_new_and_assign(line->a, c, line->b);
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
      radius = triangle_circumcircle(org, tri);
      for (z = list->tail->next; z != list->head; z = z->next) {
	net = (delaunay_net_t *)z->object;
	o = delaunay_net_get_coordinate(net);
	if (point_cmp(o, line->a) == 0 ||
	    point_cmp(o, line->b) == 0 ||
	    point_cmp(o, c) == 0)
	  continue;
	val = distance_between_points(o, org);
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
	//l = line_new_and_assign(line->a, c);
	if (line_list_query(l, lines) < 0) {
	  line_list_insert(l, lines);
	  //line_dump(l);
	  for (z = list->tail->next; z != list->head; z = z->next) {
	    net = (delaunay_net_t *)z->object;
	    o = delaunay_net_get_coordinate(net);
	    if (point_cmp(o, line->a) == 0) {
	      delaunay_net_connect(netc, net);
	      break;
	    }
	  }
	} else line_destroy(l);
	//m = line_new_and_assign(c, line->b);
	if (line_list_query(m, lines) < 0) {
	  line_list_insert(m, lines);
	  //line_dump(m);
	  for (z = list->tail->next; z != list->head; z = z->next) {
	    net = (delaunay_net_t *)z->object;
	    o = delaunay_net_get_coordinate(net);
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
}

void delaunay_list_hook_triangle(triangle_list_t *triangles, int inet, delaunay_list_t *list)
{
  point_t *p;
  triangle_list_t *delaunay_triangles;
  triangle_t *tri;
  dlink_t *x;

  assert(list);
  assert(inet >= 0 && inet < list->count);

  p = delaunay_list_get_coordinate(inet, list);
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

