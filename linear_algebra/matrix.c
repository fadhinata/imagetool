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
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <float.h>

#include <common.h>
#include <linear_algebra/matrix.h>
#include <linear_algebra/gauss.h>

matrix_t *matrix_new(int columns, int rows, bool complex_or_not)
{
  matrix_t *m;

  assert(columns > 0);
  assert(rows > 0);

  m = (matrix_t *)malloc(sizeof(*m));
  assert(m);

  m->reference = 0;
  m->columns = columns;
  m->rows = rows;
  m->real = (real_t *)malloc(columns * rows * sizeof(*(m->real)));
  assert(m->real);
  memset(m->real, 0, columns * rows * sizeof(real_t));

  if (complex_or_not) {
    m->imaginary = (real_t *)malloc(columns * rows * sizeof(real_t));
    assert(m->imaginary);
    memset(m->imaginary, 0, columns * rows * sizeof(real_t));
  } else {
    m->imaginary = NULL;
  }

  return m;
}

matrix_t *matrix_new_and_copy(matrix_t *m)
{
  matrix_t *n;

  assert(m);

  n = matrix_new(matrix_get_columns(m), matrix_get_rows(m), imatrix_get_buffer(m) != NULL);
  cmatrix_copy(n, m);

  return n;
}

void matrix_destroy(matrix_t *m)
{
  assert(m);
  
  if (matrix_get_ref(m) <= 0) {
    if (matrix_is_imaginary(m))
      free(imatrix_get_buffer(m));
    free(m->real);
    free(m);
  } else {
    matrix_dec_ref(m);
  }
}

void matrix_dump(matrix_t *m)
{
  int i, j;

  assert(m);

  printf("matrix = \n");
  if (matrix_is_imaginary(m)) {
    for (i = 0; i < matrix_get_rows(m); i++) {
      for (j = 0; j < matrix_get_columns(m); j++) {
	printf("%.4f+i%.4lf ", matrix_get_value(m, j, i), imatrix_get_value(m, j, i));
      }
      printf("\n");
    }
  } else {
    for (i = 0; i < matrix_get_rows(m); i++) {
      for (j = 0; j < matrix_get_columns(m); j++) {
	printf("%.4lf ", matrix_get_value(m, j, i));
      }
      printf("\n");
    }
  }
}

#define MATRIX_IS_ZERO(m, columns, rows, c, r, dc, dr) { \
    int i, j, nc, nr;					\
    real_t *buf;					\
    assert(m);						\
    assert(c >= 0 && c < columns);			\
    assert(r >= 0 && r < rows);				\
    assert(dc > 0);					\
    assert(dr > 0);					\
    nc = min(c+dc, columns);				\
    nr = min(r+dr, rows);				\
    buf = m + (r) * (columns);				\
    for (i = r; i < nr; i++) {				\
      for (j = c; j < nc; j++) {			\
	if (!(abs(*(buf+j)) < REAL_EPSILON))		\
	  return false;					\
      }							\
      buf += columns;					\
    }							\
    return true;					\
  }

bool matrix_is_zero(matrix_t *m, int c, int r, int dc, int dr)
{
  MATRIX_IS_ZERO(matrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m), c, r, dc, dr);
}

bool imatrix_is_zero(matrix_t *m, int c, int r, int dc, int dr)
{
  MATRIX_IS_ZERO(imatrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m), c, r, dc, dr);
}

bool cmatrix_is_zero(matrix_t *m, int c, int r, int dc, int dr)
{
  assert(m);

  if (matrix_is_zero(m, c, r, dc, dr) &&
      (!matrix_is_imaginary(m) || imatrix_is_zero(m, c, r, dc, dr)))
    return true;

  return false;
}

bool matrix_is_symmetric(matrix_t *m)
{
  int i, j, n;

  assert(m);
  assert(matrix_is_square(m));

  n = matrix_get_rows(m);
  for (i = 0; i < n - 1; i++) {
    for (j = i + 1; j < n; j++) {
      if (!(matrix_get_value(m, j, i) == matrix_get_value(m, i, j)))
	return false;
    }
  }

  return true;
}

bool imatrix_is_symmetric(matrix_t *m)
{
  int i, j, n;

  assert(m);
  assert(matrix_is_imaginary(m));
  assert(matrix_is_square(m));

  n = matrix_get_rows(m);
  for (i = 0; i < n - 1; i++) {
    for (j = i + 1; j < n; j++) {
      if (!(imatrix_get_value(m, j, i) == imatrix_get_value(m, i, j)))
	return false;
    }
  }

  return true;
}

bool cmatrix_is_symmetric(matrix_t *m)
{
  assert(m);

  if (!matrix_is_symmetric(m))
    return false;

  if (matrix_is_imaginary(m)) {
    if (!imatrix_is_symmetric(m))
      return false;
  }

  return true;
}

bool matrix_is_skew_symmetric(matrix_t *m)
{
  int i, j, n;

  assert(m);
  assert(matrix_is_square(m));

  n = matrix_get_rows(m);
  for (i = 0; i < n - 1; i++) {
    for (j = i + 1; j < n; j++) {
      if (!(matrix_get_value(m, j, i) == -1 * matrix_get_value(m, i, j)))
	return false;
    }
  }

  return true;
}

bool imatrix_is_skew_symmetric(matrix_t *m)
{
  int i, j, n;

  assert(m);
  assert(matrix_is_imaginary(m));
  assert(matrix_is_square(m));

  n = matrix_get_rows(m);
  for (i = 0; i < n - 1; i++) {
    for (j = i + 1; j < n; j++) {
      if (!(imatrix_get_value(m, j, i) == -1 * imatrix_get_value(m, i, j)))
	return false;
    }
  }

  return true;
}

bool cmatrix_is_skew_symmetric(matrix_t *m)
{
  assert(m);

  if (!matrix_is_skew_symmetric(m))
    return false;

  if (matrix_is_imaginary(m)) {
    if (!imatrix_is_skew_symmetric(m))
      return false;
  }
  return true;
}

bool matrix_is_diagonal(matrix_t *m)
{
  int i, j;

  assert(m);
  
  for (i = 0; i < matrix_get_rows(m); i++) {
    for (j = 0; j < matrix_get_columns(m); j++) {
      if (!(i == j) && !(matrix_get_value(m, j, i) == 0))
	return false;
    }
  }

  return true;
}

bool imatrix_is_diagonal(matrix_t *m)
{
  int i, j;

  assert(m);
  
  for (i = 0; i < matrix_get_rows(m); i++) {
    for (j = 0; j < matrix_get_columns(m); j++) {
      if (!(i == j) && !(imatrix_get_value(m, j, i) == 0))
	return false;
    }
  }

  return true;
}

bool cmatrix_is_diagonal(matrix_t *m)
{
  assert(m);

  if (!matrix_is_diagonal(m))
    return false;

  if (matrix_is_imaginary(m)) {
    if (!imatrix_is_diagonal(m))
      return false;
  }

  return true;
}

bool matrix_is_identity(matrix_t *m)
{
  int i, j, n;

  assert(m);
  assert(matrix_is_square(m));

  n = matrix_get_columns(m);

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      if (i == j) {
	if (!(abs(matrix_get_value(m, j, i) - 1.0) < REAL_EPSILON))
	  return false;
      } else {
	if (!(abs(matrix_get_value(m, j, i)) < REAL_EPSILON))
	  return false;
      }
    }
  }
  return true;
}

bool imatrix_is_identity(matrix_t *m)
{
  int i, j, n;

  assert(m);
  assert(matrix_is_imaginary(m));
  assert(matrix_is_square(m));

  n = matrix_get_columns(m);

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      if (i == j) {
	if (!(abs(imatrix_get_value(m, j, i) - 1.0) < REAL_EPSILON))
	  return false;
      } else {
	if (!(abs(imatrix_get_value(m, j, i)) < REAL_EPSILON))
	  return false;
      }
    }
  }
  return true;
}

bool cmatrix_is_identity(matrix_t *m)
{
  int i, j, n;

  assert(m);
  assert(matrix_is_square(m));

  if (!matrix_is_identity(m))
    return false;

  if (matrix_is_imaginary(m) &&
      !imatrix_is_zero(m, 0, 0, matrix_get_columns(m), matrix_get_rows(m)))
    return false;

  return true;
}

bool matrix_is_orthogonal(matrix_t *m)
{
  bool ret;
  matrix_t *p, *q;

  assert(m);
  assert(matrix_is_square(m));

  p = matrix_new_and_copy_matrix_transpose(m);
  q = matrix_new_and_copy_matrix_multiply_matrix(p, m);
  if (matrix_is_identity(q)) ret = true;
  else ret = false;

  matrix_destroy(q);
  matrix_destroy(p);

  return ret;
}

bool imatrix_is_orthogonal(matrix_t *m)
{
  bool ret;
  matrix_t *p, *q;

  assert(m);
  assert(matrix_is_square(m));
  assert(matrix_is_imaginary(m));
  
  p = matrix_new(matrix_get_columns(m), matrix_get_rows(m), false);
  matrix_copy_imatrix_transpose(p, m);

  q = matrix_new_and_copy_matrix_multiply_imatrix(p, m);
  if (matrix_is_identity(q)) ret = true;
  else ret = false;

  matrix_destroy(q);
  matrix_destroy(p);

  return ret;
}

bool cmatrix_is_orthogonal(matrix_t *m)
{
  bool ret;
  matrix_t *p, *q;

  assert(m);
  assert(matrix_is_square(m));

  p = cmatrix_new_and_copy_cmatrix_transpose(m);
  q = cmatrix_new_and_copy_cmatrix_multiply_cmatrix(p, m);
  if (cmatrix_is_identity(q)) ret = true;
  else ret = false;

  matrix_destroy(q);
  matrix_destroy(p);

  return ret;
}

#define MATRIX_BEZERO(m, columns, rows, c, r, dc, dr) { \
    int i, j, nc, nr;					\
    real_t *buf;					\
    assert(m);						\
    assert(c >= 0 && c < columns);			\
    assert(r >= 0 && r < rows);				\
    assert(dc > 0);					\
    assert(dr > 0);					\
    nc = min(c+dc, columns);				\
    nr = min(r+dr, rows);				\
    buf = m+(r)*(columns);				\
    for (i = r; i < nr; i++) {				\
      for (j = c; j < nc; j++) {			\
	*(buf+j) = 0.0;					\
      }							\
      buf += columns;					\
    }							\
  }

void matrix_bezero(matrix_t *m, int c, int r, int dc, int dr)
{
  MATRIX_BEZERO(matrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m), c, r, dc, dr);
}

void imatrix_bezero(matrix_t *m, int c, int r, int dc, int dr)
{
  MATRIX_BEZERO(imatrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m), c, r, dc, dr);
}

void cmatrix_bezero(matrix_t *m, int c, int r, int dc, int dr)
{
  assert(m);

  matrix_bezero(m, c, r, dc, dr);
  if (matrix_is_imaginary(m))
    imatrix_bezero(m, c, r, dc, dr);
}

#define MATRIX_FILL(m, columns, rows, c, r, dc, dr, v) {	\
    int i, j, nc, nr;						\
    real_t *buf;						\
    assert(m);							\
    assert(c >= 0 && c < columns);				\
    assert(r >= 0 && r < rows);					\
    assert(dc > 0);						\
    assert(dr > 0);						\
    nc = min(c+dc, columns);					\
    nr = min(r+dr, rows);					\
    buf = m+(r)*(columns);					\
    for (i = r; i < nr; i++) {					\
      for (j = c; j < nc; j++) {				\
	*(buf+j) = v;						\
      }								\
      buf += columns;						\
    }								\
  }

void matrix_fill(matrix_t *m, int c, int r, int dc, int dr, real_t value)
{
  MATRIX_FILL(matrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m), c, r, dc, dr, value);
}

void imatrix_fill(matrix_t *m, int c, int r, int dc, int dr, real_t value)
{
  MATRIX_FILL(imatrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m), c, r, dc, dr, value);
}

void cmatrix_fill(matrix_t *m, int c, int r, int dc, int dr, complex_t value)
{
  assert(m);

  matrix_fill(m, c, r, dc, dr, value.real);

  if (!matrix_is_imaginary(m))
    matrix_attach_imaginary(m);

  imatrix_fill(m, c, r, dc, dr, value.imag);
}

#define MATRIX_FILL_RANDOMLY(mbuf, mcols, mrows) {	\
    int i, j;						\
    assert(mbuf);					\
    for (i = 0; i < mrows; i++)				\
      for (j = 0; j < mcols; j++)			\
	*(mbuf+i*(mcols)+j) = rand() / 1000.0;		\
  }

void matrix_fill_randomly(matrix_t *m)
{
  MATRIX_FILL_RANDOMLY(matrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m));
}

void imatrix_fill_randomly(matrix_t *m)
{
  MATRIX_FILL_RANDOMLY(imatrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m));
}

void cmatrix_fill_randomly(matrix_t *m)
{
  assert(m);

  matrix_fill_randomly(m);

  if (!matrix_is_imaginary(m))
    matrix_attach_imaginary(m);

  imatrix_fill_randomly(m);
}

#define MATRIX_TRIM(m, columns, rows, bottom, top) {	\
    int i, j;						\
    real_t *buf;					\
    assert(m);						\
    assert(top > bottom);				\
    buf = m;						\
    for (i = 0; i < rows; i++) {			\
      for (j = 0; j < columns; j++) {			\
	if (*(buf+j) < bottom) *(buf+j) = bottom;	\
	else if (*(buf+j) > top) *(buf+j) = top;	\
      }							\
      buf += columns;					\
    }							\
  }

void matrix_trim(matrix_t *m, real_t bottom, real_t top)
{
  MATRIX_TRIM(matrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m), bottom, top);
}

void imatrix_trim(matrix_t *m, real_t bottom, real_t top)
{
  MATRIX_TRIM(imatrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m), bottom, top);
}

#define MATRIX_IDENTIFY(buffer, columns, rows) {		\
    int i;							\
    real_t *buf;						\
    assert(buffer);						\
    assert(columns == rows);					\
    if (columns != rows) return -1;				\
    memset(buffer, 0, (columns) * (rows) * sizeof(real_t));	\
    buf = buffer;						\
    for (i = 0; i < rows; i++) {				\
      *(buf) = 1.0;						\
      buf += columns + 1;					\
    }								\
    return 0;							\
  }

int matrix_identify(matrix_t *m)
{
  MATRIX_IDENTIFY(matrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m));
}

int imatrix_identify(matrix_t *m)
{
  MATRIX_IDENTIFY(imatrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m));
}

int cmatrix_identify(matrix_t *m)
{
  assert(m);
  assert(matrix_is_square(m));

  if (!matrix_is_square(m)) return -1;
  cmatrix_bezero(m, 0, 0, matrix_get_columns(m), matrix_get_rows(m));
  return matrix_identify(m);
}


