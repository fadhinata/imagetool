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
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <float.h>

#include <vector.h>
#include <common.h>

vector_t *vector_new(int n, bool complex_or_not)
{
  vector_t *v;

  assert(n > 0);

  v = (vector_t *)malloc(sizeof(*v));
  assert(v);
  memset(v, 0, sizeof(*v));

  v->reference = 0;
  v->length = n;
  v->real = (real_t *)malloc(n * sizeof(*(v->real)));
  assert(v->real);
  memset(v->real, 0, n * sizeof(*(v->real)));

  if (complex_or_not) {
    v->imaginary = (real_t *)malloc(n * sizeof(*(v->imaginary)));
    assert(v->imaginary);
    memset(v->imaginary, 0, n * sizeof(*(v->imaginary)));
  } else v->imaginary = NULL;

  return v;
}

void vector_destroy(vector_t *vec)
{
  assert(vec);

  if (vector_get_ref(vec) <= 0) {
    free(vector_get_buffer(vec));
    if (ivector_get_buffer(vec)) free(ivector_get_buffer(vec));
    free(vec);
  } else {
    vector_dec_ref(vec);
  }
}

int vector_cmp(vector_t *q, vector_t *p)
{
  int i, n;
  real_t u, v;

  assert(q);
  assert(p);

  if (!(vector_get_length(q) == vector_get_length(p))) return -1;
  
  n = vector_get_length(p);

  for (i = 0; i < n; i++) {
    u = vector_get_value(i, q);
    v = vector_get_value(i, p);
    if (!(abs(u - v) < REAL_EPSILON)) return 1;
  }
  return 0;
}

void vector_copy(vector_t *copyer, vector_t *copee)
{
  assert(copyer);
  assert(copee);
  assert(vector_get_length(copyer) == vector_get_length(copee));

  memcpy(vector_get_buffer(copyer),
	 vector_get_buffer(copee),
	 vector_get_length(copee) * sizeof(real_t));

  if (ivector_get_buffer(copee)) {
    assert(ivector_get_buffer(copyer));

    memcpy(ivector_get_buffer(copyer),
	   ivector_get_buffer(copee),
	   vector_get_length(copee) * sizeof(real_t));
  }
}

vector_t *vector_new_and_copy(vector_t *vec)
{
  vector_t *v;

  assert(vec);

  v = vector_new(vector_get_length(vec), (ivector_get_buffer(vec) != NULL));
  vector_copy(v, vec);

  return v;
}

void vector_dump(vector_t *p)
{
  int i;

  assert(p);

  printf("vector = \n");
  if (ivector_get_buffer(p) != NULL) {
    for (i = 0; i < vector_get_length(p); i++)
      printf("%.4lf+i%.4lf ", vector_get_value(i, p), ivector_get_value(i, p));
  } else {
    for (i = 0; i < vector_get_length(p); i++)
      printf("%.4lf ", vector_get_value(i, p));
  }
  printf("\n");
}

#define VECTOR_ISZERO(pbuf, plen, start, len) {		\
    int i, n;						\
    assert(pbuf);					\
    n = min(start + len, plen);				\
    for (i = start; i < n; i++) {			\
      if (!(abs(*(pbuf+i)) < REAL_EPSILON)) return 0;	\
    }							\
    return 1;						\
  }

int vector_iszero(vector_t *p, int start, int len)
{
  VECTOR_ISZERO(vector_get_buffer(p), vector_get_length(p), start, len);
}

int ivector_iszero(vector_t *p, int start, int len)
{
  VECTOR_ISZERO(ivector_get_buffer(p), vector_get_length(p), start, len);
}

int cvector_iszero(vector_t *p, int start, int len)
{
  if (vector_iszero(p, start, len)) {
    if (ivector_get_buffer(p)) return ivector_iszero(p, start, len);
    return 1;
  }
  return 0;
}

#define VECTOR_BEZERO(pbuf, plen, start, len) {		\
    int i, l;						\
    assert(pbuf);					\
    assert(start >= 0 && start < plen);			\
    assert(len > 0);					\
    l = min(start + len, plen);				\
    for (i = start; i < l; i++) *(pbuf+i) = 0.0;	\
  }

void vector_bezero(vector_t *p, int start, int len)
{
  VECTOR_BEZERO(vector_get_buffer(p), vector_get_length(p), start, len);
}

void ivector_bezero(vector_t *p, int start, int len)
{
  VECTOR_BEZERO(ivector_get_buffer(p), vector_get_length(p), start, len);
}

