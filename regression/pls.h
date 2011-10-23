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
#ifndef __PLS_H__
#define __PLS_H__

#include <linear_algebra/matrix.h>
#include <linear_algebra/vector.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    int iteration;
    vector_t *mx; // means of xi
    real_t my; // mean of y
    // for full prediction
    matrix_t *W; // weighting matrix
    matrix_t *P;
    vector_t *Q;
    // for short prediction
    vector_t *beta; //
    real_t beta0;
  } pls_t;

  pls_t *pls_new(int ndim);
  void pls_destroy(pls_t *p);

  void pls_orthogonal_calibrate(pls_t *pls1, matrix_t *x, vector_t *y, int iteration);
  pls_t *pls_new_and_orthogonal_calibrate(matrix_t *x, vector_t *y, int iteration);
  real_t pls_full_orthogonal_predict(pls_t *pls1, vector_t *x);
  real_t pls_short_predict(pls_t *pls1, vector_t *x);

  void pls_nonorthogonal_calibrate(pls_t *pls1, matrix_t *x, vector_t *y, int iteration);

#ifdef __cplusplus
}
#endif

#endif /* __PLS_H__ */

