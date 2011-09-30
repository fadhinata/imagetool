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
#ifndef __PIXMAP_STATISTIC_H__
#define __PIXMAP_STATISTIC_H__

#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>
#include <dwordmap.h>
#include <floatmap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* the postfix roi means the region of interest */
int bytemap_get_min_max(unsigned char *vmin, unsigned char *vmax, bytemap_t *im);

int bytemap_get_min_max_on_roi(unsigned char *vmin, unsigned char *vmax, bytemap_t *im, bitmap_t *roi);

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

int bytemap_get_variance(double *var, bytemap_t *im);
int wordmap_get_variance(double *var, wordmap_t *im);
int dwordmap_get_variance(double *var, dwordmap_t *im);
int floatmap_get_variance(double *var, floatmap_t *im);
int bytemap_get_variance_on_roi(double *var, bytemap_t *im, bitmap_t *roi);
int wordmap_get_variance_on_roi(double *var, wordmap_t *im, bitmap_t *roi);
int dwordmap_get_variance_on_roi(double *var, dwordmap_t *im, bitmap_t *roi);
int floatmap_get_variance_on_roi(double *var, floatmap_t *im, bitmap_t *roi);
int bytemap_get_variance_on_region(double *var, bytemap_t *im, int x, int y, int dx, int dy);
int wordmap_get_variance_on_region(double *var, wordmap_t *im, int x, int y, int dx, int dy);
int dwordmap_get_variance_on_region(double *var, dwordmap_t *im, int x, int y, int dx, int dy);
int floatmap_get_variance_on_region(double *var, floatmap_t *im, int x, int y, int dx, int dy);

int bytemap_get_standard_deviation(double *std, bytemap_t *im);
int wordmap_get_standard_deviation(double *std, wordmap_t *im);
int dwordmap_get_standard_deviation(double *std, dwordmap_t *im);
int floatmap_get_standard_deviation(double *std, floatmap_t *im);
int bytemap_get_standard_deviation_on_roi(double *std, bytemap_t *im, bitmap_t *roi);
int wordmap_get_standard_deviation_on_roi(double *std, wordmap_t *im, bitmap_t *roi);
int dwordmap_get_standard_deviation_on_roi(double *std, dwordmap_t *im, bitmap_t *roi);
int floatmap_get_standard_deviation_on_roi(double *std, floatmap_t *im, bitmap_t *roi);
int bytemap_get_standard_deviation_on_region(double *std, bytemap_t *im, int x, int y, int dx, int dy);
int wordmap_get_standard_deviation_on_region(double *std, wordmap_t *im, int x, int y, int dx, int dy);
int dwordmap_get_standard_deviation_on_region(double *std, dwordmap_t *im, int x, int y, int dx, int dy);
int floatmap_get_standard_deviation_on_region(double *std, floatmap_t *im, int x, int y, int dx, int dy);

int bytemap_get_moments(double *m2, double *m3, double *m4, double mean, bytemap_t *im);
int wordmap_get_moments(double *m2, double *m3, double *m4, double mean, wordmap_t *im);
int dwordmap_get_moments(double *m2, double *m3, double *m4, double mean, dwordmap_t *im);
int floatmap_get_moments(double *m2, double *m3, double *m4, double mean, floatmap_t *im);
int bytemap_get_moments_on_roi(double *m2, double *m3, double *m4, double mean, bytemap_t *im, bitmap_t *roi);
int wordmap_get_moments_on_roi(double *m2, double *m3, double *m4, double mean, wordmap_t *im, bitmap_t *roi);
int dwordmap_get_moments_on_roi(double *m2, double *m3, double *m4, double mean, dwordmap_t *im, bitmap_t *roi);
int floatmap_get_moments_on_roi(double *m2, double *m3, double *m4, double mean, floatmap_t *im, bitmap_t *roi);
int bytemap_get_moments_on_region(double *m2, double *m3, double *m4, double mean, bytemap_t *im, int x, int y, int dx, int dy);
int wordmap_get_moments_on_region(double *m2, double *m3, double *m4, double mean, wordmap_t *im, int x, int y, int dx, int dy);
int dwordmap_get_moments_on_region(double *m2, double *m3, double *m4, double mean, dwordmap_t *im, int x, int y, int dx, int dy);
int floatmap_get_moments_on_region(double *m2, double *m3, double *m4, double mean, floatmap_t *im, int x, int y, int dx, int dy);

int bytemap_get_skewness(double *skewness, bytemap_t *im);
int wordmap_get_skewness(double *skewness, wordmap_t *im);
int dwordmap_get_skewness(double *skewness, dwordmap_t *im);
int floatmap_get_skewness(double *skewness, floatmap_t *im);
int bytemap_get_skewness_on_roi(double *skewness, bytemap_t *im, bitmap_t *roi);
int wordmap_get_skewness_on_roi(double *skewness, wordmap_t *im, bitmap_t *roi);
int dwordmap_get_skewness_on_roi(double *skewness, dwordmap_t *im, bitmap_t *roi);
int floatmap_get_skewness_on_roi(double *skewness, floatmap_t *im, bitmap_t *roi);
int bytemap_get_skewness_on_region(double *skewness, bytemap_t *im, int x, int y, int dx, int dy);
int wordmap_get_skewness_on_region(double *skewness, wordmap_t *im, int x, int y, int dx, int dy);
int dwordmap_get_skewness_on_region(double *skewness, dwordmap_t *im, int x, int y, int dx, int dy);
int floatmap_get_skewness_on_region(double *skewness, floatmap_t *im, int x, int y, int dx, int dy);

int bytemap_get_kurtosis(double *kurtosis, bytemap_t *im);
int wordmap_get_kurtosis(double *kurtosis, wordmap_t *im);
int dwordmap_get_kurtosis(double *kurtosis, dwordmap_t *im);
int floatmap_get_kurtosis(double *kurtosis, floatmap_t *im);
int bytemap_get_kurtosis_on_roi(double *kurtosis, bytemap_t *im, bitmap_t *roi);
int wordmap_get_kurtosis_on_roi(double *kurtosis, wordmap_t *im, bitmap_t *roi);
int dwordmap_get_kurtosis_on_roi(double *kurtosis, dwordmap_t *im, bitmap_t *roi);
int floatmap_get_kurtosis_on_roi(double *kurtosis, floatmap_t *im, bitmap_t *roi);
int bytemap_get_kurtosis_on_region(double *kurtosis, bytemap_t *im, int x, int y, int dx, int dy);
int wordmap_get_kurtosis_on_region(double *kurtosis, wordmap_t *im, int x, int y, int dx, int dy);
int dwordmap_get_kurtosis_on_region(double *kurtosis, dwordmap_t *im, int x, int y, int dx, int dy);
int floatmap_get_kurtosis_on_region(double *kurtosis, floatmap_t *im, int x, int y, int dx, int dy);

#ifdef __cplusplus
}
#endif

#endif /* __PIXMAP_STATISTIC_H__ */
