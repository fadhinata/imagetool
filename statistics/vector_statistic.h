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
#ifndef __VECTOR_STATISTIC_H__
#define __VECTOR_STATISTIC_H__

#include <vector.h>
#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif 

#define vector_get_mean(v) (vector_get_sum(v) / (double)vector_get_length(v))
#define ivector_get_mean(v) (ivector_get_sum(v) / (double)vector_get_length(v))
  complex_t cvector_get_mean(vector_t *vec);
  double vector_get_2nd_moment(vector_t *vec, double org);
  double ivector_get_2nd_moment(vector_t *vec, double org);
  complex_t cvector_get_2nd_moment(vector_t *vec, complex_t org);
#define vector_get_variance(v) vector_get_2nd_moment(v, vector_get_mean(v))
#define ivector_get_variance(v) ivector_get_2nd_moment(v, vector_get_mean(v))
#define cvector_get_variance(v) cvector_get_2nd_moment(v, cvector_get_mean(v))
#define vector_get_standard_deviation(v) sqrt(vector_get_2nd_moment(v, vector_get_mean(v)))
#define ivector_get_standard_deviation(v) sqrt(ivector_get_2nd_moment(v, ivector_get_mean(v))
  complex_t cvector_get_standard_deviation(vector_t *vec);
  int vector_read_moments(double *scnd, double *thrd, double *frth, vector_t *vec, double org);
  int ivector_read_moments(double *scnd, double *thrd, double *frth, vector_t *vec, double org);
  int cvector_read_moments(complex_t *scnd, complex_t *thrd, complex_t *frth, vector_t *vec, complex_t org);
  double vector_get_skewness(vector_t *vec);
  double ivector_get_skewness(vector_t *vec);
  complex_t cvector_get_skewness(vector_t *vec);
  double vector_get_kurtosis(vector_t *vec);
  double ivector_get_kurtosis(vector_t *vec);
  complex_t cvector_get_kurtosis(vector_t *vec);

#ifdef __cplusplus
}
#endif 

#endif /* __VECTOR_STATISTIC_H__ */
