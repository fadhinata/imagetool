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
#ifndef __THRESHOLD_H__
#define __THRESHOLD_H__

#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <linear_algebra/matrix.h>

#ifdef __cplusplus
extern "C" {
#endif

  void bytemap_threshold(bitmap_t *bin, bytemap_t *im, int cutup, int cutdown);
  void bytemap_threshold_on_roi(bitmap_t *bin, bytemap_t *im, int cutup, int cutdown, bitmap_t *roi);
  void bytemap_threshold_on_region(bitmap_t *bin, bytemap_t *im, int cutup, int cutdown, int x, int y, int dx, int dy);

  void wordmap_threshold(bitmap_t *bin, wordmap_t *im, int cutup, int cutdown);
  void wordmap_threshold_on_roi(bitmap_t *bin, wordmap_t *im, int cutup, int cutdown, bitmap_t *roi);
  void wordmap_threshold_on_region(bitmap_t *bin, wordmap_t *im, int cutup, int cutdown, int x, int y, int dx, int dy);
  void matrix_threshold(bitmap_t *bin, matrix_t *im, real_t cutup, real_t cutdown);
  void matrixp_threshold_on_roi(bitmap_t *bin, matrix_t *im, real_t cutup, real_t cutdown, bitmap_t *roi);
  void matrix_threshold_on_region(bitmap_t *bin, matrix_t *im, real_t cutup, real_t cutdown, int x, int y, int dx, int dy);

#ifdef __cplusplus
}
#endif

#endif /* __THRESHOLD_H__ */
