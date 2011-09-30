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
#include <malloc.h>
#include <string.h>
#include <haar.h>
#include <assert.h>
#include <common.h>

#define H0 0.7071067810
#define H1 0.7071067810

#define SQRT2 1.4142135623

/* haar fast wavelet transform */
int hfwt(vector_t *q, vector_t *p)
{
  int n, m, i, k;
  double *temp;

  assert(q);
  if (!q) return -1;
  assert(p);
  if (!p) return -1;

  for (m = 0, n = 1; n < p->length; n <<= 1) m++;
  assert(n == p->length);
  if (n != p->length) return -1;
  assert(n == q->length);
  if (n != q->length) return -1;

  /*****************************************/
  temp = (double *)malloc(n*sizeof(double));
  assert(temp);
	
  memcpy(q->real, p->real, p->length*sizeof(double));
  while (n > 1) {
    for (i = 0; i < n/2; i++) {
      temp[i]     = (*(q->real+2*i)+*(q->real+2*i+1))/SQRT2;
      temp[n/2+i] = (*(q->real+2*i)-*(q->real+2*i+1))/SQRT2;
    }
    for (i = 0; i < n; i++) *(q->real+i) = temp[i];
    n = n/2;
  }
  free(temp);
  return 0;
}

/* inverse haar fast wavelet transform */
int ihfwt(vector_t *q, vector_t *p)
{
  int n, m, i, k;
  double *temp;
	
  assert(q);
  if (!q) return -1;
  assert(p);
  if (!p) return -1;
	
  for (m = 0, n = 1; n < p->length; n <<= 1) m++;
  assert(n == p->length);
  if (n != p->length) return -1;
  assert(n == q->length);
  if (n != q->length) return -1;
	
  /*****************************************/
  temp = (double *)malloc(n*sizeof(double));
  assert(temp);

  memcpy(q->real, p->real, p->length*sizeof(double));
  k = 1;
  while (k < n) {
    k = 2*k;
    for (i = 0; i < k/2; i++) {
      temp[2*i]   = (*(q->real+i)+*(q->real+k/2+i));
      temp[2*i+1] = (*(q->real+i)-*(q->real+k/2+i));
    }
    for (i = 0; i < k; i++) *(q->real+i) = temp[i];
  }
  free(temp);
  return 0;
}

int hfwt2(matrix_t *q, matrix_t *p)
{
  int i, j, k, n, m;
  double *temp, *buffer;

  assert(q);
  if (!q) return -1;
  assert(p);
  if (!p) return -1;
	
  for (m = 1; m < p->columns; m <<= 1);
  assert(m == p->columns);
  if (m != p->columns) return -1;
  assert(m == q->columns);
  if (m != q->columns) return -1;
	
  for (n = 1; n < p->rows; n <<= 1);
  assert(n == p->rows);
  if (n != p->rows) return -1;
  assert(n == q->rows);
  if (n != q->rows) return -1;
	
  /************************************************/
  temp = (double *)malloc(max(m, n)*sizeof(double));
  assert(temp);

  matrix_copy(q, p);
  //matrix_copy(q, 0, 0, p, 0, 0, p->columns, p->rows);
  /* Transform the rows */
  for (i = 0; i < n; i++) {
    buffer = q->real+i*q->columns;
    k = m;
    while (k > 1) {
      for (j = 0; j < k/2; j++) {
	temp[j]     = (*(buffer+2*j)+*(buffer+2*j+1))/SQRT2;
	temp[k/2+j] = (*(buffer+2*j)-*(buffer+2*j+1))/SQRT2;
      }
      for (j = 0; j < k; j++) *(buffer+j) = temp[j];
      k = k/2;
    }
  }
	
  /* Transform the columns */
  temp = (float *)malloc (sizeof(float)*p->rows);
  for (j = 0; j < m; j++) {
    buffer = q->real+j; //i*q->columns;
    k = n;
    while (k > 1) {
      for (i = 0; i < k/2; i++) {
	temp[i]     = (*(buffer+(2*i)*q->columns)+*(buffer+(2*i+1)*q->columns))/SQRT2;
	temp[k/2+i] = (*(buffer+(2*i)*q->columns)-*(buffer+(2*i+1)*q->columns))/SQRT2;
      }
      for (i = 0; i < k; i++) *(buffer+i*q->columns) = temp[i];
      k = k/2;
    }
  }
  free(temp);
  return 0;
}

int ihfwt2(matrix_t *q, matrix_t *p)
{
  int i, j, k, n, m;
  double *temp, *buffer;

  assert(q);
  if (!q) return -1;
  assert(p);
  if (!p) return -1;
	
  for (m = 1; m < p->columns; m <<= 1);
  assert(m == p->columns);
  if (m != p->columns) return -1;
  assert(m == q->columns);
  if (m != q->columns) return -1;
	
  for (n = 1; n < p->rows; n <<= 1);
  assert(n == p->rows);
  if (n != p->rows) return -1;
  assert(n == q->rows);
  if (n != q->rows) return -1;
	
  /************************************************/
  temp = (double *)malloc(max(m, n)*sizeof(double));
  assert(temp);

  matrix_copy(q, p);
  //matrix_copy(q, 0, 0, p, 0, 0, p->columns, p->rows);
  /* Transform the columns */
  temp = (float *)malloc (sizeof(float)*p->rows);
  for (j = 0; j < m; j++) {
    buffer = q->real+j; //i*q->columns;
    k = 1;
    while (k < n) {
      k = 2*k;
      for (i = 0; i < k/2; i++) {
	temp[2*i]   = (*(buffer+i*q->columns)+*(buffer+(k/2+i)*q->columns));
	temp[2*i+1] = (*(buffer+i*q->columns)-*(buffer+(k/2+i)*q->columns));
      }
      for (i = 0; i < k; i++) *(buffer+i*q->columns) = temp[i];
    }
  }
	
  /* Transform the rows */
  for (i = 0; i < n; i++) {
    buffer = q->real+i*q->columns;
    k = 1;
    while (k < m) {
      k = 2*k;
      for (i = 0; i < k/2; i++) {
	temp[2*i]   = (*(buffer+i)+*(buffer+k/2+i));
	temp[2*i+1] = (*(buffer+i)-*(buffer+k/2+i));
      }
      for (i = 0; i < k; i++) *(buffer+i) = temp[i];
    }
  }

  free(temp);
  return 0;
}
