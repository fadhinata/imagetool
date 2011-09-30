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
#include <assert.h>
#include <malloc.h>
#include <string.h>

#include <common.h>
#include <bitmap.h>
#include <bytemap.h>
#include <rotate.h>

int bitmap_hshear(bitmap_t *q, int xcent, int ycent, bitmap_t *p, int xorg, int yorg, double ang)
{
  int sign_of_dx, y_a_unit_x;
  real_t dx_a_unit_y, abs_dy_a_unit_x;
  int xbase1, ybase1, xbase2, ybase2, wbase, hbase;
  int x1, y1, x2, y2, shift, dy_for_xshift;
	
  assert(q);
  assert(p);

  sign_of_dx = (ang > 0.0 ? +1 : -1);
  dx_a_unit_y = tan(ang); // delta x per 1 y-shift
  abs_dy_a_unit_x = abs(1.0/dx_a_unit_y);

  if (ang == 0.0 || dx_a_unit_y == 0.0) { // just copy because of zero angle
    bitmap_copy(q, xcent - xorg, ycent - yorg,
                p, 0, 0, bitmap_get_width(p), bitmap_get_height(p));
    return 0;
  }
  
  y_a_unit_x = (int)floor(abs_dy_a_unit_x+0.5);
  
  // base band copy
  xbase1 = 0/*xorg-xorg */;
  ybase1 = yorg - (y_a_unit_x >> 1);
  xbase2 = xcent - xorg;
  ybase2 = ycent - (y_a_unit_x >> 1);
  wbase = bitmap_get_width(p);
  hbase = y_a_unit_x;
  bitmap_copy(q, xbase2, ybase2, p, xbase1, ybase1, wbase, hbase);

  // for the upper half of pixmap
  shift = 1;
  x1 = xbase1, y1 = ybase1 + hbase;
  x2 = xbase2, y2 = ybase2 + hbase;
  while (y1 < bitmap_get_height(p) && y2 < bitmap_get_height(q)) {
    dy_for_xshift = (int)floor(abs_dy_a_unit_x * (shift + 0.5) + 0.5) - (y1 - yorg);
    if (dy_for_xshift >= 1) {
      x2 = xbase2 - sign_of_dx * shift;
      bitmap_copy(q, x2, y2, p, x1, y1, bitmap_get_width(p), dy_for_xshift);
      y1 += dy_for_xshift, y2 += dy_for_xshift;
    }
    shift++;
  }
	
  // for the lower half of pixmap
  shift = 1;
  x1 = xbase1, y1 = ybase1;
  x2 = xbase2, y2 = ybase2;
  while (y1 >= 0 && y2 >= 0) {
    dy_for_xshift = (int)floor(abs_dy_a_unit_x * (shift + 0.5) + 0.5) - (yorg - y1);
    if (dy_for_xshift >= 1) {
      x2 = xbase2 + sign_of_dx * shift;
      bitmap_copy(q, x2, y2 - dy_for_xshift, p, x1, y1 - dy_for_xshift, bitmap_get_width(p), dy_for_xshift);
      y1 -= dy_for_xshift, y2 -= dy_for_xshift;
    }
    shift++;
  }

  return 1;
}

