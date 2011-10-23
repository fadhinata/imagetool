/*
  Copyright (C) 2006 Pedro Felzenszwalb

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <assert.h>

#include <common.h>
#include <linear_algebra/vector.h>
#include <linear_algebra/matrix.h>
#include <pixmap/bitmap.h>
#include <pixmap/dwordmap.h>
#include <convert/convert.h>

#define INF 1E20

// distance transform for 1d using squared distance metric
// envelope algorithm
// Df(p) = min((p-q)^2 + f(q)) as metric
// the minimum value among parabola rooted at (q, f(q)
void vector_distance_transform(vector_t *dist, vector_t *vec, int n)
{
  int p, q, k, *xvertex;
  real_t *xmaxhit, x;

  assert(dist);
  assert(vec);
  assert(vector_get_dimension(dist) >= vector_get_dimension(vec));

  if (n <= 0) n = vector_get_dimension(vec);

  xvertex = (int *)calloc(n, sizeof(*xvertex));
  assert(xvertex);

  xmaxhit = (real_t *)calloc(n + 1, sizeof(*xmaxhit));
  assert(xmaxhit);

  // The information of lower envelope of n-parabola
  k = 0;
  xvertex[0] = 0;
  xmaxhit[0] = -INF;
  xmaxhit[1] = +INF;

  for (q = 1; q < n; q++) {
    //    y = (x-q)^2 + vec(q)
    //    y = (x-p)^2 + vec(p)
    // -)____________________
    //
    //    x = ((q^2 + f(q)) - (p^2 + f(p)) / (2 * q - 2 * p)
    //
    // Find the maximum of x of common point in two parabolas
    p = xvertex[k];
    x = (((sqr(q) + vector_get_value(vec, q)) -
	  (sqr(p) + vector_get_value(vec, p))) /
	 (2 * q - 2 * p));
    while (x <= xmaxhit[k]) {
      k--;
      p = xvertex[k];
      x = (((sqr(q) + vector_get_value(vec, q)) -
	    (sqr(p) + vector_get_value(vec, p))) /
	   (2 * q - 2 * p));
    }
    k++;
    xvertex[k] = q;
    xmaxhit[k] = x;
    xmaxhit[k+1] = +INF;
  }

  k = 0;
  for (q = 0; q < n; q++) {
    while (xmaxhit[k+1] < q) k++;
    p = xvertex[k];
    vector_put_value(sqr(q-p) + vector_get_value(vec, p), dist, q);
  }

  free(xvertex);
  free(xmaxhit);
}

static void distance_transform(matrix_t *dist)
{
  int c, r, columns, rows;
  vector_t *vec, *temp;

  columns = matrix_get_columns(dist);
  rows = matrix_get_rows(dist);

  vec = vector_new(max(columns, rows), false);
  temp = vector_new(max(columns, rows), false);

  // transform along columns
  for (c = 0; c < columns; c++) {
    vector_copy_column_vector_of_matrix(vec, dist, c, 0);
    vector_distance_transform(temp, vec, rows);
    matrix_copy_column_vector(dist, c, 0, temp);
  }

  // transform along rows
  for (r = 0; r < rows; r++) {
    vector_copy_row_vector_of_matrix(vec, dist, 0, r);
    vector_distance_transform(temp, vec, columns);
    matrix_copy_row_vector(dist, 0, r, temp);
  }

  vector_destroy(temp);
  vector_destroy(vec);
}

void matrix_distance_transform(matrix_t *dist, matrix_t *mat)
{
  assert(dist);
  assert(mat);
  assert(matrix_are_matched(dist, mat));

  matrix_copy(dist, mat);
  distance_transform(dist);
}

void bitmap_distance_transform(matrix_t *dist, bitmap_t *bin)
{
  assert(dist);
  assert(bin);
  assert(matrix_get_columns(dist) == bitmap_get_width(bin));
  assert(matrix_get_rows(dist) == bitmap_get_height(bin));

  bitmap2matrix(dist, bin);
  matrix_multiply_scalar(dist, INF);
  distance_transform(dist);
}
