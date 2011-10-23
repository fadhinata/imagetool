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

bitmap_t *bitmap_new(int w, int h)
{
  int pitch = WIDTHBYTES(w);
  bitmap_t *m;

  m = (bitmap_t *)malloc(sizeof(*m)+h*pitch);
  assert(m);

  m->header.width = w;
  m->header.height = h;
  m->header.pitch = pitch;
  m->buffer = (uint8_t *)m + sizeof(*m);

  memset(m->buffer, 0, h*pitch);

  return m;
}

int bitmap_copy(bitmap_t *q, int x2, int y2, bitmap_t *p, int x1, int y1, int w, int h)
{
  int i, j, qpitch, ppitch;
  int dx = 0, dy = 0;
  uint8_t *qbuf, *pbuf;

  assert(q);
  assert(p);
  assert(w > 0);
  assert(h > 0);

  // x-coordinate verification
  if (x1 < 0) dx += -x1;
  else if (x1 >= bitmap_get_width(p)) return -1;
  if (dx > 0) { x1 += dx, x2 += dx, w -= dx; dx = 0; }
	
  if (x2 < 0) dx += -x2;
  else if (x2 >= bitmap_get_width(q)) return -1;
  if (dx > 0) { x1 += dx, x2 += dx, w -= dx; dx = 0; }

  w = min(w, min(bitmap_get_width(p) - x1, bitmap_get_width(q) - x2));

  // y-coordinate verification
  if (y1 < 0) dy += -y1;
  else if (y1 >= bitmap_get_height(p)) return -1;
  if (dy > 0) { y1 += dy, y2 += dy, h -= dy; dy = 0; }

  if (y2 < 0) dy += -y2;
  else if (y2 >= bitmap_get_height(q)) return -1;
  if (dy > 0) { y1 += dy, y2 += dy, h -= dy; dy = 0; }

  h = min(h, min(bitmap_get_height(p) - y1, bitmap_get_height(q) - y2));
  
  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);
  qbuf = bitmap_get_buffer(q) + y2 * qpitch;

  ppitch = bitmap_get_pitch(p) / sizeof(*pbuf);
  pbuf = bitmap_get_buffer(p) + y1 * ppitch;

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if ((*(pbuf + ((x1 + j) >> 3))) & (1 << ((x1 + j) % 8))) { // 1
	*(qbuf + ((x2 + j) >> 3)) |= (1 << ((x2 + j) % 8));
      } else { // 0
	*(qbuf + ((x2 + j) >> 3)) &= ~(1 << ((x2 + j) % 8));
      }
    }
    qbuf += qpitch;
    pbuf += ppitch;
  }

  return w * h;
}

void bitmap_copy_not_bitmap(bitmap_t *q, bitmap_t *p)
{
  int x, y, /* w,*/ h, qpitch, ppitch;
  uint8_t *qbuf, *pbuf;
	
  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  //w = bitmap_get_width(q);
  h = bitmap_get_height(q);

  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);
  qbuf = bitmap_get_buffer(q);

  ppitch = bitmap_get_pitch(p) / sizeof(*pbuf);
  pbuf = bitmap_get_buffer(p);

  for (y = 0; y < h; y++) {
    for (x = 0; x < qpitch; x++) *(qbuf + x) = ~(*(pbuf + x));
    qbuf += qpitch;
    pbuf += ppitch;
  }
}

void bitmap_not(bitmap_t *p)
{
  int x, y, /* w, */ h, pitch;
  uint8_t *buf;
	
  assert(p);

  //w = bitmap_get_width(p);
  h = bitmap_get_height(p);

  pitch = bitmap_get_pitch(p) / sizeof(*buf);
  buf = bitmap_get_buffer(p);

  for (y = 0; y < h; y++) {
    for (x = 0; x < pitch; x++) *(buf + x) = ~(*(buf + x));
    buf += pitch;
  }
}

