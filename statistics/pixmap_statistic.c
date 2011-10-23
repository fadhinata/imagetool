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
#include <stdint.h>
#include <math.h>
#include <assert.h>

#include <common.h>
#include <statistics/pixmap_statistic.h>

#define GENERAL_MAP_GET_MIN_MAX(type, vmin, vmax, im) {			\
    int n;								\
    int i, j, pitch;							\
    type *buf;								\
    assert(vmin);							\
    assert(vmax);							\
    assert(im);								\
    *(vmin) = -1, *(vmax) = 0, n = 0;					\
    buf = (im)->buffer, pitch = (im)->header.pitch/sizeof(type);	\
    for (i = 0; i < (im)->header.height; i++) {				\
      for (j = 0; j < (im)->header.width; j++) {			\
	*(vmin) = min(*(buf+j), *(vmin));				\
	*(vmax) = max(*(buf+j), *(vmax));				\
	n++;								\
      }									\
      buf += pitch;							\
    }									\
    return n;								\
  }

#define GENERAL_MAP_GET_MIN_MAX_ON_ROI(type, vmin, vmax, im, roi) { \
    int n;							    \
    int i, j, pitch;						    \
    type *buf;							    \
    uint8_t *roibuf;						    \
    assert(vmin);						    \
    assert(vmax);						    \
    assert(im);							    \
    assert(roi);						    \
    assert((im)->header.width == (roi)->header.width);		    \
    assert((im)->header.height == (roi)->header.height);	    \
    *(vmin) = -1, *(vmax) = 0.0, n = 0;				    \
    buf = (im)->buffer, pitch = (im)->header.pitch/sizeof(type);    \
    roibuf = (roi)->buffer;					    \
    for (i = 0; i < (im)->header.height; i++) {			    \
      for (j = 0; j < (im)->header.width; j++) {		    \
	if ((*(roibuf+(j>>3)))&(1<<(j%8))) {			    \
	  *(vmin) = min(*(buf+j), *(vmin));			    \
	  *(vmax) = max(*(buf+j), *(vmax));			    \
	  n++;							    \
	}							    \
      }								    \
      buf += pitch;						    \
      roibuf += (roi)->header.pitch;				    \
    }								    \
    return n;							    \
}

#define GENERAL_MAP_GET_MIN_MAX_ON_REGION(type, vmin, vmax, im, x, y, dx, dy) { \
    int n;								\
    int i, j, width, height, pitch;					\
    type *buf;								\
    assert(vmin);							\
    assert(vmax);							\
    assert(im);								\
    assert(x >= 0 && x < (im)->header.width);				\
    assert(y >= 0 && y < (im)->header.height);				\
    assert(dx > 0);							\
    assert(dy > 0);							\
    *(vmin) = -1;							\
    *(vmax) = 0;							\
    n = 0;								\
    width = min(x+dx, (im)->header.width);				\
    height = min(y+dy, (im)->header.height);				\
    pitch = (im)->header.pitch/sizeof(type);				\
    buf = (im)->buffer + y * pitch;					\
    for (i = y; i < height; i++) {					\
      for (j = x; j < width; j++) {					\
	*(vmin) = min(*(buf+j), *(vmin));				\
	*(vmax) = max(*(buf+j), *(vmax));				\
	n++;								\
      }									\
      buf += pitch;							\
    }									\
    return n;								\
  }

int bytemap_get_min_max(uint8_t *vmin, uint8_t *vmax, bytemap_t *im)
{
  GENERAL_MAP_GET_MIN_MAX(uint8_t, vmin, vmax, im);
}

int wordmap_get_min_max(uint16_t *vmin, uint16_t *vmax, wordmap_t *im)
{
  GENERAL_MAP_GET_MIN_MAX(uint16_t, vmin, vmax, im);
}

int dwordmap_get_min_max(uint32_t *vmin, uint32_t *vmax, dwordmap_t *im)
{
  GENERAL_MAP_GET_MIN_MAX(uint32_t, vmin, vmax, im);
}

int bytemap_get_min_max_on_roi(uint8_t *vmin, uint8_t *vmax, bytemap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MIN_MAX_ON_ROI(uint8_t, vmin, vmax, im, roi);
}

