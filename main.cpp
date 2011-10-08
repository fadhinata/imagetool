#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#include <common.h>
#include <bytemap.h>
#include <bmp.h>
#include <mean_smooth.h>
#include <median_smooth.h>
#include <histogram.h>
#include <gamma.h>
#include <threshold.h>
#include <backsym.h>
#include <dsigma.h>
#include <isodata.h>
#include <otsu.h>
#include <triangulate.h>
#include <labeling.h>
#include <kirsch.h>
#include <prewitt.h>
#include <roberts.h>
#include <robinson.h>
#include <scharr.h>
#include <sobel.h>
#include <draw_line.h>
#include <draw_circle.h>
#include <hough_line.h>
#include <hough_circle.h>
#include <convert.h>
#include <gray.h>
#include <pixmap_statistic.h>
#include <binary_morph.h>
#include <gram_schmidt.h>
#include <qr_decomp.h>
#include <dlink.h>
#include <convexhull.h>
#include <chaincode.h>
#include <thin.h>
#include <delaunay.h>
#include <dt_wshed.h>
#include <fourier.h>
//#include <examples.h>
#include <screen.h>

#define WIDTH 640
#define HEIGHT 480
#define BPP 32

int main(int argc, char *argv[])
{
  int i, j, k, i1, i2, w, h;
  bytemap_t *a, *b, *c, *se, *gray;
  bitmap_t *bin, *roi;
  chaincode_t *chain;
  point_list_t *plist;
  point_t *p, *q, *r;
  dlink_t *link, *next;
  int values[9], num;
  // hough accumulate
  hough_circle_t *hough_circle;
  dwordmap_t *cell;
  long vmax;
  int imax, jmax, kmax;
  real_t xorg, yorg, val;
  int xmin, xmax, ymin, ymax;
  char str[80], fn[80], *ptr;
  FILE *f;
  real_t mr, mg, mb;

  w = WIDTH; h = HEIGHT;

  initialize_screen(WIDTH, HEIGHT, BPP);
  
  a = bytemap_new(w, h);
  b = bytemap_new(w, h);
  c = bytemap_new(w, h);
  gray = bytemap_new(w, h);
  bin = bitmap_new(w, h);
  roi = bitmap_new(w, h);
  
  sprintf(fn, "%s", ptr);
  printf("filename: %s\n", fn);

  // Display Image
  printf("Display Image\n");
  load_and_display_BMP(argv[1]);
  read_bytemap_in_screen(a, b, c);
  wait_keyhit();

  if (argv[1]) {
    ptr = strrchr(argv[1], '\\');
    if (ptr == NULL) ptr = strrchr(argv[1], '/');
    ptr++;
  } else {
    sprintf(fn, "unknown");
    ptr = fn;
  }

  // convert RGB to gray
  printf("Convert RGB to Gray\n");
  rgb2gray(gray, a, b, c);
  write_bytemap_to_screen(gray, gray, gray);
  save_screen_as_BMP("gray.bmp");
  wait_keyhit();

  // convert gray to psuedo color
  printf("Convert Gray to psuedo color\n");
  bytemap2rgb(a, b, c, gray);
  write_bytemap_to_screen(a, b, c);
  save_screen_as_BMP("psuedo_gray.bmp");
  wait_keyhit();

  // Image smoothing
  printf("Image smoothing\n");
  mean_smooth(a, gray, 3);
  //median_smooth(a, gray, 3);
  write_bytemap_to_screen(a, a, a);
  save_screen_as_BMP("median_smooth.bmp");
  wait_keyhit();

  // Thresholding
  printf("Thresholding\n");
  bytemap_threshold(roi, a, 30, 100);
  //bytemap_otsu_threshold(bin, b);
  //bytemap_isodata_threshold(bin, c);
  //bytemap_dominant_sigma_threshold(bin, b);
  //bytemap_triangle_threshold(bin, b);
  write_bitmap_to_screen(roi, roi, roi);
  save_screen_as_BMP("threshold.bmp");
  wait_keyhit();

  /*
  // Image complement
  printf("Image complementation\n");
  bitmap_not(bin);
  write_bitmap_to_screen(bin, bin, bin, screen);
  wait_keyhit();
  */

  // Isolate an object with dominent area
  printf("Isolate the biggest object\n");
  label_grep_largest_blob(bin, roi);
  //bitmap_not(roi);
  write_bitmap_to_screen(bin, bin, bin);
  save_screen_as_BMP("isolate.bmp");
  wait_keyhit();

  // Morphological operation
  printf("Binary opening\n");
  se = bytemap_new(3, 3);
  bytemap_fill(se, 0, 0, bytemap_get_width(se), bytemap_get_height(se), 1);
  binary_opening(roi, bin, se);
  bytemap_destroy(se);
  write_bitmap_to_screen(roi, roi, roi);
  save_screen_as_BMP("opening.bmp");
  wait_keyhit();

  /*
  printf("Binary closing\n");
  se = bytemap_new(3, 3);
  bytemap_fill(se, 0, 0, bytemap_get_width(se), bytemap_get_height(se), 1);
  binary_closing(roi, bin, se);
  write_bitmap_to_screen(roi, roi, roi);
  bytemap_destroy(se);
  wait_keyhit();
  */

  // Isolate an object with dominent area
  printf("Isolate the biggest object\n");
  i = label_grep_largest_blob(bin, roi);
  //label_pickup_by_largest_area(roi, bin);
  //bitmap_not(roi);
  bitmap_copy(roi, 0, 0, bin, 0, 0, bitmap_get_width(bin), bitmap_get_height(bin));
  write_bitmap_to_screen(roi, roi, roi);
  save_screen_as_BMP("roi.bmp");
  wait_keyhit();
  
  f = fopen("Area.txt", "a+");
  fprintf(f, "%s, %d\n", fn, i);
  fclose(f);

  // Average RGB
  printf("Average RGB: ");
  num = 0;
  mr = 0; mg = 0; mb = 0;
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(j, i, roi)) {
	num++;
	mr += bytemap_get_value(j, i, a);
	mg += bytemap_get_value(j, i, b);
	mb += bytemap_get_value(j, i, c);
      }
    }
  }

  f = fopen("Average_RGB.txt", "a+");
  fprintf(f, "%s, %lf, %lf, %lf\n", fn, mr / (real_t)num, mg / (real_t)num, mb / (real_t)num);
  fclose(f);

  // Strip outlier
  /*
  bitmap_delete_shell(roi, 10);
  write_bitmap_to_screen(roi, roi, roi);
  wait_keyhit();
  */

  // Chain coding
  printf("Chain coding:");
  chain = chaincode_new();
  bitmap_create_chaincode(chain, roi);

  plist = point_list_new();
  chaincode_create_point_list(plist, chain);
  printf(" %d points\n", point_list_get_count(plist));
  bitmap_clear(bin);
  for (i = 0; i < point_list_get_count(plist); i++) {
    p = point_list_glimpse(i, plist);
    //for (link = plist->tail->next; link != plist->head; link = link->next) {
    //p = (point_t *)link->object;
    bitmap_set_value(p->x, p->y, bin);
  }
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  // Sampling chain coding
  printf("Sampling chain coding: ");
  //printf("chaincode:%d\n", point_list_get_count(plist));
  real_t ds, s;
  ds = (real_t )point_list_get_count(plist) / (64.0);
  s = 0;
  for (i = 0, link = plist->tail->next; link != plist->head; i++) {
    //if ((i % j) != 0) {
    if (i == round(s)) {
      link = link->next;
      s += ds;
    } else {
      next = link->next;
      dlink_cutoff(link); plist->count--;
      ((point_t *)link->object)->reference--;
      point_destroy((point_t *)link->object);
      dlink_destroy(link);
      link = next;
    }
  }
  printf("%d points\n", point_list_get_count(plist));
  bytemap_clear(a);
  for (link = plist->tail->next; link != plist->head; link = link->next) {
    p = (point_t *)link->object;
    bytemap_fill(a, p->x-1, p->y-1, 3, 3, 255);
    bitmap_set_value(p->x, p->y, bin);
  }
  write_bytemap_to_screen(a, NULL, NULL);
  save_screen_as_BMP("sampling.bmp");
  wait_keyhit();

  // Fourier descriptor
  printf("Fourier descriptor\n");
  vector_t *time, *fourier, *fd;
  complex_t comp;

  time = cvector_new_and_copy_point_list(plist);
  fourier = vector_new(vector_get_length(time), true);
  fd = vector_new(vector_get_length(time), true);

  dft(fourier, time);
  i = vector_get_length(time);

  // filtering
  comp.real = comp.imag = 0.0;
  for (j = i/2-16; j < i/2+16; j++) {
    cvector_put_value(comp, j, fourier);
  }

  ///////////////////////  Normalization ////////////////////////////
  vector_copy(fd, fourier);
  // For translation invariance, 0 frequency must have a zero value
  cvector_put_value(comp, 0, fd);

  // For scale invariance,
  // fourier descriptor must be divied by the absolute of 1 frequency component.
  cvector_read_value(&comp, 1, fd);
  val = complex_get_abs(&comp);
  vector_divide_scalar(fd, val);
  ivector_divide_scalar(fd, val);

  // For rotating and changes in starting point
  // remove all phase information and
  // consider only absolute values of the descriptor element
  f = fopen("fourier_descriptor.txt", "a+");
  fprintf(f, "%s", fn);
  for (i = 0; i < vector_get_length(fd); i++) {
    cvector_read_value(&comp, i, fd);
    val = complex_get_abs(&comp);
    vector_put_value(val, i, fd);
    ivector_put_value(0, i, fd);
    fprintf(f, ", %lf", val);
  }
  fprintf(f, "\n");
  fclose(f);
  ///////////////////////////////////////////////////////////////////

  idft(time, fourier);

  point_list_copy_cvector(plist, time);

  vector_destroy(fd);
  vector_destroy(fourier);
  vector_destroy(time);

  bytemap_clear(a);
  for (link = plist->tail->next; link != plist->head; link = link->next) {
    p = (point_t *)link->object;
    bytemap_isrange(p->x, p->y, a);
    bytemap_fill(a, p->x-1, p->y-1, 3, 3, 255);
    bitmap_set_value(p->x, p->y, bin);
  }
  write_bytemap_to_screen(a, NULL, NULL);
  save_screen_as_BMP("sampling1.bmp");
  wait_keyhit();

  printf("Filling polygon\n");
  xmin = xmax = ymin = ymax = -1;
  for (link = plist->tail->next; link != plist->head; link = link->next) {
    p = (point_t *)link->object;
    if ((xmin == -1) || (xmin > p->x)) xmin = p->x;
    if ((xmax == -1) || (xmax < p->x)) xmax = p->x;
    if ((ymin == -1) || (ymin > p->y)) ymin = p->y;
    if ((ymax == -1) || (ymax < p->y)) ymax = p->y;
  }

  bitmap_clear(bin);
  for (i = ymin; i < ymax; i++) {
    for (j = xmin; j < xmax; j++) {
      if (is_in_polygon(j, i, plist))
	bitmap_set_value(j, i, bin);
      else
	bitmap_reset_value(j, i, bin);
    }
  }
  write_bitmap_to_screen(bin, bin, bin);
  save_screen_as_BMP("filling_polygon.bmp");
  wait_keyhit();

  write_bytemap_to_screen(a, a, a);

  delaunay_list_t *delaunay_list;
  delaunay_net_t *net, *nbr;

  printf("Delaunay triangulation\n");
  delaunay_list = delaunay_list_new();
  delaunay_list_create_2d_network(delaunay_list, plist);

  bytemap_clear(b);
  for (i = 0; i < delaunay_list_get_count_of_networks(delaunay_list); i++) {
    net = delaunay_list_get_net(i, delaunay_list);
    p = delaunay_net_get_coordinate(net);
    for (j = 0; j < delaunay_net_get_count_of_neighbors(net); j++) {
      nbr = delaunay_net_get_neighbor(j, net);
      q = delaunay_net_get_coordinate(nbr);
      bytemap_draw_line(p->x, p->y, q->x, q->y, 255, b);
    }
  }
  write_bytemap_to_screen(a, b, NULL);
  save_screen_as_BMP("delaunay.bmp");
  wait_keyhit();

  triangle_list_t *triangle_list, *new_list;
  triangle_t *tri, *tri1, *tri2;

  bytemap_clear(b);
  bytemap_clear(c);

  p = point_new();
  new_list = triangle_list_new();
  triangle_list = delaunay_list_get_triangle_list(delaunay_list);
  for (i = 0; i < triangle_list_get_count(triangle_list); i++) {
    tri = triangle_list_glimpse(i, triangle_list);
    triangle_circumcircle(p, tri);
    if (bitmap_isrange(p->x, p->y, bin) &&
	bitmap_isset(p->x, p->y, bin)) {
      triangle_list_insert(tri, new_list);
    }
  }

  typedef struct {
    int type;
    dlink_t *shared_ab;
    dlink_t *shared_bc;
    dlink_t *shared_ca;
  } triangle_status_t;

  triangle_status_t *status, *stat;
  status = (triangle_status_t *)calloc(triangle_list_get_count(new_list), sizeof(*status));
  memset(status, 0, triangle_list_get_count(new_list) * sizeof(*status));

  bool a_overlap, b_overlap, c_overlap;
  i = 0;
  for (link = new_list->tail->next; link != new_list->head; link = link->next) {
    tri = (triangle_t *)link->object;
    a_overlap = b_overlap = c_overlap = false;
    for (next = new_list->tail->next; next != new_list->head; next = next->next) {
      if (link == next) continue;
      tri1 = (triangle_t *)next->object;
      if ((point_cmp(tri->a, tri1->a) == 0 ||
	   point_cmp(tri->a, tri1->b) == 0 ||
	   point_cmp(tri->a, tri1->c) == 0) &&
	  (point_cmp(tri->b, tri1->a) == 0 ||
	   point_cmp(tri->b, tri1->b) == 0 ||
	   point_cmp(tri->b, tri1->c) == 0)) {
	status[i].shared_ab = next;
	a_overlap = true;
      }
      if ((point_cmp(tri->b, tri1->a) == 0 ||
	   point_cmp(tri->b, tri1->b) == 0 ||
	   point_cmp(tri->b, tri1->c) == 0) &&
	  (point_cmp(tri->c, tri1->a) == 0 ||
	   point_cmp(tri->c, tri1->b) == 0 ||
	   point_cmp(tri->c, tri1->c) == 0)) {
	status[i].shared_bc = next;
	b_overlap = true;
      }
      if ((point_cmp(tri->c, tri1->a) == 0 ||
	   point_cmp(tri->c, tri1->b) == 0 ||
	   point_cmp(tri->c, tri1->c) == 0) &&
	  (point_cmp(tri->a, tri1->a) == 0 ||
	   point_cmp(tri->a, tri1->b) == 0 ||
	   point_cmp(tri->a, tri1->c) == 0)) {
	status[i].shared_ca = next;
	c_overlap = true;
      }
    }

    if (a_overlap && b_overlap && c_overlap) {
      // branch triangle
      status[i].type = 3;
      bytemap_draw_line(tri->a->x, tri->a->y, tri->b->x, tri->b->y, 255, a);
      bytemap_draw_line(tri->b->x, tri->b->y, tri->c->x, tri->c->y, 255, a);
      bytemap_draw_line(tri->c->x, tri->c->y, tri->a->x, tri->a->y, 255, a);
    } else if ((!a_overlap && b_overlap && c_overlap) ||
	       (a_overlap && !b_overlap && c_overlap) ||
	       (a_overlap && b_overlap && !c_overlap)) {
      // edge triangle
      status[i].type = 2;
      bytemap_draw_line(tri->a->x, tri->a->y, tri->b->x, tri->b->y, 255, b);
      bytemap_draw_line(tri->b->x, tri->b->y, tri->c->x, tri->c->y, 255, b);
      bytemap_draw_line(tri->c->x, tri->c->y, tri->a->x, tri->a->y, 255, b);
    } else if ((a_overlap && !b_overlap && !c_overlap) ||
	       (!a_overlap && b_overlap && !c_overlap) ||
	       (!a_overlap && !b_overlap && c_overlap)) {
      // end triangle
      status[i].type = 1;
      bytemap_draw_line(tri->a->x, tri->a->y, tri->b->x, tri->b->y, 255, c);
      bytemap_draw_line(tri->b->x, tri->b->y, tri->c->x, tri->c->y, 255, c);
      bytemap_draw_line(tri->c->x, tri->c->y, tri->a->x, tri->a->y, 255, c);
    }
    link->spare = (void *)&status[i];
    i++;
  }
  write_bytemap_to_screen(a, b, c);
  save_screen_as_BMP("delaunay1.bmp");
  wait_keyhit();

