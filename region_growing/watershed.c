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
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include <dwordmap.h>
#include <wordmap.h>
#include <bytemap.h>
#include <neighbor.h>
#include <point.h>
#include <dlink.h>
#include <watershed.h>

/* Vincent-Soille watershed algorithm
   Watershed-by-Immersion
   Input: digital gray scale image G = (D, E, im).
   Ouput: labelled watershed image lab on D
*/

#define VINCENT_SOILLE

#define INIT -1 // Initial value of lab image
#define MASK -2 // Initial value at each level
#define WSHED 0 // label of the watershed pixels

#define FICTITIOUS (void *)-1 // fictitious pixel not contained at D

typedef struct { int x, y; } xy_t;

#define fifo_new() dlist_new()
#define fifo_empty(x) (dlist_get_count(x) == 0)
#define fifo_destroy(x) dlist_destroy(x)

static xy_t *fifo_remove(dlist_t *x)
{
  dlink_t *link;
  xy_t *p;

  link = dlist_pop(x);
  p = (xy_t *)link->object;
  dlink_destroy(link);

  return p;
}

static void fifo_add(xy_t *p, dlist_t *x)
{
  dlink_t *link;

  link = dlink_new();
  link->object = (void *)p;
  dlist_insert(link, x);
}

int bytemap_create_watershed(dwordmap_t *labelmap, dlist_t *info, bytemap_t *image, neighbor_t *type)
{
  int x, y, i, j, k, w, h, u, n, neighbor, pitch;
  uint32_t val, label, plabel, label_of_mindist, curlabel;
  uint32_t *histogram, *count, *pixels, count_of_mindist;
  uint32_t dist, curdist, mindist, hmax;
  uint8_t *buf;
  dwordmap_t *distmap;

  dlink_t *link;
  dlist_t *queue;
  real_t *xsum, *ysum;
  xy_t *p, *q, *coord_pool, **sorted_coord_base, **coord_map;
  point_t *point;
  const int dx[8] = { 0,  1,  0, -1, -1,  1,  1, -1};
  const int dy[8] = {-1,  0,  1,  0, -1, -1,  1,  1};

  assert(labelmap);
  assert(image);
  assert(dwordmap_get_width(labelmap) == bytemap_get_width(image));
  assert(dwordmap_get_height(labelmap) == bytemap_get_height(image));

  if (type == NEIGHBOR_4) neighbor = 4;
  else neighbor = 8; // if (type == NEIGHBOR_8) 

  w = bytemap_get_width(image);
  h = bytemap_get_height(image);

  queue = fifo_new();
  distmap = dwordmap_new(w, h);

  if (info) {
    xsum = (real_t *)calloc(w * h, sizeof(*xsum));
    assert(xsum);
    ysum = (real_t *)calloc(w * h, sizeof(*ysum));
    assert(ysum);
    pixels = (uint32_t *)calloc(w * h, sizeof(*pixels));
    assert(pixels);
  }

  // Find the max in 'p' image
  hmax = 0;
  buf = bytemap_get_buffer(image);
  pitch = bytemap_get_pitch(image) / sizeof(*buf);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      val = *(buf + x);
      if (val > hmax) hmax = val;
    }
    buf += pitch;
  }
  //depth++;

  // Construct histogram
  histogram = (uint32_t *)calloc(hmax + 1, sizeof(*histogram));
  assert(histogram);
  memset(histogram, 0, (hmax + 1) * sizeof(*histogram));

  buf = bytemap_get_buffer(image);
  pitch = bytemap_get_pitch(image) / sizeof(*buf);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++)
      histogram[*(buf + x)]++;
    buf += pitch;
  }

  // Construct the array of pixel's coordinate sorted by gray level
  coord_pool = (xy_t *)calloc(w * h, sizeof(*coord_pool));
  assert(coord_pool);
  coord_map = (xy_t **)calloc(w * h, sizeof(*coord_map));
  assert(coord_map);
  sorted_coord_base = (xy_t **)calloc(hmax + 1, sizeof(*sorted_coord_base));
  assert(sorted_coord_base);

  for (n = i = 0; i <= hmax; i++) {
    sorted_coord_base[i] = &(coord_pool[n]);
    n += histogram[i];
  }

  count = (uint32_t *)calloc(hmax + 1, sizeof(*count));
  assert(count);
  memset(count, 0, (hmax + 1) * sizeof(*count));

  buf = bytemap_get_buffer(image);
  pitch = bytemap_get_pitch(image) / sizeof(*buf);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      val = *(buf + x);
      p = (sorted_coord_base[val] + count[val]);
      p->x = x; p->y = y;
      coord_map[y * w + x] = p;
      count[val]++;
    }
    buf += pitch;
  }

  free(count);

  // Initialize the variables
  curlabel = 0;
  dwordmap_fill(labelmap, 0, 0, w, h, INIT);
  dwordmap_fill(distmap, 0, 0, w, h, 0);

  // Initialize the variables
  curlabel = 0;
  dwordmap_fill(labelmap, 0, 0, w, h, INIT);
  dwordmap_fill(distmap, 0, 0, w, h, 0);
  
  // Start flooding
  for (u = 0; u <= hmax; u++) {
    if (histogram[u] == 0) continue;
    /* Pixel processing for the same gray level save pixels in neighbor
       which are already-labeled pixel or watersheded */
    p = sorted_coord_base[u];
    for (n = 0; n < histogram[u]; n++, p++) {
      x = p->x; y = p->y;
      dwordmap_put_value(MASK, x, y, labelmap);
      // for a neighbour of p
      for (k = 0; k < neighbor; k++) {
	j = x + dx[k]; if (!(j >= 0 && j < w)) continue;
	i = y + dy[k]; if (!(i >= 0 && i < h)) continue;
	label = dwordmap_get_value(j, i, labelmap);
	if (!(label == INIT || label == MASK) &&
	    (label > 0 || label == WSHED)) {
	  fifo_add(p, queue);
	  dwordmap_put_value(1, x, y, distmap);
	}
      }
    }

    curdist = 1;
    fifo_add(FICTITIOUS, queue);

    // exntend basins
    // For the saved pixels, label merging or watershed masking process
    while (1) {
      // check delimiter as FICTITIOUS and empty queue,
      p = fifo_remove(queue);
      if (p == FICTITIOUS) {
	if (fifo_empty(queue)) break;
	fifo_add(FICTITIOUS, queue);
	curdist++;
	p = fifo_remove(queue);
      }

      mindist = UINT32_MAX;
      label_of_mindist = 0;
      count_of_mindist = 0;

      // labelling p by inspecting neighbours
      x = p->x; y = p->y;
      plabel = dwordmap_get_value(x, y, labelmap);
      //printf("For connected pixel (%d, %d)\n", x, y);
      //printf("Its neighbors are:\n");
      for (k = 0; k < neighbor; k++) {
	j = x + dx[k]; if (!(j >= 0 && j < w)) continue;
	i = y + dy[k]; if (!(i >= 0 && i < h)) continue;
	//printf("%d: (%d, %d)\n", k, j, i);
	label = dwordmap_get_value(j, i, labelmap);
	dist = dwordmap_get_value(j, i, distmap);

#ifndef VINCENT_SOILLE
	if (!(label == INIT || label == MASK) &&
	    (label > 0 || label == WSHED)) {
	  if (dist < mindist) {
	    mindist = dist;
	    count_of_mindist = 1;
	    if (label > 0) label_of_mindist = label;
	  } else if (dist == mindist) {
	    if (label > 0) {
	      if (!(label_of_mindist == INIT || label_of_mindist == MASK) &&
		  label_of_mindist > 0 && label != label_of_mindist) {
		count_of_mindist++;
	      }
	      label_of_mindist = label;
	    }
	  }
	} else if (label == MASK && dist == 0) {
	  // (j, i) is plateau pixel
	  dwordmap_put_value(curdist + 1, j, i, distmap);
	  fifo_add(coord_map[i * w + j], queue);
	}
      }

      if (mindist < curdist &&
	  !(label_of_mindist == INIT || label_of_mindist == MASK) &&
	  label_of_mindist > 0) {
	if (count_of_mindist == 1 && (plabel == MASK || plabel == WSHED)) {
	  dwordmap_put_value(label_of_mindist, x, y, labelmap);
	  if (info) {
	    xsum[label_of_mindist] += x;
	    ysum[label_of_mindist] += y;
	    pixels[label_of_mindist]++;
	  }
	} else if (count_of_mindist > 1 || plabel != label_of_mindist) {
	  dwordmap_put_value(WSHED, x, y, labelmap);
	}
      } else if (plabel == MASK) {
	dwordmap_put_value(WSHED, x, y, labelmap);
      }
#else
      // belongs to an existing basin or to watersheds
      if (dist < curdist && (!(label == INIT || label == MASK) &&
			     (label > 0 || label == WSHED))) {
	if (label > 0) {
	  if (plabel == MASK || plabel == WSHED) {
	    dwordmap_put_value(label, x, y, labelmap);
	    if (info) {
	      xsum[label] += x;
	      ysum[label] += y;
	      pixels[label]++;
	    }
	  } else if (plabel != label) {
	    dwordmap_put_value(WSHED, x, y, labelmap);
	  }
	} else if (plabel == MASK) {
	  dwordmap_put_value(WSHED, x, y, labelmap);
	}
      } else if (label == MASK && dist == 0) {
	// (j, i) is plateau pixel
	dwordmap_put_value(curdist + 1, j, i, distmap);
	fifo_add(coord_map[i * w + j], queue);
      }
    }
#endif
    }

    // getch();
    // Detect and process new minima at current level
    p = sorted_coord_base[u];
    for (n = 0; n < histogram[u]; n++, p++) {
      x = p->x;
      y = p->y;
      dwordmap_put_value(0, x, y, distmap);
      // p is inside a new minimum
      if (dwordmap_get_value(x, y, labelmap) == MASK) {
	// creaet new label
	curlabel++;
	fifo_add(p, queue);
	dwordmap_put_value(curlabel, x, y, labelmap);
	if (info) {
	  xsum[curlabel] = x;
	  ysum[curlabel] = y;
	  pixels[curlabel] = 1;
	}
	while (!fifo_empty(queue)) {
	  q = fifo_remove(queue);
	  for (k = 0; k < neighbor; k++) {
	    j = q->x + dx[k]; if (!(j >= 0 && j < w)) continue;
	    i = q->y + dy[k]; if (!(i >= 0 && i < h)) continue;
	    if (dwordmap_get_value(j, i, labelmap) == MASK) {
	      fifo_add(coord_map[i * w + j], queue);
	      dwordmap_put_value(curlabel, j, i, labelmap);
              if (info) {
		xsum[curlabel] += j;
		ysum[curlabel] += i;
		pixels[curlabel]++;
	      }
	    }
	  }
	}
      }
    }
  }

  if (info) {
    link = dlink_new();
    link->object = (void *)NULL;
    dlist_insert(link, info);
    for (k = 1; k <= curlabel; k++) {
      point = point_new_and_assign(xsum[k]/(double)pixels[k],
				   ysum[k]/(double)pixels[k], 0);
      point->reference++;
      link = dlink_new();
      link->object = (void *)point;
      dlist_insert(link, info);
    }
  }

  free(sorted_coord_base);
  free(coord_map);
  free(coord_pool);
  free(histogram);

  if (info) {
    free(pixels);
    free(ysum);
    free(xsum);
  }

  dwordmap_destroy(distmap);
  fifo_destroy(queue);

  return curlabel + 1; // maximum label value
}

