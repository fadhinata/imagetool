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
#ifndef __GAUSS_H__
#define __GAUSS_H__

#include <matrix.h>

#ifdef __cplusplus
extern "C" {
#endif

  // matrix gaussian eliminate
  int matrix_gaussian_eliminate(matrix_t *m);
  int cmatrix_gaussian_eliminate(matrix_t *m);
  // back substitute
  void matrix_back_substitute(matrix_t *m);
  void cmatrix_back_substitute(matrix_t *m);
  // gauss jordan eliminate
  int matrix_gauss_jordan_eliminate(matrix_t *m);
  int cmatrix_gauss_jordan_eliminate(matrix_t *m);

#ifdef __cplusplus
}
#endif

#endif /* __GAUSS_H__ */
