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
#include <string.h>
#include <assert.h>

#include <common.h>
#include <labeling/labeling.h>
#include <buffering/dlink.h>
#include <geometry/point.h>

static const int dc[4] = { -1,  0, -1,  1};
static const int dr[4] = {  0, -1, -1, -1};

static void label_info_insert(point_t *p, int pixels, label_info_t *info)
{
  dlink_t *link;

  assert(p);
  assert(info);

  link = dlink_new();
  point_inc_ref(p);
  link->object = (void *)p;
  link->spare = (void *)pixels;
  dlist_insert(link, info);
}

static point_t *label_info_extract(int *pixels, label_info_t *info)
{
  dlink_t *link;
  point_t *p;

  assert(pixels);
  assert(info);

  link = dlist_extract((dlist_t *)info);
  *pixels = (int)link->spare;
  p = (point_t *)link->object;
  point_dec_ref(p);
  dlink_destroy(link);

  return p;
}

point_t *label_info_glimpse(int *pixels,int i, label_info_t *info)
{
  dlink_t *link;
  point_t *p;

  assert(pixels);
  assert(info);

  link = dlist_glimpse(i, (dlist_t *)info);
  *pixels = (int)link->spare;
  p = (point_t *)link->object;

  return p;
}

void label_info_clear(label_info_t *info)
{
  dlink_t *link;

  assert(info);

  while (info->count > 0) {
    link = dlist_pop((dlist_t *)info);
    point_destroy((point_t *)link->object);
    dlink_destroy(link);
  }
  //dlist_destroy((dlist_t *)info);
}

void label_info_destroy(label_info_t *info)
{
  dlink_t *link;

  assert(info);

  while (info->count > 0) {
    link = dlist_pop((dlist_t *)info);
    point_destroy((point_t *)link->object);
    dlink_destroy(link);
  }
  dlist_destroy((dlist_t *)info);
}

int labeling(dwordmap_t *labelmap, label_info_t *labelinfo, bitmap_t *bin, neighbor_t type)
{
  int i, j, k, c, r, w, h;
  int index, cnt, neighbor;
  int32_t *merge, *label;
  int32_t n, m, save[4];
  real_t *xsum, *ysum;
  int *pixels;
  point_t *p;

  assert(labelmap);
  assert(bin);
  assert(dwordmap_get_width(labelmap) == bitmap_get_width(bin));
  assert(dwordmap_get_height(labelmap) == bitmap_get_height(bin));

  w = dwordmap_get_width(labelmap);
  h = dwordmap_get_height(labelmap);

  merge = (int32_t *)malloc(w * h * sizeof(int32_t));
  assert(merge);
  label = (int32_t *)malloc(w * h * sizeof(int32_t));
  assert(label);

  if (labelinfo) {
    xsum = (real_t *)malloc(w * h * sizeof(real_t));
    assert(xsum);
    ysum = (real_t *)malloc(w * h * sizeof(real_t));
    assert(ysum);
    pixels = (int *)malloc(w * h * sizeof(int));
    assert(pixels);
    memset(xsum, 0, w * h * sizeof(real_t));
    memset(ysum, 0, w * h * sizeof(real_t));
    memset(pixels, 0, w * h * sizeof(int));
  }

  if (type == NEIGHBOR_4) neighbor = 2;
  else neighbor = 4;

  dwordmap_fill(labelmap, 0, 0, w, h, UNLABEL);

  index = 0;
  for (r = 0; r < h; r++) {
    for (c = 0; c < w; c++) {
      if (bitmap_isset(bin, c, r)) { // object
	n = UNLABEL;
	cnt = 0;
	// examine connected components previewously labelled
	for (k = 0; k < neighbor; k++) {
	  j = c + dc[k]; if (!(j >= 0 && j < w)) continue;
	  i = r + dr[k]; if (!(i >= 0 && i < h)) continue;
	  if (bitmap_isset(bin, j, i)) { // object
	    m = dwordmap_get_value(labelmap, j, i);
	    assert(m != UNLABEL);
	    save[cnt++] = m;
	    // find asendant label 
	    while (merge[m] != UNLABEL) m = merge[m];
	    assert(m != UNLABEL);
	    assert(merge[m] == UNLABEL);
	    // if lowest value then save it
	    if (n == UNLABEL || m < n) { n = m; }
	  }
	}
	if (n == UNLABEL) { // new one
	  /* Not exist neighborhoods containing old labels,
	     so new labeling occurred */
	  dwordmap_put_value(index, labelmap, c, r);
	  merge[index] = UNLABEL;
	  if (labelinfo) {
	    xsum[index] = c;
	    ysum[index] = r;
	    pixels[index] = 1;
	  }
	  index++;
	} else {
	  // merge to the group with minimum label number
	  dwordmap_put_value(n, labelmap, c, r);
	  assert(merge[n] == UNLABEL);
	  if (labelinfo) {
	    xsum[n] += c;
	    ysum[n] += r;
	    pixels[n]++;
	  }
	  // update the merge of other connected components
	  for (i = 0; i < cnt; i++) {
	    if (save[i] == n) continue;
	    // label conneted component to asendant value
	    k = merge[save[i]];
	    while (k != UNLABEL) {
	      m = merge[k];
	      if (k != n) merge[k] = n;
	      k = m;
	    }
	    merge[save[i]] = n;
	  }
	}
      } else { // background
	dwordmap_put_value(UNLABEL, labelmap, c, r);
      }
    }
  }

  // reassign merging table  to label
  for (cnt = 0, i = 0; i < index; i++) {
    n = merge[i];
    if (n == UNLABEL) {
      label[i] = cnt++;
      continue;
    }
    // find asendant label
    while (merge[n] != UNLABEL) n = merge[n];
    assert(n != UNLABEL);
    merge[i] = n;
    if (labelinfo) {
      xsum[n] += xsum[i]; xsum[i] = 0;
      ysum[n] += ysum[i]; ysum[i] = 0;
      pixels[n] += pixels[i]; pixels[i] = 0;
    }
  }

  for (r = 0; r < h; r++) {
    for (c = 0; c < w; c++) {
      i = dwordmap_get_value(labelmap, c, r);
      if (i != UNLABEL) {
	if (merge[i] != UNLABEL) i = merge[i];
	assert(merge[i] == UNLABEL);
	n = label[i];
	dwordmap_put_value(n, labelmap, c, r);
      }
    }
  }

  if (labelinfo) {
    for (i = 0; i < index; i++) {
      if (merge[i] == UNLABEL) {
	//printf("%d: xsum %lf, ysum %lf, pixels %d\n", label[i], xsum[i], ysum[i], pixels[i]);
	p = point_new_and_set(xsum[i]/pixels[i], ysum[i]/pixels[i], 0);
	label_info_insert(p, pixels[i], labelinfo);
	point_dec_ref(p);
      }
    }
    free(pixels);
    free(ysum);
    free(xsum);
  }

  free(label);
  free(merge);

  return cnt;
}

