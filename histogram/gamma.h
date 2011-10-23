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
#ifndef __GAMMA_H__
#define __GAMMA_H__

#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  //int *bytemap_gamma_transfer_table(int *conv, real_t r);
  int bytemap_gamma_correct(bytemap_t *q, bytemap_t *p, real_t gamma);
  //int *wordmap_gamma_transfer_table(int *conv, real_t r);
  int wordmap_gamma_correct(wordmap_t *q, wordmap_t *p, real_t gamma);

#ifdef __cplusplus
}
#endif

#endif /* __GAMMA_H__ */
