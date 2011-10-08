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

#include <common.h>
#include <bitmap.h>
#include <complex2.h>
#include <matrix.h>
#include <matrix_statistic.h>

//#define BUG_FIX
#undef BUG_FIX

#define MATRIX_GET_SUM(sum, buffer, columns, rows) {	\
    int i, j, n;					\
    real_t *buf;					\
    assert(sum);					\
    assert(buffer);					\
    buf = buffer;					\
    *(sum) = 0.0, n = 0;				\
    for (i = 0; i < rows; i++) {			\
      for (j = 0; j < columns; j++) {			\
	*(sum) += *(buf+j);				\
	n++;						\
      }							\
      buf += columns;					\
    }							\
    return n;						\
  }

#define MATRIX_GET_SUM_ON_ROI(sum, mbuf, columns, rows, roi) {	\
    int i, j, n;						\
    uint8_t *roibuf;						\
    real_t *buf;						\
    assert(sum);						\
    assert(mbuf);						\
    assert(roi);						\
    assert((roi)->header.width == columns);			\
    assert((roi)->header.height == rows);			\
    roibuf = (roi)->buffer;					\
    buf = mbuf;							\
    *(sum) = 0.0, n = 0;					\
    for (i = 0; i < rows; i++) {				\
      for (j = 0; j < columns; j++) {				\
	if ((*(roibuf+(j>>3)))&(1<<(j%8))) {			\
	  *(sum) += *(buf+j);					\
	  n++;							\
	}							\
      }								\
      buf += columns;						\
      roibuf += (roi)->header.pitch;				\
    }								\
    return n;							\
  }

#define MATRIX_GET_SUM_ON_REGION(sum, mbuf, columns, rows, c, r, dc, dr) { \
    int i, j, n, nc, nr;						\
    real_t *buf;							\
    assert(sum);							\
    assert(mbuf);							\
    assert(c >= 0 && c < columns);					\
    assert(r >= 0 && r < rows);						\
    assert(dc > 0);							\
    assert(dr > 0);							\
    *(sum) = 0.0, n = 0;						\
    buf = (mbuf)+(r)*(columns);						\
    nc = min(columns, c+dc);						\
    nr = min(rows, r+dr);						\
    for (i = r; i < nr; i++) {						\
      for (j = c; j < nc; j++) {					\
	*(sum) += *(buf+j);						\
	n++;								\
      }									\
      buf += columns;							\
    }									\
    return n;								\
  }

int matrix_get_sum(real_t *sum, matrix_t *p)
{
  MATRIX_GET_SUM(sum, p->real, p->columns, p->rows);
}

int imatrix_get_sum(real_t *sum, matrix_t *p)
{
  MATRIX_GET_SUM(sum, p->imaginary, p->columns, p->rows);
}

int cmatrix_get_sum(complex_t *sum, matrix_t *p)
{
  int n;
  assert(sum);
  assert(p);
  assert(p->imaginary);
  n = matrix_get_sum(&(sum->real), p);
  imatrix_get_sum(&(sum->imag), p);
  return n;
}

int matrix_get_sum_on_roi(real_t *sum, matrix_t *p, bitmap_t *roi)
{
  MATRIX_GET_SUM_ON_ROI(sum, p->real, p->columns, p->rows, roi);
}

int imatrix_get_sum_on_roi(real_t *sum, matrix_t *p, bitmap_t *roi)
{
  MATRIX_GET_SUM_ON_ROI(sum, p->imaginary, p->columns, p->rows, roi);
}

int cmatrix_get_sum_on_roi(complex_t *sum, matrix_t *p, bitmap_t *roi)
{
  int n;
  assert(sum);
  assert(p);
  assert(p->imaginary);
  assert(roi);
  assert(p->columns == roi->header.width);
  assert(p->rows == roi->header.height);
  n = matrix_get_sum_on_roi(&(sum->real), p, roi);
  imatrix_get_sum_on_roi(&(sum->imag), p, roi);
  return n;
}

int matrix_get_sum_on_region(real_t *sum, matrix_t *p, int c, int r, int dc, int dr)
{
  MATRIX_GET_SUM_ON_REGION(sum, p->real, p->columns, p->rows, c, r, dc, dr);
}

int imatrix_get_sum_on_region(real_t *sum, matrix_t *p, int c, int r, int dc, int dr)
{
  MATRIX_GET_SUM_ON_REGION(sum, p->imaginary, p->columns, p->rows, c, r, dc, dr);
}

int cmatrix_get_sum_on_region(complex_t *sum, matrix_t *p, int c, int r, int dc, int dr)
{
  int n;
  assert(sum);
  assert(p);
  assert(p->imaginary);
  assert(c >= 0 && c < p->columns);
  assert(r >= 0 && r < p->rows);
  assert(dc > 0);
  assert(dr > 0);
  n = matrix_get_sum_on_region(&(sum->real), p, c, r, dc, dr);
  imatrix_get_sum_on_region(&(sum->imag), p, c, r, dc, dr);
  return n;
}

int matrix_get_mean(real_t *mean, matrix_t *mat)
{
  int n;
  assert(mean);
  assert(mat);
  n = matrix_get_sum(mean, mat);
  if (n > 0) *mean /= (real_t)n;
  return n;
}

int imatrix_get_mean(real_t *mean, matrix_t *mat)
{
  int n;
  assert(mean);
  assert(mat);
  assert(mat->imaginary);
  n = imatrix_get_sum(mean, mat);
  if (n > 0) *mean /= (real_t)n;
  return n;
}

int cmatrix_get_mean(complex_t *mean, matrix_t *mat)
{
  int n;
  assert(mean);
  assert(mat);
  assert(mat->imaginary);
  n = cmatrix_get_sum(mean, mat);
  if (n > 0) {
    mean->real /= (real_t)n;
    mean->imag /= (real_t)n;
  }
  return n;
}

int matrix_get_mean_on_roi(real_t *mean, matrix_t *mat, bitmap_t *roi)
{
  int n;
  assert(mean);
  assert(mat);
  assert(roi);
  assert(mat->columns == roi->header.width);
  assert(mat->rows == roi->header.height);
  n = matrix_get_sum_on_roi(mean, mat, roi);
  if (n > 0) *mean /= (real_t)n;
  return n;
}

int imatrix_get_mean_on_roi(real_t *mean, matrix_t *mat, bitmap_t *roi)
{
  int n;
  assert(mean);
  assert(mat);
  assert(mat->imaginary);
  assert(roi);
  assert(mat->columns == roi->header.width);
  assert(mat->rows == roi->header.height);
  n = imatrix_get_sum_on_roi(mean, mat, roi);
  if (n > 0) *mean /= (real_t)n;
  return n;
}

int cmatrix_get_mean_on_roi(complex_t *mean, matrix_t *mat, bitmap_t *roi)
{
  int n;
  assert(mean);
  assert(mat);
  assert(mat->imaginary);
  assert(roi);
  assert(mat->columns == roi->header.width);
  assert(mat->rows == roi->header.height);
  n = cmatrix_get_sum_on_roi(mean, mat, roi);
  if (n > 0) {
    mean->real /= (real_t)n;
    mean->imag /= (real_t)n;
  }
  return n;
}

