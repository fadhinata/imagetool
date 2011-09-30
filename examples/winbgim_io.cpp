#include <stdio.h>
#include <winbgim.h>
#include <bitmap.h>
#include <bytemap.h>
#include <dwordmap.h>
#include <common.h>

void display_read_red_image(bytemap_t *p, void *image)
{
	int *buffer, i, j;
	int width, height, widthbytes;

	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			//*(p->buffer+i*p->header.pitch+j) = buffer[i*widthbytes/4+j]; // blue
			//*(p->buffer+i*p->header.pitch+j) = buffer[i*widthbytes/4+j]>>8; // green
			*(p->buffer+i*p->header.pitch+j) = buffer[i*widthbytes/4+j]>>16; // red
		}
	}
}

void display_read_green_image(bytemap_t *p, void *image)
{
	int *buffer, i, j;
	int width, height, widthbytes;

	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			//*(p->buffer+i*p->header.pitch+j) = buffer[i*widthbytes/4+j]; // blue
			*(p->buffer+i*p->header.pitch+j) = buffer[i*widthbytes/4+j]>>8; // green
			//*(p->buffer+i*p->header.pitch+j) = buffer[i*widthbytes/4+j]>>16; // red
		}
	}
}

void display_read_blue_image(bytemap_t *p, void *image)
{
	int *buffer, i, j;
	int width, height;
	int widthbytes;

	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*(p->buffer+i*p->header.pitch+j) = buffer[i*widthbytes/4+j]; // blue
			//*(p->buffer+i*p->header.pitch+j) = buffer[i*widthbytes/4+j]>>8; // green
			//*(p->buffer+i*p->header.pitch+j) = buffer[i*widthbytes/4+j]>>16; // red
		}
	}
}

void display_read_rgb_image(bytemap_t *r, bytemap_t *g, bytemap_t *b, void *image)
{
	int *buffer, i, j;
	int width, height;
	int widthbytes;

	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*(b->buffer+i*b->header.pitch+j) = buffer[i*widthbytes/4+j]; // blue
			*(g->buffer+i*g->header.pitch+j) = buffer[i*widthbytes/4+j]>>8; // green
			*(r->buffer+i*r->header.pitch+j) = buffer[i*widthbytes/4+j]>>16; // red
		}
	}
}

// mono image
void display_write_mono_image(void *image, int x, int y, bitmap_t *p)
{
	int *buffer, i, j;
	int width, height, widthbytes;
	int joff, ioff;
	unsigned char *pbuf;

	joff = max(0, x);
	ioff = max(0, y);
	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = min(((BITMAP *)image)->bmWidth, x+p->header.width);
	height = min(((BITMAP *)image)->bmHeight, y+p->header.height);
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	pbuf = p->buffer+ioff*p->header.pitch;
	for (i = ioff; i < height; i++) {
		for (j = joff; j < width; j++) {
			if (*(pbuf+(j>>3))&(1<<(j%8))) {
				buffer[i*widthbytes/4+j] |= 0x00ffffff;
			} else {
				buffer[i*widthbytes/4+j] &= 0xff000000;
			}
		}
		pbuf += p->header.pitch;
	}
}

void display_write_mono_image_at_red_part(void *image, int x, int y, bitmap_t *p)
{
	int *buffer, i, j;
	int width, height, widthbytes;
	int joff, ioff;
	unsigned char *pbuf;

	joff = max(0, x);
	ioff = max(0, y);
	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = min(((BITMAP *)image)->bmWidth, x+p->header.width);
	height = min(((BITMAP *)image)->bmHeight, y+p->header.height);
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	pbuf = p->buffer+ioff*p->header.pitch;
	for (i = ioff; i < height; i++) {
		for (j = joff; j < width; j++) {
			if (*(pbuf+(j>>3))&(1<<(j%8))) {
				buffer[i*widthbytes/4+j] |= 0x00ff0000;
			} else {
				buffer[i*widthbytes/4+j] &= 0xff00ffff;
			}
		}
		pbuf += p->header.pitch;
	}
}

void display_write_mono_image_at_green_part(void *image, int x, int y, bitmap_t *p)
{
	int *buffer, i, j;
	int width, height, widthbytes;
	int joff, ioff;
	unsigned char *pbuf;

	joff = max(0, x);
	ioff = max(0, y);
	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = min(((BITMAP *)image)->bmWidth, x+p->header.width);
	height = min(((BITMAP *)image)->bmHeight, y+p->header.height);
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	pbuf = p->buffer+ioff*p->header.pitch;
	for (i = ioff; i < height; i++) {
		for (j = joff; j < width; j++) {
			if (*(pbuf+(j>>3))&(1<<(j%8))) {
				buffer[i*widthbytes/4+j] |= 0x0000ff00;
			} else {
				buffer[i*widthbytes/4+j] &= 0xffff00ff;
			}
		}
		pbuf += p->header.pitch;
	}
}

void display_write_mono_image_at_blue_part(void *image, int x, int y, bitmap_t *p)
{
	int *buffer, i, j;
	int width, height, widthbytes;
	int joff, ioff;
	unsigned char *pbuf;

	joff = max(0, x);
	ioff = max(0, y);
	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = min(((BITMAP *)image)->bmWidth, x+p->header.width);
	height = min(((BITMAP *)image)->bmHeight, y+p->header.height);
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	pbuf = p->buffer+ioff*p->header.pitch;
	for (i = ioff; i < height; i++) {
		for (j = joff; j < width; j++) {
			if (*(pbuf+(j>>3))&(1<<(j%8))) {
				buffer[i*widthbytes/4+j] |= 0x000000ff;
			} else {
				buffer[i*widthbytes/4+j] &= 0xffffff00;
			}
		}
		pbuf += p->header.pitch;
	}
}

