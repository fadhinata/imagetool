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
#ifndef __DOUBLEMAP_H__
#define __DOUBLEMAP_H__

#include <pixmap/maphdr.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    maphdr_t header;
    double *buffer;
} doublemap_t;

doublemap_t *doublemap_create(int w, int h);
void doublemap_delete(doublemap_t *m);
doublemap_t *doublemap_clone(void *p);

#ifdef __cplusplus
}
#endif

#endif /* __DOUBLEMAP_H__ */
