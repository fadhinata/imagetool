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
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include <common.h>
#include <wavelet/haar.h>

#define H0 0.7071067810
#define H1 0.7071067810

#define SQRT2 1.4142135623

static void self_haar_fwt(vector_t *q)
{
  int n, m, i;
  real_t *temp;

  for (m = 0, n = 1; n < vector_get_dimension(q); n <<= 1) m++;

  temp = (real_t *)malloc(n * sizeof(real_t));
  assert(temp);

  while (n > 1) {
    for (i = 0; i < n / 2; i++) {
      temp[i]     = (vector_get_value(q, 2*i) + vector_get_value(q, 2*i + 1)) / SQRT2;
      temp[n/2+i] = (vector_get_value(q, 2*i) - vector_get_value(q, 2*i + 1)) / SQRT2;
    }
    for (i = 0; i < n; i++) vector_put_value(temp[i], q, i);
    n /= 2;
  }
  free(temp);
}

// haar fast wavelet transform
void haar_fwt(vector_t *q, vector_t *p)
{
  int n, m;

  assert(q);
  assert(p);

  for (m = 0, n = 1; n < vector_get_dimension(p); n <<= 1) m++;

  assert(n == vector_get_dimension(p));
  assert(n == vector_get_dimension(q));

  vector_copy(q, p);
  self_haar_fwt(q);
}

static void self_haar_ifwt(vector_t *q)
{
  int n, m, i, k;
  real_t *temp;
	
  for (m = 0, n = 1; n < vector_get_dimension(q); n <<= 1) m++;

  temp = (real_t *)malloc(n * sizeof(real_t));
  assert(temp);

  k = 1;
  while (k < n) {
    k = 2 * k;
    for (i = 0; i < k / 2; i++) {
      temp[2*i]   = vector_get_value(q, i) + vector_get_value(q, k/2 + i);
      temp[2*i+1] = vector_get_value(q, i) - vector_get_value(q, k/2 + i);
    }
    for (i = 0; i < k; i++) vector_put_value(temp[i], q, i);
  }
  free(temp);
}

// haar inverse fast wavelet transform
void haar_ifwt(vector_t *q, vector_t *p)
{
  int n, m;
	
  assert(q);
  assert(p);
	
  for (m = 0, n = 1; n < vector_get_dimension(p); n <<= 1) m++;

  assert(n == vector_get_dimension(p));
  assert(n == vector_get_dimension(q));
	
  vector_copy(q, p);
  self_haar_ifwt(q);
}

void haar_fwt2(matrix_t *q, matrix_t *p)
{
  int i, j, k, n, m;
  real_t *temp, *buffer;
  vector_t *v;

  assert(q);
  assert(p);
	
  for (m = 1; m < matrix_get_columns(p); m <<= 1);
  assert(m == matrix_get_columns(p));
  assert(m == matrix_get_columns(q));
	
  for (n = 1; n < matrix_get_rows(p); n <<= 1);
  assert(n == matrix_get_rows(p));
  assert(n == matrix_get_rows(q));
	
  matrix_copy(q, p);

  // Transform the rows
  v = vector_new(m, false);
  for (i = 0; i < n; i++) {
    vector_copy_row_vector_of_matrix(v, q, 0, i);
    self_haar_fwt(v);
    matrix_copy_row_vector(q, 0, i, v);
  }
  vector_destroy(v);

  // Transform the columns
  v = vector_new(n, false);
  for (j = 0; j < m; j++) {
    vector_copy_column_vector_of_matrix(v, q, j, 0);
    self_haar_fwt(v);
    matrix_copy_column_vector(q, j, 0, v);
  }
  vector_destroy(v);
}

void haar_ifwt2(matrix_t *q, matrix_t *p)
{
  int i, j, k, n, m;
  vector_t *v;
  real_t *temp, *buffer;

  assert(q);
  assert(p);
	
  for (m = 1; m < matrix_get_columns(p); m <<= 1);

  assert(m == matrix_get_columns(p));
  assert(m == matrix_get_columns(q));
	
  for (n = 1; n < matrix_get_rows(p); n <<= 1);

  assert(n == matrix_get_rows(p));
  assert(n == matrix_get_rows(q));
	
  matrix_copy(q, p);

  // Transform the columns
  v = vector_new(n, false);
  for (j = 0; j < m; j++) {
    vector_copy_column_vector_of_matrix(v, q, j, 0);
    self_haar_fwt(v);
    matrix_copy_column_vector(q, j, 0, v);
  }
  vector_destroy(v);
	
  // Transform the rows
  v = vector_new(m, false);
  for (i = 0; i < n; i++) {
    vector_copy_row_vector_of_matrix(v, q, 0, i);
    self_haar_fwt(v);
    matrix_copy_row_vector(q, 0, i, v);
  }
  vector_destroy(v);
}
