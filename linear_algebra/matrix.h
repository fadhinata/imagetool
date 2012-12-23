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
#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <common.h>
#include <linear_algebra/complex2.h>
#include <linear_algebra/vector.h>

#undef USE_INVERSE_BY_GAUSS_JORDAN_ELIMINATE

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    int reference;
    int columns, rows;
    real_t *real;
    real_t *imaginary;
  } matrix_t;

#define matrix_inc_ref(m) ((m)->reference++)
#define matrix_dec_ref(m) ((m)->reference--)
#define matrix_get_ref(m) ((m)->reference)

#define matrix_is_square(m) ((m)->columns == (m)->rows)
#define matrix_are_matched(m, n) (((m)->columns == (n)->columns) && ((m)->rows == (n)->rows))

#define matrix_is_imaginary(m) ((m)->imaginary != NULL)
#define matrix_get_columns(m) ((m)->columns)
#define matrix_get_rows(m) ((m)->rows)
#define matrix_get_buffer(m) ((m)->real)
#define imatrix_get_buffer(m) ((m)->imaginary)

#define matrix_get_value(m, c, r) *((m)->real + (r) * (m)->columns + (c))
#define imatrix_get_value(m, c, r) *((m)->imaginary + (r) * (m)->columns + (c))
#define cmatrix_read_value(v, m, c, r) ((v)->real = *((m)->real + (r) * (m)->columns + (c)), (v)->imag = (m)->imaginary ? *((m)->imaginary + (r) * (m)->columns + (c)) : 0)

#define matrix_put_value(v, m, c, r) (*((m)->real + (r) * (m)->columns + (c)) = v)
#define imatrix_put_value(v, m, c, r) (*((m)->imaginary + (r) * (m)->columns + (c)) = v)
#define cmatrix_put_value(v, m, c, r) (*((m)->real + (r) * (m)->columns + (c)) = (v).real, *((m)->imaginary + (r) * (m)->columns + (c)) = (v).imag)

#define matrix_add_value(v, m, c, r) (*((m)->real + (r) * (m)->columns + (c)) += v)
#define imatrix_add_value(v, m, c, r) (*((m)->imaginary + (r) * (m)->columns + (c)) += v)
#define cmatrix_add_value(v, m, c, r) (*((m)->real + (r) * (m)->columns + (c)) += (v).real, *((m)->imaginary + (r) * (m)->columns + (c)) += (v).imag)

#define matrix_sub_value(v, m, c, r) (*((m)->real + (r) * (m)->columns + (c)) -= v)
#define imatrix_sub_value(v, m, c, r) (*((m)->imaginary + (r) * (m)->columns + (c)) -= v)
#define cmatrix_sub_value(v, m, c, r) (*((m)->real + (r) * (m)->columns + (c)) -= (v).real, *((m)->imaginary + (r) * (m)->columns + (c)) -= (v).imag)

#define matrix_div_value(v, m, c, r) (*((m)->real + (r) * (m)->columns + (c)) /= v)
#define imatrix_div_value(v, m, c, r) (*((m)->imaginary + (r) * (m)->columns + (c)) /= v)

#define matrix_mul_value(v, m, c, r) (*((m)->real + (r) * (m)->columns + (c)) *= v)
#define imatrix_mul_value(v, m, c, r) (*((m)->imaginary + (r) * (m)->columns + (c)) *= v)

  // matrix allocation and de-allocation
  matrix_t *matrix_new(int columns, int rows, bool complex_or_not);
  matrix_t *matrix_new_and_copy(matrix_t *m);

