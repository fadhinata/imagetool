/* Copyright 2011 Hoyoung Yi. This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with this program; if not, please visit www.gnu.org.*/#include <stdio.h>#include <limits.h>#include <assert.h>#include <common.h>#include <histogram.h>#include <backsym.h>#include <threshold.h>int background_symmetry_level(int *hist, int n, real_t ratio){  int i;  int pixels, count, sum;	  assert(hist);  assert(ratio > 0.0 && ratio < 1.0);  for (pixels = 0, i = 0; i < n; i++) pixels += hist[i];	  count = (int)((real_t)pixels*ratio);  for (sum = 0, i = 0; i < n; i++) {    sum += hist[i];    if (sum >= count) break;  }  return (i >= n ? n-1 : i);}void bytemap_background_symmetry_threshold(bitmap_t *bin, bytemap_t *im, real_t ratio){  int tlevel;  int h[UINT8_MAX+1];	  assert(bin);  assert(im);  assert(bitmap_get_width(bin) == bytemap_get_width(im));  assert(bitmap_get_height(bin) == bytemap_get_height(im));	  bytemap_histogram((int *)h, im);  tlevel = background_symmetry_level((int *)h, UINT8_MAX+1, ratio);  bytemap_threshold(bin, im, tlevel, UINT8_MAX);}void bytemap_background_symmetry_threshold_on_roi(bitmap_t *bin, bytemap_t *im, real_t ratio, bitmap_t *roi){  int tlevel;  int h[UINT8_MAX+1];	  assert(bin);  assert(im);  assert(roi);  assert(bitmap_get_width(bin) == bytemap_get_width(im));  assert(bitmap_get_height(bin) == bytemap_get_height(im));  assert(bytemap_get_width(im) == bitmap_get_width(roi));  assert(bytemap_get_height(im) == bitmap_get_height(roi));	  bytemap_histogram_on_roi((int *)h, im, roi);  tlevel = background_symmetry_level((int *)h, UINT8_MAX+1, ratio);  bytemap_threshold_on_roi(bin, im, tlevel, UINT8_MAX, roi);}void bytemap_background_symmetry_threshold_on_region(bitmap_t *bin, bytemap_t *im, real_t ratio, int x, int y, int dx, int dy){  int tlevel;  int h[UINT8_MAX+1];	  assert(bin);  assert(im);  assert(bitmap_get_width(bin) == bytemap_get_width(im));  assert(bitmap_get_height(bin) == bytemap_get_height(im));  assert(x >= 0 && x < bytemap_get_width(im));  assert(y >= 0 && y < bytemap_get_height(im));  assert(dx > 0);  assert(dy > 0);  bytemap_histogram_on_region((int *)h, im, x, y, dx, dy);  tlevel = background_symmetry_level((int *)h, UINT8_MAX+1, ratio);  bytemap_threshold_on_region(bin, im, tlevel, UINT8_MAX, x, y, dx, dy);}void wordmap_background_symmetry_threshold(bitmap_t *bin, wordmap_t *im, real_t ratio){  int tlevel;  int h[UINT16_MAX+1];	  assert(bin);  assert(im);  assert(bitmap_get_width(bin) == wordmap_get_width(im));  assert(bitmap_get_height(bin) == wordmap_get_height(im));	  wordmap_histogram((int *)h, im);  tlevel = background_symmetry_level((int *)h, UINT16_MAX+1, ratio);  wordmap_threshold(bin, im, tlevel, UINT16_MAX);}void wordmap_background_symmetry_threshold_on_roi(bitmap_t *bin, wordmap_t *im, real_t ratio, bitmap_t *roi){  int tlevel;  int h[UINT16_MAX+1];	  assert(bin);  assert(im);  assert(roi);  assert(bitmap_get_width(bin) == wordmap_get_width(im));  assert(bitmap_get_height(bin) == wordmap_get_height(im));  assert(bitmap_get_width(roi) == wordmap_get_width(im));  assert(bitmap_get_height(roi) == wordmap_get_height(im));	  wordmap_histogram_on_roi((int *)h, im, roi);  tlevel = background_symmetry_level((int *)h, UINT16_MAX+1, ratio);  wordmap_threshold_on_roi(bin, im, tlevel, UINT16_MAX, roi);}void wordmap_background_symmetry_threshold_on_region(bitmap_t *bin, wordmap_t *im, real_t ratio, int x, int y, int dx, int dy){  int tlevel;  int h[UINT16_MAX+1];	  assert(bin);  assert(im);  assert(bitmap_get_width(bin) == wordmap_get_width(im));  assert(bitmap_get_height(bin) == wordmap_get_height(im));  assert(x >= 0 && x < wordmap_get_width(im));  assert(y >= 0 && y < wordmap_get_height(im));  assert(dx > 0);  assert(dy > 0);  wordmap_histogram_on_region((int *)h, im, x, y, dx, dy);  tlevel = background_symmetry_level((int *)h, UINT16_MAX+1, ratio);  wordmap_threshold_on_region(bin, im, tlevel, UINT16_MAX, x, y, dx, dy);}