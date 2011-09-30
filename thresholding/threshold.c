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
#include <assert.h>

#include <common.h>
#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>
#include <matrix.h>

void bytemap_threshold(bitmap_t *bin, bytemap_t *im, int cutup, int cutdown)
{
  int val;
  int i, j, w, h;
  int impitch, binpitch;
  uint8_t *binbuf, *imbuf;

  assert(bin);
  assert(im);
  assert(bitmap_get_width(bin) == bytemap_get_width(im));
  assert(bitmap_get_height(bin) == bytemap_get_height(im));
  assert(cutdown >= cutup);

  w = bytemap_get_width(im);
  h = bytemap_get_height(im);

  impitch = bytemap_get_pitch(im) / sizeof(*imbuf);
  imbuf = bytemap_get_buffer(im);

  binpitch = bitmap_get_pitch(bin) / sizeof(*binbuf);
  binbuf = bitmap_get_buffer(bin);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      val = (int)*(imbuf + j);
      if ((cutup <= val) && (val <= cutdown))
	*(binbuf + (j >> 3)) |= 1 << (j % 8);
      else
	*(binbuf + (j >> 3)) &= ~(1 << (j % 8));
    }
    binbuf += binpitch;
    imbuf += impitch;
  }
}

void bytemap_threshold_on_roi(bitmap_t *bin, bytemap_t *im, int cutup, int cutdown, bitmap_t *roi)
{
  int val;
  int i, j, w, h;
  int binpitch, impitch, roipitch;
  uint8_t *binbuf, *imbuf, *roibuf;

  assert(bin);
  assert(im);
  assert(bitmap_get_width(bin) == bytemap_get_width(im));
  assert(bitmap_get_height(bin) == bytemap_get_height(im));
  assert(bitmap_get_width(roi) == bytemap_get_width(im));
  assert(bitmap_get_height(roi) == bytemap_get_height(im));
  assert(cutdown >= cutup);

  w = bytemap_get_width(im);
  h = bytemap_get_height(im);

  impitch = bytemap_get_pitch(im) / sizeof(*imbuf);
  imbuf = bytemap_get_buffer(im);

  binpitch = bitmap_get_pitch(bin) / sizeof(*binbuf);
  binbuf = bitmap_get_buffer(bin);

  roipitch = bitmap_get_pitch(roi) / sizeof(*roibuf);
  roibuf = bitmap_get_buffer(roi);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (*(roibuf + (j >> 3)) & (1 << (j % 8))) {
	val = (int)*(imbuf + j);
	if ((cutup <= val) && (val <= cutdown))
	  *(binbuf + (j >> 3)) |= 1 << (j % 8);
	else
	  *(binbuf + (j >> 3)) &= ~(1 << (j % 8));
      }
    }
    imbuf += impitch;
    binbuf += binpitch;
    roibuf += roipitch;
  }
}

void bytemap_threshold_on_region(bitmap_t *bin, bytemap_t *im, int cutup, int cutdown, int x, int y, int dx, int dy)
{
  int val;
  int i, j;
  int w, h, binpitch, impitch;
  uint8_t *binbuf, *imbuf;

  assert(bin);
  assert(im);
  assert(bytemap_get_width(im) == bitmap_get_width(bin));
  assert(bytemap_get_height(im) == bitmap_get_height(bin));
  assert(cutdown >= cutup);
  assert(x >= 0 && x < bytemap_get_width(im));
  assert(y >= 0 && y < bytemap_get_height(im));
  assert(dx > 0);
  assert(dy > 0);

  w = min(x + dx, bytemap_get_width(im));
  h = min(y + dy, bytemap_get_height(im));

  impitch = bytemap_get_pitch(im) / sizeof(*imbuf);
  imbuf = bytemap_get_buffer(im) + y * impitch;

  binpitch = bitmap_get_pitch(bin) / sizeof(*binbuf);
  binbuf = bitmap_get_buffer(bin) + y * binpitch;

  for (i = y; i < h; i++) {
    for (j = x; j < w; j++) {
      val = (int)*(imbuf + j);
      if ((cutup <= val) && (val <= cutdown))
	*(binbuf + (j >> 3)) |= 1 << (j % 8);
      else
	*(binbuf + (j >> 3)) &= ~(1 << (j % 8));
    }
    imbuf += impitch;
    binbuf += binpitch;
  }
}

