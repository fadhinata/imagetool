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
#include <math.h>
#include <limits.h>
#include <common.h>
#include <gamma.h>
#include <assert.h>

static int *bytemap_gamma_transfer_table(int *conv, real_t r)
{
  int i;
  real_t maxval;

  assert(conv);

  maxval = pow(UINT8_MAX, r);
  for (i = 0; i <= UINT8_MAX; i++) {
    conv[i] = (int)round(UINT8_MAX * pow(i, r) / maxval);
    if (conv[i] < 0) conv[i] = 0;
    if (conv[i] > UINT8_MAX) conv[i] = UINT8_MAX;
  }

  return conv;
}

int bytemap_gamma_correct(bytemap_t *q, bytemap_t *p, real_t gamma)
{
  int c, r, w, h;
  int convert[UINT8_MAX+1];
  uint8_t *pbuf, *qbuf;
  int ppitch, qpitch;

  assert(q);
  assert(p);
  assert(bytemap_get_width(p) == bytemap_get_width(q));
  assert(bytemap_get_height(p) == bytemap_get_height(q));

  w = bytemap_get_width(p);
  h = bytemap_get_height(p);

  bytemap_gamma_transfer_table((int *)convert, gamma);

  pbuf = bytemap_get_buffer(p);
  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);

  qbuf = bytemap_get_buffer(q);
  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);

  for (r = 0; r < h; r++) {
    for (c = 0; c < w; c++) {
      *(qbuf+c) = convert[*(pbuf+c)];
    }
    pbuf += ppitch;
    qbuf += qpitch;
  }

  return 0;
}

static int *wordmap_gamma_transfer_table(int *conv, real_t r)
{
  int i;
  real_t maxval;

  assert(conv);

  maxval = pow(UINT16_MAX, r);
  for (i = 0; i <= UINT16_MAX; i++) {
    conv[i] = (int)round(UINT16_MAX * pow(i, r) / maxval);
    //if (conv[i] < 0) conv[i] = 0;
    //if (conv[i] > UINT16_MAX) conv[i] = UINT16_MAX;
  }

  return conv;
}

int wordmap_gamma_correct(wordmap_t *q, wordmap_t *p, real_t gamma)
{
  int c, r, w, h;
  int convert[UINT16_MAX+1];
  uint16_t *pbuf, *qbuf;
  int ppitch, qpitch;

  assert(q);
  assert(p);
  assert(wordmap_get_width(p) == wordmap_get_width(q));
  assert(wordmap_get_height(p) == wordmap_get_height(q));

  w = wordmap_get_width(p);
  h = wordmap_get_height(p);

  wordmap_gamma_transfer_table((int *)convert, gamma);

  pbuf = wordmap_get_buffer(p);
  ppitch = wordmap_get_pitch(p) / sizeof(*pbuf);

  qbuf = wordmap_get_buffer(q);
  qpitch = wordmap_get_pitch(q) / sizeof(*qbuf);

  for (r = 0; r < h; r++) {
    for (c = 0; c < w; c++) {
      *(qbuf + c) = convert[*(pbuf + c)];
    }
    pbuf += ppitch;
    qbuf += qpitch;
  }

  return 0;
}
