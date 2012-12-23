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
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include <common.h>
#include <geometry/line.h>
#include <geometry/point.h>
//#include <linear_algebra/matrix.h>

line_t *line_new(void)
{
  line_t *line;

  line = malloc(sizeof(line_t));
  assert(line);

  memset(line, 0, sizeof(line_t));

  return line;
}

void line_set_endpoints(line_t *line, point_t *a, point_t *b)
{
  assert(line);
  assert(a);
  assert(b);

  if (line->a) point_destroy(line->a);
  if (line->b) point_destroy(line->b);

  line->form_type = LINE_BY_ENDPOINTS;
  point_inc_ref(a); line->a = a;
  point_inc_ref(b); line->b = b;
}

void line_set_endpoint_direction(line_t *line, point_t *e, point_t *v)
{
  assert(line);
  assert(e);
  assert(v);

  if (line->a) point_destroy(line->a);
  if (line->b) point_destroy(line->b);

  line->form_type = LINE_BY_ENDPOINT_DIRECTION;
  point_inc_ref(e); line->a = e;
  point_inc_ref(v); line->b = v;
}

void line_set_position_direction(line_t *line, point_t *p, point_t *v)
{
  assert(line);
  assert(p);
  assert(v);

  if (line->a) point_destroy(line->a);
  if (line->b) point_destroy(line->b);

  line->form_type = LINE_BY_POSITION_DIRECTION;
  point_inc_ref(p); line->a = p;
  point_inc_ref(v); line->b = v;
}

void line_copy(line_t *copyer, line_t *copee)
{
  assert(copyer);
  assert(copyer->a);
  assert(copyer->b);

  assert(copee);
  assert(copee->a);
  assert(copee->b);

  copyer->form_type = copee->form_type;

  point_copy(copyer->a, copee->a);
  point_copy(copyer->b, copee->b);
}

line_t *line_new_and_copy(line_t *copee)
{
  line_t *line;
  point_t *a, *b;

  assert(copee);
  assert(copee->a);
  assert(copee->b);

  line = line_new();

  line->form_type = copee->form_type;
  a = point_new_and_copy(copee->a);
  b = point_new_and_copy(copee->b);

  //point_inc_ref(a);
  //point_inc_ref(b);

  line->a = a;
  line->b = b;

  return line;
}

real_t line_get_length(line_t *line)
{
  real_t len;

  assert(line);
  assert(line->a);
  assert(line->b);

  switch (line->form_type) {
  case LINE_BY_ENDPOINTS:
    len = sqrt(sqr(point_get_x(line->a) - point_get_x(line->b)) +
	       sqr(point_get_y(line->a) - point_get_y(line->b)) +
	       sqr(point_get_z(line->a) - point_get_z(line->b)));
    break;
  case LINE_BY_ENDPOINT_DIRECTION:
  case LINE_BY_POSITION_DIRECTION:
    len = INFINITY;
    break;
  default:
    abort();
    break;
  }

  return len;
}
/*
real_t line_get_slope(line_t *line)
{
  real_t a;

  assert(line);
  assert(line->a);
  assert(line->b);

  switch (line->form_type) {
  case LINE_BY_ENDPOINTS:
    a = ((point_get_y(line->b) - point_get_y(line->a)) /
	 (point_get_x(line->a) - point_get_x(line->a)));
    break;
  case LINE_BY_ENDPOINT_DIRECTION:
  case LINE_BY_POSITION_DIRECTION:
    a = point_get_y(line->b) / point_get_x(line->b);
    break;
  default:
    abort();
    break;
  }

  return a;
}

real_t line_get_x_intercept(line_t *line)
{
  real_t a, b;

  assert(line);
  assert(line->a);
  assert(line->b);

  switch (line->form_type) {
  case LINE_BY_ENDPOINTS:
    a = ((point_get_y(line->b) - point_get_y(line->a)) /
	 (point_get_x(line->a) - point_get_x(line->a)));
    break;
  case LINE_BY_ENDPOINT_DIRECTION:
  case LINE_BY_POSITION_DIRECTION:
    a = point_get_y(line->b) / point_get_x(line->b);
    break;
  default:
    abort();
    break;
  }

  b = point_get_y(line->a) - a * point_get_x(line->a);

  return -b / a;
}

real_t line_get_y_intercept(line_t *line)
{
  real_t a, b;

  assert(line);
  assert(line->a);
  assert(line->b);

  switch (line->form_type) {
  case LINE_BY_ENDPOINTS:
    a = ((point_get_y(line->b) - point_get_y(line->a)) /
	 (point_get_x(line->a) - point_get_x(line->a)));
    break;
  case LINE_BY_ENDPOINT_DIRECTION:
  case LINE_BY_POSITION_DIRECTION:
    a = point_get_y(line->b) / point_get_x(line->b);
    break;
  default:
    abort();
    break;
  }

  b = point_get_y(line->a) - a * point_get_x(line->a);

  return b;
}
*/

