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
#include <string.h>
#include <assert.h>

#include <common.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <pixmap/dwordmap.h>
#include <pixmap/floatmap.h>
#include <linear_algebra/matrix.h>
#include <buffering/dlink.h>
#include <linear_algebra/vector.h>
#include <geometry/point_list.h>

#define MATRIX_TO_BITMAP(bitmap, matbuf, columns, rows, pickup, pickdown) { \
    int i, j;								\
    real_t val;								\
    uint8_t *bbuf;                                                      \
    real_t *mbuf;							\
    int bpitch;								\
    assert(bitmap);							\
    assert(matbuf);							\
    assert(bitmap_get_width(bitmap) == columns);			\
    assert(bitmap_get_height(bitmap) == rows);				\
    bbuf = bitmap_get_buffer(bitmap);					\
    bpitch = bitmap_get_pitch(bitmap) / sizeof(*bbuf);			\
    mbuf = matbuf;							\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	val = *(mbuf + j);						\
	if (val >= pickup && val <= pickdown)				\
	  *(bbuf + (j >> 3)) |= 1 << (j % 8);				\
	else								\
	  *(bbuf + (j >> 3)) &= ~(1 << (j % 8));			\
      }									\
      mbuf += columns;							\
      bbuf += bpitch;							\
    }									\
  }

void matrix2bitmap(bitmap_t *bitmap, matrix_t *matrix, real_t pickup, real_t pickdown)
{
  MATRIX_TO_BITMAP(bitmap, matrix->real, matrix->columns, matrix->rows, pickup, pickdown);
}

void imatrix2bitmap(bitmap_t *bitmap, matrix_t *matrix, real_t pickup, real_t pickdown)
{
  MATRIX_TO_BITMAP(bitmap, matrix->imaginary, matrix->columns, matrix->rows, pickup, pickdown);
}

void cmatrix2bitmap(bitmap_t *bitmap, matrix_t *matrix, complex_t pickup, complex_t pickdown)
{
  int i, j, rows, columns;
  uint8_t *bbuf;
  real_t *rbuf, *ibuf;
  complex_t val;
  int bpitch;

  assert(bitmap);
  assert(matrix);
  assert(imatrix_get_buffer(matrix));
  assert(bitmap_get_width(bitmap) == matrix_get_columns(matrix));
  assert(bitmap_get_height(bitmap) == matrix_get_rows(matrix));

  rbuf = matrix_get_buffer(matrix);
  ibuf = imatrix_get_buffer(matrix);
  bbuf = bitmap_get_buffer(bitmap);
  bpitch = bitmap_get_pitch(bitmap) / sizeof(*bbuf);

  columns = matrix_get_columns(matrix);
  rows = matrix_get_rows(matrix);
  for (i = 0; i < rows; i++) {
    for (j = 0; j < columns; j++) {
      val.real = *(rbuf + j);
      val.imag = *(ibuf + j);
      if (complex_compare_abs(&val, &pickup) >= 0 &&
	  complex_compare_abs(&val, &pickdown) <= 0) {
	*(bbuf + (j >> 3)) |= 1 << (j % 8);
      } else {
	*(bbuf + (j >> 3)) &= ~(1 << (j % 8));
      }
    }
    rbuf += columns;
    ibuf += columns;
    bbuf += bpitch;
  }
}

#define MATRIX_TO_PIXMAP(type, umaxval, pixmap, matbuf, columns, rows) { \
    int i, j;								\
    type *bbuf;								\
    real_t *mbuf;							\
    real_t val;								\
    int bpitch;								\
    assert(pixmap);							\
    assert(matbuf);							\
    assert((pixmap)->header.width == columns);				\
    assert((pixmap)->header.height == rows);				\
    bbuf = (pixmap)->buffer;						\
    bpitch = (pixmap)->header.pitch / sizeof(*bbuf);			\
    mbuf = matbuf;							\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	val = *(mbuf+j);						\
	if (val < 0.0) *(bbuf+j) = 0;					\
	else if (val > umaxval) *(bbuf+j) = umaxval;			\
	else *(bbuf+j) = (type)val;					\
      }									\
      mbuf += columns;							\
      bbuf += bpitch;							\
    }									\
  }

