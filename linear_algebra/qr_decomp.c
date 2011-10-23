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
#include <assert.h>

#include <common.h>
#include <linear_algebra/matrix.h>
#include <linear_algebra/gram_schmidt.h>
#include <linear_algebra/vector_list.h>

void matrix_new_and_qr_decomposition(matrix_t **q, matrix_t **r, matrix_t *p)
{
  int n;
  matrix_t *m, *qt;

  assert(q);
  assert(r);
  assert(p);

  *q = matrix_new_and_gram_schmidt_process(p);
  qt = matrix_new_and_copy_matrix_transpose(*q); // or inverse matrix
  *r = matrix_new_and_copy_matrix_multiply_matrix(qt, p);
  matrix_destroy(qt);
}

void cmatrix_new_and_qr_decomposition(matrix_t **q, matrix_t **r, matrix_t *p)
{
  int n;
  matrix_t *m, *qt;

  assert(q);
  assert(r);
  assert(p);
  assert(matrix_is_imaginary(p));

  *q = cmatrix_new_and_gram_schmidt_process(p);
  qt = cmatrix_new_and_copy_cmatrix_transpose(*q); // or inverse matrix
  *r = cmatrix_new_and_copy_cmatrix_multiply_cmatrix(qt, p);
  matrix_destroy(qt);
}
