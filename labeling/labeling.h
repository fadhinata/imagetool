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
#ifndef __LABELING_H__
#define __LABELING_H__

#include <pixmap/bitmap.h>
#include <pixmap/dwordmap.h>
#include <linear_algebra/vector.h>
#include <linear_algebra/matrix.h>
#include <geometry/neighbor.h>
#include <buffering/dlink.h>
#include <geometry/point.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UNLABEL (-1)

  typedef dlist_t label_info_t;

#define label_info_new() (label_info_t *)dlist_new()
#define label_info_get_count(x) ((x)->count)
  point_t *label_info_glimpse(int *pixels,int i, label_info_t *label_info);
  void label_info_clear(label_info_t *labelinfo);
  void label_info_destroy(label_info_t *labelinfo);
  int labeling(dwordmap_t *labelmap, label_info_t *labelinfo, bitmap_t *bin, neighbor_t type);
  int labeling_grep_big_blob(bitmap_t *q, bitmap_t *p);
  int labeling_read_area_sort(int *label, int len, label_info_t *labelinfo);
  void labeling_fill_holes(bitmap_t *q, bitmap_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __LABELING_H__ */
