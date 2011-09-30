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

#ifndef __DWORDMAP_H__
#define __DWORDMAP_H__

#include <stdint.h>
#include <maphdr.h>
#include <bitmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    maphdr_t header;
    uint32_t *buffer;
  } dwordmap_t;

#define dwordmap_get_width(p) ((p)->header.width)
#define dwordmap_get_height(p) ((p)->header.height)
#define dwordmap_get_buffer(p) ((p)->buffer)
#define dwordmap_get_pitch(p) ((p)->header.pitch)
#define dwordmap_get_value(x, y, p) (*((p)->buffer+(y)*(((p)->header.pitch)/sizeof(long))+(x)))
#define dwordmap_put_value(q, x, y, p) (*((p)->buffer+(y)*((p)->header.pitch/sizeof(long))+(x)) = q)
#define dwordmap_inc_value(x, y, m) ((*((m)->buffer+(y)*((m)->header.pitch)/sizeof(*((m)->buffer)) + (x)))++)
#define dwordmap_dec_value(x, y, m) ((*((m)->buffer+(y)*((m)->header.pitch)/sizeof(*((m)->buffer)) + (x)))--)
  dwordmap_t *dwordmap_new(int w, int h);
#define dwordmap_destroy(m) { assert(m); free(m); }
#define dwordmap_clone(m) dwordmap_new(dwordmap_get_width(m), dwordmap_get_height(m))
  int dwordmap_copy(dwordmap_t *q, int x2, int y2, dwordmap_t *p, int x1, int y1, int w, int h);
  void dwordmap_add(dwordmap_t *c, dwordmap_t *a, int b);
  void dwordmap_bezero(dwordmap_t *q, int x, int y, int w, int h);
#define dwordmap_clear(m) memset(dwordmap_get_buffer(m), 0, dwordmap_get_height(m) * dwordmap_get_pitch(m))
  void dwordmap_fill(dwordmap_t *q, int x, int y, int w, int h, uint32_t v);
  void dwordmap_mask(dwordmap_t *q, bitmap_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __DWORDMAP_H__ */
