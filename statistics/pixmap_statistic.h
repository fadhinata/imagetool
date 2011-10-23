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

#include <common.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <pixmap/dwordmap.h>
#include <pixmap/floatmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  /* the postfix roi means the region of interest */
  int bytemap_get_min_max(uint8_t *vmin, uint8_t *vmax, bytemap_t *im);
  int wordmap_get_min_max(uint16_t *vmin, uint16_t *vmax, wordmap_t *im);
  int dwordmap_get_min_max(uint32_t *vmin, uint32_t *vmax, dwordmap_t *im);

  int bytemap_get_min_max_on_roi(uint8_t *vmin, uint8_t *vmax, bytemap_t *im, bitmap_t *roi);
  int wordmap_get_min_max_on_roi(uint16_t *vmin, uint16_t *vmax, wordmap_t *im, bitmap_t *roi);
  int dwordmap_get_min_max_on_roi(uint32_t *vmin, uint32_t *vmax, dwordmap_t *im, bitmap_t *roi);

  int bytemap_get_min_max_on_region(uint8_t *vmin, uint8_t *vmax, bytemap_t *im, int x, int y, int dx, int dy);
  int wordmap_get_min_max_on_region(uint16_t *vmin, uint16_t *vmax, wordmap_t *im, int x, int y, int dx, int dy);
  int dwordmap_get_min_max_on_region(uint32_t *vmin, uint32_t *vmax, dwordmap_t *im, int x, int y, int dx, int dy);

  int bytemap_get_sum(real_t *sum, bytemap_t *im);
  int wordmap_get_sum(real_t *sum, wordmap_t *im);
  int dwordmap_get_sum(real_t *sum, dwordmap_t *im);
  int floatmap_get_sum(real_t *sum, floatmap_t *im);
  int bytemap_get_sum_on_roi(real_t *sum, bytemap_t *im, bitmap_t *roi);
  int wordmap_get_sum_on_roi(real_t *sum, wordmap_t *im, bitmap_t *roi);
  int dwordmap_get_sum_on_roi(real_t *sum, dwordmap_t *im, bitmap_t *roi);
  int floatmap_get_sum_on_roi(real_t *sum, floatmap_t *im, bitmap_t *roi);
  int bytemap_get_sum_on_region(real_t *sum, bytemap_t *im, int x, int y, int dx, int dy);
  int wordmap_get_sum_on_region(real_t *sum, wordmap_t *im, int x, int y, int dx, int dy);
  int dwordmap_get_sum_on_region(real_t *sum, dwordmap_t *im, int x, int y, int dx, int dy);
  int floatmap_get_sum_on_region(real_t *sum, floatmap_t *im, int x, int y, int dx, int dy);

  int bytemap_get_mean(real_t *mean, bytemap_t *im);
  int wordmap_get_mean(real_t *mean, wordmap_t *im);
  int dwordmap_get_mean(real_t *mean, dwordmap_t *im);
  int floatmap_get_mean(real_t *mean, floatmap_t *im);
  int bytemap_get_mean_on_roi(real_t *mean, bytemap_t *im, bitmap_t *roi);
  int wordmap_get_mean_on_roi(real_t *mean, wordmap_t *im, bitmap_t *roi);
  int dwordmap_get_mean_on_roi(real_t *mean, dwordmap_t *im, bitmap_t *roi);
  int floatmap_get_mean_on_roi(real_t *mean, floatmap_t *im, bitmap_t *roi);
  int bytemap_get_mean_on_region(real_t *mean, bytemap_t *im, int x, int y, int dx, int dy);
  int wordmap_get_mean_on_region(real_t *mean, wordmap_t *im, int x, int y, int dx, int dy);
  int dwordmap_get_mean_on_region(real_t *mean, dwordmap_t *im, int x, int y, int dx, int dy);
  int floatmap_get_mean_on_region(real_t *mean, floatmap_t *im, int x, int y, int dx, int dy);

  int bytemap_get_variance(real_t *var, bytemap_t *im);
  int wordmap_get_variance(real_t *var, wordmap_t *im);
  int dwordmap_get_variance(real_t *var, dwordmap_t *im);
  int floatmap_get_variance(real_t *var, floatmap_t *im);
  int bytemap_get_variance_on_roi(real_t *var, bytemap_t *im, bitmap_t *roi);
  int wordmap_get_variance_on_roi(real_t *var, wordmap_t *im, bitmap_t *roi);
  int dwordmap_get_variance_on_roi(real_t *var, dwordmap_t *im, bitmap_t *roi);
  int floatmap_get_variance_on_roi(real_t *var, floatmap_t *im, bitmap_t *roi);
  int bytemap_get_variance_on_region(real_t *var, bytemap_t *im, int x, int y, int dx, int dy);
  int wordmap_get_variance_on_region(real_t *var, wordmap_t *im, int x, int y, int dx, int dy);
  int dwordmap_get_variance_on_region(real_t *var, dwordmap_t *im, int x, int y, int dx, int dy);
  int floatmap_get_variance_on_region(real_t *var, floatmap_t *im, int x, int y, int dx, int dy);

  int bytemap_get_standard_deviation(real_t *std, bytemap_t *im);
  int wordmap_get_standard_deviation(real_t *std, wordmap_t *im);
  int dwordmap_get_standard_deviation(real_t *std, dwordmap_t *im);
  int floatmap_get_standard_deviation(real_t *std, floatmap_t *im);
  int bytemap_get_standard_deviation_on_roi(real_t *std, bytemap_t *im, bitmap_t *roi);
  int wordmap_get_standard_deviation_on_roi(real_t *std, wordmap_t *im, bitmap_t *roi);
  int dwordmap_get_standard_deviation_on_roi(real_t *std, dwordmap_t *im, bitmap_t *roi);
  int floatmap_get_standard_deviation_on_roi(real_t *std, floatmap_t *im, bitmap_t *roi);
  int bytemap_get_standard_deviation_on_region(real_t *std, bytemap_t *im, int x, int y, int dx, int dy);
  int wordmap_get_standard_deviation_on_region(real_t *std, wordmap_t *im, int x, int y, int dx, int dy);
  int dwordmap_get_standard_deviation_on_region(real_t *std, dwordmap_t *im, int x, int y, int dx, int dy);
  int floatmap_get_standard_deviation_on_region(real_t *std, floatmap_t *im, int x, int y, int dx, int dy);

  int bytemap_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t mean, bytemap_t *im);
  int wordmap_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t mean, wordmap_t *im);
  int dwordmap_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t mean, dwordmap_t *im);
  int floatmap_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t mean, floatmap_t *im);
  int bytemap_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t mean, bytemap_t *im, bitmap_t *roi);
  int wordmap_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t mean, wordmap_t *im, bitmap_t *roi);
  int dwordmap_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t mean, dwordmap_t *im, bitmap_t *roi);
  int floatmap_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t mean, floatmap_t *im, bitmap_t *roi);
  int bytemap_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t mean, bytemap_t *im, int x, int y, int dx, int dy);
  int wordmap_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t mean, wordmap_t *im, int x, int y, int dx, int dy);
  int dwordmap_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t mean, dwordmap_t *im, int x, int y, int dx, int dy);
  int floatmap_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t mean, floatmap_t *im, int x, int y, int dx, int dy);

  int bytemap_get_skewness(real_t *skewness, bytemap_t *im);
  int wordmap_get_skewness(real_t *skewness, wordmap_t *im);
  int dwordmap_get_skewness(real_t *skewness, dwordmap_t *im);
  int floatmap_get_skewness(real_t *skewness, floatmap_t *im);
  int bytemap_get_skewness_on_roi(real_t *skewness, bytemap_t *im, bitmap_t *roi);
  int wordmap_get_skewness_on_roi(real_t *skewness, wordmap_t *im, bitmap_t *roi);
  int dwordmap_get_skewness_on_roi(real_t *skewness, dwordmap_t *im, bitmap_t *roi);
  int floatmap_get_skewness_on_roi(real_t *skewness, floatmap_t *im, bitmap_t *roi);
  int bytemap_get_skewness_on_region(real_t *skewness, bytemap_t *im, int x, int y, int dx, int dy);
  int wordmap_get_skewness_on_region(real_t *skewness, wordmap_t *im, int x, int y, int dx, int dy);
  int dwordmap_get_skewness_on_region(real_t *skewness, dwordmap_t *im, int x, int y, int dx, int dy);
  int floatmap_get_skewness_on_region(real_t *skewness, floatmap_t *im, int x, int y, int dx, int dy);

  int bytemap_get_kurtosis(real_t *kurtosis, bytemap_t *im);
  int wordmap_get_kurtosis(real_t *kurtosis, wordmap_t *im);
  int dwordmap_get_kurtosis(real_t *kurtosis, dwordmap_t *im);
  int floatmap_get_kurtosis(real_t *kurtosis, floatmap_t *im);
  int bytemap_get_kurtosis_on_roi(real_t *kurtosis, bytemap_t *im, bitmap_t *roi);
  int wordmap_get_kurtosis_on_roi(real_t *kurtosis, wordmap_t *im, bitmap_t *roi);
  int dwordmap_get_kurtosis_on_roi(real_t *kurtosis, dwordmap_t *im, bitmap_t *roi);
  int floatmap_get_kurtosis_on_roi(real_t *kurtosis, floatmap_t *im, bitmap_t *roi);
  int bytemap_get_kurtosis_on_region(real_t *kurtosis, bytemap_t *im, int x, int y, int dx, int dy);
  int wordmap_get_kurtosis_on_region(real_t *kurtosis, wordmap_t *im, int x, int y, int dx, int dy);
  int dwordmap_get_kurtosis_on_region(real_t *kurtosis, dwordmap_t *im, int x, int y, int dx, int dy);
  int floatmap_get_kurtosis_on_region(real_t *kurtosis, floatmap_t *im, int x, int y, int dx, int dy);

#ifdef __cplusplus
}
#endif

#endif /* __PIXMAP_STATISTIC_H__ */
