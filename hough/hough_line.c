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
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <assert.h>

#include <common.h>
#include <hough_line.h>

hough_line_t *hough_line_new(real_t tmin, real_t tmax, int nt, real_t rmin, real_t rmax, int nr)
{
  hough_line_t *hough;

  hough = (hough_line_t *)malloc(sizeof(*hough));
  assert(hough);

  hough->cell = dwordmap_new(nt, nr);

  if (tmin == tmax || tmin > tmax) {
    hough->tmin = 0.0;
    hough->dt = 0.0;
  } else {
    hough->tmin = tmin;
    hough->dt = (tmax - tmin) / (nt - 1);
  }
	
  if (rmin == rmax || rmin > rmax) {
    hough->rmin = 0.0;
    hough->dr = 0.0;
  } else {
    hough->rmin = rmin;
    hough->dr = (rmax - rmin) / (nr - 1);
  }

  return hough;
}

void hough_line_destroy(hough_line_t *hough)
{
  assert(hough);
  dwordmap_destroy(hough->cell);
  free(hough);
}

int hough_line_get_sorted_index(int *tindex, int *rindex, int n, hough_line_t *hough)
{
  int i, j, k, l, cnt;
  uint32_t val, tmp;
  dwordmap_t *cell;
  int w, h;

  assert(tindex);
  assert(rindex);
  assert(n > 0);
  assert(hough);

  cell = hough->cell;

  cnt = 0;
  w = dwordmap_get_width(hough->cell);
  h = dwordmap_get_height(hough->cell);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      val = dwordmap_get_value(j, i, cell);
      //val = *(cell->buffer+i*(cell->header.pitch)/sizeof(*(cell->buffer))+j);
      if (val == 0) continue;
      for (k = 0; k < cnt; k++) {
	tmp = dwordmap_get_value(tindex[k], rindex[k], cell);
	/*
	tmp = *(cell->buffer+
		(rindex[k])*(cell->header.pitch)/sizeof(*(cell->buffer))+
		tindex[k]);
	*/
	if (tmp < val) break;
      }
      //printf("1. k %d cnt %d\n", k, cnt);
      cnt = min(cnt + 1, n);
      if (k >= cnt) continue;
      for (l = k; l < cnt - 1; l++) {
	tindex[l + 1] = tindex[l];
	rindex[l + 1] = rindex[l];
      }
      //printf("2. k %d cnt %d\n", k, cnt);
      tindex[k] = j;
      rindex[k] = i;
    }
  }

  return cnt;
}

/* ax + by + c = 0; */
void hough_line_get_arguments(real_t *a, real_t *b, real_t *c, real_t xcenter, real_t ycenter, int tindex, int rindex, hough_line_t *hough)
{
  real_t theta, radius;

  assert(a);
  assert(b);
  assert(c);
  assert(hough);
  assert(tindex >= 0 && tindex < dwordmap_get_width(hough->cell));
  assert(rindex >= 0 && rindex < dwordmap_get_height(hough->cell));

  theta = hough->tmin + (hough->dt) * tindex;
  radius = hough->rmin + (hough->dr) * rindex;
  /* sin(t)*(y-bin->header.height/2.0) = -cos(t)*(x-bin->header.width/2.0)+r; */
  //y = -cos(t)/sin(t) * (x-bin->header.width/2.0) + r/sin(t) +bin->header.height/2.0
  *a = cos(theta);
  *b = sin(theta);
  *c = -radius - (*a) * xcenter - (*b) * ycenter;

#if 0
  draw_line(0, -cos(t)/sin(t)*(-d->header.width/2.0)+r/sin(t)+d->header.height/2.0,
	    d->header.width-1, -cos(t)/sin(t)*(d->header.width-1-d->header.width/2.0)+r/sin(t)+d->header.height/2.0, 
	    255, c);
#endif
}

void hough_line_accumulate(hough_line_t *hough, bitmap_t *bin)
{
  int x, y, i, j, w, h, pitch;
  real_t xbias, ybias;
  real_t t, r;
  real_t *sintbl, *costbl;
  dwordmap_t *cell;
  uint8_t *buf;
	
  assert(hough);
  assert(hough->cell);
  assert(bin);

  cell = hough->cell;
  if (hough->delta_theta == 0.0) {
    hough->tmin = -M_PI;
    hough->dt = (2 * M_PI) / (dwordmap_get_width(cell) - 1);
  }
  if (hough->dr == 0.0) {
    hough->rmin = -round(sqrt(sqr(bitmap_get_width(bin) / 2.0) + sqr(bitmap_get_height(bin)/2.0)));
    hough->dr = (-2.0 * hough->rmin) / (dwordmap_get_height(cell) - 1);
  }

  sintbl = (real_t *)malloc(dwordmap_get_width(cell) * sizeof(*sintbl));
  costbl = (real_t *)malloc(dwordmap_get_height(cell) * sizeof(*costbl));
  assert(sintbl);
  assert(costbl);

  for (i = 0; i < dwordmap_get_width(cell); i++) {
    t = hough->tmin + (hough->dt) * i;
    sintbl[i] = sin(t);
    costbl[i] = cos(t);
  }

  dwordmap_clear(cell);


  w = bitmap_get_width(bin);
  h = bitmap_get_height(bin);

  pitch = bitmap_get_pitch(bin) / sizeof(*buf);
  buf = bitmap_get_buffer(bin);

  for (y = 0; y < h; y++) {
    ybias = y - h / 2.0;
    for (x = 0; x < w; x++) {
      if (!(*(buf + (x >> 3)) & (1 << (x % 8)))) continue; 
      //if (bitmap_isreset(x, y, bin)) continue;
      xbias = x - w / 2.0;
      for (j = 0; j < dwordmap_get_width(cell); j++) {
	t = hough->tmin + hough->dt * j;
	r = costbl[j] * xbias + sintbl[j] * ybias;
	i = (int)round((r - hough->rmin) / hough->dr);
	if (i >= 0 && i < dwordmap_get_height(cell)) {
	  dwordmap_inc_value(j, i, cell);
	}
      }
    }
    buf += pitch;
  }

  free(costbl);
  free(sintbl);
}