int wordmap_get_min_max_on_roi(uint16_t *vmin, uint16_t *vmax, wordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MIN_MAX_ON_ROI(uint16_t, vmin, vmax, im, roi);
}

int dwordmap_get_min_max_on_roi(uint32_t *vmin, uint32_t *vmax, dwordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MIN_MAX_ON_ROI(uint32_t, vmin, vmax, im, roi);
}

int bytemap_get_min_max_on_region(uint8_t *vmin, uint8_t *vmax, bytemap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MIN_MAX_ON_REGION(uint8_t, vmin, vmax, im, x, y, dx, dy);
}

int wordmap_get_min_max_on_region(uint16_t *vmin, uint16_t *vmax, wordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MIN_MAX_ON_REGION(uint16_t, vmin, vmax, im, x, y, dx, dy);
}

int dwordmap_get_min_max_on_region(uint32_t *vmin, uint32_t *vmax, dwordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MIN_MAX_ON_REGION(uint32_t, vmin, vmax, im, x, y, dx, dy);
}

#define GENERAL_MAP_GET_SUM(type, sum, im) {				\
    int n;								\
    int i, j, pitch;							\
    type *buf;								\
    assert(sum);							\
    assert(im);								\
    *(sum) = 0.0, n = 0;						\
    buf = (im)->buffer, pitch = (im)->header.pitch/sizeof(type);	\
    for (i = 0; i < (im)->header.height; i++) {				\
      for (j = 0; j < (im)->header.width; j++) {			\
	*(sum) += *(buf+j);						\
	n++;								\
      }									\
      buf += pitch;							\
    }									\
    return n;								\
  }

#define GENERAL_MAP_GET_SUM_ON_ROI(type, sum, im, roi) {		\
    int n;								\
    int i, j, pitch;							\
    type *buf;								\
    uint8_t *roibuf;							\
    assert(sum);							\
    assert(im);								\
    assert(roi);							\
    assert((im)->header.width == (roi)->header.width);			\
    assert((im)->header.height == (roi)->header.height);		\
    *(sum) = 0.0, n = 0;						\
    buf = (im)->buffer, pitch = (im)->header.pitch/sizeof(type);	\
    roibuf = roi->buffer;						\
    for (i = 0; i < (im)->header.height; i++) {				\
      for (j = 0; j < (im)->header.width; j++) {			\
	if ((*(roibuf+(j>>3)))&(1<<(j%8))) {				\
	  *(sum) += *(buf+j);						\
	  n++;								\
	}								\
      }									\
      buf += pitch;							\
      roibuf += (roi)->header.pitch;					\
    }									\
    return n;								\
  }

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
    *(sum) = 0.0, n = 0;						\
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
    if (n != 0) *(mean) /= n;				\
    return n;						\
  }

#define GENERAL_MAP_GET_MEAN_ON_ROI(maptype, mean, im, roi) {	\
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
  GENERAL_MAP_GET_MEAN_ON_ROI(bytemap, mean, im, roi);
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

#define GENERAL_MAP_GET_2ND_MOMENT(type, var, org, im) {		\
    int n;								\
    int i, j, pitch;							\
    type *buf;								\
    real_t tmp;								\
    assert(var);							\
    assert(im);								\
    *(var) = 0.0, n = 0;						\
    buf = (im)->buffer, pitch = (im)->header.pitch/sizeof(type);	\
    for (i = 0; i < (im)->header.height; i++) {				\
      for (j = 0; j < (im)->header.width; j++) {			\
	tmp = (real_t)(*(buf+j))-(org);					\
	*(var) += tmp*tmp;						\
	n++;								\
      }									\
      buf += pitch;							\
    }									\
    if (n > 0) *(var) /= (real_t)n;					\
    return n;								\
  }