#define MATRIX_COMPARE(q, qcols, qrows, p, pcols, prows) {		\
    int i, j;								\
    real_t *qbuf, *pbuf;						\
    assert(q);								\
    assert(p);								\
    assert(qcols == pcols);						\
    assert(qrows == prows);						\
    if (qcols != pcols || qrows != prows) return -1;			\
    qbuf = q;								\
    pbuf = p;								\
    for (i = 0; i < prows; i++) {					\
      for (j = 0; j < pcols; j++) {					\
	if (!(abs((*(qbuf+j))-(*(pbuf+j))) < REAL_EPSILON)) return -1;	\
      }									\
      qbuf += pcols;							\
      pbuf += pcols;							\
    }									\
    return 0;								\
  }

int matrix_compare_matrix(matrix_t *q, matrix_t *p)
{
  MATRIX_COMPARE(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q),
		 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
}

int matrix_compare_imatrix(matrix_t *q, matrix_t *p)
{
  MATRIX_COMPARE(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q),
		 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
}

int imatrix_compare_matrix(matrix_t *q, matrix_t *p)
{
  MATRIX_COMPARE(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q),
		 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
}

int imatrix_compare_imatrix(matrix_t *q, matrix_t *p)
{
  MATRIX_COMPARE(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q),
		 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
}

int cmatrix_compare_cmatrix(matrix_t *q, matrix_t *p)
{
  int c, r;
  complex_t v1, v2;

  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  if ((matrix_get_columns(q) != matrix_get_columns(p)) ||
      (matrix_get_rows(q) != matrix_get_rows(p))) return -1;

  for (r = 0; r < matrix_get_rows(p); r++) {
    for (c = 0; c < matrix_get_columns(p); c++) {
      cmatrix_read_value(&v1, q, c, r);
      cmatrix_read_value(&v2, p, c, r);
      if (complex_compare(&v1, &v2) != 0) return -1;
    }
  }

  return 0;
}

void matrix_exchange_row(int i, int j, matrix_t *m)
{
  int k, columns;//, rows;
  real_t *qbuf, *pbuf, tmp;

  assert(m);
  assert(i >= 0 && i < matrix_get_rows(m));
  assert(j >= 0 && j < matrix_get_rows(m));
  assert(i != j);

  if (i == j) return;

  columns = matrix_get_columns(m);
  //rows = matrix_get_rows(m);
  qbuf = matrix_get_buffer(m) + i * columns;
  pbuf = matrix_get_buffer(m) + j * columns;
  for (k = 0; k < columns; k++) {
    tmp = *(qbuf + k);
    *(qbuf + k) = *(pbuf + k);
    *(pbuf + k) = tmp;
  }
}

void imatrix_exchange_row(int i, int j, matrix_t *m)
{
  int k, columns;//, rows;
  real_t *qbuf, *pbuf, tmp;

  assert(m);
  assert(matrix_is_imaginary(m));
  assert(i >= 0 && i < matrix_get_rows(m));
  assert(j >= 0 && j < matrix_get_rows(m));
  assert(i != j);

  if (i == j) return;

  columns = matrix_get_columns(m);
  //rows = matrix_get_rows(m);
  qbuf = imatrix_get_buffer(m) + i * columns;
  pbuf = imatrix_get_buffer(m) + j * columns;
  for (k = 0; k < columns; k++) {
    tmp = *(qbuf + k);
    *(qbuf + k) = *(pbuf + k);
    *(pbuf + k) = tmp;
  }
}

void cmatrix_exchange_row(int i, int j, matrix_t *m)
{
  int k, columns;//, rows;
  real_t *qreal, *qimag, *preal, *pimag, tmp;

  assert(m);
  assert(matrix_is_imaginary(m));
  assert(i >= 0 && i < matrix_get_rows(m));
  assert(j >= 0 && j < matrix_get_rows(m));
  assert(i != j);

  if (i == j) return;

  columns = matrix_get_columns(m);
  //rows = matrix_get_rows(m);

  qreal = matrix_get_buffer(m) + i * columns;
  qimag = imatrix_get_buffer(m) + i * columns;

  preal = matrix_get_buffer(m) + j * columns;
  pimag = imatrix_get_buffer(m) + j * columns;

  for (k = 0; k < columns; k++) {
    tmp = *(qreal + k);
    *(qreal + k) = *(preal + k);
    *(preal + k) = tmp;
    tmp = *(qimag + k);
    *(qimag + k) = *(pimag + k);
    *(pimag + k) = tmp;
  }
}

void matrix_exchange_column(int i, int j, matrix_t *m)
{
  int k, columns, rows;
  real_t *pbuf, *qbuf, tmp;

  assert(m);
  assert(i >= 0 && i < matrix_get_columns(m));
  assert(j >= 0 && j < matrix_get_columns(m));
  assert(i != j);

  if (i == j) return;

  columns = matrix_get_columns(m);
  rows = matrix_get_rows(m);

  pbuf = matrix_get_buffer(m) + i;
  qbuf = matrix_get_buffer(m) + j;
  for (k = 0; k < rows; k++) {
    tmp = *pbuf;
    *pbuf = *qbuf;
    *qbuf = tmp;
    pbuf += columns;
    qbuf += columns;
  }
}

void imatrix_exchange_column(int i, int j, matrix_t *m)
{
  int k, columns, rows;
  real_t *pbuf, *qbuf, tmp;

  assert(m);
  assert(matrix_is_imaginary(m));
  assert(i >= 0 && i < matrix_get_columns(m));
  assert(j >= 0 && j < matrix_get_columns(m));
  assert(i != j);

  if (i == j) return;

  columns = matrix_get_columns(m);
  rows = matrix_get_rows(m);

  pbuf = imatrix_get_buffer(m) + i;
  qbuf = imatrix_get_buffer(m) + j;
  for (k = 0; k < rows; k++) {
    tmp = *pbuf;
    *pbuf = *qbuf;
    *qbuf = tmp;
    pbuf += columns;
    qbuf += columns;
  }
}

void cmatrix_exchange_column(int i, int j, matrix_t *m)
{
  int k, columns, rows;
  real_t *preal, *pimag, *qreal, *qimag, tmp;

  assert(m);
  assert(matrix_is_imaginary(m));
  assert(i >= 0 && i < matrix_get_columns(m));
  assert(j >= 0 && j < matrix_get_columns(m));
  assert(i != j);

  if (i == j) return;

  columns = matrix_get_columns(m);
  rows = matrix_get_rows(m);

  preal = matrix_get_buffer(m) + i;
  pimag = imatrix_get_buffer(m) + i;

  qreal = matrix_get_buffer(m) + j;
  qimag = imatrix_get_buffer(m) + j;

  for (k = 0; k < rows; k++) {
    tmp = *preal;
    *preal = *qreal;
    *qreal = tmp;
    tmp = *pimag;
    *pimag = *qimag;
    *qimag = tmp;
    preal += columns;
    pimag += columns;
    qreal += columns;
    qimag += columns;
  }
}

#define MATRIX_TRANSPOSE(qbuf, qcols, qrows, opcode, pbuf, pcols, prows) { \
    int c, r;								\
    assert(qbuf);							\
    assert(pbuf);							\
    assert(qcols == prows);						\
    assert(qrows == pcols);						\
    for (r = 0; r < prows; r++) {					\
      for (c = 0; c < pcols; c++) {					\
	*(qbuf + c * (qcols) + r) opcode *(pbuf + r * (pcols) + c);	\
      }									\
    }									\
  }

matrix_t *matrix_copy_matrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		   matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *matrix_copy_imatrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		   imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *imatrix_copy_matrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		   matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *imatrix_copy_imatrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		   imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *cmatrix_copy_cmatrix_transpose(matrix_t *q, matrix_t *p)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_rows(p));
  assert(matrix_get_rows(q) == matrix_get_columns(p));

  // for real part
  matrix_copy_matrix_transpose(q, p);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_copy_imatrix_transpose(q, p);
  } else {
    if (matrix_is_imaginary(q))
      imatrix_bezero(q, 0, 0, matrix_get_columns(p), matrix_get_rows(p));
  }

  return q;
}

matrix_t *matrix_add_matrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		   matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *matrix_add_imatrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		   imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *imatrix_add_matrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		   matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *imatrix_add_imatrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		   imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *cmatrix_add_cmatrix_transpose(matrix_t *q, matrix_t *p)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_rows(p));
  assert(matrix_get_rows(q) == matrix_get_columns(p));

  // for real part
  matrix_add_matrix_transpose(q, p);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_add_imatrix_transpose(q, p);
  }

  return q;
}

matrix_t *matrix_subtract_matrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		   matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *matrix_subtract_imatrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		   imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *imatrix_subtract_matrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		   matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *imatrix_subtract_imatrix_transpose(matrix_t *q, matrix_t *p)
{
  MATRIX_TRANSPOSE(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		   imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *cmatrix_subtract_cmatrix_transpose(matrix_t *q, matrix_t *p)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_rows(p));
  assert(matrix_get_rows(q) == matrix_get_columns(p));

  // for real part
  matrix_subtract_matrix_transpose(q, p);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_subtract_imatrix_transpose(q, p);
  }

  return q;
}

matrix_t *matrix_new_and_copy_matrix_transpose(matrix_t *p)
{
  matrix_t *q;

  assert(p);

  q = matrix_new(matrix_get_rows(p), matrix_get_columns(p), false);
  matrix_copy_matrix_transpose(q, p);

  return q;
}

matrix_t *cmatrix_new_and_copy_cmatrix_transpose(matrix_t *p)
{
  matrix_t *q;

  assert(p);

  q = matrix_new(matrix_get_rows(p), matrix_get_columns(p), matrix_is_imaginary(p));

  cmatrix_copy_cmatrix_transpose(q, p);

  return q;
}

#define MATRIX_SELF_TRANSPOSE(qbuf, qcols, qrows) { \
    int c, r;								\
    real_t val;								\
    assert(qbuf);							\
    assert(qcols == qrows);						\
    for (r = 0; r < qrows - 1; r++) {					\
      for (c = r + 1; c < qcols; c++) {					\
	val = *(qbuf + r * (qcols) + c);				\
	*(qbuf + r * (qcols) + c) = *(qbuf + c * (qcols) + r);		\
	*(qbuf + c * (qcols) + r) = val;				\
      }									\
    }									\
  }

matrix_t *matrix_transpose(matrix_t *m)
{
  MATRIX_SELF_TRANSPOSE(matrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m));
  return m;
}

matrix_t *imatrix_transpose(matrix_t *m)
{
  MATRIX_SELF_TRANSPOSE(imatrix_get_buffer(m), matrix_get_columns(m), matrix_get_rows(m));
  return m;
}

matrix_t *cmatrix_transpose(matrix_t *m)
{
  assert(m);
  assert(matrix_is_square(m));

  matrix_transpose(m);
  if (matrix_is_imaginary(m)) imatrix_transpose(m);

  return m;
}

#define MATRIX_COPY(qbuf, qcols, qrows, qc, qr, pbuf, pcols, prows, pc, pr, pdc, pdr) { \
    int i, j;								\
    int columns, rows;							\
    assert(qbuf);							\
    assert(qc >= 0 && qc < qcols);					\
    assert(qr >= 0 && qr < qrows);					\
    assert(pbuf);							\
    assert(pc >= 0 && pc < pcols);					\
    assert(pr >= 0 && pr < prows);					\
    assert(pdc > 0);							\
    assert(pdr > 0);							\
    columns = min(pc+pdc, pcols)-pc;					\
    columns = min(qc+columns, qcols)-qc;				\
    rows = min(pr+pdr, prows)-pr;					\
    rows = min(qr+rows, qrows)-qr;					\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	*(qbuf+(qr+i)*(qcols)+(qc+j)) = *(pbuf+(pr+i)*(pcols)+(pc+j));	\
      }									\
    }									\
  }

matrix_t *matrix_copy(matrix_t *q, matrix_t *p)
{
  MATRIX_COPY(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), 0, 0, 
	      matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p),
	      0, 0, matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *imatrix_copy(matrix_t *q, matrix_t *p)
{
  MATRIX_COPY(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), 0, 0, 
	      imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p),
	      0, 0, matrix_get_columns(p), matrix_get_rows(p));
  return q;
}

matrix_t *cmatrix_copy(matrix_t *q, matrix_t *p)
{
  assert(q);
  assert(p);

  // for real part
  matrix_copy(q, p);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_copy(q, p);
  } else {
    if (matrix_is_imaginary(q))
      imatrix_bezero(q, 0, 0, matrix_get_columns(p), matrix_get_rows(p));
  }

  return q;
}

matrix_t *matrix_copy_matrix(matrix_t *q, int qc, int qr, matrix_t *p, int pc, int pr, int pdc, int pdr)
{
  MATRIX_COPY(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr,
	      matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), pc, pr, pdc, pdr);
  return q;
}

matrix_t *matrix_copy_imatrix(matrix_t *q, int qc, int qr, matrix_t *p, int pc, int pr, int pdc, int pdr)
{
  MATRIX_COPY(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr,
	      imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), pc, pr, pdc, pdr);
  return q;
}

matrix_t *imatrix_copy_matrix(matrix_t *q, int qc, int qr, matrix_t *p, int pc, int pr, int pdc, int pdr)
{
  MATRIX_COPY(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr,
	      matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), pc, pr, pdc, pdr);
  return q;
}

matrix_t *imatrix_copy_imatrix(matrix_t *q, int qc, int qr, matrix_t *p, int pc, int pr, int pdc, int pdr)
{
  MATRIX_COPY(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr,
	      imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), pc, pr, pdc, pdr);
  return q;
}

matrix_t *cmatrix_copy_cmatrix(matrix_t *q, int qc, int qr, matrix_t *p, int pc, int pr, int pdc, int pdr)
{
  assert(q);
  assert(qc >= 0 && qc < matrix_get_columns(q));
  assert(qr >= 0 && qr < matrix_get_rows(q));
  assert(p);
  assert(pc >= 0 && pc < matrix_get_columns(p));
  assert(pr >= 0 && pr < matrix_get_rows(p));
  assert(pdc > 0);
  assert(pdr > 0);

  // for real part
  matrix_copy_matrix(q, qc, qr, p, pc, pr, pdc, pdr);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_copy_imatrix(q, qc, qr, p, pc, pr, pdc, pdr);    
  } else {
    if (matrix_is_imaginary(q))
      imatrix_bezero(q, qc, qr, pdc, pdr);
  }

  return q;
}