int labeling_grep_big_blob(bitmap_t *q, bitmap_t *p)
{
  int w, h, i, j;
  int n, m, area, iarea;
  label_info_t *labelinfo;
  dwordmap_t *labelmap;
  //point_t *point;

  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  w = bitmap_get_width(p);
  h = bitmap_get_height(p);
  labelmap = dwordmap_new(w, h);
  labelinfo = label_info_new();

  n = labeling(labelmap, labelinfo, p, NEIGHBOR_8);
  //printf("label %d\n", n);
  if (n > 0) {
    for (area = 0, i = 0; i < n; i++) {
      label_info_glimpse(&m, i, labelinfo);
      if (m > area) {
	iarea = i;
	area = m;
      }
    }
    //printf("area %d\n", area);
    for (i = 0; i < h; i++) {
      for (j = 0; j < w; j++) {
	if (dwordmap_get_value(labelmap, j, i) == iarea) {
	  bitmap_set_value(q, j, i);
	} else {
	  bitmap_reset_value(q, j, i);
	}
      }
    }
    //unlabeling(labelinfo);
    /*
    while (label_info_get_count(labelinfo) > 0) {
      point = label_info_pop(&m, labelinfo);
      point_destroy(point);
    }
    */
  }
  label_info_destroy(labelinfo);
  dwordmap_destroy(labelmap);

  return area;
}

int labeling_read_area_sort(int *label, int len, label_info_t *labelinfo)
{
  int i, j, k, cnt;
  int area, temp;

  assert(label);
  assert(labelinfo);

  cnt = 0;
  for (i = 0; i < label_info_get_count(labelinfo); i++) {
    label_info_glimpse(&area, i, labelinfo);
    for (j = 0; j < cnt; j++) {
      label_info_glimpse(&temp, label[j], labelinfo);
      if (temp < area) break;
    }
    cnt = min(cnt+1, len);
    if (j >= cnt) continue;
    for (k = cnt-2; k >= j; k--) label[k+1] = label[k];
    label[j] = i;
  }

  return cnt;
}

void labeling_fill_holes(bitmap_t *q, bitmap_t *p)
{
  int w, h;
  bitmap_t *tmp;

  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  w = bitmap_get_width(p);
  h = bitmap_get_height(p);
  tmp = bitmap_new(w, h);
  bitmap_copy(tmp, 0, 0, p, 0, 0, w, h);
  bitmap_not(tmp);
  labeling_grep_big_blob(q, tmp);
  bitmap_not(q);
  bitmap_destroy(tmp);
}

/*
void unlabeling(label_info_t *info)
{
  int pixels;
  point_t *p;

  assert(info);

  while (info->count > 0) {
    p = label_info_extract(&pixels, info);
    point_destroy(p);
  }  
}
*/
