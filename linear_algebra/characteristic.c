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
#include <math.h>
#include <float.h>
#include <assert.h>

#include <common.h>
#include <linear_algebra/matrix.h>
#include <linear_algebra/vector.h>

/* output
   k(x) = q(n) * x^n + q(n-1) * x^(n-1) + ... + q(1) * x + q(0)
   Leverrier's Algorithm for Characteristic Equation
   K(x) = x^n + p(n-1) * x^(n-1) + ... + p(1) * x + p(0)
   where s(k) = trace(m^k),
   p(k) = -(s(k) + p(1) * s(k-1) + ... + p(k-1) * s(1)) / k */
void matrix_characteristic_equation(vector_t *q, matrix_t *m)
{
  int i, k, n;
  matrix_t *curr, *prev, *tmp;
  real_t *s, *p, value;

  assert(m);
  assert(matrix_is_square(m));
  assert(q);
  assert(vector_get_length(q) == matrix_get_columns(m) + 1);

  n = matrix_get_columns(m);
  s = (real_t *)malloc((n + 1) * sizeof(real_t));
  p = (real_t *)malloc((n + 1) * sizeof(real_t));

  curr = matrix_new(n, n);
  prev = matrix_new(n, n);
  matrix_identify(prev);

  for (i = 0; i < n; i++) {
    matrix_copy_matrix_multiply_matrix(curr, prev, m);
    s[i+1] = matrix_get_trace(curr);
    tmp = prev;
    prev = curr;
    curr = tmp;
  }

  vector_put_value(1.0, p, 0);

  for (k = 1; k <= n; k++) {
    p[k] = s[k];
    for (i = 1; i < k; i++) {
      p[k] += p[k - i] * s[i];
    }
    p[k] /= -k;
  }

  for (i = 0; i < n + 1; i++) {
    vector_put_value(p[i], q, n-i);
  }

  matrix_destroy(prev);
  matrix_destroy(curr);
  free(p);
  free(s);
}

