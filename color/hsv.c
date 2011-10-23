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

#include <common.h>
#include <pixmap/bytemap.h>
#include <pixmap/floatmap.h>

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,255]
// if s == 0, then h = -1 (undefined)
void rgb2hsv(floatmap_t *h, floatmap_t *s, floatmap_t *v, bytemap_t *r, bytemap_t *g, bytemap_t *b)
{
  int x, y, width, height;
  unsigned char *rbuf, *gbuf, *bbuf;
  float *hbuf, *sbuf, *vbuf;
  float minval, maxval, delta;
  float rval, gval, bval;
  float hval, sval, vval;
  int rpitch, gpitch, bpitch;
  int hpitch, spitch, vpitch;

  assert(h||s||v);
  assert(r);
  assert(g);
  assert(b);

  assert(bytemap_get_width(r) == bytemap_get_width(g));
  assert(bytemap_get_height(r) == bytemap_get_height(g));
  assert(bytemap_get_width(r) == bytemap_get_width(b));
  assert(bytemap_get_height(r) == bytemap_get_height(b));

  if (h) {
    assert(floatmap_get_width(h) == bytemap_get_width(r));
    assert(floatmap_get_height(h) == bytemap_get_height(r));
    hbuf = floatmap_get_buffer(h);
    hpitch = floatmap_get_pitch(h) / sizeof(*hbuf);
  }

  if (s) {
    assert(floatmap_get_width(s) == bytemap_get_width(r));
    assert(floatmap_get_height(s) == bytemap_get_width(r));
    sbuf = floatmap_get_buffer(s);
    spitch = floatmap_get_pitch(s) / sizeof(*sbuf);
  }

  if (v) {
    assert(floatmap_get_width(v) == bytemap_get_width(r));
    assert(floatmap_get_height(v) == bytemap_get_height(r));
    vbuf = floatmap_get_buffer(v);
    vpitch = floatmap_get_pitch(v) / sizeof(*vbuf);
  }
	
  width = bytemap_get_width(r);
  height = bytemap_get_height(r);

  rbuf = bytemap_get_buffer(r);
  rpitch = bytemap_get_pitch(r) / sizeof(*rbuf);

  gbuf = bytemap_get_buffer(g);
  gpitch = bytemap_get_pitch(g) / sizeof(*gbuf);

  bbuf = bytemap_get_buffer(b);
  bpitch = bytemap_get_pitch(b) / sizeof(*bbuf);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      rval = *(rbuf + x);
      gval = *(gbuf + x);
      bval = *(bbuf + x);
      if (rval > gval && rval > bval) {
	maxval = rval;
	minval = min(gval, bval);
      } else {
	if (gval > bval) {
	  maxval = gval;
	  minval = min(rval, bval);
	} else {
	  maxval = bval;
	  minval = min(rval, gval);
	}
      }

      vval = maxval;                // v
      delta = maxval - minval;
      if (maxval != 0) {
	sval = delta / maxval;        // s
	if (rval == maxval) hval = (gval - bval) / delta;        // between yellow & magenta
	else if (gval == maxval) hval = 2 + (bval - rval) / delta;    // between cyan & yellow
	else hval = 4 + (rval - gval) / delta;    // between magenta & cyan
	hval *= 60;                // degrees
	if (hval < 0) hval += 360;
      } else {
	// r = g = b = 0
	// s = 0, v is undefined
	sval = 0;
	hval = -1;
      }
      if (hbuf) *(hbuf + x) = hval;
      if (sbuf) *(sbuf + x) = sval;
      if (vbuf) *(vbuf + x) = vval;
    }
    rbuf += rpitch;
    gbuf += gpitch;
    bbuf += bpitch;
    if (hbuf) hbuf += hpitch;
    if (sbuf) sbuf += spitch;
    if (vbuf) vbuf += vpitch;
  }
}

