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
#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <common.h>
#include <complex2.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    int reference;
    int length;
    real_t *real;
    real_t *imaginary;
  } vector_t;

#define vector_get_ref(v) ((v)->reference)
#define vector_inc_ref(v) ((v)->reference++)
#define vector_dec_ref(v) ((v)->reference--)

#define vector_is_imaginary(v) ((v)->imaginary != NULL)
#define vector_get_length(vec) ((vec)->length)
#define vector_get_value(i, vec) (*((vec)->real + i))
#define ivector_get_value(i, vec) (*((vec)->imaginary + i))
#define cvector_read_value(c, i, v) ((c)->real = *((v)->real + i), (c)->imag = *((v)->imaginary + i))
#define vector_put_value(v, i, vec) (*((vec)->real + i) = v)
#define ivector_put_value(v, i, vec) (*((vec)->imaginary + i) = v)
#define cvector_put_value(c, i, v) (*((v)->real + i) = c.real, *((v)->imaginary + i) = c.imag)
#define vector_get_buffer(v) ((v)->real)
#define ivector_get_buffer(v) ((v)->imaginary)

#define VECTOR_LENGTH(v) ((v)->length)
#define VECTOR_GET(v, vec, idx) (v = *((vec)->real+idx))
#define IVECTOR_GET(v, vec, idx) (v = *((vec)->imaginary+idx))
#define CVECTOR_GET(v, vec, idx) (v.real = *((vec)->real+idx), v.imag = (vec)->imaginary != NULL ? *((vec)->imaginary+idx) : 0.0)
#define VECTOR_PUT(v, vec, idx) (*((vec)->real+idx) = v)
#define IVECTOR_PUT(v, vec, idx) (*((vec)->imaginary+idx) = v)
#define CVECTOR_PUT(v, vec, idx) { *((vec)->real+idx) = v.real; if ((vec)->imaginary != NULL) ? *((vec)->imaginary+idx) = v.imag; }

  vector_t *vector_new(int n, bool complex_or_not);
