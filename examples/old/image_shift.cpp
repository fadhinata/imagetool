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

#define NR_IMAGE 24

static int getsw(void)
{
	int ch;
	ch = getch() << 8;
	if (kbhit()) ch |= getch();
	else ch >>= 8;
	return ch;
}

static const int num2wavelength[2*NR_IMAGE] = {
	7, 530,
	8, 550,
	9, 570,
	10, 590,
	11, 610,
	12, 630,
	13, 650,
	14, 700,
	15, 720,
	16, 740,
	17, 760,
	18, 780,
	19, 800,
	20, 820,
	21, 840,
	22, 860,
	23, 880,
	24, 900,
	25, 920,
	26, 940,
	27, 960,
	28, 980,
	29, 1000,
	30, 1050
};

static int index2num[NR_IMAGE] = {
	7+12, 7+11, 7+13, 7+10, 7+14, 7+9, 7+15, 7+8, 7+16, 7+7,
	7+17, 7+6, 7+18, 7+5, 7+19, 7+4, 7+20, 7+3, 7+21, 7+2,
	7+22, 7+1, 7+23, 7+0
};

static bytemap_t *gray_images[NR_IMAGE];
static bytemap_t *temporary_bytemap = NULL;
static bitmap_t *bin = NULL, *roi = NULL;
static bytemap_t *se = NULL;
static matrix_t *matrix1 = NULL;
static matrix_t *matrix2 = NULL;
static int xoff[NR_IMAGE];
static int yoff[NR_IMAGE];

static int display_width = 644;
static int display_height = 492;

/* So terrible slow !!! */
void bytemap_cross_correlation(matrix_t *xcorr, bytemap_t *image, bytemap_t *mask)
{
	int i, j, x, y;
	int w, h, dx, dy, xoff, yoff;
	double ma, mb, sum;
	unsigned char *imbuf, *mskbuf;
	double *mbuf;
	assert(xcorr);
	assert(image);
	assert(mask);
	assert(xcorr->columns == image->header.width);
	assert(xcorr->rows == image->header.height);
	w = image->header.width;
	h = image->header.height;
	bytemap_get_mean(&ma, image);
	dx = mask->header.width;
	dy = mask->header.height;
	xoff = dx>>1;
	yoff = dy>>1;
	bytemap_get_mean(&mb, mask);
	mbuf = xcorr->real;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			sum = 0;
			imbuf = image->buffer+max(y-yoff, 0)*image->header.pitch;
			mskbuf = mask->buffer+(max(y-yoff, 0)-y+yoff)*mask->header.pitch;
			for (i = max(y-yoff, 0)-y; i < min(y+(dy-yoff), h)-y; i++) {
				for (j = max(x-xoff, 0)-x; j < min(x+(dx-xoff), w)-x; j++) {
					sum += (*(imbuf+(x+j))-ma)*(*(mskbuf+(j+xoff))-mb);
				}
				imbuf += image->header.pitch;
				mskbuf += mask->header.pitch;
			}
			*(mbuf+x) = sum;
		}
		printf(".");
		mbuf += xcorr->columns;
	}
}

static int switch_display_image = 0;