int bitmap_vshear(bitmap_t *q, int xcent, int ycent, bitmap_t *p, int xorg, int yorg, double ang)
{
  int sign_of_dy, x_a_unit_y;
  real_t dy_a_unit_x, abs_dx_a_unit_y;
  int xbase1, ybase1, xbase2, ybase2, wbase, hbase;
  int x1, y1, x2, y2, shift, dx_for_yshift;
	
  assert(q);
  assert(p);

  sign_of_dy = (ang > 0.0 ? +1 : -1);
  dy_a_unit_x = tan(ang); // delta x per 1 y-shift
  abs_dx_a_unit_y = abs(1.0 / dy_a_unit_x);

  if (ang == 0.0 || dy_a_unit_x == 0.0) { // just copy because of zero angle
    bitmap_copy(q, xcent - xorg, ycent - yorg,
                p, 0, 0, bitmap_get_width(p), bitmap_get_height(p));
    return 0;
  }

  x_a_unit_y = (int)floor(abs_dx_a_unit_y + 0.5);
	
  // base band copy
  xbase1 = xorg - (x_a_unit_y >> 1);
  ybase1 = 0/*yorg-yorg*/;
  xbase2 = xcent - (x_a_unit_y >> 1);
  ybase2 = ycent - yorg;
  wbase = x_a_unit_y;
  hbase = bitmap_get_height(p);
  bitmap_copy(q, xbase2, ybase2, p, xbase1, ybase1, wbase, hbase);

  // for the right half of pixmap
  shift = 1;
  x1 = xbase1 + wbase, y1 = ybase1;
  x2 = xbase2 + wbase, y2 = ybase2;
  while (x1 < bitmap_get_width(p) && x2 < bitmap_get_width(q)) {
    dx_for_yshift = (int)floor(abs_dx_a_unit_y * (shift + 0.5) + 0.5) - (x1 - xorg);
    if (dx_for_yshift >= 1) {
      y2 = ybase2 - sign_of_dy * shift;
      bitmap_copy(q, x2, y2, p, x1, y1, dx_for_yshift, bitmap_get_height(p));
      x1 += dx_for_yshift, x2 += dx_for_yshift;
    }
    shift++;
  }

  // for the left half of pixmap
  shift = 1;
  x1 = xbase1, y1 = ybase1;
  x2 = xbase2, y2 = ybase2;
  while (x1 >= 0 && x2 >= 0) {
    dx_for_yshift = (int)floor(abs_dx_a_unit_y * (shift + 0.5) + 0.5) - (xorg - x1);
    if (dx_for_yshift >= 1) {
      y2 = ybase2 + sign_of_dy * shift;
      bitmap_copy(q, x2 - dx_for_yshift, y2,
                  p, x1 - dx_for_yshift, y1, dx_for_yshift, bitmap_get_height(p));
      x1 -= dx_for_yshift, x2 -= dx_for_yshift;
    }
    shift++;
  }

  return 1;
}

static const double VERY_SMALL_ANGLE = 0.001;  /* radians; ~0.06 degrees */
static const double MAX_2_SHEAR_ANGLE = 0.05;  /* radians; ~3 degrees    */

int bitmap_rotate_by_2shear(bitmap_t *q, int xcent, int ycent, bitmap_t *p, int xorg, int yorg, real_t ang)
{
  bitmap_t *r;
  int radius;

  assert(q);
  assert(p);

  if (abs(ang) < VERY_SMALL_ANGLE) {
    bitmap_copy(q, xcent - xorg, ycent - yorg,
                p, 0, 0, bitmap_get_width(p), bitmap_get_height(p));
    return 0;
  }
	
  radius = (int)floor(sqrt(sqr(max(xorg, bitmap_get_width(p) - xorg)) +
                           sqr(max(yorg, bitmap_get_height(p) - yorg))) + 0.5);
	
  r = bitmap_new(2 * radius, 2 * radius);
  if (bitmap_hshear(r, bitmap_get_width(r) / 2, bitmap_get_height(r) / 2,
                    p, xorg, yorg, ang) == 1) {
    bitmap_vshear(q, xcent, ycent,
                  r, bitmap_get_width(r) / 2, bitmap_get_height(r) / 2, ang);
  } else {
    bitmap_copy(q, xcent - bitmap_get_width(r) / 2, ycent - bitmap_get_height(r) / 2,
                r, 0, 0, bitmap_get_width(r), bitmap_get_height(r));
  }

  bitmap_destroy(r);

  return 1;
}

