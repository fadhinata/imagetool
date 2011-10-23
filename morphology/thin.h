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
#ifndef __THIN_H__
#define __THIN_H__

#include <pixmap/bitmap.h>
#include <buffering/dlink.h>

#ifdef __cplusplus
extern "C" {
#endif

  void stentiford_pre_smooth_for_thinning(bitmap_t *q, bitmap_t *p);
  void stentiford_acute_angle_emphasis_for_thinning(bitmap_t *q, bitmap_t *p);
  void stentiford_thinning(bitmap_t *q, bitmap_t *p);
  void holt_et_al_thinning(bitmap_t *q, bitmap_t *p);
  void zhang_suen_thinning(bitmap_t *q, bitmap_t *p);
  void zhangsuen_holt_thinning(bitmap_t *q, bitmap_t *p, int stair_remove);
  void thinning_to_edge_tree(dlist_t *edge_tree, bitmap_t *bin);
  void edge_tree_dump(dlist_t *edge_tree);
  void edge_tree_delete(dlist_t *edge_tree);
  dlist_t *edge_tree_get_long_edge(dlist_t *edge_tree);

#ifdef __cplusplus
}
#endif

#endif /* __THIN_H__ */