#if 0
  dlink_t *find_connected_end_or_branch_link(dlink_t *prev, dlink_t *curr)
  {
    dlink_t *next;
    triangle_t *prev_tri, *curr_tri, *next_tri;
    triangle_status_t *stat;

    //prev_tri = (triangle_t *)prev->object;
    curr_tri = (triangle_t *)curr->object;
    stat = (triangle_status_t *)curr->private;

    // terminate when end or branch triangle
    if (stat->type == 1 || stat->type == 3) return curr;

    if (prev == stat->shared_ab) {
      if (stat->shared_bc) next = stat->shared_bc;
      else next = stat->shared_ca;
    } else if (prev == stat->shared_bc) {
      if (stat->shared_ca) next = stat->shared_ca;
      else next = stat->shared_ab;
    } else { // if (prev == state->shared_ca)
      if (stat->shared_ab) next = stat->shared_ab;
      else next = stat->shared_bc;
    }
    return find_connected_end_or_branch_link(curr, next);
  }

  for (link = new_list->tail->next; link != new_list->head; link = link->next) {
    stat = (triangle_status_t *)link->private;
    if (stat->type == 3) {
      // branch triangle
      break;
    }
  }

  link = find_connected_end_or_branch_link(link, stat->shared_ab);
  stat = (triangle_status_t *)link->private;
  while (stat->type == 3) {
    link = find_connected_end_or_branch_link(link, stat->shared_ab);
    stat = (triangle_status_t *)next->private;
    while (stat->type == 3) {
      ///
    }

    link = find_connected_end_or_branch_link(link, stat->shared_bc);
    stat = (triangle_status_t *)next->private;

    link = find_connected_end_or_branch_link(link, stat->shared_ca);
    stat = (triangle_status_t *)next->private;
  }

  link = find_connected_end_or_branch_link(link, stat->shared_bc);
  link = find_connected_end_or_branch_link(link, stat->shared_ca);