#define matrix_attach_imaginary(m) {					\
    if ((m)->imaginary == NULL) {					\
      (m)->imaginary = (real_t *)malloc((m)->columns * (m)->rows * sizeof(real_t)); \
      assert((m)->imaginary);						\
      memset((m)->imaginary, 0, (m)->columns * (m)->rows * sizeof(real_t)); \
    }									\
  }

  void matrix_destroy(matrix_t *m);
  void matrix_dump(matrix_t *m);

  // matrix is zero ?
  bool matrix_is_zero(matrix_t *m, int c, int r, int dc, int dr);
  bool imatrix_is_zero(matrix_t *m, int c, int r, int dc, int dr);
  bool cmatrix_is_zero(matrix_t *m, int c, int r, int dc, int dr);

  bool matrix_is_symmetric(matrix_t *m);
  bool imatrix_is_symmetric(matrix_t *m);
  bool cmatrix_is_symmetric(matrix_t *m);

  bool matrix_is_skew_symmetric(matrix_t *m);
  bool imatrix_is_skew_symmetric(matrix_t *m);
  bool cmatrix_is_skew_symmetric(matrix_t *m);

  bool matrix_is_diagonal(matrix_t *m);
  bool imatrix_is_diagonal(matrix_t *m);
  bool cmatrix_is_diagonal(matrix_t *m);

  bool matrix_is_identity(matrix_t *m);
  bool imatrix_is_identity(matrix_t *m);
  bool cmatrix_is_identity(matrix_t *m);

  bool matrix_is_orthogonal(matrix_t *m);
  bool imatrix_is_orthogonal(matrix_t *m);
  bool cmatrix_is_orthogonal(matrix_t *m);

  // matrix clear
  void matrix_bezero(matrix_t *m, int c, int r, int dc, int dr);
  void imatrix_bezero(matrix_t *m, int c, int r, int dc, int dr);
  void cmatrix_bezero(matrix_t *m, int c, int r, int dc, int dr);
#define matrix_clear(m) matrix_bezero(m, 0, 0, m->columns, m->rows)
#define imatrix_clear(m) imatrix_bezero(m, 0, 0, m->columns, m->rows)
#define cmatrix_clear(m) cmatrix_bezero(m, 0, 0, m->columns, m->rows)

  // matrix set
  void matrix_fill(matrix_t *m, int c, int r, int dc, int dr, real_t value);
  void imatrix_fill(matrix_t *m, int c, int r, int dc, int dr, real_t value);
  void cmatrix_fill(matrix_t *m, int c, int r, int dc, int dr, complex_t value);
  void matrix_fill_randomly(matrix_t *m);
  void imatrix_fill_randomly(matrix_t *m);
  void cmatrix_fill_randomly(matrix_t *m);

  // trimming matrix
  void matrix_trim(matrix_t *m, real_t bottom, real_t top);
  void imatrix_trim(matrix_t *m, real_t bottom, real_t top);

  // identifing matrix
  int matrix_identify(matrix_t *m);
  int imatrix_identify(matrix_t *m);
  int cmatrix_identify(matrix_t *m);

  // compare between matrix and matrix
  int matrix_compare_matrix(matrix_t *q, matrix_t *p);
  int matrix_compare_imatrix(matrix_t *q, matrix_t *p);
  int imatrix_compare_matrix(matrix_t *q, matrix_t *p);
  int imatrix_compare_imatrix(matrix_t *q, matrix_t *p);
  int cmatrix_compare_cmatrix(matrix_t *q, matrix_t *p);
