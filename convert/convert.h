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
#ifndef __CONVERT_H__
#define __CONVERT_H__

#include <linear_algebra/matrix.h>
#include <linear_algebra/vector.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <pixmap/dwordmap.h>
#include <pixmap/floatmap.h>
#include <buffering/dlink.h>
#include <geometry/point_list.h>

#ifdef __cplusplus
extern "C" {
#endif

  void matrix2bitmap(bitmap_t *bitmap, matrix_t *matrix, real_t pickup, real_t pickdown);
  void imatrix2bitmap(bitmap_t *bitmap, matrix_t *matrix, real_t pickup, real_t pickdown);
  void cmatrix2bitmap(bitmap_t *bitmap, matrix_t *matrix, complex_t pickup, complex_t pickdown);

  void matrix2bytemap(bytemap_t *bytemap, matrix_t *matrix);
  void imatrix2bytemap(bytemap_t *bytemap, matrix_t *matrix);
  void matrix2wordmap(wordmap_t *wordmap, matrix_t *matrix);
  void imatrix2wordmap(wordmap_t *wordmap, matrix_t *matrix);
  void matrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix);
  void imatrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix);

  void cmatrix2bytemap(bytemap_t *bytemap, matrix_t *matrix);
  void cmatrix2wordmap(wordmap_t *wordmap, matrix_t *matrix);
  void cmatrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix);

  void dynamic_matrix2bytemap(bytemap_t *bytemap, matrix_t *matrix);
  void dynamic_imatrix2bytemap(bytemap_t *bytemap, matrix_t *matrix);
  void dynamic_matrix2wordmap(wordmap_t *wordmap, matrix_t *matrix);
  void dynamic_imatrix2wordmap(wordmap_t *wordmap, matrix_t *matrix);
  void dynamic_matrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix);
  void dynamic_imatrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix);

  void dynamic_cmatrix2bytemap(bytemap_t *bytemap, matrix_t *matrix);
  void dynamic_cmatrix2wordmap(wordmap_t *wordmap, matrix_t *matrix);
  void dynamic_cmatrix2dwordmap(dwordmap_t *dwordmap, matrix_t *matrix);

  void bitmap2matrix(matrix_t *matrix, bitmap_t *bitmap);
  void bytemap2matrix(matrix_t *matrix, bytemap_t *bytemap);
  void bytemap2imatrix(matrix_t *matrix, bytemap_t *bytemap);
  void wordmap2matrix(matrix_t *matrix, wordmap_t *wordmap);
  void wordmap2imatrix(matrix_t *matrix, wordmap_t *wordmap);
  void dwordmap2matrix(matrix_t *matrix, dwordmap_t *dwordmap);
  void dwordmap2imatrix(matrix_t *matrix, dwordmap_t *dwordmap);

  void matrix2floatmap(floatmap_t *floatmap, matrix_t *matrix);
  void imatrix2floatmap(floatmap_t *floatmap, matrix_t *matrix);
  void cmatrix2floatmap(floatmap_t *floatmap, matrix_t *matrix);
  void floatmap2matrix(matrix_t *matrix, floatmap_t *floatmap);
  void floatmap2imatrix(matrix_t *matrix, floatmap_t *floatmap);

  void matrix_copy_bytemap(matrix_t *matrix, int c, int r, bytemap_t *bytemap, int x, int y, int w, int h);
  void matrix_copy_wordmap(matrix_t *matrix, int c, int r, wordmap_t *wordmap, int x, int y, int w, int h);
  void matrix_copy_dwordmap(matrix_t *matrix, int c, int r, dwordmap_t *dwordmap, int x, int y, int w, int h);

  void bitmap2bytemap(bytemap_t *q, bitmap_t *p);
  void bytemap2bitmap(bitmap_t *q, bytemap_t *p);

  vector_t *cvector_new_and_copy_point_list(point_list_t *list);
  void point_list_copy_cvector(point_list_t *list, vector_t *vec);

#ifdef __cplusplus
}
#endif

#endif /* __CONVERT_H__ */
