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
#include <assert.h>
#include <float.h>

#include <buffering/dlink.h>
#include <linear_algebra/complex2.h>
#include <linear_algebra/vector.h>
#include <linear_algebra/matrix.h>
#include <linear_algebra/vector_list.h>
//#include <gram_schmidt.h>

int vector_list_create_gram_schmidt_process(vector_list_t *q, vector_list_t *p)
{
  int n, i, j;
  vector_t *u, *v, *vnext;
  real_t c;
  dlink_t *a, *b;

  assert(q);
  assert(p);
  assert(vector_list_get_count(p) > 0);
  assert(vector_list_get_count(q) == 0);

  // Step 1. Let v1 = u1
  a = p->tail->next;
  u = (vector_t *)a->object;
  v = vector_new_and_copy(u);
  vector_list_insert(v, q);

  for (a = a->next; a != p->head; a = a->next) {
    u = (vector_t *)a->object;
    vnext = vector_new_and_copy(u);
    for (b = q->tail->next; b != q->head; b = b->next) {
      v = (vector_t *)b->object;
      vector_dotproduct_vector(&c, u, v);
      c /= vector_get_norm(v);
      vector_subtract_vector_multiply_scalar(vnext, v, c);
    }
    if (vector_iszero(vnext, 0, vector_get_dimension(vnext))) {
      vector_destroy(vnext);
      break;
    }
    vector_list_insert(vnext, q);
  }

  for (b = q->tail->next; b != q->head; b = b->next) {
    v = (vector_t *)b->object;
    vector_divide_scalar(v, vector_get_norm(v));
  }

  return vector_list_get_count(q);
}

int cvector_list_create_gram_schmidt_process(vector_list_t *q, vector_list_t *p)
{
  int i, j, n;
  vector_t *u, *v, *vnext;
  complex_t c, norm;
  dlink_t *a, *b;

  assert(q);
  assert(p);
  assert(vector_list_get_count(p) > 0);
  assert(vector_list_get_count(q) == 0);

  // Step 1. Let v1 = u1
  a = p->tail->next;
  u = (vector_t *)a->object;
  v = vector_new_and_copy(u);
  vector_list_insert(v, q);

  for (a = a->next; a != p->head; a = a->next) {
    u = (vector_t *)a->object;
    vnext = vector_new_and_copy(u);
    for (b = q->tail->next; b != q->head; b = b->next) {
      v = (vector_t *)b->object;
      cvector_dotproduct_cvector(&c, u, v);
      norm = cvector_get_norm(v);
      complex_divide_complex(&c, &norm);
      cvector_subtract_cvector_multiply_scalar(vnext, v, c);
    }
    if (cvector_iszero(vnext, 0, vector_get_dimension(vnext))) {
      vector_destroy(vnext);
      break;
    }
    vector_list_insert(vnext, q);
  }

  for (b = q->tail->next; b != q->head; b = b->next) {
    v = (vector_t *)b->object;
    norm = cvector_get_norm(v);
    complex_copy_complex_sqrt(&c, &norm);
    cvector_divide_scalar(v, c);
  }

  return vector_list_get_count(q);
}

/* n * basis -> (n or below n) * orthonormal basis */
int vector_gram_schmidt_process(vector_t **q, vector_t **p, int n)
{
  int i, j;
  vector_t *u, *v, *vnext;
  real_t c, norm;

  assert(q);
  assert(p);
  assert(vector_get_dimension(*q) == vector_get_dimension(*p));

  // Step 1. Let v1 = u1
  v = *(q + 0), u = *(p + 0);
  vector_copy_vector(v, 0, u, 0, vector_get_dimension(u));
  for (i = 1; i < n; i++) {
    vnext = *(q+i);
    u = *(p+i);
    vector_copy_vector(vnext, 0, u, 0, vector_get_dimension(u));
    for (j = 0; j < i; j++) {
      v = *(q+j);
      vector_dotproduct_vector(&c, u, v);
      norm = vector_get_norm(v);
      c /= norm;
      //complex_divide_complex(c, norm);
      vector_subtract_vector_multiply_scalar(vnext, v, c);
    }
    if (vector_iszero(vnext, 0, vector_get_dimension(vnext))) break;
  }
  for (j = 0; j < i; j++) {
    v = *(q+j);
    norm = vector_get_norm(v);
    vector_divide_scalar(v, sqrt(norm));
  }
  return i;
}

