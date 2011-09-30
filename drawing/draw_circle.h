/**************************************************************************
 * circle.c                                                               *
 * written by David Brackeen                                              *
 * http://www.brackeen.com/home/vga/                                      *
 *                                                                        *
 * This is a 16-bit program.                                              *
 * Tab stops are set to 2.                                                *
 * Remember to compile in the LARGE memory model!                         *
 * To compile in Borland C: bcc -ml circle.c                              *
 *                                                                        *
 * This program will only work on DOS- or Windows-based systems with a    *
 * VGA, SuperVGA or compatible video adapter.                             *
 *                                                                        *
 * Please feel free to copy this source code.                             *
 *                                                                        *
 * DESCRIPTION: This program demostrates drawing how much faster it is to *
 * draw circles using tables rather than math functions.                  *
 **************************************************************************/
#ifndef __DRAW_CIRCLE_H__
#define __DRAW_CIRCLE_H__

#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>
#include <dwordmap.h>
#include <floatmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  /* floating point operation */
  void bitmap_draw_circle(int x, int y, int r, unsigned char val, bitmap_t *m);
  void bytemap_draw_circle(int xorg, int yorg, int radius, unsigned char val, bytemap_t *m);
  void wordmap_draw_circle(int xorg, int yorg, int radius, unsigned short val, wordmap_t *m);
  void dwordmap_draw_circle(int xorg, int yorg, int radius, unsigned long val, dwordmap_t *m);
  void floatmap_draw_circle(int xorg, int yorg, int radius, float val, floatmap_t *m);
#define draw_circle(xorg, yorg, radius, val, m) bytemap_draw_circle(xorg, yorg, radius, val, m)
  void bitmap_draw_filled_circle(int xorg, int yorg, int radius, unsigned char val, bitmap_t *m);
  void bytemap_draw_filled_circle(int xorg, int yorg, int radius, int val, bytemap_t *m);
  void wordmap_draw_filled_circle(int xorg, int yorg, int radius, int val, wordmap_t *m);
  void dwordmap_draw_filled_circle(int xorg, int yorg, int radius, unsigned long val, dwordmap_t *m);
  void floatmap_draw_filled_circle(int xorg, int yorg, int radius, float val, floatmap_t *m);
#define draw_filled_circle(x, y, r, val, m) bytemap_draw_filled_circle(x, y, r, val, m)

  /* fixed point operation */
  void bitmap_fast_draw_circle(int x, int y, int r, unsigned char val, bitmap_t *m);
  void bytemap_fast_draw_circle(int xorg, int yorg, int radius, unsigned char val, bytemap_t *m);
  void wordmap_fast_draw_circle(int xorg, int yorg, int radius, unsigned short val, wordmap_t *m);
  void dwordmap_fast_draw_circle(int xorg, int yorg, int radius, unsigned long val, dwordmap_t *m);
  void floatmap_fast_draw_circle(int xorg, int yorg, int radius, float val, floatmap_t *m);
#define fast_draw_circle(xorg, yorg, radius, val, m) bytemap_fast_draw_circle(xorg, yorg, radius, val, m)
  void bitmap_fast_draw_filled_circle(int xorg, int yorg, int radius, unsigned char val, bitmap_t *m);
  void bytemap_fast_draw_filled_circle(int xorg, int yorg, int radius, int val, bytemap_t *m);
  void wordmap_fast_draw_filled_circle(int xorg, int yorg, int radius, int val, wordmap_t *m);
  void dwordmap_fast_draw_filled_circle(int xorg, int yorg, int radius, unsigned long val, dwordmap_t *m);
  void floatmap_fast_draw_filled_circle(int xorg, int yorg, int radius, float val, floatmap_t *m);
#define draw_fast_filled_circle(x, y, r, val, m) bytemap_fast_draw_filled_circle(x, y, r, val, m)

#ifdef __cplusplus
}
#endif

#endif /* __DRAW_CIRCLE_H__ */
