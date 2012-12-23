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
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include <common.h>
#include <geometry/point_list.h>
#include <pixmap/dwordmap.h>
#include <labeling/chaincode.h>

/* 3 2 1 *
 * 4   0 *
 * 5 6 7 */

static int dx[8] = {+1, +1,  0, -1, -1, -1,  0, +1};
static int dy[8] = { 0, -1, -1, -1,  0, +1, +1, +1};

void chaincode_destroy(chaincode_t *chain)
{
  assert(chain);

  if (chain->spare)
    point_destroy((point_t *)chain->spare);

  while (chain->count > 0) {
    dlink_destroy(dlist_pop(chain));
  }

  dlist_destroy(chain);
}

/*
void chaincode_delete(chaincode_t *chain)
{
  assert(chain);

  if (chain->spare) {
    point_destroy((point_t *)chain->spare);
    chain->spare = NULL;
  }

  while (chain->count > 0) {
    dlink_destroy(dlist_pop(chain));
  }
}
*/

int chaincoding_on_dwordmap(chaincode_t *chain, dwordmap_t *image, long label)
{
  int found = 0;
  int x, y, w, h, pitch;
  int index, i, j, k;
  int32_t *buf;
  dlink_t *a;
	
  assert(chain);
  assert(image);

  w = dwordmap_get_width(image);
  h = dwordmap_get_height(image);

  // look for starting pixel
  buf = (int32_t *)dwordmap_get_buffer(image);
  pitch = dwordmap_get_pitch(image) / sizeof(*buf);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      if (*(buf + x) == label) {
	chain->spare = (void *)point_new_and_set(x, y, 0);
	found = 1;
	goto __next__;
      }
    }
    buf += pitch;
  }

 __next__:;

  if (!found) return -1; // nothing at all

  index = 4; // just right direction

  do {
    // locate the next position
    found = 0;
    for (k = index + 1; k < index + 8; k++) {
      j = x + dx[k % 8];
      if (!(j >= 0 && j < w)) continue;
      i = y + dy[k % 8];
      if (!(i >= 0 && i < h)) continue;
      if (dwordmap_get_value(image, j, i) == label) {
	//if (DWORDMAP_IS_EQUAL(label, image, j, i)) {
	// find sweeping pixel
	found = 1;
	break;
      }
    }

    if (!found) return -1;
	
    x = j; y = i;
    a = dlink_new();
    a->object = (void *)(k % 8);
    dlist_insert(a, chain);
    //chain->codebook[(chain->n)++] = k%8;
    index = (k + 5) % 8;
  } while (!(x == ((point_t *)chain->spare)->x &&
	     y == ((point_t *)chain->spare)->y));

  return 0;
}

int chaincoding_on_bitmap(chaincode_t *chain, bitmap_t *image)
{
  int found = 0;
  int x, y, w, h, pitch;
  int index, i, j, k;
  unsigned char *buf;
  dlink_t *a;

  assert(chain);
  assert(image);

  w = bitmap_get_width(image);
  h = bitmap_get_height(image);

  // look for starting pixel
  buf = bitmap_get_buffer(image);
  pitch = bitmap_get_pitch(image) / sizeof(*buf);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      if ((*(buf + (x >> 3))) & (1 << (x % 8))) {
	chain->spare = (void *)point_new_and_set(x, y, 0);
	found = 1;
	goto __next__;
      }
    }
    buf += pitch;
  }

 __next__:;

  if (!found) return -1; // nothing at all

  index = 4; // just right direction

  do {
    /* locate the next position */
    found = 0;
    for (k = index + 1; k < index + 8; k++) {
      j = x + dx[k % 8];
      if (!(j >= 0 && j < w)) continue;
      i = y + dy[k % 8];
      if (!(i >= 0 && i < h)) continue;
      if (bitmap_isset(image, j, i)) {
	// find sweeping pixel
	found = 1;
	break;
      }
    }

    if (!found) return -1;
		
    x = j;
    y = i;
    a = dlink_new();
    a->object = (void *)(k%8);
    dlist_insert(a, chain);
    //chain->codebook[(chain->n)++] = k%8;
    index = (k + 5) % 8;
  } while (!(x == ((point_t *)chain->spare)->x &&
	     y == ((point_t *)chain->spare)->y));

  return 0;
}

/* pixel_xlen : actual horizontal length contained by one pixel
   pixel_ylen : actual vertical length contained by one pixel

   3 2 1
   4   0
   5 6 7   */
