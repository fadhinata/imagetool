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
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <pixmap/dwordmap.h>
#include <geometry/rotate.h>

static const double VERY_SMALL_ANGLE = 0.001;  /* radians; ~0.06 degrees */
static const double MAX_2_SHEAR_ANGLE = 0.05;  /* radians; ~3 degrees    */

#define PIXMAP_ROTATE90(pixmap, q, qxorg, qyorg, p, pxorg, pyorg) {	\
    int i, j, w, h;							\
    int l, r, t, b;							\
    int dl, dr, dt, db;							\
									\
    assert(q);								\
    assert(qxorg >= 0 && qxorg < pixmap##_get_width(q));		\
    assert(qyorg >= 0 && qyorg < pixmap##_get_height(q));		\
    assert(p);								\
    assert(pxorg >= 0 && pxorg < pixmap##_get_width(p));		\
    assert(pyorg >= 0 && pyorg < pixmap##_get_height(p));		\
									\
    l = qxorg - ((pixmap##_get_height(p) - 1) - pyorg);			\
    r = qxorg + pyorg;							\
    t = qyorg - pxorg;							\
    b = qyorg + ((pixmap##_get_width(p) - 1) - pxorg);			\
									\
    if (l < 0) {							\
      dl = -l;								\
      l = 0;								\
    } else {								\
      dl = 0;								\
    }									\
									\
    if (r >= pixmap##_get_width(q)) {					\
      dr = r - pixmap##_get_width(q);					\
      r = pixmap##_get_width(q) - 1;					\
    } else {								\
      dr = 0;								\
    }									\
									\
    if (t < 0) {							\
      dt = -t;								\
      t = 0;								\
    } else {								\
      dt = 0;								\
    }									\
									\
    if (b >= pixmap##_get_height(q)) {					\
      db = b - pixmap##_get_height(q);					\
      b = pixmap##_get_height(q) - 1;					\
    } else {								\
      db = 0;								\
    }									\
									\
    w = r - l + 1;							\
    h = b - t + 1;							\
    for (i = 0; i < h; i++) {						\
      for (j = 0; j < w; j++) {						\
	pixmap##_put_value(pixmap##_get_value(p, dt + i, (pixmap##_get_height(p) - 1) - (dl + j)), q, l + j, t + i); \
      }									\
    }									\
  }

void bitmap_rotate90(bitmap_t *q, int qxorg, int qyorg, bitmap_t *p, int pxorg, int pyorg)
{
  PIXMAP_ROTATE90(bitmap, q, qxorg, qyorg, p, pxorg, pyorg);
}

void bytemap_rotate90(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg)
{
  PIXMAP_ROTATE90(bytemap, q, qxorg, qyorg, p, pxorg, pyorg);
}

void wordmap_rotate90(wordmap_t *q, int qxorg, int qyorg, wordmap_t *p, int pxorg, int pyorg)
{
  PIXMAP_ROTATE90(wordmap, q, qxorg, qyorg, p, pxorg, pyorg);
}

void dwordmap_rotate90(dwordmap_t *q, int qxorg, int qyorg, dwordmap_t *p, int pxorg, int pyorg)
{
  PIXMAP_ROTATE90(dwordmap, q, qxorg, qyorg, p, pxorg, pyorg);
}

#define PIXMAP_ROTATE180(pixmap, q, qxorg, qyorg, p, pxorg, pyorg) {	\
    int i, j, w, h;							\
    int l, r, t, b;							\
    int dl, dr, dt, db;							\
									\
    assert(q);								\
    assert(qxorg >= 0 && qxorg < pixmap##_get_width(q));		\
    assert(qyorg >= 0 && qyorg < pixmap##_get_height(q));		\
    assert(p);								\
    assert(pxorg >= 0 && pxorg < pixmap##_get_width(p));		\
    assert(pyorg >= 0 && pyorg < pixmap##_get_height(p));		\
									\
    l = qxorg - ((pixmap##_get_width(p) - 1) - pxorg);			\
    r = qxorg + pxorg;							\
    t = qyorg - ((pixmap##_get_height(p) - 1) - pyorg);			\
    b = qyorg + pyorg;							\
									\
    if (l < 0) {							\
      dl = -l;								\
      l = 0;								\
    } else {								\
      dl = 0;								\
    }									\
									\
    if (r >= pixmap##_get_width(q)) {					\
      dr = r - pixmap##_get_width(q);					\
      r = pixmap##_get_width(q) - 1;					\
    } else {								\
      dr = 0;								\
    }									\
									\
    if (t < 0) {							\
      dt = -t;								\
      t = 0;								\
    } else {								\
      dt = 0;								\
    }									\
									\
    if (b >= pixmap##_get_height(q)) {					\
      db = b - pixmap##_get_height(q);					\
      b = pixmap##_get_height(q) - 1;					\
    } else {								\
      db = 0;								\
    }									\
									\
    w = r - l + 1;							\
    h = b - t + 1;							\
    for (i = 0; i < h; i++) {						\
      for (j = 0; j < w; j++) {						\
	pixmap##_put_value(pixmap##_get_value(p, (pixmap##_get_width(p)-1) - (dl + j), (pixmap##_get_height(p) - 1) - (dt + i)), q, l + j, t + i); \
      }									\
    }									\
  }

void bitmap_rotate180(bitmap_t *q, int qxorg, int qyorg, bitmap_t *p, int pxorg, int pyorg)
{
  PIXMAP_ROTATE180(bitmap, q, qxorg, qyorg, p, pxorg, pyorg);
}

void bytemap_rotate180(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg)
{
  PIXMAP_ROTATE180(bytemap, q, qxorg, qyorg, p, pxorg, pyorg);
}

void wordmap_rotate180(wordmap_t *q, int qxorg, int qyorg, wordmap_t *p, int pxorg, int pyorg)
{
  PIXMAP_ROTATE180(wordmap, q, qxorg, qyorg, p, pxorg, pyorg);
}

void dwordmap_rotate180(dwordmap_t *q, int qxorg, int qyorg, dwordmap_t *p, int pxorg, int pyorg)
{
  PIXMAP_ROTATE180(dwordmap, q, qxorg, qyorg, p, pxorg, pyorg);
}

#define PIXMAP_HSHEAR(pixmap, q, qxorg, qyorg, p, pxorg, pyorg, ang) {	\
    int dir_of_xshift;							\
    int dh, int_dy_per_xunit;						\
    real_t dx_per_yunit, dy_per_xunit;					\
    int pxbase, pybase, qxbase, qybase;					\
    int px, py, qx, qy, shift;						\
									\
    assert(q);								\
    assert(qxorg >= 0 && qxorg < pixmap##_get_width(p));		\
    assert(qyorg >= 0 && qyorg < pixmap##_get_height(p));		\
    assert(p);								\
    assert(pxorg >= 0 && pxorg < pixmap##_get_width(q));		\
    assert(pyorg >= 0 && pyorg < pixmap##_get_height(q));		\
									\
    dir_of_xshift = sgn(ang);						\
    dx_per_yunit = abs(tan(ang));					\
  									\
    if (abs(ang) < REAL_EPSILON || abs(dx_per_yunit) < REAL_EPSILON) {	\
      pixmap##_copy(q, qxorg - pxorg, qyorg - pxorg, p, 0, 0, pixmap##_get_width(p), pixmap##_get_height(p)); \
      return 0;								\
    }									\
									\
    dy_per_xunit = 1.0 / dx_per_yunit;					\
    int_dy_per_xunit = (int)round(dy_per_xunit);			\
									\
    pxbase = pxorg - pxorg; pybase = pyorg - (int_dy_per_xunit >> 1);	\
    qxbase = qxorg - pxorg; qybase = qyorg - (int_dy_per_xunit >> 1);	\
    pixmap##_copy(q, qxbase, qybase, p, pxbase, pybase, pixmap##_get_width(p), int_dy_per_xunit); \
									\
    shift = 1;								\
    px = pxbase; py = pybase + int_dy_per_xunit;			\
    qx = qxbase; qy = qybase + int_dy_per_xunit;			\
    while (py < pixmap##_get_height(p) && qy < pixmap##_get_height(q)) { \
      dh = (int)round(dy_per_xunit * shift) - (py - pyorg);		\
      if (dh >= 1) {							\
	qx = qxbase + dir_of_xshift * shift;				\
	pixmap##_copy(q, qx, qy, p, px, py, pixmap##_get_width(p), dh);	\
	py += dh;							\
	qy += dh;							\
      }									\
      shift++;								\
    }									\
									\
    shift = 1;								\
    px = pxbase; py = pybase;						\
    qx = qxbase; qy = qybase;						\
    while (py >= 0 && qy >= 0) {					\
      dh = (int)round(dy_per_xunit * shift) - (pyorg - py);		\
      if (dh >= 1) {							\
	qx = qxbase - dir_of_xshift * shift;				\
	pixmap##_copy(q, qx, qy - dh, p, px, py - dh, pixmap##_get_width(p), dh); \
	py -= dh;							\
	qy -= dh;							\
      }									\
      shift++;								\
    }									\
									\
    return 1;								\
  }

int bitmap_hshear(bitmap_t *q, int qxorg, int qyorg, bitmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_HSHEAR(bitmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

int bytemap_hshear(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_HSHEAR(bytemap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

int wordmap_hshear(wordmap_t *q, int qxorg, int qyorg, wordmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_HSHEAR(wordmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

int dwordmap_hshear(dwordmap_t *q, int qxorg, int qyorg, dwordmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_HSHEAR(dwordmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

/*
int bytemap_hshear(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang)
{
  int dir_of_xshift;
  int dh, int_dy_per_xunit;
  real_t dx_per_yunit, dy_per_xunit;
  int pxbase, pybase, qxbase, qybase;
  int px, py, qx, qy, shift;

  dir_of_xshift = sgn(ang);
  dx_per_yunit = abs(tan(ang));

  if (abs(ang) < REAL_EPSILON || abs(dx_per_yunit) < REAL_EPSILON) {
    bytemap_copy(q, qxorg - pxorg, qyorg - pxorg, p, 0, 0, bytemap_get_width(p), bytemap_get_height(p));
    return 0;
  }

  dy_per_xunit = 1.0 / dx_per_yunit;
  int_dy_per_xunit = (int)round(dy_per_xunit);

  // copy base band; block not vertically sheared
  pxbase = pxorg - pxorg; pybase = pyorg - (int_dy_per_xunit >> 1);
  qxbase = qxorg - pxorg; qybase = qyorg - (int_dy_per_xunit >> 1);
  bytemap_copy(q, qxbase, qybase, p, pxbase, pybase, bytemap_get_width(p), int_dy_per_xunit);

  // for the upper half of pixmap
  shift = 1;
  px = pxbase; py = pybase + int_dy_per_xunit;
  qx = qxbase; qy = qybase + int_dy_per_xunit;
  while (py < bytemap_get_height(p) && qy < bytemap_get_height(q)) {
    dh = (int)round(dy_per_xunit * shift) - (py - pyorg);
    if (dh >= 1) {
      qx = qxbase + dir_of_xshift * shift;
      bytemap_copy(q, qx, qy, p, px, py, bytemap_get_width(p), dh);
      py += dh;
      qy += dh;
    }
    shift++;
  }

  // for the lower half of pixmap
  shift = 1;
  px = pxbase; py = pybase;
  qx = qxbase; qy = qybase;
  while (py >= 0 && qy >= 0) {
    dh = (int)round(dy_per_xunit * shift) - (pyorg - py);
    if (dh >= 1) {
      qx = qxbase - dir_of_xshift * shift;
      bytemap_copy(q, qx, qy - dh, p, px, py - dh, bytemap_get_width(p), dh);
      py -= dh;
      qy -= dh;
    }
    shift++;
  }

  return 1;
}
*/

#define PIXMAP_VSHEAR(pixmap, q, qxorg, qyorg, p, pxorg, pyorg, ang) {	\
    int dir_of_yshift;							\
    int dw, int_dx_per_yunit;						\
    real_t dx_per_yunit, dy_per_xunit;					\
    int pxbase, pybase, qxbase, qybase;					\
    int px, py, qx, qy, shift;						\
									\
    assert(q);								\
    assert(qxorg >= 0 && qxorg < pixmap##_get_width(q));		\
    assert(qyorg >= 0 && qyorg < pixmap##_get_height(q));		\
    assert(p);								\
    assert(pxorg >= 0 && pxorg < pixmap##_get_width(p));		\
    assert(pyorg >= 0 && pyorg < pixmap##_get_height(p));		\
									\
    dir_of_yshift = sgn(ang);						\
    dy_per_xunit = abs(tan(ang));					\
									\
    if (abs(ang) < REAL_EPSILON || abs(dy_per_xunit) < REAL_EPSILON) {	\
      pixmap##_copy(q, qxorg - pxorg, qyorg - pyorg, p, 0, 0, pixmap##_get_width(p), pixmap##_get_height(p)); \
      return 0;								\
    }									\
									\
    dx_per_yunit = 1.0 / dy_per_xunit;					\
    int_dx_per_yunit = (int)round(dx_per_yunit);			\
									\
    pxbase = pxorg - (int_dx_per_yunit >> 1); pybase = pyorg - pyorg;	\
    qxbase = qxorg - (int_dx_per_yunit >> 1); qybase = qyorg - pyorg;	\
    pixmap##_copy(q, qxbase, qybase, p, pxbase, pybase, int_dx_per_yunit, pixmap##_get_height(p)); \
									\
    shift = 1;								\
    px = pxbase + int_dx_per_yunit; py = pybase;			\
    qx = qxbase + int_dx_per_yunit; qy = qybase;			\
    while (px < pixmap##_get_width(p) && qx < pixmap##_get_width(q)) {	\
      dw = (int)round(dx_per_yunit * shift) - (px - pxorg);		\
      if (dw >= 1) {							\
	qy = qybase + dir_of_yshift * shift;				\
	pixmap##_copy(q, qx, qy, p, px, py, dw, pixmap##_get_height(p)); \
	px += dw;							\
	qx += dw;							\
      }									\
      shift++;								\
    }									\
									\
    shift = 1;								\
    px = pxbase; py = pybase;						\
    qx = qxbase; qy = qybase;						\
    while (px >= 0 && qx >= 0) {					\
      dw = (int)round(dx_per_yunit * shift) - (pxorg - px);		\
      if (dw >= 1) {							\
	qy = qybase - dir_of_yshift * shift;				\
	pixmap##_copy(q, qx - dw, qy, p, px - dw, py, dw, pixmap##_get_height(p)); \
	px -= dw;							\
	qx -= dw;							\
      }									\
      shift++;								\
    }									\
									\
    return 1;								\
  }

int bitmap_vshear(bitmap_t *q, int qxorg, int qyorg, bitmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_VSHEAR(bitmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

int bytemap_vshear(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_VSHEAR(bytemap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

int wordmap_vshear(wordmap_t *q, int qxorg, int qyorg, wordmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_VSHEAR(wordmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

int dwordmap_vshear(dwordmap_t *q, int qxorg, int qyorg, dwordmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_VSHEAR(dwordmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

/*
int bytemap_vshear(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang)
{
  int dir_of_yshift;
  int dw, int_dx_per_yunit;
  real_t dx_per_yunit, dy_per_xunit;
  int pxbase, pybase, qxbase, qybase;
  int px, py, qx, qy, shift;

  assert(q);  
  assert(p);

  dir_of_yshift = sgn(ang);
  dy_per_xunit = abs(tan(ang));

  if (abs(ang) < REAL_EPSILON || abs(dy_per_xunit) < REAL_EPSILON) {
    // just copy because of zero angle
    bytemap_copy(q, qxorg - pxorg, qyorg - pyorg,
                 p, 0, 0, bytemap_get_width(p), bytemap_get_height(p));
    return 0;
  }

  dx_per_yunit = 1.0 / dy_per_xunit;
  int_dx_per_yunit = (int)round(dx_per_yunit);

  // copy base band; block not vertically sheared
  pxbase = pxorg - (int_dx_per_yunit >> 1); pybase = pyorg - pyorg;
  qxbase = qxorg - (int_dx_per_yunit >> 1); qybase = qyorg - pyorg;
  bytemap_copy(q, qxbase, qybase, p, pxbase, pybase, int_dx_per_yunit, bytemap_get_height(p));

  // for the right half of pixmap
  shift = 1;
  px = pxbase + int_dx_per_yunit; py = pybase;
  qx = qxbase + int_dx_per_yunit; qy = qybase;
  while (px < bytemap_get_width(p) && qx < bytemap_get_width(q)) {
    dw = (int)round(dx_per_yunit * shift) - (px - pxorg);
    if (dw >= 1) {
      qy = qybase + dir_of_yshift * shift;
      bytemap_copy(q, qx, qy, p, px, py, dw, bytemap_get_height(p));
      px += dw;
      qx += dw;
    }
    shift++;
  }

  // for the left half of pixmap
  shift = 1;
  px = pxbase; py = pybase;
  qx = qxbase; qy = qybase;
  while (px >= 0 && qx >= 0) {
    dw = (int)round(dx_per_yunit * shift) - (pxorg - px);
    if (dw >= 1) {
      qy = qybase - dir_of_yshift * shift;
      bytemap_copy(q, qx - dw, qy, p, px - dw, py, dw, bytemap_get_height(p));
      px -= dw;
      qx -= dw;
    }
    shift++;
  }

  return 1;
}
*/

#define PIXMAP_ROTATE_BY_2SHEAR(pixmap, q, qxorg, qyorg, p, pxorg, pyorg, ang) { \
    pixmap##_t *canvas;							\
    int radius;								\
									\
    assert(q);								\
    assert(qxorg >= 0 && qxorg < pixmap##_get_width(q));		\
    assert(qyorg >= 0 && qyorg < pixmap##_get_height(q));		\
    assert(p);								\
    assert(pxorg >= 0 && pxorg < pixmap##_get_width(p));		\
    assert(pyorg >= 0 && pyorg < pixmap##_get_height(p));		\
									\
    radius = (int)round(sqrt(sqr(max(pxorg, pixmap##_get_width(p) - pxorg)) + sqr(max(pyorg, pixmap##_get_height(p) - pyorg)))); \
									\
    canvas = pixmap##_new(2 * radius, 2 * radius);			\
									\
    pixmap##_hshear(canvas, pixmap##_get_width(canvas) / 2, pixmap##_get_height(canvas) / 2, p, pxorg, pyorg, -ang); \
    pixmap##_vshear(q, qxorg, qyorg, canvas, pixmap##_get_width(canvas) / 2, pixmap##_get_height(canvas) / 2, ang); \
									\
    pixmap##_destroy(canvas);						\
  }

void bitmap_rotate_by_2shear(bitmap_t *q, int qxorg, int qyorg, bitmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_2SHEAR(bitmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

void bytemap_rotate_by_2shear(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_2SHEAR(bytemap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

void wordmap_rotate_by_2shear(wordmap_t *q, int qxorg, int qyorg, wordmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_2SHEAR(wordmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

void dwordmap_rotate_by_2shear(dwordmap_t *q, int qxorg, int qyorg, dwordmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_2SHEAR(dwordmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

/*
void bytemap_rotate_by_2shear(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang)
{
  bytemap_t *canvas;
  int radius;

  assert(p);
  assert(q);

  while (ang >= 2 * M_PI) ang -= 2 * M_PI;

  radius = (int)round(sqrt(sqr(max(pxorg, bytemap_get_width(p) - pxorg)) +
			   sqr(max(pyorg, bytemap_get_height(p) - pyorg))));

  canvas = bytemap_new(2 * radius, 2 * radius);

  bytemap_hshear(canvas, bytemap_get_width(canvas) / 2, bytemap_get_height(canvas) / 2, p, pxorg, pyorg, -ang);
  bytemap_vshear(q, qxorg, qyorg, canvas, bytemap_get_width(canvas) / 2, bytemap_get_height(canvas) / 2, ang);

  bytemap_destroy(canvas);
}
*/

#define PIXMAP_ROTATE_BY_3SHEAR(pixmap, q, qxorg, qyorg, p, pxorg, pyorg, ang) { \
    pixmap##_t *r1, *r2;						\
    int radius;								\
    real_t hangle;							\
									\
    assert(q);								\
    assert(qxorg >= 0 && qxorg < pixmap##_get_width(q));		\
    assert(qyorg >= 0 && qyorg < pixmap##_get_height(q));		\
    assert(p);								\
    assert(pxorg >= 0 && pxorg < pixmap##_get_width(p));		\
    assert(pyorg >= 0 && pyorg < pixmap##_get_height(p));		\
									\
    radius = (int)round(sqrt(sqr(max(pxorg, pixmap##_get_width(p) - pxorg)) + sqr(max(pyorg, pixmap##_get_height(p) - pyorg)))); \
									\
    r1 = pixmap##_new(2 * radius, 2 * radius);				\
    r2 = pixmap##_new(2 * radius, 2 * radius);				\
									\
    pixmap##_vshear(r1, pixmap##_get_width(r1) / 2, pixmap##_get_height(r1) / 2, p, pxorg, pyorg, ang / 2.0); \
									\
    hangle = atan(sin(ang));						\
    pixmap##_hshear(r2, pixmap##_get_width(r2) / 2, pixmap##_get_height(r2) / 2, r1, pixmap##_get_width(r1) / 2, pixmap##_get_height(r1) / 2, -hangle); \
    pixmap##_vshear(q, qxorg, qyorg, r2, pixmap##_get_width(r2) / 2, pixmap##_get_height(r2) / 2, ang / 2.0); \
									\
    pixmap##_destroy(r2);						\
    pixmap##_destroy(r1);						\
  }

void bitmap_rotate_by_3shear(bitmap_t *q, int qxorg, int qyorg, bitmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_3SHEAR(bitmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

void bytemap_rotate_by_3shear(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_3SHEAR(bytemap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

void wordmap_rotate_by_3shear(wordmap_t *q, int qxorg, int qyorg, wordmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_3SHEAR(wordmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

void dwordmap_rotate_by_3shear(dwordmap_t *q, int qxorg, int qyorg, dwordmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_3SHEAR(dwordmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

/*
void bytemap_rotate_by_3shear(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang)
{
  bytemap_t *r1, *r2;
  int radius;
  real_t hangle;
	
  assert(q);
  assert(p);

  while (ang >= 2 * M_PI) ang -= 2 * M_PI;
	
  radius = (int)round(sqrt(sqr(max(pxorg, bytemap_get_width(p) - pxorg)) +
			   sqr(max(pyorg, bytemap_get_height(p) - pyorg))));

  r1 = bytemap_new(2 * radius, 2 * radius);
  r2 = bytemap_new(2 * radius, 2 * radius);

  bytemap_vshear(r1, bytemap_get_width(r1) / 2, bytemap_get_height(r1) / 2,
		 p, pxorg, pyorg, ang / 2.0);

  hangle = atan(sin(ang));
  bytemap_hshear(r2, bytemap_get_width(r2) / 2, bytemap_get_height(r2) / 2,
		 r1, bytemap_get_width(r1) / 2, bytemap_get_height(r1) / 2, -hangle);
  bytemap_vshear(q, qxorg, qyorg, r2, bytemap_get_width(r2) / 2, bytemap_get_height(r2) / 2, ang / 2.0);

  bytemap_destroy(r2);
  bytemap_destroy(r1);
}
*/

#define PIXMAP_ROTATE_BY_SHEAR(pixmap, q, qxorg, qyorg, p, pxorg, pyorg, ang) {	\
    pixmap##_t *a = NULL, *b = NULL;					\
    int radius;								\
									\
    assert(q);								\
    assert(qxorg >= 0 && qxorg < pixmap##_get_width(q));		\
    assert(qyorg >= 0 && qyorg < pixmap##_get_height(q));		\
    assert(p);								\
    assert(pxorg >= 0 && pxorg < pixmap##_get_width(p));		\
    assert(pyorg >= 0 && pyorg < pixmap##_get_height(p));		\
									\
    radius = (int)round(sqrt(sqr(max(pxorg, pixmap##_get_width(p) - pxorg)) + sqr(max(pyorg, pixmap##_get_height(p) - pyorg)))); \
									\
    while (ang >= 2 * M_PI) ang -= 2 * M_PI;				\
    while (ang < 0) ang += 2 * M_PI;					\
									\
    if (ang >= M_PI) {							\
      a = pixmap##_new(2 * radius, 2 * radius);				\
      pixmap##_rotate180(a, pixmap##_get_width(a) / 2, pixmap##_get_height(a) / 2, p, pxorg, pyorg); \
      printf("rotate180\n");						\
      ang -= M_PI;							\
      if (ang >= M_PI/2) {						\
	b = pixmap##_new(2 * radius, 2 * radius);			\
	pixmap##_rotate90(b, pixmap##_get_width(b) / 2, pixmap##_get_height(b) / 2, a, pixmap##_get_width(a) / 2, pixmap##_get_height(a) / 2); \
	printf("rotate90\n");						\
	ang -= M_PI/2;							\
	p = b;								\
	pxorg = pixmap##_get_width(b) / 2;				\
	pyorg = pixmap##_get_height(b) / 2;				\
      }	else {								\
	p = a;								\
	pxorg = pixmap##_get_width(a) / 2;				\
	pyorg = pixmap##_get_height(a) / 2;				\
      }									\
    }									\
									\
    if (abs(ang) <= MAX_2_SHEAR_ANGLE) {				\
      pixmap##_rotate_by_2shear(q, qxorg, qyorg, p, pxorg, pyorg, ang);	\
    } else {								\
      pixmap##_rotate_by_3shear(q, qxorg, qyorg, p, pxorg, pyorg, ang);	\
    }									\
									\
    if (a) pixmap##_destroy(a);						\
    if (b) pixmap##_destroy(b);						\
  }

void bitmap_rotate_by_shear(bitmap_t *q, int qxorg, int qyorg, bitmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_SHEAR(bitmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

void bytemap_rotate_by_shear(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_SHEAR(bytemap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

void wordmap_rotate_by_shear(wordmap_t *q, int qxorg, int qyorg, wordmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_SHEAR(wordmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

void dwordmap_rotate_by_shear(dwordmap_t *q, int qxorg, int qyorg, dwordmap_t *p, int pxorg, int pyorg, real_t ang)
{
  PIXMAP_ROTATE_BY_SHEAR(dwordmap, q, qxorg, qyorg, p, pxorg, pyorg, ang);
}

/*
void bytemap_rotate_by_shear(bytemap_t *q, int qxorg, int qyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang)
{
  assert(q);
  assert(p);

  if (abs(ang) <= MAX_2_SHEAR_ANGLE) {
    bytemap_rotate_by_2shear(q, qxorg, qyorg, p, pxorg, pyorg, ang);
  } else {
    bytemap_rotate_by_3shear(q, qxorg, qyorg, p, pxorg, pyorg, ang);
  }
}
*/