int matrix_get_mean_on_region(real_t *mean, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  assert(mean);
  assert(mat);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  n = matrix_get_sum_on_region(mean, mat, c, r, dc, dr);
  if (n != 0) *mean /= (real_t)n;
  return n;
}

int imatrix_get_mean_on_region(real_t *mean, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  assert(mean);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  n = imatrix_get_sum_on_region(mean, mat, c, r, dc, dr);
  if (n != 0) *mean /= (real_t)n;
  return n;
}

int cmatrix_get_mean_on_region(complex_t *mean, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  assert(mean);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  n = cmatrix_get_sum_on_region(mean, mat, c, r, dc, dr);
  if (n != 0) {
    mean->real /= (real_t)n;
    mean->imag /= (real_t)n;
  }
  return n;
}

#define MATRIX_GET_2ND_MOMENT(var, center, buffer, columns, rows) {	\
    int n, i, j;							\
    real_t *buf, tmp;							\
    assert(var);							\
    assert(buffer);							\
    buf = buffer;							\
    *(var) = 0.0, n = 0;						\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	tmp = *(buf+j)-(center);					\
	*(var) += tmp*tmp;						\
	n++;								\
      }									\
      buf += columns;							\
    }									\
    if (n != 0) *(var) /= (real_t)n;					\
    return n;								\
  }

#define MATRIX_GET_2ND_MOMENT_ON_ROI(var, center, mbuf, columns, rows, roi) { \
    int n, i, j;							\
    real_t *buf, tmp;							\
    uint8_t *roibuf;							\
    assert(var);							\
    assert(mbuf);							\
    assert(roi);							\
    assert(columns == (roi)->header.width);				\
    assert(rows == (roi)->header.height);				\
    buf = mbuf;								\
    roibuf = (roi)->buffer;						\
    *(var) = 0.0, n = 0;						\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	if ((*(roibuf+(j>>3)))&(1<<(j%8))) {				\
	  tmp = *(buf+j)-center;					\
	  *(var) += tmp*tmp;						\
	  n++;								\
	}								\
      }									\
      buf += columns;							\
      roibuf += (roi)->header.pitch;					\
    }									\
    if (n != 0) *(var) /= (real_t)n;					\
    return n;								\
  }

#define MATRIX_GET_2ND_MOMENT_ON_REGION(var, center, mbuf, columns, rows, c, r, dc, dr) { \
    int n, nc, nr, i, j;						\
    real_t *buf, tmp;							\
    assert(var);							\
    assert(mbuf);							\
    assert(c >= 0 && c < columns);					\
    assert(r >= 0 && r < rows);						\
    assert(dc > 0);							\
    assert(dr > 0);							\
    nc = min(c+dc, columns);						\
    nr = min(r+dr, rows);						\
    buf = mbuf+(r)*(columns);						\
    *(var) = 0.0, n = 0;						\
    for (i = r; i < nr; i++) {						\
      for (j = c; j < nc; j++) {					\
	tmp = *(buf+j)-center;						\
	*(var) += tmp*tmp;						\
	n++;								\
      }									\
      buf += columns;							\
    }									\
    if (n != 0) *(var) /= (real_t)n;					\
    return n;								\
  }

int matrix_get_2nd_moment(real_t *moment, real_t center, matrix_t *mat)
{
  MATRIX_GET_2ND_MOMENT(moment, center, mat->real, mat->columns, mat->rows);
}

int imatrix_get_2nd_moment(real_t *moment, real_t center, matrix_t *mat)
{
  MATRIX_GET_2ND_MOMENT(moment, center, mat->imaginary, mat->columns, mat->rows);
}

int cmatrix_get_2nd_moment(complex_t *moment, complex_t center, matrix_t *mat)
{
  int n, i, j;
  real_t *rbuf, *ibuf;
  real_t real, imag;
  assert(moment);
  assert(mat);
  assert(mat->imaginary);
  n = 0;
  moment->real = moment->imag = 0.0;
  // (a+i*b)^2 = (a^2-b^2)+i*(2*a*b)
  rbuf = mat->real;
  ibuf = mat->imaginary;
  for (i = 0; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      real = *(rbuf+j)-center.real;
      imag = *(ibuf+j)-center.imag;
      moment->real += real*real-imag*imag;
      moment->imag += 2.0*real*imag;
      n++;
    }
    rbuf += mat->columns;
    ibuf += mat->columns;
  }
  if (n > 0) {
    moment->real /= (real_t)n;
    moment->imag /= (real_t)n;
  }
  return n;	
}

int matrix_get_2nd_moment_on_roi(real_t *moment, real_t center, matrix_t *mat, bitmap_t *roi)
{
  MATRIX_GET_2ND_MOMENT_ON_ROI(moment, center, mat->real, mat->columns, mat->rows, roi);
}

int imatrix_get_2nd_moment_on_roi(real_t *moment, real_t center, matrix_t *mat, bitmap_t *roi)
{
  MATRIX_GET_2ND_MOMENT_ON_ROI(moment, center, mat->imaginary, mat->columns, mat->rows, roi);
}

int cmatrix_get_2nd_moment_on_roi(complex_t *moment, complex_t center, matrix_t *mat, bitmap_t *roi)
{
  int n, i, j;
  real_t *rbuf, *ibuf;
  real_t real, imag;
  uint8_t *roibuf;
  assert(moment);
  assert(mat);
  assert((mat)->imaginary);
  assert(roi);
  assert((mat)->columns == (roi)->header.width);
  assert((mat)->rows == (roi)->header.height);
  n = 0;
  (moment)->real = (moment)->imag = 0.0;
  // (a+i*b)^2 = (a^2-b^2)+i*(2*a*b)
  rbuf = mat->real;
  ibuf = mat->imaginary;
  roibuf = roi->buffer;
  for (i = 0; i < (mat)->rows; i++) {
    for (j = 0; j < (mat)->columns; j++) {
      if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
	real = *(rbuf+j)-center.real;
	imag = *(ibuf+j)-center.imag;
	(moment)->real += real*real-imag*imag;
	(moment)->imag += 2.0*real*imag;
	n++;
      }
    }
    rbuf += mat->columns;
    ibuf += mat->columns;
    roibuf += roi->header.pitch;
  }
  if (n > 0) {
    moment->real /= (real_t)n;
    moment->imag /= (real_t)n;
  }
  return n;	
}

int matrix_get_2nd_moment_on_region(real_t *moment, real_t center, matrix_t *mat, int c, int r, int dc, int dr)
{
  MATRIX_GET_2ND_MOMENT_ON_REGION(moment, center, mat->real, mat->columns, mat->rows, c, r, dc, dr);
}

int imatrix_get_2nd_moment_on_region(real_t *moment, real_t center, matrix_t *mat, int c, int r, int dc, int dr)
{
  MATRIX_GET_2ND_MOMENT_ON_REGION(moment, center, mat->imaginary, mat->columns, mat->rows, c, r, dc, dr);
}