void hough_line_fast_accumulate(hough_line_t *hough, matrix_t *grad, real_t thres)
{
  int i, j, x, y;
  real_t xbias, ybias, xgrad, ygrad, magnitude;
  real_t r, t;
  real_t *gxbuf, *gybuf;
  dwordmap_t *cell;
	
  assert(hough);
  assert(hough->cell);
  assert(grad);
  assert(grad->imaginary);
	
  cell = hough->cell;
  if (hough->dt == 0.0) {
    hough->tmin = -M_PI;
    hough->dt = (2.0 * M_PI) / (dwordmap_get_width(cell) - 1);
  }
  if (hough->dr == 0.0) {
    hough->rmin = -round(sqrt(sqr(matrix_get_rows(grad) / 2.0) + sqr(matrix_get_columns(grad) / 2.0)));
    hough->dr = -2.0 * hough->rmin / (dwordmap_get_height(cell) - 1);
  }

  dwordmap_clear(cell);

  gxbuf = matrix_get_buffer(grad);
  gybuf = imatrix_get_buffer(grad);
  for (y = 0; y < matrix_get_rows(grad); y++) {
    ybias = y - matrix_get_rows(grad) / 2.0;
    for (x = 0; x < matrix_get_columns(grad); x++) {
      // gradiant value threshold to find fixible edge
      xgrad = *(gxbuf + x);
      ygrad = *(gybuf + x);
      magnitude = sqrt(sqr(xgrad) + sqr(ygrad));
      if (magnitude <= thres) continue;
      xbias = x - matrix_get_columns(grad) / 2.0;
      // normal gradient
      xgrad /= magnitude;
      ygrad /= magnitude;
      // edge's arctangent value
      t = atan2(ygrad, xgrad); /* -pi ... pi */
      j = (int)round((t - hough->tmin) / hough->dt);
      if (j >= 0 && j < dwordmap_get_width(cell)) {
	// hough transform
	r = xbias * xgrad + ybias * ygrad; /* -sqrt(sqr(x)+sqr(y)) ... sqrt(sqr(x)+sqr(y))  */
	i = (int)round((r - hough->rmin) / hough->dr);
	if (i >= 0 && i < dwordmap_get_height(cell)) {
	  dwordmap_inc_value(j, i, cell);
	}
      }
    }
    gxbuf += matrix_get_columns(grad);
    gybuf += matrix_get_columns(grad);
  }
}

#if 0
void hough_line_fast_accumulate(hough_line_t *hough, wordmap_t *gx, wordmap_t *gy, real_t thres)
{
  int i, j, x, y;
  real_t xbias, ybias, xgrad, ygrad, magnitude;
  real_t r, t;
  real_t *gxbuf, *gybuf;
  dwordmap_t *cell;
	
  assert(hough);
  assert(hough->cell);
  assert(gx);
  assert(gy);
  assert((gx->header.width == gy->header.width) && (gx->header.height == gy->header.height));
	
  cell = hough->cell;
  if (hough->delta_theta == 0.0) {
    hough->theta_min = -M_PI;
    hough->delta_theta = (2.0*M_PI)/(cell->header.width-1);
  }
  if (hough->delta_radius == 0.0) {
    hough->radius_min = -floor(sqrt(sqr((gx->header.height)/2.0)+sqr((gx->header.width)/2.0))+0.5);
    hough->delta_radius = -2.0*(hough->radius_min)/(cell->header.height-1);
  }

  memset(cell->buffer, 0, cell->header.height*cell->header.pitch);

  gxbuf = gx->buffer;
  gybuf = gy->buffer;
  for (y = 0; y < gx->header.height; y++) {
    ybias = y-(gx->header.height)/2.0;
    for (x = 0; x < gx->header.width; x++) {
      // gradiant value threshold to find fixible edge
      xgrad = *(gxbuf+x);
      ygrad = *(gybuf+x);
      magnitude = sqrt(sqr(xgrad)+sqr(ygrad));
      if (magnitude <= thres) continue;
      xbias = x-(gx->header.width)/2.0;
      // normal gradient
      xgrad /= magnitude;
      ygrad /= magnitude;
      // edge's arctangent value
      t = atan2(ygrad, xgrad); /* -pi ... pi */
      j = (int)floor((t-hough->theta_min)/hough->delta_theta+0.5);
      if (j >= 0 && j < cell->header.width) {
	// hough transform
	r = xbias*xgrad+ybias*ygrad; /* -sqrt(sqr(x)+sqr(y)) ... sqrt(sqr(x)+sqr(y))  */
	i = (int)floor((r-hough->radius_min)/hough->delta_radius+0.5);
	if (i >= 0 && i < cell->header.height) {
	  (*(cell->buffer+i*cell->header.pitch/sizeof(*(cell->buffer))+j))++;
	}
      }
    }
    gxbuf += gx->header.pitch/sizeof(*(gx->buffer));
    gybuf += gy->header.pitch/sizeof(*(gy->buffer));
  }
}
#endif
