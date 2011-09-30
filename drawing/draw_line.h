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
#ifndef __DRAW_LINE_H__
#define __DRAW_LINE_H__

#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>
#include <dwordmap.h>
#include <floatmap.h>

#ifdef __cplusplus
extern "C" {
#endif

  void bitmap_draw_line(int x1, int y1, int x2, int y2, uint8_t val, bitmap_t *m);
  void bytemap_draw_line(int x1, int y1, int x2, int y2, uint8_t val, bytemap_t *m);
  void wordmap_draw_line(int x1, int y1, int x2, int y2, uint16_t val, wordmap_t *m);
  void dwordmap_draw_line(int x1, int y1, int x2, int y2, uint32_t val, dwordmap_t *m);
  void floatmap_draw_line(int x1, int y1, int x2, int y2, float val, floatmap_t *m);
#define draw_line(x1, y1, x2, y2, val, m) bytemap_draw_line(x1, y1, x2, y2, val, m)

  void bitmap_fast_draw_line(int x1, int y1, int x2, int y2, uint8_t val, bitmap_t *m);
  void bytemap_fast_draw_line(int x1, int y1, int x2, int y2, uint8_t val, bytemap_t *m);
  void wordmap_fast_draw_line(int x1, int y1, int x2, int y2, uint16_t val, wordmap_t *m);
  void dwordmap_fast_draw_line(int x1, int y1, int x2, int y2, uint32_t val, dwordmap_t *m);
  void floatmap_fast_draw_line(int x1, int y1, int x2, int y2, float val, floatmap_t *m);
#define fast_draw_line(x1, y1, x2, y2, val, m) bytemap_fast_draw_line(x1, y1, x2, y2, val, m)

#ifdef __cplusplus
}
#endif

#endif /* __DRAW_LINE_H__ */
