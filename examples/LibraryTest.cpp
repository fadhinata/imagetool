#include <string.h>

#include <common.h>
#include <bitmap.h>
#include <bytemap.h>
#include <dwordmap.h>
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

#include <screen.h>

int test_library(int argc, char *argv[])
{
  bytemap_t *a, *b, *c, *f, *g;
  bitmap_t *roi, *bin;
  dwordmap_t *d;
  int i, j, value, pixels;
  long w, h;

  hough_line_t *hough_line;
  hough_circle_t *hough_circle;
  dwordmap_t *cell;
  long vmax;
  int imax, jmax;
  double r, t, aa, bb, cc;

  w = strtol(argv[1]);
  h = strtol(argv[2]);

  initialize_screen(w, h, 32);

  a = bytemap_new(w, h);
  b = bytemap_new(w, h);
  c = bytemap_new(w, h);

  d = dwordmap_new(w, h);

  roi = bitmap_new(w, h);
  bin = bitmap_new(w, h);

  f = bytemap_new(w, h);
  g = bytemap_new(w, h);

  load_and_display_BMP(argv[3]);
  read_bytemap_in_screen(a, b, c);
  wait_keyhit();

  // rgb2gray
  printf("rgb2gray\n");
  rgb2gray(g, a, b, c);
  write_bytemap_to_screen(g, g, g);
  wait_keyhit();

  // bytemap2rgb
  printf("bytemap2rgb\n");
  bytemap2rgb(a, b, c, g);
  write_bytemap_to_screen(a, b, c);
  wait_keyhit();

  // mean_smooth
  printf("mean_smooth\n");
  mean_smooth(f, g, 4);
  write_bytemap_to_screen(f, f, f);
  wait_keyhit();

  // median_smooth
  printf("median_smooth\n");
  median_smooth(f, g, 4);
  write_bytemap_to_screen(f, f, f);
  wait_keyhit();
	
  // histogram_equalize
  printf("histogram_equalize\n");
  bytemap_histogram_equalize(f, g);
  write_bytemap_to_screen(f, f, f);
  wait_keyhit();

  // gamma_correct
  printf("gamma_correct\n");
  bytemap_gamma_correct(f, g, 0.1);
  write_bytemap_to_screen(f, f, f);
  wait_keyhit();

  //threshold
  printf("threshold\n");
  bytemap_threshold(bin, g, 128, 255);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  // background symmetric threshold
  printf("background_symmetry_threshold\n");
  bytemap_background_symmetry_threshold(bin, g, 0.5);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  // isodata_threshold
  printf("isodata_threshold\n");
  bytemap_isodata_threshold(bin, g);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  // triangle_threshold
  printf("triangle_threshold\n");
  bytemap_triangle_threshold(bin, g);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  // otsu_threshold
  printf("otsu_threshold\n");
  bytemap_otsu_threshold(bin, g);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  // dominant sigma threshold
  printf("dominant_sigma_threshold\n");
  bytemap_dominant_sigma_threshold(bin, g);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();
	
  // labeling
  printf("labeling\n");
  pixels = labeling(d, NULL, bin, NEIGHBOR_8);
  dwordmap2rgb(a, b, c, d);
  write_bytemap_to_screen(a, b, c);
  wait_keyhit();

  // kirsch_edge
  printf("kirsch_edge\n");
  kirsch_edge(bin, g, 350);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  // prewitt_edge
  printf("prewitt_edge\n");
  prewitt_edge(bin, g, 120);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  // roberts_edge
  printf("roberts_edge\n");
  roberts_edge(bin, g, 50);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();
	
  // robinson_edge
  printf("robinson_edge\n");
  robinson_edge(bin, g, 100);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();
	
  //scharr_edge
  printf("scharr_edge\n");
  scharr_edge(bin, g, 450);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  // sobel_edge
  printf("sobel_edge\n");
  sobel_edge(bin, g, 100);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  // line & circle
  printf("draw_line\n");
  bytemap_clear(f);
  bytemap_draw_line(50, 25, w - 100, h - 11, 128, f);
  bytemap_fast_draw_line(150, 100, w, h - 11, 200, f);
  draw_circle(150, 100, 50, 255, f);
  draw_filled_circle(200, 200, 50, 128, f);
  write_bytemap_to_screen(f, f, f);
  wait_keyhit();
  	
  // hough accumulate
  hough_line_t *hough_line;
  dwordmap_t *cell;
  long vmax;
  int imax, jmax;
  double r, t, aa, bb, cc;

  printf("hough circle\n");
  hough_circle = hough_circle_new(0, 0, 100, 0, 0, 100, 0, 0, 100);
  hough_circle_accumulate(hough_circle, bin);
  hough_circle_get_sorted_index(&jmax, &imax, &kmax, 1, hough_circle);
  hough_circle_get_arguments(&xorg, &yorg, &r, bitmap_get_width(bin)/2.0, bitmap_get_height(bin)/2.0, jmax, imax, kmax, hough_circle);
  hough_circle_destroy(hough_circle);

  bitmap_draw_circle((int)round(xorg), (int)round(yorg), (int)round(r), 1, bin);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

	printf("hough line\n");
	hough_line = hough_line_create(0, 0, 100, 0, 0, 100);
	bytemap_threshold(d, c, 100, 255);
	hough_line_accumulate(hough_line, d);
#if 1
	if (hough_line_sort_index(&jmax, &imax, 1, hough_line) > 0) {
		hough_line_arguments(&aa, &bb, &cc,
				d->header.width/2.0, d->header.height/2.0, jmax, imax, hough_line);
		// aa*x+bb*y+cc=0
		draw_line(0, -cc/bb,
			d->header.width-1, -aa/bb*(d->header.width-1)-cc/bb,
			255, c);
	}
#else
	// find max value;
	cell = hough_line->cell;
	vmax = 0;
	for (i = 0; i < cell->header.height; i++) {
		for (j = 0; j < cell->header.width; j++) {
			if (*(cell->buffer+i*(cell->header.pitch)/sizeof(*(cell->buffer))+j) > vmax) {
				vmax = *(cell->buffer+i*(cell->header.pitch)/sizeof(*(cell->buffer))+j);
				imax = i;
				jmax = j;
			}
		}
	}

	t = hough_line->theta_min+(hough_line->delta_theta)*jmax;
	r = hough_line->radius_min+(hough_line->delta_radius)*imax;
	/* sin(t)*(y-bin->header.height/2.0) = -cos(t)*(x-bin->header.width/2.0)+r; */
	//y = -cos(t)/sin(t) * (x-bin->header.width/2.0) + r/sin(t) +bin->header.height/2.0
	draw_line(0, -cos(t)/sin(t)*(-d->header.width/2.0)+r/sin(t)+d->header.height/2.0,
			  d->header.width-1, -cos(t)/sin(t)*(d->header.width-1-d->header.width/2.0)+r/sin(t)+d->header.height/2.0, 
			  255, c);
#endif

	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			colorpixmap[i*image_width+j] = ((((int)*(c->buffer+i*c->header.pitch+j))&0xff)<<16)| // red
										   ((((int)*(c->buffer+i*c->header.pitch+j))&0xff)<<8)| // green
										   (((int)*(c->buffer+i*c->header.pitch+j))&0xff); // blue
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// line & circle
	printf("draw_circle\n");
	memset(c->buffer, 0, c->header.pitch*c->header.height*sizeof(unsigned char));
	//draw_line(50, 25, image_width-100, image_height-11, 128, c);
	//draw_line_slow(150, 100, image_width, image_height-11, 200, c);
	bytemap_draw_circle(150, 100, 25, 255, c);
	//draw_circle(150, 100, 25, 255, c);
	bytemap_draw_circle(200, 200, 50, 255, c);
	//draw_circle(200, 200, 50, 255, c);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			colorpixmap[i*image_width+j] = ((((int)*(c->buffer+i*c->header.pitch+j))&0xff)<<16)| // red
										   ((((int)*(c->buffer+i*c->header.pitch+j))&0xff)<<8)| // green
										   (((int)*(c->buffer+i*c->header.pitch+j))&0xff); // blue
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();
	
	printf("hough circle\n");
	int k, kmax;
	double x, y;
	hough_circle_t *hough_circle;
	
	bytemap_threshold(d, c, 100, 255);
	hough_circle = hough_circle_create(0, 0, 50, 0, 0, 50, 0, 0, 50);
	hough_circle_accumulate(hough_circle, d);
	printf("hough accumulate done!\n");
#if 1
	if (hough_circle_sort_index(&jmax, &imax, &kmax, 1, hough_circle) > 0) {
		hough_circle_arguments(&x, &y, &r, (d->header.width)/2.0, (d->header.height)/2.0,
								jmax, imax, kmax, hough_circle);
		draw_circle(x, y, r, 128, c);
	}
#else
	// find max value;
	vmax = 0;
	for (k = 0; k < hough_circle->n; k++) {
		cell = *(hough_circle->cell+k);
		for (i = 0; i < cell->header.height; i++) {
			for (j = 0; j < cell->header.width; j++) {
				if (*(cell->buffer+i*(cell->header.pitch)/sizeof(*(cell->buffer))+j) > vmax) {
					vmax = *(cell->buffer+i*(cell->header.pitch)/sizeof(*(cell->buffer))+j);
					imax = i; jmax = j; kmax = k;
				}
			}
		}
	}
	
	r = hough_circle->radius_min+(hough_circle->delta_radius)*kmax;
	x = hough_circle->xorg_min+(hough_circle->delta_xorg)*jmax;
	y = hough_circle->yorg_min+(hough_circle->delta_yorg)*imax;
	printf("r(%lf), x(%lf), y(%lf)\n", r, x, y);
	draw_circle(x+(d->header.width)/2.0, y+(d->header.height)/2.0, r, 128, c);
#endif

	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			colorpixmap[i*image_width+j] = ((((int)*(c->buffer+i*c->header.pitch+j))&0xff)<<16)| // red
										   ((((int)*(c->buffer+i*c->header.pitch+j))&0xff)<<8)| // green
										   (((int)*(c->buffer+i*c->header.pitch+j))&0xff); // blue
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	hough_circle_delete(hough_circle);
	hough_line_delete(hough_line);
	bytemap_delete(f);
	dwordmap_delete(e);
	bitmap_delete(d);
	bytemap_delete(c);
	bytemap_delete(b);
	bytemap_delete(a);
	free(image1);
	closegraph();
//    system("PAUSE");
}