#define MATRIX_COPY_COLUMN_VECTOR(qbuf, qcols, qrows, qc, qr, opcode, pbuf, plen) { \
    int i, rows;							\
    assert(qbuf);							\
    assert(qc >= 0 && qc < qcols);					\
    assert(qr >= 0 && qr < qrows);					\
    assert(pbuf);							\
    rows = min(qr + plen, qrows) - qr;					\
    for (i = 0; i < rows; i++)						\
      *(qbuf + (qr + i) * (qcols) + qc) opcode *(pbuf + i);		\
  }

matrix_t *matrix_copy_column_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, =, 
			    vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *matrix_copy_column_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, =,
			    ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_copy_column_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, =,
			    vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_copy_column_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, =,
			    ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *cmatrix_copy_column_cvector(matrix_t *q, int qc, int qr, vector_t *p)
{
  assert(q);
  assert(qc >= 0 && qc < matrix_get_columns(q));
  assert(qr >= 0 && qr < matrix_get_rows(q));
  assert(p);

  // for real part
  matrix_copy_column_vector(q, qc, qr, p);

  // for imaginary part
  if (vector_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_copy_column_ivector(q, qc, qr, p);
  } else {
    if (matrix_is_imaginary(q))
      imatrix_bezero(q, qc, qr, 1, vector_get_dimension(p));
  }

  return q;
}

matrix_t *matrix_add_column_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, +=,
			    vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *matrix_add_column_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, +=,
			    ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_add_column_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, +=,
			    vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_add_column_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, +=,
			    ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *cmatrix_add_column_cvector(matrix_t *q, int qc, int qr, vector_t *p)
{
  assert(q);
  assert(qc >= 0 && qc < matrix_get_columns(q));
  assert(qr >= 0 && qr < matrix_get_rows(q));
  assert(p);

  // for real part
  matrix_add_column_vector(q, qc, qr, p);

  // for imaginary part
  if (vector_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_add_column_ivector(q, qc, qr, p);
  }

  return q;
}

matrix_t *matrix_subtract_column_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, -=,
			    vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *matrix_subtract_column_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, -=,
			    ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_subtract_column_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, -=,
			    vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_subtract_column_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_COLUMN_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, -=,
			    ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *cmatrix_subtract_column_cvector(matrix_t *q, int qc, int qr, vector_t *p)
{
  assert(q);
  assert(qc >= 0 && qc < matrix_get_columns(q));
  assert(qr >= 0 && qr < matrix_get_rows(q));
  assert(p);

  // for real part
  matrix_subtract_column_vector(q, qc, qr, p);

  // for imaginary part
  if (vector_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_subtract_column_ivector(q, qc, qr, p);    
  }

  return q;
}

#define MATRIX_COPY_ROW_VECTOR(qbuf, qcols, qrows, qc, qr, opcode, pbuf, plen) { \
    int j, cols;							\
    assert(qbuf);							\
    assert(qc >= 0 && qc < qcols);					\
    assert(qr >= 0 && qr < qrows);					\
    assert(pbuf);							\
    cols = min(qc + plen, qcols) - qc;					\
    for (j = 0; j < cols; j++)						\
      *(qbuf + (qr) * (qcols) + (qc + j)) opcode *(pbuf + j);		\
  }

matrix_t *matrix_copy_row_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, =,
			 vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *matrix_copy_row_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, =,
			 ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_copy_row_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, =,
			 vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_copy_row_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, =,
			 ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *cmatrix_copy_row_cvector(matrix_t *q, int qc, int qr, vector_t *p)
{
  assert(q);
  assert(qc >= 0 && qc < matrix_get_columns(q));
  assert(qr >= 0 && qr < matrix_get_rows(q));
  assert(p);

  // for real part
  matrix_copy_row_vector(q, qc, qr, p);

  // for imaginary part
  if (vector_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_copy_row_ivector(q, qc, qr, p);
  } else {
    if (matrix_is_imaginary(q))
      imatrix_bezero(q, qc, qr, vector_get_dimension(p), 1);
  }

  return q;
}

matrix_t *matrix_add_row_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, +=,
			 vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *matrix_add_row_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, +=,
			 ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_add_row_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, +=,
			 vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_add_row_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, +=,
			 ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *cmatrix_add_row_cvector(matrix_t *q, int qc, int qr, vector_t *p)
{
  assert(q);
  assert(qc >= 0 && qc < matrix_get_columns(q));
  assert(qr >= 0 && qr < matrix_get_rows(q));
  assert(p);

  // for real part
  matrix_add_row_vector(q, qc, qr, p);

  // for imaginary part
  if (vector_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_add_row_ivector(q, qc, qr, p);
  }

  return q;
}

matrix_t *matrix_subtract_row_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, -=,
			 vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *matrix_subtract_row_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, -=,
			 ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_subtract_row_vector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, -=,
			 vector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *imatrix_subtract_row_ivector(matrix_t *q, int qc, int qr, vector_t *p)
{
  MATRIX_COPY_ROW_VECTOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), qc, qr, -=,
			 ivector_get_buffer(p), vector_get_dimension(p));
  return q;
}

matrix_t *cmatrix_subtract_row_cvector(matrix_t *q, int qc, int qr, vector_t *p)
{
  assert(q);
  assert(qc >= 0 && qc < matrix_get_columns(q));
  assert(qr >= 0 && qr < matrix_get_rows(q));
  assert(p);

  // for real part
  matrix_subtract_row_vector(q, qc, qr, p);

  // for imaginary part
  if (vector_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_subtract_row_ivector(q, qc, qr, p);
  }

  return q;
}

#define COLUMN_VECTOR_OF_MATRIX(qbuf, qlen, opcode, pbuf, pcols, prows, c, r) { \
    int i;								\
    int rows;								\
    assert(qbuf);							\
    assert(pbuf);							\
    assert(c >= 0 && c < pcols);					\
    assert(r >= 0 && r < prows);					\
    rows = min(qlen, prows - r) + r;					\
    for (i = r; i < rows; i++) {					\
      *(qbuf + i) opcode (*(pbuf + i * (pcols) + c));			\
    }									\
  }

vector_t *vector_copy_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), =,
			  matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *vector_copy_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), =,
			  imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_copy_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), =,
			  matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_copy_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), =,
			  imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *cvector_copy_column_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r)
{
  assert(q);
  assert(p);
  assert(c >= 0 && c < matrix_get_columns(p));
  assert(r >= 0 && r < matrix_get_rows(p));

  // for real part
  vector_copy_column_vector_of_matrix(q, p, c, r);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!vector_is_imaginary(q))
      vector_attach_imaginary(q);
    ivector_copy_column_vector_of_imatrix(q, p, c, r);    
  } else {
    if (vector_is_imaginary(q))
      ivector_bezero(q, 0, matrix_get_rows(p) - r);
  }

  return q;
}

vector_t *vector_add_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), +=,
			  matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *vector_add_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), +=,
			  imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_add_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), +=,
			  matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_add_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), +=,
			  imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *cvector_add_column_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r)
{
  assert(q);
  assert(p);
  assert(c >= 0 && c < matrix_get_columns(p));
  assert(r >= 0 && r < matrix_get_rows(p));

  // for real part
  vector_add_column_vector_of_matrix(q, p, c, r);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!vector_is_imaginary(q))
      vector_attach_imaginary(q);
    ivector_add_column_vector_of_imatrix(q, p, c, r);
  }

  return q;
}

vector_t *vector_subtract_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), -=,
			  matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *vector_subtract_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), -=,
			  imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_subtract_column_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), -=,
			  matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_subtract_column_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  COLUMN_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), -=,
			  imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *cvector_subtract_column_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r)
{
  assert(q);
  assert(p);
  assert(c >= 0 && c < matrix_get_columns(p));
  assert(r >= 0 && r < matrix_get_rows(p));

  // for real part
  vector_subtract_column_vector_of_matrix(q, p, c, r);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!vector_is_imaginary(q))
      vector_attach_imaginary(q);
    ivector_subtract_column_vector_of_imatrix(q, p, c, r);
  }

  return q;
}

#define ROW_VECTOR_OF_MATRIX(qbuf, qlen, opcode, pbuf, pcols, prows, c, r) { \
    int i;								\
    int cols;								\
    assert(qbuf);							\
    assert(pbuf);							\
    assert(c >= 0 && c < pcols);					\
    assert(r >= 0 && r < prows);					\
    cols = min(qlen, pcols - c) + c;					\
    for (i = c; i < cols; i++) {					\
      *(qbuf + i) opcode (*(pbuf + r * (pcols) + i));			\
    }									\
  }

vector_t *vector_copy_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), =,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *vector_copy_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), =,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_copy_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), =,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_copy_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), =,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *cvector_copy_row_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r)
{
  assert(q);
  assert(p);
  assert(c >= 0 && c < matrix_get_columns(p));
  assert(r >= 0 && r < matrix_get_rows(p));

  // for real part
  vector_copy_row_vector_of_matrix(q, p, c, r);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!vector_is_imaginary(q))
      vector_attach_imaginary(q);
    ivector_copy_row_vector_of_imatrix(q, p, c, r);
  } else {
    if (vector_is_imaginary(q))
      ivector_bezero(q, 0, matrix_get_columns(p) - c);
  }

  return q;
}

vector_t *vector_add_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), +=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *vector_add_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), +=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_add_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), +=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_add_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), +=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *cvector_add_row_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r)
{
  assert(q);
  assert(p);
  assert(c >= 0 && c < matrix_get_columns(p));
  assert(r >= 0 && r < matrix_get_rows(p));

  // for real part
  vector_add_row_vector_of_matrix(q, p, c, r);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!vector_is_imaginary(q))
      vector_attach_imaginary(q);
    ivector_add_row_vector_of_imatrix(q, p, c, r);
  }

  return q;
}

vector_t *vector_subtract_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), -=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *vector_subtract_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(vector_get_buffer(q), vector_get_dimension(q), -=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_subtract_row_vector_of_matrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), -=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *ivector_subtract_row_vector_of_imatrix(vector_t *q, matrix_t *p, int c, int r)
{
  ROW_VECTOR_OF_MATRIX(ivector_get_buffer(q), vector_get_dimension(q), -=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), c, r);
  return q;
}

vector_t *cvector_subtract_row_vector_of_cmatrix(vector_t *q, matrix_t *p, int c, int r)
{
  assert(q);
  assert(p);
  assert(c >= 0 && c < matrix_get_columns(p));
  assert(r >= 0 && r < matrix_get_rows(p));

  // for real part
  vector_subtract_row_vector_of_matrix(q, p, c, r);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!vector_is_imaginary(q))
      vector_attach_imaginary(q);
    ivector_subtract_row_vector_of_imatrix(q, p, c, r);
  }

  return q;
}

#define MATRIX_MINOR(qbuf, qcols, qrows, opcode, pbuf, pcols, prows, del_c, del_r) { \
    int c, r;								\
    assert(qbuf);							\
    assert(pbuf);							\
    assert(qcols == pcols - 1);						\
    assert(qrows == prows - 1);						\
    for (r = 0; r < qrows; r++)						\
      for (c = 0; c < qcols; c++)					\
	*(qbuf + r * (qcols) + c) opcode *(pbuf + ((r < del_r ? r : r + 1)) * (pcols) + ((c < del_c ? c : c + 1))); \
  }

matrix_t *matrix_copy_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
	       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *matrix_copy_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
	       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *imatrix_copy_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
	       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *imatrix_copy_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
	       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *cmatrix_copy_cmatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  assert(q);
  assert(p);
  assert(del_c >= 0 && del_c < matrix_get_columns(p));
  assert(del_r >= 0 && del_r < matrix_get_rows(p));
  assert(matrix_get_columns(q) == matrix_get_columns(p)-1);
  assert(matrix_get_rows(q) == matrix_get_rows(p)-1);

  // for real part
  matrix_copy_matrix_minor(q, p, del_c, del_r);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_copy_imatrix_minor(q, p, del_c, del_r);
  } else {
    if (matrix_is_imaginary(q))
      imatrix_bezero(q, 0, 0, matrix_get_columns(q), matrix_get_rows(q));
  }

  return q;
}

matrix_t *matrix_add_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
	       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *matrix_add_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
	       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *imatrix_add_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
	       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *imatrix_add_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
	       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *cmatrix_add_cmatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  assert(q);
  assert(p);
  assert(del_c >= 0 && del_c < matrix_get_columns(p));
  assert(del_r >= 0 && del_r < matrix_get_rows(p));
  assert(matrix_get_columns(q) == matrix_get_columns(p)-1);
  assert(matrix_get_rows(q) == matrix_get_rows(p)-1);

  // for real part
  matrix_add_matrix_minor(q, p, del_c, del_r);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_add_imatrix_minor(q, p, del_c, del_r);
  }

  return q;
}

matrix_t *matrix_subtract_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
	       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *matrix_subtract_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
	       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *imatrix_subtract_matrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
	       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *imatrix_subtract_imatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  MATRIX_MINOR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
	       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), del_c, del_r);
  return q;
}

matrix_t *cmatrix_subtract_cmatrix_minor(matrix_t *q, matrix_t *p, int del_c, int del_r)
{
  assert(q);
  assert(p);
  assert(del_c >= 0 && del_c < matrix_get_columns(p));
  assert(del_r >= 0 && del_r < matrix_get_rows(p));
  assert(matrix_get_columns(q) == matrix_get_columns(p)-1);
  assert(matrix_get_rows(q) == matrix_get_rows(p)-1);

  // for real part
  matrix_subtract_matrix_minor(q, p, del_c, del_r);

  // for imaginary part
  if (matrix_is_imaginary(p)) {
    if (!matrix_is_imaginary(q))
      matrix_attach_imaginary(q);
    imatrix_subtract_imatrix_minor(q, p, del_c, del_r);
  }

  return q;
}

matrix_t *matrix_new_and_copy_matrix_minor(matrix_t *p, int del_c, int del_r)
{
  matrix_t *q;

  assert(p);
  assert(del_c >= 0 && del_c < matrix_get_columns(p));
  assert(del_r >= 0 && del_r < matrix_get_rows(p));

  q = matrix_new(matrix_get_columns(p) - 1, matrix_get_rows(p) - 1, false);
  matrix_copy_matrix_minor(q, p, del_c, del_r);

  return q;
}

matrix_t *cmatrix_new_and_copy_cmatrix_minor(matrix_t *p, int del_c, int del_r)
{
  matrix_t *q;

  assert(p);
  assert(del_c >= 0 && del_c < matrix_get_columns(p));
  assert(del_r >= 0 && del_r < matrix_get_rows(p));

  q = matrix_new(matrix_get_columns(p) - 1, matrix_get_rows(p) - 1, matrix_is_imaginary(p));
  cmatrix_copy_cmatrix_minor(q, p, del_c, del_r);

  return q;
}

