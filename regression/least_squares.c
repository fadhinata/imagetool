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

#include <linear_algebra/vector.h>
#include <regression/least_squares.h>
#include <statistics/vector_statistic.h>

/*
 * X * beta = Y
 * (Xt * X) * beta = Xt * Y
 * beta = (Xt * X)~ * (Xt * Y)
 * 
 * linear least squares for line
 * X = 1.0 x1
 *     1.0 x2
 *      ...
 *     1.0 xn
 * Y = y1
 *     y2
 *     ...
 *     yn
 *
 * linear least squares
 * X = 1.0 x1 x1^2 ... x1^n
 *     1.0 x2 x2^2 ... x2^n
 *      ...
 *     1.0 xm xm^2 ... xm^n
 * Y = y1
 *     y2
 *     ...
 *     yn
 */
void least_squares(vector_t *beta, matrix_t *x, vector_t *y)
{
  matrix_t *xt, *m, *inv;
  vector_t *vec;
	
  assert(y);
  assert(x);
  assert(beta);
  assert(matrix_get_rows(x) == vector_get_dimension(y));
  assert(matrix_get_columns(x) == vector_get_dimension(beta));
	
  xt = matrix_new_and_copy_matrix_transpose(x);
  m = matrix_new_and_copy_matrix_multiply_matrix(xt, x);
  inv = matrix_new_and_copy_matrix_inverse(m);
  vec = vector_new_and_copy_matrix_multiply_vector(xt, y);
  vector_copy_matrix_multiply_vector(beta, inv, vec);

  vector_destroy(vec);
  matrix_destroy(inv);
  matrix_destroy(m);
  matrix_destroy(xt);
}

void line_regression_using_least_squares(vector_t *beta, vector_t *xcoord, vector_t *ycoord)
{
  int i, n;
  real_t xval, yval;
  real_t mx; // the means of x
  real_t my; // the means of y
  real_t sxx; // the squared sum of x by x
  real_t sxy; // the squared sum of x by y

  assert(beta);
  assert(vector_get_dimension(beta) == 2);
  assert(xcoord);
  assert(ycoord);
  assert(vector_get_dimension(xcoord) == vector_get_dimension(ycoord));

  //beta = vector_create(2, 0);
  /* the means of xcoord and ycoord */
  n = vector_get_dimension(xcoord);
  mx = my = sxx = sxy = 0.0;
  for (i = 0; i < n; i++) {
    xval = vector_get_value(xcoord, i);
    yval = vector_get_value(ycoord, i);
    mx += xval;
    my += yval;
    sxx += sqr(xval);
    sxy += sqr(yval);
  }
  mx /= (real_t)n;
  my /= (real_t)n;

  /* the sample regresion coefficient */
  vector_put_value((sxy - n * mx * my) / (sxx - n * mx * mx), beta, 1);
  vector_put_value(my - vector_get_value(beta, 1) * mx, beta, 0);
  //*(beta->real+1) = (sxy - n*mx*my) / (sxx - n*mx*mx);
  //*(beta->real+0) = my-(*(beta->real+1))*mx;
  //return beta;
}

vector_t *vector_new_and_line_regression_using_least_squares(vector_t *xcoord, vector_t *ycoord)
{
  vector_t *beta;

  assert(xcoord);
  assert(ycoord);
  assert(vector_get_dimension(xcoord) == vector_get_dimension(ycoord));

  beta = vector_new(2, false);
  line_regression_using_least_squares(beta, xcoord, ycoord);

  return beta;
}

void polynomial_regression_using_least_squares(vector_t *beta, vector_t *xcoord, vector_t *ycoord)
{
  int i;
  matrix_t *m;
  vector_t *v;

  assert(beta);
  assert(xcoord);
  assert(ycoord);
  assert(vector_get_dimension(beta) < vector_get_dimension(xcoord));
  assert(vector_get_dimension(xcoord) == vector_get_dimension(ycoord));

  m = matrix_new(vector_get_dimension(beta), vector_get_dimension(xcoord), false);
  v = vector_new(vector_get_dimension(xcoord), false);

  vector_fill(v, 0, vector_get_dimension(v), 1.0);
  for (i = 0; i < vector_get_dimension(beta); i++) {
    matrix_copy_column_vector(m, i, 0, v);
    vector_multiply_vector(v, xcoord);
  }

  least_squares(beta, m, ycoord);
  vector_destroy(v);
  matrix_destroy(m);
}

vector_t *vector_new_and_polynomial_regression_using_least_squares(vector_t *xcoord, vector_t *ycoord, int order)
{
  vector_t *beta;

  assert(xcoord);
  assert(ycoord);
  assert(vector_get_dimension(xcoord) == vector_get_dimension(ycoord));
  assert(order < vector_get_dimension(xcoord));

  beta = vector_new(order, false);
  polynomial_regression_using_least_squares(beta, xcoord, ycoord);

  return beta;
}

void multiple_linear_regression_using_least_squares(vector_t *beta, matrix_t *xcoord, vector_t *ycoord)
{
  matrix_t *m;

  assert(beta);
  assert(xcoord);
  assert(ycoord);
  assert(ycoord);
  assert(vector_get_dimension(beta) == matrix_get_columns(xcoord) + 1);
  assert(matrix_get_rows(xcoord) == vector_get_dimension(ycoord));

  m = matrix_new(matrix_get_columns(xcoord) + 1, matrix_get_rows(xcoord), false);
  matrix_fill(m, 0, 0, 1, matrix_get_rows(m), 1.0);
  matrix_copy_matrix(m, 1, 0, xcoord, 0, 0, xcoord->columns, xcoord->rows);
  least_squares(beta, m, ycoord);
  matrix_destroy(m);
}

vector_t *vector_new_and_multiple_linear_regression_using_least_squares(matrix_t *xcoord, vector_t *ycoord)
{
  vector_t *beta;

  assert(xcoord);
  assert(ycoord);
  assert(matrix_get_rows(xcoord) == vector_get_dimension(ycoord));

  beta = vector_new(matrix_get_columns(xcoord) + 1, false);
  multiple_linear_regression_using_least_squares(beta, xcoord, ycoord);

  return beta;
}