#endif

  /*
  for (i = 0; i < triangle_list_get_count(new_list); i++) {
    tri = triangle_list_glimpse(i, new_list);
    if (status[i].type == 3) {
      // branch triangle
      tri1 = status[i].triangle_shared_ab;
      
    } else if (status[i].type == 2) {
      // edge triangle
    } else if (status[i].type == 1) {
      // end triangle
    }
    if (status[i].triangle_shared_ab &&
	status[i].triangle_shared_bc &&
	status[i].triangle_shared_ca) {
      // branch triangle
    }
  }
  */
  free(status);
  triangle_list_destroy(new_list);
  point_destroy(p);

  delaunay_list_delete(delaunay_list);
  delaunay_list_destroy(delaunay_list);

  /* calculate curvature of a graph
     given as y=f(x), curvature k = abs(y")/(1+y'^2)^(3/2)
     or mean radius of circumference circles of triangles
     formed by any three points */
  polygon_t *convexhull;

  printf("ConvexHull\n");
  convexhull = polygon_new();
  convexhull_compute(convexhull, plist, QUICK_HULL);
  bitmap_clear(bin);
  for (i = 0; i < polygon_get_count(convexhull); i++) {
    p = polygon_glimpse(i, convexhull);
    q = polygon_glimpse((i+1)%polygon_get_count(convexhull), convexhull);
    bitmap_draw_line(p->x, p->y, q->x, q->y, 1, bin);
  }
  write_bitmap_to_screen(bin, NULL, roi);
  save_screen_as_BMP("convexhull.bmp");
  wait_keyhit();

  polygon_t *minrect;

  printf("Min area bounding rectangle\n");
  minrect = polygon_new();
  convexhull_create_min_area_rectangle(minrect, convexhull);
  bitmap_clear(bin);
  for (i = 0; i < polygon_get_count(minrect); i++) {
    p = polygon_glimpse(i, minrect);
    q = polygon_glimpse((i + 1) % polygon_get_count(minrect), minrect);
    bitmap_draw_line(p->x, p->y, q->x, q->y, 1, bin);
  }
  write_bitmap_to_screen(bin, NULL, roi);
  save_screen_as_BMP("bounding.bmp");
  wait_keyhit();

  f = fopen("bounding_rectangle.txt", "a+");
  fprintf(f, "%s, ", fn);
  p = polygon_glimpse(0, minrect);
  q = polygon_glimpse(1, minrect);
  fprintf(f, "%lf, ", sqrt(sqr(p->x - q->x) + sqr(p->y - q->y)));
  p = polygon_glimpse(1, minrect);
  q = polygon_glimpse(2, minrect);
  fprintf(f, "%lf\n", sqrt(sqr(p->x - q->x) + sqr(p->y - q->y)));
  fclose(f);

  polygon_delete(minrect);
  polygon_destroy(minrect);

  //polygon_delete(convexhull);
  polygon_destroy(convexhull);

