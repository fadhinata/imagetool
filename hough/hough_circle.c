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
#include <hough/hough_circle.h>

hough_circle_t *hough_circle_new(real_t xmin, real_t xmax, int nx, real_t ymin, real_t ymax, int ny, real_t rmin, real_t rmax, int nr)
{
  int i;
  hough_circle_t *hough;

  assert(nx > 0);
  assert(ny > 0);
  assert(nr > 0);

  hough = (hough_circle_t *)malloc(sizeof(*hough));
  assert(hough);

  hough->n = nr;

  hough->amin = xmin;
  hough->da = (xmax - xmin) / (nx - 1);

  hough->bmin = ymin;
  hough->db = (ymax - ymin) / (ny - 1);

  hough->rmin = rmin;
  hough->dr = (rmax - rmin) / (nr - 1);

  hough->cell = (dwordmap_t **)malloc(nr * sizeof(dwordmap_t *));
  assert(hough->cell);

  for (i = 0; i < nr; i++)
    *(hough->cell + i) = dwordmap_new(nx, ny);

  return hough;
}

void hough_circle_destroy(hough_circle_t *hough)
{
  int i;

  assert(hough);

  for (i = 0; i < hough->n; i++)
    dwordmap_destroy(*(hough->cell + i));

  free(hough->cell);
  free(hough);
}

int hough_circle_get_sorted_index(int *xindex, int *yindex, int *rindex, int n, hough_circle_t *hough)
{
  int x, y, r, i, j, cnt;
  uint32_t val, tmp;
  dwordmap_t *cell, *cell2;

  assert(xindex);
  assert(yindex);
  assert(rindex);
  assert(n > 0);
  assert(hough);

  cnt = 0;
  for (r = 0; r < hough->n; r++) {
    cell = *(hough->cell + r);
    for (y = 0; y < dwordmap_get_height(cell); y++) {
      for (x = 0; x < dwordmap_get_width(cell); x++) {
	val = dwordmap_get_value(cell, x, y);
	if (val == 0) continue;
	//printf("val %d\n", val);
	// Find a proper bin from the sorted indeces according to val.
	for (i = 0; i < cnt; i++) {
	  cell2 = *(hough->cell + rindex[i]);
	  tmp = dwordmap_get_value(cell2, xindex[i], yindex[i]);
	  if (tmp < val) break;
	}
	//printf("1. i %d cnt %d\n", i, cnt);
	cnt = min(cnt+1, n);
	if (i >= cnt) continue;
	for (j = i; j < cnt-1; j++) {
	  xindex[j+1] = xindex[j];
	  yindex[j+1] = yindex[j];
	  rindex[j+1] = rindex[j];
	}
	//printf("2. i %d cnt %d\n", i, cnt);
	xindex[i] = x;
	yindex[i] = y;
	rindex[i] = r;
      }
    }
  }
  return cnt;
}

// (x - a)^2 + (y - b)^2 = r^2
void hough_circle_get_arguments(real_t *a, real_t *b, real_t *r, real_t xcenter, real_t ycenter, int xindex, int yindex, int rindex, hough_circle_t *hough)
{
  assert(a);
  assert(b);
  assert(r);
  assert(rindex >= 0 && rindex < hough->n);
  assert(xindex >= 0 && xindex < (*(hough->cell))->header.width);
  assert(yindex >= 0 && yindex < (*(hough->cell))->header.height);
	
  *a = hough->amin + (hough->da) * xindex + xcenter;
  *b = hough->bmin + (hough->db) * yindex + ycenter;
  *r = hough->rmin + (hough->dr) * rindex;
  //printf("r(%lf), x(%lf), y(%lf)\n", r, x, y);
  //draw_circle(x+(d->header.width)/2.0, y+(d->header.height)/2.0, r, 128, c);
}

