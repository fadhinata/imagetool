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
#ifndef __DSIGMA_H__
#define __DSIGMA_H__

#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  int dominant_sigma_level(int *hist, int n);

  void bytemap_dominant_sigma_threshold(bitmap_t *bin, bytemap_t *im);
  void bytemap_dominant_sigma_threshold_on_roi(bitmap_t *bin, bytemap_t *im, bitmap_t *roi);
  void bytemap_dominant_sigma_threshold_on_region(bitmap_t *bin, bytemap_t *im, int x, int y, int dx, int dy);

  void wordmap_dominant_sigma_threshold(bitmap_t *bin, wordmap_t *im);
  void wordmap_dominant_sigma_threshold_on_roi(bitmap_t *bin, wordmap_t *im, bitmap_t *roi);
  void wordmap_dominant_sigma_threshold_on_region(bitmap_t *bin, wordmap_t *im, int x, int y, int dx, int dy);

#ifdef __cplusplus
}
#endif

#endif /* __DSIGMA_H__ */
