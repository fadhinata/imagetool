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
#ifndef __COMPLEX2_H__
#define __COMPLEX2_H__

#include <common.h>

#ifdef __cplusplus
extern "C" {
#endif 

  typedef struct {
    real_t real;
    real_t imag;
  } complex_t;

  __inline complex_t *complex_copy_complex_add_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_add_complex_add_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_subtract_complex_add_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_add_complex(complex_t *c, complex_t *b);
  __inline complex_t *complex_copy_complex_subtract_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_add_complex_subtract_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_subtract_complex_subtract_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_subtract_complex(complex_t *c, complex_t *b);
  __inline complex_t *complex_copy_complex_multiply_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_add_complex_multiply_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_subtract_complex_multiply_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_multiply_complex(complex_t *c, complex_t *b);
  __inline complex_t *complex_copy_complex_divide_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_add_complex_divide_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_subtract_complex_divide_complex(complex_t *c, complex_t *a, complex_t *b);
  __inline complex_t *complex_divide_complex(complex_t *c, complex_t *b);
  __inline complex_t *complex_copy_complex_sqrt(complex_t *q, complex_t *p);
  __inline complex_t *complex_add_complex_sqrt(complex_t *q, complex_t *p);
  __inline complex_t *complex_subtract_complex_sqrt(complex_t *q, complex_t *p);
  __inline complex_t complex_sqrt(complex_t *q);
  __inline real_t complex_get_abs(complex_t *q);
  __inline real_t complex_abs(complex_t *q);
  __inline int complex_compare_abs(complex_t *q, complex_t *p);
  __inline int complex_compare(complex_t *q, complex_t *p);
  __inline complex_t *complex_conjugate(complex_t *q, complex_t *p);

  
#ifdef __cplusplus
}
#endif 

#endif /* __COMPLEX2_H__ */