void matrix2bytemap(bytemap_t *bytemap, matrix_t *matrix)
{
  MATRIX_TO_PIXMAP(uint8_t, UINT8_MAX, bytemap, matrix->real, matrix->columns, matrix->rows);
}

void imatrix2bytemap(bytemap_t *bytemap, matrix_t *matrix)
{
  MATRIX_TO_PIXMAP(uint8_t, UINT8_MAX, bytemap, matrix->imaginary, matrix->columns, matrix->rows);
}

void matrix2wordmap(wordmap_t *wordmap, matrix_t *matrix)
{
  MATRIX_TO_PIXMAP(uint16_t, UINT16_MAX, wordmap, matrix->real, matrix->columns, matrix->rows);
}

void imatrix2wordmap(wordmap_t *wordmap, matrix_t *matrix)
{
  MATRIX_TO_PIXMAP(uint16_t, UINT16_MAX, wordmap, matrix->imaginary, matrix->columns, matrix->rows);
}

void matrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix)
{
  MATRIX_TO_PIXMAP(uint32_t, UINT32_MAX, dwordmap, matrix->real, matrix->columns, matrix->rows);
}

void imatrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix)
{
  MATRIX_TO_PIXMAP(uint32_t, UINT32_MAX, dwordmap, matrix->imaginary, matrix->columns, matrix->rows);
}

#define CMATRIX_TO_PIXMAP(type, umaxval, pixmap, matrix) {		\
    int i, j, rows, columns;						\
    type *bbuf;								\
    int bpitch;								\
    real_t *rbuf, *ibuf;						\
    real_t val;								\
    assert(pixmap);							\
    assert(matrix);							\
    assert((matrix)->imaginary);					\
    assert((pixmap)->header.width == (matrix)->columns);		\
    assert((pixmap)->header.height == (matrix)->rows);			\
    bbuf = (pixmap)->buffer;						\
    bpitch = (pixmap)->header.pitch / sizeof(*bbuf);			\
    rbuf = (matrix)->real;						\
    ibuf = (matrix)->imaginary;						\
    columns = (matrix)->columns;					\
    rows = (matrix)->rows;						\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	val = sqrt(sqr(*(rbuf+j))+sqr(*(ibuf+j)));			\
	if (val < 0.0) *(bbuf+j) = 0;					\
	else if (val > umaxval) *(bbuf+j) = umaxval;			\
	else *(bbuf+j) = (type)val;					\
      }									\
      rbuf += columns;							\
      ibuf += columns;							\
      bbuf += bpitch;							\
    }									\
  }

void cmatrix2bytemap(bytemap_t *bytemap, matrix_t *matrix)
{
  CMATRIX_TO_PIXMAP(uint8_t, UINT8_MAX, bytemap, matrix);
}

void cmatrix2wordmap(wordmap_t *wordmap, matrix_t *matrix)
{
  CMATRIX_TO_PIXMAP(uint16_t, UINT16_MAX, wordmap, matrix);
}

void cmatrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix)
{
  CMATRIX_TO_PIXMAP(uint32_t, UINT32_MAX, dwordmap, matrix);
}

#define DYNAMIC_MATRIX_TO_PIXMAP(type, umaxval, pixmap, matbuf, columns, rows) { \
    int i, j;								\
    type *bbuf;								\
    real_t *mbuf;							\
    real_t vmax, vmin, val;						\
    int bpitch;								\
    assert(pixmap);							\
    assert(matbuf);							\
    assert((pixmap)->header.width == columns);				\
    assert((pixmap)->header.height == rows);				\
    mbuf = matbuf;							\
    vmax = vmin = *mbuf;						\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	val = *(mbuf+j);						\
	if (vmax < val) vmax = val;					\
	else if (vmin > val) vmin = val;				\
      }									\
      mbuf += columns;							\
    }									\
    mbuf = matbuf;							\
    bbuf = (pixmap)->buffer;						\
    bpitch = (pixmap)->header.pitch / sizeof(*bbuf);			\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	val = *(mbuf+j);						\
	val = (val-vmin)*umaxval/(vmax-vmin);				\
	*(bbuf+j) = (type)val;						\
      }									\
      mbuf += columns;							\
      bbuf += bpitch;							\
    }									\
  }

