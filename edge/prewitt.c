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
#include <malloc.h>
#include <assert.h>

#include <common.h>
#include <linear_algebra/matrix.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>

#define PREWITT_GRADIENT(type, gx_buf, gx_w, gx_h, gx_pitch, gy_buf, gy_w, gy_h, gy_pitch, p_buf, p_w, p_h, p_pitch) { \
    int x, y, w, h;							\
    int gxpitch, gypitch, ppitch;					\
    uint8_t *pbuf;							\
    type *gxbuf, *gybuf;						\
    assert(gx_buf);							\
    assert(gy_buf);							\
    assert(p_buf);							\
    assert(p_w == gx_w);						\
    assert(p_h == gx_h);						\
    assert(gx_w == gy_w);						\
    assert(gx_h == gy_h);						\
    w = p_w, h = p_h;							\
    ppitch = (p_pitch);							\
    gxpitch = (gx_pitch);						\
    gypitch = (gy_pitch);						\
									\
    pbuf = (p_buf) + ppitch;						\
    gxbuf = (type *)((gx_buf) + gxpitch);				\
    gybuf = (type *)((gy_buf) + gypitch);				\
    for (y = 1; y < h - 1; y++) {					\
      for (x = 1; x < w - 1; x++) {					\
	*(gxbuf+x) = -(*(pbuf - ppitch + (x - 1))) + (*(pbuf - ppitch + (x + 1))) \
	  -(*(pbuf + (x - 1))) + (*(pbuf + (x + 1)))			\
	  -(*(pbuf + ppitch + (x - 1))) + (*(pbuf + ppitch + (x + 1))); \
	*(gybuf+x) = -(*(pbuf - ppitch + (x - 1))) - (*(pbuf - ppitch + x)) - (*(pbuf - ppitch + (x + 1))) \
									\
	  +(*(pbuf+ppitch+(x-1)))+(*(pbuf+ppitch+x))+(*(pbuf+ppitch+(x+1))); \
      }									\
      pbuf += ppitch;							\
      gxbuf += gxpitch;							\
      gybuf += gypitch;							\
    }									\
									\
    pbuf = (p_buf);							\
    gxbuf = (type *)(gx_buf);						\
    gybuf = (type *)(gy_buf);						\
    for (x = 1; x < w-1; x++) {						\
      *(gxbuf+x) = -(*(pbuf+(x-1)))                          +(*(pbuf+(x+1))) \
	-(*(pbuf+ppitch+(x-1)))                   +(*(pbuf+ppitch+(x+1))); \
      *(gybuf+x) = +(*(pbuf+ppitch+(x-1)))+(*(pbuf+ppitch+x))+(*(pbuf+ppitch+(x+1))); \
    }									\
									\
    pbuf = (p_buf)+(h-1)*ppitch;					\
    gxbuf = (type *)((gx_buf)+(h-1)*gxpitch);				\
    gybuf = (type *)((gy_buf)+(h-1)*gypitch);				\
    for (x = 1; x < w-1; x++) {						\
      *(gxbuf+x) = -(*(pbuf-ppitch+(x-1)))                   +(*(pbuf-ppitch+(x+1))) \
	-(*(pbuf+(x-1)))                          +(*(pbuf+(x+1)));	\
      *(gybuf+x) = -(*(pbuf-ppitch+(x-1)))-(*(pbuf-ppitch+x))-(*(pbuf-ppitch+(x+1))); \
    }									\
									\
    pbuf = (p_buf)+ppitch;						\
    gxbuf = (type *)((gx_buf)+gxpitch);					\
    gybuf = (type *)((gy_buf)+gypitch);					\
    for (y = 1; y < h-1; y++) {						\
      *gxbuf =                                              +(*(pbuf-ppitch+1)) \
	+(*(pbuf+1))							\
	+(*(pbuf+ppitch+1));						\
      *gybuf =                             -(*(pbuf-ppitch))-(*(pbuf-ppitch+1)) \
									\
	+(*(pbuf+ppitch))+(*(pbuf+ppitch+1));				\
      pbuf += ppitch;							\
      gxbuf += gxpitch;							\
      gybuf += gypitch;							\
    }									\
									\
    pbuf = (p_buf)+ppitch+(w-1);					\
    gxbuf = (type *)((gx_buf)+gxpitch+(w-1));				\
    gybuf = (type *)((gy_buf)+gypitch+(w-1));				\
    for (y = 1; y < h-1; y++) {						\
      *gxbuf = -(*(pbuf-ppitch-1))					\
	-(*(pbuf-1))							\
	-(*(pbuf+ppitch-1));						\
      *gybuf = -(*(pbuf-ppitch-1))-(*(pbuf-ppitch))			\
									\
	+(*(pbuf+ppitch-1))+(*(pbuf+ppitch));				\
      pbuf += ppitch;							\
      gxbuf += gxpitch;							\
      gybuf += gypitch;							\
    }									\
									\
    *((gx_buf)) =                                +(*((p_buf)+1))	\
      +(*((p_buf)+ppitch+1));						\
    *((gy_buf)) =            +(*((p_buf)+ppitch))+(*((p_buf)+ppitch+1)); \
									\
    *((gx_buf)+(h-1)*gxpitch) =                           +(*((p_buf)+(h-2)*ppitch+1)) \
      +(*((p_buf)+(h-1)*ppitch+1));					\
    *((gy_buf)+(h-1)*gypitch) = -(*((p_buf)+(h-2)*ppitch))-(*((p_buf)+(h-2)*ppitch+1)); \
									\
    *((gx_buf)+(w-1)) = -(*((p_buf)+(w-2)))				\
      -(*((p_buf)+ppitch+(w-2)));					\
    *((gy_buf)+(w-1)) = +(*((p_buf)+ppitch+(w-2)))+(*((p_buf)+ppitch+(w-1))); \
									\
    *((gx_buf)+(h-1)*gxpitch+(w-1)) = -(*((p_buf)+(h-2)*ppitch+(w-2)))	\
      -(*((p_buf)+(h-1)*ppitch+(w-2)));					\
    *((gy_buf)+(h-1)*gypitch+(w-1)) = -(*((p_buf)+(h-2)*ppitch+(w-2)))-(*((p_buf)+(h-2)*ppitch+(w-1))); \
  }