int cmatrix_get_2nd_moment_on_region(complex_t *moment, complex_t center, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n, i, j, nc, nr;
  real_t *rbuf, *ibuf;
  real_t real, imag;
  assert(moment);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  nc = min(c+dc, mat->columns);
  nr = min(r+dr, mat->rows);
  n = 0;
  moment->real = moment->imag = 0.0;
  // (a+i*b)^2 = (a^2-b^2)+i*(2*a*b)
  rbuf = mat->real;
  ibuf = mat->imaginary;
  for (i = r; i < nr; i++) {
    for (j = c; j < nc; j++) {
      real = *(rbuf+j)-center.real;
      imag = *(ibuf+j)-center.imag;
      moment->real += real*real-imag*imag;
      moment->imag += 2.0*real*imag;
      n++;
    }
    rbuf += mat->columns;
    ibuf += mat->columns;
  }
  if (n > 0) {
    moment->real /= (real_t)n;
    moment->imag /= (real_t)n;
  }
  return n;	
}

int matrix_get_variance(real_t *var, matrix_t *mat)
{
  int n;
  real_t mean;
  assert(var);
  assert(mat);
  n = matrix_get_mean(&mean, mat);
  if (n > 0) matrix_get_2nd_moment(var, mean, mat);
  return n;
}

int imatrix_get_variance(real_t *var, matrix_t *mat)
{
  int n;
  real_t mean;
  assert(var);
  assert(mat);
  assert(mat->imaginary);
  n = imatrix_get_mean(&mean, mat);
  if (n > 0) imatrix_get_2nd_moment(var, mean, mat);
  return n;
}

int cmatrix_get_variance(complex_t *var, matrix_t *mat)
{
  int n;
  complex_t mean;
  assert(var);
  assert(mat);
  assert(mat->imaginary);
  n = cmatrix_get_mean(&mean, mat);
  if (n > 0) cmatrix_get_2nd_moment(var, mean, mat);
  return n;
}

int matrix_get_variance_on_roi(real_t *var, matrix_t *mat, bitmap_t *roi)
{
  int n;
  real_t mean;
  assert(var);
  assert(mat);
  assert(roi);
  n = matrix_get_mean_on_roi(&mean, mat, roi);
  if (n > 0) matrix_get_2nd_moment_on_roi(var, mean, mat, roi);
  return n;
}

int imatrix_get_variance_on_roi(real_t *var, matrix_t *mat, bitmap_t *roi)
{
  int n;
  real_t mean;
  assert(var);
  assert(mat);
  assert(mat->imaginary);
  assert(roi);
  n = imatrix_get_mean_on_roi(&mean, mat, roi);
  if (n > 0) imatrix_get_2nd_moment_on_roi(var, mean, mat, roi);
  return n;
}

int cmatrix_get_variance_on_roi(complex_t *var, matrix_t *mat, bitmap_t *roi)
{
  int n;
  complex_t mean;
  assert(var);
  assert(mat);
  assert(mat->imaginary);
  assert(roi);
  n = cmatrix_get_mean_on_roi(&mean, mat, roi);
  if (n > 0) cmatrix_get_2nd_moment_on_roi(var, mean, mat, roi);
  return n;
}

int matrix_get_variance_on_region(real_t *var, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  real_t mean;
  assert(var);
  assert(mat);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  n = matrix_get_mean_on_region(&mean, mat, c, r, dc, dr);
  if (n > 0) matrix_get_2nd_moment_on_region(var, mean, mat, c, r, dc, dr);
  return n;
}

int imatrix_get_variance_on_region(real_t *var, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  real_t mean;
  assert(var);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  n = imatrix_get_mean_on_region(&mean, mat, c, r, dc, dr);
  if (n > 0) imatrix_get_2nd_moment_on_region(var, mean, mat, c, r, dc, dr);
  return n;
}

int cmatrix_get_variance_on_region(complex_t *var, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  complex_t mean;
  assert(var);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  n = cmatrix_get_mean_on_region(&mean, mat, c, r, dc, dr);
  if (n > 0) cmatrix_get_2nd_moment_on_region(var, mean, mat, c, r, dc, dr);
  return n;
}

int matrix_get_standard_deviation(real_t *std, matrix_t *mat)
{
  int n;
  real_t mean;
  assert(std);
  assert(mat);
  n = matrix_get_mean(&mean, mat);
  if (n > 0) {
    matrix_get_2nd_moment(std, mean, mat);
    *std = sqrt(*std);
  }
  return n;
}

int imatrix_get_standard_deviation(real_t *std, matrix_t *mat)
{
  int n;
  real_t mean;
  assert(std);
  assert(mat);
  assert(mat->imaginary);
  n = imatrix_get_mean(&mean, mat);
  if (n > 0) {
    imatrix_get_2nd_moment(std, mean, mat);
    *std = sqrt(*std);
  }
  return n;
}

int cmatrix_get_standard_deviation(complex_t *std, matrix_t *mat)
{
  int n;
  complex_t mean, var;
  assert(std);
  assert(mat);
  assert(mat->imaginary);
  n = cmatrix_get_mean(&mean, mat);
  if (n > 0) {
    cmatrix_get_2nd_moment(&var, mean, mat);
    complex_copy_complex_sqrt(std, &var);
  }
  return n;
}

int matrix_get_standard_deviation_on_roi(real_t *std, matrix_t *mat, bitmap_t *roi)
{
  int n;
  real_t mean, var;
  assert(std);
  assert(mat);
  assert(roi);
  assert(mat->columns == roi->header.width);
  assert(mat->rows == roi->header.height);
  n = matrix_get_mean_on_roi(&mean, mat, roi);
  if (n > 0) {
    matrix_get_2nd_moment_on_roi(&var, mean, mat, roi);
    *std = sqrt(var);
  }
  return n;
}

int imatrix_get_standard_deviation_on_roi(real_t *std, matrix_t *mat, bitmap_t *roi)
{
  int n;
  real_t mean, var;
  assert(std);
  assert(mat);
  assert(mat->imaginary);
  assert(roi);
  assert(mat->columns == roi->header.width);
  assert(mat->rows == roi->header.height);
  n = imatrix_get_mean_on_roi(&mean, mat, roi);
  if (n > 0) {
    imatrix_get_2nd_moment_on_roi(&var, mean, mat, roi);
    *std = sqrt(var);
  }
  return n;
}

int cmatrix_get_standard_deviation_on_roi(complex_t *std, matrix_t *mat, bitmap_t *roi)
{
  int n;
  complex_t mean, var;
  assert(std);
  assert(mat);
  assert(mat->imaginary);
  assert(roi);
  assert(mat->columns == roi->header.width);
  assert(mat->rows == roi->header.height);
  n = cmatrix_get_mean_on_roi(&mean, mat, roi);
  if (n > 0) {
    cmatrix_get_2nd_moment_on_roi(&var, mean, mat, roi);
    complex_copy_complex_sqrt(std, &var);
  }
  return n;
}

int matrix_get_standard_deviation_on_region(real_t *std, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  real_t mean, var;
  assert(std);
  assert(mat);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  n = matrix_get_mean_on_region(&mean, mat, c, r, dc, dr);
  if (n > 0) {
    matrix_get_2nd_moment_on_region(&var, mean, mat, c, r, dc, dr);
    *std = sqrt(var);
  }
  return n;
}

int imatrix_get_standard_deviation_on_region(real_t *std, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  real_t mean, var;
  assert(std);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  n = imatrix_get_mean_on_region(&mean, mat, c, r, dc, dr);
  if (n > 0) {
    imatrix_get_2nd_moment_on_region(&var, mean, mat, c, r, dc, dr);
    *std = sqrt(var);
  }
  return n;
}

