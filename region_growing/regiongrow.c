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

// Reference by 
// region growing: A New Approach - S. A. Hojjatoleslami and J. Kittler */
#include <string.h>
#include <common.h>
#include <dwordmap.h>
#include <bytemap.h>
#include <bitmap.h>
#include <regiongrow.h>

/* Reference by 
   region growing: A New Approach - S. A. Hojjatoleslami and J. Kittler
   region growing is processed in the limit of area, average contrast, and peripheral contrast.
*/
int region_growing_by_contrast(bitmap_t *region, bytemap_t *image, neighbor_t type, int xseed, int yseed, int maxcount, real_t maxac, real_t maxpc)
{
  int *mark;
  int count, x, y, w, h, neighbor;
  int xmin, xmax, ymin, ymax;
  int i, j, k, pixel1, pixel2;
  int *xsave, *ysave;
  uint8_t val, maxval;

  real_t *average_contrast;
  real_t *peripheral_contrast;
  
  real_t current_region; // summation of gray value in current region
  real_t internal_boundary; // summation of gray value in internal boundary
  real_t current_boundary; // summation of gray value in current boundary

  const int dx[8] = { 0,  1,  0, -1, -1,  1,  1, -1};
  const int dy[8] = {-1,  0,  1,  0, -1, -1,  1,  1};
  
  assert(region);
  assert(image);
  assert(bitmap_get_width(region) == bytemap_get_width(image));
  assert(bitmap_get_height(region) == bytemap_get_height(image));

  w = bytemap_get_width(image);
  h = bytemap_get_height(image);

  mark = (int *)calloc(w * h, sizeof(*mark));
  assert(mark);
  memset(mark, 0, w * h * sizeof(*mark));

  xsave = (int *)calloc(w * h, sizeof(*xsave));
  assert(xsave);
  ysave = (int *)calloc(w * h, sizeof(*ysave));
  assert(ysave);
  average_contrast = (real_t *)calloc(w * h, sizeof(average_contrast));
  assert(average_contrast);
  peripheral_contrast = (real_t *)calloc(w * h, sizeof(peripheral_contrast));
  assert(peripheral_contrast);

  if (type == NEIGHBOR_4) neighbor = 4;
  else if (type == NEIGHBOR_8) neighbor = 8;
  else assert(0);

  xmin = xmax = xseed;
  ymin = ymax = yseed;

  mark[yseed * w + xseed] = 1;
  bitmap_set_value(xseed, yseed, region);
  xsave[0] = xseed;
  ysave[0] = yseed;
  current_region = bytemap_get_value(xseed, yseed, image);
  count = 1;

  while (1) {
    // update the region in consideration
    xmin = min(xmin, max(xsave[count-1] - 1, 0));
    xmax = max(xmax, min(xsave[count-1] + 1, w - 1));
    ymin = min(ymin, max(ysave[count-1] - 1, 0));
    ymax = max(ymax, min(ysave[count-1] + 1, h - 1));

    // update the internal boundary and the current boundary
    internal_boundary = current_boundary = 0.0;
    pixel1 = pixel2 = 0;
    for (y = ymin; y <= ymax; y++) {
      if (!(y >= 0 && y < h)) continue;
      for (x = xmin; x <= xmax; x++) {
	if (!(x >= 0 && x < w)) continue;

	val = bytemap_get_value(x, y, image);
        if (mark[y * w + x] == 1) {
          // current region
          for (k = 0; k < neighbor; k++) {
	    j = x + dx[k]; if (!(j >= 0 && j < w)) continue;
	    i = y + dy[k]; if (!(i >= 0 && i < h)) continue;

            if (mark[i * w + j] == 0) {
              // internal boundary
              internal_boundary += val;
              pixel1++;
	      break;
            }
          }
        } else {
          // not current region
          for (k = 0; k < neighbor; k++) {
	    j = x + dx[k]; if (!(j >= 0 && j < w)) continue;
	    i = y + dy[k]; if (!(i >= 0 && i < h)) continue;

            if (mark[i * w + j] == 1) {
              // current boundary
              current_boundary += val;
              pixel2++;
	      break;
            }
          }
        }
      }
    }

    average_contrast[count-1] = abs(current_region / count -
				    current_boundary / pixel2);

    if (average_contrast[count-1] > maxac) break;

    peripheral_contrast[count-1] = abs(internal_boundary / pixel1 -
				       current_boundary / pixel2);

    if (peripheral_contrast[count-1] > maxpc) break;

    if (maxcount > 0 && count >= maxcount) break;

    // Find the highest gray pixel in the neighborhood of the current region
    maxval = 0;
    for (y = ymin; y <= ymax; y++) {
      if (!(y >= 0 && y < h)) continue;
      for (x = xmin; x <= xmax; x++) {
	if (!(x >= 0 && x < w)) continue;

	if (mark[y * w + x] == 1) continue;

	val = bytemap_get_value(x, y, image);
        if (val <= maxval) continue;

        for (k = 0; k < neighbor; k++) {
	  j = x + dx[k]; if (!(j >= 0 && j < w)) continue;
	  i = y + dy[k]; if (!(i >= 0 && i < h)) continue;

          if (mark[i * w + j] == 1) {
            maxval = val;
            xsave[count] = x;
	    ysave[count] = y;
            break;
          }
        }
      }
    }

    /* if such the pixel exists, mark the pixel
     * if not, terminate procedure
     */
    if (maxval == 0) break;

    mark[ysave[count] * w + xsave[count]] = 1;
    bitmap_set_value(xsave[count], ysave[count], region);
    current_region += maxval;
    count++;
  }

  free(peripheral_contrast);
  free(average_contrast);
  free(ysave);
  free(xsave);
  free(mark);

  return count;
}