void matrix_prewitt_gradient(matrix_t *grad, bytemap_t *p)
{
  PREWITT_GRADIENT(real_t,
		   matrix_get_buffer(grad),
		   matrix_get_columns(grad),
		   matrix_get_rows(grad),
		   matrix_get_columns(grad),
		   imatrix_get_buffer(grad),
		   matrix_get_columns(grad),
		   matrix_get_rows(grad),
		   matrix_get_columns(grad),
		   bytemap_get_buffer(p),
		   bytemap_get_width(p),
		   bytemap_get_height(p),
		   bytemap_get_pitch(p) / sizeof(*(bytemap_get_buffer(p))));
}

void prewitt_gradient(wordmap_t *gx, wordmap_t *gy, bytemap_t *p)
{
  PREWITT_GRADIENT(int16_t,
		   wordmap_get_buffer(gx),
		   wordmap_get_width(gx),
		   wordmap_get_height(gx),
		   wordmap_get_pitch(gx) / sizeof(int16_t),
		   wordmap_get_buffer(gy),
		   wordmap_get_width(gy),
		   wordmap_get_height(gy),
		   wordmap_get_pitch(gy) / sizeof(int16_t),
		   bytemap_get_buffer(p),
		   bytemap_get_width(p),
		   bytemap_get_height(p),
		   bytemap_get_pitch(p) / sizeof(*(bytemap_get_buffer(p))));
}

void prewitt_edge(bitmap_t *q, bytemap_t *p, real_t tlevel)
{
  int c, r, w, h;
  wordmap_t *gx, *gy;
  int16_t *gxbuf, *gybuf;
  real_t mag;
  uint8_t *qbuf;
  int qpitch, gxpitch, gypitch;

  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bytemap_get_width(p));
  assert(bitmap_get_height(q) == bytemap_get_height(p));

  w = bytemap_get_width(p);
  h = bytemap_get_height(p);

  gx = wordmap_new(w, h);
  gy = wordmap_new(w, h);

  prewitt_gradient(gx, gy, p);

  qbuf = bitmap_get_buffer(q);
  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);

  gxbuf = (int16_t *)wordmap_get_buffer(gx);
  gxpitch = wordmap_get_pitch(gx) / sizeof(*gxbuf);

  gybuf = (int16_t *)wordmap_get_buffer(gy);
  gypitch = wordmap_get_pitch(gy) / sizeof(*gybuf);

  for (r = 0; r < h; r++) {
    for (c = 0; c < w; c++) {
      mag = sqrt(sqr(*(gxbuf + c)) + sqr(*(gybuf + c)));
      if (mag > tlevel) {
	*(qbuf + (c >> 3)) |= (1 << (c % 8));
      } else {
	*(qbuf + (c >> 3)) &= ~(1 << (c % 8));
      }
    }
    qbuf += qpitch;
    gxbuf += gxpitch;
    gybuf += gypitch;
  }
  wordmap_destroy(gx);
  wordmap_destroy(gy);
}