int cmatrix_get_standard_deviation_on_region(complex_t *std, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  complex_t mean, var;
  assert(std);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  n = cmatrix_get_mean_on_region(&mean, mat, c, r, dc, dr);
  if (n > 0) {
    cmatrix_get_2nd_moment_on_region(&var, mean, mat, c, r, dc, dr);
    complex_copy_complex_sqrt(std, &var);
  }
  return n;
}

#define MATRIX_GET_MOMENTS(m2, m3, m4, center, buffer, columns, rows) { \
    int n, i, j;							\
    real_t *buf, tmp, m1;						\
    assert(m2);								\
    assert(m3);								\
    assert(m4);								\
    assert(buffer);							\
    buf = buffer;							\
    *(m2) = *(m3) = *(m4) = 0.0, n = 0;					\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	m1 = *(buf+j)-center;						\
	tmp = m1;							\
	tmp *= m1;							\
	*(m2) += tmp;							\
	tmp *= m1;							\
	*(m3) += tmp;							\
	tmp *= m1;							\
	*(m4) += tmp;							\
	n++;								\
      }									\
      buf += columns;							\
    }									\
    if (n > 0) {							\
      *(m2) /= (real_t)n;						\
      *(m3) /= (real_t)n;						\
      *(m4) /= (real_t)n;						\
    }									\
    return n;								\
  }

#define MATRIX_GET_MOMENTS_ON_ROI(m2, m3, m4, center, mbuf, columns, rows, roi) { \
    int n, i, j;							\
    real_t *buf, tmp, m1;						\
    uint8_t *roibuf;							\
    assert(m2);								\
    assert(m3);								\
    assert(m4);								\
    assert(mbuf);							\
    assert(roi);							\
    assert(columns == (roi)->header.width);				\
    assert(rows == (roi)->header.height);				\
    buf = mbuf;								\
    roibuf = (roi)->buffer;						\
    *(m2) = *(m3) = *(m4) = 0.0, n = 0;					\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	if ((*(roibuf+(j>>3)))&(1<<(j%8))) {				\
	  m1 = *(buf+j)-center;						\
	  tmp = m1;							\
	  tmp *= m1;							\
	  *(m2) += tmp;							\
	  tmp *= m1;							\
	  *(m3) += tmp;							\
	  tmp *= m1;							\
	  *(m4) += tmp;							\
	  n++;								\
	}								\
      }									\
      buf += columns;							\
      roibuf += (roi)->header.pitch;					\
    }									\
    if (n > 0) {							\
      *(m2) /= (real_t)n;						\
      *(m3) /= (real_t)n;						\
      *(m4) /= (real_t)n;						\
    }									\
    return n;								\
  }

#define MATRIX_GET_MOMENTS_ON_REGION(m2, m3, m4, center, mbuf, columns, rows, c, r, dc, dr) { \
    int n, i, j;							\
    int nc, nr;								\
    real_t *buf, tmp, m1;						\
    assert(m2);								\
    assert(m3);								\
    assert(m4);								\
    assert(mbuf);							\
    assert(c >= 0 && c < columns);					\
    assert(r >= 0 && r < rows);						\
    assert(dc > 0);							\
    assert(dr > 0);							\
    buf = mbuf;								\
    nc = min(c+dc, columns);						\
    nr = min(r+dr, rows);						\
    *(m2) = *(m3) = *(m4) = 0.0, n = 0;					\
    for (i = r; i < nr; i++) {						\
      for (j = c; j < nc; j++) {					\
	m1 = *(buf+j)-center;						\
	tmp = m1;							\
	tmp *= m1;							\
	*(m2) += tmp;							\
	tmp *= m1;							\
	*(m3) += tmp;							\
	tmp *= m1;							\
	*(m4) += tmp;							\
	n++;								\
      }									\
      buf += columns;							\
    }									\
    if (n > 0) {							\
      *(m2) /= (real_t)n;						\
      *(m3) /= (real_t)n;						\
      *(m4) /= (real_t)n;						\
    }									\
    return n;								\
  }

int matrix_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat)
{
  MATRIX_GET_MOMENTS(m2, m3, m4, center, mat->real, mat->columns, mat->rows);
}

int imatrix_get_moments(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat)
{
  MATRIX_GET_MOMENTS(m2, m3, m4, center, mat->imaginary, mat->columns, mat->rows);
}

int cmatrix_get_moments(complex_t *m2, complex_t *m3, complex_t *m4, complex_t center, matrix_t *mat)
{
  int n, i, j;
  complex_t m1, tmp;
  real_t *rbuf, *ibuf;
  assert(m2);
  assert(m3);
  assert(m4);
  assert(mat);
  assert(mat->imaginary);
  n = 0;
  m2->real = m2->imag = m3->real = m3->imag = m4->real = m4->imag = 0.0;
  // (a+i*b)*(c+i*d) = (a*c-b*d)+i*(a*d+b*c)
  rbuf = mat->real, ibuf = mat->imaginary;
  for (i = 0; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      m1.real = *(rbuf+j)-center.real;
      m1.imag = *(ibuf+j)-center.imag;
      tmp.real = m1.real*m1.real-m1.imag*m1.imag;
      tmp.imag = 2.0*m1.real*m1.imag; //m1.real*m1.imag+m1.imag*m1.real;
      m2->real += tmp.real, m2->imag += tmp.imag;
      tmp.real = tmp.real*m1.real-tmp.imag*m1.imag;
      tmp.imag = tmp.real*m1.imag+tmp.imag*m1.real;
      m3->real += tmp.real, m3->imag += tmp.imag;
      tmp.real = tmp.real*m1.real-tmp.imag*m1.imag;
      tmp.imag = tmp.real*m1.imag+tmp.imag*m1.real;
      m4->real += tmp.real, m4->imag += tmp.imag;
      n++;
    }
    rbuf += mat->columns;
    ibuf += mat->columns;
  }
  if (n > 0) {
    m2->real /= (real_t)n, m2->imag /= (real_t)n;
    m3->real /= (real_t)n, m3->imag /= (real_t)n;
    m4->real /= (real_t)n, m4->imag /= (real_t)n;
  }
  return n;
}

int matrix_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat, bitmap_t *roi)
{
  MATRIX_GET_MOMENTS_ON_ROI(m2, m3, m4, center, mat->real, mat->columns, mat->rows, roi);
}

int imatrix_get_moments_on_roi(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat, bitmap_t *roi)
{
  MATRIX_GET_MOMENTS_ON_ROI(m2, m3, m4, center, mat->imaginary, mat->columns, mat->rows, roi);
}

