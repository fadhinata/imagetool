#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <winbgim.h>

#include <bytemap.h>
#include <line.h>
#include <circle.h>

static int image_width = 320;
static int image_height = 240;

int main(int argc, char *argv[])
{
	unsigned char *image1;
	bytemap_t *a, *b, *c;
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
	assert(a);
	assert(b);
	assert(c);
	assert(d);
	assert(e);
	
	colorpixmap = (int *)(((BITMAP *)image1)->bmBits);
	for (i = 0; i < image_height; i++) {
		for (j = 0; j < image_width; j++) {
			// colorpixmap[] contains red << 16 | green << 8 || blue 
			*(a->buffer+i*a->header.pitch+j) = colorpixmap[i*image_width+j]; // blue
		}
	}
	getch();
	return 0;
}
