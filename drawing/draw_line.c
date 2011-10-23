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
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <common.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <pixmap/dwordmap.h>
#include <pixmap/floatmap.h>

void bitmap_draw_line(int x1, int y1, int x2, int y2, uint8_t val, bitmap_t *m)
{
  int sdx, sdy, i;
  real_t slope;
  int dx, dy, w, h;
  int dxabs, dyabs;
  int px, py;

  assert(m);

  dx = x2 - x1;      /* the horizontal distance of the line */
  dy = y2 - y1;      /* the vertical distance of the line */

  dxabs = abs(dx) + 1;
  dyabs = abs(dy) + 1;

  sdx = sgn(dx);
  sdy = sgn(dy);

  dx = sdx * dxabs;
  dy = sdy * dyabs;

  w = bitmap_get_width(m);
  h = bitmap_get_height(m);

  if (dxabs >= dyabs) {
    /* the line is more horizontal than vertical */
    slope = (real_t)dy / (real_t)dx;
    for (i = 0; i != dx; i += sdx) {
      px = i + x1;
      if (!(px >= 0 && px < w)) continue;
      py = slope*i + y1;
      if (!(py >= 0 && py < h)) continue;
      if (val != 0) bitmap_set_value(m, px, py);
      else bitmap_reset_value(m, px, py);
      //*(p->buffer+py*p->header.pitch+px) = col;
    }
  } else {
    /* the line is more vertical than horizontal */
    slope = (real_t)dx / (real_t)dy;
    for (i = 0; i != dy; i += sdy) {
      px = slope*i + x1;
      if (!(px >= 0 && px < w)) continue;
      py = i + y1;
      if (!(py >= 0 && py < h)) continue;
      if (val != 0) bitmap_set_value(m, px, py);
      else bitmap_reset_value(m, px, py);
      //*(p->buffer+py*p->header.pitch+px) = col;
    }
  }
}

#define PIXMAP_DRAW_LINE(type, x1, y1, x2, y2, val, m) {        \
    int sdx, sdy, i;                                            \
    real_t slope;                                               \
    int dx, dy, w, h, pitch;                                    \
    int dxabs, dyabs;                                           \
    int px, py;                                                 \
    type *buf;                                                  \
    assert(m);							\
    dx = x2 - x1;                                               \
    dy = y2 - y1;                                               \
    w = (m)->header.width;                                      \
    h = (m)->header.height;                                     \
    pitch = (m)->header.pitch / sizeof(*buf);                   \
    buf = (m)->buffer;                                          \
    dxabs = abs(dx) + 1;                                        \
    dyabs = abs(dy) + 1;                                        \
    sdx = sgn(dx);                                              \
    sdy = sgn(dy);                                              \
    dx = sdx * dxabs;                                           \
    dy = sdy * dyabs;                                           \
    if (dxabs >= dyabs) {                                       \
      slope = (real_t)dy / (real_t)dx;                          \
      for (i = 0; i != dx; i += sdx) {                          \
        px = i + x1;                                            \
        if (!(px >= 0 && px < w)) continue;                     \
        py = slope * i + y1;                                    \
        if (!(py >= 0 && py < h)) continue;                     \
        *(buf + py * pitch + px) = val;                         \
      }                                                         \
    } else {                                                    \
      slope = (real_t)dx / (real_t)dy;                          \
      for (i = 0; i != dy; i += sdy) {                          \
        px = slope * i + x1;                                    \
        if (!(px >= 0 && px < w)) continue;                     \
        py = i + y1;                                            \
        if (!(py >= 0 && py < h)) continue;                     \
        *(buf + py * pitch + px) = val;                         \
      }                                                         \
    }                                                           \
  }

void bytemap_draw_line(int x1, int y1, int x2, int y2, uint8_t val, bytemap_t *m)
{
  PIXMAP_DRAW_LINE(uint8_t, x1, y1, x2, y2, val, m);
}

void wordmap_draw_line(int x1, int y1, int x2, int y2, uint16_t val, wordmap_t *m)
{
  PIXMAP_DRAW_LINE(uint16_t, x1, y1, x2, y2, val, m);
}

void dwordmap_draw_line(int x1, int y1, int x2, int y2, uint32_t val, dwordmap_t *m)
{
  PIXMAP_DRAW_LINE(uint32_t, x1, y1, x2, y2, val, m);
}

void floatmap_draw_line(int x1, int y1, int x2, int y2, float val, floatmap_t *m)
{
  PIXMAP_DRAW_LINE(float, x1, y1, x2, y2, val, m);
}

