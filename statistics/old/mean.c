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
#include <assert.h>

#include <common.h>
#include <mean.h>

#define GENERAL_MAP_GET_SUM(type, sum, im) {			\
    int n;							\
    int i, j, pitch;						\
    type *buf;							\
    assert(sum);						\
    assert(im);							\
    *sum = 0.0, n = 0;						\
    buf = (im)->buffer, pitch = (im)->header.pitch/sizeof(type);	\
    for (i = 0; i < (im)->header.height; i++) {			\
      for (j = 0; j < (im)->header.width; j++) {			\
	*(sum) += *(buf+j);					\
	n++;							\
      }								\
      buf += pitch;						\
    }								\
    return n;							\
  }

#define GENERAL_MAP_GET_SUM_ON_ROI(type, sum, im, roi) {	\
    int n;							\
    int i, j, pitch;						\
    type *buf;							\
    uint8_t *roibuf;						\
    assert(sum);						\
    assert(im);							\
    assert(roi);						\
    assert((im)->header.width == (roi)->header.width);		\
    assert((im)->header.height == (roi)->header.height);	\
    *sum = 0.0, n = 0;						\
    buf = (im)->buffer;						\
    pitch = (im)->header.pitch/sizeof(type);			\
    roibuf = (roi)->buffer;					\
    for (i = 0; i < (im)->header.height; i++) {			\
      for (j = 0; j < (im)->header.width; j++) {		\
	if ((*(roibuf+(j>>3)))&(1<<(j%8))) {			\
	  *(sum) += *(buf+j);					\
	  n++;							\
	}							\
      }								\
      buf += pitch;						\
      roibuf += (roi)->header.pitch;				\
    }								\
    return n;							\
  }

// int xxxmap_get_variance_on_region(double *var, bytemap_t *im, double mean, int x, int y, int dx, int dy)
#define GENERAL_MAP_GET_SUM_ON_REGION(type, sum, im, x, y, dx, dy) {	\
    int n;								\
    int i, j, width, height, pitch;					\
    type *buf;								\
    assert(sum);							\
    assert(im);								\
    assert(x >= 0 && x < (im)->header.width);				\
    assert(y >= 0 && y < (im)->header.height);				\
    assert(dx > 0);							\
    assert(dy > 0);							\
    *sum = 0.0, n = 0;							\
    width = min(x+dx, (im)->header.width);				\
    height = min(y+dy, (im)->header.height);				\
    pitch = (im)->header.pitch/sizeof(type);				\
    buf = (im)->buffer+y*pitch;						\
    for (i = y; i < height; i++) {					\
      for (j = x; j < width; j++) {					\
	*(sum) += *(buf+j);						\
	n++;								\
      }									\
      buf += pitch;							\
    }									\
    return n;								\
  }

int bytemap_get_sum(real_t *sum, bytemap_t *im)
{
  GENERAL_MAP_GET_SUM(uint8_t, sum, im);
}

int wordmap_get_sum(real_t *sum, wordmap_t *im)
{
  GENERAL_MAP_GET_SUM(uint16_t, sum, im);
}

int dwordmap_get_sum(real_t *sum, dwordmap_t *im)
{
  GENERAL_MAP_GET_SUM(uint32_t, sum, im);
}

int floatmap_get_sum(real_t *sum, floatmap_t *im)
{
  GENERAL_MAP_GET_SUM(float, sum, im);
}

int bytemap_get_sum_on_roi(real_t *sum, bytemap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_SUM_ON_ROI(uint8_t, sum, im, roi);
}

int wordmap_get_sum_on_roi(real_t *sum, wordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_SUM_ON_ROI(uint16_t, sum, im, roi);
}

int dwordmap_get_sum_on_roi(real_t *sum, dwordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_SUM_ON_ROI(uint32_t, sum, im, roi);
}

int floatmap_get_sum_on_roi(real_t *sum, floatmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_SUM_ON_ROI(float, sum, im, roi);
}

int bytemap_get_sum_on_region(real_t *sum, bytemap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_SUM_ON_REGION(uint8_t, sum, im, x, y, dx, dy);
}

int wordmap_get_sum_on_region(real_t *sum, wordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_SUM_ON_REGION(uint16_t, sum, im, x, y, dx, dy);
}

int dwordmap_get_sum_on_region(real_t *sum, dwordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_SUM_ON_REGION(uint32_t, sum, im, x, y, dx, dy);
}

int floatmap_get_sum_on_region(real_t *sum, floatmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_SUM_ON_REGION(float, sum, im, x, y, dx, dy);
}

#define GENERAL_MAP_GET_MEAN(maptype, mean, im) {	\
    int n;						\
    assert(mean);					\
    assert(im);						\
    n = maptype##_get_sum(mean, im);			\
    if (n != 0) *mean /= n;				\
    return n;						\
  }

#define GENERAL_MAP_GET_MEAN_ON_RIO(maptype, mean, im, roi) {	\
    int n;							\
    assert(mean);						\
    assert(im);							\
    assert(roi);						\
    assert((im)->header.width == (roi)->header.width);		\
    assert((im)->header.height == (roi)->header.height);	\
    n = maptype##_get_sum_on_roi(mean, im, roi);		\
    if (n != 0) *(mean) /= n;					\
    return n;							\
  }

#define GENERAL_MAP_GET_MEAN_ON_REGION(maptype, mean, im, x, y, dx, dy) { \
    int n;								\
    assert(mean);							\
    assert(im);								\
    assert(x >= 0 && x < (im)->header.width);				\
    assert(y >= 0 && y < (im)->header.height);				\
    assert(dx > 0);							\
    assert(dy > 0);							\
    n = maptype##_get_sum_on_region(mean, im, x, y, dx, dy);		\
    if (n != 0) *(mean) /= n;						\
    return n;								\
  }

int bytemap_get_mean(real_t *mean, bytemap_t *im)
{
  GENERAL_MAP_GET_MEAN(bytemap, mean, im);
}

int wordmap_get_mean(real_t *mean, wordmap_t *im)
{
  GENERAL_MAP_GET_MEAN(wordmap, mean, im);
}

int dwordmap_get_mean(real_t *mean, dwordmap_t *im)
{
  GENERAL_MAP_GET_MEAN(dwordmap, mean, im);
}

int floatmap_get_mean(real_t *mean, floatmap_t *im)
{
  GENERAL_MAP_GET_MEAN(floatmap, mean, im);
}

int bytemap_get_mean_on_roi(real_t *mean, bytemap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MEAN_ON_RIO(bytemap, mean, im, roi);
}

int wordmap_get_mean_on_roi(real_t *mean, wordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MEAN_ON_ROI(wordmap, mean, im, roi);
}

int dwordmap_get_mean_on_roi(real_t *mean, dwordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MEAN_ON_ROI(dwordmap, mean, im, roi);
}

int floatmap_get_mean_on_roi(real_t *mean, floatmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MEAN_ON_ROI(floatmap, mean, im, roi);
}

int bytemap_get_mean_on_region(real_t *mean, bytemap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MEAN_ON_REGION(bytemap, mean, im, x, y, dx, dy);
}

int wordmap_get_mean_on_region(real_t *mean, wordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MEAN_ON_REGION(wordmap, mean, im, x, y, dx, dy);
}

int dwordmap_get_mean_on_region(real_t *mean, dwordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MEAN_ON_REGION(dwordmap, mean, im, x, y, dx, dy);
}

int floatmap_get_mean_on_region(real_t *mean, floatmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MEAN_ON_REGION(floatmap, mean, im, x, y, dx, dy);
}

