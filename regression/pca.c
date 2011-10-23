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

typedef struct {
  matrix_t *eigen_vectors;
  vector_t *eigen_values;
} pca_t;

/* Nonlinear Iterative Partial Least Squares for PCA */
int NIPALS_for_PCA(matrix_t *pc, matrix_t *x)
{
  int i, j, n, imax;
  double v, norm, tau, vmax;
  matrix_t *inv, *x_i;
  vector_t *t, *p;

  assert(pc);
  assert(x);
  assert(pc->rows == x->rows);

  // prepare variables
  t = vector_new(x->rows, false);
  p = vector_new(x->columns, false);
  x_i = matrix_new_and_copy(x);
  // mean centering
  for (j = 0; j < x->columns; j++) {
    matrix_get_mean_on_region(&v, x_i, j, 0, 1, x_i->rows);
    matrix_subtract_scalar_on_region(x_i, v, j, 0, 1, x_i->rows);
    //VECTOR_PUT(v, mx, j);
  }
  // Select the columns vector of X that has the highest remaining sum of squares
  vmax = 0.0;
  for (j = 0; j < x->columns; j++) {
    vector_copy_column_vector_of_matrix(t, x_i, j, 0);
    norm = vector_get_norm(t);
    if (norm > vmax) imax = j;
  }
  vector_copy_column_vector_of_matrix(t, x_i, imax, 0);
  tau = vector_get_norm(t);
  // find principle components
  for (i = 0; i < pc->columns; i++) {
    do {
      // Estimate of loading vector: p
      // (i.e. project the matrix x_i onto t)
      // p' = (t' * t)^(-1) * (t' * x_i)
      norm = vector_get_norm(t);
      if (abs(norm) < DBL_EPSILON) goto __error_return__;
      vector_copy_vector_multiply_matrix(p, t, x_i);
      vector_divide_scalar(p, norm);
      // Scale length of p to 1.0 to avoid scaling ambiguity
      // p = p * (p' * p)^(0.5)
      norm = vector_get_norm(p);
      vector_divide_scalar(p, sqrt(norm));
      // Improve estimate of score t
      // (i.e. project the matrix x_i on p)
      // t = x_i * p * (p' * p)^(-1)
      vector_copy_matrix_multiply_vector(t, x_i, p);
      norm = vector_get_norm(p); // Its norm maybe one !, becaus of the scaling for p
      vector_divide_scalar(t, norm);
      // Improve estimate of the eigenvalue tau
      norm = vector_get_norm(t);
      // Check convergence:
      if (norm < 0.0001*tau) {
	matrix_subtract_vector_multiply_vector(x_i, t, p);
	break;
      }
      tau = norm;
    } while (1);
    matrix_copy_column_vector(pc, i, 0, p);
  }
  matrix_destroy(x_i);
  vector_destroy(p);
  vector_destroy(t);
  return 0;
 __error_return__:
  matrix_destroy(x_i);
  vector_destroy(p);
  vector_destroy(t);
  return -1;	
}

/* Nonlinear Iterative Partial Least Squares for PLS */
//int NIPALS_for_PLS(matrix_t *pc, matrix_t *x)
