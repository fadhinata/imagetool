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

#ifndef __WORDMAP_H__
#define __WORDMAP_H__

#ifdef _MSC_VER
typedef unsigned __int16 uint16_t;
#else
#include <stdint.h>
#endif

#include <pixmap/maphdr.h>
#include <pixmap/bitmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    maphdr_t header;
    uint16_t *buffer;
  } wordmap_t;

#define wordmap_get_width(m) ((m)->header.width)
#define wordmap_get_height(m) ((m)->header.height)
#define wordmap_get_pitch(m) ((m)->header.pitch)
#define wordmap_get_buffer(m) ((m)->buffer)
#define wordmap_get_value(m, x, y) (*((m)->buffer + (y) * (m)->header.pitch / sizeof(int16_t) + (x)))
#define wordmap_put_value(v, m, x, y) (*((m)->buffer + (y) * (m)->header.pitch / sizeof(int16_t) + (x)) = v)
  wordmap_t *wordmap_new(int w, int h);
#define wordmap_destroy(m) { assert(m); free(m); }
#define wordmap_clone(m) wordmap_new(wordmap_get_width(m), wordmap_get_height(m))
  int wordmap_copy(wordmap_t *q, int x2, int y2, wordmap_t *p, int x1, int y1, int w, int h);
  void wordmap_add(wordmap_t *c, wordmap_t *a, int b);
  void wordmap_bezero(wordmap_t *q, int x, int y, int w, int h);
#define wordmap_clear(m) memset(wordmap_get_buffer(m), 0, wordmap_get_height(m) * wordmap_get_pitch(m))
  void wordmap_fill(wordmap_t *q, int x, int y, int w, int h, uint16_t v);
  void wordmap_mask(wordmap_t *q, bitmap_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __WORDMAP_H__ */
