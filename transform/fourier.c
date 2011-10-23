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
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>

#include <common.h>
#include <linear_algebra/vector.h>
#include <linear_algebra/matrix.h>

/* Reference from http://paulbourke.net/miscellaneous/dft/
                  Written by Paul Bourke
                  June 1993
*/

// Direct fourier transform
void dft(vector_t *q, vector_t *p)
{
  int i, k, n;
  real_t val, dt, cosval, sinval;
  complex_t comp;

  assert(q);
  assert(p);
  //assert(vector_is_imaginary(q));
  assert(vector_get_dimension(q) == vector_get_dimension(p));

  if (!vector_is_imaginary(q)) vector_attach_imaginary(q);

  n = vector_get_dimension(p);
  if (vector_is_imaginary(p)) {
    for (i = 0; i < n; i++) {
      dt = - 2.0 * M_PI * (real_t)i / (real_t)n;
      comp.real = comp.imag = 0.0;
      for (k = 0; k < n; k++) {
	cosval = cos(k * dt);
	sinval = sin(k * dt);
	comp.real += (vector_get_value(p, k) * cosval -
		      ivector_get_value(p, k) * sinval);
	comp.imag += (vector_get_value(p, k) * sinval +
		      ivector_get_value(p, k) * cosval);
      }
      cvector_put_value(comp, q, i);
    }
  } else {
    for (i = 0; i < n; i++) {
      dt = - 2.0 * M_PI * (real_t)i / (real_t)n;
      comp.real = comp.imag = 0.0;
      for (k = 0; k < n; k++) {
	comp.real += vector_get_value(p, k) * cos(k*dt);
	comp.imag += vector_get_value(p, k) * sin(k*dt);
      }
      cvector_put_value(comp, q, i);
    }
  }

  for (i = 0; i < n; i++) {
    comp.real = vector_get_value(q, i) / (real_t)n;
    comp.imag = ivector_get_value(q, i) / (real_t)n;
    cvector_put_value(comp, q, i);
  }
}

void idft(vector_t *q, vector_t *p)
{
  int i, k, n;
  real_t dt, cosval, sinval;
  complex_t comp;
  //double real, imag;

  assert(q);
  assert(p);
  assert(vector_is_imaginary(p));
  //assert(vector_is_imaginary(q));
  assert(vector_get_dimension(q) == vector_get_dimension(p));

  if (!vector_is_imaginary(q)) vector_attach_imaginary(q);

  n = vector_get_dimension(p);
  for (i = 0; i < n; i++) {
    dt = 2.0 * M_PI * (real_t)i / (real_t)n;
    comp.real = comp.imag = 0.0;
    for (k = 0; k < n; k++) {
      cosval = cos(k * dt);
      sinval = sin(k * dt);
      comp.real += (vector_get_value(p, k) * cosval -
		    ivector_get_value(p, k) * sinval);
      comp.imag += (vector_get_value(p, k) * sinval +
		    ivector_get_value(p, k) * cosval);
    }
    cvector_put_value(comp, q, i);
  }
}

/*
  This computes an in-place complex-to-complex FFT 
  x and y are the real and imaginary arrays of 2^m points.
  dir =  1 gives forward transform
  dir = -1 gives reverse transform 
*/
/*-------------------------------------------------------------------------
  This computes an in-place complex-to-complex FFT
  x and y are the real and imaginary arrays of 2^m points.
  dir =  1 gives forward transform
  dir = -1 gives reverse transform

  Formula: forward
  N-1
  ---
  1   \          - j k 2 pi n / N
  X(n) = ---   >   x(k) e                    = forward transform
  N   /                                n=0..N-1
  ---
  k=0

  Formula: reverse
  N-1
  ---
  \          j k 2 pi n / N
  X(n) =       >   x(k) e                    = forward transform
  /                                n=0..N-1
  ---
  k=0
*/

static int self_fft(bool forward, vector_t *v)
{
  int i, j, k, d1, d2;
  uint32_t dim, exponent, half;
  complex_t a, b, c, u, t;

  for (exponent = 0, dim = 1; dim < vector_get_dimension(v); dim <<= 1)
    exponent++;

  if (vector_get_dimension(v) != dim) return -1;

  // do the bit reversal
  half = dim >> 1;
  j = 0;
  for (i = 0; i < dim - 1; i++) {
    if (i < j) {
      cvector_read_value(&a, v, i);
      cvector_read_value(&b, v, j);
      cvector_put_value(b, v, i);
      cvector_put_value(a, v, j);
    }
    k = half;
    while (k <= j) { j -= k; k >>= 1; }
    j += k;
  }

  // compute the FFT
  c.real = -1.0, c.imag = 0.0;
  d2 = 1;
  for (i = 0; i < exponent; i++) {
    d1 = d2;
    d2 <<= 1;
    u.real = 1.0, u.imag = 0.0;
    for (j = 0; j < d1; j++) {
      for (k = j; k < dim; k += d2) {
	cvector_read_value(&a, v, k + d1);
	complex_copy_complex_multiply_complex(&t, &u, &a);
	cvector_read_value(&a, v, k);
	complex_subtract_complex(&a, &t);
	cvector_put_value(a, v, k + d1);
	cvector_add_value(t, v, k);
      }
      complex_copy_complex_multiply_complex(&a, &u, &c);
      u.real = a.real, u.imag = a.imag;
    }
    c.imag = sqrt((1.0 - c.real) / 2.0);
    if (forward) c.imag = -c.imag;
    c.real = sqrt((1.0 + c.real) / 2.0);
  }
  // Scaling for forward transform
  if (forward) {
    for (i = 0; i < dim; i++) {
      vector_div_value(dim, v, i);
      ivector_div_value(dim, v, i);
    }
  }
  return 0;
}