void cvector_bezero(vector_t *p, int start, int len)
{
  assert(p);
  assert((start >= 0) && (start < vector_get_length(p)));
  assert(len > 0);

  vector_bezero(p, start, len);
  if (ivector_get_buffer(p) != NULL) ivector_bezero(p, start, len);
}

#define VECTOR_FILL(pbuf, plen, start, len, v) {	\
    int i, l;						\
    assert(pbuf);					\
    assert(start >= 0 && start < plen);			\
    assert(len > 0);					\
    l = min(start + len, plen);				\
    for (i = start; i < l; i++) *(pbuf+i) = v;		\
  }

void vector_fill(vector_t *p, int start, int len, real_t v)
{
  VECTOR_FILL(vector_get_buffer(p), vector_get_length(p), start, len, v);
}

void ivector_fill(vector_t *p, int start, int len, real_t v)
{
  VECTOR_FILL(ivector_get_buffer(p), vector_get_length(p), start, len, v);
}

void cvector_fill(vector_t *p, int start, int len, complex_t v)
{
  assert(p);
  assert((start >= 0) && (start < vector_get_length(p)));
  assert(len > 0);

  vector_fill(p, start, len, v.real);
  if (abs(v.imag) >= REAL_EPSILON) {
    if (ivector_get_buffer(p) == NULL) {
      p->imaginary = (real_t *)malloc(vector_get_length(p) * sizeof(real_t));
      assert(p->imaginary);
      memset(ivector_get_buffer(p), 0, vector_get_length(p) * sizeof(real_t));
    }
    ivector_fill(p, start, len, v.imag);
  } else {
    if (ivector_get_buffer(p) != NULL)
      ivector_bezero(p, 0, vector_get_length(p));
  }
}

#define VECTOR_RANDOMLY_FILL(pbuf, plen) {			\
    int i;							\
    assert(pbuf);						\
    for (i = 0; i < plen; i++) *(pbuf + i) = rand()/1000.0;	\
  }

void vector_randomly_fill(vector_t *p)
{
  VECTOR_RANDOMLY_FILL(vector_get_buffer(p), vector_get_length(p));
}

void ivector_randomly_fill(vector_t *p)
{
  VECTOR_RANDOMLY_FILL(ivector_get_buffer(p), vector_get_length(p));
}

void cvector_randomly_fill(vector_t *p)
{
  assert(p);

  vector_randomly_fill(p);
  if (ivector_get_buffer(p) != NULL) ivector_randomly_fill(p);
}

#define VECTOR_COPY(qbuf, qlen, qi, pbuf, plen, pi, n) {	\
    int i;							\
    assert(qbuf);						\
    assert(qi >= 0 && qi < qlen);				\
    assert(pbuf);						\
    assert(pi >= 0 && pi < plen);				\
    n = min(qi + n, qlen) - (qi);				\
    n = min(pi + n, plen) - (pi);				\
    for (i = 0; i < n; i++) {					\
      *((qbuf) + (qi) + i) = *((pbuf) + (pi) + i);		\
    }								\
  }

void vector_copy_vector(vector_t *q, int qi, vector_t *p, int pi, int n)
{
  VECTOR_COPY(vector_get_buffer(q), vector_get_length(q), qi,
	      vector_get_buffer(p), vector_get_length(p), pi, n);
}

void vector_copy_ivector(vector_t *q, int qi, vector_t *p, int pi, int n)
{
  VECTOR_COPY(vector_get_buffer(q), vector_get_length(q), qi,
	      ivector_get_buffer(p), vector_get_length(p), pi, n);
}

void ivector_copy_vector(vector_t *q, int qi, vector_t *p, int pi, int n)
{
  VECTOR_COPY(ivector_get_buffer(q), vector_get_length(q), qi,
	      vector_get_buffer(p), vector_get_length(p), pi, n);
}

void ivector_copy_ivector(vector_t *q, int qi, vector_t *p, int pi, int n)
{
  VECTOR_COPY(ivector_get_buffer(q), vector_get_length(q), qi,
	      ivector_get_buffer(p), vector_get_length(p), pi, n);
}