#define MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(qbuf, qcols, qrows, op1, pbuf, pcols, prows, op2, v) { \
    int c, r;								\
    assert(qbuf);							\
    assert(pbuf);							\
    assert(qcols == pcols);						\
    assert(qrows == prows);						\
    for (r = 0; r < qrows; r++)						\
      for (c = 0; c < qcols; c++)					\
	*(qbuf+r*(qcols)+c) op1 (*(pbuf+r*(pcols)+c)) op2 v;		\
  }

matrix_t *matrix_copy_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *matrix_copy_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *imatrix_copy_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *imatrix_copy_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *cmatrix_copy_cmatrix_add_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part
  matrix_copy_matrix_add_scalar(q, p, v.real);

  // for imaginary part
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_copy_imatrix_add_scalar(q, p, v.imag);
  else
    imatrix_fill(q, 0, 0, matrix_get_columns(q), matrix_get_rows(q), v.imag);

  return q;
}

matrix_t *matrix_add_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *matrix_add_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *imatrix_add_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *imatrix_add_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *cmatrix_add_cmatrix_add_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part
  matrix_add_matrix_add_scalar(q, p, v.real);

  // for imaginary part
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_add_scalar(q, p, v.imag);
  else
    imatrix_add_scalar(q, v.imag);

  return q;
}

matrix_t *matrix_subtract_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *matrix_subtract_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *imatrix_subtract_matrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *imatrix_subtract_imatrix_add_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +, v);
  return q;
}

matrix_t *cmatrix_subtract_cmatrix_add_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part
  matrix_subtract_matrix_add_scalar(q, p, v.real);

  // for imaginary part
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_subtract_imatrix_add_scalar(q, p, v.imag);
  else
    imatrix_subtract_scalar(q, v.imag);

  return q;
}

#define MATRIX_OPCODE_SCALAR(qbuf, qcols, qrows, op, v) {	\
    int c, r;							\
    assert(qbuf);						\
    for (r = 0; r < qrows; r++)					\
      for (c = 0; c < qcols; c++)				\
	*(qbuf + r * (qcols) + c) op v;				\
  }

matrix_t *matrix_add_scalar(matrix_t *q, real_t v)
{
  MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=, v);
  return q;
}

matrix_t *imatrix_add_scalar(matrix_t *q, real_t v)
{
  MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=, v);
  return q;
}

matrix_t *cmatrix_add_scalar(matrix_t *q, complex_t v)
{
  matrix_add_scalar(q, v.real);
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);
  imatrix_add_scalar(q, v.imag);

  return q;      
}

matrix_t *matrix_copy_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

// Subtraction between matrix and scalar
matrix_t *matrix_copy_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *imatrix_copy_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *imatrix_copy_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *cmatrix_copy_cmatrix_subtract_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part
  matrix_copy_matrix_subtract_scalar(q, p, v.real);

  // for imaginary part
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_copy_imatrix_subtract_scalar(q, p, v.imag);
  else
    imatrix_fill(q, 0, 0, matrix_get_columns(q), matrix_get_rows(q), -v.imag);

  return q;
}

matrix_t *matrix_add_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *matrix_add_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *imatrix_add_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *imatrix_add_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *cmatrix_add_cmatrix_subtract_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part
  matrix_add_matrix_subtract_scalar(q, p, v.real);

  // for imaginary part
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_subtract_scalar(q, p, v.imag);
  else
    imatrix_subtract_scalar(q, v.imag);

  return q;
}

matrix_t *matrix_subtract_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *matrix_subtract_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *imatrix_subtract_matrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *imatrix_subtract_imatrix_subtract_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -, v);
  return q;
}

matrix_t *cmatrix_subtract_cmatrix_subtract_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part
  matrix_subtract_matrix_subtract_scalar(q, p, v.real);

  // for imaginary part
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_subtract_imatrix_subtract_scalar(q, p, v.imag);
  else
    imatrix_add_scalar(q, v.imag);

  return q;
}

matrix_t *matrix_subtract_scalar(matrix_t *q, real_t v)
{
  MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=, v);
  return q;
}

matrix_t *imatrix_subtract_scalar(matrix_t *q, real_t v)
{
  MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=, v);
  return q;
}

matrix_t *cmatrix_subtract_scalar(matrix_t *q, complex_t v)
{
  matrix_subtract_scalar(q, v.real);
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);
  imatrix_subtract_scalar(q, v.imag);

  return q;
}

matrix_t *matrix_copy_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *matrix_copy_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *imatrix_copy_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *imatrix_copy_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *cmatrix_copy_cmatrix_multiply_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part (p->real * real - p->imag * imag)
  matrix_copy_matrix_multiply_scalar(q, p, v.real);
  if (matrix_is_imaginary(p))
    matrix_subtract_imatrix_multiply_scalar(q, p, v.imag);

  // for imaginary part i * (p->real * imag + p->imag * real)
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  imatrix_copy_matrix_multiply_scalar(q, p, v.imag);
  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_multiply_scalar(q, p, v.real);

  return q;
}

matrix_t *matrix_add_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *matrix_add_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *imatrix_add_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *imatrix_add_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *cmatrix_add_cmatrix_multiply_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part (p->real * real - p->imag*imag)
  matrix_add_matrix_multiply_scalar(q, p, v.real);
  if (matrix_is_imaginary(p))
    matrix_subtract_imatrix_multiply_scalar(q, p, v.imag);

  // for imaginary part i*(p->real * imag + p->imag * real)
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  imatrix_add_matrix_multiply_scalar(q, p, v.imag);
  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_multiply_scalar(q, p, v.real);

  return q;
}

matrix_t *matrix_subtract_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *matrix_subtract_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *imatrix_subtract_matrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *imatrix_subtract_imatrix_multiply_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *, v);
  return q;
}

matrix_t *cmatrix_subtract_cmatrix_multiply_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part (p->real * real - p->imag * imag)
  matrix_subtract_matrix_multiply_scalar(q, p, v.real);
  if (matrix_is_imaginary(p))
    matrix_add_imatrix_multiply_scalar(q, p, v.imag);

  // for imaginary part i*(p->real * imag + p->imag * real)
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  imatrix_subtract_matrix_multiply_scalar(q, p, v.imag);
  if (matrix_is_imaginary(p))
    imatrix_subtract_imatrix_multiply_scalar(q, p, v.real);

  return q;
}

matrix_t *matrix_multiply_scalar(matrix_t *q, real_t v)
{
  MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), *=, v);
  return q;
}

matrix_t *imatrix_multiply_scalar(matrix_t *q, real_t v)
{
  MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), *=, v);
  return q;
}

matrix_t *cmatrix_multiply_scalar(matrix_t *q, complex_t v)
{
  matrix_t *p;

  assert(q);

  p = matrix_new_and_copy(q);
  cmatrix_copy_cmatrix_multiply_scalar(q, p, v);
  matrix_destroy(p);

  return q;
}

// division between matrix and scalar
matrix_t *matrix_copy_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *matrix_copy_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *imatrix_copy_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *imatrix_copy_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

/*
   (p->real + i * p->imag) / (real + i * imag)
 = ((p->real * real + p->imag * imag) + i * (-p->real * imag + p->imag * real)) / (real^2 + imag^2);
*/
matrix_t *cmatrix_copy_cmatrix_divide_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  real_t norm;

  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  norm = sqr(v.real) + sqr(v.imag);
  assert(norm >= REAL_EPSILON);

  // for real part (p->real * real + p->imag * imag) / (real^2 + imag^2)
  matrix_copy_matrix_multiply_scalar(q, p, v.real);
  if (matrix_is_imaginary(p))
    matrix_add_imatrix_multiply_scalar(q, p, v.imag);
  matrix_divide_scalar(q, norm);

  // for imaginary part (-p->real * imag + p->imag * real) / (real^2 + imag^2);
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  imatrix_copy_matrix_multiply_scalar(q, p, -v.imag);
  //  imatrix_subtract_matrix_multiply_scalar(q, p, v.imag);
  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_multiply_scalar(q, p, v.real);
  imatrix_divide_scalar(q, norm);

  return q;
}

matrix_t *matrix_add_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *matrix_add_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *imatrix_add_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *imatrix_add_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *cmatrix_add_cmatrix_divide_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  real_t norm;

  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  norm = sqr(v.real) + sqr(v.imag);
  assert(norm >= REAL_EPSILON);

  // for real part (p->real * real + p->imag * imag) / (real^2 + imag^2)
  matrix_add_matrix_multiply_scalar(q, p, v.real);
  if (matrix_is_imaginary(p))
    matrix_add_imatrix_multiply_scalar(q, p, v.imag);
  matrix_divide_scalar(q, norm);

  // for imaginary part (-p->real * imag + p->imag * real) / (real^2 + imag^2);
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  imatrix_subtract_matrix_multiply_scalar(q, p, v.imag);
  //  imatrix_subtract_matrix_multiply_scalar(q, p, v.imag);
  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_multiply_scalar(q, p, v.real);
  imatrix_divide_scalar(q, norm);

  return q;
}

matrix_t *matrix_subtract_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *matrix_subtract_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *imatrix_subtract_matrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *imatrix_subtract_imatrix_divide_scalar(matrix_t *q, matrix_t *p, real_t v)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
		       imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /, v);
  return q;
}

matrix_t *cmatrix_subtract_cmatrix_divide_scalar(matrix_t *q, matrix_t *p, complex_t v)
{
  real_t norm;

  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  norm = sqr(v.real) + sqr(v.imag);
  assert(norm >= REAL_EPSILON);

  // for real part (p->real * real + p->imag * imag) / (real^2 + imag^2)
  matrix_subtract_matrix_multiply_scalar(q, p, v.real);
  if (matrix_is_imaginary(p))
    matrix_subtract_imatrix_multiply_scalar(q, p, v.imag);
  matrix_divide_scalar(q, norm);

  // for imaginary part (-p->real * imag + p->imag * real) / (real^2 + imag^2);
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  imatrix_add_matrix_multiply_scalar(q, p, v.imag);
  if (matrix_is_imaginary(p))
    imatrix_subtract_imatrix_multiply_scalar(q, p, v.real);
  imatrix_divide_scalar(q, norm);

  return q;
}

matrix_t *matrix_divide_scalar(matrix_t *q, real_t v)
{
  MATRIX_OPCODE_SCALAR(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), /=, v);
  return q;
}

matrix_t *imatrix_divide_scalar(matrix_t *q, real_t v)
{
  MATRIX_OPCODE_SCALAR(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), /=, v);
  return q;
}

matrix_t *cmatrix_divide_scalar(matrix_t *q, complex_t v)
{
  matrix_t *p;

  assert(q);

  p = matrix_new_and_copy(q);
  cmatrix_copy_cmatrix_divide_scalar(q, p, v);
  matrix_destroy(p);

  return q;
}

#define MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(qbuf, qcols, qrows, op1, pbuf, pcols, prows, op2, v, c, r, dc, dr) { \
    int i, j, nc, nr;							\
    assert(qbuf);							\
    assert(pbuf);							\
    assert(qcols == pcols);						\
    assert(qrows == prows);						\
    assert(c >= 0 && c < qcols);					\
    assert(r >= 0 && r < qrows);					\
    assert(dc > 0);							\
    assert(dr > 0);							\
    nr = min(r + dr, qrows);						\
    nc = min(c + dc, qcols);						\
    for (i = r; i < nr; i++)						\
      for (j = c; j < nc; j++)						\
	*(qbuf + i * (qcols) + j) op1 (*(pbuf + i * (pcols) + j)) op2 v; \
  }

#define MATRIX_OPCODE_SCALAR_ON_REGION(qbuf, qcols, qrows, opcode, v, c, r, dc, dr) { \
    int i, j, nc, nr;							\
    assert(qbuf);							\
    assert(c >= 0 && c < qcols);					\
    assert(r >= 0 && r < qrows);					\
    assert(dc > 0);							\
    assert(dr > 0);							\
    nr = min(r + dr, qrows);						\
    nc = min(c + dc, qcols);						\
    for (i = r; i < nr; i++)						\
      for (j = c; j < nc; j++)						\
	*(qbuf + i * (qcols) + j) opcode v;				\
  }

matrix_t *matrix_copy_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_copy_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_copy_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_copy_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_copy_cmatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));
  assert(c >= 0 && c < matrix_get_columns(q));
  assert(r >= 0 && r < matrix_get_rows(q));

  // for real part
  matrix_copy_matrix_add_scalar_on_region(q, p, v.real, c, r, dc, dr);

  // for imaginary part
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_copy_imatrix_add_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  else
    imatrix_fill(q, c, r, dc, dr, v.imag);

  return q;
}

matrix_t *matrix_add_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_add_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_add_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_add_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_add_cmatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));
  assert(c >= 0 && c < matrix_get_columns(q));
  assert(r >= 0 && r < matrix_get_rows(q));

  // for real part
  matrix_add_matrix_add_scalar_on_region(q, p, v.real, c, r, dc, dr);

  // for imaginary part
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_add_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  else
    imatrix_add_scalar_on_region(q, v.imag, c, r, dc, dr);

  return q;
}

matrix_t *matrix_subtract_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_subtract_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_subtract_matrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_subtract_imatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), +,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_subtract_cmatrix_add_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));
  assert(c >= 0 && c < matrix_get_columns(q));
  assert(r >= 0 && r < matrix_get_rows(q));

  // for real part
  matrix_subtract_matrix_add_scalar_on_region(q, p, v.real, c, r, dc, dr);

  // for imaginary part
  if (!matrix_is_imaginary(q))
    matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_subtract_imatrix_add_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  else
    imatrix_copy_imatrix_subtract_scalar_on_region(q, q, v.imag, c, r, dc, dr);

  return q;
}

matrix_t *matrix_add_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=, v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_add_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=, v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_add_scalar_on_region(matrix_t *q, complex_t v, int c, int r, int dc, int dr)
{
  matrix_add_scalar_on_region(q, v.real, c, r, dc, dr);
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);
  imatrix_add_scalar_on_region(q, v.imag, c, r, dc, dr);

  return q;
}

// subtraction between matrix and scalar on region
matrix_t *matrix_copy_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_copy_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_copy_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_copy_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_copy_cmatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));
  assert(c >= 0 && c < matrix_get_columns(q));
  assert(r >= 0 && r < matrix_get_rows(q));

  // for real part
  matrix_copy_matrix_subtract_scalar_on_region(q, p, v.real, c, r, dc, dr);

  // for imaginary part
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_copy_imatrix_subtract_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  else
    imatrix_fill(q, c, r, dc, dr, -v.imag);

  return q;
}

matrix_t *matrix_add_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_add_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_add_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_add_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_add_cmatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));
  assert(c >= 0 && c < matrix_get_columns(q));
  assert(r >= 0 && r < matrix_get_rows(q));

  // for real part
  matrix_add_matrix_subtract_scalar_on_region(q, p, v.real, c, r, dc, dr);

  // for imaginary part
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_subtract_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  else
    imatrix_subtract_scalar_on_region(q, v.imag, c, r, dc, dr);

  return q;
}

