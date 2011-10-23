/*
  Copyright (C) 2006 Pedro Felzenszwalb

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/
#ifndef __DT_H__
#define __DT_H__

#include <linear_algebra/vector.h>
#include <linear_algebra/matrix.h>
#include <pixmap/bitmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  void vector_distance_transform(vector_t *dist, vector_t *vec, int n);
  void matrix_distance_transform(matrix_t *dist, matrix_t *mat);
  void bitmap_distance_transform(matrix_t *dist, bitmap_t *bin);

#ifdef __cplusplus
}
#endif

#endif /* __DT_H__ */

