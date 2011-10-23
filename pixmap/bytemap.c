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
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include <common.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>

bytemap_t *bytemap_new(int w, int h)
{
  int pitch = WIDTHBYTES(w*8);
  bytemap_t *m;

  m = (bytemap_t *)malloc(sizeof(*m) + h * pitch);
  assert(m);

  m->header.width = w;
  m->header.height = h;
  m->header.pitch = pitch;
  m->buffer = (uint8_t *)m + sizeof(*m);
  memset(m->buffer, 0, h * pitch);

  return m;
}

int bytemap_copy(bytemap_t *q, int x2, int y2, bytemap_t *p, int x1, int y1, int w, int h)
{
  int i, qpitch, ppitch;
  int dx = 0, dy = 0;
  uint8_t *qbuf, *pbuf;
	
  assert(q);
  assert(p);
  assert(w > 0);
  assert(h > 0);
	
  // x-coordinate verification
  if (x1 < 0) dx += -x1;
  else if (x1 >= bytemap_get_width(p)) return -1;
  if (dx > 0) { x1 += dx, x2 += dx, w -= dx; dx = 0; }
	
  if (x2 < 0) dx += -x2;
  else if (x2 >= bytemap_get_width(q)) return -1;
  if (dx > 0) { x1 += dx, x2 += dx, w -= dx; dx = 0; }

  w = min(w, min(bytemap_get_width(p) - x1, bytemap_get_width(q) - x2));

  // y-coordinate verification
  if (y1 < 0) dy += -y1;
  else if (y1 >= bytemap_get_height(p)) return -1;
  if (dy > 0) { y1 += dy, y2 += dy, h -= dy; dy = 0; }
	
  if (y2 < 0) dy += -y2;
  else if (y2 >= bytemap_get_height(q)) return -1;
  if (dy > 0) { y1 += dy, y2 += dy, h -= dy; dy = 0; }

  h = min(h, min(bytemap_get_height(p) - y1, bytemap_get_height(q) - y2));

  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);
  qbuf = bytemap_get_buffer(q) + y2 * qpitch + x2;

  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);
  pbuf = bytemap_get_buffer(p) + y1 * ppitch + x1;

  for (i = 0; i < h; i++) {
    memcpy(qbuf, pbuf, w * sizeof(*pbuf));
    qbuf += qpitch;
    pbuf += ppitch;
  }

  return w * h;
}

void bytemap_add(bytemap_t *c, bytemap_t *a, int b)
{
  int i, j, w, h, v, cpitch, apitch;
  uint8_t *cbuf, *abuf;

  assert(c);
  assert(a);
  assert(bytemap_get_width(c) == bytemap_get_width(a));
  assert(bytemap_get_height(c) == bytemap_get_height(a));

  w = bytemap_get_width(c);
  h = bytemap_get_height(c);

  cpitch = bytemap_get_pitch(c) / sizeof(*cbuf);
  cbuf = bytemap_get_buffer(c);

  apitch = bytemap_get_pitch(a) / sizeof(*abuf);
  abuf = bytemap_get_buffer(a);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      v = *(abuf + j) + b;
      *(cbuf + j) = (v < 0 ? 0 : (v > UINT8_MAX ? UINT8_MAX : v));
    }
    cbuf += cpitch;
    abuf += apitch;
  }
}

void bytemap_bezero(bytemap_t *q, int x, int y, int w, int h)
{
  int i, nx, ny, qpitch;
  uint8_t *qbuf;

  assert(q);
  assert(x >= 0 && x < bytemap_get_width(q));
  assert(y >= 0 && y < bytemap_get_height(q));
  assert(w > 0 && (x + w) <= bytemap_get_width(q));
  assert(h > 0 && (y + h) <= bytemap_get_height(q));

  nx = min(x + w, bytemap_get_width(q)) - x;
  ny = min(y + h, bytemap_get_height(q)) - y;

  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);
  qbuf = bytemap_get_buffer(q) + y * qpitch + x;

  for (i = 0; i < ny; i++) {
    memset(qbuf, 0, nx * sizeof(*qbuf));
    qbuf += qpitch;
  }
}

void bytemap_fill(bytemap_t *q, int x, int y, int w, int h, uint8_t v)
{
  int i, j, nx, ny, qpitch;
  uint8_t *qbuf;

  assert(q);
  assert(x >= 0 && x < bytemap_get_width(q));
  assert(y >= 0 && y < bytemap_get_height(q));
  assert(w > 0 && (x + w) <= bytemap_get_width(q));
  assert(h > 0 && (y + h) <= bytemap_get_height(q));

  nx = min(x + w, bytemap_get_width(q)) - x;
  ny = min(y + h, bytemap_get_height(q)) - y;

  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);
  qbuf = bytemap_get_buffer(q) + y * qpitch + x;

  for (i = 0; i < ny; i++) {
    for (j = 0; j < nx; j++) *(qbuf + j) = v;
    qbuf += qpitch;
  }
}

void bytemap_mask(bytemap_t *q, bitmap_t *p)
{
  int x, y, w, h, qpitch, ppitch;
  uint8_t *qbuf, *pbuf;

  assert(q);
  assert(p);
  assert(bytemap_get_width(q) == bitmap_get_width(p));
  assert(bytemap_get_height(q) == bitmap_get_height(p));

  w = bytemap_get_width(q);
  h = bytemap_get_height(q);

  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);
  qbuf = bytemap_get_buffer(q);

  ppitch = bitmap_get_pitch(p) / sizeof(*pbuf);
  pbuf = bitmap_get_buffer(p);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      if (!(*(pbuf + (x >> 3)) & (1 << (x % 8))))
	*(qbuf + x) = 0;
    }
    qbuf += qpitch;
    pbuf += ppitch;
  }
}