void line_read_normal_direction(point_t *dir, line_t *line)
{
  real_t dist;

  assert(dir);
  assert(line);
  assert(line->a);
  assert(line->b);

  switch (line->form_type) {
  case LINE_BY_ENDPOINTS: point_subtract(dir, line->b, line->a); break;
  case LINE_BY_ENDPOINT_DIRECTION:
  case LINE_BY_POSITION_DIRECTION: point_copy(dir, line->b); break;
  default: abort(); break;
  }

  dist = point_get_distance(dir);
  point_put_x(point_get_x(dir) / dist, dir);
  point_put_y(point_get_y(dir) / dist, dir);
  point_put_z(point_get_z(dir) / dist, dir);
}

// shortest distance between origin and line
// Vector equation of line in 3D
// (x - a->x) / b->x = (y - a->y) / b->y = (z - a->z) / b->z = t
// /p = /a + t * /b
// /p = (ax + t * bx, ay + t * by, az + t * bz)
// Get the point with shortest distance between origin and line
real_t line_read_shortest_point(point_t *p, line_t *line)
{
  real_t l, m, n;
  real_t tmp, dist, t;
  point_t v;

  assert(line);
  assert(line->a);
  assert(line->b);

  switch (line->form_type) {
  case LINE_BY_ENDPOINTS: point_subtract(&v, line->b, line->a); break;
  case LINE_BY_ENDPOINT_DIRECTION:
  case LINE_BY_POSITION_DIRECTION: point_copy(&v, line->b); break;
  default: abort(); break;
  }

  l = (sqr(point_get_x(line->a)) +
       sqr(point_get_y(line->a)) +
       sqr(point_get_z(line->a)));

  m = (point_get_x(line->a) * point_get_x(&v) +
       point_get_y(line->a) * point_get_y(&v) +
       point_get_z(line->a) * point_get_z(&v));

  n = (sqr(point_get_x(&v)) +
       sqr(point_get_y(&v)) +
       sqr(point_get_z(&v)));

  t = -m / n;

  //dist = l - sqr(m) / n;

  switch (line->form_type) {
  case LINE_BY_ENDPOINTS:
    if (!(t >= 0.0 && t <= 1.0)) {
      tmp = l + 2 * m + n;
      if (tmp < l) {
	dist = sqrt(tmp); t = 1.0;
      } else {
	dist = sqrt(l); t = 0.0;
      }
    } else {
      dist = sqrt(l - sqr(m) / n);
    }
    break;
  case LINE_BY_ENDPOINT_DIRECTION:
    if (t >= 0) {
      dist = sqrt(l - sqr(m) / n);
    } else {
      dist = sqrt(l); t = 0.0;
    }
    break;
  case LINE_BY_POSITION_DIRECTION:
    dist = sqrt(l - sqr(m) / n);
    break;
  default:
    abort();
    break;
  }

  if (p) {
    point_put_x(point_get_x(line->a) + t * point_get_x(&v), p);
    point_put_y(point_get_y(line->a) + t * point_get_y(&v), p);
    point_put_z(point_get_z(line->a) + t * point_get_z(&v), p);
  }

  return dist;
}

void line_move(line_t *line, real_t dx, real_t dy, real_t dz)
{
  assert(line);
  
  switch (line->form_type) {
  case LINE_BY_ENDPOINTS:
    point_move(line->a, dx, dy, dz);
    point_move(line->b, dx, dy, dz);
    break;
  case LINE_BY_ENDPOINT_DIRECTION:
  case LINE_BY_POSITION_DIRECTION:
    point_move(line->a, dx, dy, dz);
    break;
  default:
    abort();
    break;
  }
}