void dynamic_matrix2bytemap(bytemap_t *bytemap, matrix_t *matrix)
{
  DYNAMIC_MATRIX_TO_PIXMAP(uint8_t, UINT8_MAX, bytemap, matrix->real, matrix->columns, matrix->rows);
}

void dynamic_imatrix2bytemap(bytemap_t *bytemap, matrix_t *matrix)
{
  DYNAMIC_MATRIX_TO_PIXMAP(uint8_t, UINT8_MAX, bytemap, matrix->imaginary, matrix->columns, matrix->rows);
}

void dynamic_matrix2wordmap(wordmap_t *wordmap, matrix_t *matrix)
{
  DYNAMIC_MATRIX_TO_PIXMAP(uint16_t, UINT16_MAX, wordmap, matrix->real, matrix->columns, matrix->rows);
}

void dynamic_imatrix2wordmap(wordmap_t *wordmap, matrix_t *matrix)
{
  DYNAMIC_MATRIX_TO_PIXMAP(uint16_t, UINT16_MAX, wordmap, matrix->imaginary, matrix->columns, matrix->rows);
}

void dynamic_matrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix)
{
  DYNAMIC_MATRIX_TO_PIXMAP(uint32_t, UINT32_MAX, dwordmap, matrix->real, matrix->columns, matrix->rows);
}

void dynamic_imatrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix)
{
  DYNAMIC_MATRIX_TO_PIXMAP(uint32_t, UINT32_MAX, dwordmap, matrix->imaginary, matrix->columns, matrix->rows);
}

#define DYNAMIC_CMATRIX_TO_PIXMAP(type, umaxval, pixmap, matrix) {	\
    int i, j, rows, columns;						\
    type *bbuf;								\
    real_t *rbuf, *ibuf;						\
    real_t vmin, vmax, val;						\
    int bpitch;								\
    assert(pixmap);							\
    assert(matrix);							\
    assert((matrix)->imaginary);					\
    assert((pixmap)->header.width == (matrix)->columns);		\
    assert((pixmap)->header.height == (matrix)->rows);			\
    rbuf = (matrix)->real, ibuf = (matrix)->imaginary;			\
    columns = (matrix)->columns;					\
    rows = (matrix)->rows;						\
    vmax = vmin = sqrt(sqr(*(rbuf))+sqr(*(ibuf)));			\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	val = sqrt(sqr(*(rbuf+j))+sqr(*(ibuf+j)));			\
	if (vmax < val) vmax = val;					\
	else if (vmin > val) vmin = val;				\
      }									\
      rbuf += columns;							\
      ibuf += columns;							\
    }									\
    bbuf = (pixmap)->buffer;						\
    bpitch = (pixmap)->header.pitch / sizeof(*bbuf);			\
    rbuf = (matrix)->real;						\
    ibuf = (matrix)->imaginary;						\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++) {					\
	val = sqrt(sqr(*(rbuf+j))+sqr(*(ibuf+j)));			\
	val = (val-vmin)*umaxval/(vmax-vmin);				\
	*(bbuf+j) = (type)val;						\
      }									\
      rbuf += columns;							\
      ibuf += columns;							\
      bbuf += bpitch;							\
    }									\
  }

void dynamic_cmatrix2bytemap(bytemap_t *bytemap, matrix_t *matrix)
{
  DYNAMIC_CMATRIX_TO_PIXMAP(uint8_t, UINT8_MAX, bytemap, matrix);
}

void dynamic_cmatrix2wordmap(wordmap_t *wordmap, matrix_t *matrix)
{
  DYNAMIC_CMATRIX_TO_PIXMAP(uint16_t, UINT16_MAX, wordmap, matrix);
}