int ImageShift(int argc, char *argv[])
{
	int i, j, ch, index;
	char filename[256], dirname[256];
	double m, vmin, vmean, vmax;

	unsigned char *display_buffer = NULL;
	unsigned char *image_buffer = NULL;
	BITMAPINFO *image_info = NULL;
	
	FILE *fd;
	
#define LEFT  0x004b
#define RIGHT 0x004d
#define UP    0x0048
#define DOWN  0x0050
#define ENTER 0x000d
#define STOP  0x0020  /* SPACE */
#define FUNC  0x003b  /* F1 */
#define DEL   0x0053
#define ESC   0x001b

	if (argc < 2) {
		printf("Error : No argument that is path of multispectral images!\n");
		return -1;
	}

	initwindow(display_width, display_height, "ImageShift");
	display_buffer = (unsigned char *)malloc(imagesize(0, 0, display_width-1, display_height-1));
	getimage(0, 0, display_width-1, display_height-1, display_buffer);

	for (i = 0; i < NR_IMAGE; i++) {
		xoff[i] = yoff[i] = 0;
		gray_images[i] = NULL;
	}
	index = 0;
	
	// load base image and display
	sprintf(filename, "%s\\%d.bmp", argv[1], index2num[index]);
	printf("index %d\n", index);
	printf("filename %s\n", filename);

	if (gray_images[index] == NULL) {
		if (image_buffer) free(image_buffer);
		image_buffer = LoadDIBitmap(filename, &image_info);
		printf("image_info->bmiHeader.biWidth %d\n", image_info->bmiHeader.biWidth);
		printf("image_info->bmiHeader.biHeight %d\n", image_info->bmiHeader.biHeight);
		printf("image_info->bmiHeader.biBitCount %d\n", image_info->bmiHeader.biBitCount);
		assert(image_info->bmiHeader.biWidth == display_width);
		assert(image_info->bmiHeader.biHeight == display_height);
		assert(image_info->bmiHeader.biBitCount == 8);
		gray_images[index] = bytemap_create(display_width, display_height);
		for (i = 0; i < display_height; i++) {
			memcpy(gray_images[index]->buffer+i*gray_images[index]->header.pitch,
				image_buffer+i*image_info->bmiHeader.biWidth,
				image_info->bmiHeader.biWidth);
		}
		//display_write_green_image(display_buffer, 0, 0, gray_images[index]);
		//putimage(0, 0, display_buffer, COPY_PUT);
	}
	roi = bitmap_create(display_width, display_height);
	bytemap_get_mean(&m, gray_images[0]);
	bytemap_threshold(roi, gray_images[0], m, 255);

	bin = bitmap_create(display_width, display_height);
	label_pickup_by_largest_area(bin, roi);

	se = bytemap_create(3, 3);
	bytemap_fill(se, 0, 0, se->header.width, se->header.height, 1);
	binary_erode(roi, bin, se);
	
	// fill holes
	bitmap_not(roi);
	label_pickup_by_largest_area(bin, roi);
	bitmap_copy_not_bitmap(roi, bin);

	display_write_mono_image(display_buffer, 0, 0, roi);
	putimage(0, 0, display_buffer, COPY_PUT);
//	getch();
//	display_clear_all_parts(display_buffer);
//	display_write_green_image(display_buffer, 0, 0, gray_images[index]);
//	putimage(0, 0, display_buffer, COPY_PUT);

	temporary_bytemap = bytemap_create(display_width, display_height);
	matrix1 = matrix_create(display_width, display_height, 0);
	matrix2 = matrix_create(display_width, display_height, 0);
	
	bytemap2matrix(matrix1, gray_images[index]);
	
	while (1) {
		if (kbhit()) {
			ch = getsw();
			printf("0x%x\n", ch);
			switch (ch) {
			case DEL:
//				switch_display_image = (switch_display_image+1)%3;
				switch_display_image = (switch_display_image+1)%2;
				break;
			case LEFT:
				if (xoff[index] > -display_width) xoff[index]--;
				break;
			case RIGHT:
				if (xoff[index] < display_width) xoff[index]++;
				break;
			case UP:
				if (yoff[index] > -display_height) yoff[index]--;
				break;
			case DOWN:
				if (yoff[index] < display_height) yoff[index]++;
				break;
			case STOP:
				// make directory
				sprintf(dirname, "%s\\shifting", argv[1]);
				mkdir(dirname);
				printf("make directory (%s)\n", dirname);
				// save and statistics
				for (i = 0; i < NR_IMAGE; i++) {
					printf("matrix_be_zero\n");
					matrix_be_zero(matrix2, 0, 0, matrix2->columns, matrix2->rows);
					printf("matrix_copy_bytemap\n");
					matrix_copy_bytemap(
						matrix2,
						xoff[i] > 0 ? xoff[i] : 0,
						yoff[i] > 0 ? yoff[i] : 0,
						gray_images[i],
						xoff[i] < 0 ? -xoff[i] : 0,
						yoff[i] < 0 ? -yoff[i] : 0,
						gray_images[i]->header.width+(xoff[i] < 0 ? xoff[i] : 0),
						gray_images[i]->header.height+(yoff[i] < 0 ? yoff[i] : 0));
					printf("matrix_get_min_mean_max_on_roi\n");
					matrix_get_min_mean_max_on_roi(&vmin, &vmean, &vmax, matrix2, roi);
					printf("matrix2bytemap\n");
					matrix2bytemap(temporary_bytemap, matrix2);
					assert(image_buffer);
					for (j = 0; j < temporary_bytemap->header.height; j++) {
						memcpy(image_buffer+j*image_info->bmiHeader.biWidth,
							temporary_bytemap->buffer+j*temporary_bytemap->header.pitch,
							temporary_bytemap->header.width);
					}
					// save shifting
					printf("save shifting : \n");
					sprintf(filename, "%s\\%d.sht", dirname, num2wavelength[2*(index2num[i]-7)+1]);
					printf("xoff(%d), yoff(%d)\n", xoff[i], yoff[i]);
					printf("filename %s\n", filename);
					fd = fopen(filename, "wb");
					fprintf(fd, "%d, %d\n", xoff[i], yoff[i]);
					fclose(fd);
					// save statistics
					printf("save statistics : \n");
					sprintf(filename, "%s\\%d.txt", dirname, num2wavelength[2*(index2num[i]-7)+1]);
					printf("vmin(%lf), vmean(%lf), vmax(%lf)\n", vmin, vmean, vmax);
					printf("filename %s\n", filename);
					fd = fopen(filename, "wb");
					fprintf(fd, "%lf, %lf, %lf\n", vmin, vmean, vmax);
					fclose(fd);
					sprintf(filename, "./%d.txt", num2wavelength[2*(index2num[i]-7)+1]);
					printf("filename %s\n", filename);
					fd = fopen(filename, "a");
					fprintf(fd, "%lf, %lf, %lf\n", vmin, vmean, vmax);
					fclose(fd);
					// save image
					printf("save image : \n");
					sprintf(filename, "%s\\%d.bmp", dirname, num2wavelength[2*(index2num[i]-7)+1]);
					printf("index %d\n", i);
					printf("filename %s\n", filename);
					printf("image_info->bmiHeader.biWidth %d\n", image_info->bmiHeader.biWidth);
					printf("image_info->bmiHeader.biHeight %d\n", image_info->bmiHeader.biHeight);
					printf("image_info->bmiHeader.biBitCount %d\n", image_info->bmiHeader.biBitCount);
					assert(image_info->bmiHeader.biWidth == gray_images[i]->header.width);
					assert(image_info->bmiHeader.biHeight == gray_images[i]->header.height);
					assert(image_info->bmiHeader.biBitCount == 8);
					SaveDIBitmap(filename, image_info, image_buffer);
				}
				goto __return__;
				break;
			case ENTER:
				index = (index+1)%NR_IMAGE;
				if (index == 0) index = 1;
				sprintf(filename, "%s\\%d.bmp", argv[1], index2num[index]);
				printf("index %d\n", index);
				printf("filename %s\n", filename);
				if (gray_images[index] == NULL) {
					if (image_info) free(image_info);
					if (image_buffer) free(image_buffer);
					image_buffer = LoadDIBitmap(filename, &image_info);
					printf("image_info->bmiHeader.biWidth %d\n", image_info->bmiHeader.biWidth);
					printf("image_info->bmiHeader.biHeight %d\n", image_info->bmiHeader.biHeight);
					printf("image_info->bmiHeader.biBitCount %d\n", image_info->bmiHeader.biBitCount);
					assert(image_info->bmiHeader.biWidth == display_width);
					assert(image_info->bmiHeader.biHeight == display_height);
					assert(image_info->bmiHeader.biBitCount == 8);
					gray_images[index] = bytemap_create(display_width, display_height);
					for (i = 0; i < display_height; i++) {
						memcpy(gray_images[index]->buffer+i*gray_images[index]->header.pitch,
							image_buffer+i*image_info->bmiHeader.biWidth,
							image_info->bmiHeader.biWidth);
					}
					// so terrible slow
					//bytemap_cross_correlation(xcorr, gray_images[0], gray_images[index]);
				}
				break;
			case ESC:
				exit(0);
				break;
			default:
//				exit(0);
				break;
			}

			switch (switch_display_image) {
			case 0:
				display_clear_all_parts(display_buffer);
				display_write_green_image(display_buffer, 0, 0, gray_images[0]);
				break;
			case 1:
				display_clear_all_parts(display_buffer);
				display_write_red_image(display_buffer, xoff[index], yoff[index], gray_images[index]);
				break;
			case 2:
				display_clear_all_parts(display_buffer);
				display_write_mono_image(display_buffer, 0, 0, roi);
				break;
			}
			putimage(0, 0, display_buffer, COPY_PUT);
			matrix_be_zero(matrix2, 0, 0, matrix2->columns, matrix2->rows);
			matrix_copy_bytemap(
				matrix2,
				xoff[index] > 0 ? xoff[index] : 0,
				yoff[index] > 0 ? yoff[index] : 0,
				gray_images[index],
				xoff[index] < 0 ? -xoff[index] : 0,
				yoff[index] < 0 ? -yoff[index] : 0,
				gray_images[index]->header.width+(xoff[index] < 0 ? xoff[index] : 0),
				gray_images[index]->header.height+(yoff[index] < 0 ? yoff[index] : 0));
			matrix_get_correlation_coefficient(&m, matrix1, matrix2);
			printf("correlation coefficient %lf\n", m);
			//bytemap_create(xcorr, gray_images[index], mask);
		}
	}
	
__return__:;

	matrix_delete(matrix2);
	matrix_delete(matrix1);
	bytemap_delete(temporary_bytemap);
	bitmap_delete(roi);
	bitmap_delete(bin);
	for (i = 0; i < NR_IMAGE; i++) {
		bytemap_delete(gray_images[i]);
	}
	if (image_info) free(image_info);
	if (image_buffer) free(image_buffer);
	if (display_buffer) free(display_buffer);
	
	return 0;
	//sprintf("%s\\%d.bmp", 
	//readimagefile(NULL, 0, 0, image_width-1, image_height-1);
	//image1 = (unsigned char *)malloc(imagesize(0, 0, width-1, height-1));
	//getimage(0, 0, image_width-1, image_height-1, image1);
}
