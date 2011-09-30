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
#ifndef __ROBERTS_H__
#define __ROBERTS_H__

#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  void roberts_magnitude(wordmap_t *q, bytemap_t *p);
  void roberts_edge(bitmap_t *q, bytemap_t *p, int tlevel);

#ifdef __cplusplus
}
#endif

#endif /* __ROBERTS_H__ */
