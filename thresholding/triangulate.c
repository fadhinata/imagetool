/* Copyright 2011 Hoyoung Yi. This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with this program; if not, please visit www.gnu.org.*/#include <stdio.h>#include <limits.h>#include <histogram.h>#include <threshold.h>#include <triangulate.h>#include <assert.h>int triangle_level(int *hist, int n){  int i, hmax, imax, tlevel, imin;  real_t a, b, dist, dmax, u, x, y;  assert(hist);  assert(n > 0);	  // find the index of the maximum histogram value  imax = 0;  hmax = hist[imax];  for (i = 1; i < n; i++) {    if (hmax < hist[i]) {      hmax = hist[i];      imax = i;    }  }	  if (imax > (n/2))	{    // find the support of historam    for (i = 0; i < imax; i++) {      if (hist[i] > 0) {	imin = i;	break;      }    }    // mesh line from imax to imin    a = (hist[imax]-hist[imin])/(imax-imin);    b = (hist[imax]-a*imax);    for (dmax = 0.0, tlevel = i = imin; i < imax; i++) {      if ((a*i+b) > hist[i]) {	u = ((i-imin)*(imax-imin)+(hist[i]-hist[imin])*(hist[imax]-hist[imin]))	  /((imax-imin)*(imax-imin)+(hist[imax]-hist[imin])*(hist[imax]-hist[imin]));	x = imin+u*(imax-imin);	y = hist[imin]+u*(hist[imax]-hist[imin]);	dist = ((x-i)*(x-i)+(y-hist[i])*(y-hist[i]));	if (dmax < dist) {	  dmax = dist;	  tlevel = i;	}      }    }  } else {    for (i = (n-1); i > imax; i--) {      if (hist[i] > 0) {	imin = i;	break;      }    }	    a = (hist[imax]-hist[imin])/(imax-imin);    b = (hist[imax]-a*imax);    for (dmax = 0.0, tlevel = i = imin; i > imax; i--) {      if ((a*i+b) > hist[i]) {	u = ((i-imin)*(imax-imin)+(hist[i]-hist[imin])*(hist[imax]-hist[imin]))	  /((imax-imin)*(imax-imin)+(hist[imax]-hist[imin])*(hist[imax]-hist[imin]));	x = imin+u*(imax-imin);	y = hist[imin]+u*(hist[imax]-hist[imin]);	dist = ((x-i)*(x-i)+(y-hist[i])*(y-hist[i]));	if (dmax < dist) {	  dmax = dist;	  tlevel = i;	}      }    }  }  return tlevel;}void bytemap_triangle_threshold(bitmap_t *bin, bytemap_t *im){  int tlevel;  int hist[UINT8_MAX+1];  assert(bin);  assert(im);  assert(im->header.width == bin->header.width);  assert(im->header.height == bin->header.height);	  bytemap_histogram((int *)hist, im);  tlevel = triangle_level((int *)hist, UINT8_MAX+1);  bytemap_threshold(bin, im, tlevel, UINT8_MAX);}void bytemap_triangle_threshold_on_roi(bitmap_t *bin, bytemap_t *im, bitmap_t *roi){  int tlevel;  int hist[UINT8_MAX+1];  assert(bin);  assert(im);  assert(roi);  assert(im->header.width == bin->header.width);  assert(im->header.height == bin->header.height);  assert(roi->header.width == bin->header.width);  assert(roi->header.height == bin->header.height);	  bytemap_histogram_on_roi((int *)hist, im, roi);  tlevel = triangle_level((int *)hist, UINT8_MAX+1);  bytemap_threshold_on_roi(bin, im, tlevel, UINT8_MAX, roi);}void bytemap_triangle_threshold_on_region(bitmap_t *bin, bytemap_t *im, int x, int y, int dx, int dy){  int tlevel;  int hist[UINT8_MAX+1];  assert(bin);  assert(im);  assert(im->header.width == bin->header.width);  assert(im->header.height == bin->header.height);  assert(x >= 0 && x < bin->header.width);  assert(y >= 0 && y < bin->header.height);  assert(dx > 0);  assert(dy > 0);	  bytemap_histogram_on_region((int *)hist, im, x, y, dx, dy);  tlevel = triangle_level((int *)hist, UINT8_MAX+1);  bytemap_threshold_on_region(bin, im, tlevel, UINT8_MAX, x, y, dx, dy);}void wordmap_triangle_threshold(bitmap_t *bin, wordmap_t *im){  int tlevel;  int hist[UINT16_MAX+1];  assert(bin);  assert(im);  assert(im->header.width == bin->header.width);  assert(im->header.height == bin->header.height);	  wordmap_histogram((int *)hist, im);  tlevel = triangle_level((int *)hist, UINT16_MAX+1);  wordmap_threshold(bin, im, tlevel, UINT16_MAX);}void wordmap_triangle_threshold_on_roi(bitmap_t *bin, wordmap_t *im, bitmap_t *roi){  int tlevel;  int hist[UINT16_MAX+1];  assert(bin);  assert(im);  assert(roi);  assert(im->header.width == bin->header.width);  assert(im->header.height == bin->header.height);  assert(roi->header.width == bin->header.width);  assert(roi->header.height == bin->header.height);	  wordmap_histogram_on_roi((int *)hist, im, roi);  tlevel = triangle_level((int *)hist, UINT16_MAX+1);  wordmap_threshold_on_roi(bin, im, tlevel, UINT16_MAX, roi);}void wordmap_triangle_threshold_on_region(bitmap_t *bin, wordmap_t *im, int x, int y, int dx, int dy){  int tlevel;  int hist[UINT16_MAX+1];  assert(bin);  assert(im);  assert(im->header.width == bin->header.width);  assert(im->header.height == bin->header.height);  assert(x >= 0 && x < bin->header.width);  assert(y >= 0 && y < bin->header.height);  assert(dx > 0);  assert(dy > 0);	  wordmap_histogram_on_region((int *)hist, im, x, y, dx, dy);  tlevel = triangle_level((int *)hist, UINT16_MAX+1);  wordmap_threshold_on_region(bin, im, tlevel, UINT16_MAX, x, y, dx, dy);}