int bitmap_rotate_by_3shear(bitmap_t *q, int xcent, int ycent, bitmap_t *p, int xorg, int yorg, real_t ang)
{
  bitmap_t *r1, *r2;
  int radius;
  real_t hangle;
	
  assert(q);
  assert(p);
  
  if (abs(ang) < VERY_SMALL_ANGLE) {
    bitmap_copy(q, xcent - xorg, ycent - yorg,
                p, 0, 0, bitmap_get_width(p), bitmap_get_height(p));
    return 0;
  }
  
  radius = (int)floor(sqrt(sqr(max(xorg, bitmap_get_width(p) - xorg)) +
                           sqr(max(yorg, bitmap_get_height(p) - yorg))) + 0.5);
	
  r1 = bitmap_new(2 * radius, 2 * radius);
  if (bitmap_vshear(r1, bitmap_get_width(r1) / 2, bitmap_get_height(r1) / 2,
                    p, xorg, yorg, ang / 2.0) == 1) {
    r2 = bitmap_new(2 * radius, 2 * radius);
    hangle = atan(sin(ang));

    if (bitmap_hshear(r2, bitmap_get_width(r2) / 2, bitmap_get_height(r2) / 2,
                      r1, bitmap_get_width(r1) / 2, bitmap_get_height(r1) / 2, hangle) == 1) {
      bitmap_vshear(q, xcent, ycent,
                    r2, bitmap_get_width(r2) / 2, bitmap_get_height(r2) / 2, ang/2.0);
    } else {
      bitmap_copy(q, xcent - bitmap_get_width(r2) / 2, ycent - bitmap_get_height(r2) / 2,
                  r2, 0, 0, bitmap_get_width(r2), bitmap_get_height(r2));
    }
    bitmap_destroy(r2);
  } else {
    bitmap_copy(q, xcent - bitmap_get_width(r1) / 2, ycent - bitmap_get_height(r1) / 2,
                r1, 0, 0, bitmap_get_width(r1), bitmap_get_height(r1));
  }

  bitmap_destroy(r1);

  return 1;
}

int bitmap_rotate_by_shear(bitmap_t *q, int xcent, int ycent, bitmap_t *p, int xorg, int yorg, real_t ang)
{
  assert(q);
  assert(p);

  if (abs(ang) < VERY_SMALL_ANGLE) {
    bitmap_copy(q, xcent - xorg, ycent - yorg,
                p, 0, 0, bitmap_get_width(p), bitmap_get_height(p));
    return 0;
  }

  if (abs(ang) <= MAX_2_SHEAR_ANGLE)
    return bitmap_rotate_by_2shear(q, xcent, ycent, p, xorg, yorg, ang);

  return bitmap_rotate_by_3shear(q, xcent, ycent, p, xorg, yorg, ang);
}

int bytemap_hshear(bytemap_t *q, int xcent, int ycent, bytemap_t *p, int xorg, int yorg, real_t ang)
{
  int sign_of_dx, y_a_unit_x;
  real_t dx_a_unit_y, abs_dy_a_unit_x;
  int xbase1, ybase1, xbase2, ybase2, wbase, hbase;
  int x1, y1, x2, y2, shift, dy_for_xshift;
	
  assert(q);
  assert(p);

  sign_of_dx = (ang > 0.0 ? +1 : -1);
  dx_a_unit_y = tan(ang); // delta x per 1 y-shift
  abs_dy_a_unit_x = abs(1.0/dx_a_unit_y);

  if (ang == 0.0 || dx_a_unit_y == 0.0) { // just copy because of zero angle
    bytemap_copy(q, xcent - xorg, ycent - yorg,
                 p, 0, 0, bytemap_get_width(p), bytemap_get_height(p));
    return 0;
  }
  
  y_a_unit_x = (int)floor(abs_dy_a_unit_x + 0.5);
  
  // base band copy
  xbase1 = 0/*xorg-xorg */;
  ybase1 = yorg - (y_a_unit_x >> 1);
  xbase2 = xcent - xorg;
  ybase2 = ycent - (y_a_unit_x >> 1);
  wbase = bitmap_get_width(p);
  hbase = y_a_unit_x;
  bytemap_copy(q, xbase2, ybase2, p, xbase1, ybase1, wbase, hbase);

  // for the upper half of pixmap
  shift = 1;
  x1 = xbase1, y1 = ybase1 + hbase;
  x2 = xbase2, y2 = ybase2 + hbase;
  while (y1 < bytemap_get_height(p) && y2 < bytemap_get_height(q)) {
    dy_for_xshift = (int)floor(abs_dy_a_unit_x * (shift + 0.5) + 0.5) - (y1 - yorg);
    if (dy_for_xshift >= 1) {
      x2 = xbase2 - sign_of_dx * shift;
      bytemap_copy(q, x2, y2, p, x1, y1, bytemap_get_width(p), dy_for_xshift);
      y1 += dy_for_xshift, y2 += dy_for_xshift;
    }
    shift++;
  }
	
  // for the lower half of pixmap
  shift = 1;
  x1 = xbase1, y1 = ybase1;
  x2 = xbase2, y2 = ybase2;
  while (y1 >= 0 && y2 >= 0) {
    dy_for_xshift = (int)floor(abs_dy_a_unit_x * (shift + 0.5) + 0.5) - (yorg - y1);
    if (dy_for_xshift >= 1) {
      x2 = xbase2 + sign_of_dx * shift;
      bytemap_copy(q, x2, y2 - dy_for_xshift,
                   p, x1, y1 - dy_for_xshift, bytemap_get_width(p), dy_for_xshift);
      y1 -= dy_for_xshift, y2 -= dy_for_xshift;
    }
    shift++;
  }

  return 1;
}

