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
#ifndef __WATERSHED_H__
#define __WATERSHED_H__

//#define NDEBUG

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <common.h>
#include <assert.h>
#include <errno.h>

#include <linear_algebra/matrix.h>
#include <linear_algebra/vector.h>
#include <geometry/neighbor.h>
#include <buffering/dlink.h>

#ifdef __cplusplus
extern "C" {
#endif

  int bytemap_create_watershed(dwordmap_t *label_map, dlist_t *info, bytemap_t *image, neighbor_t type);
  int wordmap_create_watershed(dwordmap_t *label_map, dlist_t *info, wordmap_t *image, neighbor_t type);

#ifdef __cplusplus
}
#endif

#endif /* __WATERSHED_H__ */
