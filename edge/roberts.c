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
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <common.h>
#include <linear_algebra/matrix.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>

void roberts_magnitude(wordmap_t *q, bytemap_t *p)
{
  int x, y, w, h, qpitch, ppitch;
  int16_t *qbuf, h1, h2;
  uint8_t *pbuf;

  assert(q);
  assert(p);
  assert(wordmap_get_width(q) == bytemap_get_width(p));
  assert(wordmap_get_height(q) == bytemap_get_height(p));

  w = bytemap_get_width(p);
  h = bytemap_get_height(p);

  qbuf = (int16_t *)wordmap_get_buffer(q);
  qpitch = wordmap_get_pitch(q) / sizeof(*qbuf);

  pbuf = bytemap_get_buffer(p);
  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);

  for (y = 0; y < h - 1; y++) {
    for (x = 0; x < w - 1; x++) {
      h1 = *(pbuf + x) - *(pbuf + ppitch + (x + 1));
      h2 = *(pbuf + (x + 1)) - *(pbuf + ppitch + x);
      *(qbuf + x) = abs(h1) + abs(h2);
    }
    qbuf += qpitch;
    pbuf += ppitch;
  }
  // bottom band
  pbuf = bytemap_get_buffer(p) + (h - 1) * ppitch;
  qbuf = (int16_t *)(wordmap_get_buffer(q) + (h - 1) * qpitch);
  for (x = 0; x < w - 1; x++) {
    *(qbuf + x) = *(pbuf + x) + *(pbuf + (x + 1));
  }
  // right band
  pbuf = bytemap_get_buffer(p) + (w - 1);
  qbuf = (int16_t *)(wordmap_get_buffer(q) + (w - 1));
  for (y = 0; y < h - 1; y++) {
    *(qbuf) = *(pbuf) + *(pbuf + ppitch);
    pbuf += ppitch;
    qbuf += qpitch;
  }
  // right bottom
  *((int16_t *)(wordmap_get_buffer(q) + (h - 1) * qpitch + (w - 1))) = *(bytemap_get_buffer(p) + (h - 1) * ppitch + (w - 1));
}

void roberts_edge(bitmap_t *q, bytemap_t *p, int tlevel)
{
  int c, r, w, h;
  wordmap_t *mag;
  int16_t *magbuf;
  uint8_t *qbuf, *pbuf;
  int qpitch, ppitch, magpitch;

  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bytemap_get_width(p));
  assert(bitmap_get_height(q) == bytemap_get_height(p));

  w = bytemap_get_width(p);
  h = bytemap_get_height(p);

  mag = wordmap_new(w, h);

  roberts_magnitude(mag, p);

  qbuf = bitmap_get_buffer(q);
  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);

  pbuf = bytemap_get_buffer(p);
  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);

  magbuf = (int16_t *)wordmap_get_buffer(mag);
  magpitch = wordmap_get_pitch(mag) / sizeof(*magbuf);

  for (r = 0; r < h; r++) {
    for (c = 0; c < w; c++) {
      if (*(magbuf + c) > tlevel) {
	*(qbuf + (c>>3)) |= (1 << (c % 8));
      } else {
	*(qbuf + (c>>3)) &= ~(1 << (c % 8));
      }
    }
    qbuf += qpitch;
    pbuf += ppitch;
    magbuf += magpitch;
  }
  wordmap_destroy(mag);
}
