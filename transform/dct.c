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
#include <math.h>
#include <matrix.h>

#define COEFF 0.7071067781

int dct2(matrix_t *q, matrix_t *p)
{
  int u, v;
  int c, r;
  double accum;
  double columns2, rows2;
  double scale;
	
  columns2 = 2.0*p->columns;
  rows2 = 2.0*p->rows;
  scale = (2.0/sqrt((double)p->columns*(double)p->rows));
	
  for (u = 0; u < q->columns; u++) {
    for (v = 0; v < q->rows; v++) {
      accum = 0.0;
      for (c = 0; c < p->columns; c++) {
	for (r = 0; r < p->rows; r++) {
	  accum += (cos((M_PI*u*(2*c+1))/columns2) *
		    cos((M_PI*v*(2*v+1))/rows2) *
		    *(p->real+r*p->columns+c));
	}
      }
      accum *= scale;
      if (u == 0) accum *= COEFF;
      if (v == 0) accum *= COEFF;
      *(q->real+v*q->rows+u) = accum;
    }
  }
  return 0;
}

int idct2(matrix_t *q, matrix_t *p)
{
  int u, v;
  int c, r;
  double accum;
  double columns2, rows2;
  double scale;
  double cu, cv;

  columns2 = 2.0*p->columns;
  rows2 = 2.0*p->rows;
  scale = (2.0/sqrt((double)p->columns*(double)p->rows));

  for (c = 0; c < q->columns; c++) {
    for (r = 0; r < q->rows; r++) {
      accum = 0.0;
      for (u = 0; u < p->columns; u++) {
	for (v = 0; v < p->rows; v++) {
	  if (u == 0) cu = COEFF;
	  else cu = 1.0;
	  if (v == 0) cv = COEFF;
	  else cv = 1.0;
	  accum += cu*cv*cos((M_PI*u*(2*c+1))/columns2) *
	    cos((M_PI*v*(2*r+1))/rows2) *
	    *(p->real+r*p->columns+c);
	}
      }
      *(q->real+v*q->columns+u) = accum*scale;
    }
  }
  return 0;
}