int cmatrix_get_moments_on_roi(complex_t *m2, complex_t *m3, complex_t *m4, complex_t center, matrix_t *mat, bitmap_t *roi)
{
  int n, i, j;
  complex_t m1, tmp;
  unsigned char *roibuf;
  real_t *rbuf, *ibuf;
  assert(m2);
  assert(m3);
  assert(m4);
  assert(mat);
  assert(mat->imaginary);
  n = 0;
  m2->real = m2->imag = m3->real = m3->imag = m4->real = m4->imag = 0.0;
  // (a+i*b)*(c+i*d) = (a*c-b*d)+i*(a*d+b*c)
  rbuf = mat->real, ibuf = mat->imaginary;
  roibuf = roi->buffer;
  for (i = 0; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
	m1.real = *(rbuf+j)-center.real;
	m1.imag = *(ibuf+j)-center.imag;
	tmp.real = m1.real*m1.real-m1.imag*m1.imag;
	tmp.imag = 2.0*m1.real*m1.imag; //m1.real*m1.imag+m1.imag*m1.real;
	m2->real += tmp.real, m2->imag += tmp.imag;
	tmp.real = tmp.real*m1.real-tmp.imag*m1.imag;
	tmp.imag = tmp.real*m1.imag+tmp.imag*m1.real;
	m3->real += tmp.real, m3->imag += tmp.imag;
	tmp.real = tmp.real*m1.real-tmp.imag*m1.imag;
	tmp.imag = tmp.real*m1.imag+tmp.imag*m1.real;
	m4->real += tmp.real, m4->imag += tmp.imag;
	n++;
      }
    }
    rbuf += mat->columns;
    ibuf += mat->columns;
    roibuf += roi->header.pitch;
  }
  if (n > 0) {
    m2->real /= (real_t)n, m2->imag /= (real_t)n;
    m3->real /= (real_t)n, m3->imag /= (real_t)n;
    m4->real /= (real_t)n, m4->imag /= (real_t)n;
  }
  return n;
}

int matrix_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat, int c, int r, int dc, int dr)
{
  MATRIX_GET_MOMENTS_ON_REGION(m2, m3, m4, center, mat->real, mat->columns, mat->rows, c, r, dc, dr);
}

int imatrix_get_moments_on_region(real_t *m2, real_t *m3, real_t *m4, real_t center, matrix_t *mat, int c, int r, int dc, int dr)
{
  MATRIX_GET_MOMENTS_ON_REGION(m2, m3, m4, center, mat->imaginary, mat->columns, mat->rows, c, r, dc, dr);
}

int cmatrix_get_moments_on_region(complex_t *m2, complex_t *m3, complex_t *m4, complex_t center, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n, i, j;
  int nc, nr;
  complex_t m1, tmp;
  real_t *rbuf, *ibuf;
  assert(m2);
  assert(m3);
  assert(m4);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  nc = min(c+dc, mat->columns);
  nr = min(r+dr, mat->rows);
  n = 0;
  m2->real = m2->imag = m3->real = m3->imag = m4->real = m4->imag = 0.0;
  // (a+i*b)*(c+i*d) = (a*c-b*d)+i*(a*d+b*c)
  rbuf = mat->real+r*mat->columns, ibuf = mat->imaginary+r*mat->columns;
  for (i = r; i < nr; i++) {
    for (j = c; j < nc; j++) {
      m1.real = *(rbuf+j)-center.real;
      m1.imag = *(ibuf+j)-center.imag;
      tmp.real = m1.real*m1.real-m1.imag*m1.imag;
      tmp.imag = 2.0*m1.real*m1.imag; //m1.real*m1.imag+m1.imag*m1.real;
      m2->real += tmp.real, m2->imag += tmp.imag;
      tmp.real = tmp.real*m1.real-tmp.imag*m1.imag;
      tmp.imag = tmp.real*m1.imag+tmp.imag*m1.real;
      m3->real += tmp.real, m3->imag += tmp.imag;
      tmp.real = tmp.real*m1.real-tmp.imag*m1.imag;
      tmp.imag = tmp.real*m1.imag+tmp.imag*m1.real;
      m4->real += tmp.real, m4->imag += tmp.imag;
      n++;
    }
    rbuf += mat->columns;
    ibuf += mat->columns;
  }
  if (n > 0) {
    m2->real /= (real_t)n, m2->imag /= (real_t)n;
    m3->real /= (real_t)n, m3->imag /= (real_t)n;
    m4->real /= (real_t)n, m4->imag /= (real_t)n;
  }
  return n;
}

int matrix_get_skewness(real_t *skewness, matrix_t *mat)
{
  int n;
  real_t m, std, m2, m3, m4;
  assert(skewness);
  assert(mat);
  n = matrix_get_mean(&m, mat);
  if (n > 0) {
    matrix_get_moments(&m2, &m3, &m4, m, mat);
    std = sqrt(m2);
    *skewness = m3/(m2*std);
  }
  return n;
}

int imatrix_get_skewness(real_t *skewness, matrix_t *mat)
{
  int n;
  real_t m, std, m2, m3, m4;
  assert(skewness);
  assert(mat);
  assert(mat->imaginary);
  n = imatrix_get_mean(&m, mat);
  if (n > 0) {
    imatrix_get_moments(&m2, &m3, &m4, m, mat);
    std = sqrt(m2);
    *(skewness) = m3/(m2*std);
  }
  return n;
}

int cmatrix_get_skewness(complex_t *skewness, matrix_t *mat)
{
  int n;
  complex_t m, std, m2, m3, m4;
  assert(skewness);
  assert(mat);
  assert(mat->imaginary);
  n = cmatrix_get_mean(&m, mat);
  if (n > 0) {
    cmatrix_get_moments(&m2, &m3, &m4, m, mat);
    std = complex_sqrt(&m2);
    complex_copy_complex_multiply_complex(&m4, &std, &m2);
    complex_copy_complex_divide_complex(skewness, &m3, &m4);
  }
  return n;
}

int matrix_get_skewness_on_roi(real_t *skewness, matrix_t *mat, bitmap_t *roi)
{
  int n;
  real_t m, std, m2, m3, m4;
  assert(skewness);
  assert(mat);
  assert(roi);
  n = matrix_get_mean_on_roi(&m, mat, roi);
  if (n > 0) {
    matrix_get_moments_on_roi(&m2, &m3, &m4, m, mat, roi);
    std = sqrt(m2);
    *skewness = m3/(m2*std);
  }
  return n;
}

int imatrix_get_skewness_on_roi(real_t *skewness, matrix_t *mat, bitmap_t *roi)
{
  int n;
  real_t m, std, m2, m3, m4;
  assert(skewness);
  assert(mat);
  assert(mat->imaginary);
  n = imatrix_get_mean_on_roi(&m, mat, roi);
  if (n > 0) {
    imatrix_get_moments_on_roi(&m2, &m3, &m4, m, mat, roi);
    std = sqrt(m2);
    *skewness = m3/(m2*std);
  }
  return n;
}

int cmatrix_get_skewness_on_roi(complex_t *skewness, matrix_t *mat, bitmap_t *roi)
{
  int n;
  complex_t m, std, m2, m3, m4;
  assert(skewness);
  assert(mat);
  assert(mat->imaginary);
  n = cmatrix_get_mean_on_roi(&m, mat, roi);
  if (n > 0) {
    cmatrix_get_moments_on_roi(&m2, &m3, &m4, m, mat, roi);
    std = complex_sqrt(&m2);
    complex_copy_complex_multiply_complex(&m4, &std, &m2);
    complex_copy_complex_divide_complex(skewness, &m3, &m4);
  }
  return n;
}

int matrix_get_skewness_on_region(real_t *skewness, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  real_t m, std, m2, m3, m4;
  assert(skewness);
  assert(mat);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  n = matrix_get_mean_on_region(&m, mat, c, r, dc, dr);
  if (n > 0) {
    matrix_get_moments_on_region(&m2, &m3, &m4, m, mat, c, r, dc, dr);
    std = sqrt(m2);
    *skewness = m3/(m2*std);
  }
  return n;
}

