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

#include <common.h>
#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>
#include <dwordmap.h>

#ifdef __cplusplus
extern "C" {
#endif

int bytemap_get_central_moments(real_t *m2, real_t *m3, real_t *m4, bytemap_t *im, real_t mean);
int wordmap_get_central_moments(real_t *m2, real_t *m3, real_t *m4, wordmap_t *im, real_t mean);
int dwordmap_get_central_moments(real_t *m2, real_t *m3, real_t *m4, dwordmap_t *im, real_t mean);
int floatmap_get_central_moments(real_t *m2, real_t *m3, real_t *m4, floatmap_t *im, real_t mean);
int bytemap_get_central_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, bytemap_t *im, real_t mean, bitmap_t *roi);
int wordmap_get_central_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, wordmap_t *im, real_t mean, bitmap_t *roi);
int dwordmap_get_central_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, dwordmap_t *im, real_t mean, bitmap_t *roi);
int floatmap_get_central_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, floatmap_t *im, real_t mean, bitmap_t *roi);
int bytemap_get_central_moments_on_region(real_t *m2, real_t *m3, real_t *m4, bytemap_t *im, real_t mean, int x, int y, int dx, int dy);
int wordmap_get_central_moments_on_region(real_t *m2, real_t *m3, real_t *m4, wordmap_t *im, real_t mean, int x, int y, int dx, int dy);
int dwordmap_get_central_moments_on_region(real_t *m2, real_t *m3, real_t *m4, dwordmap_t *im, real_t mean, int x, int y, int dx, int dy);
int floatmap_get_central_moments_on_region(real_t *m2, real_t *m3, real_t *m4, floatmap_t *im, real_t mean, int x, int y, int dx, int dy);

int bytemap_get_skewness(real_t *skewness, bytemap_t *im, real_t mean);
int wordmap_get_skewness(real_t *skewness, wordmap_t *im, real_t mean);
int dwordmap_get_skewness(real_t *skewness, dwordmap_t *im, real_t mean);
int floatmap_get_skewness(real_t *skewness, floatmap_t *im, real_t mean);
int bytemap_get_skewness_on_roi(real_t *skewness, bytemap_t *im, real_t mean, bitmap_t *roi);
int wordmap_get_skewness_on_roi(real_t *skewness, wordmap_t *im, real_t mean, bitmap_t *roi);
int dwordmap_get_skewness_on_roi(real_t *skewness, dwordmap_t *im, real_t mean, bitmap_t *roi);
int floatmap_get_skewness_on_roi(real_t *skewness, floatmap_t *im, real_t mean, bitmap_t *roi);
int bytemap_get_skewness_on_region(real_t *skewness, bytemap_t *im, real_t mean, int x, int y, int dx, int dy);
int wordmap_get_skewness_on_region(real_t *skewness, wordmap_t *im, real_t mean, int x, int y, int dx, int dy);
int dwordmap_get_skewness_on_region(real_t *skewness, dwordmap_t *im, real_t mean, int x, int y, int dx, int dy);
int floatmap_get_skewness_on_region(real_t *skewness, floatmap_t *im, real_t mean, int x, int y, int dx, int dy);

int bytemap_get_kurtosis(real_t *kurtosis, bytemap_t *im, real_t mean);
int wordmap_get_kurtosis(real_t *kurtosis, wordmap_t *im, real_t mean);
int dwordmap_get_kurtosis(real_t *kurtosis, dwordmap_t *im, real_t mean);
int floatmap_get_kurtosis(real_t *kurtosis, floatmap_t *im, real_t mean);
int bytemap_get_kurtosis_on_roi(real_t *kurtosis, bytemap_t *im, real_t mean, bitmap_t *roi);
int wordmap_get_kurtosis_on_roi(real_t *kurtosis, wordmap_t *im, real_t mean, bitmap_t *roi);
int dwordmap_get_kurtosis_on_roi(real_t *kurtosis, dwordmap_t *im, real_t mean, bitmap_t *roi);
int floatmap_get_kurtosis_on_roi(real_t *kurtosis, floatmap_t *im, real_t mean, bitmap_t *roi);
int bytemap_get_kurtosis_on_region(real_t *kurtosis, bytemap_t *im, real_t mean, int x, int y, int dx, int dy);
int wordmap_get_kurtosis_on_region(real_t *kurtosis, wordmap_t *im, real_t mean, int x, int y, int dx, int dy);
int dwordmap_get_kurtosis_on_region(real_t *kurtosis, dwordmap_t *im, real_t mean, int x, int y, int dx, int dy);
int floatmap_get_kurtosis_on_region(real_t *kurtosis, floatmap_t *im, real_t mean, int x, int y, int dx, int dy);

#ifdef __cplusplus
}
#endif

#endif /* __CENTRAL_MOMENT_H__ */