void bitmap_fast_draw_line(int x1, int y1, int x2, int y2, uint8_t val, bitmap_t *m)
{
  int x, y;
  int dx, dy, w, h;
  int absdx, absdy;
  int px, py;
  int sdx, sdy, i;

  assert(m);

  w = bitmap_get_width(m);
  h = bitmap_get_height(m);

  dx = x2 - x1;      /* the horizontal distance of the line */
  dy = y2 - y1;      /* the vertical distance of the line */

  absdx = abs(dx) + 1;
  absdy = abs(dy) + 1;

  sdx = sgn(dx);
  sdy = sgn(dy);

  x = absdy >> 1;
  y = absdx >> 1;

  px = x1;
  py = y1;

  if ((px >= 0 && px < w) &&
      (py >= 0 && py < h)) {
    if (val != 0) bitmap_set_value(m, px, py);
    else bitmap_reset_value(m, px, py);
    //*((m)->buffer+py*m->header.pitch+px) = val;
  }

  if (absdx >= absdy) {
    /* the line is more horizontal than vertical */
    for (i = 0; i < absdx-1; i++) {
      y += absdy;
      if (y >= absdx) {
	y -= absdx;
	py += sdy;
      }
      px += sdx;
      if ((px >= 0 && px < w) &&
	  (py >= 0 && py < h)) {
	if (val != 0) bitmap_set_value(m, px, py);
	else bitmap_reset_value(m, px, py);
	//*((m)->buffer+py*m->header.pitch+px) = val;
      }
    }
  } else {
    /* the line is more vertical than horizontal */
    for (i = 0; i < absdy-1; i++) {
      x += absdx;
      if (x >= absdy) {
	x -= absdy;
	px += sdx;
      }
      py += sdy;
      if ((px >= 0 && px < w) &&
	  (py >= 0 && py < h)) {
	if (val != 0) bitmap_set_value(m, px, py);
	else bitmap_reset_value(m, px, py);
	//*((m)->buffer+py*m->header.pitch+px) = val;
      }
    }
  }
}

#define PIXMAP_FAST_DRAW_LINE(type, x1, y1, x2, y2, val, m) {   \
    int x, y;                                                   \
    int dx, dy, w, h, pitch;                                    \
    int absdx, absdy;                                           \
    int px, py;                                                 \
    int sdx, sdy, i;                                            \
    type *buf;                                                  \
    assert(m);							\
    w = (m)->header.width;                                      \
    h = (m)->header.height;                                     \
    pitch = (m)->header.pitch / sizeof(*buf);                   \
    buf = (m)->buffer;                                          \
    dx = x2-x1;                                                 \
    dy = y2-y1;                                                 \
    absdx = abs(dx)+1;                                          \
    absdy = abs(dy)+1;                                          \
    sdx = sgn(dx);                                              \
    sdy = sgn(dy);                                              \
    x = absdy>>1;                                               \
    y = absdx>>1;                                               \
    px = x1;                                                    \
    py = y1;                                                    \
    if ((px >= 0 && px < w) &&                                  \
	(py >= 0 && py < h)) {                                  \
      *(buf + py * pitch + px) = val;                           \
    }                                                           \
    if (absdx >= absdy) {                                       \
      for (i = 0; i < absdx-1; i++) {                           \
	y += absdy;                                             \
	if (y >= absdx) {                                       \
	  y -= absdx;                                           \
	  py += sdy;                                            \
	}                                                       \
	px += sdx;                                              \
	if ((px >= 0 && px < w) &&                              \
	    (py >= 0 && py < h)) {                              \
	  *(buf + py * pitch + px) = val;                       \
	}                                                       \
      }                                                         \
    } else {                                                    \
      for (i = 0; i < absdy-1; i++) {                           \
	x += absdx;                                             \
	if (x >= absdy) {                                       \
	  x -= absdy;                                           \
	  px += sdx;                                            \
	}                                                       \
	py += sdy;                                              \
	if ((px >= 0 && px < w) &&                              \
	    (py >= 0 && py < h)) {                              \
	  *(buf + py * pitch + px) = val;                       \
	}                                                       \
      }                                                         \
    }                                                           \
  }

void bytemap_fast_draw_line(int x1, int y1, int x2, int y2, uint8_t val, bytemap_t *m)
{
  PIXMAP_DRAW_LINE(uint8_t, x1, y1, x2, y2, val, m);
}

void wordmap_fast_draw_line(int x1, int y1, int x2, int y2, uint16_t val, wordmap_t *m)
{
  PIXMAP_DRAW_LINE(uint16_t, x1, y1, x2, y2, val, m);
}

void dwordmap_fast_draw_line(int x1, int y1, int x2, int y2, uint32_t val, dwordmap_t *m)
{
  PIXMAP_DRAW_LINE(uint32_t, x1, y1, x2, y2, val, m);
}

void floatmap_fast_draw_line(int x1, int y1, int x2, int y2, float val, floatmap_t *m)
{
  PIXMAP_DRAW_LINE(float, x1, y1, x2, y2, val, m);
}
