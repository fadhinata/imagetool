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
#ifndef __DAUB4_H__
#define __DAUB4_H__

#include <linear_algebra/vector.h>
#include <linear_algebra/matrix.h>

#ifdef __cplusplus
extern "C" {
#endif

  void daub4_fwt(vector_t *q, vector_t *p);
  void daub4_ifwt(vector_t *q, vector_t *p);
  int daub4_fwt2(matrix_t *q, matrix_t *p);
  int daub4_ifwt2(matrix_t *q, matrix_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __DAUB4_H__ */
