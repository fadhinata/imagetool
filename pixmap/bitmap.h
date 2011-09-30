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

#ifndef __BITMAP_H__
#define __BITMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <maphdr.h>

  typedef struct {
    maphdr_t header;
    uint8_t *buffer;
  } bitmap_t;

#define bitmap_isrange(x, y, p) (((x) >= 0 && (x) < (p)->header.width) && ((y) >= 0 && (y) < (p)->header.height))
#define bitmap_get_pitch(p) ((p)->header.pitch)
#define bitmap_get_buffer(p) ((p)->buffer)
#define bitmap_get_width(p) ((p)->header.width)
#define bitmap_get_height(p) ((p)->header.height)
#define bitmap_get_value(x, y, p) (((*((p)->buffer+((int)(y))*(p)->header.pitch+(((int)(x))>>3)))>>(((int)(x))%8))&1)
#define bitmap_set_value(x, y, p) ((*((p)->buffer+((int)(y))*(p)->header.pitch+(((int)(x))>>3))) |= 1<<(((int)(x))%8))
#define bitmap_reset_value(x, y, p) ((*((p)->buffer+((int)(y))*(p)->header.pitch+(((int)(x))>>3))) &= ~(1<<(((int)(x))%8)))
#define bitmap_isset(x, y, p) ((*((p)->buffer+((int)(y))*(p)->header.pitch+(((int)(x))>>3)))&(1<<(((int)(x))%8)))
#define bitmap_isreset(x, y, p) (!((*((p)->buffer+((int)(y))*(p)->header.pitch+(((int)(x))>>3)))&(1<<(((int)(x))%8))))
  bitmap_t *bitmap_new(int w, int h);
#define bitmap_destroy(m) { assert(m); free(m); }
#define bitmap_clone(m) bitmap_new(bitmap_get_width(m), bitmap_get_height(m))
  int bitmap_copy(bitmap_t *q, int x2, int y2, bitmap_t *p, int x1, int y1, int w, int h);
  void bitmap_copy_not_bitmap(bitmap_t *q, bitmap_t *p);
  void bitmap_not(bitmap_t *p);
  void bitmap_copy_bitmap_and_bitmap(bitmap_t *c, bitmap_t *a, bitmap_t *b);
  void bitmap_and(bitmap_t *q, bitmap_t *p);
  void bitmap_copy_bitmap_or_bitmap(bitmap_t *c, bitmap_t *a, bitmap_t *b);
  void bitmap_or(bitmap_t *q, bitmap_t *p);
  void bitmap_copy_bitmap_xor_bitmap(bitmap_t *c, bitmap_t *a, bitmap_t *b);
  void bitmap_xor(bitmap_t *q, bitmap_t *p);
#define bitmap_clear(m) { assert(m); memset(bitmap_get_buffer(m), 0, bitmap_get_height(m) * bitmap_get_pitch(m)); }
  void bitmap_complement(bitmap_t *q, bitmap_t *p);
  void bitmap_set(bitmap_t *m, int x, int y, int dx, int dy);
  void bitmap_reset(bitmap_t *m, int x, int y, int dx, int dy);

#ifdef __cplusplus
}
#endif

#endif /* __BITMAP_H__ */