real_t chaincode_compute_perimeter(chaincode_t *chain, real_t pixel_xlen, real_t pixel_ylen)
{
  real_t perimeter;
  dlink_t *a;

  assert(chain);

  for (a = chain->tail->next; a != chain->head; a = a->next) {
    switch ((int)a->object) {
    case 0: perimeter += pixel_xlen; break;
    case 4: perimeter += pixel_xlen; break;
    case 2: perimeter += pixel_ylen; break;
    case 6: perimeter += pixel_ylen; break;
    default: perimeter += sqrt(sqr(pixel_xlen)+sqr(pixel_ylen)); break;
    }
  }

  return perimeter;
}

/* pixel_xlen : actual horizontal length contained by one pixel
   pixel_ylen : actual vertical length contained by one pixel
   3 2 1
   4   0
   5 6 7   */
real_t chaincode_compute_area(chaincode_t *chain, real_t pixel_xlen, real_t pixel_ylen)
{
  real_t area;
  int x, y;
  dlink_t *a;

  assert(chain);

  area = 0;
  x = ((point_t *)chain->spare)->x;
  y = ((point_t *)chain->spare)->y;

  for (a = chain->tail->next; a != chain->head; a = a->next) {
    switch ((int)a->object) {
    case 0: area += (double)y*pixel_ylen*pixel_xlen; x++; break;
    case 1: area += ((double)y-0.5)*pixel_ylen*pixel_xlen; x++; y--; break;
    case 2: y--; break;
    case 3: area -= ((double)y-0.5)*pixel_ylen*pixel_xlen; x--; y--; break;
    case 4: area -= (double)y*pixel_ylen*pixel_xlen; x--; break;
    case 5: area -= ((double)y+0.5)*pixel_ylen*pixel_xlen; x--; y++; break;
    case 6: y++; break;
    case 7: area += ((double)y+0.5)*pixel_ylen*pixel_xlen; x++; y++; break;
    }
  }

  return area;
}

int chaincode_create_point_list(point_list_t *list, chaincode_t *chain)
{
  int x, y, n;
  dlink_t *a;
  point_t *p;

  assert(list);
  assert(chain);

  x = ((point_t *)chain->spare)->x;
  y = ((point_t *)chain->spare)->y;
  p = point_new_and_set(x, y, 0);
  point_list_insert(p, list);
  point_dec_ref(p);

  for (a = chain->tail->next; a->next != chain->head; a = a->next) {
    n = (int)a->object;
    x += dx[n];
    y += dy[n];
    p = point_new_and_set(x, y, 0);
    point_list_insert(p, list);
    point_dec_ref(p);
  }

  return list->count;
}

int dwordmap_strip_shell(dwordmap_t *pixmap, long label, int depth)
{
  int i, n;
  int x, y;
  chaincode_t *chain;
  dlink_t *a;

  assert(pixmap);
  assert(depth > 0);

  for (i = 0; i < depth; i++) {
    chain = chaincode_new();
    if (chaincoding_on_dwordmap(chain, pixmap, label) < 0)
      break;
    x = ((point_t *)chain->spare)->x;
    y = ((point_t *)chain->spare)->y;
    dwordmap_put_value(-1, pixmap, x, y);
    //DWORDMAP_PUT(-1, pixmap, x, y);
    for (a = chain->tail->next; a->next != chain->head; a = a->next) {
      //for (j = 0; j < chain->n-1; j++) {
      n = (int)a->object;
      x += dx[n];
      y += dy[n];
      dwordmap_put_value(-1, pixmap, x, y);
      //DWORDMAP_PUT(-1, pixmap, x, y);
    }
    //chaincode_delete(chain);
    chaincode_destroy(chain);
  }

  return i;
}

int bitmap_strip_shell(bitmap_t *pixmap, int depth)
{
  int i, n;
  int x, y;
  chaincode_t *chain;
  dlink_t *a;

  assert(pixmap);
  assert(depth > 0);

  for (i = 0; i < depth; i++) {
    chain = chaincode_new();
    if (chaincoding_on_bitmap(chain, pixmap) < 0) break;
    //printf("chaincoding complete\n");
    x = ((point_t *)chain->spare)->x;
    y = ((point_t *)chain->spare)->y;
    //x = chain->x;
    //y = chain->y;
    bitmap_reset_value(pixmap, x, y);
    //BITMAP_RESET(pixmap, x, y);
    //printf("delete start point\n");
    //DWORDMAP_PUT(-1, pixmap, x, y);
    for (a = chain->tail->next; a != chain->head; a = a->next) {
      //for (j = 0; j < chain->n-1; j++) {      
      n = (int)a->object;
      x += dx[n];
      y += dy[n];
      bitmap_reset_value(pixmap, x, y);
      //BITMAP_RESET(pixmap, x, y);
      //printf("delete shell\n");
      //DWORDMAP_PUT(-1, pixmap, x, y);
    }
    //chaincode_delete(chain);
    chaincode_destroy(chain);
  }

  return i;
}