#if 0
  hough_circle = hough_circle_new(0, 0, 100, 0, 0, 100, 0, 0, 100);
  hough_circle_accumulate(hough_circle, bin);
  hough_circle_get_sorted_index(&jmax, &imax, &kmax, 1, hough_circle);
  hough_circle_get_arguments(&xorg, &yorg, &r, bitmap_get_width(bin)/2.0, bitmap_get_height(bin)/2.0, jmax, imax, kmax, hough_circle);
  hough_circle_destroy(hough_circle);
  bitmap_draw_circle((int)round(xorg), (int)round(yorg), (int)round(r), 1, bin);
  write_bitmap_to_screen(bin, NULL, NULL);
  wait_keyhit();
#endif

#if 0
  // pre-smoothing for stentiford's thinning
  stentiford_pre_smooth_for_thinning(roi, bin);
  write_bitmap_to_screen(roi, NULL, NULL);
  wait_keyhit();

  // Acute angle emphasis for stentiford's thinning
  stentiford_acute_angle_emphasis_for_thinning(roi, bin);
  write_bitmap_to_screen(roi, NULL, NULL);
  wait_keyhit();

  // Stentiford's thinning
  printf("Stentiford's thinning\n");
  stentiford_thinning(roi, bin);
  write_bitmap_to_screen(roi, NULL, NULL);
  wait_keyhit();

  // Zhang and suen thinning
  printf("Zhang and suen thinning\n");
  zhang_suen_thinning(roi, bin);
  write_bitmap_to_screen(roi, bin, NULL);
  wait_keyhit();

  // holt et al thinning
  holt_et_al_thinning(roi, bin);
  write_bitmap_to_screen(roi, NULL, NULL);
  wait_keyhit();

  test_distance_transform_and_watershed(w, h);

  matrix_t *m;
  real_t dmax;

  m = matrix_new(w, h, false);
  bitmap_distance_transform(m, roi);
  dynamic_matrix2bytemap(a, m);
  write_bytemap_to_screen(a, a, a);
  wait_keyhit();

  matrix_get_max(&dmax, m);
  printf("dmax: %lf\n", dmax);
  matrix_threshold(bin, m, dmax / 30, dmax);
  write_bitmap_to_screen(bin, roi, bin);
  wait_keyhit();

  dwordmap_t *labelmap;
  wordmap_t *distmap;

  labelmap = dwordmap_new(w, h);
  distmap = wordmap_new(w, h);
  matrix_subtract_scalar(m, dmax);
  matrix_multiply_scalar(m, -1.0);
  matrix2wordmap(distmap, m);
  dynamic_wordmap2rgb(a, b, c, distmap);
  write_bytemap_to_screen(a, b, c);
  wait_keyhit();

  wordmap_create_watershed(labelmap, NULL, distmap, NEIGHBOR_8);
  dynamic_dwordmap2rgb(a, b, c, labelmap);
  bytemap_mask(a, roi);
  bytemap_mask(b, roi);
  bytemap_mask(c, roi);
  write_bytemap_to_screen(a, b, c);
  wait_keyhit();

  wordmap_destroy(distmap);
  dwordmap_destroy(labelmap);
  matrix_destroy(m);

  // Zhang, suen and holt thinning
  printf("Zhang, suen and holt thinning\n");
  zhangsuen_holt_thinning(bin, roi, 1);
  write_bitmap_to_screen(bin, NULL, roi);
  save_screen_as_BMP("thin.bmp");
  wait_keyhit();

  // thining to point list
  dlist_t *edge, *edge_tree;

  edge_tree = dlist_new();
  thinning_to_edge_tree(edge_tree, bin);
  edge_tree_dump(edge_tree);
  edge = edge_tree_get_long_edge(edge_tree);
  printf("The longest edge: %d\n", dlist_get_count(edge));

  // calculate the curvature of main edge
  edge_tree_delete(edge_tree);
  dlist_destroy(edge_tree);
#endif

  /*
  printf("edge: %d\n", dlist_get_count(edge_tree));
  link = dlist_glimpse(dlist_get_count(edge_tree)-1, edge_tree);
  while (1) {
    if (link->private) {
      list = link->private;
    } else {
    }
  */

  point_list_delete(plist);
  point_list_destroy(plist);

  chaincode_destroy(chain);

  bitmap_destroy(roi);
  bitmap_destroy(bin);
  bytemap_destroy(gray);
  bytemap_destroy(c);
  bytemap_destroy(b);
  bytemap_destroy(a);

  deinitialize_screen();

  return 0;
}


