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
#include <common.h>
#include <labeling.h>
#include <assert.h>
#include <dlink.h>
#include <point.h>

#define UNLABEL (-1)

static const int dc[4] = { -1,  0, -1,  1};
static const int dr[4] = {  0, -1, -1, -1};

void label_info_insert(point_t *p, int pixels, label_info_t *labelinfo)
{
  dlink_t *link;

  assert(p);
  assert(labelinfo);

  link = dlink_new();
  point_inc_ref(p);
  //p->reference++;
  link->object = (void *)p;
  link->spare = (void *)pixels;
  dlist_insert(link, labelinfo);
}

point_t *label_info_pop(int *pixels, label_info_t *labelinfo)
{
  dlink_t *link;
  point_t *p;

  assert(pixels);
  assert(labelinfo);

  link = dlist_pop((dlist_t *)labelinfo);
  *pixels = (int)link->spare;
  p = (point_t *)link->object;
  //p->reference--;
  point_dec_ref(p);
  dlink_destroy(link);

  return p;
}

point_t *label_info_glimpse(int *pixels,int i, label_info_t *label_info)
{
  dlink_t *link;
  point_t *p;

  assert(pixels);
  assert(label_info);

  link = dlist_glimpse(i, (dlist_t *)label_info);
  *pixels = (int)link->spare;
  p = (point_t *)link->object;

  return p;
}

void label_info_delete(label_info_t *labelinfo)
{
  dlink_t *link;

  assert(labelinfo);

  while (labelinfo->count > 0) {
    link = dlist_pop((dlist_t *)labelinfo);
    point_dec_ref((point_t *)link->object);
    point_destroy((point_t *)link->object);
    dlink_destroy(link);
  }
}