#define matrix_compare(q, p) matrix_compare_matrix(q, p)
#define matrix_cmp(q, p) matrix_compare_matrix(q, p)
#define imatrix_cmp(q, p) imatrix_compare_imatrix(q, p)
#define imatrix_compare(q, p) imatrix_compare_imatrix(q, p)
#define cmatrix_compare(q, p) cmatrix_compare_cmatrix(q, p)
#define cmatrix_cmp(q, p) cmatrix_compare_cmatrix(q, p)

  // exchange row
  void matrix_exchange_row(int i, int j, matrix_t *m);
  void imatrix_exchange_row(int i, int j, matrix_t *m);
  void cmatrix_exchange_row(int i, int j, matrix_t *m);

  // exchange column
  void matrix_exchange_column(int i, int j, matrix_t *m);
  void imatrix_exchange_column(int i, int j, matrix_t *m);
  void cmatrix_exchange_column(int i, int j, matrix_t *m);

  // transpose matrix
  matrix_t *matrix_copy_matrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *matrix_copy_imatrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *imatrix_copy_matrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *imatrix_copy_imatrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *cmatrix_copy_cmatrix_transpose(matrix_t *q, matrix_t *p);

  matrix_t *matrix_add_matrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *matrix_add_imatrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *imatrix_add_matrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *imatrix_add_imatrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *cmatrix_add_cmatrix_transpose(matrix_t *q, matrix_t *p);

  matrix_t *matrix_subtract_matrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *matrix_subtract_imatrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *imatrix_subtract_matrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *imatrix_subtract_imatrix_transpose(matrix_t *q, matrix_t *p);
  matrix_t *cmatrix_subtract_cmatrix_transpose(matrix_t *q, matrix_t *p);

  matrix_t *matrix_new_and_copy_matrix_transpose(matrix_t *p);
  matrix_t *cmatrix_new_and_copy_cmatrix_transpose(matrix_t *p);

  matrix_t *matrix_transpose(matrix_t *p);
  matrix_t *imatrix_transpose(matrix_t *p);
  matrix_t *cmatrix_transpose(matrix_t *p);

  // matrix copy
  matrix_t *matrix_copy(matrix_t *q, matrix_t *p);
  matrix_t *imatrix_copy(matrix_t *q, matrix_t *p);
  matrix_t *cmatrix_copy(matrix_t *q, matrix_t *p);

  matrix_t *matrix_copy_matrix(matrix_t *q, int qc, int qr, matrix_t *p, int pc, int pr, int pdc, int pdr);
  matrix_t *matrix_copy_imatrix(matrix_t *q, int qc, int qr, matrix_t *p, int pc, int pr, int pdc, int pdr);
  matrix_t *imatrix_copy_matrix(matrix_t *q, int qc, int qr, matrix_t *p, int pc, int pr, int pdc, int pdr);
  matrix_t *imatrix_copy_imatrix(matrix_t *q, int qc, int qr, matrix_t *p, int pc, int pr, int pdc, int pdr);
  matrix_t *cmatrix_copy_cmatrix(matrix_t *q, int qc, int qr, matrix_t *p, int pc, int pr, int pdc, int pdr);

  // matrix copy column vector
  matrix_t *matrix_copy_column_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *matrix_copy_column_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_copy_column_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_copy_column_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *cmatrix_copy_column_cvector(matrix_t *q, int qc, int qr, vector_t *p);

  matrix_t *matrix_add_column_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *matrix_add_column_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_add_column_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_add_column_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *cmatrix_add_column_cvector(matrix_t *q, int qc, int qr, vector_t *p);

  matrix_t *matrix_subtract_column_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *matrix_subtract_column_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_subtract_column_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_subtract_column_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *cmatrix_subtract_column_cvector(matrix_t *q, int qc, int qr, vector_t *p);

  // matrix copy row vector
  matrix_t *matrix_copy_row_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *matrix_copy_row_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_copy_row_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_copy_row_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *cmatrix_copy_row_cvector(matrix_t *q, int qc, int qr, vector_t *p);

  matrix_t *matrix_add_row_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *matrix_add_row_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_add_row_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_add_row_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *cmatrix_add_row_cvector(matrix_t *q, int qc, int qr, vector_t *p);

  matrix_t *matrix_subtract_row_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *matrix_subtract_row_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_subtract_row_vector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *imatrix_subtract_row_ivector(matrix_t *q, int qc, int qr, vector_t *p);
  matrix_t *cmatrix_subtract_row_cvector(matrix_t *q, int qc, int qr, vector_t *p);

  // vector copy column vector of matrix
  vector_t *vector_copy_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *vector_copy_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_copy_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_copy_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *cvector_copy_column_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r);

  vector_t *vector_add_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *vector_add_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_add_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_add_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *cvector_add_column_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r);

  vector_t *vector_subtract_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *vector_subtract_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_subtract_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_subtract_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *cvector_subtract_column_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r);

  // vector copy row vector of matrix
  vector_t *vector_copy_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *vector_copy_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_copy_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_copy_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *cvector_copy_row_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r);

  vector_t *vector_add_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *vector_add_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_add_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_add_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *cvector_add_row_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r);

  vector_t *vector_subtract_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *vector_subtract_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_subtract_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *ivector_subtract_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r);
  vector_t *cvector_subtract_row_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r);

  // matrix minor
  matrix_t *matrix_copy_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *matrix_copy_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *imatrix_copy_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *imatrix_copy_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *cmatrix_copy_cmatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);

  matrix_t *matrix_add_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *matrix_add_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *imatrix_add_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *imatrix_add_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *cmatrix_add_cmatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);

  matrix_t *matrix_subtract_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *matrix_subtract_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *imatrix_subtract_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *imatrix_subtract_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);
  matrix_t *cmatrix_subtract_cmatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r);

  matrix_t *matrix_new_and_copy_matrix_minor(matrix_t *p, int del_c, int del_r);
  matrix_t *cmatrix_new_and_copy_cmatrix_minor(matrix_t *p, int del_c, int del_r);

  // addition between matrix and scalar
  matrix_t *matrix_copy_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_copy_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_copy_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_copy_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_copy_cmatrix_add_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_add_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_add_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_add_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_add_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_add_cmatrix_add_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_subtract_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_subtract_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_subtract_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_subtract_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_subtract_cmatrix_add_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_add_scalar(matrix_t *q, real_t v);
  matrix_t *imatrix_add_scalar(matrix_t *q, real_t v);
  matrix_t *cmatrix_add_scalar(matrix_t *q, complex_t v);

  // subtraction between matrix and scalar
  matrix_t *matrix_copy_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_copy_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_copy_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_copy_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_copy_cmatrix_subtract_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_add_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_add_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_add_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_add_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_add_cmatrix_subtract_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_subtract_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_subtract_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_subtract_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_subtract_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_subtract_cmatrix_subtract_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_subtract_scalar(matrix_t *q, real_t v);
  matrix_t *imatrix_subtract_scalar(matrix_t *q, real_t v);
  matrix_t *cmatrix_subtract_scalar(matrix_t *q, complex_t v);

  // multiplication between matrix and scalar
  matrix_t *matrix_copy_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_copy_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_copy_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_copy_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_copy_cmatrix_multiply_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_add_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_add_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_add_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_add_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_add_cmatrix_multiply_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_subtract_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_subtract_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_subtract_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_subtract_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_subtract_cmatrix_multiply_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_multiply_scalar(matrix_t *q, real_t v);
  matrix_t *imatrix_multiply_scalar(matrix_t *q, real_t v);
  matrix_t *cmatrix_multiply_scalar(matrix_t *q, complex_t v);

  // division between matrix and scalar
  matrix_t *matrix_copy_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_copy_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_copy_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_copy_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_copy_cmatrix_divide_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_add_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_add_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_add_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_add_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_add_cmatrix_divide_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_subtract_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *matrix_subtract_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_subtract_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *imatrix_subtract_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v);
  matrix_t *cmatrix_subtract_cmatrix_divide_scalar(matrix_t *q, matrix_t *p, complex_t v);

  matrix_t *matrix_divide_scalar(matrix_t *q, real_t v);
  matrix_t *imatrix_divide_scalar(matrix_t *q, real_t v);
  matrix_t *cmatrix_divide_scalar(matrix_t *q, complex_t v);

  // addtion between matrix and scalar on region
  matrix_t *matrix_copy_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_copy_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_copy_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_copy_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_copy_cmatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_add_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_add_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_add_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_add_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_add_cmatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_subtract_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_subtract_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_subtract_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_subtract_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_subtract_cmatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_add_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_add_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_add_scalar_on_region(matrix_t *q, complex_t v, int c, int r, int dc, int dr);

  // subtraction between matrix and scalar on region
  matrix_t *matrix_copy_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_copy_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_copy_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_copy_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_copy_cmatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_add_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_add_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_add_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_add_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_add_cmatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_subtract_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_subtract_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_subtract_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_subtract_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_subtract_cmatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_subtract_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_subtract_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_subtract_scalar_on_region(matrix_t *q, complex_t v, int c, int r, int dc, int dr);

  // multiplication between matrix and scalar on region
  matrix_t *matrix_copy_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_copy_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_copy_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_copy_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_copy_cmatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_add_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_add_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_add_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_add_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_add_cmatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_subtract_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_subtract_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_subtract_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_subtract_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_subtract_cmatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_multiply_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_multiply_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_multiply_scalar_on_region(matrix_t *q, complex_t v, int c, int r, int dc, int dr);

  // division between matrix and scalar on region
  matrix_t *matrix_copy_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_copy_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_copy_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_copy_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_copy_cmatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_add_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_add_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_add_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_add_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_add_cmatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_subtract_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *matrix_subtract_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_subtract_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_subtract_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_subtract_cmatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr);

  matrix_t *matrix_divide_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr);
  matrix_t *imatrix_divide_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr);
  matrix_t *cmatrix_divide_scalar_on_region(matrix_t *q, complex_t v, int c, int r, int dc, int dr);

  // addition between matrix and matrix
  matrix_t *matrix_copy_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_copy_cmatrix_add_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_add_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_add_cmatrix_add_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_subtract_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_subtract_cmatrix_add_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_add_matrix(matrix_t *c, matrix_t *a);
  matrix_t *matrix_add_imatrix(matrix_t *c, matrix_t *a);
  matrix_t *imatrix_add_matrix(matrix_t *c, matrix_t *a);
  matrix_t *imatrix_add_imatrix(matrix_t *c, matrix_t *a);
  matrix_t *cmatrix_add_cmatrix(matrix_t *c, matrix_t *a);

  // subtraction between matrix and matrix
  matrix_t *matrix_copy_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_copy_cmatrix_subtract_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_add_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_add_cmatrix_subtract_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_subtract_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_subtract_cmatrix_subtract_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_subtract_matrix(matrix_t *c, matrix_t *a);
  matrix_t *matrix_subtract_imatrix(matrix_t *c, matrix_t *a);
  matrix_t *imatrix_subtract_matrix(matrix_t *c, matrix_t *a);
  matrix_t *imatrix_subtract_imatrix(matrix_t *c, matrix_t *a);
  matrix_t *cmatrix_subtract_cmatrix(matrix_t *c, matrix_t *a);

  // multiplication between matrix and matrix
  matrix_t *matrix_copy_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_copy_cmatrix_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_add_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_add_cmatrix_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_subtract_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_subtract_cmatrix_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_new_and_copy_matrix_multiply_matrix(matrix_t *a, matrix_t *b);
  matrix_t *matrix_new_and_copy_matrix_multiply_imatrix(matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_new_and_copy_cmatrix_multiply_cmatrix(matrix_t *a, matrix_t *b);

  // piecewise multiplication between matrix and matrix
  matrix_t *matrix_copy_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_copy_cmatrix_piecewise_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_add_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_add_cmatrix_piecewise_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_subtract_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_subtract_cmatrix_piecewise_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a);
  matrix_t *matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a);
  matrix_t *imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a);
  matrix_t *imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a);
  matrix_t *cmatrix_piecewise_multiply_cmatrix(matrix_t *c, matrix_t *a);