matrix_t *matrix_subtract_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_subtract_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_subtract_matrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_subtract_imatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), -,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_subtract_cmatrix_subtract_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));
  assert(c >= 0 && c < matrix_get_columns(q));
  assert(r >= 0 && r < matrix_get_rows(q));

  // for real part
  matrix_subtract_matrix_subtract_scalar_on_region(q, p, v.real, c, r, dc, dr);

  // for imaginary part
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);

  if (matrix_is_imaginary(p))
    imatrix_subtract_imatrix_subtract_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  else
    imatrix_add_scalar_on_region(q, v.imag, c, r, dc, dr);

  return q;
}

matrix_t *matrix_subtract_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=, v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_subtract_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=, v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_subtract_scalar_on_region(matrix_t *q, complex_t v, int c, int r, int dc, int dr)
{
  matrix_subtract_scalar_on_region(q, v.real, c, r, dc, dr);
  if (matrix_is_imaginary(q)) matrix_attach_imaginary(q);
  imatrix_subtract_scalar_on_region(q, v.imag, c, r, dc, dr);

  return q;
}

// multiplication between matrix and scalar on region
matrix_t *matrix_copy_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_copy_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_copy_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_copy_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_copy_cmatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part (p->real * real - p->imag * imag)
  matrix_copy_matrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    matrix_subtract_imatrix_multiply_scalar_on_region(q, p, v.imag, c, r, dc, dr);

  // for imaginary part (p->real * imag + p->imag * real)
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);
  imatrix_copy_matrix_multiply_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);

  return q;
}

matrix_t *matrix_add_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_add_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_add_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_add_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_add_cmatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part (p->real * real - p->imag * imag)
  matrix_add_matrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    matrix_subtract_imatrix_multiply_scalar_on_region(q, p, v.imag, c, r, dc, dr);

  // for imaginary part (p->real * imag + p->imag * real)
  if (matrix_is_imaginary(q)) matrix_attach_imaginary(q);
  imatrix_add_matrix_multiply_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);

  return q;
}

matrix_t *matrix_subtract_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_subtract_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_subtract_matrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_subtract_imatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), *,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_subtract_cmatrix_multiply_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  // for real part (p->real * real - p->imag * imag)
  matrix_subtract_matrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    matrix_add_imatrix_multiply_scalar_on_region(q, p, v.imag, c, r, dc, dr);

  // for imaginary part i*(p->real * imag + p->imag * real)
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);
  imatrix_subtract_matrix_multiply_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    imatrix_subtract_imatrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);

  return q;
}

matrix_t *matrix_multiply_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), *=, v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_multiply_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), *=, v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_multiply_scalar_on_region(matrix_t *q, complex_t v, int c, int r, int dc, int dr)
{
  matrix_t *p;

  assert(q);

  p = matrix_new_and_copy(q);
  cmatrix_copy_cmatrix_multiply_scalar_on_region(q, p, v, c, r, dc, dr);
  matrix_destroy(p);

  return q;
}

// division between matrix and scalar on region
matrix_t *matrix_copy_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_copy_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_copy_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_copy_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), =,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

/*
   (p->real + i * p->imag) / (real + i * imag)
 = ((p->real * real + p->imag * imag) + i * (-p->real * imag + p->imag * real)) / (real^2 + imag^2);
*/
matrix_t *cmatrix_copy_cmatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  real_t norm;

  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  norm = sqr(v.real) + sqr(v.imag);
  assert(norm >= REAL_EPSILON);

  // for real part (p->real * real + p->imag * imag) / (real^2 + imag^2)
  matrix_copy_matrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    matrix_add_imatrix_multiply_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  matrix_divide_scalar_on_region(q, norm, c, r, dc, dr);

  // for imaginary part (-p->real * imag + p->imag * real) / (real^2 + imag^2);
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);
  imatrix_copy_matrix_multiply_scalar_on_region(q, p, -v.imag, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);
  imatrix_divide_scalar_on_region(q, norm, c, r, dc, dr);

  return q;
}

matrix_t *matrix_add_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_add_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_add_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_add_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), +=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_add_cmatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  real_t norm;

  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  norm = v.real*v.real+v.imag*v.imag;
  assert(norm >= REAL_EPSILON);

  // for real part (p->real * real + p->imag * imag) / (real^2 + imag^2)
  matrix_add_matrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    matrix_add_imatrix_multiply_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  matrix_divide_scalar_on_region(q, norm, c, r, dc, dr);

  // for imaginary part (-p->real * imag + p->imag * real) / (real^2 + imag^2);
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);
  imatrix_add_matrix_multiply_scalar_on_region(q, p, -v.imag, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    imatrix_add_imatrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);
  imatrix_divide_scalar_on_region(q, norm, c, r, dc, dr);

  return q;
}

matrix_t *matrix_subtract_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *matrix_subtract_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_subtract_matrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 matrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_subtract_imatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), -=,
				 imatrix_get_buffer(p), matrix_get_columns(p), matrix_get_rows(p), /,
				 v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_subtract_cmatrix_divide_scalar_on_region(matrix_t *q, matrix_t *p, complex_t v, int c, int r, int dc, int dr)
{
  real_t norm;

  assert(q);
  assert(p);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  norm = sqr(v.real) + sqr(v.imag);
  assert(norm >= REAL_EPSILON);

  // for real part (p->real * real + p->imag * imag) / (real^2 + imag^2)
  matrix_subtract_matrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    matrix_subtract_imatrix_multiply_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  matrix_divide_scalar_on_region(q, norm, c, r, dc, dr);

  // for imaginary part (-p->real * imag + p->imag * real) / (real^2 + imag^2);
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);
  imatrix_add_matrix_multiply_scalar_on_region(q, p, v.imag, c, r, dc, dr);
  if (matrix_is_imaginary(p))
    imatrix_subtract_imatrix_multiply_scalar_on_region(q, p, v.real, c, r, dc, dr);
  imatrix_divide_scalar_on_region(q, norm, c, r, dc, dr);

  return q;
}

matrix_t *matrix_divide_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_SCALAR_ON_REGION(matrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), /=, v, c, r, dc, dr);
  return q;
}

matrix_t *imatrix_divide_scalar_on_region(matrix_t *q, real_t v, int c, int r, int dc, int dr)
{
  MATRIX_OPCODE_SCALAR_ON_REGION(imatrix_get_buffer(q), matrix_get_columns(q), matrix_get_rows(q), /=, v, c, r, dc, dr);
  return q;
}

matrix_t *cmatrix_divide_scalar_on_region(matrix_t *q, complex_t v, int c, int r, int dc, int dr)
{
  matrix_t *p;

  assert(q);

  p = matrix_new_and_copy(q);
  cmatrix_copy_cmatrix_divide_scalar_on_region(q, p, v, c, r, dc, dr);
  matrix_destroy(p);

  return q;
}

#define MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(cbuf, ccols, crows, op1, abuf, acols, arows, op2, bbuf, bcols, brows) { \
    int i, j;								\
    assert(cbuf);							\
    assert(abuf);							\
    assert(bbuf);							\
    assert(ccols == acols);						\
    assert(crows == arows);						\
    assert(acols == bcols);						\
    assert(arows == brows);						\
    for (i = 0; i < arows; i++)						\
      for (j = 0; j < acols; j++)					\
	*(cbuf + i * (ccols) + j) op1 (*(abuf + i * (acols) + j)) op2 (*(bbuf + i * (bcols) + j)); \
  }

#define MATRIX_OPCODE_MATRIX(cbuf, ccols, crows, opcode, abuf, acols, arows) { \
    int i, j;								\
    assert(cbuf);							\
    assert(abuf);							\
    assert(ccols == acols);						\
    assert(crows == arows);						\
    for (i = 0; i < arows; i++)						\
      for (j = 0; j < acols; j++)					\
	*(cbuf + i * (ccols) + j) opcode (*(abuf + i * (acols) + j));	\
  }
  
matrix_t *matrix_copy_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_copy_cmatrix_add_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_get_columns(c) == matrix_get_columns(a));
  assert(matrix_get_rows(c) == matrix_get_rows(a));
  assert(matrix_get_columns(a) == matrix_get_columns(b));
  assert(matrix_get_rows(a) == matrix_get_rows(b));

  // for real part
  matrix_copy_matrix_add_matrix(c, a, b);

  // for imaginary part
  if (!matrix_is_imaginary(c) &&
      (matrix_is_imaginary(a) || matrix_is_imaginary(b)))
    matrix_attach_imaginary(c);

  if (matrix_is_imaginary(a)) {
    if (matrix_is_imaginary(b))
      imatrix_copy_imatrix_add_imatrix(c, a, b);
    else
      imatrix_copy_imatrix(c, 0, 0, a, 0, 0, matrix_get_columns(a), matrix_get_rows(a));
  } else {
    if (matrix_is_imaginary(b))
      imatrix_copy_imatrix(c, 0, 0, b, 0, 0, matrix_get_columns(b), matrix_get_rows(b));
  }

  return c;
}

matrix_t *matrix_add_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_add_cmatrix_add_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // for real part
  matrix_add_matrix_add_matrix(c, a, b);

  // for imaginary part
  if (!matrix_is_imaginary(c) &&
      (matrix_is_imaginary(a) || matrix_is_imaginary(b)))
    matrix_attach_imaginary(c);

  if (matrix_is_imaginary(a)) {
    if (matrix_is_imaginary(b))
      imatrix_add_imatrix_add_imatrix(c, a, b);
    else
      imatrix_add_imatrix(c, a);
  } else {
    if (matrix_is_imaginary(b))
      imatrix_add_imatrix(c, b);
  }

  return c;
}

matrix_t *matrix_subtract_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_matrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_matrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_imatrix_add_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_imatrix_add_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), +,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_subtract_cmatrix_add_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // for real part
  matrix_subtract_matrix_add_matrix(c, a, b);

  // for imaginary part
  if (!matrix_is_imaginary(c) &&
      (matrix_is_imaginary(a) || matrix_is_imaginary(b)))
    matrix_attach_imaginary(c);

  if (matrix_is_imaginary(a)) {
    if (matrix_is_imaginary(b))
      imatrix_subtract_imatrix_add_imatrix(c, a, b);
    else
      imatrix_subtract_imatrix(c, a);
  } else {
    if (matrix_is_imaginary(b))
      imatrix_subtract_imatrix(c, b);
  }

  return c;
}

matrix_t *matrix_add_matrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=, matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *matrix_add_imatrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=, imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *imatrix_add_matrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=, matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *imatrix_add_imatrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=, imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *cmatrix_add_cmatrix(matrix_t *c, matrix_t *a)
{
  assert(c);
  assert(a);
  assert(matrix_are_matched(c, a));

  // for real part
  matrix_add_matrix(c, a);

  // for imaginary part
  if (matrix_is_imaginary(a)) {
    if (!matrix_is_imaginary(c))
      matrix_attach_imaginary(c);
    imatrix_add_imatrix(c, a);
  }

  return c;
}

// subtraction between matrix and matrix
matrix_t *matrix_copy_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_copy_cmatrix_subtract_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // for real part
  matrix_copy_matrix_subtract_matrix(c, a, b);

  // for imaginary part
  if (!matrix_is_imaginary(c) &&
      (matrix_is_imaginary(a) || matrix_is_imaginary(b)))
    matrix_attach_imaginary(c);

  if (matrix_is_imaginary(a)) {
    if (matrix_is_imaginary(b))
      imatrix_copy_imatrix_subtract_imatrix(c, a, b);
    else
      imatrix_copy_imatrix(c, 0, 0, a, 0, 0, matrix_get_columns(a), matrix_get_rows(a));
  } else {
    if (matrix_is_imaginary(b))
      imatrix_copy_imatrix_multiply_scalar(c, b, -1);
  }

  return c;
}

matrix_t *matrix_add_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_add_cmatrix_subtract_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // for real part
  matrix_add_matrix_subtract_matrix(c, a, b);

  // for imaginary part
  if (!matrix_is_imaginary(c) &&
      (matrix_is_imaginary(a) || matrix_is_imaginary(b)))
    matrix_attach_imaginary(c);

  if (matrix_is_imaginary(a)) {
    if (matrix_is_imaginary(b))
      imatrix_add_imatrix_subtract_imatrix(c, a, b);
    else
      imatrix_add_imatrix(c, a);
  } else {
    if (matrix_is_imaginary(b))
      imatrix_subtract_imatrix(c, b);
  }

  return c;
}

matrix_t *matrix_subtract_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_matrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_matrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_imatrix_subtract_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_imatrix_subtract_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), -,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_subtract_cmatrix_subtract_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // for real part
  matrix_subtract_matrix_subtract_matrix(c, a, b);

  // for imaginary part
  if ((!matrix_is_imaginary(c)) &&
      (matrix_is_imaginary(a) || matrix_is_imaginary(b)))
    matrix_attach_imaginary(c);

  if (matrix_is_imaginary(a)) {
    if (matrix_is_imaginary(b))
      imatrix_subtract_imatrix_subtract_imatrix(c, a, b);
    else
      imatrix_subtract_imatrix(c, a);
  } else {
    if (matrix_is_imaginary(b))
      imatrix_add_imatrix(c, b);
  }

  return c;
}

matrix_t *matrix_subtract_matrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=, matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *matrix_subtract_imatrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=, imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *imatrix_subtract_matrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=, matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *imatrix_subtract_imatrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=, imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *cmatrix_subtract_cmatrix(matrix_t *c, matrix_t *a)
{
  assert(c);
  assert(a);
  assert(matrix_are_matched(c, a));

  // for real part
  matrix_subtract_matrix(c, a);

  // for imaginary part
  if (matrix_is_imaginary(a)) {
    if (!matrix_is_imaginary(c))
      matrix_attach_imaginary(c);
    imatrix_subtract_imatrix(c, a);
  }

  return c;
}

#define MATRIX_OPCODE_MATRIX_MUL_MATRIX(cbuf, ccols, crows, opcode, abuf, acols, arows, bbuf, bcols, brows) { \
    int i, j, k;							\
    real_t value;							\
    assert(cbuf);							\
    assert(abuf);							\
    assert(bbuf);							\
    assert(crows == arows);						\
    assert(ccols == bcols);						\
    assert(acols == brows);						\
    for (i = 0; i < crows; i++) {					\
      for (j = 0; j < ccols; j++) {					\
	value = 0.0;							\
	for (k = 0; k < acols; k++) {					\
	  value += (*(abuf + i * (acols) + k)) * (*(bbuf + k * (bcols) + j)); \
	}								\
	*(cbuf + i * (ccols) + j) opcode value;				\
      }									\
    }									\
  }

