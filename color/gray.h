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
#ifndef __GRAY_H__
#define __GRAY_H__

#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <pixmap/dwordmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  void rgb2gray(bytemap_t *gray, bytemap_t *r, bytemap_t *g, bytemap_t *b);
  void wavelength2rgb(unsigned char *r, unsigned char *g, unsigned char *b, float wavelength);
  void bytemap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, bytemap_t *p);
  void wordmap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, wordmap_t *p);
  void dwordmap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, dwordmap_t *p);
  void dynamic_bytemap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, bytemap_t *p);
  void dynamic_wordmap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, wordmap_t *p);
  void dynamic_dwordmap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, dwordmap_t *p);

#ifdef __cplusplus
}
#endif

#endif /* __GRAY_H__ */
