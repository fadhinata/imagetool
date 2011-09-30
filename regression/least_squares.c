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
#include <vector.h>
#include <least_squares.h>
#include <vector_statistic.h>

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
void linear_least_squares(vector_t *y, matrix_t *x, vector_t *beta)
{
  matrix_t *xt, *m, *inv;
  vector_t *vec;
	
  assert(y);
  assert(x);
  assert(beta);
  assert(x->rows == y->length);
  assert(x->columns == beta->length);
	
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

//void line_regression_get(double *real, double *imag, vector_t *beta, 
void line_regression_using_least_squares(vector_t *beta, vector_t *xcoord, vector_t *ycoord)
{
  int i, n;
  double mx; // the means of x
  double my; // the means of y
  double sxx; // the squared sum of x by x
  double sxy; // the squared sum of x by y
  //vector_t *beta;
  assert(beta);
  assert(beta->length == 2);
  assert(xcoord);
  assert(ycoord);
  assert(xcoord->length == ycoord->length);
  //beta = vector_create(2, 0);
  /* the means of xcoord and ycoord */
  n = xcoord->length;
  mx = my = sxx = sxy = 0.0;
  for (i = 0; i < n; i++) {
    mx += *(xcoord->real+i);
    my += *(ycoord->real+i);
    sxx += (*(xcoord->real+i))*(*(xcoord->real+i));
    sxy += (*(xcoord->real+i))*(*(ycoord->real+i));
  }
  mx /= (double)n;
  my /= (double)n;
  /* the sample regresion coefficient */
  *(beta->real+1) = (sxy - n*mx*my) / (sxx - n*mx*mx);
  *(beta->real+0) = my-(*(beta->real+1))*mx;
  //return beta;
}

vector_t *vector_create_line_regression_using_least_squares(vector_t *xcoord, vector_t *ycoord)
{
  vector_t *beta;
  assert(xcoord);
  assert(ycoord);
  assert(xcoord->length == ycoord->length);
  beta = vector_new(2, 0);
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
  assert(beta->length < xcoord->length);
  assert(xcoord->length == ycoord->length);
  m = matrix_new(beta->length ,xcoord->length, false);
  v = vector_new(xcoord->length, false);
  vector_fill(v, 0, v->length, 1.0);
  for (i = 0; i < beta->length; i++) {
    matrix_copy_column_vector(m, i, 0, v);
    vector_multiply_vector(v, xcoord);
  }
  linear_least_squares(ycoord, m, beta);
  vector_destroy(v);
  matrix_destroy(m);
}

vector_t *vector_create_polynomial_regression_using_least_squares(vector_t *xcoord, vector_t *ycoord, int n)
{
  vector_t *beta;
  assert(xcoord);
  assert(ycoord);
  assert(xcoord->length == ycoord->length);
  assert(n < xcoord->length);
  beta = vector_new(n, false);
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
  assert(beta->length == xcoord->columns+1);
  assert(xcoord->rows == ycoord->length);
  m = matrix_new(xcoord->columns+1, xcoord->rows, false);
  matrix_fill(m, 0, 0, 1, m->rows, 1);
  matrix_copy_matrix(m, 1, 0, xcoord, 0, 0, xcoord->columns, xcoord->rows);
  linear_least_squares(ycoord, m, beta);
  matrix_destroy(m);
}

vector_t *vector_create_multiple_linear_regression_using_least_squares(matrix_t *xcoord, vector_t *ycoord)
{
  vector_t *beta;
  assert(xcoord);
  assert(ycoord);
  assert(xcoord->rows == ycoord->length);
  beta = vector_new(xcoord->columns+1, false);
  multiple_linear_regression_using_least_squares(beta, xcoord, ycoord);
  return beta;
}

/* nsample : the # of dimension */
pls1_t *pls1_create(int ndim)
{
  pls1_t *pls1;
  pls1 = (pls1_t *)malloc(sizeof(pls1_t));
  pls1->mx = vector_new(ndim, false);
  pls1->beta = vector_new(ndim, false);
  return pls1;
}

void pls1_delete(pls1_t *p)
{
  assert(p);
  vector_destroy(p->beta);
  vector_destroy(p->mx);
  free(p);
}

/* x : the # of sample by the # of dimension
 * y : the # of sample by one                 */
void pls1_calibration(pls1_t *pls1, matrix_t *x, vector_t *y, int n)
{
  int i;
  matrix_t *xcurr; /* the # of sample    by the # of dimension */
  matrix_t *xt;    /* the # of dimension by the # of sample    */
  vector_t *ycurr; /* the # of sample    by one                */
  vector_t *mx;    /* one                by the # of dimension */
  vector_t *w;     /* one                by the # of dimension */
  vector_t *t;     /* the # of sample    by one                */
  vector_t *p;     /* one                by the # of dimension */
  vector_t *vec;   /* the # of sample    by one                */
  double q, v;
	
  assert(pls1);
  assert(x);
  assert(y);
  assert(pls1->mx->length == x->columns);
  assert(x->rows == y->length);
  assert(n > 1);

  xcurr = matrix_new_and_copy(x);
  xt = matrix_new(x->rows, x->columns, false);
  mx = pls1->mx;
  //mx = vector_create(x->columns, 0);
  ycurr = vector_new_and_copy(y);
  w = vector_new(x->columns, false);
  t = vector_new(x->rows, false);
  p = vector_new(x->columns, false);
  vec = vector_new(x->rows, false);

  /* mean centering for x matrix */
  for (i = 0; i < x->columns; i++) {
    vector_copy_column_vector_of_matrix(vec, xcurr, i, 0);
    v = vector_get_mean(vec);
    vector_subtract_scalar(vec, v);
    vector_put_value(v, i, mx);//VECTOR_PUT(v, mx, i);
    //VECTOR_PUT_REAL(v, mx, i);
    matrix_copy_column_vector(xcurr, i, 0, vec);
  }
	
  /* mean centering for y vector */
  pls1->my = vector_get_mean(ycurr);
  vector_subtract_scalar(ycurr, pls1->my);
	
  // scoring
  for (i = 1; i < n; i++) {
    matrix_copy_matrix_transpose(xt, xcurr);
    // auxiliary weight vector
    vector_copy_matrix_multiply_vector(w, xt, ycurr);
    v = vector_get_norm(w);
    vector_divide_scalar(w, v);
    // factor coefficient vector
    vector_copy_matrix_multiply_vector(t, xcurr, w);
    // loading vector
    vector_copy_matrix_multiply_vector(p, xt, t);
    v = vector_get_norm(t);
    //vector_get_norm(&v, p);
    vector_divide_scalar(p, v);
    // scalar coefficient of relationship between factor and activity
    vector_dotproduct_vector(&q, ycurr, t);
    q /= v;
    matrix_subtract_vector_multiply_vector(xcurr, t, p);
    vector_subtract_vector_multiply_scalar(ycurr, t, q);
  }
	
  // for pls factors
  // beta = w*(p`*w)^(-1)*q
  vector_dotproduct_vector(&v, p, w);
  vector_copy_vector_divide_scalar(pls1->beta, w, v);
  vector_multiply_scalar(pls1->beta, q);
  // b0 = my-mx`*beta
  vector_dotproduct_vector(&v, pls1->mx, pls1->beta);
  pls1->beta0 = pls1->my-v;
	
  vector_destroy(vec);
  vector_destroy(p);
  vector_destroy(t);
  vector_destroy(w);
  vector_destroy(ycurr);
  //vector_delete(mx);
  matrix_destroy(xt);
  matrix_destroy(xcurr);
}