void hsv2rgb(bytemap_t *r, bytemap_t *g, bytemap_t *b, floatmap_t *h, floatmap_t *s, floatmap_t *v)
{
  int x, y, i, width, height;
  unsigned char *rbuf, *gbuf, *bbuf;
  unsigned char rval, gval, bval;
  float *hbuf, *sbuf, *vbuf;
  float hval, sval, vval;
  float f, p, q, t;
  int rpitch, gpitch, bpitch;
  int hpitch, spitch, vpitch;
	
  assert(r || g || b);
  assert(h);
  assert(s);
  assert(v);
  assert(floatmap_get_width(h) == floatmap_get_width(s));
  assert(floatmap_get_height(h) == floatmap_get_height(s));
  assert(floatmap_get_width(h) == floatmap_get_width(v));
  assert(floatmap_get_height(h) == floatmap_get_height(v));

  if (r) {
    assert(floatmap_get_width(h) == bytemap_get_width(r));
    assert(floatmap_get_height(h) == bytemap_get_height(r));
    rbuf = bytemap_get_buffer(r);
    rpitch = bytemap_get_pitch(r) / sizeof(*rbuf);
  }

  if (g) {
    assert(floatmap_get_width(h) == bytemap_get_width(g));
    assert(floatmap_get_height(h) == bytemap_get_height(g));
    gbuf = bytemap_get_buffer(g);
    gpitch = bytemap_get_pitch(g) / sizeof(*gbuf);
  }

  if (b) {
    assert(floatmap_get_width(h) == bytemap_get_width(b));
    assert(floatmap_get_height(h) == bytemap_get_height(b));
    bbuf = bytemap_get_buffer(b);
    bpitch = bytemap_get_pitch(b) / sizeof(*bbuf);
  }

  width = floatmap_get_width(h);
  height = floatmap_get_height(h);

  hbuf = floatmap_get_buffer(h);
  hpitch = floatmap_get_pitch(h) / sizeof(*hbuf);

  sbuf = floatmap_get_buffer(s);
  spitch = floatmap_get_pitch(s) / sizeof(*sbuf);

  vbuf = floatmap_get_buffer(v);
  vpitch = floatmap_get_pitch(v) / sizeof(*vbuf);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      hval = *(hbuf + x);
      sval = *(sbuf + x);
      vval = *(vbuf + x);

      if (sval == 0.0) {
	//achromatic (gray)
	rval = gval = bval = vval;
      } else {
	hval /= 60;            // sector 0 to 5
	i = (int)floor(hval);
	f = hval - i;            // factorial part of h
	p = vval * (1 - sval);
	q = vval * (1 - sval * f);
	t = vval * (1 - sval * (1 - f));
	switch (i) {
	case 0: rval = vval; gval = t; bval = p; break;
	case 1: rval = q; gval = vval; bval = p; break;
	case 2: rval = p; gval = vval; bval = t; break;
	case 3: rval = p; gval = q; bval = vval; break;
	case 4: rval = t; gval = p; bval = vval; break;
	default: /* case 5: */ rval = vval; gval = p; bval = q; break;
	}
      }
      if (rbuf) {
	/*
	  if (rval > 255) *(rbuf+x) = 255;
	  else if (rval < 0) *(rbuf+x) = 0;
	  else *(rbuf+x) = rval;
	*/
	*(rbuf + x) = rval;
      }
      if (gbuf) {
	/*
	  if (gval > 255) *(gbuf+x) = 255;
	  else if (gval < 0) *(gbuf+x) = 0;
	  else *(gbuf+x) = gval;
	*/
	*(gbuf + x) = gval;
      }
      if (bbuf) {
	/*
	  if (bval > 255) *(bbuf+x) = 255;
	  else if (bval < 0) *(bbuf+x) = 0;
	  else *(bbuf+x) = bval;
	*/
	*(bbuf + x) = bval;
      }
    }
    if (rbuf) rbuf += rpitch;
    if (gbuf) gbuf += gpitch;
    if (bbuf) bbuf += bpitch;
    hbuf += hpitch;
    sbuf += spitch;
    vbuf += vpitch;
  }
}
