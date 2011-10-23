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
#include <assert.h>

#include <common.h>
#include <linear_algebra/vector.h>
#include <linear_algebra/matrix.h>
#include <statistics/vector_statistic.h>
#include <regression/least_squares.h>
#include <regression/pls.h>

/* nsample : the # of dimension */
pls_t *pls_new(int ndim)
{
  pls_t *pls1;

  pls1 = (pls_t *)malloc(sizeof(pls_t));
  pls1->iteration = 0;
  pls1->mx = vector_new(ndim, false);
  pls1->W = NULL;
  pls1->P = NULL;
  pls1->Q = NULL;
  pls1->beta = vector_new(ndim, false);

  return pls1;
}

void pls_destroy(pls_t *p)
{
  assert(p);

  vector_destroy(p->beta);
  vector_destroy(p->mx);
  if (p->W) matrix_destroy(p->W);

  free(p);
}

// matrix : rows x columns
// the number of sample: n
// the number of dimension: m
// x  : n x m
// y  : n x 1
// b  : m x 1
// b0 : 1 x 1
// Y = X * B + B0 
void pls_orthogonal_calibrate(pls_t *pls1, matrix_t *x, vector_t *y, int iteration)
{
  int i;
  matrix_t *xcurr; // [n x m]
  matrix_t *xt;    // [m x n]
  vector_t *ycurr; // [n x 1]
  vector_t *w;     // [m x 1]
  vector_t *t;     // [n x 1]
  vector_t *p;     // [m x 1]
  vector_t *vec;   // [n x 1]
  real_t q, v;
	
  assert(pls1);
  assert(x);
  assert(y);
  assert(vector_get_dimension(pls1->mx) == matrix_get_columns(x));
  assert(matrix_get_rows(x) == vector_get_dimension(y));
  assert(iteration > matrix_get_rows(x));

  // xcurr : [n x m]
  xcurr = matrix_new_and_copy(x);
  // ycurr : [n x 1]
  ycurr = vector_new_and_copy(y);
  // xt : [m x n]
  xt = matrix_new(matrix_get_rows(x), matrix_get_columns(x), false);
  // w : [m x 1]
  w = vector_new(matrix_get_columns(x), false); // weight vector
  // W : [m x iteration]
  if (pls1->W) matrix_destroy(pls1->W);
  pls1->W = matrix_new(iteration, matrix_get_columns(x), false);
  // t : [n x 1]
  t = vector_new(matrix_get_rows(x), false); // score vector
  // p : [m x 1]
  p = vector_new(matrix_get_columns(x), false);
  // P : [m x iteration]
  if (pls1->P) matrix_destroy(pls1->P);
  pls1->P = matrix_new(iteration, matrix_get_columns(x), false);
  // Q : [iteration x 1]
  if (pls1->Q) vector_destroy(pls1->Q);
  pls1->Q = vector_new(iteration, false);
  // vec : [n x 1]
  vec = vector_new(matrix_get_rows(x), false);

  // Mean centering
  for (i = 0; i < matrix_get_columns(x); i++) {
    vector_copy_column_vector_of_matrix(vec, xcurr, i, 0);
    v = vector_get_mean(vec);
    vector_put_value(v, pls1->mx, i);
    vector_subtract_scalar(vec, v);
    matrix_copy_column_vector(xcurr, i, 0, vec);
  }

  pls1->my = vector_get_mean(ycurr);
  vector_subtract_scalar(ycurr, pls1->my);

  // Scoring
  for (i = 0; i < iteration; i++) {
    // Get transpose of matrix x; tranpose(x): m x n
    matrix_copy_matrix_transpose(xt, xcurr);

    // Auxiliary weight vector; w = transpose(x) * y / norm(transpose(x) * y)
    // w : [m x 1]
    vector_copy_matrix_multiply_vector(w, xt, ycurr);
    v = vector_get_norm(w);
    vector_divide_scalar(w, v);
    // W : [m x iteration]
    matrix_copy_column_vector(pls1->W, i, 0, w);

    // Factor coefficient vector
    // t : [n x 1]
    vector_copy_matrix_multiply_vector(t, xcurr, w);

    // Loading vector p
    // p : [m x 1]
    vector_copy_matrix_multiply_vector(p, xt, t);
    vector_dotproduct_vector(&v, t, t);
    vector_divide_scalar(p, v);
    matrix_copy_column_vector(pls1->P, i, 0, p);

    // Scalar coefficient of relationship between factor and activity
    // y : [n x 1], t : [n x 1]
    vector_dotproduct_vector(&q, ycurr, t);
    q /= v;
    vector_put_value(q, pls1->Q, i);

    if (abs(q) < REAL_EPSILON) break;

    // Update x and y
    if (i < iteration - 1) {
      matrix_subtract_vector_multiply_vector(xcurr, t, p);
      vector_subtract_vector_multiply_scalar(ycurr, t, q);
    }
  }
  pls1->iteration = min(i+1, iteration);
  // for pls factors

  // beta = W * (p` * W)^(-1) * q
  // [m x 1] = [m x n] * ([m x 1]` * [m x n])^(-1) * [1 x 1]
  vector_copy_vector_multiply_matrix(vec, p, pls1->W);
  vector_dotproduct_vector(&v, vec, vec);
  vector_divide_scalar(vec, v);
  vector_copy_matrix_multiply_vector(pls1->beta, pls1->W, vec);
  vector_multiply_scalar(pls1->beta, q);

  // b0 = my - mx` * beta
  // [1 x 1] = [1 x 1] - ([m x 1])` * [m x 1]
  vector_dotproduct_vector(&v, pls1->mx, pls1->beta);
  pls1->beta0 = pls1->my - v;

  vector_destroy(vec);
  vector_destroy(p);
  vector_destroy(t);
  vector_destroy(w);
  matrix_destroy(xt);
  vector_destroy(ycurr);
  matrix_destroy(xcurr);
}