real_t line_read_shortest_point_from(point_t *p, line_t *line, point_t *q)
{
  real_t dist;
  line_t *tmp;

  assert(line);
  assert(line->a);
  assert(line->b);

  tmp = line_new_and_copy(line);
  line_move(tmp, -point_get_x(q), -point_get_y(q), -point_get_z(q));
  dist = line_read_shortest_point(p, tmp);
  point_move(p, point_get_x(q), point_get_y(q), point_get_z(q));
  //line_clear(tmp);
  line_destroy(tmp);

  return dist;	
}

int line_cmp(line_t *x, line_t *y)
{
  real_t l, m;
  point_t p, q;

  assert(x);
  assert(x->a);
  assert(x->b);

  assert(y);
  assert(y->a);
  assert(y->b);

  if (x->form_type == y->form_type) {
    switch (x->form_type) {
    case LINE_BY_ENDPOINTS:
      if ((point_cmp(x->a, y->a) == 0 && point_cmp(x->b, y->b) == 0) ||
	  (point_cmp(x->a, y->b) == 0 && point_cmp(x->b, y->a) == 0))
	return 0;
      break;
    case LINE_BY_ENDPOINT_DIRECTION:
      if (point_cmp(x->a, y->a) == 0) {
	line_read_normal_direction(&p, x);
	line_read_normal_direction(&q, y);
	if (point_cmp(&p, &q) == 0) return 0;
      }
      break;
    case LINE_BY_POSITION_DIRECTION:
      l = line_read_shortest_point(&p, x);
      m = line_read_shortest_point(&q, y);
      if (abs(l - m) < REAL_EPSILON) {
	if ((abs(point_get_x(&p) - point_get_x(&q)) < REAL_EPSILON &&
	     abs(point_get_y(&p) - point_get_y(&q)) < REAL_EPSILON &&
	     abs(point_get_z(&p) - point_get_z(&q)) < REAL_EPSILON) ||
	    (abs(point_get_x(&p) + point_get_x(&q)) < REAL_EPSILON &&
	     abs(point_get_y(&p) + point_get_y(&q)) < REAL_EPSILON &&
	     abs(point_get_z(&p) + point_get_z(&q)) < REAL_EPSILON))
	  return 0;
      }
      break;
    default:
      abort();
      break;
    }
  }

  return -1;
}

void line_dump(line_t *line)
{
  point_t pos, dir;

  assert(line);
  assert(line->a);
  assert(line->b);

  switch (line->form_type) {
  case LINE_BY_ENDPOINTS:
    printf("line\n a(%.2lf,%.2lf,%.2lf) to b(%.2lf,%.2lf,%.2lf)\n",
	   point_get_x(line->a), point_get_y(line->a), point_get_z(line->a),
	   point_get_x(line->b), point_get_y(line->b), point_get_z(line->b));
    break;
  case LINE_BY_ENDPOINT_DIRECTION:
    printf("line\n e(%.2lf,%.2lf,%.2lf) v(%.2lf,%.2lf,%.2lf)\n",
	   point_get_x(line->a), point_get_y(line->a), point_get_z(line->a),
	   point_get_x(line->b), point_get_y(line->b), point_get_z(line->b));

    line_read_normal_direction(&dir, line);
    printf(" /(%.2lf,%.2lf,%.2lf) @ (%.2lf,%.2lf,%.2lf)\n",
	   point_get_x(&dir), point_get_y(&dir), point_get_z(&dir),
	   point_get_x(line->a), point_get_y(line->a), point_get_z(line->a));
    break;
  case LINE_BY_POSITION_DIRECTION:
    printf("line\n e(%.2lf,%.2lf,%.2lf) v(%.2lf,%.2lf,%.2lf)\n",
	   point_get_x(line->a), point_get_y(line->a), point_get_z(line->a),
	   point_get_x(line->b), point_get_y(line->b), point_get_z(line->b));

    line_read_shortest_point(&pos, line);
    line_read_normal_direction(&dir, line);
    printf(" /(%.2lf,%.2lf,%.2lf) @ (%.2lf,%.2lf,%.2lf)\n",
	   point_get_x(&dir), point_get_y(&dir), point_get_z(&dir),
	   point_get_x(&pos), point_get_y(&pos), point_get_z(&pos));
    break;
  default:
    abort();
    break;
  }
}

void line_clear(line_t *line)
{
  assert(line);

  if (line->a) {
    point_destroy(line->a);
    line->a = NULL;
  }

  if (line->b) {
    point_destroy(line->b);
    line->b = NULL;
  }
}