void bitmap_copy_bitmap_and_bitmap(bitmap_t *c, bitmap_t *a, bitmap_t *b)
{
  int x, y, /* w, */ h, cpitch, apitch, bpitch;
  uint8_t *cbuf, *abuf, *bbuf;
	
  assert(c);
  assert(a);
  assert(b);
  assert(bitmap_get_width(c) == bitmap_get_width(a));
  assert(bitmap_get_width(c) == bitmap_get_width(b));
  assert(bitmap_get_height(c) == bitmap_get_height(a));
  assert(bitmap_get_height(c) == bitmap_get_height(b));

  //w = bitmap_get_width(c);
  h = bitmap_get_height(c);

  cpitch = bitmap_get_pitch(c) / sizeof(*cbuf);
  cbuf = bitmap_get_buffer(c);

  apitch = bitmap_get_pitch(a) / sizeof(*abuf);
  abuf = bitmap_get_buffer(a);

  bpitch = bitmap_get_pitch(b) / sizeof(*bbuf);
  bbuf = bitmap_get_buffer(b);

  for (y = 0; y < h; y++) {
    for (x = 0; x < cpitch; x++) {
      *(cbuf + x) = (*(abuf + x)) & (*(bbuf + x));
    }
    cbuf += cpitch;
    abuf += apitch;
    bbuf += bpitch;
  }
}

void bitmap_and(bitmap_t *q, bitmap_t *p)
{
  int x, y, /* w, */ h, qpitch, ppitch;
  uint8_t *qbuf, *pbuf;
	
  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  //w = bitmap_get_width(q);
  h = bitmap_get_height(q);

  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);
  qbuf = bitmap_get_buffer(q);

  ppitch = bitmap_get_pitch(p) / sizeof(*pbuf);
  pbuf = bitmap_get_buffer(p);

  for (y = 0; y < h; y++) {
    for (x = 0; x < qpitch; x++)
      *(qbuf + x) = (*(qbuf + x)) & (*(pbuf + x));
    qbuf += qpitch;
    pbuf += ppitch;
  }
}

void bitmap_copy_bitmap_or_bitmap(bitmap_t *c, bitmap_t *a, bitmap_t *b)
{
  int x, y, /* w, */ h, cpitch, apitch, bpitch;
  uint8_t *cbuf, *abuf, *bbuf;

  assert(c);
  assert(a);
  assert(b);
  assert(bitmap_get_width(c) == bitmap_get_width(a));
  assert(bitmap_get_width(c) == bitmap_get_width(b));
  assert(bitmap_get_height(c) == bitmap_get_height(a));
  assert(bitmap_get_height(c) == bitmap_get_height(b));

  //w = bitmap_get_width(c);
  h = bitmap_get_height(c);

  cpitch = bitmap_get_pitch(c) / sizeof(*cbuf);
  cbuf = bitmap_get_buffer(c);

  apitch = bitmap_get_pitch(a) / sizeof(*abuf);
  abuf = bitmap_get_buffer(a);

  bpitch = bitmap_get_pitch(b) / sizeof(*bbuf);
  bbuf = bitmap_get_buffer(b);

  for (y = 0; y < h; y++) {
    for (x = 0; x < cpitch; x++)
      *(cbuf + x) = (*(abuf + x)) | (*(bbuf + x));
    cbuf += cpitch;
    abuf += apitch;
    bbuf += bpitch;
  }
}

void bitmap_or(bitmap_t *q, bitmap_t *p)
{
  int x, y, /* w, */ h, qpitch, ppitch;
  uint8_t *qbuf, *pbuf;
	
  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  h = bitmap_get_height(q);

  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);
  qbuf = bitmap_get_buffer(q);

  ppitch = bitmap_get_pitch(p) / sizeof(*pbuf);
  pbuf = bitmap_get_buffer(p);

  for (y = 0; y < h; y++) {
    for (x = 0; x < qpitch; x++)
      *(qbuf + x) = (*(qbuf + x)) | (*(pbuf + x));
    qbuf += qpitch;
    pbuf += ppitch;
  }
}

void bitmap_copy_bitmap_xor_bitmap(bitmap_t *c, bitmap_t *a, bitmap_t *b)
{
  int x, y, /* w, */ h, cpitch, apitch, bpitch;
  uint8_t *cbuf, *abuf, *bbuf;
	
  assert(c);
  assert(a);
  assert(b);
  assert(bitmap_get_width(c) == bitmap_get_width(a));
  assert(bitmap_get_width(c) == bitmap_get_width(b));
  assert(bitmap_get_height(c) == bitmap_get_height(a));
  assert(bitmap_get_height(c) == bitmap_get_height(b));

  //w = bitmap_get_width(c);
  h = bitmap_get_height(c);

  cpitch = bitmap_get_pitch(c) / sizeof(*cbuf);
  cbuf = bitmap_get_buffer(c);

  apitch = bitmap_get_pitch(a) / sizeof(*abuf);
  abuf = bitmap_get_buffer(a);

  bpitch = bitmap_get_pitch(b) / sizeof(*bbuf);
  bbuf = bitmap_get_buffer(b);

  for (y = 0; y < h; y++) {
    for (x = 0; x < cpitch; x++)
      *(cbuf + x) = (*(abuf + x)) ^ (*(bbuf + x));
    cbuf += cpitch;
    abuf += apitch;
    bbuf += bpitch;
  }
}

