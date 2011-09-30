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

#include <dlink.h>
#include <complex2.h>
#include <vector.h>
#include <matrix.h>
//#include <gram_schmidt.h>

/*
int vector_list_gram_schmidt_process(dlist_t *q, dlist_t *p)
{
  int n, len;
  vector_t *u, *v, *vnext;
  dlink_t *a, *b;

  assert(q);
  assert(p);
  assert(dlist_get_count(p) > 0);

  n = dlist_get_count(p);

  a = dlist_glimpse(0, p);
  u = (vector_t *)a->object;
  len = vector_get_length(u);

  v = vector_new(len, false);
  vector_copy(v, u);

  vector_inc_ref(v);
  b = dlink_new();
  b->object = (void *)v;
  dlist_insert(b, q);

  for (a = a->next; a != p->head; a = a->next) {
    u = (vector_t *)a->object;
    vnext = vector_new(len, false);
    vector_copy(vnext, u);
  }
}
*/

/* n * basis -> (n or below n) * orthonormal basis */
int vector_gram_schmidt_process(vector_t **q, vector_t **p, int n)
{
  int i, j;
  vector_t *u, *v, *vnext;
  real_t c, norm;

  assert(q);
  assert(p);
  assert((*q)->length == (*p)->length);

  // Step 1. Let v1 = u1
  v = *(q + 0), u = *(p + 0);
  vector_copy_vector(v, 0, u, 0, u->length);
  for (i = 1; i < n; i++) {
    vnext = *(q+i);
    u = *(p+i);
    vector_copy_vector(vnext, 0, u, 0, u->length);
    for (j = 0; j < i; j++) {
      v = *(q+j);
      vector_dotproduct_vector(&c, u, v);
      norm = vector_get_norm(v);
      c /= norm;
      //complex_divide_complex(c, norm);
      vector_subtract_vector_multiply_scalar(vnext, v, c);
    }
    if (vector_iszero(vnext, 0, vnext->length)) break;
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
  assert((*q)->length == (*p)->length);
  // Step 1. Let v1 = u1
  v = *(q+0), u = *(p+0);
  cvector_copy_cvector(v, 0, u, 0, u->length);
  for (i = 1; i < n; i++) {
    vnext = *(q+i);
    u = *(p+i);
    cvector_copy_cvector(vnext, 0, u, 0, u->length);
    for (j = 0; j < i; j++) {
      v = *(q+j);
      cvector_dotproduct_cvector(&c, u, v);
      norm = cvector_get_norm(v);
      complex_divide_complex(&c, &norm);
      cvector_subtract_cvector_multiply_scalar(vnext, v, c);
    }
    if (cvector_iszero(vnext, 0, vnext->length)) break;
  }
  for (j = 0; j < i; j++) {
    v = *(q+j);
    norm = cvector_get_norm(v);
    complex_copy_complex_sqrt(&c, &norm);
    cvector_divide_scalar(v, c);
  }
  return i;
}

int matrix_gram_schmidt_process(matrix_t *q, matrix_t *p)
{
  int i, j, k;
  real_t u, v, vnext;
  real_t c, norm;
  assert(q);
  assert(p);
  assert(q->columns == p->columns);
  assert(q->rows == p->rows);
  // Step 1. Let v1 = u1
  matrix_copy_matrix(q, 0, 0, p, 0, 0, 1, p->rows);
  for (i = 1; i < p->columns; i++) {
    //vector_copy_column_vector_of_matrix(vnext, q, i, 0);
    //vector_copy_column_vector_of_matrix(u, p, i, 0);
    //vector_copy_vector(vnext, 0, u, 0, u->length);
    matrix_copy_matrix(q, i, 0, p, i, 0, 1, p->rows);
    for (j = 0; j < i; j++) {
      //vector_copy_column_vector_of_matrix(v, q, j, 0);
      //vector_dotproduct_vector(&c, u, v);
      //vector_get_norm(&norm, v);
      c = norm = 0.0;
      for (k = 0; k < p->rows; k++) {
	v = matrix_get_value(j, k, q);//MATRIX_GET(v, q, j, k);
	u = matrix_get_value(i, k, q);//MATRIX_GET(u, p, i, k);
	c += v*u;
	norm += v*v;
      }
      c /= norm;
      //vector_subtract_vector_multiply_scalar(vnext, v, c);
      for (k = 0; k < q->rows; k++) {
	v = matrix_get_value(j, k, q);//MATRIX_GET(v, q, j, k);
	vnext = matrix_get_value(i, k, q);//MATRIX_GET(vnext, q, i, k);
	vnext -= v*c;
	matrix_put_value(vnext, i, k, q);//MATRIX_PUT(vnext, q, i, k);
      }
    }
    //if (!vector_is_zero(vnext, 0, vnext->length)) break;
    for (k = 0; k < q->rows; k++) {
      vnext = matrix_get_value(i, k, q);//MATRIX_GET(vnext, q, i, k);
      if (!(vnext < DBL_EPSILON)) break; // non-zero
    }
    if (k >= q->rows) break;
  }
  for (j = 0; j < i; j++) {
    norm = 0.0;
    for (k = 0; k < q->rows; k++) {
      v = matrix_get_value(j, k, q);//MATRIX_GET(v, q, j, k);
      norm += v*v;
    }
    norm = sqrt(norm);
    for (k = 0; k < q->rows; k++) {
      v = matrix_get_value(j, k, q);//MATRIX_GET(v, q, j, k);
      v /= norm;
      matrix_put_value(v, j, k, q);//MATRIX_PUT(v, q, j, k);
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
  assert(q->imaginary);
  assert(p);
  assert(p->imaginary);
  assert(q->columns == p->columns);
  assert(q->rows == p->rows);
  // Step 1. Let v1 = u1
  //v = &q[0], u = &p[0];
  cmatrix_copy_cmatrix(q, 0, 0, p, 0, 0, 1, p->rows);
  for (i = 1; i < p->columns; i++) {
    //vnext = &q[i];
    //cvector_copy_column_vector_of_cmatrix(vnext, q, i, 0);
    //u = &p[i];
    //cvector_copy_column_vector_of_cmatrix(u, p, i, 0);
    //cvector_copy_cvector(vnext, 0, u, 0, u->length);
    cmatrix_copy_cmatrix(q, i, 0, p, i, 0, 1, p->rows);
    for (j = 0; j < i; j++) {
      //v = &q[j];
      //cvector_copy_column_vector_of_cmatrix(v, q, j, 0);
      //cvector_dotproduct_cvector(&c, u, v);
      //cvector_get_norm(&norm, v);
      c.real = c.imag = norm.real = norm.imag = 0.0;
      for (k = 0; k < q->rows; k++) {
	cmatrix_read_value(&v, j, k, q);//CMATRIX_GET(v, q, j, k);
	cmatrix_read_value(&u, i, k, q);//CMATRIX_GET(u, p, i, k);
	complex_add_complex_multiply_complex(&c, &v, &u);
	complex_add_complex_multiply_complex(&norm, &v, &v);
      }
      complex_divide_complex(&c, &norm);
      //printf("c(%lf, %lf)\n", c.real, c.imag);
      //cvector_subtract_cvector_multiply_scalar(vnext, v, c);
      for (k = 0; k < q->rows; k++) {
	cmatrix_read_value(&v, j, k, q);//CMATRIX_GET(v, q, j, k);
	cmatrix_read_value(&vnext, i, k, q);//CMATRIX_GET(vnext, q, i, k);
	complex_subtract_complex_multiply_complex(&vnext, &v, &c);
	cmatrix_put_value(vnext, i, k, q);//CMATRIX_PUT(vnext, q, i, k);
      }
      //matrix_dump(q);
    }
    //if (!cvector_is_zero(vnext, 0, vnext->length)) break;
    //cmatrix_copy_column_cvector(q, i, 0, vnext);
    for (k = 0; k < q->rows; k++) {
      cmatrix_read_value(&vnext, i, k, q);//CMATRIX_GET(vnext, q, i, k);
      if (!(vnext.real < DBL_EPSILON && vnext.imag < DBL_EPSILON)) break;
    }
    if (k >= q->rows) break;
  }
  for (j = 0; j < i; j++) {
    norm.real = norm.imag = 0.0;
    for (k = 0; k < q->rows; k++) {
      cmatrix_read_value(&v, j, k, q);//CMATRIX_GET(v, q, j, k);
      complex_add_complex_multiply_complex(&norm, &v, &v);
    }
    complex_copy_complex_sqrt(&c, &norm);
    //complex_sqrt(&norm);
    for (k = 0; k < q->rows; k++) {
      cmatrix_read_value(&v, j, k, q);//CMATRIX_GET(v, q, j, k);
      //complex_divide_complex(&v, &norm);
      complex_divide_complex(&v, &c);
      cmatrix_put_value(v, j, k, q);//CMATRIX_PUT(v, q, j, k);
    }
  }
  return i;
}
