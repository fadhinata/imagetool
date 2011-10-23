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
#include <math.h>
#include <assert.h>

#include <common.h>
#include <smoothing/mean_smooth.h>

void mean_smooth(bytemap_t *q, bytemap_t *p, int bsize)
{
  int c, r, i, j, w, h, qpitch, ppitch, offset;
  uint8_t *qbuf;
  uint8_t *pbuf;
  real_t sum;

  assert(p);
  assert(q);
  assert(bytemap_get_width(p) == bytemap_get_width(q));
  assert(bytemap_get_height(p) == bytemap_get_height(q));
	
  assert(bsize > 0);
  assert(bsize < bytemap_get_width(p));
  assert(bsize < bytemap_get_height(p));

  w = bytemap_get_width(p);
  h = bytemap_get_height(p);
  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);
  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);

  offset = bsize >> 1;

  // main body
  qbuf = bytemap_get_buffer(q) + offset * qpitch;
  for (r = offset; r < h - (bsize - offset); r++) {
    for (c = offset; c < w - (bsize - offset); c++) {
      sum = 0.0;
      pbuf = bytemap_get_buffer(p) + (r - offset) * ppitch;
      for (i = -offset; i < bsize - offset; i++) {
	for (j = -offset; j < bsize - offset; j++) {
	  sum += *(pbuf + (c + j));
	}
	pbuf += ppitch;
      }
      *(qbuf + c) = (uint8_t)(sum / sqr((real_t)bsize));
    }
    qbuf += qpitch;
  }

  // for left top block
  qbuf = bytemap_get_buffer(q);
  for (r = 0; r < offset; r++) {
    for (c = 0; c < offset; c++) {
      sum = 0.0;
      for (i = -offset; i < bsize - offset; i++) {
	if ((r + i) < 0) continue;
        pbuf = bytemap_get_buffer(p) + (r + i) * ppitch;
        for (j = -offset; j < bsize - offset; j++) {
          if ((c + j) >= 0) sum += *(pbuf + (c + j));
        }
      }
      *(qbuf + c) = (uint8_t)(sum / sqr((real_t)bsize));
    }
    qbuf += qpitch;
  }

  //for upper band
  qbuf = bytemap_get_buffer(q);
  for (r = 0; r < offset; r++) {
    for (c = offset; c < w - (bsize - offset); c++) {
      sum = 0.0;
      for (i = -offset; i < bsize - offset; i++) {
	if ((r + i) < 0) continue;
	pbuf = bytemap_get_buffer(p) + (r + i) * ppitch;
	for (j = -offset; j < bsize - offset; j++)
	  sum += *(pbuf + (c + j));
      }
      *(qbuf + c) = (uint8_t)(sum / sqr((real_t)bsize));
    }
    qbuf += qpitch;
  }

  // for right top block
  qbuf = bytemap_get_buffer(q);
  for (r = 0; r < offset; r++) {
    for (c = w - (bsize - offset); c < w; c++) {
      sum = 0.0;
      for (i = -offset; i < bsize - offset; i++) {
	if ((r + i) < 0) continue;
	pbuf = bytemap_get_buffer(p) + (r + i) * ppitch;
	for (j = -offset; j < bsize - offset; j++) {
          if ((c + j) >= w) break;
	  sum += *(pbuf + (c + j));
        }
      }
      *(qbuf + c) = (uint8_t)(sum / sqr((real_t)bsize));
    }
    qbuf += qpitch;
  }

  // for right band
  qbuf = bytemap_get_buffer(q) + offset * qpitch;
  for (r = offset; r < h - (bsize - offset); r++) {
    for (c = w - (bsize - offset); c < w; c++) {
      sum = 0.0;
      pbuf = bytemap_get_buffer(p) + (r - offset) * ppitch;
      for (i = -offset; i < bsize - offset; i++) {
	for (j = -offset; j < bsize - offset; j++) {
	  if ((c + j) >= w) break;
	  sum += *(pbuf + (c + j));
	}
	pbuf += ppitch;
      }
      *(qbuf + c) = (uint8_t)(sum / sqr((real_t)bsize));
    }
    qbuf += qpitch;
  }

  // for right bottom block
  qbuf = bytemap_get_buffer(q) + (h - (bsize - offset)) * qpitch;
  for (r = h - (bsize - offset); r < h; r++) {
    for (c = w - (bsize - offset); c < w; c++) {
      sum = 0.0;
      pbuf = bytemap_get_buffer(p) + (r - offset) * ppitch;
      for (i = -offset; i < bsize - offset; i++) {
	if ((r + i) >= h) break;
	for (j = -offset; j < bsize - offset; j++) {
	  if ((c + j) >= w) break;
          sum += *(pbuf + (c + j));
	}
        pbuf += ppitch;
      }
      *(qbuf + c) = (uint8_t)(sum / sqr((real_t)bsize));
    }
    qbuf += qpitch;
  }

  // for lower band
  qbuf = bytemap_get_buffer(q) + (h - (bsize - offset)) * qpitch;
  for (r = h - (bsize - offset); r < h; r++) {
    for (c = offset; c < w - (bsize - offset); c++) {
      sum = 0.0;
      pbuf = bytemap_get_buffer(p) + (r - offset) * ppitch;
      for (i = -offset; i < bsize - offset; i++) {
	if ((r + i) >= h) break;
	for (j = -offset; j < bsize - offset; j++) {
	  sum += *(pbuf + (c + j));
	}
        pbuf += ppitch;
      }
      *(qbuf + c) = (uint8_t)(sum / sqr((real_t)bsize));
    }
    qbuf += qpitch;
  }

  // for left bottom block
  qbuf = bytemap_get_buffer(q) + (h - (bsize - offset)) * qpitch;
  for (r = h - (bsize - offset); r < h; r++) {
    for (c = 0; c < offset; c++) {
      sum = 0.0;
      pbuf = bytemap_get_buffer(p) + (r - offset) * ppitch;
      for (i = -offset; i < bsize - offset; i++) {
	if ((r + i) >= h) break;
	for (j = -offset; j < bsize - offset; j++) {
	  if ((c + j) < 0) continue;
	  sum += *(pbuf + (c + j));
	}
        pbuf += ppitch;
      }
      *(qbuf + c) = (uint8_t)(sum / sqr((real_t)bsize));
    }
    qbuf += qpitch;
  }

  // for left band
  qbuf = bytemap_get_buffer(q) + offset * qpitch;
  for (r = offset; r < h - (bsize - offset); r++) {
    for (c = 0; c < offset; c++) {
      sum = 0.0;
      pbuf = bytemap_get_buffer(p) + (r - offset) * ppitch;
      for (i = -offset; i < bsize - offset; i++) {
	for (j = -offset; j < bsize - offset; j++) {
	  if ((c + j) < 0) continue;
	  sum += *(pbuf + (c + j));
	}
	pbuf += ppitch;
      }
      *(qbuf + c) = (uint8_t)(sum / sqr((real_t)bsize));
    }
    qbuf += qpitch;
  }
}
