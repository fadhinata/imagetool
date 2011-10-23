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
#ifndef __KIRSCH_H__
#define __KIRSCH_H__

#include <common.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <linear_algebra/matrix.h>

#ifdef __cplusplus
extern "C" {
#endif

  void matrix_kirsch_gradient(matrix_t *grad, bytemap_t *p);
  void kirsch_gradient(wordmap_t *gx, wordmap_t *gy, bytemap_t *p);
  void kirsch_edge(bitmap_t *q, bytemap_t *p, real_t tlevel);

#ifdef __cplusplus
}
#endif

#endif /* __KIRSCH_H__ */
