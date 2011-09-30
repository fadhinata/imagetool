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
#include <dwordmap.h>
#include <bitmap.h>
#include <dlink.h>
#include <point_list.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef dlist_t chaincode_t;

#define chaincode_new() (chaincode_t *)dlist_new()
  void chaincode_destroy(chaincode_t *chain);
  int dwordmap_create_chaincode(chaincode_t *chain, dwordmap_t *image, long label);
  int bitmap_create_chaincode(chaincode_t *chain, bitmap_t *image);
  real_t chaincode_get_perimeter(chaincode_t *chain, real_t pixel_xlen, real_t pixel_ylen);
  real_t chaincode_get_area(chaincode_t *chain, real_t pixel_xlen, real_t pixel_ylen);
  int chaincode_create_point_list(point_list_t *list, chaincode_t *chain);
  int dwordmap_delete_shell(dwordmap_t *pixmap, long label, int depth);
  int bitmap_delete_shell(bitmap_t *pixmap, int depth);

#ifdef __cplusplus
}
#endif

#endif /* __CHAINCODE_H__ */