matrix_t *matrix_copy_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_copy_cmatrix_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_get_rows(c) == matrix_get_rows(a));
  assert(matrix_get_columns(a) == matrix_get_rows(b));
  assert(matrix_get_columns(c) == matrix_get_columns(b));

  // for real part (a->real) * (b->real) - (a->imaginary) * (b->imaginary)
  matrix_copy_matrix_multiply_matrix(c, a, b);
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b))
    matrix_subtract_imatrix_multiply_imatrix(c, a, b);

  // for imaginary part (a->real) * (b->imaginary) + (a->imaginary) * (b->real)
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_copy_imatrix_multiply_matrix(c, a, b);
    imatrix_add_matrix_multiply_imatrix(c, a, b);
  } else {
    if (matrix_is_imaginary(c))
      imatrix_bezero(c, 0, 0, matrix_get_columns(c), matrix_get_rows(c));
  }

  return c;
}

matrix_t *matrix_add_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_add_cmatrix_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_get_rows(c) == matrix_get_rows(a));
  assert(matrix_get_columns(a) == matrix_get_rows(b));
  assert(matrix_get_columns(c) == matrix_get_columns(b));

  // for real part (a->real) * (b->real) - (a->imaginary) * (b->imaginary)
  matrix_add_matrix_multiply_matrix(c, a, b);
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b))
    matrix_subtract_imatrix_multiply_imatrix(c, a, b);

  // for imaginary part (a->real) * (b->imaginary) + (a->imaginary) * (b->real)
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_add_imatrix_multiply_matrix(c, a, b);
    imatrix_add_matrix_multiply_imatrix(c, a, b);
  }

  return c;
}

matrix_t *matrix_subtract_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_matrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_matrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_imatrix_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_imatrix_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_MUL_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_subtract_cmatrix_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_get_rows(c) == matrix_get_rows(a));
  assert(matrix_get_columns(a) == matrix_get_rows(b));
  assert(matrix_get_columns(c) == matrix_get_columns(b));

  // for real part (a->real) * (b->real) - (a->imaginary) * (b->imaginary)
  matrix_subtract_matrix_multiply_matrix(c, a, b);
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b))
    matrix_add_imatrix_multiply_imatrix(c, a, b);

  // for imaginary part (a->real) * (b->imaginary) + (a->imaginary) * (b->real)
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_subtract_imatrix_multiply_matrix(c, a, b);
    imatrix_subtract_matrix_multiply_imatrix(c, a, b);
  }

  return c;
}

matrix_t *matrix_new_and_copy_matrix_multiply_matrix(matrix_t *a, matrix_t *b)
{
  matrix_t *c;

  assert(a);
  assert(b);
  assert(matrix_get_columns(a) == matrix_get_rows(b));	

  c = matrix_new(matrix_get_columns(b), matrix_get_rows(a), false);
  matrix_copy_matrix_multiply_matrix(c, a, b);

  return c;
}

matrix_t *matrix_new_and_copy_matrix_multiply_imatrix(matrix_t *a, matrix_t *b)
{
  matrix_t *c;

  assert(a);
  assert(b);
  assert(matrix_get_columns(a) == matrix_get_rows(b));

  c = matrix_new(matrix_get_columns(b), matrix_get_rows(a), false);
  matrix_copy_matrix_multiply_imatrix(c, a, b);

  return c;
}

matrix_t *cmatrix_new_and_copy_cmatrix_multiply_cmatrix(matrix_t *a, matrix_t *b)
{
  matrix_t *c;

  assert(a);
  assert(b);
  assert(matrix_get_columns(a) == matrix_get_rows(b));

  c = matrix_new(matrix_get_columns(b), matrix_get_rows(a), false);
  cmatrix_copy_cmatrix_multiply_cmatrix(c, a, b);

  return c;
}

matrix_t *matrix_copy_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_copy_cmatrix_piecewise_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // for real part (a->real * b->real - a->imaginary * b->imaginary)
  matrix_copy_matrix_piecewise_multiply_matrix(c, a, b);
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b))
    matrix_subtract_imatrix_piecewise_multiply_imatrix(c, a, b);

  // for imaginary part (a->imaginary * b->imaginary + a->imaginary * b->real)
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_copy_imatrix_piecewise_multiply_matrix(c, a, b);
    imatrix_add_matrix_piecewise_multiply_imatrix(c, a, b);
  } else {
    if (matrix_is_imaginary(c))
      imatrix_bezero(c, 0, 0, matrix_get_columns(c), matrix_get_rows(c));
  }

  return c;
}

matrix_t *matrix_add_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_add_cmatrix_piecewise_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // for real part (a->real * b->real - a->imaginary * b->imaginary)
  matrix_add_matrix_piecewise_multiply_matrix(c, a, b);
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b))
    matrix_subtract_imatrix_piecewise_multiply_imatrix(c, a, b);

  // for imaginary part (a->imaginary * b->imaginary + a->imaginary * b->real)
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_add_imatrix_piecewise_multiply_matrix(c, a, b);
    imatrix_add_matrix_piecewise_multiply_imatrix(c, a, b);
  }

  return c;
}

matrix_t *matrix_subtract_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), *,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_subtract_cmatrix_piecewise_multiply_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // for real part (a->real * b->real - a->imaginary * b->imaginary)
  matrix_subtract_matrix_piecewise_multiply_matrix(c, a, b);
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b))
    matrix_add_imatrix_piecewise_multiply_imatrix(c, a, b);

  // for imaginary part (a->imaginary * b->imaginary + a->imaginary * b->real)
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_subtract_imatrix_piecewise_multiply_matrix(c, a, b);
    imatrix_subtract_matrix_piecewise_multiply_imatrix(c, a, b);
  }

  return c;
}

matrix_t *matrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), *=, matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *matrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), *=, imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *imatrix_piecewise_multiply_matrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), *=, matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *imatrix_piecewise_multiply_imatrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), *=, imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *cmatrix_piecewise_multiply_cmatrix(matrix_t *c, matrix_t *a)
{
  matrix_t *tmp;

  tmp = matrix_new_and_copy(c);
  cmatrix_copy_cmatrix_piecewise_multiply_cmatrix(c, tmp, a);
  matrix_destroy(tmp);

  return c;
}

matrix_t *matrix_copy_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_copy_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_copy_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_copy_cmatrix_piecewise_divide_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  matrix_t *denom;

  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // (b->real * b->real + b->imag * b->imag)
  denom = matrix_new_and_copy(a);
  matrix_copy_matrix_piecewise_multiply_matrix(denom, b, b);
  matrix_add_imatrix_piecewise_multiply_imatrix(denom, b, b);

  // for real part (a->real * b->real + a->imag * b->imag) / (b->real * b->real + b->imag * b->imag)
  matrix_copy_matrix_piecewise_multiply_matrix(c, a, b);
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b))
    matrix_add_imatrix_piecewise_multiply_imatrix(c, a, b);
  matrix_piecewise_divide_matrix(c, denom);

  // for imaginary part (-a->real * b->imag + a->imag * b->real) / (b->real * b->real + b->imag * b->imag)
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_copy_imatrix_piecewise_multiply_matrix(c, a, b);
    imatrix_subtract_matrix_piecewise_multiply_imatrix(c, a, b);
    imatrix_piecewise_divide_matrix(c, denom);
  } else {
    if (matrix_is_imaginary(c))
      imatrix_bezero(c, 0, 0, matrix_get_columns(c), matrix_get_rows(c));
  }
  matrix_destroy(denom);

  return c;
}

matrix_t *matrix_add_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_add_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_add_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_add_cmatrix_piecewise_divide_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  matrix_t *denom;

  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // (b->real * b->real + b->imag * b->imag)
  denom = matrix_new_and_copy(a);
  matrix_copy_matrix_piecewise_multiply_matrix(denom, b, b);
  matrix_add_imatrix_piecewise_multiply_imatrix(denom, b, b);

  // for real part (a->real * b->real + a->imag * b->imag) / (b->real * b->real + b->imag * b->imag)
  matrix_add_matrix_piecewise_multiply_matrix(c, a, b);
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b))
    matrix_add_imatrix_piecewise_multiply_imatrix(c, a, b);
  matrix_piecewise_divide_matrix(c, denom);

  // for imaginary part (-a->real * b->imag + a->imag * b->real) / (b->real * b->real + b->imag * b->imag)
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_add_imatrix_piecewise_multiply_matrix(c, a, b);
    imatrix_subtract_matrix_piecewise_multiply_imatrix(c, a, b);
    imatrix_piecewise_divide_matrix(c, denom);
  }
  matrix_destroy(denom);

  return c;
}

matrix_t *matrix_subtract_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *matrix_subtract_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *imatrix_subtract_imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  MATRIX_OPCODE_MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				     imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a), /,
				     imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

matrix_t *cmatrix_subtract_cmatrix_piecewise_divide_cmatrix(matrix_t *c, matrix_t *a, matrix_t *b)
{
  matrix_t *denom;

  assert(c);
  assert(a);
  assert(b);
  assert(matrix_are_matched(c, a));
  assert(matrix_are_matched(c, b));

  // (b->real * b->real + b->imag * b->imag)
  denom = matrix_new_and_copy(a);
  matrix_copy_matrix_piecewise_multiply_matrix(denom, b, b);
  matrix_add_imatrix_piecewise_multiply_imatrix(denom, b, b);

  // for real part (a->real * b->real + a->imag * b->imag) / (b->real * b->real + b->imag * b->imag)
  matrix_subtract_matrix_piecewise_multiply_matrix(c, a, b);
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b))
    matrix_add_imatrix_piecewise_multiply_imatrix(c, a, b);
  matrix_piecewise_divide_matrix(c, denom);

  // for imaginary part (-a->real * b->imag + a->imag * b->real) / (b->real * b->real + b->imag * b->imag)
  if (matrix_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_subtract_imatrix_piecewise_multiply_matrix(c, a, b);
    imatrix_add_matrix_piecewise_multiply_imatrix(c, a, b);
    imatrix_piecewise_divide_matrix(c, denom);
  }
  matrix_destroy(denom);

  return c;
}

matrix_t *matrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), /=, matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *matrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), /=, imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *imatrix_piecewise_divide_matrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), /=, matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *imatrix_piecewise_divide_imatrix(matrix_t *c, matrix_t *a)
{
  MATRIX_OPCODE_MATRIX(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), /=, imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a));
  return c;
}

matrix_t *cmatrix_piecewise_divide_cmatrix(matrix_t *c, matrix_t *a)
{
  matrix_t *tmp;

  assert(c);
  assert(a);
  assert(matrix_are_matched(c, a));

  tmp = matrix_new_and_copy(c);
  cmatrix_copy_cmatrix_piecewise_divide_cmatrix(c, tmp, a);
  matrix_destroy(tmp);

  return c;
}

#define VECTOR_OPCODE_MATRIX_MUL_VECTOR(cbuf, clen, opcode, abuf, acols, arows, bbuf, blen) { \
    int i, j;								\
    real_t value;							\
    assert(cbuf);							\
    assert(abuf);							\
    assert(bbuf);							\
    assert(clen == arows);						\
    assert(acols == blen);						\
    for (i = 0; i < arows; i++) {					\
      value = 0.0;							\
      for (j = 0; j < acols; j++) {					\
	value += (*(abuf + i * (acols) + j)) * (*(bbuf + j));		\
      }									\
      *(cbuf + i) opcode value;						\
    }									\
  }

// multiplication between matrix and vector
vector_t *vector_copy_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), =,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *vector_copy_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), =,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *vector_copy_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), =,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *vector_copy_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), =,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_copy_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), =,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_copy_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), =,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_copy_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), =,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_copy_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), =,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *cvector_copy_cmatrix_multiply_cvector(vector_t *c, matrix_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_dimension(c) == matrix_get_rows(a));
  assert(matrix_get_columns(a) == vector_get_dimension(b));

  // for real part (a->real) * (b->real) - (a->imaginary) * (b->imaginary)
  vector_copy_matrix_multiply_vector(c, a, b);
  if (matrix_is_imaginary(a) && vector_is_imaginary(b))
    vector_subtract_imatrix_multiply_ivector(c, a, b);

  // for imaginary part (a->real) * (b->imaginary) + (a->imaginary) * (b->real)
  if (matrix_is_imaginary(a) && vector_is_imaginary(b)) {
    if (!vector_is_imaginary(c)) vector_attach_imaginary(c);
    ivector_copy_imatrix_multiply_vector(c, a, b);
    ivector_add_matrix_multiply_ivector(c, a, b);
  } else {
    if (vector_is_imaginary(c))
      ivector_bezero(c, 0, vector_get_dimension(c));
  }

  return c;
}

vector_t *vector_add_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), +=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *vector_add_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), +=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *vector_add_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), +=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *vector_add_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), +=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_add_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), +=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_add_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), +=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_add_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), +=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_add_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), +=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *cvector_add_cmatrix_multiply_cvector(vector_t *c, matrix_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_dimension(c) == matrix_get_rows(a));
  assert(matrix_get_columns(a) == vector_get_dimension(b));

  // for real part (a->real) * (b->real) - (a->imaginary) * (b->imaginary)
  vector_add_matrix_multiply_vector(c, a, b);
  if (matrix_is_imaginary(a) && vector_is_imaginary(b))
    vector_subtract_imatrix_multiply_ivector(c, a, b);

  // for imaginary part (a->real) * (b->imaginary) + (a->imaginary) * (b->real)
  if (matrix_is_imaginary(a) && vector_is_imaginary(b)) {
    if (!vector_is_imaginary(c)) vector_attach_imaginary(c);
    ivector_add_imatrix_multiply_vector(c, a, b);
    ivector_add_matrix_multiply_ivector(c, a, b);
  }

  return c;
}

