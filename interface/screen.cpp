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
#include <stdio.h>
#include <assert.h>

#if defined(USE_LIBSDL)
#include <SDL/SDL.h>
//#include <SDL/SDL_image.h>
#elif defined(USE_LIBBGI)
#include <winbgim.h>
#else
#error "Undefined graphic library!"
#endif

#include <common.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>

#ifdef USE_LIBSDL

static SDL_Surface *screen = NULL;
static SDL_Surface *image = NULL;

/* Return the pixel value at (x, y)
   NOTE: The surface must be locked before calling this! */
static Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp) {
  case 1: return *p;
  case 2: return *(Uint16 *)p;
  case 3:
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
      return p[0] | p[1] << 8 | p[2] << 16;
  case 4: return *(Uint32 *)p;
  default: return 0;       /* shouldn't happen, but avoids warnings */
  } // switch
}

static void getbytemap(bytemap_t *r, bytemap_t *g, bytemap_t *b, SDL_Surface *image)
{
  int x, y, w, h;
  Uint8 rval, gval, bval;
  Uint32 colour;

  assert(r || g || b);

  if (SDL_MUSTLOCK(image)) {
    if (SDL_LockSurface(image) < 0) return;
  }

  if (r) { w = r->header.width; h = r->header.height; }
  if (g) { w = g->header.width; h = g->header.height; }
  if (b) { w = b->header.width; h = b->header.height; }

  w = min(w, image->w);
  h = min(h, image->h);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      colour = getpixel(image, x, y);
      SDL_GetRGB(colour, image->format, &rval, &gval, &bval);
      if (r) bytemap_put_value(rval, r, x, y);
      if (g) bytemap_put_value(gval, g, x, y);
      if (b) bytemap_put_value(bval, b, x, y);
    }
  }

  if (SDL_MUSTLOCK(image)) SDL_UnlockSurface(image);
}

/* Set the pixel at (x, y) to the given value
   NOTE: The surface must be locked before calling this! */
static void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  // Here p is the address to the pixel we want to set
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp) {
  case 1:
    *p = pixel;
    break;
   
  case 2:
    *(Uint16 *)p = pixel;
    break;
   
  case 3:
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    }
    else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;
   
  case 4:
    *(Uint32 *)p = pixel;
    break;
   
  default:
    break;           /* shouldn't happen, but avoids warnings */
  } // switch
}

static void putline(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 pixel)
{
  int sdx, sdy, i;
  double slope;
  int dx, dy;
  int dxabs, dyabs;
  int px, py;

  dx = x2-x1;      /* the horizontal distance of the line */
  dy = y2-y1;      /* the vertical distance of the line */

  dxabs = abs(dx)+1;
  dyabs = abs(dy)+1;

  sdx = sgn(dx);
  sdy = sgn(dy);

  dx = sdx*dxabs;
  dy = sdy*dyabs;
	
  if (dxabs >= dyabs) {
    /* the line is more horizontal than vertical */
    slope = (double)dy / (double)dx;
    for (i = 0; i != dx; i += sdx) {
      px = i+x1;
      py = slope*i + y1;
      putpixel(surface, px, py, pixel);
    }
  } else {
    /* the line is more vertical than horizontal */
    slope = (double)dx/(double)dy;
    for (i = 0; i != dy; i += sdy) {
      px = slope*i + x1;
      py = i + y1;
      putpixel(surface, px, py, pixel);
    }
  }
}

static void putbytemap(bytemap_t *r, bytemap_t *g, bytemap_t *b, SDL_Surface *surface)
{
  int x, y, w, h;
  Uint8 rval, gval, bval;
  Uint32 colour;

  assert(r || g || b);

  if (SDL_MUSTLOCK(surface)) {
    if (SDL_LockSurface(surface) < 0) return;
  }

  if (r) { w = r->header.width; h = r->header.height; }
  if (g) { w = g->header.width; h = g->header.height; }
  if (b) { w = b->header.width; h = b->header.height; }

  w = min(w, surface->w);
  h = min(h, surface->h);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      if (r) rval = bytemap_get_value(r, x, y);
      else rval = 0;
      if (g) gval = bytemap_get_value(g, x, y);
      else gval = 0;
      if (b) bval = bytemap_get_value(b, x, y);
      else bval = 0;
      colour = SDL_MapRGB(surface->format, rval, gval, bval);
      putpixel(surface, x, y, colour);
    }
  }

  if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);

  SDL_Flip(surface);
}

static void putbitmap(bitmap_t *r, bitmap_t *g, bitmap_t *b, SDL_Surface *surface)
{
  int x, y, w, h;
  Uint8 rval, gval, bval;
  Uint32 colour;

  assert(r || g || b);
  /*
  if (SDL_MUSTLOCK(surface)) {
    if (SDL_LockSurface(surface) < 0) return;
  }
  */
  if (r) { w = r->header.width; h = r->header.height; }
  if (g) { w = g->header.width; h = g->header.height; }
  if (b) { w = b->header.width; h = b->header.height; }

  w = min(w, surface->w);
  h = min(h, surface->h);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      if (r) rval = bitmap_get_value(r, x, y) ? 255 : 0;
      else rval = 0;
      if (g) gval = bitmap_get_value(g, x, y) ? 255 : 0;
      else gval = 0;
      if (b) bval = bitmap_get_value(b, x, y) ? 255 : 0;
      else bval = 0;
      colour = SDL_MapRGB(surface->format, rval, gval, bval);
      putpixel(surface, x, y, colour);
    }
  }
  /*
  if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
  */
  SDL_Flip(surface);
}

void wait_keyhit(void)
{
  SDL_Event event;
  int keypress = 0;

  SDL_Flip(screen);

  while (!keypress) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: keypress = 1; break;
      case SDL_KEYDOWN: keypress = 1; break;
      }
    }
  }
}

