#include <stdio.h>
#include <math.h>
#include <dir.h>
#include <winbgim_io.h>
#include <bmp.h>
#include <assert.h>
#include <bytemap.h>
#include <pixmap_statistic.h>
#include <threshold.h>
#include <labeling.h>
#include <matrix.h>
#include <convert.h>
#include <matrix_statistic.h>
#include <binary_morph.h>
#include <common.h>

static int display_width = 644;
static int display_height = 492;

static int getsw(void)
{
	int ch;
	ch = getch() << 8;
	if (kbhit()) ch |= getch();
	else ch >>= 8;
	return ch;
}


static void SaveImage(const char *filename, bytemap_t *gray)
{
	int i, sizeimage, widthbytes;
	unsigned char *image_buffer;
	BITMAPINFO *image_info;

	widthbytes = (gray->header.width*8+7)/8;
	sizeimage = (gray->header.height)*widthbytes;
	image_info = (BITMAPINFO *)malloc(sizeof(BITMAPINFO)+256*sizeof(RGBQUAD));
	assert(image_info);
	*((RGBQUAD **)(&(image_info->bmiColors))) = (RGBQUAD *)(image_info+1);
	//memset(&image_info, 0, sizeof(BITMAPINFO));
	//printf("info-header initialization\n");
	image_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	image_info->bmiHeader.biWidth = gray->header.width;
	image_info->bmiHeader.biHeight = gray->header.height;
	image_info->bmiHeader.biPlanes = 1;
	image_info->bmiHeader.biBitCount = 8;
	image_info->bmiHeader.biCompression = 0;
	image_info->bmiHeader.biSizeImage = sizeimage;
	image_info->bmiHeader.biXPelsPerMeter = 0;
	image_info->bmiHeader.biYPelsPerMeter = 0;
	image_info->bmiHeader.biClrUsed = 0;
	image_info->bmiHeader.biClrImportant = 0;
	//printf("palette initialization\n");
	for (i = 0; i < 256; i++) {
		image_info->bmiColors[i].rgbBlue = i;
		image_info->bmiColors[i].rgbGreen = i;
		image_info->bmiColors[i].rgbRed = i;
		image_info->bmiColors[i].rgbReserved = 0;
	}
	//printf("pixel data initialization\n");
	image_buffer = (unsigned char *)malloc(sizeimage);
	for (i = 0; i < gray->header.height; i++) {
		memcpy(image_buffer+i*widthbytes,
					gray->buffer+i*gray->header.pitch,
					gray->header.width);
	}
	SaveDIBitmap(filename, image_info, image_buffer);
	free(image_buffer);
	free(image_info);
}

int Shifting(int argc, char *argv[])
{
	int i, j, ch, xoff, yoff, need_display;
	unsigned char *display_buffer = NULL;
	bytemap_t *image = NULL;

#define LEFT  0x004b
#define RIGHT 0x004d
#define UP    0x0048
#define DOWN  0x0050
#define ENTER 0x000d
#define STOP  0x0020  /* SPACE */
#define FUNC  0x003b  /* F1 */
#define DEL   0x0053
#define ESC   0x001b

	initwindow(display_width, display_height, "Shifting");
	readimagefile(NULL, 0, 0, display_width-1, display_height-1);
	display_buffer = (unsigned char *)malloc(imagesize(0, 0, display_width-1, display_height-1));
	getimage(0, 0, display_width-1, display_height-1, display_buffer);
	image = bytemap_create(display_width, display_height);
	display_read_red_image(image, display_buffer);
	xoff = yoff = 0;
	need_display = 0;
	
	while (1) {
		if (kbhit()) {
			ch = getsw();
			printf("0x%x\n", ch);
			switch (ch) {
			case DEL:
			case LEFT:
				if (xoff > -display_width) {
					xoff--;
					need_display = 1;
				}
				break;
			case RIGHT:
				if (xoff < display_width) {
					xoff++;
					need_display = 1;
				}
				break;
			case UP:
				if (yoff > -display_height) {
					yoff--;
					need_display = 1;
				}
				break;
			case DOWN:
				if (yoff < display_height) {
					yoff++;
					need_display = 1;
				}
				break;
			case STOP:
				writeimagefile(NULL, 0, 0, display_width-1, display_height-1);
				SaveImage("C:\\Unknown.bmp", image);
				goto __return__;
				break;
			case ENTER:
				break;
			case ESC:
				exit(0);
				break;
			default:
//				exit(0);
				break;
			}

			if (need_display) {
				display_clear_all_parts(display_buffer);
				display_write_gray_image(display_buffer, xoff, yoff, image);
				putimage(0, 0, display_buffer, COPY_PUT);
				need_display = 0;
			}
		}
	}
	
__return__:;

	bytemap_delete(image);
	free(display_buffer);
	
	return 0;
}
