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
#ifndef __HOUGH_LINE_H__
#define __HOUGH_LINE_H__

#include <common.h>
#include <bitmap.h>
#include <dwordmap.h>
#include <matrix.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    real_t tmin, dt;
    real_t rmin, dr;
    real_t theta_min, delta_theta;
    real_t radius_min, delta_radius;
    dwordmap_t *cell;
  } hough_line_t;

  hough_line_t *hough_line_new(real_t tmin, real_t tmax, int nt, real_t rmin, real_t rmax, int nr);
  void hough_line_destroy(hough_line_t *hough);
  void hough_line_accumulate(hough_line_t *hough, bitmap_t *bin);
  void hough_line_fast_accumulate(hough_line_t *hough, matrix_t *grad, real_t thres);
  //void hough_line_fast_accumulate(hough_line_t *hough, real_tmap_t *gx, real_tmap_t *gy, real_t thres);
  int hough_line_get_sorted_index(int *tindex, int *rindex, int n, hough_line_t *hough);
  void hough_line_get_arguments(real_t *a, real_t *b, real_t *c, real_t xorg, real_t yorg, int tindex, int rindex, hough_line_t *hough);

#ifdef __cplusplus
}
#endif

#endif /* __HOUGH_LINE_H__ */
