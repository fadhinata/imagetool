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
#include <gauss.h>

static int pivoting_sign = 1;

/* return value (0: complete, +: singular matrix, -: i/o error) */
int matrix_gaussian_eliminate(matrix_t *m)
{
  int r, c, i, j, columns, rows;
  int imax, ret = 0;
  real_t value, vmax, ratio;

  assert(m);

  columns = matrix_get_columns(m);
  rows = matrix_get_rows(m);

  pivoting_sign = 1; // clear pivot sign
	
  r = 0, c = 0;
  while (r < (rows - 1) && c < columns) {
    // find the leading one
    imax = r, vmax = abs(matrix_get_value(c, r, m));
    for (i = r + 1; i < rows; i++) {
      value = abs(matrix_get_value(c, i, m));
      if (value > vmax) {
	imax = i;
	vmax = value;
      }
    }

    if (r != imax) {
      // swap r-rows and imax or pivoting
      for (j = c; j < columns; j++) {
	value = matrix_get_value(j, r, m);
	matrix_put_value(matrix_get_value(j, imax, m), j, r, m);
	matrix_put_value(value, j, imax, m);
	pivoting_sign *= -1;
      }
    }

    // forward substitution
    if (vmax != 0.0) {
      for (i = r + 1; i < rows; i++) {
	value = matrix_get_value(c, i, m);
	if (abs(value) < REAL_EPSILON) continue;
	//if (value == 0) continue;
	ratio = value / matrix_get_value(c, r, m);
	for (j = c; j < columns; j++)
	  matrix_sub_value(ratio * matrix_get_value(j, r, m), j, i, m);
      }
      r++;
    } else {
      ret++; // singular matrix
    }

    c++;
  }

  return ret;
}

int cmatrix_gaussian_eliminate(matrix_t *m)
{
  int c, r, i, j, columns, rows;
  int imax, ret = 0;
  complex_t comp1, comp2, comp3;
  //real_t real, imag, real1, imag1, real2, imag2, denom;
  real_t value, vmax;

  assert(m);
  assert(matrix_is_imaginary(m));

  columns = matrix_get_columns(m);
  rows = matrix_get_rows(m);

  pivoting_sign = 1;
  r = 0, c = 0;

  while (r < (rows - 1) && c < columns) {
    imax = r;
    cmatrix_read_value(&comp1, c, r, m);
    vmax = sqr(comp1.real) + sqr(comp1.imag);
    //vmax = sqr(*(m->real+r*columns+c))+sqr(*(m->imaginary+r*columns+c));
    for (i = r + 1; i < rows; i++) {
      cmatrix_read_value(&comp1, c, i, m);
      value = sqr(comp1.real) + sqr(comp1.imag);
      //value = sqr(*(m->real+i*columns+c))+sqr(*(m->imaginary+i*columns+c));
      if (value > vmax) {
	imax = i;
	vmax = value;
      }
    }
    if (r != imax) {
      // swap rows r and imax
      for (j = c; j < columns; j++) {
	cmatrix_read_value(&comp1, j, r, m);
	cmatrix_read_value(&comp2, j, imax, m);
	cmatrix_put_value(comp2, j, r, m);
	cmatrix_put_value(comp1, j, imax, m);
	/*
	real = matrix_get_value(j, r, m);
	imag = *(m->imaginary+r*columns+j);
	matrix_put_value(matrix_get_value(j, imax, m), j, r, m);
	*(m->imaginary+r*columns+j) = *(m->imaginary+imax*columns+j);
	*(m->real+imax*columns+j) = real;
	*(m->imaginary+imax*columns+j) = imag;
	*/
	pivoting_sign *= -1;
      }
    }
    if (!(abs(vmax) < REAL_EPSILON)) {
      cmatrix_read_value(&comp1, c, r, m);
      /*
      real = *(m->real+r*columns+c);
      imag = *(m->imaginary+r*columns+c);
      */
      for (i = r + 1; i < rows; i++) {
	cmatrix_read_value(&comp2, c, i, m);
	/*
	real1 = matrix_get_value(c, i, m);
	imag1 = *(m->imaginary+i*columns+c);
	*/
	if (abs(comp2.real) < REAL_EPSILON && abs(comp2.imag) < REAL_EPSILON)
	  continue;
	complex_divide_complex(&comp2, &comp1); // comp2 <= comp2 / comp1
	//if (real == 0 && imag == 0) continue;
	//denom = sqr(real)+sqr(imag);
	//real2 = real*real1+imag*imag1;
	//imag2 = real*imag1-imag*real1;
	for (j = c; j < columns; j++) {
	  cmatrix_read_value(&comp3, j, r, m);
	  complex_multiply_complex(&comp3, &comp2);
	  // comp3 <= comp3 * comp2 / comp1
	  cmatrix_sub_value(comp3, j, i, m);
	  //real1 = matrix_get_value(j, r, m);
	  //imag1 = *(m->imaginary+r*columns+j);
	  //*(m->real+i*columns+j) -= (real2*real1-imag2*imag1)/denom;
	  //*(m->imaginary+i*columns+j) -= (real2*imag1+imag2*real1)/denom;
	}
      }
      r++;
    } else {
      // singular matrix
      ret++;
    }
    c++;
  }

  return ret;
}