#define GENERAL_MAP_GET_2ND_MOMENT_ON_ROI(type, var, org, im, roi) {	\
    int n;								\
    int i, j, pitch;							\
    type *buf;								\
    uint8_t *roibuf;							\
    real_t tmp;								\
    assert(var);							\
    assert(im);								\
    assert(roi);							\
    assert((im)->header.width == (roi)->header.width);			\
    assert((im)->header.height == (roi)->header.height);		\
    *(var) = 0.0, n = 0;						\
    buf = (im)->buffer, pitch = (im)->header.pitch/sizeof(type);	\
    roibuf = (roi)->buffer;						\
    for (i = 0; i < (im)->header.height; i++) {				\
      for (j = 0; j < (im)->header.width; j++) {			\
	if ((*(roibuf+(j>>3)))&(1<<(j%8))) {				\
	  tmp = (real_t)(*(buf+j))-(org);				\
	  *(var) += tmp*tmp;						\
	  n++;								\
	}								\
      }									\
      buf += pitch;							\
      roibuf += (roi)->header.pitch;					\
    }									\
    if (n > 0) *(var) /= n;						\
    return n;								\
  }

#define GENERAL_MAP_GET_2ND_MOMENT_ON_REGION(type, var, org, im, x, y, dx, dy) { \
    int n;								\
    int i, j, width, height, pitch;					\
    type *buf;								\
    real_t tmp;								\
    assert(var);							\
    assert(im);								\
    assert(x >= 0 && x < (im)->header.width);				\
    assert(y >= 0 && y < (im)->header.height);				\
    assert(dx > 0);							\
    assert(dy > 0);							\
    *(var) = 0.0, n = 0;						\
    width = min(x+dx, (im)->header.width);				\
    height = min(y+dy, (im)->header.height);				\
    pitch = (im)->header.pitch/sizeof(type);				\
    buf = (im)->buffer+y*pitch;						\
    for (i = y; i < height; i++) {					\
      for (j = x; j < width; j++) {					\
	tmp = (real_t)(*(buf+j))-(org);					\
	*(var) += tmp*tmp;						\
	n++;								\
      }									\
      buf += pitch;							\
    }									\
    if (n > 0) *(var) /= n;						\
    return n;								\
  }

int bytemap_get_2nd_moment(real_t *var, real_t org, bytemap_t *im)
{
  GENERAL_MAP_GET_2ND_MOMENT(uint8_t, var, org, im);
}

int wordmap_get_2nd_moment(real_t *var, real_t org, wordmap_t *im)
{
  GENERAL_MAP_GET_2ND_MOMENT(uint16_t, var, org, im);
}

int dwordmap_get_2nd_moment(real_t *var, real_t org, dwordmap_t *im)
{
  GENERAL_MAP_GET_2ND_MOMENT(uint32_t, var, org, im);
}

int floatmap_get_2nd_moment(real_t *var, real_t org, floatmap_t *im)
{
  GENERAL_MAP_GET_2ND_MOMENT(float, var, org, im);
}

int bytemap_get_2nd_moment_on_roi(real_t *var, real_t org, bytemap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_2ND_MOMENT_ON_ROI(uint8_t, var, org, im, roi);
}

int wordmap_get_2nd_moment_on_roi(real_t *var, real_t org, wordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_2ND_MOMENT_ON_ROI(uint16_t, var, org, im, roi);
}

int dwordmap_get_2nd_moment_on_roi(real_t *var, real_t org, dwordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_2ND_MOMENT_ON_ROI(uint32_t, var, org, im, roi);
}

int floatmap_get_2nd_moment_on_roi(real_t *var, real_t org, floatmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_2ND_MOMENT_ON_ROI(float, var, org, im, roi);
}

int bytemap_get_2nd_moment_on_region(real_t *var, real_t org, bytemap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_2ND_MOMENT_ON_REGION(uint8_t, var, org, im, x, y, dx, dy);
}

int wordmap_get_2nd_moment_on_region(real_t *var, real_t org, wordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_2ND_MOMENT_ON_REGION(uint16_t, var, org, im, x, y, dx, dy);
}

int dwordmap_get_2nd_moment_on_region(real_t *var, real_t org, dwordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_2ND_MOMENT_ON_REGION(uint32_t, var, org, im, x, y, dx, dy);
}

int floatmap_get_2nd_moment_on_region(real_t *var, real_t org, floatmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_2ND_MOMENT_ON_REGION(float, var, org, im, x, y, dx, dy);
}