int wordmap_create_watershed(dwordmap_t *labelmap, dlist_t *info, wordmap_t *image, neighbor_t *type)
{
  int i, j, k, u, n, x, y, w, h, neighbor, pitch;
  uint32_t val, label, plabel, label_of_mindist, curlabel;
  uint32_t *histogram, *count, *pixels, count_of_mindist;
  uint32_t dist, curdist, mindist, hmax;
  uint16_t *buf;
  dwordmap_t *distmap;

  dlink_t *link;
  dlist_t *queue;
  real_t *xsum, *ysum;
  xy_t *p, *q, *coord_pool, **coord_map, **sorted_coord_base;
  point_t *point;
  const int dx[8] = { 0,  1,  0, -1, -1,  1,  1, -1};
  const int dy[8] = {-1,  0,  1,  0, -1, -1,  1,  1};

  assert(labelmap);
  assert(image);
  assert(dwordmap_get_width(labelmap) == wordmap_get_width(image));
  assert(dwordmap_get_height(labelmap) == wordmap_get_height(image));

  if (type == NEIGHBOR_4) neighbor = 4;
  else neighbor = 8;

  w = wordmap_get_width(image);
  h = wordmap_get_height(image);

  queue = fifo_new();
  distmap = dwordmap_new(w, h);

  if (info) {
    xsum = (real_t *)calloc(w * h, sizeof(*xsum));
    //memset(xsum, 0, w * h * sizeof(*xsum));
    ysum = (real_t *)calloc(w * h, sizeof(*ysum));
    //memset(ysum, 0, w * h * sizeof(*ysum));
    pixels = (uint32_t *)calloc(w * h, sizeof(*pixels));
    //memset(pixels, 0, w * h * sizeof(*pixels));
  }

  // Find the max in image
  hmax = 0;
  buf = (uint16_t *)wordmap_get_buffer(image);
  pitch = wordmap_get_pitch(image) / sizeof(*buf);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      val = *(buf + x);
      if (val > hmax) hmax = val;
    }
    buf += pitch;
  }
  //printf("hmax:%d\n", hmax);

  // Construct histogram
  histogram = (uint32_t *)calloc(hmax + 1, sizeof(*histogram));
  memset(histogram, 0, (hmax + 1) * sizeof(*histogram));

  buf = (uint16_t *)wordmap_get_buffer(image);
  pitch = wordmap_get_pitch(image) / sizeof(*buf);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++)
      histogram[*(buf + x)]++;
    buf += pitch;
  }

  // Construct the array of pixel's coordinate sorted by gray level
  coord_pool = (xy_t *)calloc(w * h, sizeof(*coord_pool));
  coord_map = (xy_t **)calloc(w * h, sizeof(*coord_map));
  sorted_coord_base = (xy_t **)calloc(hmax + 1, sizeof(*sorted_coord_base));

  for (n = i = 0; i <= hmax; i++) {
    sorted_coord_base[i] = &(coord_pool[n]);
    n += histogram[i];
  }

  count = (uint32_t *)calloc(hmax + 1, sizeof(*count));
  memset(count, 0, (hmax + 1) * sizeof(*count));

  buf = (uint16_t *)wordmap_get_buffer(image);
  pitch = wordmap_get_pitch(image) / sizeof(*buf);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      val = *(buf + x);
      p = (sorted_coord_base[val] + count[val]);
      p->x = x;
      p->y = y;
      coord_map[y * w + x] = p;
      count[val]++;
    }
    buf += pitch;
  }

  free(count);

  // Initialize the variables
  curlabel = 0;
  dwordmap_fill(labelmap, 0, 0, w, h, INIT);
  dwordmap_fill(distmap, 0, 0, w, h, 0);
  
  // Start flooding
  for (u = 0; u <= hmax; u++) {
    if (histogram[u] == 0) continue;
    /* Pixel processing for the same gray level save pixels in neighbor
       which are already-labeled pixel or watersheded */
    p = sorted_coord_base[u];
    for (n = 0; n < histogram[u]; n++, p++) {
      x = p->x; y = p->y;
      dwordmap_put_value(MASK, x, y, labelmap);
      // for a neighbour of p
      for (k = 0; k < neighbor; k++) {
	j = x + dx[k]; if (!(j >= 0 && j < w)) continue;
	i = y + dy[k]; if (!(i >= 0 && i < h)) continue;
	label = dwordmap_get_value(j, i, labelmap);
	if (!(label == INIT || label == MASK) &&
	    (label > 0 || label == WSHED)) {
	  fifo_add(p, queue);
	  dwordmap_put_value(1, x, y, distmap);
	}
      }
    }

    curdist = 1;
    fifo_add(FICTITIOUS, queue);

    // exntend basins
    // For the saved pixels, label merging or watershed masking process
    while (1) {
      // check delimiter as FICTITIOUS and empty queue,
      p = fifo_remove(queue);
      if (p == FICTITIOUS) {
	if (fifo_empty(queue)) break;
	fifo_add(FICTITIOUS, queue);
	curdist++;
	p = fifo_remove(queue);
      }

      mindist = UINT32_MAX;
      label_of_mindist = 0;
      count_of_mindist = 0;

      // labelling p by inspecting neighbours
      x = p->x; y = p->y;
      plabel = dwordmap_get_value(x, y, labelmap);
      //printf("For connected pixel (%d, %d)\n", x, y);
      //printf("Its neighbors are:\n");
      for (k = 0; k < neighbor; k++) {
	j = x + dx[k]; if (!(j >= 0 && j < w)) continue;
	i = y + dy[k]; if (!(i >= 0 && i < h)) continue;
	//printf("%d: (%d, %d)\n", k, j, i);
	label = dwordmap_get_value(j, i, labelmap);
	dist = dwordmap_get_value(j, i, distmap);

#ifndef VINCENT_SOILLE
	if (!(label == INIT || label == MASK) &&
	    (label > 0 || label == WSHED)) {
	  if (dist < mindist) {
	    mindist = dist;
	    count_of_mindist = 1;
	    if (label > 0) label_of_mindist = label;
	  } else if (dist == mindist) {
	    if (label > 0) {
	      if (!(label_of_mindist == INIT || label_of_mindist == MASK) &&
		  label_of_mindist > 0 && label != label_of_mindist) {
		count_of_mindist++;
	      }
	      label_of_mindist = label;
	    }
	  }
	} else if (label == MASK && dist == 0) {
	  // (j, i) is plateau pixel
	  dwordmap_put_value(curdist + 1, j, i, distmap);
	  fifo_add(coord_map[i * w + j], queue);
	}
      }

      if (mindist < curdist &&
	  !(label_of_mindist == INIT || label_of_mindist == MASK) &&
	  label_of_mindist > 0) {
	if (count_of_mindist == 1 && (plabel == MASK || plabel == WSHED)) {
	  dwordmap_put_value(label_of_mindist, x, y, labelmap);
	  if (info) {
	    xsum[label_of_mindist] += x;
	    ysum[label_of_mindist] += y;
	    pixels[label_of_mindist]++;
	  }
	} else if (count_of_mindist > 1 || plabel != label_of_mindist) {
	  dwordmap_put_value(WSHED, x, y, labelmap);
	}
      } else if (plabel == MASK) {
	dwordmap_put_value(WSHED, x, y, labelmap);
      }
#else
	// belongs to an existing basin or to watersheds
	if (dist < curdist && (!(label == INIT || label == MASK) &&
			       (label > 0 || label == WSHED))) {
	  if (label > 0) {
	    if (plabel == MASK || plabel == WSHED) {
	      dwordmap_put_value(label, x, y, labelmap);
	      if (info) {
		xsum[label] += x;
		ysum[label] += y;
		pixels[label]++;
	      }
	    } else if (plabel != label) {
	      dwordmap_put_value(WSHED, x, y, labelmap);
	    }
	  } else if (plabel == MASK) {
	    dwordmap_put_value(WSHED, x, y, labelmap);
	  }
	} else if (label == MASK && dist == 0) {
	  // (j, i) is plateau pixel
	  dwordmap_put_value(curdist + 1, j, i, distmap);
	  fifo_add(coord_map[i * w + j], queue);
	}
      }
#endif
    }

    // getch();
    // Detect and process new minima at current level
    p = sorted_coord_base[u];
    for (n = 0; n < histogram[u]; n++, p++) {
      x = p->x;
      y = p->y;
      dwordmap_put_value(0, x, y, distmap);
      // p is inside a new minimum
      if (dwordmap_get_value(x, y, labelmap) == MASK) {
	// creaet new label
	curlabel++;
	fifo_add(p, queue);
	dwordmap_put_value(curlabel, x, y, labelmap);
	if (info) {
	  xsum[curlabel] = x;
	  ysum[curlabel] = y;
	  pixels[curlabel] = 1;
	}
	while (!fifo_empty(queue)) {
	  q = fifo_remove(queue);
	  for (k = 0; k < neighbor; k++) {
	    j = q->x + dx[k]; if (!(j >= 0 && j < w)) continue;
	    i = q->y + dy[k]; if (!(i >= 0 && i < h)) continue;
	    if (dwordmap_get_value(j, i, labelmap) == MASK) {
	      fifo_add(coord_map[i * w + j], queue);
	      dwordmap_put_value(curlabel, j, i, labelmap);
              if (info) {
		xsum[curlabel] += j;
		ysum[curlabel] += i;
		pixels[curlabel]++;
	      }
	    }
	  }
	}
      }
    }
  }

  if (info) {
    link = dlink_new();
    link->object = (void *)NULL;
    dlist_insert(link, info);
    for (k = 1; k <= curlabel; k++) {
      point = point_new_and_assign(xsum[k]/(double)pixels[k],
				   ysum[k]/(double)pixels[k], 0);
      point->reference++;
      link = dlink_new();
      link->object = (void *)point;
      dlist_insert(link, info);
    }
  }

  free(sorted_coord_base);
  free(coord_map);
  free(coord_pool);
  free(histogram);

  if (info) {
    free(pixels);
    free(ysum);
    free(xsum);
  }

  dwordmap_destroy(distmap);
  fifo_destroy(queue);

  return curlabel + 1; // maximum label value
}