int imatrix_get_skewness_on_region(real_t *skewness, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  real_t m, std, m2, m3, m4;
  assert(skewness);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  n = imatrix_get_mean_on_region(&m, mat, c, r, dc, dr);
  if (n > 0) {
    imatrix_get_moments_on_region(&m2, &m3, &m4, m, mat, c, r, dc, dr);
    std = sqrt(m2);
    *skewness = m3/(m2*std);
  }
  return n;
}

int cmatrix_get_skewness_on_region(complex_t *skewness, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  complex_t m, std, m2, m3, m4;
  assert(skewness);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  n = cmatrix_get_mean_on_region(&m, mat, c, r, dc, dr);
  if (n > 0) {
    cmatrix_get_moments_on_region(&m2, &m3, &m4, m, mat, c, r, dc, dr);
    std = complex_sqrt(&m2);
    complex_copy_complex_multiply_complex(&m4, &std, &m2);
    complex_copy_complex_divide_complex(skewness, &m3, &m4);
  }
  return n;
}

int matrix_get_kurtosis(real_t *kurtosis, matrix_t *mat)
{
  int n;
  real_t m, m2, m3, m4;
  assert(kurtosis);
  assert(mat);
  n = matrix_get_mean(&m, mat);
  if (n > 0) {
    matrix_get_moments(&m2, &m3, &m4, m, mat);
    *kurtosis = m4/(m2*m2);
  }
  return n;
}

int imatrix_get_kurtosis(real_t *kurtosis, matrix_t *mat)
{
  int n;
  real_t m, m2, m3, m4;
  assert(kurtosis);
  assert(mat);
  assert(mat->imaginary);
  n = imatrix_get_mean(&m, mat);
  if (n > 0) {
    imatrix_get_moments(&m2, &m3, &m4, m, mat);
    *kurtosis = m4/(m2*m2);
  }
  return n;
}

int cmatrix_get_kurtosis(complex_t *kurtosis, matrix_t *mat)
{
  int n;
  complex_t m, m2, m3, m4;
  assert(kurtosis);
  assert(mat);
  assert(mat->imaginary);
  n = cmatrix_get_mean(&m, mat);
  if (n > 0) {
    cmatrix_get_moments(&m2, &m3, &m4, m, mat);
    complex_copy_complex_multiply_complex(&m3, &m2, &m2);
    complex_copy_complex_divide_complex(kurtosis, &m4, &m3);
  }
  return n;
}

int matrix_get_kurtosis_on_roi(real_t *kurtosis, matrix_t *mat, bitmap_t *roi)
{
  int n;
  real_t m, m2, m3, m4;
  assert(kurtosis);
  assert(mat);
  assert(roi);
  n = matrix_get_mean_on_roi(&m, mat, roi);
  if (n > 0) {
    matrix_get_moments_on_roi(&m2, &m3, &m4, m, mat, roi);
    *kurtosis = m4/(m2*m2);
  }
  return n;
}

int imatrix_get_kurtosis_on_roi(real_t *kurtosis, matrix_t *mat, bitmap_t *roi)
{
  int n;
  real_t m, m2, m3, m4;
  assert(kurtosis);
  assert(mat);
  assert(mat->imaginary);
  n = imatrix_get_mean_on_roi(&m, mat, roi);
  if (n > 0) {
    imatrix_get_moments_on_roi(&m2, &m3, &m4, m, mat, roi);
    *kurtosis = m4/(m2*m2);
  }
  return n;
}

int cmatrix_get_kurtosis_on_roi(complex_t *kurtosis, matrix_t *mat, bitmap_t *roi)
{
  int n;
  complex_t m, m2, m3, m4;
  assert(kurtosis);
  assert(mat);
  assert(mat->imaginary);
  n = cmatrix_get_mean_on_roi(&m, mat, roi);
  if (n > 0) {
    cmatrix_get_moments_on_roi(&m2, &m3, &m4, m, mat, roi);
    complex_copy_complex_multiply_complex(&m3, &m2, &m2);
    complex_copy_complex_divide_complex(kurtosis, &m4, &m3);
  }
  return n;
}

int matrix_get_kurtosis_on_region(real_t *kurtosis, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  real_t m, m2, m3, m4;
  assert(kurtosis);
  assert(mat);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  n = matrix_get_mean_on_region(&m, mat, c, r, dc, dr);
  if (n > 0) {
    matrix_get_moments_on_region(&m2, &m3, &m4, m, mat, c, r, dc, dr);
    *kurtosis = m4/(m2*m2);
  }
  return n;
}

int imatrix_get_kurtosis_on_region(real_t *kurtosis, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  real_t m, m2, m3, m4;
  assert(kurtosis);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  n = imatrix_get_mean_on_region(&m, mat, c, r, dc, dr);
  if (n > 0) {
    imatrix_get_moments_on_region(&m2, &m3, &m4, m, mat, c, r, dc, dr);
    *kurtosis = m4/(m2*m2);
  }
  return n;
}

int cmatrix_get_kurtosis_on_region(complex_t *kurtosis, matrix_t *mat, int c, int r, int dc, int dr)
{
  int n;
  complex_t m, m2, m3, m4;
  assert(kurtosis);
  assert(mat);
  assert(mat->imaginary);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  n = cmatrix_get_mean_on_region(&m, mat, c, r, dc, dr);
  if (n > 0) {
    cmatrix_get_moments_on_region(&m2, &m3, &m4, m, mat, c, r, dc, dr);
    complex_copy_complex_multiply_complex(&m3, &m2, &m2);
    complex_copy_complex_divide_complex(kurtosis, &m4, &m3);
  }
  return n;
}

#define MATRIX_GET_COVARIANCE(var, buffer1, columns1, rows1, buffer2, columns2, rows2) { \
    int n, i, j;							\
    real_t *buf1, *buf2;						\
    real_t m1, m2;							\
    assert(var);							\
    assert(buffer1);							\
    assert(buffer2);							\
    assert(columns1 == columns2);					\
    assert(rows1 == rows2);						\
    buf1 = buffer1;							\
    buf2 = buffer2;							\
    n = 0;								\
    m1 = m2 = 0;							\
    for (i = 0; i < rows1; i++) {					\
      for (j = 0; j < columns1; j++) {					\
	m1 += *(buf1+j);						\
	m2 += *(buf2+j);						\
	n++;								\
      }									\
      buf1 += columns1;							\
      buf2 += columns2;							\
    }									\
    m1 /= (real_t)n;							\
    m2 /= (real_t)n;							\
    buf1 = buffer1;							\
    buf2 = buffer2;							\
    *(var) = 0.0;							\
    for (i = 0; i < rows1; i++) {					\
      for (j = 0; j < columns1; j++) {					\
	*(var) += (*(buf1+j)-m1)*(*(buf2+j)-m2);			\
      }									\
      buf1 += columns1;							\
      buf2 += columns2;							\
    }									\
    if (n != 0) *(var) /= (real_t)n;					\
    return n;								\
  }