#define GENERAL_MAP_GET_VARIANCE(maptype, var, im) {	\
    int n;						\
    real_t mean;					\
    assert(var);					\
    assert(im);						\
    n = maptype##_get_mean(&mean, im);			\
    if (n > 0) maptype##_get_2nd_moment(var, mean, im); \
    return n;						\
  }

#define GENERAL_MAP_GET_VARIANCE_ON_ROI(maptype, var, im, roi) {	\
    int n;								\
    real_t mean;							\
    assert(var);							\
    assert(im);								\
    assert(roi);							\
    assert((im)->header.width == (roi)->header.width);			\
    assert((im)->header.height == (roi)->header.height);		\
    n = maptype##_get_mean_on_roi(&mean, im, roi);			\
    if (n > 0) maptype##_get_2nd_moment_on_roi(var, mean, im, roi);	\
    return n;								\
  }

#define GENERAL_MAP_GET_VARIANCE_ON_REGION(maptype, var, im, x, y, dx, dy) { \
    int n;								\
    real_t mean;							\
    assert(var);							\
    assert(im);								\
    assert(x >= 0 && x < (im)->header.width);				\
    assert(y >= 0 && y < (im)->header.height);				\
    assert(dx > 0);							\
    assert(dy > 0);							\
    n = maptype##_get_mean_on_region(&mean, im, x, y, dx, dy);		\
    if (n > 0) maptype##_get_2nd_moment_on_region(var, mean, im, x, y, dx, dy); \
    return n;								\
  }

int bytemap_get_variance(real_t *var, bytemap_t *im)
{
  GENERAL_MAP_GET_VARIANCE(bytemap, var, im);
}

int wordmap_get_variance(real_t *var, wordmap_t *im)
{
  GENERAL_MAP_GET_VARIANCE(wordmap, var, im);
}

int dwordmap_get_variance(real_t *var, dwordmap_t *im)
{
  GENERAL_MAP_GET_VARIANCE(dwordmap, var, im);
}

int floatmap_get_variance(real_t *var, floatmap_t *im)
{
  GENERAL_MAP_GET_VARIANCE(floatmap, var, im);
}

int bytemap_get_variance_on_roi(real_t *var, bytemap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_VARIANCE_ON_ROI(bytemap, var, im, roi);
}

int wordmap_get_variance_on_roi(real_t *var, wordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_VARIANCE_ON_ROI(wordmap, var, im, roi);
}

int dwordmap_get_variance_on_roi(real_t *var, dwordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_VARIANCE_ON_ROI(dwordmap, var, im, roi);
}

int floatmap_get_variance_on_roi(real_t *var, floatmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_VARIANCE_ON_ROI(floatmap, var, im, roi);
}

int bytemap_get_variance_on_region(real_t *var, bytemap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_VARIANCE_ON_REGION(bytemap, var, im, x, y, dx, dy);
}

int wordmap_get_variance_on_region(real_t *var, wordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_VARIANCE_ON_REGION(wordmap, var, im, x, y, dx, dy);
}

int dwordmap_get_variance_on_region(real_t *var, dwordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_VARIANCE_ON_REGION(dwordmap, var, im, x, y, dx, dy);
}

int floatmap_get_variance_on_region(real_t *var, floatmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_VARIANCE_ON_REGION(floatmap, var, im, x, y, dx, dy);
}

#define GENERAL_MAP_GET_STANDARD_DEVIATION(maptype, var, im) {	\
    int n;							\
    real_t mean;						\
    assert(var);						\
    assert(im);							\
    n = maptype##_get_mean(&mean, im);				\
    if (n > 0) {						\
      maptype##_get_2nd_moment(var, mean, im);			\
      *(var) = sqrt(*(var));					\
    }								\
    return n;							\
  }

#define GENERAL_MAP_GET_STANDARD_DEVIATION_ON_ROI(maptype, var, im, roi) { \
    int n;								\
    real_t mean;							\
    assert(var);							\
    assert(im);								\
    assert(roi);							\
    assert((im)->header.width == (roi)->header.width);			\
    assert((im)->header.height == (roi)->header.height);		\
    n = maptype##_get_mean_on_roi(&mean, im, roi);			\
    if (n > 0) {							\
      maptype##_get_2nd_moment_on_roi(var, mean, im, roi);		\
      *(var) = sqrt(*(var));						\
    }									\
    return n;								\
  }

