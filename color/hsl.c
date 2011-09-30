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
#include <assert.h>
#include <bytemap.h>
#include <floatmap.h>
#include <common.h>

void rgb2hsl(floatmap_t *h, floatmap_t *s, floatmap_t *l, bytemap_t *r, bytemap_t *g, bytemap_t *b)
{
  int x, y, width, height;
  unsigned char *rbuf, *gbuf, *bbuf;
  float *hbuf = NULL, *sbuf = NULL, *lbuf = NULL;
  float rval, gval, bval;
  float rdelta, gdelta, bdelta;
  float hval, sval, lval;
  float minval, maxval, delta;
  int rpitch, gpitch, bpitch;
  int hpitch, spitch, lpitch;
	
  assert(h || s || l);
  assert(r);
  assert(g);
  assert(b);
  assert(bytemap_get_width(r) == bytemap_get_width(g));
  assert(bytemap_get_height(r) == bytemap_get_height(g));
  assert(bytemap_get_width(r) == bytemap_get_width(b));
  assert(bytemap_get_height(r) == bytemap_get_height(b));

  if (h) {
    assert(floatmap_get_width(h) == floatmap_get_width(r));
    assert(floatmap_get_height(h) == floatmap_get_height(r));
    hbuf = floatmap_get_buffer(h);
    hpitch = floatmap_get_pitch(h) / sizeof(*hbuf);
  }

  if (s) {
    assert(floatmap_get_width(s) == floatmap_get_width(r));
    assert(floatmap_get_height(s) == floatmap_get_height(r));
    sbuf = floatmap_get_buffer(s);
    spitch = floatmap_get_pitch(s) / sizeof(*sbuf);
  }

  if (l) {
    assert(floatmap_get_width(l) == floatmap_get_width(r));
    assert(floatmap_get_height(l) == floatmap_get_height(r));
    lbuf = floatmap_get_buffer(l);
    lpitch = floatmap_get_pitch(l) / sizeof(*lbuf);
  }
	
  width = bytemap_get_width(r);
  height = bytemap_get_height(r);

  rbuf = (unsigned char *)bytemap_get_buffer(r);
  rpitch = bytemap_get_pitch(r) / sizeof(*rbuf);
  gbuf = (unsigned char *)bytemap_get_buffer(g);
  gpitch = bytemap_get_pitch(g) / sizeof(*gbuf);
  bbuf = (unsigned char *)bytemap_get_buffer(b);
  bpitch = bytemap_get_pitch(b) / sizeof(*bbuf);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      rval = (*(rbuf + x)) / 255.0;
      gval = (*(gbuf + x)) / 255.0;
      bval = (*(bbuf + x)) / 255.0;
      minval = min(rval, min(gval, bval));    //Min. value of RGB
      maxval = max(rval, max(gval, bval));    //Max. value of RGB
      delta = maxval - minval;             //Delta RGB value
      lval = (maxval + minval) / 2.0;
      //This is a gray, no chroma...
      if (delta == 0) {
	hval = 0;                                //hsl results = 0 ¡À 1
	sval = 0;
      } else {
	if (lval < 0.5) sval = delta / (maxval + minval);
	else sval = delta / (2 - maxval - minval);

	rdelta = (((maxval - rval) / 6.0) + (delta / 2.0)) / delta;
	gdelta = (((maxval - gval) / 6.0) + (delta / 2.0)) / delta;
	bdelta = (((maxval - bval) / 6.0) + (delta / 2.0)) / delta;

	if (rval == maxval) hval = bdelta - gdelta;
	else if (gval == maxval) hval = (1.0 / 3.0) + rdelta - bdelta;
	else if (bval == maxval) hval = (2.0 / 3.0) + gdelta - rdelta;

	if (hval < 0.0) hval += 1.0;
	if (hval > 1.0) hval -= 1.0;
      }
			
      if (hbuf) *(hbuf + x) = hval;
      if (sbuf) *(sbuf + x) = sval;
      if (lbuf) *(lbuf + x) = lval;
    }
    rbuf += rpitch;
    gbuf += gpitch;
    bbuf += bpitch;
    if (hbuf) hbuf += hpitch;
    if (sbuf) sbuf += spitch;
    if (lbuf) lbuf += lpitch;
  }
}

