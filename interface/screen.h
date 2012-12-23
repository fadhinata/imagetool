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
#ifndef __SCREEN_H__
#define __SCREEN_H__

//#ifdef __cplusplus
//extern "C" {
//#endif

#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>

  //#define USE_LIBSDL
  //#define USE_LIBBGI

  void initialize_screen(int w, int h, int bpp);
  void deinitialize_screen(void);
  void load_and_display_BMP(const char *path);
  void save_screen_as_BMP(const char *fn);
  void read_bytemap_in_screen(bytemap_t *red, bytemap_t *green, bytemap_t *blue);
  void write_bytemap_to_screen(bytemap_t *red, bytemap_t *green, bytemap_t *blue);
  void write_bitmap_to_screen(bitmap_t *red, bitmap_t *green, bitmap_t *blue);
  void wait_keyhit(void);

//#ifdef __cplusplus
//}
//#endif

#endif /* __SCREEN_H__ */
