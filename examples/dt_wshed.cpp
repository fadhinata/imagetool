#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <common.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <pixmap/dwordmap.h>
#include <drawing/draw_circle.h>
#include <linear_algebra/matrix.h>
#include <convert/convert.h>
#include <geometry/dt.h>
#include <statistics/matrix_statistic.h>
#include <color/gray.h>
#include <region_growing/watershed.h>
#include <geometry/neighbor.h>
#include <interface/screen.h>

void test_distance_transform_and_watershed(int w, int h)
{
  int n;
  bitmap_t *bin;
  bytemap_t *a, *b, *c;
  matrix_t *m;
  dwordmap_t *labelmap;
  wordmap_t *distmap;
  real_t dmax;

  initialize_screen(w, h, 32);

  bin = bitmap_new(w, h);
  a = bytemap_new(w, h);
  b = bytemap_new(w, h);
  c = bytemap_new(w, h);
  m = matrix_new(w, h, false);
  labelmap = dwordmap_new(w, h);
  distmap = wordmap_new(w, h);

  bitmap_clear(bin);
  bitmap_draw_filled_circle(w/4, h/4, w/6, 1, bin);
  bitmap_draw_filled_circle(3*w/4, h/4, w/6, 1, bin);
  bitmap_draw_filled_circle(w/2, h/2, w/6, 1, bin);
  bitmap_draw_filled_circle(w/4, 3*h/4, w/6, 1, bin);
  bitmap_draw_filled_circle(3*w/4, 3*h/4, w/6, 1, bin);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  bitmap_distance_transform(m, bin);
  dynamic_matrix2bytemap(a, m);
  write_bytemap_to_screen(a, a, a);
  wait_keyhit();

  matrix_get_max(&dmax, m);
  matrix_subtract_scalar(m, dmax);
  matrix_multiply_scalar(m, -1.0);
  matrix2wordmap(distmap, m);
  dynamic_wordmap2rgb(a, b, c, distmap);
  write_bytemap_to_screen(a, b, c);
  wait_keyhit();

  n = wordmap_create_watershed(labelmap, NULL, distmap, NEIGHBOR_8);
  printf("watershed : %d\n", n);
  dynamic_dwordmap2rgb(a, b, c, labelmap);
  /*
  bytemap_mask(a, roi);
  bytemap_mask(b, roi);
  bytemap_mask(c, roi);
  */
  write_bytemap_to_screen(a, b, c);
  wait_keyhit();

  wordmap_destroy(distmap);
  dwordmap_destroy(labelmap);
  matrix_destroy(m);
  bytemap_destroy(c);
  bytemap_destroy(b);
  bytemap_destroy(a);
  bitmap_destroy(bin);

  deinitialize_screen();
}
