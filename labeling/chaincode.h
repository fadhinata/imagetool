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
#ifndef __CHAINCODE_H__
#define __CHAINCODE_H__

#include <common.h>
#include <pixmap/dwordmap.h>
#include <pixmap/bitmap.h>
#include <buffering/dlink.h>
#include <geometry/point_list.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef dlist_t chaincode_t;

#define chaincode_new() (chaincode_t *)dlist_new()
  void chaincode_destroy(chaincode_t *chain);
  int chaincoding_on_dwordmap(chaincode_t *chain, dwordmap_t *image, long label);
  int chaincoding_on_bitmap(chaincode_t *chain, bitmap_t *image);
  real_t chaincode_compute_perimeter(chaincode_t *chain, real_t pixel_xlen, real_t pixel_ylen);
  real_t chaincode_compute_area(chaincode_t *chain, real_t pixel_xlen, real_t pixel_ylen);
  int chaincode_create_point_list(point_list_t *list, chaincode_t *chain);
  int dwordmap_strip_shell(dwordmap_t *pixmap, long label, int depth);
  int bitmap_strip_shell(bitmap_t *pixmap, int depth);

#ifdef __cplusplus
}
#endif

#endif /* __CHAINCODE_H__ */
