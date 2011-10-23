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

// Reference by 
// region growing: A New Approach - S. A. Hojjatoleslami and J. Kittler */

#ifndef __REGIONGROW_H__
#define __REGIONGROW_H__

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>

#include <common.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <geometry/neighbor.h>

#ifdef __cplusplus
extern "C" {
#endif

  int region_growing_by_contrast(bitmap_t *region, bytemap_t *image, neighbor_t type, int xseed, int yseed, int maxcount, real_t maxac, real_t maxpc);

#ifdef __cplusplus
}
#endif

#endif /* __REGIONGROW_H__ */