void wordmap_threshold(bitmap_t *bin, wordmap_t *im, int cutup, int cutdown)
{
  int val, binpitch, impitch;
  int i, j, w, h;
  uint8_t *binbuf;
  uint16_t *imbuf;

  assert(bin);
  assert(im);
  assert(wordmap_get_width(im) == bitmap_get_width(bin));
  assert(wordmap_get_height(im) == bitmap_get_height(bin));
  assert(cutdown >= cutup);

  w = wordmap_get_width(im);
  h = wordmap_get_height(im);

  impitch = wordmap_get_pitch(im) / sizeof(*imbuf);
  imbuf = wordmap_get_buffer(im);

  binpitch = bitmap_get_pitch(bin) / sizeof(*binbuf);
  binbuf = bitmap_get_buffer(bin);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      val = (int)*(imbuf + j);
      if ((cutup <= val) && (val <= cutdown))
	*(binbuf + (j >> 3)) |= 1 << (j % 8);
      else
	*(binbuf + (j >> 3)) &= ~(1 << (j % 8));
    }
    imbuf += impitch;
    binbuf += binpitch;
  }
}

void wordmap_threshold_on_roi(bitmap_t *bin, wordmap_t *im, int cutup, int cutdown, bitmap_t *roi)
{
  int val, binpitch, roipitch, impitch;
  int i, j, w, h;
  uint8_t *binbuf, *roibuf;
  uint16_t *imbuf;

  assert(bin);
  assert(im);
  assert(wordmap_get_width(im) == bitmap_get_width(bin));
  assert(wordmap_get_height(im) == bitmap_get_height(bin));
  assert(wordmap_get_width(im) == bitmap_get_width(roi));
  assert(wordmap_get_height(im) == bitmap_get_height(roi));
  assert(cutdown >= cutup);

  w = wordmap_get_width(im);
  h = wordmap_get_height(im);

  impitch = wordmap_get_pitch(im) / sizeof(*imbuf);
  imbuf = wordmap_get_buffer(im);

  binpitch = bitmap_get_pitch(bin) / sizeof(*binbuf);
  binbuf = bitmap_get_buffer(bin);

  roipitch = bitmap_get_pitch(roi) / sizeof(*roibuf);
  roibuf = bitmap_get_buffer(roi);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (*(roibuf + (j >> 3)) & (1 << (j % 8))) {
	val = (int)*(imbuf + j);
	if ((cutup <= val) && (val <= cutdown))
	  *(binbuf + (j >> 3)) |= 1 << (j % 8);
	else
	  *(binbuf + (j >> 3)) &= ~(1 << (j % 8));
      }
    }
    imbuf += impitch;
    binbuf += binpitch;
    roibuf += roipitch;
  }
}

void wordmap_threshold_on_region(bitmap_t *bin, wordmap_t *im, int cutup, int cutdown, int x, int y, int dx, int dy)
{
  int val, binpitch, impitch;
  int i, j;
  int w, h;
  uint8_t *binbuf;
  uint16_t *imbuf;

  assert(bin);
  assert(im);
  assert(wordmap_get_width(im) == bitmap_get_width(bin));
  assert(wordmap_get_height(im) == bitmap_get_height(bin));
  assert(cutdown >= cutup);
  assert(x >= 0 && x < wordmap_get_width(im));
  assert(y >= 0 && y < wordmap_get_height(im));
  assert(dx > 0);
  assert(dy > 0);

  w = min(x + dx, wordmap_get_width(im));
  h = min(y + dy, wordmap_get_height(im));

  impitch = wordmap_get_pitch(im) / sizeof(*imbuf);
  imbuf = wordmap_get_buffer(im) + y * impitch;

  binpitch = bitmap_get_pitch(bin) / sizeof(*binbuf);
  binbuf = bitmap_get_buffer(bin) + y * binpitch;

  for (i = y; i < h; i++) {
    for (j = x; j < w; j++) {
      val = (int)*(imbuf + j);
      if ((cutup <= val) && (val <= cutdown))
	*(binbuf + (j >> 3)) |= 1 << (j % 8);
      else
	*(binbuf + (j >> 3)) &= ~(1 << (j % 8));
    }
    imbuf += impitch;
    binbuf += binpitch;
  }
}

