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
#include <math.h>
#include <malloc.h>
#include <assert.h>
#include <float.h>

#include <buffering/dlink.h>
#include <linear_algebra/complex2.h>
#include <linear_algebra/vector.h>
#include <linear_algebra/matrix.h>
#include <linear_algebra/vector_list.h>
//#include <gram_schmidt.h>

// orthonormalization
int vector_list_create_gram_schmidt_process(vector_list_t *q, vector_list_t *p)
{
  vector_t *u, *v, *u_curr, *v_curr;
  real_t c, d;
  dlink_t *a, *b;

  assert(q);
  assert(p);
  assert(vector_list_get_count(p) > 0);
  assert(vector_list_get_count(q) == 0);

  // Step 1. Let u(0) = v(0)
  a = p->tail->next;
  v = (vector_t *)a->object;

  u = vector_new_and_copy(v);
  vector_list_insert(u, q);

  for (a = a->next; a != p->head; a = a->next) {
    // u(k) = v(k) - proj_{u(0)}(v(k)) - .... - proj_{u(n)}(v(k))
    v_curr = (vector_t *)a->object;
    u_curr = vector_new_and_copy(v_curr);
    for (b = q->tail->next; b != q->head; b = b->next) {
      u = (vector_t *)b->object;
      // proj_u (v) = ((u * v) / (u * u)) * u
      vector_dotproduct_vector(&c, u, v_curr);
      vector_dotproduct_vector(&d, u, u);
      c /= d;
      vector_subtract_vector_multiply_scalar(u_curr, u, c);
    }

    if (vector_iszero(u_curr, 0, vector_get_dimension(u_curr))) {
      vector_destroy(u_curr);
      break;
    }

    vector_list_insert(u_curr, q);
  }

  // normalization
  for (b = q->tail->next; b != q->head; b = b->next) {
    u = (vector_t *)b->object;
    vector_divide_scalar(u, vector_get_norm(u));
  }

  return vector_list_get_count(q);
}

int cvector_list_create_gram_schmidt_process(vector_list_t *q, vector_list_t *p)
{
  vector_t *u, *v, *u_curr, *v_curr;
  complex_t c, d;
  dlink_t *a, *b;

  assert(q);
  assert(p);
  assert(vector_list_get_count(p) > 0);
  assert(vector_list_get_count(q) == 0);

  // Step 1. Let u0 = v0
  a = p->tail->next;
  v = (vector_t *)a->object;

  u = vector_new_and_copy(v);
  vector_list_insert(u, q);

  for (a = a->next; a != p->head; a = a->next) {
    v_curr = (vector_t *)a->object;
    u_curr = vector_new_and_copy(v_curr);
    for (b = q->tail->next; b != q->head; b = b->next) {
      u = (vector_t *)b->object;
      cvector_dotproduct_cvector(&c, u, v_curr);
      cvector_dotproduct_cvector(&d, u, u);
      //norm = cvector_get_norm(v);
      complex_divide_complex(&c, &d);
      cvector_subtract_cvector_multiply_scalar(u_curr, u, c);
    }
    if (cvector_iszero(u_curr, 0, vector_get_dimension(u_curr))) {
      vector_destroy(u_curr);
      break;
    }
    vector_list_insert(u_curr, q);
  }

  // normalization
  for (b = q->tail->next; b != q->head; b = b->next) {
    u = (vector_t *)b->object;
    d = cvector_get_norm(u);
    cvector_divide_scalar(u, d);
  }

  return vector_list_get_count(q);
}

// m : h x w (h: dimension,  w: tries)
static int matrix_self_gram_schmidt_process(matrix_t *m, int x, int y, int w, int h)
{
  int i, j, k;
  real_t u, v, *u_vec;
  real_t c, d;

  u_vec = (real_t *)malloc(h * sizeof(real_t));

  // Step 1. Let U0 = V0
  // nothing to do

  for (i = x + 1; i < x + w; i++) {
    // Step 2.
    // U(i) = V(i) - proj_{U(0)}(V(i)) - proj_{U(1)}(V(i)) - ....
    // proj_{U}(V) = (<U,V> / <U,U>) * U

    // U(i) = V(i)
    for (k = 0; k < h; k++) u_vec[k] = matrix_get_value(m, i, y + k);

    for (j = x; j < i; j++) {
      // U(i) -= proj_{U(j)}(V(i))
      c = d = 0;
      for (k = y; k < y + h; k++) {
	v = matrix_get_value(m, i, k);
	u = matrix_get_value(m, j, k);
	c += u * v;
	d += u * u;
      }
      c /= d;
      for (k = y; k < y + h; k++) {
	u = matrix_get_value(m, j, k);
	u_vec[k - y] -= c * u;
      }
    }

    for (k = 0; k < h; k++) matrix_put_value(u_vec[k], m, i, y + k);

    if (matrix_is_zero(m, i, y, 1, h)) break;
  }


  free(u_vec);

  // normalization
  for (j = x; j < i; j++) {
    d = 0;
    for (k = 0; k < h; k++) {
      u = matrix_get_value(m, j, y + k);
      d += u * u;
    }
    d = sqrt(d);
    matrix_divide_scalar_on_region(m, d, j, y, 1, h);
  }

  return i - x;
}