static __inline float Hue_2_RGB(float v1, float v2, float vH)
{
  if (vH < 0.0) vH += 1.0;
  if (vH > 1.0) vH -= 1.0;
  if ((6.0 * vH) < 1.0) return ( v1 + ( v2 - v1 ) * 6.0 * vH );
  if ((2.0 * vH) < 1.0) return ( v2 );
  if ((3.0 * vH) < 2.0) return ( v1 + ( v2 - v1 ) * ( (2.0 / 3.0) - vH ) * 6.0);
  return ( v1 );
}

void hsl2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, floatmap_t *h, floatmap_t *s, floatmap_t *l)
{
  int x, y, width, height;
  unsigned char *rbuf = NULL, *gbuf = NULL, *bbuf = NULL;
  float *hbuf, *sbuf, *lbuf;
  float rval, gval, bval;
  float hval, sval, lval;
  float val1, val2;
  int rpitch, gpitch, bpitch, hpitch, spitch, lpitch;
	
  assert(r || g || b);
  assert(h);
  assert(s);
  assert(l);
  assert(floatmap_get_width(h) == floatmap_get_width(s));
  assert(floatmap_get_height(h) == floatmap_get_height(s));
  assert(floatmap_get_width(h) == floatmap_get_width(l));
  assert(floatmap_get_height(h) == floatmap_get_height(l));

  if (r) {
    assert(bytemap_get_width(r) == floatmap_get_width(h));
    assert(bytemap_get_height(r) == floatmap_get_height(h));
    rbuf = bytemap_get_buffer(r);
    rpitch = bytemap_get_pitch(r) / sizeof(*rbuf);
  }

  if (g) {
    assert(bytemap_get_width(g) == floatmap_get_width(h));
    assert(bytemap_get_height(g) == floatmap_get_height(h));
    gbuf = bytemap_get_buffer(g);
    gpitch = bytemap_get_pitch(g) / sizeof(*gbuf);
  }

  if (b) {
    assert(bytemap_get_width(b) == floatmap_get_width(h));
    assert(bytemap_get_height(b) == floatmap_get_height(h));
    bbuf = bytemap_get_buffer(b);
    bpitch = bytemap_get_pitch(b) / sizeof(*bbuf);
  }
	
  width = floatmap_get_width(h);
  height = floatmap_get_height(h);

  hbuf = floatmap_get_buffer(h);
  hpitch = floatmap_get_pitch(h) / sizeof(*hbuf);
  sbuf = floatmap_get_buffer(s);
  spitch = floatmap_get_pitch(s) / sizeof(*sbuf);
  lbuf = floatmap_get_buffer(l);
  lpitch = floatmap_get_pitch(l) / sizeof(*lbuf);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      hval = *(hbuf + x);
      sval = *(sbuf + x);
      lval = *(lbuf + x);
      // hsl values = 0~1
      if (sval == 0.0) {
	rval = gval = bval = lval * 255.0;
	if (rbuf) *(rbuf + x) = rval;
	if (gbuf) *(gbuf + x) = gval;
	if (bbuf) *(bbuf + x) = bval;
      } else {
	if (lval < 0.5) val2 = lval * (1.0 + sval);
	else val2 = (lval + sval) - (sval * lval);
	val1 = 2.0 * lval - val2;
	/*
	  rval = 255.0*Hue_2_RGB(val1, val2, hval+(1.0/3.0));
	  gval = 255.0*Hue_2_RGB(val1, val2, hval);
	  bval = 255.0*Hue_2_RGB(val1, val2, hval-(1.0/3.0));
	  if (rval > 255.0) rval = 255.0;
	  else if (rval < 0.0) rval = 0.0;
	  if (gval > 255.0) gval = 255.0;
	  else if (gval < 0.0) gval = 0.0;
	  if (bval > 255.0) bval = 255.0;
	  else if (bval < 0.0) bval = 0.0;
	*/
	if (rbuf) *(rbuf + x) = 255.0 * Hue_2_RGB(val1, val2, hval + (1.0 / 3.0));
	if (gbuf) *(gbuf + x) = 255.0 * Hue_2_RGB(val1, val2, hval);
	if (bbuf) *(bbuf + x) = 255.0 * Hue_2_RGB(val1, val2, hval - (1.0 / 3.0));
      }
    }
    if (rbuf) rbuf += rpitch;
    if (gbuf) gbuf += gpitch;
    if (bbuf) bbuf += bpitch;
    hbuf += hpitch;
    sbuf += spitch;
    lbuf += lpitch;
  }
}