#define MATRIX_GET_COVARIANCE_ON_ROI(var, buffer1, columns1, rows1, buffer2, columns2, rows2, roi) { \
    int n, i, j;							\
    real_t *buf1, *buf2;						\
    real_t m1, m2;							\
    uint8_t *roibuf;							\
    assert(var);							\
    assert(buffer1);							\
    assert(buffer2);							\
    assert(columns1 == columns2);					\
    assert(rows1 == rows2);						\
    assert(roi);							\
    assert(columns1 == (roi)->header.width);				\
    assert(rows1 == (roi)->header.height);				\
    buf1 = buffer1;							\
    buf2 = buffer2;							\
    m1 = m2 = 0.0;							\
    n = 0;								\
    roibuf = (roi)->buffer;						\
    for (i = 0; i < rows1; i++) {					\
      for (j = 0; j < columns1; j++) {					\
	if ((*(roibuf+(j>>3)))&(1<<(j%8))) {				\
	  m1 += *(buf1+j);						\
	  m2 += *(buf2+j);						\
	  n++;								\
	}								\
      }									\
      buf1 += columns1;							\
      buf2 += columns2;							\
      roibuf += (roi)->header.pitch;					\
    }									\
    m1 /= (real_t)n;							\
    m2 /= (real_t)n;							\
    buf1 = buffer1;							\
    buf2 = buffer2;							\
    roibuf = (roi)->buffer;						\
    *(var) = 0.0, n = 0;						\
    for (i = 0; i < rows1; i++) {					\
      for (j = 0; j < columns1; j++) {					\
	if ((*(roibuf+(j>>3)))&(1<<(j%8))) {				\
	  *(var) += (*(buf1+j)-m1)*(*(buf2+j)-m2);			\
	}								\
      }									\
      buf1 += columns1;							\
      buf2 += columns2;							\
      roibuf += (roi)->header.pitch;					\
    }									\
    if (n != 0) *(var) /= (real_t)n;					\
    return n;								\
  }

#define MATRIX_GET_COVARIANCE_ON_REGION(var, buffer1, columns1, rows1, buffer2, columns2, rows2, c, r, dc, dr) { \
    int n, nc, nr, i, j;						\
    real_t *buf1, *buf2;						\
    real_t m1, m2;							\
    assert(var);							\
    assert(buffer1);							\
    assert(buffer2);							\
    assert(columns1 == columns2);					\
    assert(rows1 == rows2);						\
    assert(c >= 0 && c < columns1);					\
    assert(r >= 0 && r < rows1);					\
    assert(dc > 0);							\
    assert(dr > 0);							\
    nc = min(c+dc, columns1);						\
    nr = min(r+dr, rows1);						\
    buf1 = buffer1+(r)*(columns1);					\
    buf2 = buffer2+(r)*(columns2);					\
    m1 = m2 = 0;							\
    n = 0;								\
    for (i = r; i < nr; i++) {						\
      for (j = c; j < nc; j++) {					\
	m1 += *(buf1+j);						\
	m2 += *(buf2+j);						\
	n++;								\
      }									\
      buf1 += columns1;							\
      buf2 += columns2;							\
    }									\
    m1 /= (real_t)n;							\
    m2 /= (real_t)n;							\
    buf1 = buffer1+(r)*(columns1);					\
    buf2 = buffer2+(r)*(columns2);					\
    m1 = m2 = 0;							\
    for (i = r; i < nr; i++) {						\
      for (j = c; j < nc; j++) {					\
	*(var) += (*(buf1+j)-m1)*(*(buf2+j)-m2);			\
      }									\
      buf1 += columns1;							\
      buf2 += columns2;							\
    }									\
    if (n != 0) *(var) /= (real_t)n;					\
    return n;								\
  }

int matrix_get_covariance(real_t *v, matrix_t *p, matrix_t *q)
{
  MATRIX_GET_COVARIANCE(v, p->real, p->columns, p->rows, q->real, q->columns, q->rows);
}

int imatrix_get_covariance(real_t *v, matrix_t *p, matrix_t *q)
{
  MATRIX_GET_COVARIANCE(v, p->imaginary, p->columns, p->rows, q->real, q->columns, q->rows);
}

int matrix_get_covariance_on_roi(real_t *v, matrix_t *p, matrix_t *q, bitmap_t *roi)
{
  MATRIX_GET_COVARIANCE_ON_ROI(v, p->real, p->columns, p->rows, q->real, q->columns, q->rows, roi);
}

int imatrix_get_covariance_on_roi(real_t *v, matrix_t *p, matrix_t *q, bitmap_t *roi)
{
  MATRIX_GET_COVARIANCE_ON_ROI(v, p->imaginary, p->columns, p->rows, q->real, q->columns, q->rows, roi);
}

int matrix_get_covariance_on_region(real_t *v, matrix_t *p, matrix_t *q, int c, int r, int dc, int dr)
{
  MATRIX_GET_COVARIANCE_ON_REGION(v, p->real, p->columns, p->rows, q->real, q->columns, q->rows, c, r, dc, dr);
}

int imatrix_get_covariance_on_region(real_t *v, matrix_t *p, matrix_t *q, int c, int r, int dc, int dr)
{
  MATRIX_GET_COVARIANCE_ON_REGION(v, p->imaginary, p->columns, p->rows, q->real, q->columns, q->rows, c, r, dc, dr);
}

// reference from http://mathworld.wolfram.com/CorrelationCoefficient.html
int matrix_get_correlation_coefficient(real_t *r, matrix_t *p, matrix_t *q)
{
  int n;
  real_t ssxx, ssyy, ssxy;
  real_t m1, m2;
  matrix_t *mat;
  assert(p);
  assert(q);
  assert(p->columns == q->columns);
  assert(p->rows == q->rows);
  mat = matrix_new(p->columns, p->rows, false);
  n = (p->columns)*(p->rows);
  // ssxx
  matrix_get_mean(&m1, p);
  matrix_copy_matrix_piecewise_square(mat, p);
  matrix_get_sum(&ssxx, mat);
  ssxx = ssxx-n*m1*m1;
  // ssyy
  matrix_get_mean(&m2, q);
  matrix_copy_matrix_piecewise_square(mat, q);
  matrix_piecewise_square(mat);
  matrix_get_sum(&ssyy, mat);
  ssyy = ssyy-n*m2*m2;
  // ssxy;
  matrix_copy_matrix_piecewise_multiply_matrix(mat, p, q);
  matrix_get_sum(&ssxy, mat);
  ssxy = ssxy-n*m1*m2;
  *r = sqrt(ssxy*ssxy/(ssxx*ssyy));
  matrix_destroy(mat);
  return n;
}

int matrix_get_min(real_t *vmin, matrix_t *mat)
{
  int i, j, n;
  real_t *mbuf;
  assert(vmin);
  assert(mat);
  *(vmin) = *(mat->real);
  n = 0;
  mbuf = mat->real;
  for (i = 0; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      if (*(mbuf+j) < *vmin) *(vmin) = *(mbuf+j);
      n++;
    }
    mbuf += mat->columns;
  }
  return n;
}

