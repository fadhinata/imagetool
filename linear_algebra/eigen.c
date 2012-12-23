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
#include <float.h>
#include <assert.h>

#include <common.h>
#include <linear_algebra/matrix.h>
#include <statistics/matrix_statistic.h>

/* Leverrier's Algorithm to compute characteristic equation
 *
 * Characteristic equ.
 *  X(t) = t^n + p(1) * t^(n-1) + ... + p(n-1) * t + p(n)
 * where
 *  p(k) = -1 / k * (s[k] + p(1) * s[k-1] + ... + p(k-1) * s[1])
 *  s[k] = trace(A^k)
 *
 * Output
 *  X(t) = c(0) + c(1) * t + ... + c(n-1) * t^(n-1) + t^n 
 */
void matrix_compute_characteristic_polynomial(vector_t *c, matrix_t *A)
{
  int i, k, n;
  matrix_t *curr, *prev, *tmp;
  real_t *s, *p;

  assert(A);
  assert(matrix_is_square(A));
  assert(c);
  assert(vector_get_length(c) == matrix_get_columns(A));

  n = matrix_get_columns(A);

  s = (real_t *)malloc((n + 1) * sizeof(real_t));
  assert(s);

  p = (real_t *)malloc((n + 1) * sizeof(real_t));
  assert(p);

  curr = matrix_new(n, n, false);
  prev = matrix_new(n, n, false);
  matrix_identify(prev);

  // s[k] = trace(A^k)
  for (k = 1; k <= n; k++) {
    matrix_copy_matrix_multiply_matrix(curr, prev, A);
    s[k] = matrix_get_trace(curr);
    tmp = prev; prev = curr; curr = tmp;
  }

  // p[k] = -1 / k * (s[k] + p[1] * s[k-1] + ... + p[k-1] * s[1])
  for (k = 1; k <= n; k++) {
    p[k] = s[k];
    for (i = 1; i < k; i++)
      p[k] += p[i] * s[k - i];
    p[k] /= -k;
  }

  for (i = 0; i < n; i++) vector_put_value(p[n - i], c, i);

  matrix_destroy(prev);
  matrix_destroy(curr);
  free(p);
  free(s);
}

/* Companion matrix of given polynomial
 *
 * Monic polynomial
 *  p(t) = a(0) + a(1) * t + ... + a(n-1) * t^(n-1) + t^n
 *
 * Companion matrix
 *  C = 0   ... 0   -a(0)
 *      1 0 ... 0   -a(1)
 *      . . ... .     . 
 *      . . ... .     .
 *      . . ... .     .
 *      0 0 ... 1 -a(n-1)
 */
void matrix_companion_matrix(matrix_t *C, vector_t *a)
{
  int i, n;

  assert(a);
  assert(C);
  assert(matrix_is_square(C));
  assert(matrix_get_columns(C) == vector_get_dimension(a));

  n = vector_get_dimension(a);

  matrix_clear(C);
  for (i = 0; i < n - 1; i++) {
    matrix_put_value(1, C, i, i+1);
    matrix_put_value(-1 * vector_get_value(a, i), C, n-1, i);
  }
}

int matrix_eigen_solve(matrix_t *eigen_vectors, vector_t *eigen_values, matrix_t *m)
{
  assert(eigen_vectors);
  assert(eigen_values);
  assert(m);
  assert(matrix_is_square(m));

}

/* Inequality
 *  (A * q = lambda * q) or (A^k * q = lambda^k * q) in general
 * where
 *  A : given n x n matrix
 *  q : eigen vector of A
 *  lambda : eigen value
 * In iterative process
 *  {q(i)} : eigenvectors of A
 *  {lambda(i)} : corresponding eigenvalues such that |lambda(1)| > |lambda(2)| > ... > |lambda(n)|
 * Initiative approximation of eigenvector
 *  v(0) = c1 * q(1) + ... + cn * q(n) with |v(0)| = 1
 *  w = A * v(k-1)
 *  v(k) = w / |w|
 *  v(k) -> q(1) as k -> infinity
 */
// compute the eigen vector corresponding to the eigen value with largest magnitude
int matrix_compute_eigenvector_by_power_iteration(vector_t *v, matrix_t *A)
{
  int n;
  vector_t *w;
  real_t norm;

  assert(v);
  assert(A);
  assert(matrix_is_square(A));
  assert(vector_get_dimension(v) == matrix_get_columns(A));

  n = matrix_get_columns(A);
  w = vector_new(n, false);

  // v(0) as random
  vector_fill_randomly(v);
  norm = vector_get_norm(v);
  vector_divide_scalar(v, norm);

  for (;;) {
    vector_copy_matrix_multiply_vector(w, A, v);
    norm = vector_get_norm(w);
    vector_divide_scalar(w, norm);
    if (vector_cmp(v, w) == 0) break;
    vector_copy(v, w);
  }

  vector_destroy(w);
}

// compute the eigen vector corresponding to the eigen value with smallest magnitude
int matrix_compute_eigenvector_by_inverse_iteration(vector_t *v, matrix_t *A)
{
  int i, n;
  vector_t *w;
  real_t norm;

  assert(v);
  assert(A);
  assert(matrix_is_square(A));
  assert(vector_get_dimension(v) == matrix_get_columns(A));

  n = matrix_get_columns(A);
  w = vector_new(n, false);

  inv = matrix_new_and_copy_matrix_inverse(A);

  // v(0) as random
  vector_fill_randomly(v);
  norm = vector_get_norm(v);
  vector_divide_scalar(v, norm);

  for (;;) {
    vector_copy_matrix_multiply_vector(w, inv, v);
    norm = vector_get_norm(w);
    vector_divide_scalar(w, norm);
    if (vector_cmp(w, v) == 0) break;
    vector_copy(v, w);
  }

  matrix_destroy(inv);
  vector_destroy(w);
}