int bytemap_vshear(bytemap_t *q, int xcent, int ycent, bytemap_t *p, int xorg, int yorg, real_t ang)
{
  int sign_of_dy, x_a_unit_y;
  real_t dy_a_unit_x, abs_dx_a_unit_y;
  int xbase1, ybase1, xbase2, ybase2, wbase, hbase;
  int x1, y1, x2, y2, shift, dx_for_yshift;
	
  assert(q);
  assert(p);

  sign_of_dy = (ang > 0.0 ? +1 : -1);
  dy_a_unit_x = tan(ang); // delta x per 1 y-shift
  abs_dx_a_unit_y = abs(1.0 / dy_a_unit_x);

  if (ang == 0.0 || dy_a_unit_x == 0.0) { // just copy because of zero angle
    bytemap_copy(q, xcent - xorg, ycent - yorg,
                 p, 0, 0, bytemap_get_width(p), bytemap_get_height(p));
    return 0;
  }

  x_a_unit_y = (int)floor(abs_dx_a_unit_y + 0.5);
	
  // base band copy
  xbase1 = xorg - (x_a_unit_y >> 1);
  ybase1 = 0/*yorg-yorg*/;
  xbase2 = xcent - (x_a_unit_y >> 1);
  ybase2 = ycent - yorg;
  wbase = x_a_unit_y, hbase = bytemap_get_height(p);
  bytemap_copy(q, xbase2, ybase2, p, xbase1, ybase1, wbase, hbase);

  // for the right half of pixmap
  shift = 1;
  x1 = xbase1 + wbase, y1 = ybase1;
  x2 = xbase2 + wbase, y2 = ybase2;
  while (x1 < bytemap_get_width(p) && x2 < bytemap_get_width(q)) {
    dx_for_yshift = (int)floor(abs_dx_a_unit_y * (shift + 0.5) + 0.5) - (x1 - xorg);
    if (dx_for_yshift >= 1) {
      y2 = ybase2 - sign_of_dy * shift;
      bytemap_copy(q, x2, y2, p, x1, y1, dx_for_yshift, bytemap_get_height(p));
      x1 += dx_for_yshift, x2 += dx_for_yshift;
    }
    shift++;
  }

  // for the left half of pixmap
  shift = 1;
  x1 = xbase1, y1 = ybase1;
  x2 = xbase2, y2 = ybase2;
  while (x1 >= 0 && x2 >= 0) {
    dx_for_yshift = (int)floor(abs_dx_a_unit_y * (shift + 0.5) + 0.5) - (xorg - x1);
    if (dx_for_yshift >= 1) {
      y2 = ybase2 + sign_of_dy * shift;
      bytemap_copy(q, x2 - dx_for_yshift, y2,
                   p, x1 - dx_for_yshift, y1, dx_for_yshift, bytemap_get_height(p));
      x1 -= dx_for_yshift, x2 -= dx_for_yshift;
    }
    shift++;
  }

  return 1;
}