int matrix_get_min_on_roi(real_t *vmin, matrix_t *mat, bitmap_t *roi)
{
  int i, j, n;
  uint8_t *roibuf;
  real_t *mbuf;
  assert(vmin);
  assert(mat);
  assert(roi);
  assert(mat->columns == roi->header.width);
  assert(mat->rows == roi->header.height);
  n = 0;
  mbuf = mat->real;
  roibuf = roi->buffer;
  for (i = 0; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
	*(vmin) = *(mbuf+j);
	n++;
	j++;
	goto __next__;
      }
    }
    mbuf += mat->columns;
    roibuf += roi->header.pitch;
  }
 __next__:;
  for (; j < mat->columns; j++) {
    if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
      if (*(mbuf+j) < *(vmin)) *(vmin) = *(mbuf+j);
      n++;
    }
  }
  mbuf += mat->columns;
  roibuf += roi->header.pitch;
  for (; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
	if (*(mbuf+j) < *(vmin)) *(vmin) = *(mbuf+j);
	n++;
      }
    }
    mbuf += mat->columns;
    roibuf += roi->header.pitch;
  }
  return n;
}

int matrix_get_min_on_region(real_t *vmin, matrix_t *mat, int c, int r, int dc, int dr)
{
  int i, j, n;
  int nc, nr;
  real_t *mbuf;
  assert(vmin);
  assert(mat);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  nc = min(c+dc, mat->columns);
  nr = min(r+dr, mat->rows);
  mbuf = mat->real+r*mat->columns;
  *(vmin) = *(mbuf+c);
  n = 0;
  for (i = r; i < nr; i++) {
    for (j = c; j < nc; j++) {
      if (*(mbuf+j) < *vmin) *(vmin) = *(mbuf+j);
      n++;
    }
    mbuf += mat->columns;
  }
  return n;
}

int matrix_get_max(real_t *vmax, matrix_t *mat)
{
  int i, j, n;
  real_t *mbuf;
  assert(vmax);
  assert(mat);
  *(vmax) = *(mat->real);
  n = 0;
  mbuf = mat->real;
  for (i = 0; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      if (*(mbuf+j) > *vmax) *(vmax) = *(mbuf+j);
      n++;
    }
    mbuf += mat->columns;
  }
  return n;
}

int matrix_get_max_on_roi(real_t *vmax, matrix_t *mat, bitmap_t *roi)
{
  int i, j, n;
  uint8_t *roibuf;
  real_t *mbuf;
  assert(vmax);
  assert(mat);
  assert(roi);
  assert(mat->columns == roi->header.width);
  assert(mat->rows == roi->header.height);
  n = 0;
  mbuf = mat->real;
  roibuf = roi->buffer;
  for (i = 0; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
	*(vmax) = *(mbuf+j);
	n++;
	j++;
	goto __next__;
      }
    }
    mbuf += mat->columns;
    roibuf += roi->header.pitch;
  }
 __next__:;
  for (; j < mat->columns; j++) {
    if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
      if (*(mbuf+j) > *(vmax)) *(vmax) = *(mbuf+j);
      n++;
    }
  }
  mbuf += mat->columns;
  roibuf += roi->header.pitch;
  for (; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
	if (*(mbuf+j) > *(vmax)) *(vmax) = *(mbuf+j);
	n++;
      }
    }
    mbuf += mat->columns;
    roibuf += roi->header.pitch;
  }
  return n;
}

int matrix_get_max_on_region(real_t *vmax, matrix_t *mat, int c, int r, int dc, int dr)
{
  int i, j, n;
  int nc, nr;
  real_t *mbuf;
  assert(vmax);
  assert(mat);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  nc = min(c+dc, mat->columns);
  nr = min(r+dr, mat->rows);
  mbuf = mat->real+r*mat->columns;
  *(vmax) = *(mbuf+c);
  n = 0;
  for (i = r; i < nr; i++) {
    for (j = c; j < nc; j++) {
      if (*(mbuf+j) > *vmax) *(vmax) = *(mbuf+j);
      n++;
    }
    mbuf += mat->columns;
  }
  return n;
}

int matrix_get_min_mean_max(real_t *vmin, real_t *vmean, real_t *vmax, matrix_t *mat)
{
  int i, j, n;
  real_t *mbuf, v;
  assert(vmin);
  assert(vmean);
  assert(vmax);
  assert(mat);
  *(vmin) = *(mat->real);
  *vmean = 0;
  *(vmax) = *(mat->real);
  n = 0;
  mbuf = mat->real;
  for (i = 0; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      v = *(mbuf+j);
      if (v < *vmin) *vmin = v;
      if (v > *vmax) *vmax = v;
      *vmean += v;
      n++;
    }
    mbuf += mat->columns;
  }
  if (n > 0) *vmean /= (real_t)n;
  return n;
}

int matrix_get_min_mean_max_on_roi(real_t *vmin, real_t *vmean, real_t *vmax, matrix_t *mat, bitmap_t *roi)
{
  int i, j, n;
  uint8_t *roibuf;
  real_t *mbuf, v;
  assert(vmin);
  assert(vmean);
  assert(vmax);
  assert(mat);
  assert(roi);
  assert(mat->columns == roi->header.width);
  assert(mat->rows == roi->header.height);
  n = 0;
  *vmean = 0;
  mbuf = mat->real;
  roibuf = roi->buffer;
  //printf("1");
  for (i = 0; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
	v = *(mbuf+j);
	*vmin = v;
	*vmean += v;
	*vmax = v;
	n++;
	j++;
	goto __next__;
      }
    }
    mbuf += mat->columns;
    roibuf += roi->header.pitch;
  }
  return n;
 __next__:;
  //printf("1");
  for (; j < mat->columns; j++) {
    if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
      v = *(mbuf+j);
      if (v < *vmin) *vmin = v;
      *vmean += v;
      if (v > *vmax) *vmax = v;
      n++;
    }
  }
  mbuf += mat->columns;
  roibuf += roi->header.pitch;
  i++;
  //printf("1");
  for (; i < mat->rows; i++) {
    for (j = 0; j < mat->columns; j++) {
      if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
	v = *(mbuf+j);
	if (v < *vmin) *vmin = v;
	*vmean += v;
	if (v > *vmax) *vmax = v;
	n++;
      }
    }
    mbuf += mat->columns;
    roibuf += roi->header.pitch;
  }
  //printf("1");
  if (n > 0) *vmean /= (real_t)n;
  return n;
}

int matrix_get_min_mean_max_on_region(real_t *vmin, real_t *vmean, real_t *vmax, matrix_t *mat, int c, int r, int dc, int dr)
{
  int i, j, n;
  int nc, nr;
  real_t *mbuf, v;
  assert(vmin);
  assert(vmean);
  assert(vmax);
  assert(mat);
  assert(c >= 0 && c < mat->columns);
  assert(r >= 0 && r < mat->rows);
  assert(dc > 0);
  assert(dr > 0);
  nc = min(c+dc, mat->columns);
  nr = min(r+dr, mat->rows);
  mbuf = mat->real+r*mat->columns;
  *vmin = *(mbuf+c);
  *vmax = *(mbuf+c);
  n = 0;
  for (i = r; i < nr; i++) {
    for (j = c; j < nc; j++) {
      v = *(mbuf+j);
      if (v < *vmin) *vmin = v;
      *vmean += v;
      if (v > *vmax) *vmax = v;
      n++;
    }
    mbuf += mat->columns;
  }
  if (n > 0) *vmean /= (real_t)n;
  return n;
}
