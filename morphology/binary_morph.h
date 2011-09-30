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
#ifndef __BINARY_MORPH_H__
#define __BINARY_MORPH_H__

#include <bitmap.h>
#include <bytemap.h>

#ifdef __cplusplus
extern "C" {
#endif

  void binary_dilate(bitmap_t *q, bitmap_t *p, bytemap_t *kernel);
  void binary_erode(bitmap_t *q, bitmap_t *p, bytemap_t *kernel);
  void binary_opening(bitmap_t *q, bitmap_t *p, bytemap_t *kernel);
  void binary_closing(bitmap_t *q, bitmap_t *p, bytemap_t *kernel);
  void hit_and_miss(bitmap_t *q, bitmap_t *p, bytemap_t *hit_kernel, bytemap_t *miss_kernel);
  /* filling hole nested by white region */
  void region_filling(bitmap_t *q, bitmap_t *p, int xhole, int yhole);
  void morphology_boundary(bitmap_t *q, bitmap_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __BINARY_MORPH_H__ */