vector_t *vector_subtract_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), -=, 
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *vector_subtract_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), -=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *vector_subtract_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), -=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *vector_subtract_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(vector_get_buffer(c), vector_get_dimension(c), -=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_subtract_matrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), -=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_subtract_matrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), -=,
				  matrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_subtract_imatrix_multiply_vector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), -=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *ivector_subtract_imatrix_multiply_ivector(vector_t *c, matrix_t *a, vector_t *b)
{
  VECTOR_OPCODE_MATRIX_MUL_VECTOR(ivector_get_buffer(c), vector_get_dimension(c), -=,
				  imatrix_get_buffer(a), matrix_get_columns(a), matrix_get_rows(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

vector_t *cvector_subtract_cmatrix_multiply_cvector(vector_t *c, matrix_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_dimension(c) == matrix_get_rows(a));
  assert(matrix_get_columns(a) == vector_get_dimension(b));

  // for real part (a->real) * (b->real) - (a->imaginary) * (b->imaginary)
  vector_subtract_matrix_multiply_vector(c, a, b);
  if (matrix_is_imaginary(a) && vector_is_imaginary(b))
    vector_add_imatrix_multiply_ivector(c, a, b);

  // for imaginary part (a->real) * (b->imaginary) + (a->imaginary) * (b->real)
  if (matrix_is_imaginary(a) && vector_is_imaginary(b)) {
    if (!vector_is_imaginary(c)) vector_attach_imaginary(c);
    ivector_subtract_imatrix_multiply_vector(c, a, b);
    ivector_subtract_matrix_multiply_ivector(c, a, b);
  }

  return c;
}

vector_t *vector_new_and_copy_matrix_multiply_vector(matrix_t *a, vector_t *b)
{
  vector_t *c;

  assert(a);
  assert(b);
  assert(matrix_get_columns(a) == vector_get_dimension(b));

  c = vector_new(matrix_get_rows(a), false);
  vector_copy_matrix_multiply_vector(c, a, b);

  return c;
}

vector_t *cvector_new_and_copy_cmatrix_multiply_cvector(matrix_t *a, vector_t *b)
{
  vector_t *c;

  assert(a);
  assert(b);
  assert(matrix_get_columns(a) == vector_get_dimension(b));

  c = vector_new(matrix_get_rows(a), false);
  cvector_copy_cmatrix_multiply_cvector(c, a, b);

  return c;
}

#define VECTOR_OPCODE_VECTOR_MUL_MATRIX(cbuf, clen, opcode, abuf, alen, bbuf, bcols, brows) { \
    int i, j;								\
    real_t value;							\
    assert(cbuf);							\
    assert(abuf);							\
    assert(bbuf);							\
    assert(clen == bcols);						\
    assert(alen == brows);						\
    for (i = 0; i < clen; i++) {					\
      value = 0.0;							\
      for (j = 0; j < alen; j++) {					\
	value += (*(abuf + j)) * (*(bbuf + j * (bcols) + i));		\
      }									\
      *(cbuf + i) opcode value;						\
    }									\
  }

vector_t *vector_copy_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), =,
				  vector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *vector_copy_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), =,
				  vector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *vector_copy_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), =,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *vector_copy_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), =,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_copy_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), =,
				  vector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_copy_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), =,
				  vector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_copy_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), =,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_copy_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), =,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *cvector_copy_cvector_multiply_cmatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_dimension(c) == matrix_get_columns(b));
  assert(vector_get_dimension(a) == matrix_get_rows(b));

  // for real part (a->real) * (b->real) - (a->imaginary) * (b->imaginary)
  vector_copy_vector_multiply_matrix(c, a, b);
  if (vector_is_imaginary(a) && matrix_is_imaginary(b))
    vector_subtract_ivector_multiply_imatrix(c, a, b);

  // for imaginary part (a->imaginary) * (b->imaginary) + (a->imaginary) * (b->real)
  if (vector_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!vector_is_imaginary(c)) vector_attach_imaginary(c);
    ivector_copy_ivector_multiply_matrix(c, a, b);
    ivector_add_vector_multiply_imatrix(c, a, b);
  } else {
    if (vector_is_imaginary(c))
      ivector_bezero(c, 0, vector_get_dimension(c));
  }

  return c;
}

vector_t *vector_add_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), +=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *vector_add_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), +=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *vector_add_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), +=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *vector_add_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), +=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_add_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), +=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_add_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), +=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_add_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), +=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_add_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), +=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *cvector_add_cvector_multiply_cmatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_dimension(c) == matrix_get_columns(b));
  assert(vector_get_dimension(a) == matrix_get_rows(b));

  // for real part (a->real) * (b->real) - (a->imaginary) * (b->imaginary)
  vector_add_vector_multiply_matrix(c, a, b);
  if (vector_is_imaginary(a) && (matrix_is_imaginary(b)))
    vector_subtract_ivector_multiply_imatrix(c, a, b);

  // for imaginary part (a->imaginary) * (b->imaginary) + (a->imaginary) * (b->real)
  if (vector_is_imaginary(a) && (matrix_is_imaginary(b))) {
    if (!vector_is_imaginary(c)) vector_attach_imaginary(c);
    ivector_add_ivector_multiply_matrix(c, a, b);
    ivector_add_vector_multiply_imatrix(c, a, b);
  }

  return c;
}

vector_t *vector_subtract_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), -=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *vector_subtract_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), -=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *vector_subtract_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), -=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *vector_subtract_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(vector_get_buffer(c), vector_get_dimension(c), -=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_subtract_vector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), -=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_subtract_vector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), -=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_subtract_ivector_multiply_matrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), -=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  matrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *ivector_subtract_ivector_multiply_imatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  VECTOR_OPCODE_VECTOR_MUL_MATRIX(ivector_get_buffer(c), vector_get_dimension(c), -=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  imatrix_get_buffer(b), matrix_get_columns(b), matrix_get_rows(b));
  return c;
}

vector_t *cvector_subtract_cvector_multiply_cmatrix(vector_t *c, vector_t *a, matrix_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_dimension(c) == matrix_get_columns(b));
  assert(vector_get_dimension(a) == matrix_get_rows(b));

  // for real part (a->real) * (b->real) - (a->imaginary) * (b->imaginary)
  vector_subtract_vector_multiply_matrix(c, a, b);
  if (vector_is_imaginary(a) && matrix_is_imaginary(b))
    vector_add_ivector_multiply_imatrix(c, a, b);

  // for imaginary part (a->imaginary) * (b->imaginary) + (a->imaginary) * (b->real)
  if (vector_is_imaginary(a) && matrix_is_imaginary(b)) {
    if (!vector_is_imaginary(c)) vector_attach_imaginary(c);
    ivector_subtract_ivector_multiply_matrix(c, a, b);
    ivector_subtract_vector_multiply_imatrix(c, a, b);
  }

  return c;
}

#define MATRIX_OPCODE_VECTOR_MUL_VECTOR(cbuf, ccols, crows, opcode, abuf, alen, bbuf, blen) { \
    int i, j;								\
    assert(cbuf);							\
    assert(abuf);							\
    assert(bbuf);							\
    assert(ccols == blen);						\
    assert(crows == alen);						\
    for (i = 0; i < crows; i++) {					\
      for (j = 0; j < ccols; j++) {					\
	*(cbuf+i*(ccols)+j) opcode (*(abuf+i))*(*(bbuf+j));		\
      }									\
    }									\
  }

matrix_t *matrix_copy_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  vector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *matrix_copy_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  vector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *matrix_copy_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *matrix_copy_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_copy_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  vector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_copy_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  vector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_copy_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_copy_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), =,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *cmatrix_copy_cvector_multiply_cvector(matrix_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);

  // for real part
  matrix_copy_vector_multiply_vector(c, a, b);
  if (vector_is_imaginary(a) && vector_is_imaginary(b))
    matrix_subtract_ivector_multiply_ivector(c, a, b);

  // for imaginary part
  if (vector_is_imaginary(a) && vector_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_copy_ivector_multiply_vector(c, a, b);
    imatrix_add_vector_multiply_ivector(c, a, b);
  } else {
    if (matrix_is_imaginary(c))
      imatrix_bezero(c, 0, 0, matrix_get_columns(c), matrix_get_rows(c));
  }

  return c;
}

matrix_t *matrix_add_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *matrix_add_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *matrix_add_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *matrix_add_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_add_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_add_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_add_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_add_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), +=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *cmatrix_add_cvector_multiply_cvector(matrix_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);

  // for real part
  matrix_add_vector_multiply_vector(c, a, b);
  if (vector_is_imaginary(a) && vector_is_imaginary(b))
    matrix_subtract_ivector_multiply_ivector(c, a, b);

  // for imaginary part
  if (vector_is_imaginary(a) && vector_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_add_ivector_multiply_vector(c, a, b);
    imatrix_add_vector_multiply_ivector(c, a, b);
  }

  return c;
}

matrix_t *matrix_subtract_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *matrix_subtract_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *matrix_subtract_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *matrix_subtract_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(matrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_subtract_vector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_subtract_vector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_subtract_ivector_multiply_vector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  vector_get_buffer(a), vector_get_dimension(a),
				  vector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *imatrix_subtract_ivector_multiply_ivector(matrix_t *c, vector_t *a, vector_t *b)
{
  MATRIX_OPCODE_VECTOR_MUL_VECTOR(imatrix_get_buffer(c), matrix_get_columns(c), matrix_get_rows(c), -=,
				  ivector_get_buffer(a), vector_get_dimension(a),
				  ivector_get_buffer(b), vector_get_dimension(b));
  return c;
}

matrix_t *cmatrix_subtract_cvector_multiply_cvector(matrix_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);

  // for real part
  matrix_subtract_vector_multiply_vector(c, a, b);
  if (vector_is_imaginary(a) && vector_is_imaginary(b))
    matrix_add_ivector_multiply_ivector(c, a, b);

  // for imaginary part
  if (vector_is_imaginary(a) && vector_is_imaginary(b)) {
    if (!matrix_is_imaginary(c)) matrix_attach_imaginary(c);
    imatrix_subtract_ivector_multiply_vector(c, a, b);
    imatrix_subtract_vector_multiply_ivector(c, a, b);
  }

  return c;
}

matrix_t *matrix_new_and_copy_vector_multiply_vector(vector_t *a, vector_t *b)
{
  matrix_t *c;

  assert(a);
  assert(b);

  c = matrix_new(vector_get_dimension(b), vector_get_dimension(a), false);
  matrix_copy_vector_multiply_vector(c, a, b);

  return c;
}

matrix_t *cmatrix_new_and_copy_cvector_multiply_cvector(vector_t *a, vector_t *b)
{
  matrix_t *c;

  assert(a);
  assert(b);

  c = matrix_new(vector_get_dimension(b), vector_get_dimension(a), false);
  cmatrix_copy_cvector_multiply_cvector(c, a, b);

  return c;
}

real_t matrix_get_trace(matrix_t *m)
{
  int i, n;
  real_t tr, *buf;

  assert(m);
  assert(matrix_is_square(m));

  n = matrix_get_columns(m);
  buf = matrix_get_buffer(m);

  for (tr = 0.0, i = 0; i < n; i++) {
    tr += *(buf + i);
    buf += n;
  }

  return tr;
}

complex_t cmatrix_get_trace(matrix_t *m)
{
  int i, n;
  real_t *real, *imag;
  complex_t tr;

  assert(m);
  assert(matrix_is_square(m));
  assert(matrix_is_imaginary(m));

  n = matrix_get_columns(m);
  real = matrix_get_buffer(m);
  imag = imatrix_get_buffer(m);

  for (tr.real = tr.imag = 0.0, i = 0; i < n; i++) {
    tr.real += *(real + i);
    tr.imag += *(imag + i);
    real += n;
    imag += n;
  }
  return tr;
}

static void matrix_determinant(real_t *det, matrix_t *mat)
{
  int i, columns, rows;
  matrix_t *minor;
  real_t value;

  columns = matrix_get_columns(mat);
  rows = matrix_get_rows(mat);

  if (columns > 2) {
    *det = 0.0;
    minor = matrix_new(columns - 1, rows - 1, false);
    for (i = 0; i < columns; i++) {
      matrix_copy_matrix_minor(minor, mat, i, 0);
      matrix_determinant(&value, minor);
      *det += matrix_get_value(mat, i, 0) * value * pow(-1.0, 2.0 + i);
    }
    matrix_destroy(minor);
  } else if (columns  == 2) {
    *det = matrix_get_value(mat, 0, 0) * matrix_get_value(mat, 1, 1) -
      matrix_get_value(mat, 1, 0) * matrix_get_value(mat, 0, 1);
  }
  /* else { // mat->columns == 1
    *det = matrix_get_value(0, 0, mat);
  }
  */
}

real_t matrix_get_determinant(matrix_t *mat)
{
  real_t det;

  assert(mat);
  assert(matrix_is_square(mat)); // square matrix
  
  matrix_determinant(&det, mat);

  return det;
}

static void cmatrix_determinant(complex_t *det, matrix_t *p)
{
  int i, columns, rows;
  complex_t value;
  matrix_t *minor;

  columns = matrix_get_columns(p);
  rows = matrix_get_rows(p);

  if (columns > 2) {
    det->real = 0.0, det->imag = 0.0;
    minor = matrix_new(columns - 1, rows - 1, true);
    for (i = 0; i < columns; i++) {
      cmatrix_copy_cmatrix_minor(minor, p, i, 0);
      cmatrix_determinant(&value, minor);
      det->real += (matrix_get_value(p, i, 0) * value.real - imatrix_get_value(p, i, 0) * value.imag) * pow(-1.0, 2.0 + i);
      det->imag += (matrix_get_value(p, i, 0) * value.imag + imatrix_get_value(p, i, 0) * value.real) * pow(-1.0, 2.0 + i);
      /*
      det->real += ((*(p->real+i)) * value.real - (*(p->imaginary+i)) * value.imag) * pow(-1.0, 2.0 + i);
      det->imag += ((*(p->real+i)) * value.imag + (*(p->imaginary+i)) * value.real) * pow(-1.0, 2.0 + i);
      */
    }
    matrix_destroy(minor);
  } else if (columns  == 2) {
    det->real = (matrix_get_value(p, 0, 0) * matrix_get_value(p, 1, 1) - imatrix_get_value(p, 0, 0) * imatrix_get_value(p, 1, 1)) - 
      (matrix_get_value(p, 1, 0) * matrix_get_value(p, 0, 1) - imatrix_get_value(p, 1, 0) * imatrix_get_value(p, 0, 1));

    det->imag = (matrix_get_value(p, 0, 0) * imatrix_get_value(p, 1, 1) + imatrix_get_value(p, 0, 0) * matrix_get_value(p, 1, 1)) -
      (matrix_get_value(p, 1, 0) * imatrix_get_value(p, 0, 1) + imatrix_get_value(p, 1, 0) * matrix_get_value(p, 0, 1));
    /*
    det->real = ((*(p->real)) * (*(p->real+matrix_get_columns(p)+1)) - (*(p->imaginary)) * (*(p->imaginary+matrix_get_columns(p) + 1)))
      -((*(p->real+1)) * (*(p->real+matrix_get_columns(p))) - (*(p->imaginary+1)) * (*(p->imaginary+matrix_get_columns(p))));

    det->imag = ((*(p->real)) * (*(p->imaginary+matrix_get_columns(p)+1)) + (*(p->imaginary)) * (*(p->real+matrix_get_columns(p) + 1)))
      -((*(p->real+1)) * (*(p->imaginary+matrix_get_columns(p))) + (*(p->imaginary+1)) * (*(p->real+matrix_get_columns(p))));
    */
  }
  /* else {
    det->real = *(p->real);
    det->imag = *(p->imaginary);
  }
  */
}