int bytemap_rotate_by_2shear(bytemap_t *q, int xcent, int ycent, bytemap_t *p, int xorg, int yorg, real_t ang)
{
  bytemap_t *r;
  int radius;

  assert(q);
  assert(p);

  if (abs(ang) < VERY_SMALL_ANGLE) {
    bytemap_copy(q, xcent - xorg, ycent - yorg,
                 p, 0, 0, bytemap_get_width(p), bytemap_get_height(p));
    return 0;
  }
	
  radius = (int)floor(sqrt(sqr(max(xorg, bytemap_get_width(p) - xorg)) +
                           sqr(max(yorg, bytemap_get_height(p) - yorg))) + 0.5);
	
  r = bytemap_new(2 * radius, 2 * radius);
  if (bytemap_hshear(r, bytemap_get_width(r) / 2, bytemap_get_height(r) / 2,
                     p, xorg, yorg, ang) == 1) {
    bytemap_vshear(q, xcent, ycent,
                   r, bytemap_get_width(r) / 2, bytemap_get_height(r) / 2, ang);
  } else {
    bytemap_copy(q, xcent - bytemap_get_width(r) / 2, ycent - bytemap_get_height(r) / 2,
                 r, 0, 0, bytemap_get_width(r), bytemap_get_height(r));
  }
  bytemap_destroy(r);

  return 1;
}

int bytemap_rotate_by_3shear(bytemap_t *q, int xcent, int ycent, bytemap_t *p, int xorg, int yorg, real_t ang)
{
  bytemap_t *r1, *r2;
  int radius;
  real_t hangle;
	
  assert(q);
  assert(p);
	
  if (abs(ang) < VERY_SMALL_ANGLE) {
    bytemap_copy(q, xcent - xorg, ycent - yorg,
                 p, 0, 0, bytemap_get_width(p), bytemap_get_height(p));
    return 0;
  }
	
  radius = (int)floor(sqrt(sqr(max(xorg, bytemap_get_width(p) - xorg)) +
                           sqr(max(yorg, bytemap_get_height(p) - yorg))) + 0.5);
	
  r1 = bytemap_new(2 * radius, 2 * radius);

  if (bytemap_vshear(r1, bytemap_get_width(r1) / 2, bytemap_get_height(r1) / 2,
                     p, xorg, yorg, ang / 2.0) == 1) {
    r2 = bytemap_new(2 * radius, 2 * radius);
    hangle = atan(sin(ang));
    if (bytemap_hshear(r2, bytemap_get_width(r2) / 2, bytemap_get_height(r2) / 2,
                       r1, bytemap_get_width(r1) / 2, bytemap_get_height(r1) / 2, hangle) == 1) {
      bytemap_vshear(q, xcent, ycent, r2, bytemap_get_width(r2) / 2, bytemap_get_height(r2) / 2, ang / 2.0);
    } else {
      bytemap_copy(q, xcent - bytemap_get_width(r2) / 2, ycent - bytemap_get_height(r2) / 2, r2, 0, 0, bytemap_get_width(r2), bytemap_get_height(r2));
    }
    bytemap_destroy(r2);
  } else {
    bytemap_copy(q, xcent - bytemap_get_width(r1) / 2, ycent - bytemap_get_height(r1) / 2, r1, 0, 0, bytemap_get_width(r1), bytemap_get_height(r1));
  }

  bytemap_destroy(r1);

  return 1;
}

int bytemap_rotate_by_shear(bytemap_t *q, int xcent, int ycent, bytemap_t *p, int xorg, int yorg, real_t ang)
{
  assert(q);
  assert(p);

  if (abs(ang) < VERY_SMALL_ANGLE) {
    bytemap_copy(q, xcent-xorg, ycent-yorg, p, 0, 0, p->header.width, p->header.height);
    return 0;
  }

  if (abs(ang) <= MAX_2_SHEAR_ANGLE)
    return bytemap_rotate_by_2shear(q, xcent, ycent, p, xorg, yorg, ang);

  return bytemap_rotate_by_3shear(q, xcent, ycent, p, xorg, yorg, ang);
}