#define GENERAL_MAP_GET_STANDARD_DEVIATION_ON_REGION(maptype, var, im, x, y, dx, dy) { \
    int n;								\
    real_t mean;							\
    assert(var);							\
    assert(im);								\
    assert(x >= 0 && x < (im)->header.width);				\
    assert(y >= 0 && y < (im)->header.height);				\
    assert(dx > 0);							\
    assert(dy > 0);							\
    n = maptype##_get_mean_on_region(&mean, im, x, y, dx, dy);		\
    if (n > 0) {							\
      maptype##_get_2nd_moment_on_region(var, mean, im, x, y, dx, dy);	\
      *(var) = sqrt(*(var));						\
    }									\
    return n;								\
  }


int bytemap_get_standard_deviation(real_t *var, bytemap_t *im)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION(bytemap, var, im);
}

int wordmap_get_standard_deviation(real_t *var, wordmap_t *im)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION(wordmap, var, im);
}

int dwordmap_get_standard_deviation(real_t *var, dwordmap_t *im)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION(dwordmap, var, im);
}

int floatmap_get_standard_deviation(real_t *var, floatmap_t *im)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION(floatmap, var, im);
}

int bytemap_get_standard_deviation_on_roi(real_t *var, bytemap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION_ON_ROI(bytemap, var, im, roi);
}

int wordmap_get_standard_deviation_on_roi(real_t *var, wordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION_ON_ROI(wordmap, var, im, roi);
}

int dwordmap_get_standard_deviation_on_roi(real_t *var, dwordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION_ON_ROI(dwordmap, var, im, roi);
}

int floatmap_get_standard_deviation_on_roi(real_t *var, floatmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION_ON_ROI(floatmap, var, im, roi);
}

int bytemap_get_standard_deviation_on_region(real_t *var, bytemap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION_ON_REGION(bytemap, var, im, x, y, dx, dy);
}

int wordmap_get_standard_deviation_on_region(real_t *var, wordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION_ON_REGION(wordmap, var, im, x, y, dx, dy);
}

int dwordmap_get_standard_deviation_on_region(real_t *var, dwordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION_ON_REGION(dwordmap, var, im, x, y, dx, dy);
}

int floatmap_get_standard_deviation_on_region(real_t *var, floatmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_STANDARD_DEVIATION_ON_REGION(floatmap, var, im, x, y, dx, dy);
}

#define GENERAL_MAP_GET_MOMENTS(type, m2, m3, m4, mean, im) {		\
    int n;								\
    int i, j, pitch;							\
    type *buf;								\
    real_t m1, tmp;							\
    assert(m2);								\
    assert(m3);								\
    assert(m4);								\
    assert(im);								\
    *(m2) = *(m3) = *(m4) = 0.0, n = 0;					\
    buf = (im)->buffer, pitch = (im)->header.pitch/sizeof(type);	\
    for (i = 0; i < (im)->header.height; i++) {				\
      for (j = 0; j < (im)->header.width; j++) {			\
	m1 = (real_t)(*(buf+j))-(mean);					\
	tmp = m1*m1;							\
	*(m2) += tmp;							\
	tmp *= m1;							\
	*(m3) += tmp;							\
	tmp *= m1;							\
	*(m4) += tmp;							\
	n++;								\
      }									\
      buf += pitch;							\
    }									\
    if (n > 0) {							\
      *(m2) /= (real_t)n;						\
      *(m3) /= (real_t)n;						\
      *(m4) /= (real_t)n;						\
    }									\
    return n;								\
  }

