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
#ifndef __BEZIER_H__
#define __BEZIER_H__

#include <linear_algebra/matrix.h>
#include <geometry/point_list.h>

#ifdef __cplusplus
extern "C" {
#endif

  /* Bezier curve equation
   * for 0 <= u <= 1 and n + 1 ctrol-points,
   * b(u) = sum(k=0..n, kth-ctrl * (n! / (k! * (n - k)!)) * u^k * (1 - u)^(n - k)
   * blending function : (n! / (k! * (n - k)!)) * u^k * (1 - u)^(n - k)
   */
  void matrix_bezier_spline_curve(matrix_t *plots, matrix_t *controls);
  void point_list_create_bezier_spline(point_list_t *bezier, int nbezier, point_list_t *controls);

#ifdef __cplusplus
}
#endif

#endif /* __BEZIER_H__ */
