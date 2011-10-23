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

#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <pixmap/dwordmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  void bitmap_rotat90(bitmap_t *q, int qpxorg, int qpyorg, bitmap_t *p, int pxorg, int pyorg);
  void bytemap_rotat90(bytemap_t *q, int qpxorg, int qpyorg, bytemap_t *p, int pxorg, int pyorg);
  void wordmap_rotat90(wordmap_t *q, int qpxorg, int qpyorg, wordmap_t *p, int pxorg, int pyorg);
  void dwordmap_rotat90(dwordmap_t *q, int qpxorg, int qpyorg, dwordmap_t *p, int pxorg, int pyorg);

  void bitmap_rotat180(bitmap_t *q, int qpxorg, int qpyorg, bitmap_t *p, int pxorg, int pyorg);
  void bytemap_rotat180(bytemap_t *q, int qpxorg, int qpyorg, bytemap_t *p, int pxorg, int pyorg);
  void wordmap_rotat180(wordmap_t *q, int qpxorg, int qpyorg, wordmap_t *p, int pxorg, int pyorg);
  void dwordmap_rotat180(dwordmap_t *q, int qpxorg, int qpyorg, dwordmap_t *p, int pxorg, int pyorg);

  int bitmap_hshear(bitmap_t *q, int qpxorg, int qpyorg, bitmap_t *p, int pxorg, int pyorg, real_t ang);
  int bytemap_hshear(bytemap_t *q, int qpxorg, int qpyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang);
  int wordmap_hshear(wordmap_t *q, int qpxorg, int qpyorg, wordmap_t *p, int pxorg, int pyorg, real_t ang);
  int dwordmap_hshear(dwordmap_t *q, int qpxorg, int qpyorg, dwordmap_t *p, int pxorg, int pyorg, real_t ang);

  int bitmap_vshear(bitmap_t *q, int qpxorg, int qpyorg, bitmap_t *p, int pxorg, int pyorg, real_t ang);
  int bytemap_vshear(bytemap_t *q, int qpxorg, int qpyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang);
  int wordmap_vshear(wordmap_t *q, int qpxorg, int qpyorg, wordmap_t *p, int pxorg, int pyorg, real_t ang);
  int dwordmap_vshear(dwordmap_t *q, int qpxorg, int qpyorg, dwordmap_t *p, int pxorg, int pyorg, real_t ang);

  void bitmap_rotate_by_2shear(bitmap_t *q, int qpxorg, int qpyorg, bitmap_t *p, int pxorg, int pyorg, real_t ang);
  void bytemap_rotate_by_2shear(bytemap_t *q, int qpxorg, int qpyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang);
  void wordmap_rotate_by_2shear(wordmap_t *q, int qpxorg, int qpyorg, wordmap_t *p, int pxorg, int pyorg, real_t ang);
  void dwordmap_rotate_by_2shear(dwordmap_t *q, int qpxorg, int qpyorg, dwordmap_t *p, int pxorg, int pyorg, real_t ang);

  void bitmap_rotate_by_3shear(bitmap_t *q, int qpxorg, int qpyorg, bitmap_t *p, int pxorg, int pyorg, real_t ang);
  void bytemap_rotate_by_3shear(bytemap_t *q, int qpxorg, int qpyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang);
  void wordmap_rotate_by_3shear(wordmap_t *q, int qpxorg, int qpyorg, wordmap_t *p, int pxorg, int pyorg, real_t ang);
  void dwordmap_rotate_by_3shear(dwordmap_t *q, int qpxorg, int qpyorg, dwordmap_t *p, int pxorg, int pyorg, real_t ang);

  void bitmap_rotate_by_shear(bitmap_t *q, int qpxorg, int qpyorg, bitmap_t *p, int pxorg, int pyorg, real_t ang);
  void bytemap_rotate_by_shear(bytemap_t *q, int qpxorg, int qpyorg, bytemap_t *p, int pxorg, int pyorg, real_t ang);
  void wordmap_rotate_by_shear(wordmap_t *q, int qpxorg, int qpyorg, wordmap_t *p, int pxorg, int pyorg, real_t ang);
  void dwordmap_rotate_by_shear(dwordmap_t *q, int qpxorg, int qpyorg, dwordmap_t *p, int pxorg, int pyorg, real_t ang);

#ifdef __cplusplus
}
#endif

#endif /* __ROTATE_H__ */