#define GENERAL_MAP_GET_MOMENTS_ON_ROI(type, m2, m3, m4, mean, im, roi) { \
    int n;								\
    int i, j, pitch;							\
    type *buf;								\
    uint8_t *roibuf;							\
    real_t m1, tmp;							\
    assert(m2);								\
    assert(m3);								\
    assert(m4);								\
    assert(im);								\
    assert(roi);							\
    assert((im)->header.width == (roi)->header.width);			\
    assert((im)->header.height == (roi)->header.height);		\
    *(m2) = *(m3) = *(m4) = 0.0, n = 0;					\
    buf = (im)->buffer, pitch = (im)->header.pitch/sizeof(type);	\
    roibuf = (roi)->buffer;						\
    for (i = 0; i < (im)->header.height; i++) {				\
      for (j = 0; j < (im)->header.width; j++) {			\
	if ((*(roibuf+(j>>3)))&(1<<(j%8))) {				\
	  m1 = (real_t)(*(buf+j))-(mean);				\
	  tmp = m1*m1;							\
	  *(m2) += tmp;							\
	  tmp *= m1;							\
	  *(m3) += tmp;							\
	  tmp *= m1;							\
	  *(m4) += tmp;							\
	  n++;								\
	}								\
      }									\
      buf += pitch;							\
      roibuf += (roi)->header.pitch;					\
    }									\
    if (n > 0) {							\
      *(m2) /= (real_t)n;						\
      *(m3) /= (real_t)n;						\
      *(m4) /= (real_t)n;						\
    }									\
    return n;								\
}

#define GENERAL_MAP_GET_MOMENTS_ON_REGION(type, m2, m3, m4, mean, im, x, y, dx, dy) { \
    int n;								\
    int i, j, width, height, pitch;					\
    type *buf;								\
    real_t tmp, m1;							\
    assert(m2);								\
    assert(m3);								\
    assert(m4);								\
    assert(im);								\
    assert(x >= 0 && x < (im)->header.width);				\
    assert(y >= 0 && y < (im)->header.height);				\
    assert(dx > 0);							\
    assert(dy > 0);							\
    *(m2) = *(m3) = *(m4) = 0.0, n = 0;					\
    width = min(x+dx, (im)->header.width);				\
    height = min(y+dy, (im)->header.height);				\
    pitch = (im)->header.pitch/sizeof(type);				\
    buf = (im)->buffer+y*pitch;						\
    for (i = y; i < height; i++) {					\
      for (j = x; j < width; j++) {					\
	m1 = (real_t)(*(buf+j))-(mean);					\
	tmp = m1*m1;							\
	*(m2) += tmp;							\
	tmp *= m1;							\
	*(m3) += tmp;							\
	tmp *= m1;							\
	*(m4) += tmp;							\
	n++;								\
      }									\
      buf += pitch;							\
    }									\
    if (n > 0) {							\
      *(m2) /= (real_t)n;						\
      *(m3) /= (real_t)n;						\
      *(m4) /= (real_t)n;						\
    }									\
    return n;								\
  }

int bytemap_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t mean, bytemap_t *im)
{
  GENERAL_MAP_GET_MOMENTS(uint8_t, m2, m3, m4, mean, im);
}

int wordmap_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t mean, wordmap_t *im)
{
  GENERAL_MAP_GET_MOMENTS(uint16_t, m2, m3, m4, mean, im);
}

int dwordmap_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t mean, dwordmap_t *im)
{
  GENERAL_MAP_GET_MOMENTS(uint32_t, m2, m3, m4, mean, im);
}

int floatmap_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t mean, floatmap_t *im)
{
  GENERAL_MAP_GET_MOMENTS(float, m2, m3, m4, mean, im);
}

int bytemap_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t mean, bytemap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MOMENTS_ON_ROI(uint8_t, m2, m3, m4, mean, im, roi);
}

int wordmap_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t mean, wordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MOMENTS_ON_ROI(uint16_t, m2, m3, m4, mean, im, roi);
}

int dwordmap_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t mean, dwordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MOMENTS_ON_ROI(uint32_t, m2, m3, m4, mean, im, roi);
}

int floatmap_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t mean, floatmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_MOMENTS_ON_ROI(float, m2, m3, m4, mean, im, roi);
}

int bytemap_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t mean, bytemap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MOMENTS_ON_REGION(uint8_t, m2, m3, m4, mean, im, x, y, dx, dy);
}

int wordmap_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t mean, wordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MOMENTS_ON_REGION(uint16_t, m2, m3, m4, mean, im, x, y, dx, dy);
}

int dwordmap_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t mean, dwordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MOMENTS_ON_REGION(uint32_t, m2, m3, m4, mean, im, x, y, dx, dy);
}

int floatmap_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t mean, floatmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_MOMENTS_ON_REGION(float, m2, m3, m4, mean, im, x, y, dx, dy);
}