void dynamic_cmatrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix)
{
  DYNAMIC_CMATRIX_TO_PIXMAP(uint32_t, UINT32_MAX, dwordmap, matrix);
}

void bitmap2matrix(matrix_t *matrix, bitmap_t *bitmap)
{
  int x, y, w, h; 
  real_t *mbuf;
  uint8_t *bbuf;
  int bpitch;

  assert(matrix);
  assert(bitmap);
  assert(matrix_get_columns(matrix) == bitmap_get_width(bitmap));
  assert(matrix_get_rows(matrix) == bitmap_get_height(bitmap));

  w = bitmap_get_width(bitmap);
  h = bitmap_get_height(bitmap);

  bbuf = bitmap_get_buffer(bitmap);
  bpitch = bitmap_get_pitch(bitmap) / sizeof(*bbuf);
  mbuf = matrix_get_buffer(matrix);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      *(mbuf + x) = ((*(bbuf + (x>>3))) >> (x%8)) & 1;
    }
    mbuf += w;
    bbuf += bpitch;
  }
}

#define PIXMAP_TO_MATRIX(type, matbuf, columns, rows, pixmap) {		\
    int i, j;								\
    real_t *mbuf;							\
    type *pbuf;								\
    int ppitch;								\
    assert(pixmap);							\
    assert(matbuf);							\
    assert((pixmap)->header.width == columns);				\
    assert((pixmap)->header.height == rows);				\
    mbuf = matbuf;							\
    pbuf = (pixmap)->buffer;						\
    ppitch = (pixmap)->header.pitch / sizeof(*pbuf);			\
    for (i = 0; i < rows; i++) {					\
      for (j = 0; j < columns; j++)					\
	*(mbuf+j) = *(pbuf+j);						\
      mbuf += columns;							\
      pbuf += ppitch;							\
    }									\
  }

void bytemap2matrix(matrix_t *matrix, bytemap_t *bytemap)
{
  PIXMAP_TO_MATRIX(uint8_t, matrix->real, matrix->columns, matrix->rows, bytemap);
}

void bytemap2imatrix(matrix_t *matrix, bytemap_t *bytemap)
{
  PIXMAP_TO_MATRIX(uint8_t, matrix->imaginary, matrix->columns, matrix->rows, bytemap);
}

void wordmap2matrix(matrix_t *matrix, wordmap_t *wordmap)
{
  PIXMAP_TO_MATRIX(uint16_t, matrix->real, matrix->columns, matrix->rows, wordmap);
}

void wordmap2imatrix(matrix_t *matrix, wordmap_t *wordmap)
{
  PIXMAP_TO_MATRIX(uint16_t, matrix->imaginary, matrix->columns, matrix->rows, wordmap);
}

void dwordmap2matrix(matrix_t *matrix, dwordmap_t *dwordmap)
{
  PIXMAP_TO_MATRIX(uint32_t, matrix->real, matrix->columns, matrix->rows, dwordmap);
}

void dwordmap2imatrix(matrix_t *matrix, dwordmap_t *dwordmap)
{
  PIXMAP_TO_MATRIX(uint32_t, matrix->imaginary, matrix->columns, matrix->rows, dwordmap);
}

void matrix2floatmap(floatmap_t *floatmap, matrix_t *matrix)
{
  int i, j;
  real_t *mbuf;
  float *fbuf;
  int fpitch, rows, columns;

  assert(floatmap);
  assert(matrix);
  assert(floatmap_get_width(floatmap) == matrix_get_columns(matrix));
  assert(floatmap_get_height(floatmap) == matrix_get_rows(matrix));

  mbuf = matrix_get_buffer(matrix);
  columns = matrix_get_columns(matrix);
  rows = matrix_get_rows(matrix);

  fbuf = floatmap_get_buffer(floatmap);
  fpitch = floatmap_get_pitch(floatmap) / sizeof(*fbuf);

  for (i = 0; i < rows; i++) {
    for (j = 0; j < columns; j++)
      *(fbuf + j) = *(mbuf + j);
    fbuf += fpitch;
    mbuf += columns;
  }
}