void initialize_screen(int w, int h, int bpp)
{
  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(w, h, bpp, SDL_SWSURFACE | SDL_ANYFORMAT);
}

void deinitialize_screen(void)
{
  SDL_FreeSurface(screen);
  SDL_FreeSurface(image);
  SDL_Quit();
}

void load_and_display_BMP(const char *path)
{
  SDL_Rect src, dst;

  image = SDL_LoadBMP(path);

  /*
   * Palettized screen modes will have a default palette (a standard
   * 8*8*4 colour cube), but if the image is palettized as well we can
   * use that palette for a nicer colour matching
   */
  if (image->format->palette && screen->format->palette) {
    SDL_SetColors(screen, image->format->palette->colors, 0, image->format->palette->ncolors);
  }

  if (image->w > screen->w) {
    src.x = (image->w - screen->w) / 2; src.w = screen->w;
    dst.x = 0; dst.w = screen->w;
  } else {
    src.x = 0; src.w = image->w;
    dst.x = (screen->w - image->w) / 2; dst.w = image->w;
  }

  if (image->h > screen->h) {
    src.y = (image->h - screen->h) / 2; src.h = screen->h;
    dst.y = 0; dst.h = screen->h;
  } else {
    src.y = 0; src.h = image->h;
    dst.y = (screen->h - image->h) / 2; dst.h = image->h;
  }

  SDL_BlitSurface(image, &src, screen, &dst);
}

void save_screen_as_BMP(const char *fn)
{
  SDL_SaveBMP(screen, fn);
}

void read_bytemap_in_screen(bytemap_t *red, bytemap_t *green, bytemap_t *blue)
{
  getbytemap(red, green, blue, screen);
}

void write_bytemap_to_screen(bytemap_t *red, bytemap_t *green, bytemap_t *blue)
{
  putbytemap(red, green, blue, screen);
}

void write_bitmap_to_screen(bitmap_t *red, bitmap_t *green, bitmap_t *blue)
{
  putbitmap(red, green, blue, screen);
}

#elif defined(USE_LIBBGI)

static unsigned char *image = NULL;

void wait_keyhit(void)
{
  getch();
}

void initialize_screen(int w, int h, int bpp)
{
  initwindow(w, h, "ImageTool");
  //printf("imagesize: %d\n", imagesize(0, 0, w-1, h-1));
  image = (unsigned char *)malloc(imagesize(0, 0, w-1, h-1));
  //assert(image);
  getimage(0, 0, w-1, h-1, image);
}

void deinitialize_screen(void)
{
  free(image);
  closegraph();
}

void load_and_display_BMP(const char *path)
{
  int w, h;

  w = ((BITMAP *)image)->bmWidth;
  h = ((BITMAP *)image)->bmHeight;

  printf("path: %s, w:%d, h:%d\n", path, w, h);
  //readimagefile(path);
  readimagefile(path, 0, 0, w-1, h-1);
  //putimage(0, 0, image, COPY_PUT);
}

void save_screen_as_BMP(const char *fn)
{
  int w, h;

  w = ((BITMAP *)image)->bmWidth;
  h = ((BITMAP *)image)->bmHeight;

  writeimagefile(fn, 0, 0, w-1, h-1);
}

void read_bytemap_in_screen(bytemap_t *red, bytemap_t *green, bytemap_t *blue)
{
  int *buffer, value, i, j;
  int w, h, pitch;

  buffer = (int *)(((BITMAP *)image)->bmBits);
  w = ((BITMAP *)image)->bmWidth;
  h = ((BITMAP *)image)->bmHeight;
  pitch = ((BITMAP *)image)->bmWidthBytes / sizeof(*buffer);

  getimage(0, 0, w-1, h-1, image);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = buffer[i * pitch + j];
      if (red) bytemap_put_value((value >> 16) & 0xff, red, j, i);
      if (green) bytemap_put_value((value >> 8) & 0xff, green, j, i);
      if (blue) bytemap_put_value((value & 0xff), blue, j, i);
    }
  }
}

void write_bytemap_to_screen(bytemap_t *red, bytemap_t *green, bytemap_t *blue)
{
  int *buffer, value, i, j;
  int w, h, pitch;

  buffer = (int *)(((BITMAP *)image)->bmBits);
  w = ((BITMAP *)image)->bmWidth;
  h = ((BITMAP *)image)->bmHeight;
  pitch = ((BITMAP *)image)->bmWidthBytes / sizeof(*buffer);
	
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = 0;
      if (red) value |= (bytemap_get_value(red, j, i) << 16);
      if (green) value |= (bytemap_get_value(green, j, i) << 8);
      if (blue) value |= bytemap_get_value(blue, j, i);
      buffer[i * pitch + j] = value;
    }
  }
  putimage(0, 0, image, COPY_PUT);
}

void write_bitmap_to_screen(bitmap_t *red, bitmap_t *green, bitmap_t *blue)
{
  int *buffer, value, i, j;
  int w, h, pitch;

  buffer = (int *)(((BITMAP *)image)->bmBits);
  w = ((BITMAP *)image)->bmWidth;
  h = ((BITMAP *)image)->bmHeight;
  pitch = ((BITMAP *)image)->bmWidthBytes / sizeof(*buffer);
	
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = 0;
      if (red && bitmap_isset(red, j, i)) value |= (255 << 16);
      if (green && bitmap_isset(green, j, i)) value |= (255 << 8);
      if (blue && bitmap_isset(blue, j, i)) value |= 255;
      buffer[i * pitch + j] = value;
    }
  }
  putimage(0, 0, image, COPY_PUT);
}

#else
#error "Undefined graphic library!"
#endif
