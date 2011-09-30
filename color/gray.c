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
#include <math.h>
#include <limits.h>
#include <assert.h>
#include <stdint.h>

#include <common.h>
#include <gray.h>

void rgb2gray(bytemap_t *gray, bytemap_t *r, bytemap_t *g, bytemap_t *b)
{
  int x, y, w, h;
  uint8_t *rbuf, *gbuf, *bbuf, *graybuf;
  uint32_t grayval;
  int rpitch, gpitch, bpitch, graypitch;

  assert(r);
  assert(g);
  assert(b);
  assert(gray);
  assert(bytemap_get_width(r) == bytemap_get_width(g));
  assert(bytemap_get_height(r) == bytemap_get_height(g));
  assert(bytemap_get_width(r) == bytemap_get_width(b));
  assert(bytemap_get_height(r) == bytemap_get_height(b));
  assert(bytemap_get_width(r) == bytemap_get_width(gray));
  assert(bytemap_get_height(r) == bytemap_get_height(gray));

  w = bytemap_get_width(r);
  h = bytemap_get_height(r);

  rbuf = bytemap_get_buffer(r);
  rpitch = bytemap_get_pitch(r) / sizeof(*rbuf);

  gbuf = bytemap_get_buffer(g);
  gpitch = bytemap_get_pitch(g) / sizeof(*gbuf);

  bbuf = bytemap_get_buffer(b);
  bpitch = bytemap_get_pitch(b) / sizeof(*bbuf);

  graybuf = bytemap_get_buffer(gray);
  graypitch = bytemap_get_pitch(gray) / sizeof(*graybuf);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      //grayval = 0.3*rval+0.59*gval+0.11*bval;
      //if (grayval > 255) grayval = 255;
      //else if (grayval < 0) grayval = 0;
      //*(gray->buffer+y*gray->header.pitch+x) = grayval;
      grayval = 30 * (*(rbuf + x)) + 59 * (*(gbuf + x)) + 11 * (*(bbuf + x));
      *(graybuf + x) = grayval / 100;
    }
    rbuf += rpitch;
    gbuf += gpitch;
    bbuf += bpitch;
    graybuf += graypitch;
  }
}

/* range 380nm ~ 780nm */
/* RGB VALUES FOR VISIBLE WAVELENGTHS   by Dan Bruton (astro@tamu.edu)
   This program can be found at 
   http://www.physics.sfasu.edu/astro/color.html
   and was last updated on February 20, 1996.

   This program will create a ppm (portable pixmap) image of a spectrum.
   The spectrum is generated using approximate RGB values for visible 
   wavelengths between 380 nm and 780 nm.
   NetPBM's ppmtogif can be used to convert the ppm image
   to a gif.  The red, green and blue values (RGB) are
   assumed to vary linearly with wavelength (for GAMMA=1).  
   NetPBM Software: ftp://ftp.cs.ubc.ca/ftp/archive/netpbm/ */

#define GAMMA 1.0 //0.80

void wavelength2rgb(uint8_t *r, uint8_t *g, uint8_t *b, float wavelength)
{
  int itype;
  float rval, gval, bval;
  float sss;

  assert(wavelength >= 380 && wavelength <= 780);

  if (wavelength < 440) { // 380 <= wavelength < 440
    rval = -1.0 * (wavelength - 440.0) / (440.0 - 380.0);
    gval = 0.0;
    bval = 1.0;
  } else if (wavelength < 490) { // 440 <= wavelength < 490
    rval = 0.0;
    gval = (wavelength - 440.0) / (490.0 - 440.0);
    bval = 1.0;
  } else if (wavelength < 510) { // 490 <= wavelength < 510
    rval = 0.0;
    gval = 1.0;
    bval = -1.0 * (wavelength - 510.0) / (510.0 - 490.0);
  } else if (wavelength < 580) { // 510 <= wavelength < 580
    rval = (wavelength - 510.0) / (580.0 - 510.0);
    gval = 1.0;
    bval = 0.0;
  } else if (wavelength < 645) { // 580 <= wavelength < 645
    rval = 1.0;
    gval = -1.0 * (wavelength - 645.0) / (645.0 - 580.0);
    bval = 0.0;
  } else { // 645 <= wavelength <= 780
    rval = 1.0;
    gval = 0.0;
    bval = 0.0;
  }
		
  /* Let the intensity sss fall off near the vision limits */
  if (wavelength > 700.0)
    sss = 0.3 + 0.7 * (780.0 - wavelength) / (780.0 - 700.0);
  else if (wavelength < 420.0)
    sss = 0.3 + 0.7 * (wavelength - 380.0) / (420.0 - 380.0);
  else
    sss = 1.0;

  /* Gamma adjust and write image to an array */
  *r = (uint8_t)(255.0 * powf(sss * rval, GAMMA));
  *g = (uint8_t)(255.0 * powf(sss * gval, GAMMA));
  *b = (uint8_t)(255.0 * powf(sss * bval, GAMMA));

  /* ITYPE=1 - PLAIN SPECTUM
     ITYPE=2 - MARK SPECTRUM AT 100 nm INTEVALS
     ITYPE=3 - HYDROGEN BALMER EMISSION SPECTRA
     ITYPE=4 - HYDROGEN BALMER ABSORPTION SPECTRA */
  itype = 1;
  switch (itype) {
  case 2:
    if (abs((int)wavelength-400) < 1 ||
	abs((int)wavelength-500) < 1 ||
	abs((int)wavelength-600) < 1 ||
	abs((int)wavelength-700) < 1) {
      *r = 255; *g = 255; *b = 255;
    }
    /*
      for (k = 400; k <= 700; k += 100) {
      if (abs((int)wavelength-k) < 1) {
      *r = 255; *g = 255; *b = 255;
      }
      }
    */
    break;
  case 3:
    if (abs(wavelength-656.0) > 1.0 &&
	abs(wavelength-486.0) > 1.0 &&
	abs(wavelength-433.0) > 1.0 &&
	abs(wavelength-410.0) > 1.0 &&
	abs(wavelength-396.0) > 1.0) {
      *r = 0; *g = 0; *b = 0;
    }
    break;
  case 4:
    if (abs(wavelength-656.0) < 1.1 ||
	abs(wavelength-486.0) < 1.1 ||
	abs(wavelength-433.0) < 1.1 ||
	abs(wavelength-410.0) < 1.1 ||
	abs(wavelength-396.0) < 1.1) {
      *r = 0; *g = 0; *b = 0;
    }
    break;
  }
}

