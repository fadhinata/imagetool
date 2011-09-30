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

#ifndef __ROTATE_H__
#define __ROTATE_H__

#include <bitmap.h>
#include <bytemap.h>

#ifdef __cplusplus
extern "C" {
#endif

  int bitmap_hshear(bitmap_t *q, int xcent, int ycent, bitmap_t *p, int xorg, int yorg, real_t ang);
  int bitmap_vshear(bitmap_t *q, int xcent, int ycent, bitmap_t *p, int xorg, int yorg, real_t ang);
  int bitmap_rotate_by_2shear(bitmap_t *q, int xcent, int ycent, bitmap_t *p, int xorg, int yorg, real_t ang);
  int bitmap_rotate_by_3shear(bitmap_t *q, int xcent, int ycent, bitmap_t *p, int xorg, int yorg, real_t ang);
  int bitmap_rotate_by_shear(bitmap_t *q, int xcent, int ycent, bitmap_t *p, int xorg, int yorg, real_t ang);

  int bytemap_hshear(bytemap_t *q, int xcent, int ycent, bytemap_t *p, int xorg, int yorg, real_t ang);
  int bytemap_vshear(bytemap_t *q, int xcent, int ycent, bytemap_t *p, int xorg, int yorg, real_t ang);
  int bytemap_rotate_by_2shear(bytemap_t *q, int xcent, int ycent, bytemap_t *p, int xorg, int yorg, real_t ang);
  int bytemap_rotate_by_3shear(bytemap_t *q, int xcent, int ycent, bytemap_t *p, int xorg, int yorg, real_t ang);
  int bytemap_rotate_by_shear(bytemap_t *q, int xcent, int ycent, bytemap_t *p, int xorg, int yorg, real_t ang);

#ifdef __cplusplus
}
#endif

#endif /* __ROTATE_H__ */