void hough_circle_accumulate(hough_circle_t *hough, bitmap_t *bin)
{
  int x, y, i, j, k;
  int w, h;

  real_t xbias, ybias;
  real_t r;
  real_t dx, dy;

  dwordmap_t *cell;

  assert(hough);
  assert(bin);

  w = bitmap_get_width(bin);
  h = bitmap_get_height(bin);

  // initialize dimension of cell
  if (hough->da <= 0) {
    hough->amin = -w / 2.0;
    hough->da = w / ((*(hough->cell))->header.width - 1);
  }

  if (hough->db <= 0) {
    hough->bmin = -h / 2.0;
    hough->db = h / ((*(hough->cell))->header.height - 1);
  }

  if (hough->dr <= 0) {
    hough->rmin = max(max(hough->da, hough->db) + 1, 2);
    hough->dr = (min(h / 2.0, w / 2.0) - hough->rmin) / (hough->n - 1);
  }

  // clear hough accumulator 
  for (i = 0; i < hough->n; i++) {
    cell = *(hough->cell + i);
    memset(cell->buffer, 0, dwordmap_get_height(cell) * dwordmap_get_pitch(cell));
  }

  for (y = 0; y < h; y++) {
    ybias = (real_t)y - h / 2.0;

    for (x = 0; x < w; x++) {
      if (bitmap_isreset(bin, x, y)) continue;
      //printf(".");

      xbias = (real_t)x - w / 2.0;

      for (k = 0; k < hough->n; k++) {
	r = hough->rmin + hough->dr * k;
	cell = *(hough->cell + k);
	// draw circle with the radius of r on cell 
	// draw 4 vertex
	i = (int)round(((ybias + 0) - hough->bmin) / hough->db);
	if (i >= 0 && i < dwordmap_get_height(cell)) {
	  j = (int)round(((xbias + r) - hough->amin) / hough->da);
	  if (j >= 0 && j < dwordmap_get_width(cell))
	    dwordmap_inc_value(cell, j, i);
	  j = (int)round(((xbias - r) - hough->amin) / hough->da);
	  if (j >= 0 && j < dwordmap_get_width(cell))
	    dwordmap_inc_value(cell, j, i);
	}

	j = (int)round(((xbias + 0) - hough->amin) / hough->da);
	if (j >= 0 && j < dwordmap_get_width(cell)) {
	  i = (int)round(((ybias + r) - hough->bmin) / hough->db);
	  if (i >= 0 && i < dwordmap_get_height(cell))
	    dwordmap_inc_value(cell, j, i);
	  i = (int)round(((ybias - r) - hough->bmin) / hough->db);
	  if (i >= 0 && i < dwordmap_get_height(cell))
	    dwordmap_inc_value(cell, j, i);
	}

	// draw arc 
	if (hough->da < hough->db) {
	  // sweeping along x axis
	  dx = r - hough->da;
	  dy = sqrt(sqr(r) - sqr(dx)); //r*sin(acos(dx/r));
	  while (dx > 0) {
	    i = (int)round(((ybias + dy) - hough->bmin) / hough->db);
	    if (i >= 0 && i < dwordmap_get_height(cell)) {
	      // quater 0 (xbias+dx, ybias+dy)
	      j = (int)round(((xbias + dx) - hough->amin) / hough->da);
	      if (j >= 0 && j < dwordmap_get_width(cell))
		dwordmap_inc_value(cell, j, i);
	      // quater 1 (xbias-dx, ybias+dy)
	      j = (int)round(((xbias - dx) - hough->amin) / hough->da);
	      if (j >= 0 && j < dwordmap_get_width(cell))
		dwordmap_inc_value(cell, j, i);
	    }

	    i = (int)round(((ybias - dy) - hough->bmin) / hough->db);
	    if (i >= 0 && i < dwordmap_get_height(cell)) {
	      // quater 2 (xbias-dx, ybias-dy)
	      j = (int)round(((xbias - dx) - hough->amin) / hough->da);
	      if (j >= 0 && j < dwordmap_get_width(cell))
		dwordmap_inc_value(cell, j, i);
	      // quater 3 (xbias+dx, ybias-dy)
	      j = (int)round(((xbias + dx) - hough->amin) / hough->da);
	      if (j >= 0 && j < dwordmap_get_width(cell))
		dwordmap_inc_value(cell, j, i);
	    }
	    dx -= hough->da;
	    dy = sqrt(sqr(r) - sqr(dx)); //r*sin(acos(dx/r));
	  }
	} else {
	  // sweeping along y axis
	  dy = r - hough->db;
	  dx = sqrt(sqr(r) - sqr(dy)); //r*cos(asin(dy/r));
	  while (dy > 0) {
	    i = (int)round(((ybias + dy) - hough->bmin) / hough->db);
	    if (i >= 0 && i < dwordmap_get_height(cell)) {
	      // quater 0 (xbias+dx, ybias+dy)
	      j = (int)round(((xbias + dx) - hough->amin) / hough->da);
	      if (j >= 0 && j < dwordmap_get_width(cell))
		dwordmap_inc_value(cell, j, i);
	      // quater 1 (xbias-dx, ybias+dy)
	      j = (int)round(((xbias - dx) - hough->amin) / hough->da);
	      if (j >= 0 && j < dwordmap_get_width(cell))
		dwordmap_inc_value(cell, j, i);
	    }
	    i = (int)round(((ybias - dy) - hough->bmin) / hough->db);
	    if (i >= 0 && i < dwordmap_get_height(cell)) {
	      // quater 2 (xbias-dx, ybias-dy)
	      j = (int)round(((xbias - dx) - hough->amin) / hough->da);
	      if (j >= 0 && j < dwordmap_get_width(cell))
		dwordmap_inc_value(cell, j, i);
	      // quater 3 (xbias+dx, ybias-dy)
	      j = (int)round(((xbias + dx) - hough->amin) / hough->da);
	      if (j >= 0 && j < dwordmap_get_width(cell))
		dwordmap_inc_value(cell, j, i);
	    }
	    dy -= hough->db;
	    dx = sqrt(sqr(r) - sqr(dy)); //r*cos(asin(dy/r));
	  }
	}
      }
    }
  }
}

