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
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <assert.h>

#include <common.h>
#include <spline/bezier.h>
#include <geometry/point_list.h>

/* Bezier curve equation
 * for 0 <= u <= 1 and n + 1 ctrol-points,
 * b(u) = sum(k=0..n, kth-ctrl * (n! / (k! * (n - k)!)) * u^k * (1 - u)^(n - k)
 * blending function : (n! / (k! * (n - k)!)) * u^k * (1 - u)^(n - k) */
void point_list_create_bezier_spline(point_list_t *bezier, int nbezier, point_list_t *controls)
{
  int i, k, c, n;
  int nctrl;
  real_t mu, blending;
  point_t *p, *q;

  assert(bezier);
  assert(nbezier > 3);
  assert(controls);
  assert(point_list_get_count(controls) > 3);

  nctrl = point_list_get_count(controls);

  p = point_new();
  point_copy(p, point_list_glimpse(0, controls));
  point_list_insert(p, bezier);

  for (i = 1; i < (nbezier - 1); i++) {
    p = point_new();
    mu = (real_t)i / (real_t)nbezier;
    n = nctrl - 1;
    for (c = 0; c <= n; c++) {
      /* blending function need to be prevented OVERFLOW
	 In nCr * t^r * (1-t)^(n-r)
	 The blend calculation after component processes must be avoided,
	 because a variable overflow occures especially factorial calculation
	 so, we rearrange blending function to prevent overflow like this;
	 (n-0)*(n-1)*...*(n-(r-1)) / (r-0)*(r-1)*...*(r-(i-1)) * t * t * ... * t * (1-t) * (1-t) * ... * (1-t) 
	 = [(n-0) * t * (1-t) / (r-0)] * [(n-1) * t * (1-t) / (r-1)] * ....  */
      blending = 1.0;
      for (k = 0; k < max(c, n - c); k++) {
	if (k < c) blending = blending * (n - k) * mu / (c - k);
	if (k < (n - c)) blending = blending * (1 - mu);
      }
      q = point_list_glimpse(c, controls);
      p->x = q->x * blending;
      p->y = q->y * blending;
      p->z = q->z * blending;
    }
    point_list_insert(p, bezier);
  }
  p = point_new();
  point_copy(p, point_list_glimpse(nctrl-1, controls));
  point_list_insert(p, bezier);
  //point_destroy(point);
}

void matrix_bezier_spline_curve(matrix_t *plots, matrix_t *controls)
{
  int i, j, k, c, n;
  int ndim, nctrl, nsample;
  real_t mu, *pbuf, blending, *temp;

  assert(plots);
  assert(matrix_get_rows(plots) > 3);
  assert(controls);
  assert(matrix_get_rows(controls) > 3);
  assert(matrix_get_columns(plots) == matrix_get_columns(controls));
	
  nsample = matrix_get_rows(plots);
  nctrl = matrix_get_rows(controls);
  ndim = matrix_get_columns(controls);
	
  temp = (real_t *)malloc(ndim * sizeof(real_t));
  assert(temp);

  // Just copy the first point
  //  matrix_copy_matrix(plots, 0, 0, controls, 0, 0, matrix_get_columns(controls), 1);
  pbuf = matrix_get_buffer(plots);
  for (j = 0; j < ndim; j++)
    *(pbuf + j) = *(controls->real+j);
  pbuf += matrix_get_columns(plots);

  for (i = 1; i < nsample - 1; i++) {
    memset(temp, 0, ndim * sizeof(real_t));
    mu = (real_t)i / (real_t)nsample;
    n = nctrl-1;
    for (c = 0; c <= n; c++) {
      /* blending function need to be prevented OVERFLOW
       * In nCr*t^r*(1-t)^(n-r)
       * The blend calculation after component processes must be avoided,
       * because a variable overflow occures especially factorial calculation
       * so, we rearrange blending function to prevent overflow like this;
       * (n-0)*(n-1)*...*(n-(r-1)) / (r-0)*(r-1)*...*(r-(i-1)) * t*t*...*t * (1-t)*(1-t)*...*(1-t) 
       * = [(n-0)*t*(1-t)/(r - 0)] * [(n-1)*t*(1-t)/(r-1)] * ....  */
      blending = 1.0;
      for (k = 0; k < max(c, n - c); k++) {
	if (k < c) blending = blending * (n - k) * mu / (c - k);
	if (k < (n - c)) blending = blending * (1 - mu);
      }
      for (j = 0; j < ndim; j++)
	*(temp+j) += *(controls->real+c*controls->columns+j)*blending;
    }
    for (j = 0; j < ndim; j++)
      *(pbuf+j) = *(temp+j);
    pbuf += plots->columns;
  }

  for (j = 0; j < ndim; j++) {
    *(pbuf+j) = *(controls->real+(controls->rows-1)*controls->columns+j);
  }
}