void matrix_back_substitute(matrix_t *m)
{
  int c, r, i, j, columns, rows;
  real_t value;

  assert(m);

  columns = matrix_get_columns(m);
  rows = matrix_get_rows(m);

  // find the last leading one
  i = min(columns, rows) - 1;
  c = i, r = i;
  while (c >= 0 && r >= 0) {
    // normalization
    value = matrix_get_value(c, r, m);
    //value = *(m->real+r*columns+c);
    matrix_put_value(1.0, c, r, m);
    //*(m->real+r*columns+c) = 1.0;
    for (j = c + 1; j < columns; j++) {
      matrix_div_value(value, j, r, m);
      //*(m->real+r*columns+j) /= value;
    }
    r--;
    // substitution
    for (i = r; i >= 0; i--) {
      value = matrix_get_value(c, i, m);
      matrix_put_value(0.0, c, i, m);
      //*(m->real+i*columns+c) = 0.0;
      for (j = c + 1; j < columns; j++) {
	matrix_sub_value(value * matrix_get_value(j, r + 1, m), j, i, m);
	//*(m->real+i*columns+j) -= value*(*(m->real+(r + 1)*columns+j));
      }
    }
    c--;
  }
}

void cmatrix_back_substitute(matrix_t *m)
{
  int c, r, i, j, columns, rows;
  complex_t comp1, comp2;
  //real_t real, imag, real1, imag1, denom;

  assert(m);
  assert(matrix_is_imaginary(m));

  columns = matrix_get_columns(m);
  rows = matrix_get_rows(m);

  // find the last leading one
  i = min(columns, rows) - 1;
  c = i, r = i;
  while (c >= 0 && r >= 0) {
    // normalization
    cmatrix_read_value(&comp1, c, r, m);
    matrix_put_value(1.0, c, r, m);
    imatrix_put_value(0.0, c, r, m);
    //real = *(m->real+r*columns+c);
    //imag = *(m->imaginary+r*columns+c);
    //*(m->real+r*columns+c) = 1.0;
    //*(m->imaginary+r*columns+c) = 0.0;
    //denom = sqr(real)+sqr(imag);
    for (j = c + 1; j < columns; j++) {
      cmatrix_read_value(&comp2, j, r, m);
      complex_divide_complex(&comp2, &comp1); // comp2 <= comp2 / comp1
      cmatrix_put_value(comp2, j, r, m);
      //real1 = matrix_get_value(j, r, m);
      //imag1 = *(m->imaginary+r*columns+j);
      //*(m->real+r*columns+j) = (real1*real+imag1*imag)/denom;
      //*(m->imaginary+r*columns+j) = (imag1*real-real1*imag)/denom;
    }
    r--;
    // substitution
    for (i = r; i >= 0; i--) {
      cmatrix_read_value(&comp1, c, i, m);
      matrix_put_value(0.0, c, i, m);
      imatrix_put_value(0.0, c, i, m);
      //real = matrix_get_value(c, i, m);
      //imag = *(m->imaginary+i*columns+c);
      //*(m->real+i*columns+c) = 0.0;
      //*(m->imaginary+i*columns+c) = 0.0;
      for (j = c + 1; j < columns; j++) {
	cmatrix_read_value(&comp2, j, r + 1, m);
	complex_multiply_complex(&comp2, &comp1); // comp2 <= comp2 * comp1
	cmatrix_sub_value(comp2, j, i, m);
	//real1 = *(m->real+(r + 1)*columns+j);
	//imag1 = *(m->imaginary+(r + 1)*columns+j);
	//*(m->real+i*columns+j) -= real*real1-imag*imag1;
	//*(m->imaginary+i*columns+j) -= real*imag1+imag*real1;
      }
    }
    c--;
  }
}

int matrix_gauss_jordan_eliminate(matrix_t *m)
{
  int ret;

  assert(m);

  ret = matrix_gaussian_eliminate(m);
  if (ret == 0) matrix_back_substitute(m);

  return ret;
}

int cmatrix_gauss_jordan_eliminate(matrix_t *m)
{
  int ret = 0;

  assert(m);
  assert(m->imaginary);

  ret = cmatrix_gaussian_eliminate(m);
  if (ret == 0) cmatrix_back_substitute(m);

  return ret;
}
