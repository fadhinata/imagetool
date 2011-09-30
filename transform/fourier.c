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
#include <float.h>
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>

#include <vector.h>
#include <common.h>
#include <matrix.h>

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
  assert(vector_get_length(q) == vector_get_length(p));

  if (!vector_is_imaginary(q)) vector_attach_imaginary(q);

  n = vector_get_length(p);
  if (vector_is_imaginary(p)) {
    for (i = 0; i < n; i++) {
      dt = - 2.0 * M_PI * (real_t)i / (real_t)n;
      comp.real = comp.imag = 0.0;
      for (k = 0; k < n; k++) {
	cosval = cos(k * dt);
	sinval = sin(k * dt);
	comp.real += (vector_get_value(k, p) * cosval -
		      ivector_get_value(k, p) * sinval);
	comp.imag += (vector_get_value(k, p) * sinval +
		      ivector_get_value(k, p) * cosval);
      }
      cvector_put_value(comp, i, q);
    }
  } else {
    for (i = 0; i < n; i++) {
      dt = - 2.0 * M_PI * (real_t)i / (real_t)n;
      comp.real = comp.imag = 0.0;
      for (k = 0; k < n; k++) {
	comp.real += vector_get_value(k, p) * cos(k*dt);
	comp.imag += vector_get_value(k, p) * sin(k*dt);
      }
      cvector_put_value(comp, i, q);
    }
  }

  for (i = 0; i < n; i++) {
    comp.real = vector_get_value(i, q) / (real_t)n;
    comp.imag = ivector_get_value(i, q) / (real_t)n;
    cvector_put_value(comp, i, q);
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
  assert(vector_get_length(q) == vector_get_length(p));

  if (!vector_is_imaginary(q)) vector_attach_imaginary(q);

  n = vector_get_length(p);
  for (i = 0; i < n; i++) {
    dt = 2.0 * M_PI * (real_t)i / (real_t)n;
    comp.real = comp.imag = 0.0;
    for (k = 0; k < n; k++) {
      cosval = cos(k * dt);
      sinval = sin(k * dt);
      comp.real += (vector_get_value(k, p) * cosval -
		    ivector_get_value(k, p) * sinval);
      comp.imag += (vector_get_value(k, p) * sinval +
		    ivector_get_value(k, p) * cosval);
    }
    cvector_put_value(comp, i, q);
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

int fft(vector_t *q, vector_t *p)
{
  int i, j, n, m, half, i1, k, i2, l, l1, l2;
  double value, c1, c2, t1, t2, u1, u2, z;

  assert(q);
  assert(p);
  assert(q->imaginary);
	
  if (q->length != p->length) return -1;

  /* m = log2(n); */
  /* the number of points for fft */
  for (m = 0, n = 1; n < p->length; n <<= 1) m++;
	
  assert(q->length == n);
  if (q->length != n) return -1;

  memcpy(q->real, p->real, p->length*sizeof(*(p->real)));
	
  if (p->imaginary) {
    memcpy(q->imaginary, p->imaginary, p->length*sizeof(*(p->imaginary)));
  } else {
    memset(q->imaginary, 0, q->length*sizeof(*(q->imaginary)));
  }
	
  /* do the bit reversal  see wikipermedia*/
  half = n>>1, j = 0;
  for (i = 0; i < n-1; i++) {
    if (i < j) {
      value = *(q->real+i), *(q->real+i) = *(q->real+j), *(q->real+j) = value;
      value = *(q->imaginary+i), *(q->imaginary+i) = *(q->imaginary+j), *(q->imaginary+j) = value;
    }
    k = half;
    while (k <= j) { j -= k; k >>= 1; }
    j += k;
  }

  c1 = -1.0, c2 = 0.0, l2 = 1;
  for (l = 0; l < m; l++) {
    l1 = l2;
    l2 <<= 1;
    u1 = 1.0; 
    u2 = 0.0;
    for (j = 0; j < l1; j++) {
      for (i = j; i < m; i += l2) {
	i1 = i + l1;
	t1 = u1 * (*(q->real+i1)) - u2 * (*(q->imaginary+i1));
	t2 = u1 * (*(q->imaginary+i1)) + u2 * (*(q->real+i1));
	*(q->real+i1) = *(q->real+i) - t1; 
	*(q->imaginary+i1) = *(q->imaginary+i) - t2;
	*(q->real+i) += t1;
	*(q->imaginary+i) += t2;
      }
      z =  u1 * c1 - u2 * c2;
      u2 = u1 * c2 + u2 * c1;
      u1 = z;
    }
    c2 = sqrt((1.0 - c1) / 2.0);
    c2 = -c2;
    c1 = sqrt((1.0 + c1) / 2.0);
  }

  /* Scaling for forward transform */
  for (i = 0; i < n; i++) {
    *(q->real+i) /= n;
    *(q->imaginary+i) /= n;
  }

  return n;
}

int ifft(vector_t *q, vector_t *p)
{
  int i, j, n, m, k, i1, i2, l, l1, l2, half;
  double value, c1, c2, t1, t2, u1, u2, z;
  double *real, *imag;
	
  assert(q);
  assert(p);
  assert(p->imaginary);
  assert(q->length == p->length);
	
  /* m = log2(n) */
  for (m = 0, n = 1; n < p->length; n <<= 1) m++;
  assert(n == p->length);

  memcpy(q->real, p->real, p->length*sizeof(*(p->real)));
  real = q->real;
	
  if (q->imaginary) {
    memcpy(q->imaginary, p->imaginary, p->length*sizeof(*(p->imaginary)));
    imag = q->imaginary;
  } else {
    imag = (double *)malloc(n*sizeof(*p->imaginary));
    memcpy(imag, p->imaginary, p->length*sizeof(*(p->imaginary)));
  }
	
  /* do the bit reversal  see wikipermedia*/
  half = n>>1, j = 0;
  for (i = 0; i < n-1; i++) {
    if (i < j) {
      value = *(real+i);
      *(real+i) = *(real+j);
      *(real+j) = value;
      value = *(imag+i);
      *(imag+i) = *(imag+j);
      *(imag+j) = value;
    }
    k = half;
    while (k <= j) { j -= k; k >>= 1; }
    j += k;
  }
  /* Compute the FFT */
  c1 = -1.0, c2 = 0.0, l2 = 1;
  for (l = 0; l < m; l++) {
    l1 = l2;
    l2 <<= 1;
    u1 = 1.0;
    u2 = 0.0;
    for (j = 0; j < l1; j++) {
      for (i = j; i < n; i += l2) {
	i1 = i + l1;
	t1 = u1 * (*(real+i1)) - u2 * (*(imag+i1));
	t2 = u1 * (*(imag+i1)) + u2 * (*(real+i1));
	*(real+i1) = *(real+i) - t1;
	*(imag+i1) = *(imag+i) - t2;
	*(real+i) += t1;
	*(imag+i) += t2;
      }
      z =  u1 * c1 - u2 * c2;
      u2 = u1 * c2 + u2 * c1;
      u1 = z;
    }
    c2 = sqrt((1.0 - c1) / 2.0);
    c1 = sqrt((1.0 + c1) / 2.0);
  }

  if (!q->imaginary) {
    for (i = 0; i < n; i++) {
      if (!(*(imag+i) < DBL_EPSILON)) {
	free(imag);
	return -1;
      }
    }
    free(imag);
  }
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
  int i, j, n, nx, ny, mx, my;
  vector_t *freq, *time;
	
  assert(q);
  assert(q->real);
  assert(q->imaginary);
  assert(p);
  assert(p->real);
	
  for (mx = 0, nx = 1; nx < p->columns; nx <<= 1) mx++;
  assert(nx == p->columns);
  assert(nx <= q->columns);
	
  for (my = 0, my = 1; ny < p->rows; ny <<= 1) my++;
  assert(ny == p->rows);
  assert(ny <= q->rows);

  /* transform the rows */
  time = vector_new(nx, false);
  freq = vector_new(nx, true);
  for (i = 0; i < ny; i++) {
    memcpy(time->real, p->real+i*p->columns, nx*sizeof(double));
    n = fft(freq, time);
    memcpy(q->real+i*q->columns, freq->real, nx*sizeof(double));
    memcpy(q->columns+i*q->columns, freq->imaginary, nx*sizeof(double));
  }
  vector_destroy(freq);
  vector_destroy(time);

  /* Transform the columns */
  time = vector_new(ny, true);
  freq = vector_new(ny, true);
  for (j = 0; j < nx; j++) {
    for (i = 0; i < ny; i++) {
      *(time->real+i) = matrix_get_value(j, i, q);
      *(time->imaginary+i) = imatrix_get_value(j, i, q);
    }
    fft(freq, time);
    for (i = 0; i < ny; i++) {
      matrix_put_value(*(freq->real+i), j, i, q);
      imatrix_put_value(*(freq->imaginary+i), j, i, q);
    }
  }
  vector_destroy(freq);
  vector_destroy(time);
  return 0;
}

int ifft2(matrix_t *q, matrix_t *p)
{
  int i, j, n, nx, ny, mx, my;
  vector_t *freq, *time;
	
  assert(q);
  assert(q->real);
  //assert(q->imaginary);
  assert(p);
  assert(p->real);
  assert(p->imaginary);
	
  for (mx = 0, nx = 1; nx < p->columns; nx <<= 1) mx++;
  assert(nx == p->columns);
  assert(nx <= q->columns);
	
  for (my = 0, my = 1; ny < p->rows; ny <<= 1) my++;
  assert(ny == p->rows);
  assert(ny <= q->rows);

  /* transform the rows */
  time = vector_new(nx, false);
  freq = vector_new(nx, true);
  for (i = 0; i < ny; i++) {
    memcpy(freq->real, p->real+i*p->columns, nx*sizeof(double));
    memcpy(freq->imaginary, p->imaginary+i*p->columns, nx*sizeof(double));
    n = ifft(time, freq);
    memcpy(q->real+i*q->columns, time->real, nx*sizeof(double));
    //memcpy(q->columns+i*q->columns, freq->imaginary, nx*sizeof(double));
  }
  vector_destroy(freq);
  vector_destroy(time);

  /* Transform the columns */
  time = vector_new(ny, true);
  freq = vector_new(ny, true);
  for (j = 0; j < nx; j++) {
    for (i = 0; i < ny; i++) {
      *(freq->real+i) = matrix_get_value(j, i, q);
      *(freq->imaginary+i) = imatrix_get_value(j, i, q);
    }
    ifft(time, freq);
    for (i = 0; i < ny; i++) {
      matrix_put_value(*(time->real+i), j, i, q);
      //MATRIX_IMAG_PUT(*(freq->imaginary+i), j, i, q);
    }
  }
  vector_destroy(freq);
  vector_destroy(time);
  return 0;
}

#if 0
int FFT2D(COMPLEX **c,int nx,int ny,int dir)
{
  int i,j;
  int m,twopm;
  double *real,*imag;

  /* Transform the rows */
  real = (double *)malloc(nx * sizeof(double));
  imag = (double *)malloc(nx * sizeof(double));
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
  real = (double *)malloc(ny * sizeof(double));
  imag = (double *)malloc(ny * sizeof(double));
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
