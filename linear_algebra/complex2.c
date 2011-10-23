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
#include <common.h>
#include <float.h>
#include <math.h>
#include <assert.h>

#include <linear_algebra/complex2.h>

#define COMPLEX_OPCODE_COMPLEX(c, op1, a, op2, b) {	\
    assert(c);						\
    assert(a);						\
    assert(b);						\
    (c)->real op1 (a)->real op2 (b)->real;		\
    (c)->imag op1 (a)->imag op2 (b)->imag;		\
  }

__inline complex_t *complex_copy_complex_add_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_OPCODE_COMPLEX(c, =, a, +, b);
  return c;
}

__inline complex_t *complex_add_complex_add_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_OPCODE_COMPLEX(c, +=, a, +, b);
  return c;
}

__inline complex_t *complex_subtract_complex_add_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_OPCODE_COMPLEX(c, -=, a, +, b);
  return c;
}

__inline complex_t *complex_add_complex(complex_t *c, complex_t *b)
{
  COMPLEX_OPCODE_COMPLEX(c, =, c, +, b);
  return c;
}

__inline complex_t *complex_copy_complex_subtract_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_OPCODE_COMPLEX(c, =, a, -, b);
  return c;
}

__inline complex_t *complex_add_complex_subtract_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_OPCODE_COMPLEX(c, +=, a, -, b);
  return c;
}

__inline complex_t *complex_subtract_complex_subtract_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_OPCODE_COMPLEX(c, -=, a, -, b);
  return c;
}

__inline complex_t *complex_subtract_complex(complex_t *c, complex_t *b)
{
  COMPLEX_OPCODE_COMPLEX(c, =, c, -, b);
  return c;
}

#define COMPLEX_MUL_COMPLEX(c, op, a, b) {				\
    assert(c);								\
    assert(a);								\
    assert(b);								\
    (c)->real op ((a)->real)*((b)->real)-((a)->imag)*((b)->imag);	\
    (c)->imag op ((a)->imag)*((b)->real)+((a)->real)*((b)->imag);	\
  }

__inline complex_t *complex_copy_complex_multiply_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_MUL_COMPLEX(c, =, a, b);
  return c;
}

__inline complex_t *complex_add_complex_multiply_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_MUL_COMPLEX(c, +=, a, b);
  return c;
}

__inline complex_t *complex_subtract_complex_multiply_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_MUL_COMPLEX(c, -=, a, b);
  return c;
}

__inline complex_t *complex_multiply_complex(complex_t *c, complex_t *b)
{
  complex_t a;
  assert(c);
  assert(b);
  a = *c;
  complex_copy_complex_multiply_complex(c, &a, b);
  return c;
}

#define COMPLEX_DIV_COMPLEX(c, op, a, b) {				\
    real_t r;								\
    assert(c);								\
    assert(a);								\
    assert(b);								\
    r = ((b)->real)*((b)->real)+((b)->imag)*((b)->imag);		\
    assert(r != 0.0);							\
    (c)->real op (((a)->real)*((b)->real)+((a)->imag)*((b)->imag))/r;	\
    (c)->imag op (((a)->imag)*((b)->real)-((a)->real)*((b)->imag))/r;	\
  }

__inline complex_t *complex_copy_complex_divide_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_DIV_COMPLEX(c, =, a, b);
  return c;
}

__inline complex_t *complex_add_complex_divide_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_DIV_COMPLEX(c, +=, a, b);
  return c;
}

__inline complex_t *complex_subtract_complex_divide_complex(complex_t *c, complex_t *a, complex_t *b)
{
  COMPLEX_DIV_COMPLEX(c, -=, a, b);
  return c;
}

__inline complex_t *complex_divide_complex(complex_t *c, complex_t *b)
{
  complex_t a;
  assert(c);
  assert(b);
  a = *c;
  complex_copy_complex_divide_complex(c, &a, b);
  return c;
}

#define COMPLEX_SQRT(c, op, a) {				\
    real_t r;							\
    assert(c);							\
    assert(a);							\
    r = sqrt(((a)->real)*((a)->real)+((a)->imag)*((a)->imag));	\
    assert(r != 0.0);						\
    (c)->real op sqrt((r+(a)->real)/2.0);			\
    (c)->imag op sgn((a)->imag) * sqrt((r-(a)->real)/2.0);	\
  }

__inline complex_t *complex_copy_complex_sqrt(complex_t *q, complex_t *p)
{
  COMPLEX_SQRT(q, =, p);
  return q;
}

__inline complex_t *complex_add_complex_sqrt(complex_t *q, complex_t *p)
{
  COMPLEX_SQRT(q, +=, p);
  return q;
}

__inline complex_t *complex_subtract_complex_sqrt(complex_t *q, complex_t *p)
{
  COMPLEX_SQRT(q, -=, p);
  return q;
}

__inline complex_t complex_sqrt(complex_t *q)
{
  complex_t p;
  complex_copy_complex_sqrt(&p, q);
  return p;
}

__inline real_t complex_get_abs(complex_t *q)
{
  assert(q);
  return sqrt(sqr(q->real) + sqr(q->imag));
}

__inline real_t complex_abs(complex_t *q)
{
  assert(q);
  return sqrt(sqr(q->real) + sqr(q->imag));
}

__inline int complex_compare_abs(complex_t *q, complex_t *p)
{
  real_t v1, v2;

  assert(q);
  assert(p);

  v1 = complex_get_abs(q);
  v2 = complex_get_abs(p);

  if (abs(v1-v2) < REAL_EPSILON) return 0;
  if (v1 > v2) return +1;
  return -1;
}

__inline int complex_compare(complex_t *q, complex_t *p)
{
  assert(q);
  assert(p);

  if (!(abs(q->real - p->real) < REAL_EPSILON)) return -1;
  if (!(abs(q->imag - p->imag) < REAL_EPSILON)) return -1;
  return 0;
}
