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
#ifndef __MEAN_H__
#define __MEAN_H__

#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>
#include <dwordmap.h>
#include <floatmap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* the postfix roi means the region of interest */
int bytemap_get_sum(double *sum, bytemap_t *im);
int wordmap_get_sum(double *sum, wordmap_t *im);
int dwordmap_get_sum(double *sum, dwordmap_t *im);
int floatmap_get_sum(double *sum, floatmap_t *im);
int bytemap_get_sum_on_roi(double *sum, bytemap_t *im, bitmap_t *roi);
int wordmap_get_sum_on_roi(double *sum, wordmap_t *im, bitmap_t *roi);
int dwordmap_get_sum_on_roi(double *sum, dwordmap_t *im, bitmap_t *roi);
int floatmap_get_sum_on_roi(double *sum, floatmap_t *im, bitmap_t *roi);
int bytemap_get_sum_on_region(double *sum, bytemap_t *im, int x, int y, int dx, int dy);
int wordmap_get_sum_on_region(double *sum, wordmap_t *im, int x, int y, int dx, int dy);
int dwordmap_get_sum_on_region(double *sum, dwordmap_t *im, int x, int y, int dx, int dy);
int floatmap_get_sum_on_region(double *sum, floatmap_t *im, int x, int y, int dx, int dy);

int bytemap_get_mean(double *mean, bytemap_t *im);
int wordmap_get_mean(double *mean, wordmap_t *im);
int dwordmap_get_mean(double *mean, dwordmap_t *im);
int floatmap_get_mean(double *mean, floatmap_t *im);
int bytemap_get_mean_on_roi(double *mean, bytemap_t *im, bitmap_t *roi);
int wordmap_get_mean_on_roi(double *mean, wordmap_t *im, bitmap_t *roi);
int dwordmap_get_mean_on_roi(double *mean, dwordmap_t *im, bitmap_t *roi);
int floatmap_get_mean_on_roi(double *mean, floatmap_t *im, bitmap_t *roi);
int bytemap_get_mean_on_region(double *mean, bytemap_t *im, int x, int y, int dx, int dy);
int wordmap_get_mean_on_region(double *mean, wordmap_t *im, int x, int y, int dx, int dy);
int dwordmap_get_mean_on_region(double *mean, dwordmap_t *im, int x, int y, int dx, int dy);
int floatmap_get_mean_on_region(double *mean, floatmap_t *im, int x, int y, int dx, int dy);

#ifdef __cplusplus
}
#endif

#endif /* __MEAN_H__ */
