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
#ifndef __MEAN_SMOOTH_H__
#define __MEAN_SMOOTH_H__

#include <pixmap/bytemap.h>

#ifdef __cplusplus
extern "C" {
#endif

  void mean_smooth(bytemap_t *q, bytemap_t *p, int bsize);

#ifdef __cplusplus
}
#endif

#endif /* __MEAN_SMOOTH_H__ */