#define GENERAL_MAP_GET_SKEWNESS(maptype, skewness, im) { \
    int n;						  \
    real_t m, std, m2, m3, m4;				  \
    assert(skewness);					  \
    assert(im);						  \
    n = maptype##_get_mean(&m, im);			  \
    if (n > 0) {					  \
      maptype##_get_moments(&m2, &m3, &m4, m, im);	  \
      std = sqrt(m2);					  \
      *(skewness) = m3/(m2*std);			  \
    }							  \
    return n;						  \
}

#define GENERAL_MAP_GET_SKEWNESS_ON_ROI(maptype, skewness, im, roi) { \
    int n;							      \
    real_t m, std, m2, m3, m4;					      \
    assert(skewness);						      \
    assert(im);							      \
    assert(roi);						      \
    assert((im)->header.width == (roi)->header.width);		      \
    assert((im)->header.height == (roi)->header.height);	      \
    n = maptype##_get_mean_on_roi(&m, im, roi);			      \
    if (n > 0) {						      \
      maptype##_get_moments_on_roi(&m2, &m3, &m4, m, im, roi);	      \
      std = sqrt(m2);						      \
      *(skewness) = m3/(m2*std);				      \
    }								      \
    return n;							      \
  }


#define GENERAL_MAP_GET_SKEWNESS_ON_REGION(maptype, skewness, im, x, y, dx, dy) { \
    int n;								\
    real_t m, std, m2, m3, m4;						\
    assert(skewness);							\
    assert(im);								\
    assert(x >= 0 && x < (im)->header.width);				\
    assert(y >= 0 && y < (im)->header.height);				\
    assert(dx > 0);							\
    assert(dy > 0);							\
    n = maptype##_get_mean_on_region(&m, im, x, y, dx, dy);		\
    if (n > 0) {							\
      maptype##_get_moments_on_region(&m2, &m3, &m4, m, im, x, y, dx, dy); \
      std = sqrt(m2);							\
      *(skewness) = m3/(m2*std);					\
    }									\
    return n;								\
  }
int bytemap_get_skewness(real_t *skewness, bytemap_t *im)
{
  GENERAL_MAP_GET_SKEWNESS(bytemap, skewness, im);
}

int wordmap_get_skewness(real_t *skewness, wordmap_t *im)
{
  GENERAL_MAP_GET_SKEWNESS(wordmap, skewness, im);
}

int dwordmap_get_skewness(real_t *skewness, dwordmap_t *im)
{
  GENERAL_MAP_GET_SKEWNESS(dwordmap, skewness, im);
}

int floatmap_get_skewness(real_t *skewness, floatmap_t *im)
{
  GENERAL_MAP_GET_SKEWNESS(floatmap, skewness, im);
}

int bytemap_get_skewness_on_roi(real_t *skewness, bytemap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_SKEWNESS_ON_ROI(bytemap, skewness, im, roi);
}

int wordmap_get_skewness_on_roi(real_t *skewness, wordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_SKEWNESS_ON_ROI(wordmap, skewness, im, roi);
}

int dwordmap_get_skewness_on_roi(real_t *skewness, dwordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_SKEWNESS_ON_ROI(dwordmap, skewness, im, roi);
}

int floatmap_get_skewness_on_roi(real_t *skewness, floatmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_SKEWNESS_ON_ROI(floatmap, skewness, im, roi);
}

int bytemap_get_skewness_on_region(real_t *skewness, bytemap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_SKEWNESS_ON_REGION(bytemap, skewness, im, x, y, dx, dy);
}

int wordmap_get_skewness_on_region(real_t *skewness, wordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_SKEWNESS_ON_REGION(wordmap, skewness, im, x, y, dx, dy);
}

int dwordmap_get_skewness_on_region(real_t *skewness, dwordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_SKEWNESS_ON_REGION(dwordmap, skewness, im, x, y, dx, dy);
}

int floatmap_get_skewness_on_region(real_t *skewness, floatmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_SKEWNESS_ON_REGION(floatmap, skewness, im, x, y, dx, dy);
}