/* Inequality
 *  (A * q = lambda * q) or (A^k * q = lambda^k * q) in general
 * where
 *  A : given n x n matrix
 *  q : eigen vector of A
 *  lambda : eigen value
 *  mu closed to eigenvalue corresponding to find eigenvector
 * In iterative process
 *  {q(i)} : eigenvectors of A
 *  {lambda(i)} : corresponding eigenvalues such that |lambda(1)| > |lambda(2)| > ... > |lambda(n)|
 * Initiative approximation of eigenvector
 *  v(0) = c1 * q(1) + ... + cn * q(n) with |v(0)| = 1
 *  (A - mu * I) * w = v(k-1)
 *  v(k) = w / |w|
 *  v(k) -> q(1) as k -> infinity
 */
int matrix_compute_eigenvector_by_shifted_inverse_iteration(vector_t *v, real_t mu, matrix_t *A)
{
  int i, n;
  vector_t *w;
  real_t norm;

  assert(v);
  assert(A);
  assert(matrix_is_square(A));
  assert(vector_get_dimension(v) == matrix_get_columns(A));

  n = matrix_get_columns(A);
  w = vector_new(n, false);
  m = matrix_new(n, n, false);

  matrix_copy(m, A);
  for (i = 0; i < n; i++)
    matrix_put_value(matrix_get_value(A, i, i) - mu, m, i, i);

  inv = matrix_new_and_copy_matrix_inverse(m);

  // v(0) as random
  vector_fill_randomly(v);
  norm = vector_get_norm(v);
  vector_divide_scalar(v, norm);

  for (;;) {
    vector_copy_matrix_multiply_vector(w, inv, v);
    norm = vector_get_norm(w);
    vector_divide_scalar(w, norm);
    if (vector_cmp(w, v) == 0) break;
    vector_copy(v, w);
  }

  matrix_destroy(inv);
  matrix_destroy(m);
  vector_destroy(w);
}

/* Rayleigh quotient iteration
 *
 * Initiative
 *  v(0) with |v(0)| = 1
 *  lambda(0) = r(v(0)) = transpose(v(0)) * A * v(0)
 *
 * Interative
 *  (A - lambda(k-1) * I) * w = v(k-1) for finding w
 *  v(k) = w / |w|
 *  lambda(k) = r(v(k)) = transpose(v(k)) * A * v(k)
 */
int matrix_compute_eigenvector_by_rayleigh_quotient_interation(vector_t *v, matrix_t *A)
{
  int i, n;
  vector_t *w;
  real_t norm, lambda;

  assert(v);
  assert(A);
  assert(matrix_is_square(A));
  assert(vector_get_dimension(v) == matrix_get_columns(A));

  n = matrix_get_columns(A);
  w = vector_new(n, false);
  m = matrix_new(n, n, false);
  inv = matrix_new(n, n, false);

  // v(0) as random
  //vector_fill_randomly(v);
  norm = vector_get_norm(v);
  vector_divide_scalar(v, norm);

  // lamba(0) corresponding to v(0)
  for (;;) {
    vector_copy_matrix_multiply_vector(w, A, v);
    lambda = vector_dotproduct_vector(v, w);
    matrix_copy(m, A);
    for (i = 0; i < n; i++)
      matrix_sub_value(lambda, m, i, i);
    matrix_copy_matrix_inverse(inv, m);
    vector_copy_matrix_multiply_vector(w, inv, v);
    norm = vector_get_norm(w);
    vector_divide_scalar(w, norm);
    if (vector_cmp(w, v) == 0) break;
    vector_copy(v, w);
    //vector_copy_matrix_multiply_vector(w, A, v);
    //lambda = vector_dotproduct_vector(v, w);
  }

  matrix_destroy(inv);
  matrix_destroy(m);
  vector_destroy(w);
}

int matrix_compute_eigenvectors_by_simultaneous_iteration(matrix_t *V, matrix_t *A)
{
  int i, n;
  vector_t *v;
  real_t norm, lambda;
  matrix_t *Q, *R, *W, *Qnext, *tmp;

  assert(V);
  assert(A);
  assert(matrix_is_square(A));
  assert(matrix_are_matched(V, B));

  n = matrix_get_columns(A);
  v = vector_new(n, false);
  Q = matrix_new(n, n, false);
  R = matrix_new(n, n, false);
  W = matrix_new(n, n, false);
  Qnext = matrix_new(n, n, false);

  // linearly independent set
  matrix_gram_schmidt_process(V, A);

  matrix_qr_decomposite(Q, R, V);

  for (;;) {
    matrix_copy_matrix_multiply_matrix(W, A, Q);
    matrix_qr_decomposite(Qnext, R, W);
    if (matix_cmp(Q, Qnext) == 0) break;
    tmp = Qnext;
    Qnext = Q;
    Q = tmp;
  }

  matrix_copy(V, Q);

  matrix_destroy(Qnext);
  matrix_destroy(W);
  matrix_destroy(R);
  matrix_destroy(Q);
  vector_destroy(v);
}

int matrix_compute_eigenvectors_by_qr_method(matrix_t *V, matrix_t *A)
{
}