void bitmap_xor(bitmap_t *q, bitmap_t *p)
{
  int x, y, /* w, */ h, qpitch, ppitch;
  uint8_t *qbuf, *pbuf;
	
  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  //w = bitmap_get_width(q);
  h = bitmap_get_height(q);

  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);
  qbuf = bitmap_get_buffer(q);

  ppitch = bitmap_get_pitch(p) / sizeof(*pbuf);
  pbuf = bitmap_get_buffer(p);

  for (y = 0; y < h; y++) {
    for (x = 0; x < qpitch; x++)
      *(qbuf + x) = (*(qbuf + x)) ^ (*(pbuf + x));
    qbuf += qpitch;
    pbuf += ppitch;
  }
}

void bitmap_complement(bitmap_t *q, bitmap_t *p)
{
  int x, y, /* w, */ h, qpitch, ppitch;
  uint8_t *qbuf, *pbuf;

  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  //w = bitmap_get_width(q);
  h = bitmap_get_height(q);

  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);
  qbuf = bitmap_get_buffer(q);

  ppitch = bitmap_get_pitch(p) / sizeof(*pbuf);
  pbuf = bitmap_get_buffer(p);

  for (y = 0; y < h; y++) {
    for (x = 0; x < qpitch; x++)
      *(qbuf + x) = (*(pbuf + x)) ^ 0xff;
    qbuf += qpitch;
    pbuf += ppitch;
  }
}

void bitmap_set(bitmap_t *m, int x, int y, int dx, int dy)
{
  int i, j, pitch;
  int nx, ny;
  //int offset1, offset2;
  uint8_t *buf;

  assert(m);
  assert(x > 0 && x < bitmap_get_width(m));
  assert(y > 0 && y < bitmap_get_height(m));
  assert(dx > 0);
  assert(dy > 0);

  nx = min(x + dx, bitmap_get_width(m));
  ny = min(y + dy, bitmap_get_height(m));

  pitch = bitmap_get_pitch(m) / sizeof(*buf);
  buf = bitmap_get_buffer(m) + y * pitch;

  //offset1 = (8 - (x % 8)) % 8;
  //offset2 = (nx % 8);
  for (i = y; i < ny; i++) {
#if 1
    for (j = x; j < nx; j++)
      *(buf + (j>>3)) |= 1 << (j % 8);
#else // fast version (it needs to test !!!)
    if (offset1 > 0) {
      for (j = 0; j < offset1; j++)
	*(buf+((x+j)>>3)) |= 1<<((x+j)%8);
    }
    for (j = ((x+offset1)>>3); j < ((nx-offset2)>>3); j++)
      *(buf+j) |= 0xff;
    if (offset2 > 0) {
      for (j = 0; j < offset2; j++)
	*(buf+((nx+j)>>3)) |= 1<<((nx+j)%8);
    }
#endif
    buf += pitch;
  }
}

void bitmap_reset(bitmap_t *m, int x, int y, int dx, int dy)
{
  int i, j, pitch;
  int nx, ny;
  uint8_t *buf;

  assert(m);
  assert(x > 0 && x < bitmap_get_width(m));
  assert(y > 0 && y < bitmap_get_height(m));
  assert(dx > 0);
  assert(dy > 0);

  nx = min(x + dx, bitmap_get_width(m));
  ny = min(y + dy, bitmap_get_height(m));

  pitch = bitmap_get_pitch(m) / sizeof(*buf);
  buf = bitmap_get_buffer(m) + y * pitch;

  for (i = y; i < ny; i++) {
#if 1
    for (j = x; j < nx; j++)
      *(buf + (j >> 3)) &= ~(1 << (j % 8));
#else // fast version (it needs to test !!!)
    if (offset1 > 0) {
      for (j = 0; j < offset1; j++)
	*(bbuf+((x+j)>>3)) &= ~(1<<((x+j)%8));
    }
    for (j = ((x+offset1)>>3); j < ((nx-offset2)>>3); j++)
      *(buff+j) &= 0x00;
    if (offset2 > 0) {
      for (j = 0; j < offset2; j++)
	*(bbuf+((nx+j)>>3)) &= ~(1<<((nx+j)%8));
    }
#endif
    buf += pitch;
  }
}