#define matrix_copy_matrix_piecewise_square(c, a) matrix_copy_matrix_piecewise_multiply_matrix(c, a, a)
#define matrix_piecewise_square(m) matrix_piecewise_multiply_matrix(m, m)

  // piecewise division between matrix and matrix
  matrix_t *matrix_copy_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_copy_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_copy_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_copy_cmatrix_piecewise_divide_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_add_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_add_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_add_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_add_cmatrix_piecewise_divide_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_subtract_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *matrix_subtract_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *imatrix_subtract_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b);
  matrix_t *cmatrix_subtract_cmatrix_piecewise_divide_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b);

  matrix_t *matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a);
  matrix_t *matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a);
  matrix_t *imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a);
  matrix_t *imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a);
  matrix_t *cmatrix_piecewise_divide_cmatrix(matrix_t *c, matrix_t *a);

  // multiplication between matrix and vector
  vector_t *vector_copy_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *vector_copy_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *vector_copy_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *vector_copy_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_copy_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_copy_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_copy_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_copy_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *cvector_copy_cmatrix_multiply_cvector(vector_t *c, matrix_t *a, vector_t *b);

  vector_t *vector_add_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *vector_add_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *vector_add_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *vector_add_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_add_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_add_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_add_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_add_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *cvector_add_cmatrix_multiply_cvector(vector_t *c, matrix_t *a, vector_t *b);

  vector_t *vector_subtract_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *vector_subtract_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *vector_subtract_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *vector_subtract_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_subtract_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_subtract_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_subtract_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *ivector_subtract_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b);
  vector_t *cvector_subtract_cmatrix_multiply_cvector(vector_t *c, matrix_t *a, vector_t *b);

  vector_t *vector_new_and_copy_matrix_multiply_vector(matrix_t *a, vector_t *b);
  vector_t *cvector_new_and_copy_cmatrix_multiply_cvector(matrix_t *a, vector_t *b);

  // multiplication between vector and matrix
  vector_t *vector_copy_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *vector_copy_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *vector_copy_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *vector_copy_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_copy_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_copy_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_copy_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_copy_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *cvector_copy_cvector_multiply_cmatrix(vector_t *c, vector_t *a, matrix_t *b);

  vector_t *vector_add_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *vector_add_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *vector_add_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *vector_add_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_add_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_add_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_add_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_add_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *cvector_add_cvector_multiply_cmatrix(vector_t *c, vector_t *a, matrix_t *b);

  vector_t *vector_subtract_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *vector_subtract_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *vector_subtract_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *vector_subtract_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_subtract_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_subtract_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_subtract_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *ivector_subtract_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b);
  vector_t *cvector_subtract_cvector_multiply_cmatrix(vector_t *c, vector_t *a, matrix_t *b);

  // multiplication between column vector and row vector
  matrix_t *matrix_copy_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *matrix_copy_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *matrix_copy_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *matrix_copy_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_copy_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_copy_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_copy_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_copy_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *cmatrix_copy_cvector_multiply_cvector(matrix_t *c, vector_t *a, vector_t *b);

  matrix_t *matrix_add_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *matrix_add_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *matrix_add_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *matrix_add_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_add_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_add_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_add_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_add_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *cmatrix_add_cvector_multiply_cvector(matrix_t *c, vector_t *a, vector_t *b);

  matrix_t *matrix_subtract_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *matrix_subtract_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *matrix_subtract_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *matrix_subtract_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_subtract_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_subtract_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_subtract_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *imatrix_subtract_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b);
  matrix_t *cmatrix_subtract_cvector_multiply_cvector(matrix_t *c, vector_t *a, vector_t *b);

  matrix_t *matrix_new_and_copy_vector_multiply_vector(vector_t *a, vector_t *b);
  matrix_t *cmatrix_new_and_copy_cvector_multiply_cvector(vector_t *a, vector_t *b);

  real_t matrix_get_trace(matrix_t *m);
  complex_t cmatrix_get_trace(matrix_t *m);

  // matrix determinant
  //void matrix_determinant(real_t *det, matrix_t *mat);
  real_t matrix_get_determinant(matrix_t *mat);
  //void cmatrix_determinant(complex_t *det, matrix_t *p);
  complex_t cmatrix_get_determinant(matrix_t *p);

  // matrix cofactor
  matrix_t *matrix_copy_matrix_cofactor(matrix_t *q, matrix_t *p);
  matrix_t *cmatrix_copy_cmatrix_cofactor(matrix_t *q, matrix_t *p);
  // matrix adjugate or adjoint
  matrix_t *matrix_copy_matrix_adjugate(matrix_t *q, matrix_t *p);
  matrix_t *cmatrix_copy_cmatrix_adjugate(matrix_t *q, matrix_t *p);
  matrix_t *matrix_copy_matrix_adjoint(matrix_t *q, matrix_t *p);
  matrix_t *cmatrix_copy_cmatrix_adjoint(matrix_t *q, matrix_t *p);
  // matrix inverse
  int matrix_copy_matrix_inverse(matrix_t *q, matrix_t *p);
  int cmatrix_copy_cmatrix_inverse(matrix_t *q, matrix_t *p);
  matrix_t *matrix_new_and_copy_matrix_inverse(matrix_t *p);
  matrix_t *cmatrix_new_and_copy_cmatrix_inverse(matrix_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __MATRIX_H__ */