complex_t cmatrix_get_determinant(matrix_t *mat)
{
  complex_t det;

  assert(mat);
  assert(matrix_is_imaginary(mat));
  assert(matrix_is_square(mat)); // square matrix

  cmatrix_determinant(&det, mat);

  return det;
}

matrix_t *matrix_copy_matrix_cofactor(matrix_t *q, matrix_t *p)
{
  int c, r, columns, rows;
  matrix_t *minor;
  real_t det;

  assert(q);
  assert(matrix_is_square(q)); // square matrix
  assert(p);
  assert(matrix_are_matched(q, p));

  columns = matrix_get_columns(p);
  rows = matrix_get_rows(p);
  if (columns > 2) {
    minor = matrix_new(columns - 1, rows - 1, false);
    for (r = 0; r < rows; r++) {
      for (c = 0; c < columns; c++) {
	matrix_copy_matrix_minor(minor, p, c, r);
	matrix_determinant(&det, minor);
	matrix_put_value(det * pow(-1, 2 + c + r), q, c, r);
	//*(matrix_get_buffer(q)+r*matrix_get_columns(q)+c) = pow(-1.0, 2.0+c+r)*det;
      }
    }
    matrix_destroy(minor);
  } else if (columns == 2) {
    /*  x  y  =>  w -z
	z  w     -y  x */
    matrix_put_value(matrix_get_value(p, 1, 1), q, 0, 0);
    matrix_put_value(-1 * matrix_get_value(p, 0, 1), q, 1, 0);
    matrix_put_value(-1 * matrix_get_value(p, 1, 0), q, 0, 1);
    matrix_put_value(matrix_get_value(p, 0, 0), q, 1, 1);
    /*
    *(matrix_get_buffer(q)) = *(p->real+matrix_get_columns(p)+1);
    *(matrix_get_buffer(q)+1) = -(*(p->real+matrix_get_columns(p)));
    *(matrix_get_buffer(q)+matrix_get_columns(q)) = -(*(p->real+1));
    *(matrix_get_buffer(q)+matrix_get_columns(q)+1) = *(p->real);
    */
  } else if (columns == 1) {
    matrix_put_value(matrix_get_value(p, 0, 0), q, 0, 0);
  } else {
    assert(0);
  }

  return q;
}

matrix_t *cmatrix_copy_cmatrix_cofactor(matrix_t *q, matrix_t *p)
{
  int c, r, columns, rows;
  complex_t det;
  matrix_t *minor;

  assert(q);
  assert(matrix_is_imaginary(q));
  assert(matrix_is_square(q)); // square matrix
  assert(p);
  assert(matrix_is_imaginary(p));
  assert(matrix_are_matched(q, p));

  if (!matrix_is_imaginary(q)) 
    matrix_attach_imaginary(q);

  columns = matrix_get_columns(q);
  rows = matrix_get_rows(q);
  if (columns > 2) {
    minor = matrix_new(columns - 1, rows - 1, true);
    for (r = 0; r < rows; r++) {
      for (c = 0; c < columns; c++) {
	cmatrix_copy_cmatrix_minor(minor, p, c, r);
	cmatrix_determinant(&det, minor);
	matrix_put_value(pow(-1, 2 + c + r) * det.real, q, c, r);
	imatrix_put_value(pow(-1, 2 + c + r) * det.imag, q, c, r);
	/*
	*(matrix_get_buffer(q)+r*matrix_get_columns(q)+c) = pow(-1.0, 2.0+c+r)*det.real;
	*(imatrix_get_buffer(q)+r*matrix_get_columns(q)+c) = pow(-1.0, 2.0+c+r)*det.imag;
	*/
      }
    }
    matrix_destroy(minor);
  } else if (columns == 2) {
    /* x y  =>  w -z
       z w     -y  x */
    matrix_put_value(matrix_get_value(p, 1, 1), q, 0, 0);
    imatrix_put_value(imatrix_get_value(p, 1, 1), q, 0, 0);
    matrix_put_value(-1 * matrix_get_value(p, 0, 1), q, 1, 0);
    imatrix_put_value(-1 * imatrix_get_value(p, 0, 1), q, 1, 0);
    matrix_put_value(-1 * matrix_get_value(p, 1, 0), q, 0, 1);
    imatrix_put_value(-1 * imatrix_get_value(p, 1, 0), q, 0, 1);
    matrix_put_value(matrix_get_value(p, 0, 0), q, 1, 1);
    imatrix_put_value(imatrix_get_value(p, 0, 0), q, 1, 1);
    /*
    *(matrix_get_buffer(q)) = *(p->real+matrix_get_columns(p)+1);
    *(imatrix_get_buffer(q)) = *(p->imaginary+matrix_get_columns(p)+1);
    *(matrix_get_buffer(q)+1) = -(*(p->real+matrix_get_columns(p)));
    *(imatrix_get_buffer(q)+1) = -(*(p->imaginary+matrix_get_columns(p)));
    *(matrix_get_buffer(q)+matrix_get_columns(q)) = -(*(p->real+1));
    *(imatrix_get_buffer(q)+matrix_get_columns(q)) = -(*(p->imaginary+1));
    *(matrix_get_buffer(q)+matrix_get_columns(q)+1) = *(p->real);
    *(imatrix_get_buffer(q)+matrix_get_columns(q)+1) = *(p->imaginary);
    */
  }
  /* else {
    *(matrix_get_buffer(q)) = *(p->real);
    *(imatrix_get_buffer(q)) = *(p->imaginary);
  }
  */
  return q;
}

matrix_t *matrix_copy_matrix_adjugate(matrix_t *q, matrix_t *p)
{
  matrix_t *m;

  assert(q);
  assert(matrix_is_square(q)); // square matrix
  assert(p);
  assert(matrix_are_matched(q, p));

  m = matrix_new_and_copy(p);
  matrix_copy_matrix_cofactor(m, p);
  matrix_copy_matrix_transpose(q, m);
  matrix_destroy(m);

  return q;
}

matrix_t *cmatrix_copy_cmatrix_adjugate(matrix_t *q, matrix_t *p)
{
  matrix_t *m;

  assert(q);
  assert(imatrix_get_buffer(q));
  assert(matrix_get_columns(q) == matrix_get_rows(q)); // square matrix
  assert(p);
  assert(p->imaginary);
  assert(matrix_get_columns(q) == matrix_get_columns(p));
  assert(matrix_get_rows(q) == matrix_get_rows(p));

  m = matrix_new_and_copy(p);
  cmatrix_copy_cmatrix_cofactor(m, p);
  cmatrix_copy_cmatrix_transpose(q, m);
  matrix_destroy(m);

  return q;
}

matrix_t *matrix_copy_matrix_adjoint(matrix_t *q, matrix_t *p)
{
  assert(q);
  assert(matrix_get_columns(q) == matrix_get_rows(q));
  assert(p);
  assert(matrix_get_columns(p) == matrix_get_columns(q));
  assert(matrix_get_rows(p) == matrix_get_rows(q));

  matrix_copy_matrix_adjugate(q, p);

  return q;
}
	
matrix_t * cmatrix_copy_cmatrix_adjoint(matrix_t *q, matrix_t *p)
{
  assert(q);
  assert(imatrix_get_buffer(q));
  assert(matrix_get_columns(q) == matrix_get_rows(q));
  assert(p);
  assert(p->imaginary);
  assert(matrix_get_columns(p) == matrix_get_columns(q));
  assert(matrix_get_rows(p) == matrix_get_rows(q));

  cmatrix_copy_cmatrix_adjugate(q, p);

  return q;
}

int matrix_copy_matrix_inverse(matrix_t *q, matrix_t *p)
{
  //real_t real;
  matrix_t *m;

  assert(q);
  assert(matrix_get_columns(q) == matrix_get_rows(q));
  assert(p);
  assert(matrix_get_columns(p) == matrix_get_columns(q));
  assert(matrix_get_rows(p) == matrix_get_rows(q));

#ifdef USE_INVERSE_BY_GAUSS_JORDAN_ELIMINATE
  matrix_determinant(&real, p);
  if (abs(real) < REAL_EPSILON) return -1;
  m = matrix_new_and_copy(p);
  matrix_copy_matrix_cofactor(m, p);
  matrix_copy_matrix_transpose(q, m);
  matrix_divide_scalar(q, real);
  matrix_destroy(m);
#else
  m = matrix_new(2 * matrix_get_columns(p), matrix_get_rows(p), false);
  matrix_identify(q);
  matrix_copy_matrix(m, 0, 0, p, 0, 0, matrix_get_columns(p), matrix_get_rows(p));
  matrix_copy_matrix(m, matrix_get_columns(p), 0, q, 0, 0, matrix_get_columns(q), matrix_get_rows(q));
  if (matrix_gauss_jordan_eliminate(m) != 0) {
    matrix_destroy(m);
    return -1;
  }
  matrix_copy_matrix(q, 0, 0, m, matrix_get_columns(p), 0, matrix_get_columns(q), matrix_get_rows(q));
  matrix_destroy(m);
#endif

  return 0;
}

int cmatrix_copy_cmatrix_inverse(matrix_t *q, matrix_t *p)
{
  complex_t det;
  matrix_t *m;

  assert(q);
  assert(imatrix_get_buffer(q));
  assert(matrix_get_columns(q) == matrix_get_rows(q));
  assert(p);
  assert(p->imaginary);
  assert(matrix_get_columns(p) == matrix_get_columns(q));
  assert(matrix_get_rows(p) == matrix_get_rows(q));

#ifndef USE_INVERSE_BY_GAUSS_JORDAN_ELIMINATE
  cmatrix_determinant(&det, p);
  if (abs(det.real) < REAL_EPSILON && abs(det.imag) < REAL_EPSILON) return -1;

  m = matrix_new_and_copy(p);
  cmatrix_copy_cmatrix_cofactor(m, p);
  cmatrix_copy_cmatrix_transpose(q, m);
  cmatrix_divide_scalar(q, det);
  matrix_destroy(m);
#else
  m = matrix_create(2*matrix_get_columns(p), matrix_get_rows(p), p->imaginary);
  matrix_be_identity(q);
  cmatrix_copy_cmatrix(m, 0, 0, p, 0, 0, matrix_get_columns(p), matrix_get_rows(p));
  cmatrix_copy_cmatrix(m, matrix_get_columns(p), 0, q, 0, 0, matrix_get_columns(q), matrix_get_rows(q));
  if (cmatrix_gauss_jordan_eliminate(m) != 0) {
    matrix_destroy(m);
    return -1;
  }
  cmatrix_copy_cmatrix(q, 0, 0, m, matrix_get_columns(p), 0, matrix_get_columns(q), matrix_get_rows(q));
  matrix_destroy(m);
#endif

  return 0;
}

matrix_t *matrix_new_and_copy_matrix_inverse(matrix_t *p)
{
  matrix_t *q;

  assert(p);
  assert(matrix_get_columns(p) == matrix_get_rows(p));

  q = matrix_new(matrix_get_columns(p), matrix_get_rows(p), false);
  matrix_copy_matrix_inverse(q, p);

  return q;	
}

matrix_t *cmatrix_new_and_copy_cmatrix_inverse(matrix_t *p)
{
  matrix_t *q;
  assert(p);
  assert(p->imaginary);
  assert(matrix_get_columns(p) == matrix_get_rows(p));
  q = matrix_new(matrix_get_columns(p), matrix_get_rows(p), true);
  cmatrix_copy_cmatrix_inverse(q, p);
  return q;	
}

#if 0
int least_squares_fitting(real_t *coeff, uint32_t m,
                          uint32_t *xcoord, uint32_t *ycoord, uint32_t n) {
  uint32_t i, j, k;
  real_t temp;
  real_t **matrix, **transpose, **square, **inverse;
    
  if (!(coeff && xcoord && ycoord && n >= m)) return 0;

  /* allocate matrix */
  matrix = (real_t **)calloc(n, sizeof(real_t *));
  transpose = (real_t **)calloc(n, sizeof(real_t *));
  square = (real_t **)calloc(n, sizeof(real_t *));
  inverse = (real_t **)calloc(n, sizeof(real_t *));

  for (i = 0; i < n; i++) {
    matrix[i] = (real_t *)calloc(n, sizeof(real_t));
    transpose[i] = (real_t *)calloc(n, sizeof(real_t));
    square[i] = (real_t *)calloc(n, sizeof(real_t));
    inverse[i] = (real_t *)calloc(n, sizeof(real_t));
    memset(matrix[i], 0, n*sizeof(real_t));
    memset(transpose[i], 0, n*sizeof(real_t));
    memset(square[i], 0, n*sizeof(real_t));
    memset(inverse[i], 0, n*sizeof(real_t));
  }

  /* load inputs */
  for (i = 0; i < n; i++) {
    for (temp = 1.0, j = 0; j < m; j++) {
      matrix[i][j] = temp;
      temp *= xcoord[i];
    }
  }

  /* transpose */
  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) {
      transpose[j][i] = matrix[i][j];
    }
  }
    
  /* multiply transpose and matrix */
  for (i = 0; i < m; i++) {
    for (j = 0; j < m; j++) {
      square[i][j] = 0.0;
      for (k = 0; k < n; k++)
	square[i][j] += transpose[i][k]*matrix[k][j];
    }
  }
    
  /* inverse of square matrix */
  if (inverse_of_square(inverse, square, m) == 0) return 0;

  /* multiply inverse and transpose */
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      for (matrix[i][j] = 0.0, k =0; k < m; k++)
	matrix[i][j] += inverse[i][k]*transpose[k][j];
    }
  }

  /* multiply_of_matrix the above matrix and ys matrix */
  for (i = 0; i < m; i++) {
    coeff[i] = 0;
    for (j = 0; j < n; j++) {
      coeff[i] += matrix[i][j]*ycoord[j];
    }
  }

  for (i = 0; i < m; i++) {
    free(matrix[i]);
    free(transpose[i]);
    free(square[i]);
    free(inverse[i]);
  }

  free(matrix);
  free(transpose);
  free(square);
  free(inverse);

  return 1;
}

int least_square_line(real_t *coeff, uint32_t *xcoord, uint32_t *ycoord, uint32_t n) {
  uint32_t i;
  real_t mx, my, ssx, sxy;

  if (!(coeff && xcoord && ycoord && n >= 2)) return 0;

  /* the means of xcoord and ycoord */
  mx = my = ssx = sxy = 0.0;
  for (i = 0; i < n; i++) {
    mx += xcoord[i];
    my += ycoord[i];
    ssx += (real_t)xcoord[i] * (real_t)xcoord[i];
    sxy += (real_t)xcoord[i] * (real_t)ycoord[i];
  }
  mx /= n; my /= n;

  /* the sample regresion coefficient */
  coeff[1] = (sxy - n*mx*my) / (ssx - n*mx*mx);
  coeff[0] = my-coeff[1]*mx;

  return 1;
}
#endif