void cvector_copy_cvector(vector_t *q, int qi, vector_t *p, int pi, int n)
{
  vector_copy_vector(q, qi, p, pi, n);

  if (ivector_get_buffer(p) != NULL) {
    if (ivector_get_buffer(q) == NULL) {
      q->imaginary = (real_t *)malloc(vector_get_length(q) * sizeof(real_t));
      assert(q->imaginary);
      memset(ivector_get_buffer(q), 0, vector_get_length(q) * sizeof(real_t));
    }
    ivector_copy_ivector(q, qi, p, pi, n);
  } else {
    if (ivector_get_buffer(q) != NULL) ivector_bezero(q, qi, n);
  }
}

#define VECTOR_OPCODE_SCALAR(cbuf, clen, op1, abuf, alen, op2, v) {	\
    int i;								\
    assert(cbuf);							\
    assert(abuf);							\
    assert(clen == alen);						\
    for (i = 0; i < clen; i++) {					\
      *(cbuf + i) op1 (*(abuf + i)) op2 (v);				\
    }									\
  }

vector_t *vector_copy_vector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *vector_copy_ivector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *ivector_copy_vector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *ivector_copy_ivector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *vector_add_vector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *vector_add_ivector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *ivector_add_vector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *ivector_add_ivector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *vector_subtract_vector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *vector_subtract_ivector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *ivector_subtract_vector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *ivector_subtract_ivector_add_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), +, v);
  return c;
}

vector_t *vector_add_scalar(vector_t *c, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(c), vector_get_length(c), +, v);
  return c;
}

vector_t *ivector_add_scalar(vector_t *c, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(c), vector_get_length(c), +, v);
  return c;
}

vector_t *cvector_add_scalar(vector_t *c, complex_t v)
{
  assert(c);

  vector_add_scalar(c, v.real);

  if ((ivector_get_buffer(c) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    ivector_add_scalar(c, v.imag);
  }
  return c;
}

vector_t *cvector_copy_cvector_add_scalar(vector_t *c, vector_t *a, complex_t v)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  vector_copy_vector_add_scalar(c, a, v.real);
  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      //memset(ivector_get_buffer(c), 0, vector_get_length(c)*sizeof(real_t));
      */
    }
    ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL)
      ivector_copy_ivector(c, 0, a, 0, vector_get_length(a));
    if (!(abs(v.imag) < REAL_EPSILON)) ivector_add_scalar(c, v.imag);
  } else {
    if (ivector_get_buffer(c) != NULL)
      ivector_bezero(c, 0, vector_get_length(c));
  }
  return c;
}

vector_t *cvector_add_cvector_add_scalar(vector_t *c, vector_t *a, complex_t v)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  vector_add_vector_add_scalar(c, a, v.real);

  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (vector_get_buffer(a) != NULL) ivector_add_ivector(c, a);
    if (!(abs(v.imag) < REAL_EPSILON)) ivector_add_scalar(c, v.imag);
  }
  return c;
}

vector_t *cvector_subtract_cvector_add_scalar(vector_t *c, vector_t *a, complex_t v)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  vector_subtract_vector_add_scalar(c, a, v.real);
 
  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_subtract_ivector(c, a);
    if (!(abs(v.imag) < REAL_EPSILON)) ivector_subtract_scalar(c, v.imag);
  }
  return c;
}

vector_t *vector_copy_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *vector_copy_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *ivector_copy_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *ivector_copy_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *vector_add_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *vector_add_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *ivector_add_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *ivector_add_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *vector_subtract_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *vector_subtract_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *ivector_subtract_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *ivector_subtract_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), -, v);
  return c;
}

vector_t *vector_subtract_scalar(vector_t *c, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(c), vector_get_length(c), -, v);
  return c;
}

vector_t *ivector_subtract_scalar(vector_t *c, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(c), vector_get_length(c), -, v);
  return c;
}

vector_t *cvector_subtract_scalar(vector_t *c, complex_t v)
{
  assert(c);

  vector_subtract_scalar(c, v.real);

  if ((ivector_get_buffer(c) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    ivector_subtract_scalar(c, v.imag);
  }
  return c;
}

vector_t *cvector_copy_cvector_subtract_scalar(vector_t *c, vector_t *a, complex_t v)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  vector_copy_vector_subtract_scalar(c, a, v.real);

  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      //memset(ivector_get_buffer(c), 0, vector_get_length(c)*sizeof(real_t));
      */
    }
    ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_add_ivector(c, a);
    if (!(abs(v.imag) < REAL_EPSILON)) ivector_subtract_scalar(c, v.imag);
  } else {
    if (ivector_get_buffer(c) != NULL)
      ivector_bezero(c, 0, vector_get_length(c));
  }
  return c;
}