int cvector_gram_schmidt_process(vector_t **q, vector_t **p, int n)
{
  int i, j;
  vector_t *u, *v, *vnext;
  complex_t c, norm;
  assert(q);
  assert(p);
  assert(vector_get_dimension(*q) == vector_get_dimension(*p));
  // Step 1. Let v1 = u1
  v = *(q+0), u = *(p+0);
  cvector_copy_cvector(v, 0, u, 0, vector_get_dimension(u));
  for (i = 1; i < n; i++) {
    vnext = *(q+i);
    u = *(p+i);
    cvector_copy_cvector(vnext, 0, u, 0, vector_get_dimension(u));
    for (j = 0; j < i; j++) {
      v = *(q+j);
      cvector_dotproduct_cvector(&c, u, v);
      norm = cvector_get_norm(v);
      complex_divide_complex(&c, &norm);
      cvector_subtract_cvector_multiply_scalar(vnext, v, c);
    }
    if (cvector_iszero(vnext, 0, vector_get_dimension(vnext))) break;
  }
  for (j = 0; j < i; j++) {
    v = *(q+j);
    norm = cvector_get_norm(v);
    complex_copy_complex_sqrt(&c, &norm);
    cvector_divide_scalar(v, c);
  }
  return i;
}

matrix_t *matrix_new_and_gram_schmidt_process(matrix_t *p)
{
  int i, j, k;
  real_t u, v, vnext;
  real_t c, norm;
  matrix_t *tmp, *q;

  assert(p);

  tmp = matrix_new_and_copy(p);

  // Step 1. Let v1 = u1
  for (i = 1; i < matrix_get_columns(p); i++) {
    for (j = 0; j < i; j++) {
      c = norm = 0.0;
      for (k = 0; k < matrix_get_rows(p); k++) {
	v = matrix_get_value(tmp, j, k);//MATRIX_GET(v, q, j, k);
	u = matrix_get_value(tmp, i, k);//MATRIX_GET(u, p, i, k);
	c += v*u;
	norm += v*v;
      }
      c /= norm;
      for (k = 0; k < matrix_get_rows(tmp); k++) {
	v = matrix_get_value(tmp, j, k);//MATRIX_GET(v, q, j, k);
	vnext = matrix_get_value(tmp, i, k);//MATRIX_GET(vnext, q, i, k);
	vnext -= v * c;
	matrix_put_value(vnext, tmp, i, k);//MATRIX_PUT(vnext, q, i, k);
      }
    }

    for (k = 0; k < matrix_get_rows(tmp); k++) {
      vnext = matrix_get_value(tmp, i, k);//MATRIX_GET(vnext, q, i, k);
      if (!(vnext < DBL_EPSILON)) break; // non-zero
    }
    if (k >= matrix_get_rows(tmp)) break;
  }

  for (j = 0; j < i; j++) {
    norm = 0.0;
    for (k = 0; k < matrix_get_rows(tmp); k++) {
      v = matrix_get_value(tmp, j, k);//MATRIX_GET(v, q, j, k);
      norm += v*v;
    }
    norm = sqrt(norm);
    for (k = 0; k < matrix_get_rows(tmp); k++) {
      v = matrix_get_value(tmp, j, k);//MATRIX_GET(v, q, j, k);
      v /= norm;
      matrix_put_value(v, tmp, j, k);//MATRIX_PUT(v, q, j, k);
    }
  }

  q = matrix_new(i, matrix_get_rows(tmp), false);
  matrix_copy_matrix(q, 0, 0, tmp, 0, 0, i, matrix_get_rows(tmp));

  matrix_destroy(tmp);

  return q;
}