void imatrix2floatmap(floatmap_t *floatmap, matrix_t *matrix)
{
  int i, j;
  real_t *mbuf;
  float *fbuf;
  int fpitch, rows, columns;

  assert(floatmap);
  assert(matrix);
  assert(matrix->imaginary);
  assert(floatmap_get_width(floatmap) == matrix_get_columns(matrix));
  assert(floatmap_get_height(floatmap) == matrix_get_rows(matrix));

  mbuf = imatrix_get_buffer(matrix);
  columns = matrix_get_columns(matrix);
  rows = matrix_get_rows(matrix);

  fbuf = floatmap_get_buffer(floatmap);
  fpitch = floatmap_get_pitch(floatmap) / sizeof(*fbuf);

  for (i = 0; i < rows; i++) {
    for (j = 0; j < columns; j++)
      *(fbuf + j) = *(mbuf + j);
    fbuf += fpitch;
    mbuf += columns;
  }
}

void cmatrix2floatmap(floatmap_t *floatmap, matrix_t *matrix)
{
  int i, j;
  real_t *rbuf, *ibuf;
  float *fbuf;
  int columns, rows, fpitch;

  assert(floatmap);
  assert(matrix);
  assert(matrix->imaginary);
  assert(floatmap_get_width(floatmap) == matrix_get_columns(matrix));
  assert(floatmap_get_height(floatmap) == matrix_get_rows(matrix));

  rbuf = matrix_get_buffer(matrix);
  ibuf = imatrix_get_buffer(matrix);
  columns = matrix_get_columns(matrix);
  rows = matrix_get_rows(matrix);

  fbuf = floatmap_get_buffer(floatmap);
  fpitch = floatmap_get_pitch(floatmap) / sizeof(*fbuf);

  for (i = 0; i < rows; i++) {
    for (j = 0; j < columns; j++)
      *(fbuf + j) = sqrt(sqr(*(rbuf + j)) + sqr(*(ibuf + j)));
    fbuf += fpitch;
    rbuf += columns;
    ibuf += columns;
  }
}

void floatmap2matrix(matrix_t *matrix, floatmap_t *floatmap)
{
  PIXMAP_TO_MATRIX(float, matrix->real, matrix->columns, matrix->rows, floatmap);
}

void floatmap2imatrix(matrix_t *matrix, floatmap_t *floatmap)
{
  PIXMAP_TO_MATRIX(float, matrix->imaginary, matrix->columns, matrix->rows, floatmap);
}

#define MATRIX_COPY_PIXMAP(type, matrix, c, r, pixmap, x, y, w, h) {	\
    int i, j;								\
    real_t *mbuf;							\
    type *bbuf;								\
    int columns, bpitch;						\
    assert(matrix);							\
    assert((c >= 0) && (c < (matrix)->columns));			\
    assert((r >= 0) && (r < (matrix)->rows));				\
    assert(pixmap);							\
    assert((x >= 0) && (x < (pixmap)->header.width));			\
    assert((y >= 0) && (y < (pixmap)->header.height));			\
    assert((w > 0) && ((x+w) <= (pixmap)->header.width));		\
    assert((h > 0) && ((y+h) <= (pixmap)->header.height));		\
    w = min(c + w, (matrix)->columns) - c;				\
    h = min(r + h, (matrix)->rows) - r;					\
    mbuf = (matrix)->real + r * (matrix)->columns;			\
    columns = (matrix)->columns;					\
    bbuf = (pixmap)->buffer + (y) * (pixmap)->header.pitch / sizeof(type); \
    bpitch = (pixmap)->header.pitch / sizeof(*bbuf);			\
    for (i = 0; i < h; i++) {						\
      for (j = 0; j < w; j++) {						\
	*(mbuf + (c + j)) = (real_t)*(bbuf + (x + j));			\
      }									\
      mbuf += columns;							\
      bbuf += bpitch;							\
    }									\
  }

