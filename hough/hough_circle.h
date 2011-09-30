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
#ifndef __HOUGH_CIRCLE_H__
#define __HOUGH_CIRCLE_H__

#include <bitmap.h>
#include <wordmap.h>
#include <dwordmap.h>
#include <matrix.h>

#ifdef __cplusplus
extern "C" {
#endif

  // (x - a)^2 + (y - b)^2 = r^2
  typedef struct {
    int n;
    real_t amin, da;
    real_t bmin, db;
    real_t rmin, dr;
    dwordmap_t **cell;
  } hough_circle_t;

#define hough_circle_get_count_of_cells(m) ((m)->n)
#define hough_circle_get_width_of_cell(m) dwordmap_get_width(*((m)->cell))
#define hough_circle_get_height_of_cell(m) dwordmap_get_height(*((m)->cell))
#define hough_circle_get_amin(m) ((m)->amin)
#define hough_circle_get_da(m) ((m)->da)
#define hough_circle_get_bmin(m) ((m)->bmin)
#define hough_circle_get_db(m) ((m)->db)
#define hough_circle_get_rmin(m) ((m)->rmin)
#define hough_circle_get_dr(m) ((m)->dr)
  hough_circle_t *hough_circle_new(real_t xmin, real_t xmax, int nx, real_t ymin, real_t ymax, int ny, real_t rmin, real_t rmax, int nr);
  void hough_circle_destroy(hough_circle_t *hough);
  void hough_circle_accumulate(hough_circle_t *hough, bitmap_t *bin);
  void hough_circle_fast_accumulate(hough_circle_t *hough, matrix_t *grad, real_t thres);
  //void hough_circle_fast_accumulate(hough_circle_t *hough, real_tmap_t *gx, real_tmap_t *gy, real_t thres);
  int hough_circle_get_sorted_index(int *xindex, int *yindex, int *rindex, int n, hough_circle_t *hough);
  void hough_circle_get_arguments(real_t *a, real_t *b, real_t *r, real_t xcenter, real_t ycenter, int xindex, int yindex, int rindex, hough_circle_t *hough);

#ifdef __cplusplus
}
#endif

#endif /* __HOUGH_CIRCLE_H__ */