matrix_t *cmatrix_new_and_gram_schmidt_process(matrix_t *p)
{
  int i, j, k;
  complex_t u, v, vnext;
  complex_t c, norm;
  matrix_t *q, *tmp;

  assert(p);
  assert(matrix_is_imaginary(p));

  tmp = matrix_new_and_copy(p);

  // Step 1. Let v1 = u1
  for (i = 1; i < matrix_get_columns(p); i++) {
    for (j = 0; j < i; j++) {
      c.real = c.imag = norm.real = norm.imag = 0.0;
      for (k = 0; k < matrix_get_rows(tmp); k++) {
	cmatrix_read_value(&v, tmp, j, k);//CMATRIX_GET(v, q, j, k);
	cmatrix_read_value(&u, tmp, i, k);//CMATRIX_GET(u, p, i, k);
	complex_add_complex_multiply_complex(&c, &v, &u);
	complex_add_complex_multiply_complex(&norm, &v, &v);
      }
      complex_divide_complex(&c, &norm);
      for (k = 0; k < matrix_get_rows(tmp); k++) {
	cmatrix_read_value(&v, q, j, k);//CMATRIX_GET(v, q, j, k);
	cmatrix_read_value(&vnext, q, i, k);//CMATRIX_GET(vnext, q, i, k);
	complex_subtract_complex_multiply_complex(&vnext, &v, &c);
	cmatrix_put_value(vnext, q, i, k);//CMATRIX_PUT(vnext, q, i, k);
      }
    }

    for (k = 0; k < matrix_get_rows(tmp); k++) {
      cmatrix_read_value(&vnext, tmp, i, k);//CMATRIX_GET(vnext, q, i, k);
      if (!(abs(vnext.real) < DBL_EPSILON && abs(vnext.imag) < DBL_EPSILON))
	break;
    }
    if (k >= matrix_get_rows(tmp)) break;
  }

  for (j = 0; j < i; j++) {
    norm.real = norm.imag = 0.0;
    for (k = 0; k < matrix_get_rows(tmp); k++) {
      cmatrix_read_value(&v, tmp, j, k);//CMATRIX_GET(v, q, j, k);
      complex_add_complex_multiply_complex(&norm, &v, &v);
    }
    complex_copy_complex_sqrt(&c, &norm);
    for (k = 0; k < matrix_get_rows(tmp); k++) {
      cmatrix_read_value(&v, tmp, j, k);//CMATRIX_GET(v, q, j, k);
      complex_divide_complex(&v, &c);
      cmatrix_put_value(v, tmp, j, k);//CMATRIX_PUT(v, q, j, k);
    }
  }

  q = matrix_new(i, matrix_get_rows(tmp), true);
  cmatrix_copy_cmatrix(q, 0, 0, tmp, 0, 0, i, matrix_get_rows(tmp));

  matrix_destroy(tmp);

  return q;
}

int matrix_gram_schmidt_process(matrix_t *q, matrix_t *p)
{
  int i, j, k;
  real_t u, v, vnext;
  real_t c, norm;

  assert(q);
  assert(p);
  assert(matrix_are_matched(q, p));

  // Step 1. Let v1 = u1
  matrix_copy_matrix(q, 0, 0, p, 0, 0, 1, matrix_get_rows(p));
  for (i = 1; i < matrix_get_columns(p); i++) {
    //vector_copy_column_vector_of_matrix(vnext, q, i, 0);
    //vector_copy_column_vector_of_matrix(u, p, i, 0);
    //vector_copy_vector(vnext, 0, u, 0, u->length);
    matrix_copy_matrix(q, i, 0, p, i, 0, 1, matrix_get_rows(p));
    for (j = 0; j < i; j++) {
      //vector_copy_column_vector_of_matrix(v, q, j, 0);
      //vector_dotproduct_vector(&c, u, v);
      //vector_get_norm(&norm, v);
      c = norm = 0.0;
      for (k = 0; k < matrix_get_rows(p); k++) {
	v = matrix_get_value(q, j, k);//MATRIX_GET(v, q, j, k);
	u = matrix_get_value(q, i, k);//MATRIX_GET(u, p, i, k);
	c += v*u;
	norm += v*v;
      }
      c /= norm;
      //vector_subtract_vector_multiply_scalar(vnext, v, c);
      for (k = 0; k < matrix_get_rows(q); k++) {
	v = matrix_get_value(q, j, k);//MATRIX_GET(v, q, j, k);
	vnext = matrix_get_value(q, i, k);//MATRIX_GET(vnext, q, i, k);
	vnext -= v*c;
	matrix_put_value(vnext, q, i, k);//MATRIX_PUT(vnext, q, i, k);
      }
    }
    //if (!vector_is_zero(vnext, 0, vnext->length)) break;
    for (k = 0; k < matrix_get_rows(q); k++) {
      vnext = matrix_get_value(q, i, k);//MATRIX_GET(vnext, q, i, k);
      if (!(vnext < DBL_EPSILON)) break; // non-zero
    }
    if (k >= q->rows) break;
  }
  for (j = 0; j < i; j++) {
    norm = 0.0;
    for (k = 0; k < matrix_get_rows(q); k++) {
      v = matrix_get_value(q, j, k);//MATRIX_GET(v, q, j, k);
      norm += v*v;
    }
    norm = sqrt(norm);
    for (k = 0; k < matrix_get_rows(q); k++) {
      v = matrix_get_value(q, j, k);//MATRIX_GET(v, q, j, k);
      v /= norm;
      matrix_put_value(v, q, j, k);//MATRIX_PUT(v, q, j, k);
    }
  }
  return i;
}