pls_t *pls_new_and_orthogonal_calibrate(matrix_t *x, vector_t *y, int iteration)
{
  pls_t *pls1;

  assert(x);
  assert(y);
  assert(matrix_get_rows(x) == vector_get_dimension(y));
  assert(iteration > matrix_get_rows(x));

  pls1 = pls_new(matrix_get_columns(x));
  pls_orthogonal_calibrate(pls1, x, y, iteration);

  return pls1;
}

real_t pls_full_orthogonal_predict(pls_t *pls1, vector_t *x)
{
  int i;
  real_t v;
  vector_t *xcurr; // [m x 1]
  vector_t *t;     // [iteration x 1]
  vector_t *p;     // [m x 1]
  vector_t *w;     // [m x 1]

  assert(pls1);
  assert(pls1->W);
  assert(pls1->P);
  assert(pls1->Q);
  assert(vector_get_dimension(pls1->mx) == vector_get_dimension(x));
  assert(vector_get_dimension(pls1->beta) == vector_get_dimension(x));

  xcurr = vector_new(vector_get_dimension(x), false);
  t = vector_new(pls1->iteration, false);
  p = vector_new(vector_get_dimension(x), false);
  w = vector_new(vector_get_dimension(x), false);

  // Scaling
  vector_copy_vector_subtract_vector(xcurr, x, pls1->mx);

  for (i = 0; i < pls1->iteration; i++) {
    // ti = x` * Wi
    vector_copy_column_vector_of_matrix(w, pls1->W, i, 0);
    vector_dotproduct_vector(&v, xcurr, w);
    vector_put_value(v, t, i);
    // compute new residual
    vector_copy_column_vector_of_matrix(p, pls1->P, i, 0);
    vector_subtract_vector_multiply_scalar(xcurr, p, v);
  }
  
  vector_dotproduct_vector(&v, t, pls1->Q);

  vector_destroy(w);
  vector_destroy(p);
  vector_destroy(t);
  vector_destroy(xcurr);

  return pls1->my + v;
}

real_t pls_short_predict(pls_t *pls1, vector_t *x)
{
  real_t v;

  assert(pls1);
  assert(vector_get_dimension(pls1->mx) == vector_get_dimension(x));
  assert(vector_get_dimension(pls1->beta) == vector_get_dimension(x));

  vector_dotproduct_vector(&v, x, pls1->beta);

  return v + pls1->beta0;
}

