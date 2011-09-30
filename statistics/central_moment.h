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
#ifndef __CENTRAL_MOMENT_H__
#define __CENTRAL_MOMENT_H__

#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>
#include <dwordmap.h>

#ifdef __cplusplus
extern "C" {
#endif

int bytemap_get_central_moments(double *m2, double *m3, double *m4, bytemap_t *im, double mean);
int wordmap_get_central_moments(double *m2, double *m3, double *m4, wordmap_t *im, double mean);
int dwordmap_get_central_moments(double *m2, double *m3, double *m4, dwordmap_t *im, double mean);
int floatmap_get_central_moments(double *m2, double *m3, double *m4, floatmap_t *im, double mean);
int bytemap_get_central_moments_on_roi(double *m2, double *m3, double *m4, bytemap_t *im, double mean, bitmap_t *roi);
int wordmap_get_central_moments_on_roi(double *m2, double *m3, double *m4, wordmap_t *im, double mean, bitmap_t *roi);
int dwordmap_get_central_moments_on_roi(double *m2, double *m3, double *m4, dwordmap_t *im, double mean, bitmap_t *roi);
int floatmap_get_central_moments_on_roi(double *m2, double *m3, double *m4, floatmap_t *im, double mean, bitmap_t *roi);
int bytemap_get_central_moments_on_region(double *m2, double *m3, double *m4, bytemap_t *im, double mean, int x, int y, int dx, int dy);
int wordmap_get_central_moments_on_region(double *m2, double *m3, double *m4, wordmap_t *im, double mean, int x, int y, int dx, int dy);
int dwordmap_get_central_moments_on_region(double *m2, double *m3, double *m4, dwordmap_t *im, double mean, int x, int y, int dx, int dy);
int floatmap_get_central_moments_on_region(double *m2, double *m3, double *m4, floatmap_t *im, double mean, int x, int y, int dx, int dy);

int bytemap_get_skewness(double *skewness, bytemap_t *im, double mean);
int wordmap_get_skewness(double *skewness, wordmap_t *im, double mean);
int dwordmap_get_skewness(double *skewness, dwordmap_t *im, double mean);
int floatmap_get_skewness(double *skewness, floatmap_t *im, double mean);
int bytemap_get_skewness_on_roi(double *skewness, bytemap_t *im, double mean, bitmap_t *roi);
int wordmap_get_skewness_on_roi(double *skewness, wordmap_t *im, double mean, bitmap_t *roi);
int dwordmap_get_skewness_on_roi(double *skewness, dwordmap_t *im, double mean, bitmap_t *roi);
int floatmap_get_skewness_on_roi(double *skewness, floatmap_t *im, double mean, bitmap_t *roi);
int bytemap_get_skewness_on_region(double *skewness, bytemap_t *im, double mean, int x, int y, int dx, int dy);
int wordmap_get_skewness_on_region(double *skewness, wordmap_t *im, double mean, int x, int y, int dx, int dy);
int dwordmap_get_skewness_on_region(double *skewness, dwordmap_t *im, double mean, int x, int y, int dx, int dy);
int floatmap_get_skewness_on_region(double *skewness, floatmap_t *im, double mean, int x, int y, int dx, int dy);

int bytemap_get_kurtosis(double *kurtosis, bytemap_t *im, double mean);
int wordmap_get_kurtosis(double *kurtosis, wordmap_t *im, double mean);
int dwordmap_get_kurtosis(double *kurtosis, dwordmap_t *im, double mean);
int floatmap_get_kurtosis(double *kurtosis, floatmap_t *im, double mean);
int bytemap_get_kurtosis_on_roi(double *kurtosis, bytemap_t *im, double mean, bitmap_t *roi);
int wordmap_get_kurtosis_on_roi(double *kurtosis, wordmap_t *im, double mean, bitmap_t *roi);
int dwordmap_get_kurtosis_on_roi(double *kurtosis, dwordmap_t *im, double mean, bitmap_t *roi);
int floatmap_get_kurtosis_on_roi(double *kurtosis, floatmap_t *im, double mean, bitmap_t *roi);
int bytemap_get_kurtosis_on_region(double *kurtosis, bytemap_t *im, double mean, int x, int y, int dx, int dy);
int wordmap_get_kurtosis_on_region(double *kurtosis, wordmap_t *im, double mean, int x, int y, int dx, int dy);
int dwordmap_get_kurtosis_on_region(double *kurtosis, dwordmap_t *im, double mean, int x, int y, int dx, int dy);
int floatmap_get_kurtosis_on_region(double *kurtosis, floatmap_t *im, double mean, int x, int y, int dx, int dy);

#ifdef __cplusplus
}
#endif

#endif /* __CENTRAL_MOMENT_H__ */