vector_t *cvector_add_cvector_subtract_scalar(vector_t *c, vector_t *a, complex_t v)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  vector_add_vector_subtract_scalar(c, a, v.real);

  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_add_ivector(c, a);
    if (!(abs(v.imag) < REAL_EPSILON)) ivector_subtract_scalar(c, v.imag);
  }
  return c;
}

vector_t *cvector_subtract_cvector_subtract_scalar(vector_t *c, vector_t *a, complex_t v)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  vector_subtract_vector_subtract_scalar(c, a, v.real);

  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_subtract_ivector(c, a);
    if (!(abs(v.imag) < REAL_EPSILON)) ivector_add_scalar(c, v.imag);
  }
  return c;
}

vector_t *vector_copy_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *vector_copy_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *ivector_copy_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *ivector_copy_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *vector_add_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *vector_add_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *ivector_add_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *ivector_add_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *vector_subtract_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *vector_subtract_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *ivector_subtract_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *ivector_subtract_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), *, v);
  return c;
}

vector_t *vector_multiply_scalar(vector_t *c, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(c), vector_get_length(c), *, v);
  return c;
}

vector_t *ivector_multiply_scalar(vector_t *c, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(c), vector_get_length(c), *, v);
  return c;
}

vector_t *cvector_multiply_scalar(vector_t *c, complex_t v)
{
  vector_t *a;

  assert(c);

  a = vector_new_and_copy(c);
  cvector_copy_cvector_multiply_scalar(c, a, v);
  vector_destroy(a);

  return c;
}

vector_t *cvector_copy_cvector_multiply_scalar(vector_t *c, vector_t *a, complex_t v)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  //(a.real * v.real - a.imag * v.imag) + J*(a.real * v.imag + a.imag * v.real)
  vector_copy_vector_multiply_scalar(c, a, v.real);
  if ((ivector_get_buffer(a) != NULL) && !(abs(v.imag) < REAL_EPSILON))
    vector_subtract_ivector_multiply_scalar(c, a, v.imag);

  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      //memset(ivector_get_buffer(c), 0, vector_get_length(c)*sizeof(real_t));
      */
    }
    ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL)
      ivector_add_ivector_multiply_scalar(c, a, v.real);
    if (!(abs(v.imag) < REAL_EPSILON))
      ivector_add_vector_multiply_scalar(c, a, v.imag);
  } else {
    if (ivector_get_buffer(c) != NULL)
      ivector_bezero(c, 0, vector_get_length(c));
  }

  return c;
}

vector_t *cvector_add_cvector_multiply_scalar(vector_t *c, vector_t *a, complex_t v)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  //(a.real * v.real - a.imag * v.imag) + J*(a.real * v.imag + a.imag * v.real)
  vector_add_vector_multiply_scalar(c, a, v.real);
  if ((ivector_get_buffer(a) != NULL) && !(abs(v.imag) < REAL_EPSILON))
    vector_subtract_ivector_multiply_scalar(c, a, v.imag);

  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL)
      ivector_add_ivector_multiply_scalar(c, a, v.real);
    if (!(abs(v.imag) < REAL_EPSILON))
      ivector_add_vector_multiply_scalar(c, a, v.imag);
  }

  return c;
}

vector_t *cvector_subtract_cvector_multiply_scalar(vector_t *c, vector_t *a, complex_t v)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  //(a.real * v.real - a.imag * v.imag) + J*(a.real * v.imag + a.imag * v.real)
  vector_subtract_vector_multiply_scalar(c, a, v.real);
  if ((ivector_get_buffer(a) != NULL) && !(abs(v.imag) < REAL_EPSILON))
    vector_add_ivector_multiply_scalar(c, a, v.imag);

  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL)
      ivector_subtract_ivector_multiply_scalar(c, a, v.real);
    if (!(abs(v.imag) < REAL_EPSILON))
      ivector_subtract_vector_multiply_scalar(c, a, v.imag);
  }

  return c;
}

vector_t *vector_copy_vector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *vector_copy_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *ivector_copy_vector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *ivector_copy_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *vector_add_vector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *vector_add_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *ivector_add_vector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *ivector_add_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *vector_subtract_vector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *vector_subtract_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *ivector_subtract_vector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *ivector_subtract_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), /, v);
  return c;
}

vector_t *vector_divide_scalar(vector_t *c, real_t v)
{
  VECTOR_OPCODE_SCALAR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(c), vector_get_length(c), /, v);
  return c;
}

