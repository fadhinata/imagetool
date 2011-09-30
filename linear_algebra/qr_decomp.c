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
#include <matrix.h>
#include <gram_schmidt.h>

int matrix_qr_decomposition(matrix_t **q, matrix_t **r, matrix_t *p)
{
	int n;
	matrix_t *m, *qt;
	assert(q);
	assert(r);
	assert(p);
	m = matrix_new_and_copy(p);
	n = matrix_gram_schmidt_process(m, p);
	*q = matrix_new(n, p->rows, false);
	matrix_copy_matrix(*q, 0, 0, m, 0, 0, n, m->rows);
	qt = matrix_new_and_copy_matrix_transpose(*q); // or inverse matrix
	*r = matrix_new_and_copy_matrix_multiply_matrix(qt, p);
	matrix_destroy(qt);
	matrix_destroy(m);
	return 0;
}

int cmatrix_qr_decomposition(matrix_t **q, matrix_t **r, matrix_t *p)
{
	int n;
	matrix_t *m, *qt;
	assert(q);
	assert(r);
	assert(p);
	assert(p->imaginary);
	m = matrix_new_and_copy(p);
	n = cmatrix_gram_schmidt_process(m, p);
	*q = matrix_new(n, p->rows, true);
	cmatrix_copy_cmatrix(*q, 0, 0, m, 0, 0, n, m->rows);
	qt = cmatrix_new_and_copy_cmatrix_transpose(*q);
	*r = cmatrix_new_and_copy_cmatrix_multiply_cmatrix(qt, p);
	matrix_destroy(qt);
	matrix_destroy(m);
	return 0;
}