#define PIXMAP_TO_RGB(type, umax, r, g, b, p) {				\
    int x, y, w, h;							\
    uint8_t *rbuf, *gbuf, *bbuf;					\
    type *pbuf, pval;							\
    int rpitch, gpitch, bpitch, ppitch;					\
    assert(r);								\
    assert(g);								\
    assert(b);								\
    assert(p);								\
    assert((r)->header.width == (g)->header.width);			\
    assert((r)->header.height == (g)->header.height);			\
    assert((r)->header.width == (b)->header.width);			\
    assert((r)->header.height == (b)->header.height);			\
    assert((r)->header.width == (p)->header.width);			\
    assert((r)->header.height == (p)->header.height);			\
    rbuf = (r)->buffer;							\
    rpitch = (r)->header.pitch / sizeof(*rbuf);				\
    gbuf = (g)->buffer;							\
    gpitch = (g)->header.pitch / sizeof(*gbuf);				\
    bbuf = (b)->buffer;							\
    bpitch = (b)->header.pitch / sizeof(*bbuf);				\
    pbuf = (p)->buffer;							\
    ppitch = (p)->header.pitch / sizeof(*pbuf);				\
    w = (p)->header.width;						\
    h = (p)->header.height;						\
    for (y = 0; y < h; y++) {						\
      for (x = 0; x < w; x++) {						\
	pval = *(pbuf + x);						\
	wavelength2rgb(rbuf+x, gbuf+x, bbuf+x, 380.0 + 400.0 * pval / umax); \
      }									\
      rbuf += rpitch;							\
      gbuf += gpitch;							\
      bbuf += bpitch;							\
      pbuf += ppitch;							\
    }									\
  }

void bytemap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, bytemap_t *p)
{
  PIXMAP_TO_RGB(uint8_t, UINT8_MAX, r, g, b, p);
}

void wordmap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, wordmap_t *p)
{
  PIXMAP_TO_RGB(uint16_t, UINT16_MAX, r, g, b, p);
}

void dwordmap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, dwordmap_t *p)
{
  PIXMAP_TO_RGB(uint32_t, UINT32_MAX, r, g, b, p);
}

#define DYNAMIC_PIXMAP_TO_RGB(type, r, g, b, p) {			\
    int x, y, w, h;							\
    uint8_t *rbuf, *gbuf, *bbuf;					\
    type *pbuf, pval, pmax;						\
    int rpitch, gpitch, bpitch, ppitch;					\
    assert(r);								\
    assert(g);								\
    assert(b);								\
    assert(p);								\
    assert((r)->header.width == (g)->header.width);			\
    assert((r)->header.height == (g)->header.height);			\
    assert((r)->header.width == (b)->header.width);			\
    assert((r)->header.height == (b)->header.height);			\
    assert((r)->header.width == (p)->header.width);			\
    assert((r)->header.height == (p)->header.height);			\
    w = (p)->header.width;						\
    h = (p)->header.height;						\
    pbuf = (p)->buffer;							\
    ppitch = (p)->header.pitch / sizeof(*pbuf);				\
    pmax = 0;								\
    for (y = 0; y < h; y++) {						\
      for (x = 0; x < w; x++) {						\
	pval = *(pbuf + x);						\
	if (pval > pmax) pmax = pval;					\
      }									\
      pbuf += ppitch;							\
    }									\
    rbuf = (r)->buffer;							\
    rpitch = (r)->header.pitch / sizeof(*rbuf);				\
    gbuf = (g)->buffer;							\
    gpitch = (g)->header.pitch / sizeof(*gbuf);				\
    bbuf = (b)->buffer;							\
    bpitch = (b)->header.pitch / sizeof(*bbuf);				\
    pbuf = (p)->buffer;							\
    ppitch = (p)->header.pitch / sizeof(*pbuf);				\
    for (y = 0; y < h; y++) {						\
      for (x = 0; x < w; x++) {						\
	pval = *(pbuf + x);						\
	wavelength2rgb(rbuf+x, gbuf+x, bbuf+x, 380.0 + 400.0 * pval / pmax); \
      }									\
      rbuf += rpitch;							\
      gbuf += gpitch;							\
      bbuf += bpitch;							\
      pbuf += ppitch;							\
    }									\
  }

void dynamic_bytemap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, bytemap_t *p)
{
  DYNAMIC_PIXMAP_TO_RGB(uint8_t, r, g, b, p);
}

void dynamic_wordmap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, wordmap_t *p)
{
  DYNAMIC_PIXMAP_TO_RGB(uint16_t, r, g, b, p);
}

void dynamic_dwordmap2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, dwordmap_t *p)
{
  DYNAMIC_PIXMAP_TO_RGB(uint32_t, r, g, b, p);
}