vector_t *ivector_divide_scalar(vector_t *c, real_t v)
{
  VECTOR_OPCODE_SCALAR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(c), vector_get_length(c), /, v);
  return c;
}

vector_t *cvector_divide_scalar(vector_t *c, complex_t v)
{
  /*
    (c.real + i * c.imag) / (v.real + i * v.imag)
    = (c.real + i * c.imag) * (v.real - i * v.imag) / ((v.real + i * v.imag) * (v.real - i * v.imag))
    = (c.real * v.real + c.imag * v.imag + i * (c.imag * v.real - c.real * v.imag)) / (v.real * v.real + v.imag * v.imag)
  */
  vector_t *a;
  real_t denom;

  assert(c);

  a = vector_new_and_copy(c);
  denom = sqr(v.real) + sqr(v.imag);

  // For real part
  vector_copy_vector_multiply_scalar(c, a, v.real);
  if ((ivector_get_buffer(a) != NULL) && !(abs(v.imag) < REAL_EPSILON))
    vector_add_ivector_multiply_scalar(c, a, v.imag);
  vector_divide_scalar(c, denom);

  // For imaginary part
  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    if (ivector_get_buffer(a) != NULL)
      ivector_copy_ivector_multiply_scalar(c, a, v.real);
    if (!(abs(v.imag) < REAL_EPSILON))
      ivector_subtract_vector_multiply_scalar(c, a, v.imag);
    ivector_divide_scalar(c, denom);
  } else {
    if (ivector_get_buffer(c) != NULL)
      ivector_bezero(c, 0, vector_get_length(c));
  }
  vector_destroy(a);
  return c;
}

vector_t *cvector_copy_cvector_divide_scalar(vector_t *c, vector_t *a, complex_t v)
{
  real_t denom;

  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  /*
    (c.real + i * c.imag) / (v.real + i * v.imag)
    = (c.real + i * c.imag) * (v.real - i * v.imag) / ((v.real + i * v.imag) * (v.real - i * v.imag))
    = (c.real * v.real + c.imag * v.imag + i * (c.imag * v.real - c.real * v.imag)) / (v.real * v.real + v.imag * v.imag)
  */

  denom = sqr(v.real) + sqr(v.imag);
  // For real part
  vector_copy_vector_multiply_scalar(c, a, v.real);
  if ((ivector_get_buffer(a) != NULL) && !(abs(v.imag) < REAL_EPSILON))
    vector_add_ivector_multiply_scalar(c, a, v.imag);
  vector_divide_scalar(c, denom);

  // For imaginary part
  if ((ivector_get_buffer(a) != NULL) || !(abs(v.imag) < REAL_EPSILON)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      //memset(ivector_get_buffer(c), 0, vector_get_length(c)*sizeof(real_t));
      */
    }
    ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL)
      ivector_add_ivector_multiply_scalar(c, a, v.real);
    if (!(abs(v.imag) < REAL_EPSILON))
      ivector_subtract_vector_multiply_scalar(c, a, v.imag);
    ivector_divide_scalar(c, denom);
  } else {
    if (ivector_get_buffer(c) != NULL)
      ivector_bezero(c, 0, vector_get_length(c));
  }
  return c;
}

vector_t *cvector_add_cvector_divide_scalar(vector_t *c, vector_t *a, complex_t v)
{
  vector_t *tmp;

  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  tmp = vector_new_and_copy(a);
  cvector_copy_cvector_divide_scalar(tmp, a, v);
  cvector_add_cvector(c, tmp);
  vector_destroy(tmp);
  return c;
}

vector_t *cvector_subtract_cvector_divide_scalar(vector_t *c, vector_t *a, complex_t v)
{
  vector_t *tmp;

  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  tmp = vector_new_and_copy(a);
  cvector_copy_cvector_divide_scalar(tmp, a, v);
  cvector_subtract_cvector(c, tmp);
  vector_destroy(tmp);
  return c;
}

#define VECTOR_OPCODE_VECTOR(cbuf, clen, op1, abuf, alen, op2, bbuf, blen) { \
    int i;								\
    assert(cbuf);							\
    assert(abuf);							\
    assert(bbuf);							\
    assert(clen == alen);						\
    assert(alen == blen);						\
    for (i = 0; i < clen; i++)						\
      *(cbuf + i) op1 (*(abuf + i)) op2 (*(bbuf + i));			\
  }