void line_destroy(line_t *line)
{
  assert(line);

  if (line_get_ref(line) <= 0) {
    if (line->a) point_destroy(line->a);
    if (line->b) point_destroy(line->b);
    free(line);
  } else {
    line->reference--;
  }
}

bool point_in_line(point_t *p, line_t *line)
{
  real_t tx, ty, tz;
  point_t a, u;

  assert(p);
  assert(line);

  switch (line->form_type) {
  case LINE_BY_ENDPOINTS:
    point_copy(&a, line->a);
    point_subtract(&u, line->b, line->a);
    tx = (point_get_x(p) - point_get_x(&a)) / point_get_x(&u);
    ty = (point_get_y(p) - point_get_y(&a)) / point_get_y(&u);
    tz = (point_get_z(p) - point_get_z(&a)) / point_get_z(&u);
    if (abs(tx - ty) < REAL_EPSILON &&
	abs(ty - tz) < REAL_EPSILON &&
	(abs(tx) < REAL_EPSILON ||
	 abs(tx - 1.0) < REAL_EPSILON ||
	 (tx > 0.0 && tx < 1.0)))
      return true;
    break;
  case LINE_BY_ENDPOINT_DIRECTION:
    point_copy(&a, line->a);
    point_copy(&u, line->b);
    tx = (point_get_x(p) - point_get_x(&a)) / point_get_x(&u);
    ty = (point_get_y(p) - point_get_y(&a)) / point_get_y(&u);
    tz = (point_get_z(p) - point_get_z(&a)) / point_get_z(&u);
    if (abs(tx - ty) < REAL_EPSILON &&
	abs(ty - tz) < REAL_EPSILON &&
	(abs(tx) < REAL_EPSILON || tx > 0.0))
      return true;
    break;
  case LINE_BY_POSITION_DIRECTION:
    line_read_shortest_point(&a, line);
    line_read_normal_direction(&u, line);
    tx = (point_get_x(p) - point_get_x(&a)) / point_get_x(&u);
    ty = (point_get_y(p) - point_get_y(&a)) / point_get_y(&u);
    tz = (point_get_z(p) - point_get_z(&a)) / point_get_z(&u);
    if ((abs(tx - ty) < REAL_EPSILON) &&
	(abs(ty - tz) < REAL_EPSILON))
      return true;
    break;
  default:
    abort();
    break;
  }

  return false;
}