int cmatrix_gram_schmidt_process(matrix_t *q, matrix_t *p)
{
  int i, j, k;
  complex_t u, v, vnext;
  complex_t c, norm;

  assert(q);
  assert(matrix_is_imaginary(q));
  assert(p);
  assert(matrix_is_imaginary(p));
  assert(matrix_are_matched(p, q));

  // Step 1. Let v1 = u1
  //v = &q[0], u = &p[0];
  cmatrix_copy_cmatrix(q, 0, 0, p, 0, 0, 1, matrix_get_rows(p));
  for (i = 1; i < matrix_get_columns(p); i++) {
    //vnext = &q[i];
    //cvector_copy_column_vector_of_cmatrix(vnext, q, i, 0);
    //u = &p[i];
    //cvector_copy_column_vector_of_cmatrix(u, p, i, 0);
    //cvector_copy_cvector(vnext, 0, u, 0, u->length);
    cmatrix_copy_cmatrix(q, i, 0, p, i, 0, 1, matrix_get_rows(p));
    for (j = 0; j < i; j++) {
      //v = &q[j];
      //cvector_copy_column_vector_of_cmatrix(v, q, j, 0);
      //cvector_dotproduct_cvector(&c, u, v);
      //cvector_get_norm(&norm, v);
      c.real = c.imag = norm.real = norm.imag = 0.0;
      for (k = 0; k < matrix_get_rows(q); k++) {
	cmatrix_read_value(&v, q, j, k);//CMATRIX_GET(v, q, j, k);
	cmatrix_read_value(&u, q, i, k);//CMATRIX_GET(u, p, i, k);
	complex_add_complex_multiply_complex(&c, &v, &u);
	complex_add_complex_multiply_complex(&norm, &v, &v);
      }
      complex_divide_complex(&c, &norm);
      //printf("c(%lf, %lf)\n", c.real, c.imag);
      //cvector_subtract_cvector_multiply_scalar(vnext, v, c);
      for (k = 0; k < matrix_get_rows(q); k++) {
	cmatrix_read_value(&v, q, j, k);//CMATRIX_GET(v, q, j, k);
	cmatrix_read_value(&vnext, q, i, k);//CMATRIX_GET(vnext, q, i, k);
	complex_subtract_complex_multiply_complex(&vnext, &v, &c);
	cmatrix_put_value(vnext, q, i, k);//CMATRIX_PUT(vnext, q, i, k);
      }
      //matrix_dump(q);
    }
    //if (!cvector_is_zero(vnext, 0, vnext->length)) break;
    //cmatrix_copy_column_cvector(q, i, 0, vnext);
    for (k = 0; k < matrix_get_rows(q); k++) {
      cmatrix_read_value(&vnext, q, i, k);//CMATRIX_GET(vnext, q, i, k);
      if (!(abs(vnext.real) < DBL_EPSILON && abs(vnext.imag) < DBL_EPSILON))
	break;
    }
    if (k >= matrix_get_rows(q)) break;
  }
  for (j = 0; j < i; j++) {
    norm.real = norm.imag = 0.0;
    for (k = 0; k < matrix_get_rows(q); k++) {
      cmatrix_read_value(&v, q, j, k);//CMATRIX_GET(v, q, j, k);
      complex_add_complex_multiply_complex(&norm, &v, &v);
    }
    complex_copy_complex_sqrt(&c, &norm);
    //complex_sqrt(&norm);
    for (k = 0; k < matrix_get_rows(q); k++) {
      cmatrix_read_value(&v, q, j, k);//CMATRIX_GET(v, q, j, k);
      //complex_divide_complex(&v, &norm);
      complex_divide_complex(&v, &c);
      cmatrix_put_value(v, q, j, k);//CMATRIX_PUT(v, q, j, k);
    }
  }
  return i;
}
