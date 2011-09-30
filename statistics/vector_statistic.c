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
#include <string.h>
#include <math.h>
#include <assert.h>

#include <complex2.h>
#include <vector.h>
#include <vector_statistic.h>

complex_t cvector_get_mean(vector_t *vec)
{
  complex_t mean;

  assert(vec);

  mean = cvector_get_sum(vec);
  mean.real /= (double)vector_get_length(vec);
  mean.imag /= (double)vector_get_length(vec);

  return mean;
}

double vector_get_2nd_moment(vector_t *vec, double org)
{
  double moment;
  vector_t *tmp;

  assert(vec);

  tmp = vector_new_and_copy(vec);
  vector_copy_vector_subtract_scalar(tmp, vec, org);
  moment = vector_get_norm(tmp) / (double)vector_get_length(tmp);
  vector_destroy(tmp);

  return moment;
}

double ivector_get_2nd_moment(vector_t *vec, double org)
{
  double moment;
  vector_t *tmp;

  assert(vec);
  //assert(ivector_get_buffer(vec));

  tmp = vector_new_and_copy(vec);
  ivector_copy_ivector_subtract_scalar(tmp, vec, org);
  moment = ivector_get_norm(tmp) / vector_get_length(tmp);
  vector_destroy(tmp);

  return moment;
}

complex_t cvector_get_2nd_moment(vector_t *vec, complex_t org)
{
  complex_t moment;
  vector_t *tmp;

  assert(vec);

  tmp = vector_new_and_copy(vec);
  cvector_copy_cvector_subtract_scalar(tmp, vec, org);
  moment = cvector_get_norm(tmp);
  moment.real /= (double)vector_get_length(tmp);
  moment.imag /= (double)vector_get_length(tmp);
  vector_destroy(tmp);

  return moment;
}

/*
 * sqrt(a+i*b) =
 *  sqrt((sqrt(a*a+b*b)+a)/2)+sqrt((sqrt(a*a+b*b)-a)/2)
 * or
 *  sqrt((sqrt(a*a+b*b)+a)/2)-sqrt((sqrt(a*a+b*b)-a)/2)
 */
complex_t cvector_get_standard_deviation(vector_t *vec)
{
  complex_t std, var;

  assert(vec);
  var = cvector_get_2nd_moment(vec, cvector_get_mean(vec));
  complex_copy_complex_sqrt(&std, &var);

  return std;
}

int vector_read_moments(double *scnd, double *thrd, double *frth, vector_t *vec, double org)
{
  vector_t *moment, *diff;

  assert(scnd);
  assert(thrd);
  assert(frth);
  assert(vec);

  moment = vector_new_and_copy(vec);
  diff = vector_new_and_copy(vec);
  vector_copy_vector_subtract_scalar(diff, vec, org);
  vector_copy_vector_multiply_vector(moment, diff, diff);

  *scnd = vector_get_sum(moment) / vector_get_length(moment);
  vector_multiply_vector(moment, diff);
  *thrd = vector_get_sum(moment) / vector_get_length(moment);
  vector_multiply_vector(moment, diff);
  *frth = vector_get_sum(moment) / vector_get_length(moment);

  vector_destroy(diff);
  vector_destroy(moment);

  return vector_get_length(vec);
}

int ivector_read_moments(double *scnd, double *thrd, double *frth, vector_t *vec, double org)
{
  vector_t *moment, *diff;

  assert(scnd);
  assert(thrd);
  assert(frth);
  assert(vec);

  moment = vector_new_and_copy(vec);
  diff = vector_new_and_copy(vec);
  ivector_copy_ivector_subtract_scalar(diff, vec, org);
  ivector_copy_ivector_multiply_ivector(moment, diff, diff);

  *scnd = ivector_get_sum(moment) / vector_get_length(moment);
  ivector_multiply_ivector(moment, diff);
  *thrd = ivector_get_sum(moment) / vector_get_length(moment);
  ivector_multiply_ivector(moment, diff);
  *frth = ivector_get_sum(moment) / vector_get_length(moment);

  vector_destroy(diff);
  vector_destroy(moment);

  return vector_get_length(vec);
}

int cvector_read_moments(complex_t *scnd, complex_t *thrd, complex_t *frth, vector_t *vec, complex_t org)
{
  int n;
  vector_t *moment, *diff;

  assert(scnd);
  assert(thrd);
  assert(frth);
  assert(vec);

  moment = vector_new_and_copy(vec);
  diff = vector_new_and_copy(vec);
  cvector_copy_cvector_subtract_scalar(diff, vec, org);
  cvector_copy_cvector_multiply_cvector(moment, diff, diff);

  n = vector_get_length(vec);
  *scnd = cvector_get_sum(moment);
  scnd->real /= (double)n, scnd->imag /= (double)n;
  cvector_multiply_cvector(moment, diff);
  *thrd = cvector_get_sum(moment);
  thrd->real /= (double)n, thrd->imag /= (double)n;
  cvector_multiply_cvector(moment, diff);
  *frth = cvector_get_sum(moment);
  frth->real /= (double)n, frth->imag /= (double)n;

  vector_destroy(diff);
  vector_destroy(moment);

  return n;
}

double vector_get_skewness(vector_t *vec)
{
  double r2, r3, r4, std;

  assert(vec);

  vector_read_moments(&r2, &r3, &r4, vec, vector_get_mean(vec));
  std = sqrt(r2);

  return r3 / (std * std * std);
}

double ivector_get_skewness(vector_t *vec)
{
  double r2, r3, r4, std;

  assert(vec);

  ivector_read_moments(&r2, &r3, &r4, vec, ivector_get_mean(vec));
  std = sqrt(r2);

  return r3 / (std * std * std);
}

complex_t cvector_get_skewness(vector_t *vec)
{
  complex_t skew, m, std, scnd, thrd, frth;
  complex_t tmp;

  assert(vec);

  m = cvector_get_mean(vec);
  cvector_read_moments(&scnd, &thrd, &frth, vec, m);
  complex_copy_complex_sqrt(&std, &scnd);
  // std^3
  complex_copy_complex_multiply_complex(&tmp, &std, &std);
  complex_multiply_complex(&tmp, &std);
  // complex division
  complex_copy_complex_divide_complex(&skew, &thrd, &tmp);

  return skew;
}

double vector_get_kurtosis(vector_t *vec)
{
  double r2, r3, r4;

  assert(vec);

  vector_read_moments(&r2, &r3, &r4, vec, vector_get_mean(vec));

  return r4/(r2*r2);
}

double ivector_get_kurtosis(vector_t *vec)
{
  double r2, r3, r4;

  assert(vec);

  ivector_read_moments(&r2, &r3, &r4, vec, ivector_get_mean(vec));

  return r4/(r2*r2);
}

complex_t cvector_get_kurtosis(vector_t *vec)
{
  complex_t tmp;
  complex_t scnd, thrd, frth, kurtosis;

  assert(vec);

  cvector_read_moments(&scnd, &thrd, &frth, vec, cvector_get_mean(vec));
  complex_copy_complex_multiply_complex(&tmp, &scnd, &scnd);
  complex_copy_complex_divide_complex(&kurtosis, &frth, &tmp);

  return kurtosis;
}