// /a + t1 * /u
// /b + t2 * /v
// INT_MAX : too many
// 1 : ok
// -1 : none
int compute_intersection_point_of_lines(point_t *p, line_t *l, line_t *m)
{
  point_t a, b, u, v;
  real_t det, t1, t2, t3, t4;

  assert(p);
  assert(l);
  assert(m);

  line_read_shortest_point(&a, l);
  line_read_normal_direction(&u, l);

  line_read_shortest_point(&b, m);
  line_read_normal_direction(&v, m);

  det = -point_get_x(&u) * point_get_y(&v) + point_get_y(&u) * point_get_x(&v);

  t1 = -point_get_y(&v) * (point_get_x(&b) - point_get_x(&a)) +
    point_get_x(&v) * (point_get_y(&b) - point_get_y(&a));

  t2 = -point_get_y(&u) * (point_get_x(&b) - point_get_x(&a)) +
    point_get_x(&v) * (point_get_y(&b) - point_get_y(&a));

  if (abs(det) < REAL_EPSILON) {
    if (abs(t1) < REAL_EPSILON || abs(t2) < REAL_EPSILON) {
      if (l->form_type == LINE_BY_POSITION_DIRECTION ||
	  m->form_type == LINE_BY_POSITION_DIRECTION)
	return INT_MAX;

      if (l->form_type == LINE_BY_ENDPOINT_DIRECTION) {
	if (m->form_type == LINE_BY_ENDPOINT_DIRECTION) {
	  if (point_cmp(&u, &v) == 0) return INT_MAX;
	  if (point_cmp(line_get_a(l), line_get_a(m)) == 0) {
	    point_copy(p, line_get_a(l));
	    return 1;
	  }
	  if (point_in_line(line_get_a(l), m)) return INT_MAX;
	  return -1;
	} else if (m->form_type == LINE_BY_ENDPOINTS) {
	  if (point_cmp(&u, &v) == 0) {
	    if (!point_in_line(line_get_b(m), l)) return -1;
	    if (point_cmp(line_get_b(m), line_get_a(l)) == 0) {
	      point_copy(p, line_get_b(m));
	      return 1;
	    }
	    return INT_MAX;
	  } else {
	    if (!point_in_line(line_get_a(m), l)) return -1;
	    if (point_cmp(line_get_a(m), line_get_a(l)) == 0) {
	      point_copy(p, line_get_a(m));
	      return 1;
	    }
	    return INT_MAX;
	  }
	} else {
	  abort();
	  return -1;
	}
      } else if (l->form_type == LINE_BY_ENDPOINTS) {
	if (m->form_type == LINE_BY_ENDPOINT_DIRECTION) {
	  if (point_cmp(&u, &v) == 0) {
	    if (!point_in_line(line_get_b(l), m)) return -1;
	    if (point_cmp(line_get_b(l), line_get_a(m)) == 0) {
	      point_copy(p, line_get_b(l));
	      return 1;
	    }
	    return INT_MAX;
	  } else {
	    if (!point_in_line(line_get_a(l), m)) return -1;
	    if (point_cmp(line_get_a(l), line_get_a(m)) == 0) {
	      point_copy(p, line_get_a(l));
	      return 1;
	    }
	    return INT_MAX;
	  }
	} else if (m->form_type == LINE_BY_ENDPOINTS) {
	  if (line_get_length(l) > line_get_length(m)) {
	    if (!point_in_line(line_get_a(m), l) && !point_in_line(line_get_b(m), l))
	      return -1;
	    if (point_cmp(&u, &v) == 0) {
	      if (point_cmp(line_get_b(m), line_get_a(l)) == 0) {
		point_copy(p, line_get_b(m));
		return 1;
	      }
	      if (point_cmp(line_get_a(m), line_get_b(l)) == 0) {
		point_copy(p, line_get_a(m));
		return 1;
	      }
	      return INT_MAX;
	    } else {
	      if (point_cmp(line_get_a(m), line_get_a(l)) == 0) {
		point_copy(p, line_get_a(m));
		return 1;
	      }
	      if (point_cmp(line_get_b(m), line_get_b(l)) == 0) {
		point_copy(p, line_get_b(m));
		return 1;
	      }
	      return INT_MAX;
	    }
	  } else {
	    if (!point_in_line(line_get_a(l), m) && !point_in_line(line_get_b(l), m))
	      return -1;
	    if (point_cmp(&u, &v) == 0) {
	      if (point_cmp(line_get_b(l), line_get_a(m)) == 0) {
		point_copy(p, line_get_b(l));
		return 1;
	      }
	      if (point_cmp(line_get_a(l), line_get_b(m)) == 0) {
		point_copy(p, line_get_a(l));
		return 1;
	      }
	      return INT_MAX;
	    } else {
	      if (point_cmp(line_get_a(l), line_get_a(m)) == 0) {
		point_copy(p, line_get_a(l));
		return 1;
	      }
	      if (point_cmp(line_get_b(l), line_get_b(m)) == 0) {
		point_copy(p, line_get_b(l));
		return 1;
	      }
	      return INT_MAX;
	    }
	  }
	} else {
	  abort();
	  return -1;
	}
      } else {
	abort();
	return -1;
      }

    }
    return -1;
  }

  t1 /= det;
  t2 /= det;

  det = -point_get_y(&u) * point_get_z(&v) + point_get_z(&u) * point_get_y(&v);

  t3 = -point_get_z(&v) * (point_get_y(&b) - point_get_y(&a)) +
    point_get_y(&v) * (point_get_z(&b) - point_get_z(&a));

  t4 = -point_get_z(&u) * (point_get_y(&b) - point_get_y(&a)) +
    point_get_y(&u) * (point_get_z(&b) - point_get_z(&a));

  if (abs(det) < REAL_EPSILON) {
    if (abs(t3) < REAL_EPSILON || abs(t4) < REAL_EPSILON) {
      if (l->form_type == LINE_BY_POSITION_DIRECTION ||
	  m->form_type == LINE_BY_POSITION_DIRECTION)
	return INT_MAX;

      if (l->form_type == LINE_BY_ENDPOINT_DIRECTION) {
	if (m->form_type == LINE_BY_ENDPOINT_DIRECTION) {
	  if (point_cmp(&u, &v) == 0) return INT_MAX;
	  if (point_cmp(line_get_a(l), line_get_a(m)) == 0) {
	    point_copy(p, line_get_a(l));
	    return 1;
	  }
	  if (point_in_line(line_get_a(l), m)) return INT_MAX;
	  return -1;
	} else if (m->form_type == LINE_BY_ENDPOINTS) {
	  if (point_cmp(&u, &v) == 0) {
	    if (!point_in_line(line_get_b(m), l)) return -1;
	    if (point_cmp(line_get_b(m), line_get_a(l)) == 0) {
	      point_copy(p, line_get_b(m));
	      return 1;
	    }
	    return INT_MAX;
	  } else {
	    if (!point_in_line(line_get_a(m), l)) return -1;
	    if (point_cmp(line_get_a(m), line_get_a(l)) == 0) {
	      point_copy(p, line_get_a(m));
	      return 1;
	    }
	    return INT_MAX;
	  }
	} else {
	  abort();
	  return -1;
	}
      } else if (l->form_type == LINE_BY_ENDPOINTS) {
	if (m->form_type == LINE_BY_ENDPOINT_DIRECTION) {
	  if (point_cmp(&u, &v) == 0) {
	    if (!point_in_line(line_get_b(l), m)) return -1;
	    if (point_cmp(line_get_b(l), line_get_a(m)) == 0) {
	      point_copy(p, line_get_b(l));
	      return 1;
	    }
	    return INT_MAX;
	  } else {
	    if (!point_in_line(line_get_a(l), m)) return -1;
	    if (point_cmp(line_get_a(l), line_get_a(m)) == 0) {
	      point_copy(p, line_get_a(l));
	      return 1;
	    }
	    return INT_MAX;
	  }
	} else if (m->form_type == LINE_BY_ENDPOINTS) {
	  if (line_get_length(l) > line_get_length(m)) {
	    if (!point_in_line(line_get_a(m), l) && !point_in_line(line_get_b(m), l))
	      return -1;
	    if (point_cmp(&u, &v) == 0) {
	      if (point_cmp(line_get_b(m), line_get_a(l)) == 0) {
		point_copy(p, line_get_b(m));
		return 1;
	      }
	      if (point_cmp(line_get_a(m), line_get_b(l)) == 0) {
		point_copy(p, line_get_a(m));
		return 1;
	      }
	      return INT_MAX;
	    } else {
	      if (point_cmp(line_get_a(m), line_get_a(l)) == 0) {
		point_copy(p, line_get_a(m));
		return 1;
	      }
	      if (point_cmp(line_get_b(m), line_get_b(l)) == 0) {
		point_copy(p, line_get_b(m));
		return 1;
	      }
	      return INT_MAX;
	    }
	  } else {
	    if (!point_in_line(line_get_a(l), m) && !point_in_line(line_get_b(l), m))
	      return -1;
	    if (point_cmp(&u, &v) == 0) {
	      if (point_cmp(line_get_b(l), line_get_a(m)) == 0) {
		point_copy(p, line_get_b(l));
		return 1;
	      }
	      if (point_cmp(line_get_a(l), line_get_b(m)) == 0) {
		point_copy(p, line_get_a(l));
		return 1;
	      }
	      return INT_MAX;
	    } else {
	      if (point_cmp(line_get_a(l), line_get_a(m)) == 0) {
		point_copy(p, line_get_a(l));
		return 1;
	      }
	      if (point_cmp(line_get_b(l), line_get_b(m)) == 0) {
		point_copy(p, line_get_b(l));
		return 1;
	      }
	      return INT_MAX;
	    }
	  }
	} else {
	  abort();
	  return -1;
	}
      } else {
	abort();
	return -1;
      }
    }
    return -1;
  }

  t3 /= det;
  t4 /= det;

  if (abs(t1 - t3) < REAL_EPSILON && abs(t2 - t4) < REAL_EPSILON) {
    point_put_x(point_get_x(&a) + t1 * point_get_x(&u), p);
    point_put_y(point_get_y(&a) + t1 * point_get_y(&u), p);
    point_put_z(point_get_z(&a) + t1 * point_get_z(&u), p);

    if (point_in_line(p, l) && point_in_line(p, m))
      return 1;
  }

  return -1;
}