void display_clear_all_parts(void *image)
{
	int *buffer, i, j;
	int width, height, widthbytes;

	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			buffer[i*widthbytes/4+j] = 0;
		}
	}
}

void display_write_gray_image(void *image, int x, int y, bytemap_t *p)
{
	int *buffer, i, j;
	int ioff, joff;
	int width, height, widthbytes;
	
	joff = max(x, 0);
	ioff = max(y, 0);
	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = min(((BITMAP *)image)->bmWidth, x+p->header.width);
	height = min(((BITMAP *)image)->bmHeight, y+p->header.height);
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = ioff; i < height; i++) {
		for (j = joff; j < width; j++) {
			buffer[i*widthbytes/4+j] = (*(p->buffer+(i-y)*p->header.pitch+(j-x))) | // blue
									(*(p->buffer+(i-y)*p->header.pitch+(j-x))<<8) | // green
									(*(p->buffer+(i-y)*p->header.pitch+(j-x))<<16); // red
		}
	}
}

void display_clear_red_part(void *image)
{
	int *buffer, i, j;
	int width, height, widthbytes;

	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			buffer[i*widthbytes/4+j] &= (~0xffff0000); // red
		}
	}
}

void display_write_red_image(void *image, int x, int y, bytemap_t *p)
{
	int *buffer, i, j;
	int joff, ioff;
	int width, height, widthbytes;

	joff = max(x, 0);
	ioff = max(y, 0);
	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = min(((BITMAP *)image)->bmWidth, x+p->header.width);
	height = min(((BITMAP *)image)->bmHeight, y+p->header.height);
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = ioff; i < height; i++) {
		for (j = joff; j < width; j++) {
			buffer[i*widthbytes/4+j] = (buffer[i*widthbytes/4+j]&~0xffff0000)|((*(p->buffer+(i-y)*p->header.pitch+(j-x))<<16)&0x00ff0000); // red
		}
	}
}

void display_clear_green_part(void *image)
{
	int *buffer, i, j;
	int width, height, widthbytes;

	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			buffer[i*widthbytes/4+j] &= (~0xff00ff00); // green
		}
	}
}

void display_write_green_image(void *image, int x, int y, bytemap_t *p)
{
	int *buffer, i, j;
	int ioff, joff;
	int width, height, widthbytes;

	joff = max(x, 0);
	ioff = max(y, 0);
	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = min(((BITMAP *)image)->bmWidth, x+p->header.width);
	height = min(((BITMAP *)image)->bmHeight, y+p->header.height);
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = ioff; i < height; i++) {
		for (j = joff; j < width; j++) {
			buffer[i*widthbytes/4+j] = (buffer[i*widthbytes/4+j]&~0xff00ff00)|((*(p->buffer+(i-y)*p->header.pitch+(j-x))<<8)&0x0000ff00); // green
		}
	}
}

void display_clear_blue_part(void *image)
{
	int *buffer, i, j;
	int width, height, widthbytes;

	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			buffer[i*widthbytes/4+j] &= (~0xff0000ff); // blue
		}
	}
}

void display_write_blue_image(void *image, int x, int y, bytemap_t *p)
{
	int *buffer, i, j;
	int ioff, joff;
	int width, height, widthbytes;

	ioff = max(y, 0);
	joff = max(x, 0);
	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = min(((BITMAP *)image)->bmWidth, x+p->header.width);
	height = min(((BITMAP *)image)->bmHeight, y+p->header.height);
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = ioff; i < height; i++) {
		for (j = joff; j < width; j++) {
			buffer[i*widthbytes/4+j] = (buffer[i*widthbytes/4+j]&~0xff0000ff)|((*(p->buffer+(i-y)*p->header.pitch+(j-x)))&0x0000ff); // blue
		}
	}
}

static int *label2rgb = NULL;

void write_label_image(void *image, dwordmap_t *p)
{
	int *buffer, i, j;
	int width, height, widthbytes;
	long val;

	if (!label2rgb) {
		srand ( time(NULL) );
		label2rgb = (int *)malloc(0xffffff*sizeof(int));
		memset(label2rgb, 0, 0xffffff*sizeof(int));
	}
	
	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			val = DWORDMAP_GET(p, j, i);
			if (val == -1) {
				buffer[i*widthbytes/4+j] = 0;
			} else {
				if (label2rgb[val%0x1000000] == 0) {
					label2rgb[val%0x1000000] = (((rand()%255)+1)<<16) | (((rand()%255)+1)<<8) | ((rand()%255)+1);
				}
				val = label2rgb[val%0x1000000];
				buffer[i*widthbytes/4+j] = val;
			}
		}
	}
}

void write_rgb_image(void *image, dwordmap_t *p)
{
	int *buffer, i, j;
	int width, height, widthbytes;
	long val;

	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			val = DWORDMAP_GET(p, j, i);
			val++;
			buffer[i*widthbytes/4+j] = val;
		}
	}
}

void write_mask_image(void *image, bitmap_t *p)
{
	int *buffer, i, j;
	int width, height, widthbytes;
	long val;

	buffer = (int *)(((BITMAP *)image)->bmBits);
	width = ((BITMAP *)image)->bmWidth;
	height = ((BITMAP *)image)->bmHeight;
	widthbytes = ((BITMAP *)image)->bmWidthBytes;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (BITMAP_ISSET(p, j, i)) {
				buffer[i*widthbytes/4+j] |= ((((buffer[i*widthbytes/4+j]>>16)&0xff)>>1) + ((128)>>1))<<16;
			}
		}
	}
}