#define vector_attach_imaginary(v) {					\
    if ((v)->imaginary == NULL) {					\
      (v)->imaginary = (real_t *)malloc((v)->length * sizeof(real_t)); \
      assert((v)->imaginary);						\
      memset((v)->imaginary, 0, (v)->length * sizeof(real_t)); \
    }									\
  }
  void vector_destroy(vector_t *vec);
  void vector_copy(vector_t *copyer, vector_t *copee);
  vector_t *vector_new_and_copy(vector_t *vec);
  
  //vector_t *vector_create(int n, int complex);
  //vector_t *vector_clone(vector_t *p);
  //void vector_delete(vector_t *p);
  void vector_dump(vector_t *p);
  int vector_cmp(vector_t *q, vector_t *p);

  int vector_iszero(vector_t *p, int start, int len);
  int ivector_iszero(vector_t *p, int start, int len);
  int cvector_iszero(vector_t *p, int start, int len);

  void vector_bezero(vector_t *p, int start, int len);
  void ivector_bezero(vector_t *p, int start, int len);
  void cvector_bezero(vector_t *p, int start, int len);
  void vector_fill(vector_t *p, int index, int len, real_t v);
  void ivector_fill(vector_t *p, int index, int len, real_t v);
  void cvector_fill(vector_t *p, int index, int len, complex_t v);
  void vector_randomly_fill(vector_t *p);
  void ivector_randomly_fill(vector_t *p);
  void cvector_randomly_fill(vector_t *p);
  void vector_copy_vector(vector_t *q, int qi, vector_t *p, int pi, int n);
  void vector_copy_ivector(vector_t *q, int qi, vector_t *p, int pi, int n);
  void ivector_copy_vector(vector_t *q, int qi, vector_t *p, int pi, int n);
  void ivector_copy_ivector(vector_t *q, int qi, vector_t *p, int pi, int n);
  void cvector_copy_cvector(vector_t *q, int qi, vector_t *p, int pi, int n);

  // add scalar
  vector_t *vector_copy_vector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_copy_ivector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_copy_vector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_copy_ivector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_add_vector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_add_ivector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_add_vector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_add_ivector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_subtract_vector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_subtract_ivector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_subtract_vector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_subtract_ivector_add_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_add_scalar(vector_t *c, real_t v);
  vector_t *ivector_add_scalar(vector_t *c, real_t v);
  vector_t *cvector_add_scalar(vector_t *c, complex_t v);
  vector_t *cvector_copy_cvector_add_scalar(vector_t *c, vector_t *a, complex_t v);
  vector_t *cvector_add_cvector_add_scalar(vector_t *c, vector_t *a, complex_t v);
  vector_t *cvector_subtract_cvector_add_scalar(vector_t *c, vector_t *a, complex_t v);
  // subtract scalar
  vector_t *vector_copy_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_copy_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_copy_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_copy_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_add_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_add_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_add_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_add_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_subtract_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_subtract_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_subtract_vector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_subtract_ivector_subtract_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_subtract_scalar(vector_t *c, real_t v);
  vector_t *ivector_subtract_scalar(vector_t *c, real_t v);
  vector_t *cvector_subtract_scalar(vector_t *c, complex_t v);
  vector_t *cvector_copy_cvector_subtract_scalar(vector_t *c, vector_t *a, complex_t v);
  vector_t *cvector_add_cvector_subtract_scalar(vector_t *c, vector_t *a, complex_t v);
  vector_t *cvector_subtract_cvector_subtract_scalar(vector_t *c, vector_t *a, complex_t v);
  // multiplication scalar
  vector_t *vector_copy_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_copy_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_copy_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_copy_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_add_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_add_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_add_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_add_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_subtract_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_subtract_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_subtract_vector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_subtract_ivector_multiply_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_multiply_scalar(vector_t *c, real_t v);
  vector_t *ivector_multiply_scalar(vector_t *c, real_t v);
  vector_t *cvector_multiply_scalar(vector_t *c, complex_t v);
  vector_t *cvector_copy_cvector_multiply_scalar(vector_t *c, vector_t *a, complex_t v);
  vector_t *cvector_add_cvector_multiply_scalar(vector_t *c, vector_t *a, complex_t v);
  vector_t *cvector_subtract_cvector_multiply_scalar(vector_t *c, vector_t *a, complex_t v);
  // division scalar
  vector_t *vector_copy_vector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_copy_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_copy_vector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_copy_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_add_vector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_add_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_add_vector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_add_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_subtract_vector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_subtract_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_subtract_vector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *ivector_subtract_ivector_divide_scalar(vector_t *c, vector_t *a, real_t v);
  vector_t *vector_divide_scalar(vector_t *c, real_t v);
  vector_t *ivector_divide_scalar(vector_t *c, real_t v);
  vector_t *cvector_divide_scalar(vector_t *c, complex_t v);
  vector_t *cvector_copy_cvector_divide_scalar(vector_t *c, vector_t *a, complex_t v);
  vector_t *cvector_add_cvector_divide_scalar(vector_t *c, vector_t *a, complex_t v);
  vector_t *cvector_subtract_cvector_divide_scalar(vector_t *c, vector_t *a, complex_t v);
  // addtion
  vector_t *vector_copy_vector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_copy_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_copy_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_copy_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_vector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_vector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_vector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_vector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_vector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_vector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_ivector_add_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_ivector_add_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_vector(vector_t *c, vector_t *a);
  vector_t *ivector_add_ivector(vector_t *c, vector_t *a);
  vector_t *cvector_add_cvector(vector_t *c, vector_t *a);
  vector_t *cvector_copy_cvector_add_cvector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *cvector_add_cvector_add_cvector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *cvector_subtract_cvector_add_cvector(vector_t *c, vector_t *a, vector_t *b);
  // subtract
  vector_t *vector_copy_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_copy_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_copy_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_copy_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_vector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_vector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_ivector_subtract_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_ivector_subtract_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_vector(vector_t *c, vector_t *a);
  vector_t *ivector_subtract_ivector(vector_t *c, vector_t *a);
  vector_t *cvector_subtract_cvector(vector_t *c, vector_t *a);
  vector_t *cvector_copy_cvector_subtract_cvector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *cvector_add_cvector_subtract_cvector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *cvector_subtract_cvector_subtract_cvector(vector_t *c, vector_t *a, vector_t *b);
  // multiply
  vector_t *vector_copy_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_copy_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_copy_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_copy_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_copy_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_add_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_add_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_subtract_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_vector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_vector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_ivector_multiply_vector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *ivector_subtract_ivector_multiply_ivector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *cvector_copy_cvector_multiply_cvector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *cvector_add_cvector_multiply_cvector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *cvector_subtract_cvector_multiply_cvector(vector_t *c, vector_t *a, vector_t *b);
  vector_t *vector_multiply_vector(vector_t *c, vector_t *b);
  vector_t *vector_multiply_ivector(vector_t *c, vector_t *b);
  vector_t *ivector_multiply_vector(vector_t *c, vector_t *b);
  vector_t *ivector_multiply_ivector(vector_t *c, vector_t *b);
  vector_t *cvector_multiply_cvector(vector_t *c, vector_t *b);
  // dot product
  real_t vector_dotproduct_vector(real_t *c, vector_t *a, vector_t *b);
  real_t vector_dotproduct_ivector(real_t *c, vector_t *a, vector_t *b);
  real_t ivector_dotproduct_vector(real_t *c, vector_t *a, vector_t *b);
  real_t ivector_dotproduct_ivector(real_t *c, vector_t *a, vector_t *b);
  complex_t cvector_dotproduct_cvector(complex_t *c, vector_t *a, vector_t *b);
  // summation
  real_t vector_get_sum(vector_t *p);
  real_t ivector_get_sum(vector_t *p);
  complex_t cvector_get_sum(vector_t *p);
  // norm
  real_t vector_get_norm(vector_t *p);
  real_t ivector_get_norm(vector_t *p);
  complex_t cvector_get_norm(vector_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __VECTOR_H__ */
