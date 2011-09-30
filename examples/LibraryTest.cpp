
static int image_width = 320;
static int image_height = 240;

void LibraryTest(void)
{
	unsigned char *image1;
	bytemap_t *a, *b, *c, *f;
	bitmap_t *d;
	dwordmap_t *e;
	int *colorpixmap;
	int i, j, value, pixels;

    initwindow(image_width, image_height, "ImageTool");

	// read image in left top
	readimagefile(NULL, 0, 0, image_width-1, image_height-1);
	image1 = (unsigned char *)malloc(imagesize(0, 0, image_width-1, image_height-1));
	assert(image1);
	getimage(0, 0, image_width-1, image_height-1, image1);
	a = bytemap_create(image_width, image_height);
	b = bytemap_create(image_width, image_height);
	c = bytemap_create(image_width, image_height);
	d = bitmap_create(image_width, image_height);
	e = dwordmap_create(image_width, image_height);
	f = bytemap_create(image_width, image_height);
	assert(a);
	assert(b);
	assert(c);
	assert(d);
	assert(e);
	assert(f);
	
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			*(a->buffer+i*a->header.pitch+j) = colorpixmap[i*image_width+j]>>16; // red
			*(b->buffer+i*b->header.pitch+j) = colorpixmap[i*image_width+j]>>8; // green
			*(c->buffer+i*c->header.pitch+j) = colorpixmap[i*image_width+j]; // blue
		}
	}
	getch();

	// rgb2gray
	printf("rgb2gray\n");
	rgb2gray(f, a, b, c);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			colorpixmap[i*image_width+j] = ((((int)*(f->buffer+i*f->header.pitch+j))&0xff)<<16)| // red
										   ((((int)*(f->buffer+i*f->header.pitch+j))&0xff)<<8)| // green
										   (((int)*(f->buffer+i*f->header.pitch+j))&0xff); // blue
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// bytemap2rgb
	printf("bytemap2rgb\n");
	bytemap2rgb(a, b, c, f);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			colorpixmap[i*image_width+j] = ((((int)*(a->buffer+i*a->header.pitch+j))&0xff)<<16)| // red
										   ((((int)*(b->buffer+i*b->header.pitch+j))&0xff)<<8)| // green
										   (((int)*(c->buffer+i*c->header.pitch+j))&0xff); // blue
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();
	
	// mean_smooth
	printf("mean_smooth\n");
	mean_smooth(c, a, 4);
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
	
	// median_smooth
	printf("median_smooth\n");
	median_smooth(c, a, 4);
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
	
	// histogram_equalize
	printf("histogram_equalize\n");
	bytemap_histogram_equalize(c, a);
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

	// gamma_correct
	printf("gamma_correct\n");
	bytemap_gamma_correct(c, a, 0.1);
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
	
	//threshold
	printf("threshold\n");
	bytemap_threshold(d, a, 64, 128+64);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// background symmetric threshold
	printf("background_symmetry_threshold\n");
	bytemap_background_symmetry_threshold(d, a, 0.5);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();


	// isodata_threshold
	printf("isodata_threshold\n");
	bytemap_isodata_threshold(d, a);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// triangle_threshold
	printf("triangle_threshold\n");
	bytemap_triangle_threshold(d, a);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// otsu_threshold
	printf("otsu_threshold\n");
	bytemap_otsu_threshold(d, a);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// dominant sigma threshold
	printf("dominant_sigma_threshold\n");
	bytemap_dominant_sigma_threshold(d, a);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();
	
	// labeling
	printf("labeling\n");
	pixels = labeling(e, NULL, NULL, d, NEIGHBOR_8);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(e->buffer+i*e->header.pitch/sizeof(long)+j) != -1) {
				colorpixmap[i*image_width+j] = (*(e->buffer+i*e->header.pitch/sizeof(long)+j))*0xfffff/(pixels+1)+0xfffff/(pixels+1);
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// kirsch_edge
	printf("kirsch_edge\n");
	kirsch_edge(d, a, 350);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// prewitt_edge
	printf("prewitt_edge\n");
	prewitt_edge(d, a, 120);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// roberts_edge
	printf("roberts_edge\n");
	roberts_edge(d, a, 50);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();
	
	// robinson_edge
	printf("robinson_edge\n");
	robinson_edge(d, a, 100);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();
	
	//scharr_edge
	printf("scharr_edge\n");
	scharr_edge(d, a, 450);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// sobel_edge
	printf("sobel_edge\n");
	sobel_edge(d, a, 100);
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			if (*(d->buffer+i*d->header.pitch+(j>>3)) & (1<<(j%8))) {
				colorpixmap[i*image_width+j] = 0x00ffffff;
			} else {
				colorpixmap[i*image_width+j] = 0;
			}
		}
	}
	putimage(0, 0, image1, COPY_PUT);
	getch();

	// line & circle
	printf("draw_line\n");
	memset(c->buffer, 0, c->header.pitch*c->header.height*sizeof(unsigned char));
	bytemap_draw_line(50, 25, image_width-100, image_height-11, 128, c);
	bytemap_fast_draw_line(150, 100, image_width, image_height-11, 200, c);
	//draw_circle(150, 100, 50, 255, c);
	//draw_filled_circle(200, 200, 50, 128, c);
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
	
	// hough accumulate
	hough_line_t *hough_line;
	dwordmap_t *cell;
	long vmax;
	int imax, jmax;
	double r, t, aa, bb, cc;
	
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