vector_t *vector_copy_vector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_copy_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_copy_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_copy_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_vector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_vector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_vector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_vector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_vector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), +,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_vector(vector_t *c, vector_t *a)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(c), vector_get_length(c), +,
		       vector_get_buffer(a), vector_get_length(a));
  return c;
}

vector_t *ivector_add_ivector(vector_t *c, vector_t *a)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(c), vector_get_length(c), +,
		       ivector_get_buffer(a), vector_get_length(a));
  return c;
}

vector_t *cvector_add_cvector(vector_t *c, vector_t *a)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  // for real part
  vector_add_vector(c, a);

  // for imaginary part
  if (ivector_get_buffer(a) != NULL) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    ivector_add_ivector(c, a);
  }
  return c;
}

vector_t *cvector_copy_cvector_add_cvector(vector_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_length(c) == vector_get_length(a));
  assert(vector_get_length(a) == vector_get_length(b));

  // for real part
  vector_copy_vector_add_vector(c, a, b);

  // for imaginary part
  if ((ivector_get_buffer(a) != NULL) || (ivector_get_buffer(b) != NULL)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      //memset(ivector_get_buffer(c), 0, vector_get_length(c)*sizeof(real_t));
      */
    }
    ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_add_ivector(c, a);
    if (ivector_get_buffer(b) != NULL) ivector_add_ivector(c, b);
  } else {
    if (ivector_get_buffer(c) != NULL)
      ivector_bezero(c, 0, vector_get_length(c));
  }
  return c;
}

vector_t *cvector_add_cvector_add_cvector(vector_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_length(c) == vector_get_length(a));
  assert(vector_get_length(a) == vector_get_length(b));

  // for real part
  vector_add_vector_add_vector(c, a, b);

  // for imaginary part
  if ((ivector_get_buffer(a) != NULL) || (ivector_get_buffer(b) != NULL)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_add_ivector(c, a);
    if (ivector_get_buffer(b) != NULL) ivector_add_ivector(c, b);
  }
  return c;
}

vector_t *cvector_subtract_cvector_add_cvector(vector_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_length(c) == vector_get_length(a));
  assert(vector_get_length(a) == vector_get_length(b));

  // for real part
  vector_subtract_vector_add_vector(c, a, b);

  // for imaginary part
  if ((ivector_get_buffer(a) != NULL) || (ivector_get_buffer(b) != NULL)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_subtract_ivector(c, a);
    if (ivector_get_buffer(b) != NULL) ivector_subtract_ivector(c, b);
  }
  return c;
}

// subtraction
vector_t *vector_copy_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_copy_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_copy_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_copy_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b)); 
  return c;
}

vector_t *ivector_add_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), -,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_vector(vector_t *c, vector_t *a)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(c), vector_get_length(c), -,
		       vector_get_buffer(a), vector_get_length(a));
  return c;
}

vector_t *ivector_subtract_ivector(vector_t *c, vector_t *a)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(c), vector_get_length(c), -,
		       ivector_get_buffer(a), vector_get_length(a));
  return c;
}

vector_t *cvector_subtract_cvector(vector_t *c, vector_t *a)
{
  assert(c);
  assert(a);
  assert(vector_get_length(c) == vector_get_length(a));

  // for real part
  vector_subtract_vector(c, a);

  // for imaginary part
  if (ivector_get_buffer(a) != NULL) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    ivector_subtract_ivector(c, a);
  }
  return c;
}

vector_t *cvector_copy_cvector_subtract_cvector(vector_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_length(c) == vector_get_length(a));
  assert(vector_get_length(a) == vector_get_length(b));

  // for real part
  vector_copy_vector_subtract_vector(c, a, b);

  // for imaginary part
  if ((ivector_get_buffer(a) != NULL) || (ivector_get_buffer(b) != NULL)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      //memset(ivector_get_buffer(c), 0, vector_get_length(c)*sizeof(real_t));
      */
    }
    ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_add_ivector(c, a);
    if (ivector_get_buffer(b) != NULL) ivector_subtract_ivector(c, b);
  } else {
    if (ivector_get_buffer(c) != NULL)
      ivector_bezero(c, 0, vector_get_length(c));
  }	
  return c;
}

vector_t *cvector_add_cvector_subtract_cvector(vector_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_length(c) == vector_get_length(a));
  assert(vector_get_length(a) == vector_get_length(b));

  // for real part
  vector_add_vector_subtract_vector(c, a, b);

  // for imaginary part
  if ((ivector_get_buffer(a) != NULL) || (ivector_get_buffer(b) != NULL)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_add_ivector(c, a);
    if (ivector_get_buffer(b) != NULL) ivector_subtract_ivector(c, b);
  }
  return c;
}

