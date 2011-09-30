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

#include <vector.h>
#include <matrix.h>

#ifdef __cplusplus
extern "C" {
#endif

  void d4fwt(vector_t *q, vector_t *p);
  void id4fwt(vector_t *q, vector_t *p);
  int d4fwt2(matrix_t *q, matrix_t *p);
  int id4fwt2(matrix_t *q, matrix_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __DAUB4_H__ */
