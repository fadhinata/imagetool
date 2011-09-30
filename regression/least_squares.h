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
#ifndef __LEAST_SQUARES_H__
#define __LEAST_SQUARES_H__

#include <matrix.h>
#include <vector.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    vector_t *mx;
    double my;
    vector_t *beta;
    double beta0;
  } pls1_t;

  void linear_least_squares(vector_t *y, matrix_t *x, vector_t *beta);
  void line_regression_using_least_squares(vector_t *beta, vector_t *xcoord, vector_t *ycoord);
  vector_t *vector_create_line_regression_using_least_squares(vector_t *xcoord, vector_t *ycoord);
  void polynomial_regression_using_least_squares(vector_t *beta, vector_t *xcoord, vector_t *ycoord);
  vector_t *vector_create_polynomial_regression_using_least_squares(vector_t *xcoord, vector_t *ycoord, int n);
  void multiple_linear_regression_using_least_squares(vector_t *beta, matrix_t *xcoord, vector_t *ycoord);
  vector_t *vector_create_multiple_linear_regression_using_least_squares(matrix_t *xcoord, vector_t *ycoord);
  pls1_t *pls1_create(int ndim);
  void pls1_delete(pls1_t *p);
  void pls1_calibration(pls1_t *pls1, matrix_t *x, vector_t *y, int n);

#ifdef __cplusplus
}
#endif

#endif /* __LEAST_SQUARES_H__ */
