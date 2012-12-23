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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <float.h>
#include <math.h>

#define REAL_TYPE_AS_DOUBLE

#undef sqr
#define sqr(x) ((x)*(x))

#undef min
#define min(x, y) ((x) > (y) ? (y) : (x))

#undef max
#define max(x, y) ((x) > (y) ? (x) : (y))

#undef abs
#define abs(x) ((x) > 0 ? (x) : -(x))

#undef sgn
#define sgn(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))

#undef round
#define round(x) floor((x) + 0.5)

#ifdef REAL_TYPE_AS_DOUBLE
typedef double real_t;
# define cosr(x) cos(x)
# define acosr(x) acos(x)
# define sinr(x) sin(x)
# define asinr(x) asin(x)
# define tanr(x) tan(x)
# define atanr(x) atan(x)
# define tan2r(y, x) tan2(y, x)
# define atan2r(y, x) atan2(y, x)
# define REAL_EPSILON DBL_EPSILON
#elif REAL_TYPE_AS_FLOAT
typedef float real_t;
# define cosr(x) cosf(x)
# define acosr(x) acosf(x)
# define sinr(x) sinf(x)
# define asinr(x) asinf(x)
# define tanr(x) tanf(x)
# define atanr(x) atanf(x)
# define tan2r(y, x) tan2f(y, x)
# define atan2r(y, x) atan2f(y, x)
# define REAL_EPSILON FLT_EPSILON
#else
# error "Undefined real_t!: data-type"
#endif

#ifndef __cplusplus
typedef enum { false, true } bool;
#endif

#endif /* __COMMON_H__ */