void matrix_copy_bytemap(matrix_t *matrix, int c, int r, bytemap_t *bytemap, int x, int y, int w, int h)
{
  MATRIX_COPY_PIXMAP(uint8_t, matrix, c, r, bytemap, x, y, w, h);
}

void matrix_copy_wordmap(matrix_t *matrix, int c, int r, wordmap_t *wordmap, int x, int y, int w, int h)
{
  MATRIX_COPY_PIXMAP(uint16_t, matrix, c, r, wordmap, x, y, w, h);
}

void matrix_copy_dwordmap(matrix_t *matrix, int c, int r, dwordmap_t *dwordmap, int x, int y, int w, int h)
{
  MATRIX_COPY_PIXMAP(uint32_t, matrix, c, r, dwordmap, x, y, w, h);
}

void bitmap2bytemap(bytemap_t *q, bitmap_t *p)
{
  int i, j, w, h;
  uint8_t *qbuf, *pbuf;
  int qpitch, ppitch;

  assert(q);
  assert(p);
  assert(bytemap_get_width(q) == bitmap_get_width(p));
  assert(bytemap_get_height(q) == bitmap_get_height(p));

  w = bytemap_get_width(q);
  h = bytemap_get_height(q);

  qbuf = bytemap_get_buffer(q);
  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);

  pbuf = bitmap_get_buffer(p);
  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      *(qbuf + j) = ((*(pbuf + (j >> 3))) >> (j % 8)) & 1;
    }
    qbuf += qpitch;
    pbuf += ppitch;
  }
}

void dynamic_bitmap2bytemap(bytemap_t *q, bitmap_t *p)
{
  int i, j, w, h;
  uint8_t *qbuf, *pbuf;
  int qpitch, ppitch;

  assert(q);
  assert(p);
  assert(bytemap_get_width(q) == bitmap_get_width(p));
  assert(bytemap_get_height(q) == bitmap_get_height(p));

  w = bytemap_get_width(q);
  h = bytemap_get_height(q);

  qbuf = bytemap_get_buffer(q);
  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);

  pbuf = bitmap_get_buffer(p);
  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      *(qbuf + j) = ((*(pbuf + (j >> 3))) >> (j % 8)) & 1 ? 255 : 0;
    }
    qbuf += qpitch;
    pbuf += ppitch;
  }
}

void bytemap2bitmap(bitmap_t *q, bytemap_t *p)
{
  int i, j, w, h;
  uint8_t *qbuf, *pbuf;
  int qpitch, ppitch;

  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bytemap_get_width(p));
  assert(bitmap_get_height(q) == bytemap_get_height(p));

  w = bitmap_get_width(q);
  h = bitmap_get_height(q);

  qbuf = bitmap_get_buffer(q);
  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);

  pbuf = bytemap_get_buffer(p);
  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (*(pbuf + j) == 0) {
	*(qbuf + (j >> 3)) &= ~(1 << (j % 8));
      } else {
	*(qbuf + (j >> 3)) |= (1 << (j % 8));
      }
    }
    qbuf += qpitch;
    pbuf += ppitch;
  }
}
vector_t *cvector_new_and_copy_point_list(point_list_t *list)
{
  int i;
  vector_t *vec;
  dlink_t *link;
  point_t *p;

  assert(list);

  i = 0;
  vec = vector_new(point_list_get_count(list), true);
  for (link = list->tail->next; link != list->head; link = link->next) {
    p = (point_t *)link->object;
    vector_put_value(p->x, vec, i);
    ivector_put_value(p->y, vec, i);
    i++;
  }

  return vec;
}

void point_list_copy_cvector(point_list_t *list, vector_t *vec)
{
  int i;
  dlink_t *link;
  point_t *p;

  assert(list);
  assert(vec);
  assert(vector_get_dimension(vec) == point_list_get_count(list));

  i = 0;
  for (link = list->tail->next; link != list->head; link = link->next) {
    p = (point_t *)link->object;
    p->x = vector_get_value(vec, i);
    p->y = ivector_get_value(vec, i);
    i++;
  }
}