vector_t *cvector_subtract_cvector_subtract_cvector(vector_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_length(c) == vector_get_length(a));
  assert(vector_get_length(a) == vector_get_length(b));

  // for real part
  vector_subtract_vector_subtract_vector(c, a, b);

  // for imaginary part
  if ((ivector_get_buffer(a) != NULL) || (ivector_get_buffer(b) != NULL)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary != NULL);
      memset(c->imaginary, 0, vector_get_length(c)*sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_subtract_ivector(c, a);
    if (ivector_get_buffer(b) != NULL) ivector_add_ivector(c, b);
  }
  return c;
}

// multiplication
vector_t *vector_copy_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_copy_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_copy_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_copy_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_copy_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_add_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       vector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_add_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), +=,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_subtract_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       vector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_subtract_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), -=,
		       ivector_get_buffer(a), vector_get_length(a), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *cvector_copy_cvector_multiply_cvector(vector_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_length(c) == vector_get_length(a));
  assert(vector_get_length(a) == vector_get_length(b));

  // real part
  vector_copy_vector_multiply_vector(c, a, b);
  if ((ivector_get_buffer(a) != NULL) && (ivector_get_buffer(b) != NULL))
    vector_subtract_ivector_multiply_ivector(c, a, b);

  // imaginary part
  if ((ivector_get_buffer(a) != NULL) || (ivector_get_buffer(b) != NULL)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary);
      //memset(ivector_get_buffer(c), 0, vector_get_length(c)*sizeof(real_t));
      */
    }
    ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_add_ivector_multiply_vector(c, a, b);
    if (ivector_get_buffer(b) != NULL) ivector_add_vector_multiply_ivector(c, a, b);
  } else {
    if (ivector_get_buffer(c) != NULL)
      ivector_bezero(c, 0, vector_get_length(c));
  }
  return c;
}

vector_t *cvector_add_cvector_multiply_cvector(vector_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_length(c) == vector_get_length(a));
  assert(vector_get_length(a) == vector_get_length(b));

  // real part
  vector_add_vector_multiply_vector(c, a, b);
  if ((ivector_get_buffer(a) != NULL) && (ivector_get_buffer(b) != NULL))
    vector_subtract_ivector_multiply_ivector(c, a, b);

  // imaginary part
  if ((ivector_get_buffer(a) != NULL) || (ivector_get_buffer(b) != NULL)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary != NULL);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_add_ivector_multiply_vector(c, a, b);
    if (ivector_get_buffer(b) != NULL) ivector_add_vector_multiply_ivector(c, a, b);
  }
  return c;
}

vector_t *cvector_subtract_cvector_multiply_cvector(vector_t *c, vector_t *a, vector_t *b)
{
  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_length(c) == vector_get_length(a));
  assert(vector_get_length(a) == vector_get_length(b));

  // real part
  vector_subtract_vector_multiply_vector(c, a, b);
  if ((ivector_get_buffer(a) != NULL) && (ivector_get_buffer(b) != NULL))
    vector_add_ivector_multiply_ivector(c, a, b);

  // imaginary part
  if ((ivector_get_buffer(a) != NULL) || (ivector_get_buffer(b) != NULL)) {
    if (ivector_get_buffer(c) == NULL) {
      vector_attach_imaginary(c);
      /*
      c->imaginary = (real_t *)malloc(vector_get_length(c) * sizeof(real_t));
      assert(c->imaginary != NULL);
      memset(c->imaginary, 0, vector_get_length(c) * sizeof(real_t));
      */
    }
    //ivector_bezero(c, 0, vector_get_length(c));
    if (ivector_get_buffer(a) != NULL) ivector_subtract_ivector_multiply_vector(c, a, b);
    if (ivector_get_buffer(b) != NULL) ivector_subtract_vector_multiply_ivector(c, a, b);
  }
  return c;
}

