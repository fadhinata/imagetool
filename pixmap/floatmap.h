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
#ifndef __FLOATMAP_H__
#define __FLOATMAP_H__

#include <pixmap/maphdr.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    maphdr_t header;
    float *buffer;
  } floatmap_t;

  floatmap_t *floatmap_new(int w, int h);
  void floatmap_destroy(floatmap_t *m);
  floatmap_t *floatmap_clone(void *p);

#define floatmap_get_width(m) ((m)->header.width)
#define floatmap_get_height(m) ((m)->header.height)
#define floatmap_get_pitch(m) ((m)->header.pitch)
#define floatmap_get_buffer(m) ((m)->buffer)
#define floatmap_get_value(m, x, y) *((float *)((m)->buffer + (y) * (m)->header.pitch / sizeof(float) + (x)))
#define floatmap_put_value(v, m, x, y) (*((float *)((m)->buffer + (y) * (m)->header.pitch / sizeof(float) + (x))) = (v))

#ifdef __cplusplus
}
#endif

#endif /* __FLOATMAP_H__ */
