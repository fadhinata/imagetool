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

#ifndef __BYTEMAP_H__
#define __BYTEMAP_H__

#include <stdint.h>

#include <maphdr.h>
#include <bitmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    maphdr_t header;
    uint8_t *buffer;
  } bytemap_t;

#define bytemap_isrange(x, y, p) (((x) >= 0 && (x) < (p)->header.width) && ((y) >= 0 && (y) < (p)->header.height))
#define bytemap_get_width(m) ((m)->header.width)
#define bytemap_get_height(m) ((m)->header.height)
#define bytemap_get_pitch(m) ((m)->header.pitch)
#define bytemap_get_buffer(m) ((m)->buffer)
#define bytemap_get_value(x, y, m) (*((m)->buffer + (y) * (m)->header.pitch + (x)))
#define bytemap_put_value(v, x, y, m) (*((m)->buffer + (y) * (m)->header.pitch + (x))) = (v)
  bytemap_t *bytemap_new(int w, int h);
#define bytemap_destroy(m) { assert(m); free(m); }
#define bytemap_clone(m) bytemap_new(bytemap_get_width(m), bytemap_get_height(m))
  int bytemap_copy(bytemap_t *q, int x2, int y2, bytemap_t *p, int x1, int y1, int w, int h);
  void bytemap_add(bytemap_t *c, bytemap_t *a, int b);
  void bytemap_bezero(bytemap_t *q, int x, int y, int w, int h);
#define bytemap_clear(m) memset(bytemap_get_buffer(m), 0, bytemap_get_height(m) * bytemap_get_pitch(m))
  void bytemap_fill(bytemap_t *q, int x, int y, int w, int h, uint8_t v);
  void bytemap_mask(bytemap_t *q, bitmap_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __BYTEMAP_H__ */