int fft(vector_t *q, vector_t *p)
{
  assert(q);
  assert(p);
  assert(vector_is_imaginary(q));
  assert(vector_get_dimension(q) == vector_get_dimension(p));

  vector_copy(q, p);
  if (vector_is_imaginary(p)) ivector_copy(q, p);
  else ivector_clear(q);

  return self_fft(true, q);
}

int ifft(vector_t *q, vector_t *p)
{
  assert(q);
  assert(p);
  assert(vector_is_imaginary(p));
  assert(vector_get_dimension(q) == vector_get_dimension(p));
	
  vector_copy(q, p);
  if (!vector_is_imaginary(q)) vector_attach_imaginary(q);
  ivector_copy(q, p);

  return self_fft(false, q);
}

static int self_fft2(bool forward, matrix_t *m)
{
  int i, nx, ny, mx, my;
  vector_t *v;

  for (mx = 0, nx = 1; nx < matrix_get_columns(m); nx <<= 1) mx++;
  if (matrix_get_columns(m) != nx) return -1;

  for (my = 0, ny = 1; ny < matrix_get_rows(m); ny <<= 1) my++;
  if (matrix_get_rows(m) != ny) return -1;

  v = vector_new(nx, true);
  for (i = 0; i < ny; i++) {
    cvector_copy_row_vector_of_cmatrix(v, m, 0, i);
    self_fft(forward, v);
    cmatrix_copy_row_cvector(m, 0, i, v);
  }
  vector_destroy(v);

  v = vector_new(ny, true);
  for (i = 0; i < nx; i++) {
    cvector_copy_column_vector_of_cmatrix(v, m, i, 0);
    self_fft(forward, v);
    cmatrix_copy_column_cvector(m, i, 0, v);
  }
  vector_destroy(v);

  return 0;
}

/*-------------------------------------------------------------------------
  Perform a 2D FFT inplace given a complex 2D array
  The direction dir, 1 for forward, -1 for reverse
  The size of the array (nx,ny)
  Return false if there are memory problems or
  the dimensions are not powers of 2
*/
int fft2(matrix_t *q, matrix_t *p)
{
  assert(q);
  assert(matrix_is_imaginary(q));
  assert(p);

  matrix_copy(q, p);
  if (matrix_is_imaginary(p)) imatrix_copy(q, p);
  else imatrix_clear(q);

  return self_fft2(true, q);
}

int ifft2(matrix_t *q, matrix_t *p)
{
  int i, j, n, nx, ny, mx, my;
  vector_t *freq, *time;
	
  assert(q);
  assert(p);
  assert(matrix_is_imaginary(p));
	
  matrix_copy(q, p);
  if (!matrix_is_imaginary(q)) matrix_attach_imaginary(q);
  imatrix_copy(q, p);

  return self_fft2(false, q);
}

#if 0
int FFT2D(COMPLEX **c,int nx,int ny,int dir)
{
  int i,j;
  int m,twopm;
  real_t *real,*imag;

  /* Transform the rows */
  real = (real_t *)malloc(nx * sizeof(real_t));
  imag = (real_t *)malloc(nx * sizeof(real_t));
  if (real == NULL || imag == NULL)
    return(FALSE);
  if (!Powerof2(nx,&m,&twopm) || twopm != nx)
    return(FALSE);
  for (j=0;j<ny;j++) {
    for (i=0;i<nx;i++) {
      real[i] = c[i][j].real;
      imag[i] = c[i][j].imag;
    }
    FFT(dir,m,real,imag);
    for (i=0;i<nx;i++) {
      c[i][j].real = real[i];
      c[i][j].imag = imag[i];
    }
  }
  free(real);
  free(imag);

  /* Transform the columns */
  real = (real_t *)malloc(ny * sizeof(real_t));
  imag = (real_t *)malloc(ny * sizeof(real_t));
  if (real == NULL || imag == NULL)
    return(FALSE);
  if (!Powerof2(ny,&m,&twopm) || twopm != ny)
    return(FALSE);
  for (i=0;i<nx;i++) {
    for (j=0;j<ny;j++) {
      real[j] = c[i][j].real;
      imag[j] = c[i][j].imag;
    }
    FFT(dir,m,real,imag);
    for (j=0;j<ny;j++) {
      c[i][j].real = real[j];
      c[i][j].imag = imag[j];
    }
  }
  free(real);
  free(imag);

  return(TRUE);
}
#endif
