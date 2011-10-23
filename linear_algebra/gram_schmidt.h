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
#ifndef __GRAM_SCHMIDT_H__
#define __GRAM_SCHMIDT_H__

#include <linear_algebra/vector.h>
#include <linear_algebra/matrix.h>
#include <linear_algebra/vector_list.h>

#ifdef __cplusplus
extern "C" {
#endif

  int vector_list_create_gram_schmidt_process(vector_list_t *q, vector_list_t *p);
  int cvector_list_create_gram_schmidt_process(vector_list_t *q, vector_list_t *p);
  matrix_t *matrix_new_and_gram_schmidt_process(matrix_t *p);
  matrix_t *cmatrix_new_and_gram_schmidt_process(matrix_t *p);

  int vector_gram_schmidt_process(vector_t **q, vector_t **p, int n);
  int cvector_gram_schmidt_process(vector_t **q, vector_t **p, int n);
  int matrix_gram_schmidt_process(matrix_t *q, matrix_t *p);
  int cmatrix_gram_schmidt_process(matrix_t *q, matrix_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __GRAM_SCHMIDT_H__ */
