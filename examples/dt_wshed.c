#include <SDL/SDL.h>

#include <common.h>
#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>
#include <dwordmap.h>
#include <circle.h>
#include <matrix.h>
#include <convert.h>
#include <dt.h>
#include <matrix_statistic.h>
#include <gray.h>
#include <watershed.h>
#include <neighbor.h>

extern SDL_Surface *screen;
extern void keyhit(void);
extern void putbitmap(bitmap_t *r, bitmap_t *g, bitmap_t *b, SDL_Surface *surface);
extern void putbytemap(bytemap_t *r, bytemap_t *g, bytemap_t *b, SDL_Surface *surface);

void test_distance_transform_and_watershed(int w, int h)
{
  int n;
  bitmap_t *bin;
  bytemap_t *a, *b, *c;
  matrix_t *m;
  dwordmap_t *labelmap;
  wordmap_t *distmap;
  real_t dmax;

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
  putbitmap(bin, bin, bin, screen);
  keyhit();

  bitmap_distance_transform(m, bin);
  dynamic_matrix2bytemap(a, m);
  putbytemap(a, a, a, screen);
  keyhit();

  matrix_get_max(&dmax, m);
  matrix_subtract_scalar(m, dmax);
  matrix_multiply_scalar(m, -1.0);
  matrix2wordmap(distmap, m);
  dynamic_wordmap2rgb(a, b, c, distmap);
  putbytemap(a, b, c, screen);
  keyhit();

  n = wordmap_create_watershed(labelmap, NULL, distmap, NEIGHBOR_8);
  printf("watershed : %d\n", n);
  dynamic_dwordmap2rgb(a, b, c, labelmap);
  /*
  bytemap_mask(a, roi);
  bytemap_mask(b, roi);
  bytemap_mask(c, roi);
  */
  putbytemap(a, b, c, screen);
  keyhit();

  wordmap_destroy(distmap);
  dwordmap_destroy(labelmap);
  matrix_destroy(m);
  bytemap_destroy(c);
  bytemap_destroy(b);
  bytemap_destroy(a);
  bitmap_destroy(bin);
}