void label_info_destroy(label_info_t *labelinfo)
{
  dlink_t *link;

  assert(labelinfo);

  while (labelinfo->count > 0) {
    link = dlist_pop((dlist_t *)labelinfo);
    point_destroy((point_t *)link->object);
    dlink_destroy(link);
  }
  dlist_destroy((dlist_t *)labelinfo);
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

  index = 0;
  for (r = 0; r < h; r++) {
    for (c = 0; c < w; c++) {
      if (bitmap_isset(c, r, bin)) { // object
	n = UNLABEL;
	cnt = 0;
	// examine connected components previewously labelled
	for (k = 0; k < neighbor; k++) {
	  j = c + dc[k]; if (!(j >= 0 && j < w)) continue;
	  i = r + dr[k]; if (!(i >= 0 && i < h)) continue;
	  if (bitmap_isset(j, i, bin)) { // object
	    m = dwordmap_get_value(j, i, labelmap);
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
	  dwordmap_put_value(index, c, r, labelmap);
	  merge[index] = UNLABEL;
	  if (labelinfo) {
	    xsum[index] = c;
	    ysum[index] = r;
	    pixels[index] = 1;
	  }
	  index++;
	} else {
	  // merge to the group with minimum label number
	  dwordmap_put_value(n, c, r, labelmap);
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
	dwordmap_put_value(UNLABEL, c, r, labelmap);
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
      i = dwordmap_get_value(c, r, labelmap);
      if (i != UNLABEL) {
	if (merge[i] != UNLABEL) i = merge[i];
	assert(merge[i] == UNLABEL);
	n = label[i];
	dwordmap_put_value(n, c, r, labelmap);
      }
    }
  }

  if (labelinfo) {
    for (i = 0; i < index; i++) {
      if (merge[i] == UNLABEL) {
	//printf("%d: xsum %lf, ysum %lf, pixels %d\n", label[i], xsum[i], ysum[i], pixels[i]);
	p = point_new_and_assign(xsum[i]/pixels[i], ysum[i]/pixels[i], 0);
	label_info_insert(p, pixels[i], labelinfo);
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

int label_grep_largest_blob(bitmap_t *q, bitmap_t *p)
{
  int w, h, i, j;
  int n, m, area, iarea;
  label_info_t *labelinfo;
  dwordmap_t *labelmap;
  point_t *point;

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
	if (dwordmap_get_value(j, i, labelmap) == iarea) {
	  bitmap_set_value(j, i, q);
	} else {
	  bitmap_reset_value(j, i, q);
	}
      }
    }
    while (label_info_get_count(labelinfo) > 0) {
      point = label_info_pop(&m, labelinfo);
      point_destroy(point);
    }
  }
  label_info_destroy(labelinfo);
  dwordmap_destroy(labelmap);

  return area;
}

int label_get_sort_by_area(int *label, int len, label_info_t *labelinfo)
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

#if 0
int labeling(dwordmap_t *labelmap, matrix_t *centroid, vector_t *area, bitmap_t *bin, neighbor_t type)
{
  double *csum = NULL, *rsum = NULL;
  int *count = NULL, *merge, *label, save[4];
  int c, r, i, j, k, n, m, index, cnt;
  unsigned char *binbuf;
  long *labelbuf;
	
  assert(labelmap);
  assert(bin);
  assert(dwordmap_get_width(labelmap) == bitmap_get_width(bin));
  assert(dwordmap_get_height(labelmap) == bitmap_get_height(bin));
  //assert((type == NEIGHBOR_4) || (type == NEIGHBOR_8));
	
  if (centroid) {
    assert(MATRIX_COLUMNS(centroid) >= bitmap_get_width(bin)*bitmap_get_height(bin));
    assert(MATRIX_ROWS(centroid) >= 2);
    csum = (double *)malloc(bitmap_get_width(bin)*bitmap_get_height(bin)*sizeof(*csum));
    rsum = (double *)malloc(bitmap_get_width(bin)*bitmap_get_height(bin)*sizeof(*rsum));
    count = (int *)malloc(bitmap_get_width(bin)*bitmap_get_height(bin)*sizeof(*count));
    assert(csum);
    assert(rsum);
    assert(count);
    memset(csum, 0, bitmap_get_width(bin)*bitmap_get_height(bin)*sizeof(*csum));
    memset(rsum, 0, bitmap_get_width(bin)*bitmap_get_height(bin)*sizeof(*csum));
    memset(count, 0, bitmap_get_width(bin)*bitmap_get_height(bin)*sizeof(*count));
  }

  if (area) {
    assert(VECTOR_LENGTH(area) >= bitmap_get_width(bin)*bitmap_get_height(bin));
    if (!count) {
      count = (int *)malloc(bitmap_get_width(bin)*bitmap_get_height(bin)*sizeof(*count));
      assert(count);
      memset(count, 0, bitmap_get_width(bin)*bitmap_get_height(bin)*sizeof(*count));
    }
  }
	
  merge = (int *)malloc(bitmap_get_width(bin)*bitmap_get_height(bin)*sizeof(int));
  label = (int *)malloc(bitmap_get_width(bin)*bitmap_get_height(bin)*sizeof(int));
  assert(merge);
  assert(label);

  /*
  // for debugging
  for (i = 0; i < p->header.height; i++) {
  for (j = 0; j < p->header.width; j++) {
  if (*(p->buffer+i*p->header.pitch+(j>>3))&(1<<(j%8))) { // object
  printf("*");
  } else { // background
  printf("_");
  }
  }
  printf("\n");
  }
  */

  index = 0;
  binbuf = bin->buffer;
  labelbuf = labelmap->buffer;
  for (r = 0; r < bitmap_get_height(bin); r++) {
    for (c = 0; c < bitmap_get_width(bin); c++) {
      if (*(binbuf+(c>>3))&(1<<(c%8))) { // object
	n = UNLABEL; cnt = 0;
	// examine connected components previewously labelled
	for (k = 0; ((type == NEIGHBOR_4) && (k < 2)) || ((type == NEIGHBOR_8) && (k < 4)); k++) {
	  i = r+dr[k]; if (!(i >= 0 && i < bitmap_get_height(bin))) continue;
	  j = c+dc[k]; if (!(j >= 0 && j < bitmap_get_width(bin))) continue;
	  if (bitmap_isset(j, i, bin)) { // object
	    m = DWORDMAP_GET(labelmap, j, i);
	    assert(m != UNLABEL);
	    save[cnt++] = m;
	    // find asendant label 
	    while (merge[m] != UNLABEL) m = merge[m];
	    assert(m != UNLABEL);
	    assert(merge[m] == UNLABEL);
	    // if lowest value then save it
	    if ((n == UNLABEL) || (n > m)) { n = m; }
	  }
	}
	if (n == UNLABEL) { // new one
	  /* not exist neighborhoods containing old labels, so new labeling occurred */
	  *(labelbuf+c) = index;
	  merge[index] = UNLABEL;
	  if (count) count[index] = 1;
	  if (csum) csum[index] = c;
	  if (rsum) rsum[index] = r;
	  index++;
	} else {
	  /* merge to the group with minimum label number */
	  *(labelbuf+c) = n;
	  assert(merge[n] == UNLABEL);
	  if (count) count[n]++;
	  if (csum) csum[n] += c;
	  if (rsum) rsum[n] += r;

	  /* update the merge of other connected components */
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
	*(labelbuf+c) = UNLABEL;
      }
    }
    /*
    // for debugging
    for (i = 0; i < q->header.height; i++) {
    for (j = 0; j < q->header.width; j++) {
    printf("%d", *(q->buffer+i*q->header.pitch/sizeof(long)+j));
    }
    printf("\n");
    }
    */
    // update line buffer to next line buffer
    binbuf += bin->header.pitch;
    labelbuf += labelmap->header.pitch/sizeof(long);
  }

  /* reassign merging table  to label */
  for (cnt = 0, i = 0; i < index; i++) {
    n = merge[i];
    if (n == UNLABEL) { label[i] = cnt++; continue; }
    // find asendant label
    while (merge[n] != UNLABEL) n = merge[n];
    assert(n != UNLABEL);
    merge[i] = n;
    if (count) { count[n] += count[i]; count[i] = 0; }
    if (csum) { csum[n] += csum[i]; csum[i] = 0; }
    if (rsum) { rsum[n] += rsum[i]; rsum[i] = 0; }
  }

  labelbuf = labelmap->buffer;
  for (r = 0; r < DWORDMAP_HEIGHT(labelmap); r++) {
    for (c = 0; c < DWORDMAP_WIDTH(labelmap); c++) {
      i = *(labelbuf+c);
      if (i != UNLABEL) {
	if (merge[i] != UNLABEL) i = merge[i];
	assert(merge[i] == UNLABEL);

	n = label[i];
	*(labelbuf+c) = n;
	if (centroid) {
	  //MATRIX_PUT_REAL(csum[i]/(double)count[i], centroid, n, 0);
	  //MATRIX_PUT_REAL(rsum[i]/(double)count[i], centroid, n, 1);
	  MATRIX_PUT(csum[i]/(double)count[i], centroid, n, 0);
	  MATRIX_PUT(rsum[i]/(double)count[i], centroid, n, 1);
	}
	if (area) {
	  VECTOR_PUT(count[i], area, n);
	  //VECTOR_PUT_REAL(count[i], area, n);
	}
      }
    }
    labelbuf += labelmap->header.pitch/sizeof(long);
  }
  /*
    for (i = 0; i < q->header.height; i++) {
    for (j = 0; j < q->header.width; j++) {
    printf("%d", *(q->buffer+i*q->header.pitch/sizeof(long)+j));
    }
    printf("\n");
    }
  */
  free(label);
  free(merge);
	
  if (area && count) free(count);
	
  if (centroid) {
    if (count) free(count);
    free(rsum);
    free(csum);
  }
  return cnt;	
}

int label_pickup_by_largest_area(bitmap_t *q, bitmap_t *p)
{
  int i, j, imax, n;
  real_t val;
  vector_t *area;
  dwordmap_t *label;
  uint8_t *qbuf;
  int32_t *buf;

  //FILE *fd = fopen("C:\\work\\ImageTool\\label.txt", "wb");

  assert(q);
  assert(p);
  assert(q->header.width == p->header.width);
  assert(q->header.height == p->header.height);

  area = vector_create(p->header.width*p->header.height, 0);
  label = dwordmap_new(p->header.width, p->header.height);

  n = labeling(label, NULL, area, p, NEIGHBOR_8);
  //printf("n(%d)\n", n);
  if (n == 0) {
    dwordmap_destroy(label);
    vector_delete(area);
    return -1;
  }
	
  for (val = *(area->real), imax = i = 0; i < n; i++) {
    //fprintf(fd, "area(%lf) at label(%d)\n", *(area->real+i), i);
    if (*(area->real+i) > val) {
      val = *(area->real+i);
      imax = i;
    }
  }
  //printf("area(%lf), at %d\n", *(area->real+imax), imax);

  n = 0;
  buf = label->buffer;
  qbuf = q->buffer;
  for (i = 0; i < label->header.height; i++) {
    for (j = 0; j < label->header.width; j++) {
      if (*(buf+j) == imax) {
	n++;
	*(qbuf+(j>>3)) |= 1<<(j%8);
      } else {
	*(qbuf+(j>>3)) &= ~(1<<(j%8));
      }
    }
    //fprintf(fd, "\n");
    buf += label->header.pitch/sizeof(int32_t);
    qbuf += q->header.pitch;
  }
  //fclose(fd);
  //printf("n(%d)\n", n);
  dwordmap_destroy(label);
  vector_delete(area);
  return n;
}

int label_sort_by_area(int *label, int len, vector_t *vect)
{
  int n, i, j, cnt;
  real_t area, tmp;
	
  assert(label);
  assert(len > 0);
  assert(vect);
	
  cnt = 0;
  for (n = 0; n < VECTOR_LENGTH(vect); n++) {
    //area = VECTOR_GET_REAL(vect, n);
    VECTOR_GET(area, vect, n);
    if (area == 0.0) continue;
    // find position
    for (i = 0; i < cnt; i++) {
      VECTOR_GET(tmp, vect, label[i]);
      if (tmp < area) break;
      //if (VECTOR_GET_REAL(vect, label[i]) < area) break;
    }
    cnt = min(cnt+1, len);
    if (i >= cnt) continue;
    for (j = cnt-2; j >= i; j--) label[j+1] = label[j];
    label[i] = n;
  }

  return cnt;
}

void label_fill_holes(bitmap_t *q, bitmap_t *p)
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
  label_pickup_by_largest_area(q, tmp);
  bitmap_not(q);
  bitmap_destroy(tmp);
}
#endif

