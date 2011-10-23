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

#include <common.h>
#include <linear_algebra/matrix.h>
#include <linear_algebra/vector.h>

#ifdef __cplusplus
extern "C" {
#endif

  void least_squares(vector_t *beta, matrix_t *x, vector_t *y);
  void line_regression_using_least_squares(vector_t *beta, vector_t *xcoord, vector_t *ycoord);
  vector_t *vector_new_and_line_regression_using_least_squares(vector_t *xcoord, vector_t *ycoord);
  void polynomial_regression_using_least_squares(vector_t *beta, vector_t *xcoord, vector_t *ycoord);
  vector_t *vector_new_and_polynomial_regression_using_least_squares(vector_t *xcoord, vector_t *ycoord, int order);
  void multiple_linear_regression_using_least_squares(vector_t *beta, matrix_t *xcoord, vector_t *ycoord);
  vector_t *vector_new_and_multiple_linear_regression_using_least_squares(matrix_t *xcoord, vector_t *ycoord);

#ifdef __cplusplus
}
#endif

#endif /* __LEAST_SQUARES_H__ */
