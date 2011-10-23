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
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include <pixmap/floatmap.h>

floatmap_t *floatmap_new(int w, int h)
{
  int pitch = WIDTHBYTES(w*sizeof(float)*8);
  floatmap_t *m;

  m = (floatmap_t *)malloc(sizeof(*m)+h*pitch);
  assert(m);
  if (m == NULL) goto __error_return__;
  m->header.width = w;
  m->header.height = h;
  m->header.pitch = pitch;
  m->buffer = (float *)((unsigned char *)m+sizeof(*m));
  memset(m->buffer, 0, h*pitch);
  return m;
 __error_return__:;
  return NULL;
}

void floatmap_destroy(floatmap_t *m)
{
  free(m);
}

floatmap_t *floatmap_clone(void *p)
{
  return floatmap_new(((maphdr_t *)p)->width, ((maphdr_t *)p)->height);
}