void hough_circle_fast_accumulate(hough_circle_t *hough, matrix_t *grad, real_t thres)
{
  int x, y, i, j, k;
  real_t r, xbias, ybias, magnitude;
  dwordmap_t *cell;
  real_t *gxbuf, *gybuf;

  assert(hough);
  assert(grad);
  assert(grad->imaginary);

  if (hough->da <= 0.0) {
    hough->amin = -(grad->columns) / 2.0;
    hough->da = (grad->columns) / ((*(hough->cell))->header.width - 1);
  }

  if (hough->db <= 0.0) {
    hough->bmin = -(grad->rows) / 2.0;
    hough->db = (grad->rows) / ((*(hough->cell))->header.height - 1);
  }

  if (hough->dr <= 0.0) {
    hough->rmin = max(max(hough->da, hough->db) + 1, 2);
    hough->dr = (min((grad->rows) / 2.0, (grad->columns) / 2.0) - hough->rmin) / (hough->n - 1);
  }

  for (i = 0; i < hough->n; i++) {
    cell = *(hough->cell + i);
    memset(cell->buffer, 0, dwordmap_get_pitch(cell) * dwordmap_get_height(cell));
  }

  gxbuf = grad->real, gybuf = grad->imaginary;
  for (y = 0; y < grad->rows; y++) {
    ybias = (real_t)y-(grad->rows)/2.0;
    for (x = 0; x < grad->columns; x++) {
      magnitude = sqrt(sqr(*(gxbuf + x)) + sqr(*(gybuf + x)));
      if (magnitude <= thres) continue;
      xbias = (real_t)x - (grad->columns) / 2.0;
      for (k = 0; k < hough->n; k++) {
	cell = *(hough->cell + k);
	r = hough->rmin + hough->dr * k;
	i = (int)round(((ybias + r * (*(gybuf + x)) / magnitude) - hough->bmin) / hough->db);
	if (i >= 0 && i < cell->header.height) {
	  j = (int)round(((xbias + r * (*(gxbuf + x)) / magnitude) - hough->amin) / hough->da);
	  if (j >= 0 && j < cell->header.width) {
	    dwordmap_inc_value(cell, j, i);
	    //(*(cell->buffer+i*cell->header.pitch/sizeof(*(cell->buffer))+j))++;
	  }
	}
      }
    }
    gxbuf += grad->columns;
    gybuf += grad->columns;
  }
}

void hough_circle_fast_accumulate_(hough_circle_t *hough, wordmap_t *gx, wordmap_t *gy, real_t thres)
{
  int x, y, i, j, k, w, h;
  real_t r, xbias, ybias, magnitude;
  dwordmap_t *cell;
  int gxpitch, gypitch;
  int16_t *gxbuf, *gybuf;
	
  assert(hough);
  assert(gx);
  assert(gy);
  assert(wordmap_get_width(gx) == wordmap_get_width(gy));
  assert(wordmap_get_height(gx) == wordmap_get_height(gy));

  w = wordmap_get_width(gx);
  h = wordmap_get_height(gx);

  if (hough->da <= 0.0) {
    hough->amin = -w / 2.0;
    hough->da = w / (dwordmap_get_width(*(hough->cell)) - 1);
  }

  if (hough->db <= 0.0) {
    hough->bmin = -h / 2.0;
    hough->db = h / (dwordmap_get_height(*(hough->cell)) - 1);
  }

  if (hough->dr <= 0.0) {
    hough->rmin = max(max(hough->da, hough->db) + 1, 2);
    hough->dr = (min(h / 2.0, w / 2.0) - hough->rmin) / (hough->n - 1);
  }

  for (i = 0; i < hough->n; i++) {
    cell = *(hough->cell + i);
    dwordmap_clear(cell);
  }

  gxpitch = wordmap_get_pitch(gx) / sizeof(*gxbuf);
  gxbuf = (int16_t *)wordmap_get_buffer(gx);

  gypitch = wordmap_get_pitch(gy) / sizeof(*gybuf);
  gybuf = (int16_t *)wordmap_get_buffer(gy);

  for (y = 0; y < h; y++) {
    ybias = (real_t)y - h / 2.0;
    for (x = 0; x < w; x++) {
      magnitude = sqrt(sqr(*(gxbuf + x)) + sqr(*(gybuf + x)));
      if (magnitude <= thres) continue;
      xbias = (real_t)x - w / 2.0;
      for (k = 0; k < hough->n; k++) {
	cell = *(hough->cell + k);
	r = hough->rmin + hough->dr * k;
	i = (int)round(((ybias + r * (*(gybuf + x)) / magnitude) - hough->bmin) / hough->db);
	if (i >= 0 && i < dwordmap_get_height(cell)) {
	  j = (int)round(((xbias + r * (*(gxbuf + x)) / magnitude) - hough->amin) / hough->da);
	  if (j >= 0 && j < dwordmap_get_width(cell))
	    dwordmap_inc_value(cell, j, i);
	}
      }
    }
    gxbuf += gxpitch;
    gybuf += gypitch;
  }
}