vector_t *vector_multiply_vector(vector_t *c, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(c), vector_get_length(c), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *vector_multiply_ivector(vector_t *c, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(vector_get_buffer(c), vector_get_length(c), =,
		       vector_get_buffer(c), vector_get_length(c), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_multiply_vector(vector_t *c, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(c), vector_get_length(c), *,
		       vector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *ivector_multiply_ivector(vector_t *c, vector_t *b)
{
  VECTOR_OPCODE_VECTOR(ivector_get_buffer(c), vector_get_length(c), =,
		       ivector_get_buffer(c), vector_get_length(c), *,
		       ivector_get_buffer(b), vector_get_length(b));
  return c;
}

vector_t *cvector_multiply_cvector(vector_t *c, vector_t *b)
{
  vector_t *tmp;

  assert(c);
  assert(b);
  assert(vector_get_length(c) == vector_get_length(b));

  tmp = vector_new_and_copy(c);
  cvector_copy_cvector_multiply_cvector(c, tmp, b);
  vector_destroy(tmp);
  return c;
}

#define VECTOR_DOTPRODUCT_VECTOR(c, abuf, alen, bbuf, blen) {	\
    int i;							\
    assert(c);							\
    assert(abuf);						\
    assert(bbuf);						\
    assert(alen == blen);					\
    *c = 0.0;							\
    for (i = 0; i < alen; i++)					\
      *c += (*(abuf+i))*(*(bbuf+i));				\
  }

real_t vector_dotproduct_vector(real_t *c, vector_t *a, vector_t *b)
{
  VECTOR_DOTPRODUCT_VECTOR(c, vector_get_buffer(a), vector_get_length(a),
			   vector_get_buffer(b), vector_get_length(b));
  return *c;
}

real_t vector_dotproduct_ivector(real_t *c, vector_t *a, vector_t *b)
{
  VECTOR_DOTPRODUCT_VECTOR(c, vector_get_buffer(a), vector_get_length(a),
			   ivector_get_buffer(b), vector_get_length(b));
  return *c;
}

real_t ivector_dotproduct_vector(real_t *c, vector_t *a, vector_t *b)
{
  VECTOR_DOTPRODUCT_VECTOR(c, ivector_get_buffer(a), vector_get_length(a),
			   vector_get_buffer(b), vector_get_length(b));
  return *c;
}

real_t ivector_dotproduct_ivector(real_t *c, vector_t *a, vector_t *b)
{
  VECTOR_DOTPRODUCT_VECTOR(c, ivector_get_buffer(a), vector_get_length(a),
			   ivector_get_buffer(b), vector_get_length(b));
  return *c;
}

complex_t cvector_dotproduct_cvector(complex_t *c, vector_t *a, vector_t *b)
{
  real_t tmp;

  assert(c);
  assert(a);
  assert(b);
  assert(vector_get_length(a) == vector_get_length(b));

  // for real part
  vector_dotproduct_vector(&c->real, a, b);
  if (ivector_get_buffer(a) && ivector_get_buffer(b)) {
    ivector_dotproduct_ivector(&tmp, a, b);
    c->real -= tmp;
  }

  // imaginary part
  c->imag = 0.0;
  if ((ivector_get_buffer(a) != NULL) || (ivector_get_buffer(b) != NULL)) {
    if (ivector_get_buffer(a) != NULL) {
      ivector_dotproduct_vector(&tmp, a, b);
      c->imag += tmp;
    }
    if (ivector_get_buffer(b) != NULL) {
      vector_dotproduct_ivector(&tmp, a, b);
      c->imag += tmp;
    }
  }
  return *c;
}

#define VECTOR_SUM(v, pbuf, plen) {		\
    int i;					\
    assert(pbuf);				\
    (v) = 0.0;					\
    for (i = 0; i < plen; i++) {		\
      (v) += *(pbuf + i);			\
    }						\
  }

real_t vector_get_sum(vector_t *p)
{
  real_t v;
  VECTOR_SUM(v, vector_get_buffer(p), vector_get_length(p));
  return v;
}

real_t ivector_get_sum(vector_t *p)
{
  real_t v;
  VECTOR_SUM(v, ivector_get_buffer(p), vector_get_length(p));
  return v;
}

complex_t cvector_get_sum(vector_t *p)
{
  complex_t v;

  assert(p);

  v.real = vector_get_sum(p);
  if (ivector_get_buffer(p)) v.imag = ivector_get_sum(p);
  else v.imag = 0;

  return v;
}

real_t vector_get_norm(vector_t *p)
{
  real_t v;
  assert(p);
  vector_dotproduct_vector(&v, p, p);
  return v;
}

real_t ivector_get_norm(vector_t *p)
{
  real_t v;
  assert(p);
  ivector_dotproduct_ivector(&v, p, p);
  return v;
}

complex_t cvector_get_norm(vector_t *p)
{
  complex_t v;
  assert(p);
  cvector_dotproduct_cvector(&v, p, p);
  return v;
}
