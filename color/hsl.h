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

#ifndef __HSL_H__
#define __HSL_H__

#include <bytemap.h>
#include <floatmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  void rgb2hsl(floatmap_t *h, floatmap_t *s, floatmap_t *l, bytemap_t *r, bytemap_t *g, bytemap_t *b);
  void hsl2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, floatmap_t *h, floatmap_t *s, floatmap_t *l);

#ifdef __cplusplus
}
#endif

#endif /* __HSL_H__ */