#define GENERAL_MAP_GET_KURTOSIS(maptype, kurtosis, im) {	\
    int n;							\
    real_t m, m2, m3, m4;					\
    assert(kurtosis);						\
    assert(im);							\
    n = maptype##_get_mean(&m, im);				\
    if (n > 0) {						\
      maptype##_get_moments(&m2, &m3, &m4, m, im);		\
      *(kurtosis) = m4/(m2*m2);					\
    }								\
    return n;							\
  }

#define GENERAL_MAP_GET_KURTOSIS_ON_ROI(maptype, kurtosis, im, roi) {	\
    int n;								\
    real_t m, m2, m3, m4;						\
    assert(kurtosis);							\
    assert(im);								\
    assert(roi);							\
    assert((im)->header.width == (roi)->header.width);			\
    assert((im)->header.height == (roi)->header.height);		\
    n = maptype##_get_mean_on_roi(&m, im, roi);				\
    if (n > 0) {							\
      maptype##_get_moments_on_roi(&m2, &m3, &m4, m, im, roi);		\
      *(kurtosis) = m4/(m2*m2);						\
    }									\
    return n;								\
  }

#define GENERAL_MAP_GET_KURTOSIS_ON_REGION(maptype, kurtosis, im, x, y, dx, dy) { \
    int n;								\
    real_t m, m2, m3, m4;						\
    assert(kurtosis);							\
    assert(im);								\
    assert(x >= 0 && x < (im)->header.width);				\
    assert(y >= 0 && y < (im)->header.height);				\
    assert(dx > 0);							\
    assert(dy > 0);							\
    n = maptype##_get_mean_on_region(&m, im, x, y, dx, dy);		\
    if (n > 0) {							\
      maptype##_get_moments_on_region(&m2, &m3, &m4, m, im, x, y, dx, dy); \
      *(kurtosis) = m4/(m2*m2);						\
    }									\
    return n;								\
  }

int bytemap_get_kurtosis(real_t *kurtosis, bytemap_t *im)
{
  GENERAL_MAP_GET_KURTOSIS(bytemap, kurtosis, im);
}

int wordmap_get_kurtosis(real_t *kurtosis, wordmap_t *im)
{
  GENERAL_MAP_GET_KURTOSIS(wordmap, kurtosis, im);
}

int dwordmap_get_kurtosis(real_t *kurtosis, dwordmap_t *im)
{
  GENERAL_MAP_GET_KURTOSIS(dwordmap, kurtosis, im);
}

int floatmap_get_kurtosis(real_t *kurtosis, floatmap_t *im)
{
  GENERAL_MAP_GET_KURTOSIS(floatmap, kurtosis, im);
}

int bytemap_get_kurtosis_on_roi(real_t *kurtosis, bytemap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_KURTOSIS_ON_ROI(bytemap, kurtosis, im, roi);
}

int wordmap_get_kurtosis_on_roi(real_t *kurtosis, wordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_KURTOSIS_ON_ROI(wordmap, kurtosis, im, roi);
}

int dwordmap_get_kurtosis_on_roi(real_t *kurtosis, dwordmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_KURTOSIS_ON_ROI(dwordmap, kurtosis, im, roi);
}

int floatmap_get_kurtosis_on_roi(real_t *kurtosis, floatmap_t *im, bitmap_t *roi)
{
  GENERAL_MAP_GET_KURTOSIS_ON_ROI(floatmap, kurtosis, im, roi);
}

int bytemap_get_kurtosis_on_region(real_t *kurtosis, bytemap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_KURTOSIS_ON_REGION(bytemap, kurtosis, im, x, y, dx, dy);
}

int wordmap_get_kurtosis_on_region(real_t *kurtosis, wordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_KURTOSIS_ON_REGION(wordmap, kurtosis, im, x, y, dx, dy);
}

int dwordmap_get_kurtosis_on_region(real_t *kurtosis, dwordmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_KURTOSIS_ON_REGION(dwordmap, kurtosis, im, x, y, dx, dy);
}

int floatmap_get_kurtosis_on_region(real_t *kurtosis, floatmap_t *im, int x, int y, int dx, int dy)
{
  GENERAL_MAP_GET_KURTOSIS_ON_REGION(floatmap, kurtosis, im, x, y, dx, dy);
}