matrix_t *matrix_new_and_gram_schmidt_process(matrix_t *p)
{
  int n;
  matrix_t *tmp, *q;

  assert(p);

  tmp = matrix_new_and_copy(p);
  n = matrix_self_gram_schmidt_process(tmp, 0, 0, matrix_get_columns(tmp), matrix_get_rows(tmp));

  if (n == matrix_get_columns(tmp)) return tmp;

  q = matrix_new(n, matrix_get_rows(tmp), false);
  matrix_copy_matrix(q, 0, 0, tmp, 0, 0, n, matrix_get_rows(tmp));

  matrix_destroy(tmp);

  return q;
}

static int cmatrix_self_gram_schmidt_process(matrix_t *m, int x, int y, int w, int h)
{
  int i, j, k;
  complex_t u, v, *u_vec;
  complex_t c, d, tmp;

  u_vec = (complex_t *)malloc(h * sizeof(complex_t));

  // Step 1. Let U0 = V0
  // nothing to do

  for (i = x + 1; i < x + w; i++) {
    // U(i) = V(i) - proj_{U(0)}(V(i)) - proj_{U(2)}(V(i)) - ....
    // proj_{U}(V) = ((U * V) / (U * U)) * U

    for (k = 0; k < h; k++) cmatrix_read_value(&u_vec[k], m, i, y + k);

    for (j = x; j < i; j++) {
      c.real = c.imag = d.real = d.imag = 0;
      for (k = y; k < y + h; k++) {
	cmatrix_read_value(&v, m, i, k);
	cmatrix_read_value(&u, m, j, k);
	complex_add_complex_multiply_complex(&c, &u, complex_conjugate(&tmp, &v));
	complex_add_complex_multiply_complex(&d, &u, complex_conjugate(&tmp, &u));
      }
      complex_divide_complex(&c, &d);
      for (k = y; k < y + h; k++) {
	cmatrix_read_value(&u, m, j, k);
	complex_subtract_complex_multiply_complex(&u_vec[k-y], &u, &c);
      }
    }

    for (k = 0; k < h; k++) cmatrix_put_value(u_vec[k], m, i, y + k);

    if (cmatrix_is_zero(m, i, y, 1, y +h)) break;
  }

  free(u_vec);

  // normalization
  for (j = x; j < i; j++) {
    d.real = d.imag = 0;
    for (k = y; k < y + h; k++) {
      cmatrix_read_value(&u, m, j, k);
      complex_add_complex_multiply_complex(&d, &u, complex_conjugate(&tmp, &u));
    }
    complex_copy_complex_sqrt(&c, &d);
    for (k = y; k < y + h; k++) {
      cmatrix_read_value(&u, m, j, k);
      complex_divide_complex(&u, &c);
      cmatrix_put_value(u, m, j, k);
    }
  }

  return i - x;
}

matrix_t *cmatrix_new_and_gram_schmidt_process(matrix_t *p)
{
  int n;
  matrix_t *q, *tmp;

  assert(p);
  assert(matrix_is_imaginary(p));

  tmp = matrix_new_and_copy(p);
  n = cmatrix_self_gram_schmidt_process(tmp, 0, 0, matrix_get_columns(tmp), matrix_get_rows(tmp));

  if (n == matrix_get_columns(tmp)) return tmp;

  q = matrix_new(n, matrix_get_rows(tmp), true);
  cmatrix_copy_cmatrix(q, 0, 0, tmp, 0, 0, n, matrix_get_rows(tmp));

  matrix_destroy(tmp);

  return q;
}

int matrix_gram_schmidt_process(matrix_t *q, matrix_t *p)
{
  int n;
  matrix_t *tmp;

  assert(q);
  assert(p);
  assert(matrix_get_columns(q) >= matrix_get_columns(p));
  assert(matrix_get_rows(q) >= matrix_get_rows(p));

  tmp = matrix_new_and_copy(p);
  n = matrix_self_gram_schmidt_process(tmp, 0, 0, matrix_get_columns(tmp), matrix_get_rows(tmp));

  matrix_copy_matrix(q, 0, 0, tmp, 0, 0, n, matrix_get_rows(tmp));
  matrix_destroy(tmp);

  return n;
}

int cmatrix_gram_schmidt_process(matrix_t *q, matrix_t *p)
{
  int n;
  matrix_t *tmp;

  assert(q);
  assert(matrix_is_imaginary(q));
  assert(p);
  assert(matrix_is_imaginary(p));
  assert(matrix_get_columns(q) >= matrix_get_columns(p));
  assert(matrix_get_rows(q) >= matrix_get_rows(p));

  tmp = matrix_new_and_copy(p);
  n = cmatrix_self_gram_schmidt_process(tmp, 0, 0, matrix_get_columns(tmp), matrix_get_rows(tmp));

  cmatrix_copy_cmatrix(q, 0, 0, tmp, 0, 0, n, matrix_get_rows(tmp));
  matrix_destroy(tmp);

  return n;
}