void matrix_threshold(bitmap_t *bin, matrix_t *im, real_t cutup, real_t cutdown)
{
  int x, y, w, h, binpitch;
  uint8_t *binbuf;
  real_t *imbuf, val;

  assert(bin);
  assert(im);
  assert(cutup < cutdown);
  assert(bitmap_get_width(bin) == matrix_get_columns(im));
  assert(bitmap_get_height(bin) == matrix_get_rows(im));

  w = matrix_get_columns(im);
  h = matrix_get_rows(im);

  imbuf = matrix_get_buffer(im);

  binpitch = bitmap_get_pitch(bin) / sizeof(*binbuf);
  binbuf = bitmap_get_buffer(bin);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      val = *(imbuf + x);
      if ((val >= cutup) && (val <= cutdown))
	*(binbuf + (x >> 3)) |= 1 << (x % 8);
      else
	*(binbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    imbuf += w;
    binbuf += binpitch;
  }
}

void matrixp_threshold_on_roi(bitmap_t *bin, matrix_t *im, real_t cutup, real_t cutdown, bitmap_t *roi)
{
  int binpitch, roipitch;
  int i, j, w, h;
  uint8_t *binbuf, *roibuf;
  real_t *imbuf, val;

  assert(bin);
  assert(im);
  assert(matrix_get_columns(im) == bitmap_get_width(bin));
  assert(matrix_get_rows(im) == bitmap_get_height(bin));
  assert(matrix_get_columns(im) == bitmap_get_width(roi));
  assert(matrix_get_rows(im) == bitmap_get_height(roi));
  assert(cutdown >= cutup);

  w = matrix_get_columns(im);
  h = matrix_get_rows(im);

  imbuf = matrix_get_buffer(im);

  binpitch = bitmap_get_pitch(bin) / sizeof(*binbuf);
  binbuf = bitmap_get_buffer(bin);

  roipitch = bitmap_get_pitch(roi) / sizeof(*roibuf);
  roibuf = bitmap_get_buffer(roi);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (*(roibuf + (j >> 3)) & (1 << (j % 8))) {
	val = *(imbuf + j);
	if ((cutup <= val) && (val <= cutdown))
	  *(binbuf + (j >> 3)) |= 1 << (j % 8);
	else
	  *(binbuf + (j >> 3)) &= ~(1 << (j % 8));
      }
    }
    imbuf += w;
    binbuf += binpitch;
    roibuf += roipitch;
  }
}

void matrix_threshold_on_region(bitmap_t *bin, matrix_t *im, real_t cutup, real_t cutdown, int x, int y, int dx, int dy)
{
  int binpitch;
  int i, j;
  int w, h;
  uint8_t *binbuf;
  real_t *imbuf, val;

  assert(bin);
  assert(im);
  assert(matrix_get_columns(im) == bitmap_get_width(bin));
  assert(matrix_get_rows(im) == bitmap_get_height(bin));
  assert(cutdown >= cutup);
  assert(x >= 0 && x < matrix_get_columns(im));
  assert(y >= 0 && y < matrix_get_rows(im));
  assert(dx > 0);
  assert(dy > 0);

  w = min(x + dx, matrix_get_columns(im));
  h = min(y + dy, matrix_get_rows(im));

  imbuf = matrix_get_buffer(im) + y * w;

  binpitch = bitmap_get_pitch(bin) / sizeof(*binbuf);
  binbuf = bitmap_get_buffer(bin) + y * binpitch;

  for (i = y; i < h; i++) {
    for (j = x; j < w; j++) {
      val = *(imbuf + j);
      if ((cutup <= val) && (val <= cutdown))
	*(binbuf + (j >> 3)) |= 1 << (j % 8);
      else
	*(binbuf + (j >> 3)) &= ~(1 << (j % 8));
    }
    imbuf += w;
    binbuf += binpitch;
  }
}