// matrix : rows x columns
// the number of sample: n
// the number of dimension: m
// x  : n x m
// y  : n x 1
// b  : m x 1
// b0 : 1 x 1
// Y = X * B + B0 
void pls_nonorthogonal_calibrate(pls_t *pls1, matrix_t *x, vector_t *y, int iteration)
{
  int i, j;
  matrix_t *xcurr; // [n x m]
  matrix_t *xt;    // [m x n]

  vector_t *ycurr; // [n x 1]
  vector_t *y0;

  vector_t *w;     // [m x 1]

  vector_t *t;     // [n x 1]
  matrix_t *T;     // [n x iteration]
  matrix_t *Tcurr;

  vector_t *p;     // [m x 1]
  vector_t *vec;   // [n x 1]
  vector_t *q;
  real_t v;
	
  assert(pls1);
  assert(x);
  assert(y);
  assert(vector_get_dimension(pls1->mx) == matrix_get_columns(x));
  assert(matrix_get_rows(x) == vector_get_dimension(y));
  assert(iteration > matrix_get_rows(x));

  xcurr = matrix_new_and_copy(x);
  ycurr = vector_new_and_copy(y);
  y0 = vector_new_and_copy(y);

  xt = matrix_new(matrix_get_rows(x), matrix_get_columns(x), false);

  // w : [m x 1]
  w = vector_new(matrix_get_columns(x), false); // weight vector
  // W : [m x iteration]
  if (pls1->W) matrix_destroy(pls1->W);
  pls1->W = matrix_new(iteration, matrix_get_columns(x), false);

  // t : [n x 1]
  t = vector_new(matrix_get_rows(x), false); // score vector
  T = matrix_new(matrix_get_rows(x), iteration, false);

  // p : [m x 1]
  p = vector_new(matrix_get_columns(x), false);
  // P : [m x iteration]
  if (pls1->P) matrix_destroy(pls1->P);
  pls1->P = matrix_new(iteration, matrix_get_columns(x), false);
  // Q : [iteration x 1]
  if (pls1->Q) vector_destroy(pls1->Q);
  pls1->Q = vector_new(iteration, false);

  // vec : [n x 1]
  vec = vector_new(matrix_get_rows(x), false);

  // Mean centering
  for (i = 0; i < matrix_get_columns(x); i++) {
    vector_copy_column_vector_of_matrix(vec, xcurr, i, 0);
    v = vector_get_mean(vec);
    vector_put_value(v, pls1->mx, i);
    vector_subtract_scalar(vec, v);
    matrix_copy_column_vector(xcurr, i, 0, vec);
  }

  pls1->my = vector_get_mean(ycurr);
  vector_subtract_scalar(ycurr, pls1->my);
  vector_copy(y0, ycurr);

  // Scoring
  for (i = 0; i < iteration; i++) {
    // Get transpose of matrix x; tranpose(x): m x n
    matrix_copy_matrix_transpose(xt, xcurr);

    // Auxiliary weight vector; w = transpose(x) * y / norm(transpose(x) * y)
    // w : m x 1
    vector_copy_matrix_multiply_vector(w, xt, ycurr);
    v = vector_get_norm(w);
    vector_divide_scalar(w, v);
    // W : m x iteration
    matrix_copy_column_vector(pls1->W, i, 0, w);

    // Factor coefficient vector
    // t : n x 1
    vector_copy_matrix_multiply_vector(t, xcurr, w);
    matrix_copy_column_vector(T, i, 0, t);

    Tcurr = matrix_new(i + 1, vector_get_dimension(t), false);
    matrix_copy_matrix(Tcurr, 0, 0, T, 0, 0, i + 1, matrix_get_rows(T));
    // (Tcurr` * Tcurr)^(-1) * Tcurr` * y
    // ([(i+1) x n] * [n x (i+1)])^(-1) * [(i+1) x n] * [n x 1]
    q = vector_new(i + 1, false);
    least_squares(q, Tcurr, y0);
    vector_copy_vector(pls1->Q, 0, q, 0, vector_get_dimension(q));
    vector_destroy(q);
    matrix_destroy(Tcurr);

    // Update x and y
    if (i < iteration - 1) {
      matrix_subtract_vector_multiply_vector(xcurr, t, w);
      for (j = 0; j <= i; j++) {
	vector_copy_column_vector_of_matrix(t, T, j, 0);
	vector_subtract_vector_multiply_scalar(ycurr, t, vector_get_value(q, j));
      }
    }
  }
  pls1->iteration = i;
  // for pls factors

  // beta = W * q
  vector_copy_vector(q, 0, pls1->Q, 0, vector_get_dimension(pls1->Q));
  vector_copy_matrix_multiply_vector(pls1->beta, pls1->W, q);
  //vector_multiply_scalar(pls1->beta, q);

  // b0 = my - mx` * beta
  // [1 x 1] = [1 x 1] - ([m x 1])` * [m x 1]
  vector_dotproduct_vector(&v, pls1->mx, pls1->beta);
  pls1->beta0 = pls1->my - v;

  vector_destroy(vec);
  vector_destroy(p);
  matrix_destroy(Tcurr);
  matrix_destroy(T);
  vector_destroy(t);
  vector_destroy(w);
  matrix_destroy(xt);
  vector_destroy(y0);
  vector_destroy(ycurr);
  matrix_destroy(xcurr);
}

