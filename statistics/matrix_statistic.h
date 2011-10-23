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
#ifndef __MATRIX_STATISTIC_H__
#define __MATRIX_STATISTIC_H__

#include <linear_algebra/matrix.h>
#include <linear_algebra/complex2.h>
#include <pixmap/bitmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  int matrix_get_sum(real_t *sum, matrix_t *p);
  int imatrix_get_sum(real_t *sum, matrix_t *p);
  int cmatrix_get_sum(complex_t *sum, matrix_t *p);
  int matrix_get_sum_on_roi(real_t *sum, matrix_t *p, bitmap_t *roi);
  int imatrix_get_sum_on_roi(real_t *sum, matrix_t *p, bitmap_t *roi);
  int cmatrix_get_sum_on_roi(complex_t *sum, matrix_t *p, bitmap_t *roi);
  int matrix_get_sum_on_region(real_t *sum, matrix_t *p, int c, int r, int dc, int dr);
  int imatrix_get_sum_on_region(real_t *sum, matrix_t *p, int c, int r, int dc, int dr);
  int cmatrix_get_sum_on_region(complex_t *sum, matrix_t *p, int c, int r, int dc, int dr);

  int matrix_get_mean(real_t *mean, matrix_t *mat);
  int imatrix_get_mean(real_t *mean, matrix_t *mat);
  int cmatrix_get_mean(complex_t *mean, matrix_t *mat);
  int matrix_get_mean_on_roi(real_t *mean, matrix_t *mat, bitmap_t *roi);
  int imatrix_get_mean_on_roi(real_t *mean, matrix_t *mat, bitmap_t *roi);
  int cmatrix_get_mean_on_roi(complex_t *mean, matrix_t *mat, bitmap_t *roi);
  int matrix_get_mean_on_region(real_t *mean, matrix_t *mat, int c, int r, int dc, int dr);
  int imatrix_get_mean_on_region(real_t *mean, matrix_t *mat, int c, int r, int dc, int dr);
  int cmatrix_get_mean_on_region(complex_t *mean, matrix_t *mat, int c, int r, int dc, int dr);

  int matrix_get_2nd_moment(real_t *moment, real_t center, matrix_t *mat);
  int imatrix_get_2nd_moment(real_t *moment, real_t center, matrix_t *mat);
  int cmatrix_get_2nd_moment(complex_t *moment, complex_t center, matrix_t *mat);
  int matrix_get_2nd_moment_on_roi(real_t *moment, real_t center, matrix_t *mat, bitmap_t *roi);
  int imatrix_get_2nd_moment_on_roi(real_t *moment, real_t center, matrix_t *mat, bitmap_t *roi);
  int cmatrix_get_2nd_moment_on_roi(complex_t *moment, complex_t center, matrix_t *mat, bitmap_t *roi);
  int matrix_get_2nd_moment_on_region(real_t *moment, real_t center, matrix_t *mat, int c, int r, int dc, int dr);
  int imatrix_get_2nd_moment_on_region(real_t *moment, real_t center, matrix_t *mat, int c, int r, int dc, int dr);
  int cmatrix_get_2nd_moment_on_region(complex_t *moment, complex_t center, matrix_t *mat, int c, int r, int dc, int dr);

  int matrix_get_variance(real_t *var, matrix_t *mat);
  int imatrix_get_variance(real_t *var, matrix_t *mat);
  int cmatrix_get_variance(complex_t *var, matrix_t *mat);
  int matrix_get_variance_on_roi(real_t *var, matrix_t *mat, bitmap_t *roi);
  int imatrix_get_variance_on_roi(real_t *var, matrix_t *mat, bitmap_t *roi);
  int cmatrix_get_variance_on_roi(complex_t *var, matrix_t *mat, bitmap_t *roi);
  int matrix_get_variance_on_region(real_t *var, matrix_t *mat, int c, int r, int dc, int dr);
  int imatrix_get_variance_on_region(real_t *var, matrix_t *mat, int c, int r, int dc, int dr);
  int cmatrix_get_variance_on_region(complex_t *var, matrix_t *mat, int c, int r, int dc, int dr);

  int matrix_get_standard_deviation(real_t *std, matrix_t *mat);
  int imatrix_get_standard_deviation(real_t *std, matrix_t *mat);
  int cmatrix_get_standard_deviation(complex_t *std, matrix_t *mat);
  int matrix_get_standard_deviation_on_roi(real_t *std, matrix_t *mat, bitmap_t *roi);
  int imatrix_get_standard_deviation_on_roi(real_t *std, matrix_t *mat, bitmap_t *roi);
  int cmatrix_get_standard_deviation_on_roi(complex_t *std, matrix_t *mat, bitmap_t *roi);
  int matrix_get_standard_deviation_on_region(real_t *std, matrix_t *mat, int c, int r, int dc, int dr);
  int imatrix_get_standard_deviation_on_region(real_t *std, matrix_t *mat, int c, int r, int dc, int dr);
  int cmatrix_get_standard_deviation_on_region(complex_t *std, matrix_t *mat, int c, int r, int dc, int dr);

  int matrix_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat);
  int imatrix_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat);
  int cmatrix_get_moments(complex_t *m2, complex_t *m3, complex_t *m4, complex_t center, matrix_t *mat);
  int matrix_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat, bitmap_t *roi);
  int imatrix_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat, bitmap_t *roi);
  int cmatrix_get_moments_on_roi(complex_t *m2, complex_t *m3, complex_t *m4, complex_t center, matrix_t *mat, bitmap_t *roi);
  int matrix_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat, int c, int r, int dc, int dr);
  int imatrix_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat, int c, int r, int dc, int dr);
  int cmatrix_get_moments_on_region(complex_t *m2, complex_t *m3, complex_t *m4, complex_t center, matrix_t *mat, int c, int r, int dc, int dr);

  int matrix_get_skewness(real_t *skewness, matrix_t *mat);
  int imatrix_get_skewness(real_t *skewness, matrix_t *mat);
  int cmatrix_get_skewness(complex_t *skewness, matrix_t *mat);
  int matrix_get_skewness_on_roi(real_t *skewness, matrix_t *mat, bitmap_t *roi);
  int imatrix_get_skewness_on_roi(real_t *skewness, matrix_t *mat, bitmap_t *roi);
  int cmatrix_get_skewness_on_roi(complex_t *skewness, matrix_t *mat, bitmap_t *roi);
  int matrix_get_skewness_on_region(real_t *skewness, matrix_t *mat, int c, int r, int dc, int dr);
  int imatrix_get_skewness_on_region(real_t *skewness, matrix_t *mat, int c, int r, int dc, int dr);
  int cmatrix_get_skewness_on_region(complex_t *skewness, matrix_t *mat, int c, int r, int dc, int dr);

  int matrix_get_kurtosis(real_t *kurtosis, matrix_t *mat);
  int imatrix_get_kurtosis(real_t *kurtosis, matrix_t *mat);
  int cmatrix_get_kurtosis(complex_t *kurtosis, matrix_t *mat);
  int matrix_get_kurtosis_on_roi(real_t *kurtosis, matrix_t *mat, bitmap_t *roi);
  int imatrix_get_kurtosis_on_roi(real_t *kurtosis, matrix_t *mat, bitmap_t *roi);
  int cmatrix_get_kurtosis_on_roi(complex_t *kurtosis, matrix_t *mat, bitmap_t *roi);
  int matrix_get_kurtosis_on_region(real_t *kurtosis, matrix_t *mat, int c, int r, int dc, int dr);
  int imatrix_get_kurtosis_on_region(real_t *kurtosis, matrix_t *mat, int c, int r, int dc, int dr);
  int cmatrix_get_kurtosis_on_region(complex_t *kurtosis, matrix_t *mat, int c, int r, int dc, int dr);

  int matrix_get_covariance(real_t *v, matrix_t *p, matrix_t *q);
  int imatrix_get_covariance(real_t *v, matrix_t *p, matrix_t *q);
  int matrix_get_covariance_on_roi(real_t *v, matrix_t *p, matrix_t *q, bitmap_t *roi);
  int imatrix_get_covariance_on_roi(real_t *v, matrix_t *p, matrix_t *q, bitmap_t *roi);
  int matrix_get_covariance_on_region(real_t *v, matrix_t *p, matrix_t *q, int c, int r, int dc, int dr);
  int imatrix_get_covariance_on_region(real_t *v, matrix_t *p, matrix_t *q, int c, int r, int dc, int dr);

  // reference from http://mathworld.wolfram.com/CorrelationCoefficient.html
  int matrix_get_correlation_coefficient(real_t *v, matrix_t *p, matrix_t *q);

  int matrix_get_min(real_t *vmin, matrix_t *mat);
  int matrix_get_min_on_roi(real_t *vmin, matrix_t *mat, bitmap_t *roi);
  int matrix_get_min_on_region(real_t *vmin, matrix_t *mat, int c, int r, int dc, int dr);

  int matrix_get_max(real_t *vmax, matrix_t *mat);
  int matrix_get_max_on_roi(real_t *vmax, matrix_t *mat, bitmap_t *roi);
  int matrix_get_max_on_region(real_t *vmax, matrix_t *mat, int c, int r, int dc, int dr);

  int matrix_get_min_mean_max(real_t *vmin, real_t *vmean, real_t *vmax, matrix_t *mat);
  int matrix_get_min_mean_max_on_roi(real_t *vmin, real_t *vmean, real_t *vmax, matrix_t *mat, bitmap_t *roi);
  int matrix_get_min_mean_max_on_region(real_t *vmin, real_t *vmean, real_t *vmax, matrix_t *mat, int c, int r, int dc, int dr);


#ifdef __cplusplus
}
#endif

#endif /* __MATRIX_STATISTIC_H__ */
