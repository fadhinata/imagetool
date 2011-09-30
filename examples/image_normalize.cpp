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
#include <chaincode.h>
#include <common.h>

#define NR_APPLE 58
#define NR_IMAGE 24
#define NR_SPECIES 9

static int getsw(void)
{
	int ch;
	ch = getch() << 8;
	if (kbhit()) ch |= getch();
	else ch >>= 8;
	return ch;
}

static char *apple_dirname[NR_APPLE] = {
	"1-1",
	"1-2",
	"1-3",
	"2-1",
	"2-2",
	"2-3",
	"3-1",
	"3-2",
	"3-3",
	"4-1",
	"4-2",
	"4-3",
	"5-1",
	"5-2",
	"5-3",
	"6-1",
	"6-2",
	"6-3",
	"7-1",
	"7-2",
	"7-3",
	"8-1",
	"8-2",
	"8-3",
	"9-1",
	"9-2",
	"10-1",
	"10-2",
	"10-3",
	"11-1",
	"11-2",
	"11-3",
	"11-4",
	"12-1",
	"12-2",
	"12-3",
	"13-1",
	"13-2",
	"13-3",
	"14-1",
	"14-2",
	"14-3",
	"15-1",
	"15-2",
	"15-3",
	"15-4",
	"16-1",
	"16-2",
	"16-3",
	"17-1",
	"17-2",
	"17-3",
	"18-1",
	"18-2",
	"18-3",
	"19-1",
	"19-2",
	"19-3"
};

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

// for multiple images
static bytemap_t *gray_images[NR_IMAGE];
static matrix_t *gray_matrices[NR_IMAGE];
static bitmap_t *roi_images[NR_APPLE];
static unsigned long histograms[NR_IMAGE][256];
static int min_values[NR_APPLE][NR_IMAGE];
static int mean_values[NR_APPLE][NR_IMAGE];
static int max_values[NR_APPLE][NR_IMAGE];

// for general purpose
static bytemap_t *temporary_bytemap = NULL;
// image dimension
static int display_width = 644;
static int display_height = 492;
// switch image to display
static int switch_display_image = 0;
	
#define LEFT  0x004b
#define RIGHT 0x004d
#define UP    0x0048
#define DOWN  0x0050
#define ENTER 0x000d
#define STOP  0x0020  /* SPACE */
#define FUNC  0x003b  /* F1 */
#define DEL   0x0053
#define ESC   0x001b

static void LoadImage(bytemap_t *image, const char *filename)
{
	int i, j;
	unsigned char *image_buffer = NULL;
	BITMAPINFO *image_info = NULL;

	//if (image_info) free(image_info);
	image_buffer = LoadDIBitmap(filename, &image_info);
#if 0
	assert(image_info->bmiHeader.biWidth == display_width);
	assert(image_info->bmiHeader.biHeight == display_height);
	assert(image_info->bmiHeader.biBitCount == 8);
	printf("image_info->bmiHeader.biSize %d\n", image_info->bmiHeader.biSize);
	printf("image_info->bmiHeader.biWidth %d\n", image_info->bmiHeader.biWidth);
	printf("image_info->bmiHeader.biHeight %d\n", image_info->bmiHeader.biHeight);
	printf("image_info->bmiHeader.biPlanes %d\n", image_info->bmiHeader.biPlanes);
	printf("image_info->bmiHeader.biBitCount %d\n", image_info->bmiHeader.biBitCount);
	printf("image_info->bmiHeader.biCompression %d\n", image_info->bmiHeader.biCompression);
	printf("image_info->bmiHeader.biSizeImage %d\n", image_info->bmiHeader.biSizeImage);
	printf("image_info->bmiHeader.biXPelsPerMeter %d\n", image_info->bmiHeader.biXPelsPerMeter);
	printf("image_info->bmiHeader.biYPelsPerMeter %d\n", image_info->bmiHeader.biYPelsPerMeter);
	printf("image_info->bmiHeader.biClrUsed %d\n", image_info->bmiHeader.biClrUsed);
	printf("image_info->bmiHeader.biClrImportant %d\n", image_info->bmiHeader.biClrImportant);
//	assert(0);
#endif
	// read to bytemap
	for (j = 0; j < image->header.height; j++) {
		memcpy(image->buffer+j*image->header.pitch,
			image_buffer+j*image_info->bmiHeader.biWidth,
			image_info->bmiHeader.biWidth);
	}
	free(image_info);
	free(image_buffer);
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

static void GetROIImage(bitmap_t *roi, bytemap_t *gray)
{
	double m;
	bitmap_t *bin;
	bytemap_t *se;
	assert(roi);
	assert(gray);
	assert(roi->header.width == gray->header.width);
	assert(roi->header.height == gray->header.height);
	bin = bitmap_create(gray->header.width, gray->header.height);
	se = bytemap_create(3, 3);
	// region of interest
	//printf("ROI setting\n");
	bytemap_get_mean(&m, gray);
	bytemap_threshold(bin, gray, m, 255);
	label_pickup_by_largest_area(roi, bin);
	bytemap_fill(se, 0, 0, se->header.width, se->header.height, 1);
	binary_erode(bin, roi, se);
	label_fill_holes(roi, bin);
	bitmap_delete_shell(roi, 3);
	bytemap_delete(se);
	bitmap_delete(bin);
}
		
int ImageNormalize(int argc, char *argv[])
{
	int i, j, k, n, x, y, count;
	int ch, species_index, cascade, save_index;
	char filename[256], dirname[256];
	double m, v, vmin, vmean, vmax;
	double *mbuf;
	unsigned char *display_buffer = NULL;
	unsigned char *bbuf, *roibuf;
	
	if (argc < 2) {
		printf("Error : No argument that is path of multispectral images!\n");
		return -1;
	}

	initwindow(display_width+10+10*NR_IMAGE+10, display_height, "ImageNormalize");
	display_buffer = (unsigned char *)malloc(imagesize(0, 0, display_width-1, display_height-1));
	getimage(0, 0, display_width-1, display_height-1, display_buffer);

	for (n = 0; n < NR_APPLE; n++) {
		roi_images[n] = bitmap_create(display_width, display_height);
	}
	
	for (n = 0; n < NR_IMAGE; n++) {
		for (i = 0; i < 256; i++) histograms[n][i] = 0;
		gray_images[n] = bytemap_create(display_width, display_height);
		gray_matrices[n] = matrix_create(display_width, display_height, 0);
	}

	temporary_bytemap = bytemap_create(display_width, display_height);

	// analysis images
	for (n = 0; n < NR_APPLE; n++) {
		sprintf(dirname, "%s\\%s", argv[1], apple_dirname[n]);
		printf("current directory %s\n", dirname);
		for (i = 0; i < NR_IMAGE; i++) {
			sprintf(filename, "%s\\%d.bmp", dirname, num2wavelength[2*i+1]);
			//printf("filename %s\n", filename);
			LoadImage(gray_images[i], filename);
		}
		//LoadImages(gray_images, dirname);
		GetROIImage(roi_images[n], gray_images[NR_IMAGE/2]);
//		display_write_mono_image(display_buffer, 0, 0, roi);
//		putimage(0, 0, display_buffer, COPY_PUT);
//		getch();

		// analysis image
		for (i = 0; i < NR_IMAGE; i++) {
			min_values[n][i] = 255;
			mean_values[n][i] = 0;
			max_values[n][i] = 0;
			count = 0;
			bbuf = gray_images[i]->buffer;
			roibuf = roi_images[n]->buffer;
			for (y = 0; y < gray_images[i]->header.height; y++) {
				for (x = 0; x < gray_images[i]->header.width; x++) {
					if ((*(roibuf+(x>>3)))&(1<<(x%8))) {
						v = *(bbuf+x);
						if (v < min_values[n][i]) min_values[n][i] = v;
						mean_values[n][i] += v;
						if (v > max_values[n][i]) max_values[n][i] = v;
						count++;
						histograms[i][*(bbuf+x)]++;
//						BYTEMAP_PUT(*(bbuf+x), temporary_bytemap, x, y);
					} else {
//						BYTEMAP_PUT(128, temporary_bytemap, x, y);
					}
				}
				bbuf += gray_images[i]->header.pitch;
				roibuf += roi_images[n]->header.pitch;
			}
			mean_values[n][i] /= (double)count;
//			display_write_gray_image(display_buffer, 0, 0, temporary_bytemap);
//			putimage(0, 0, display_buffer, COPY_PUT);
//			getch();
		}
	}
	
	// save roi images
	for (n = 0; n < NR_APPLE; n++) {
		sprintf(filename, "%s\\%s\\roi.bmp", argv[1], apple_dirname[n]);
		roibuf = roi_images[n]->buffer;
		for (i = 0; i < display_height; i++) {
			for (j = 0; j < display_width; j++) {
				if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
					*(temporary_bytemap->buffer+i*temporary_bytemap->header.pitch+j) = 255;
				} else {
					*(temporary_bytemap->buffer+i*temporary_bytemap->header.pitch+j) = 0;
				}
			}
			roibuf += roi_images[n]->header.pitch;
		}
		SaveImage(filename, temporary_bytemap);
	}

	printf("save images\n");
	for (n = 0; n < NR_APPLE; n++) {
		// load images
		sprintf(dirname, "%s\\%s", argv[1], apple_dirname[n]);
		printf("current directory %s\n", dirname);
		for (i = 0; i < NR_IMAGE; i++) {
			sprintf(filename, "%s\\%d.bmp", dirname, num2wavelength[2*i+1]);
			LoadImage(gray_images[i], filename);
		}

		// make directory
		sprintf(dirname, "%s\\%s\\normalize by histogram and mean", argv[1], apple_dirname[n]);
		mkdir(dirname);
		printf("make directory (%s)\n", dirname);
		// normalized by histogram
		for (i = 0; i < NR_IMAGE; i++) {
			for (j = 0; histograms[i][j] == 0; j++);
			vmin = j;
			for (j = 255; histograms[i][j] == 0; j--);
			vmax = j;
			vmean = 0;
			count = 0;
			for (j = 0; j < 256; j++) {
				vmean += histograms[i][j]*j;
				count += histograms[i][j];
			}
			vmean /= (double)count;
			
			// normalize images			
			bbuf = gray_images[i]->buffer;
			mbuf = gray_matrices[i]->real;
			roibuf = roi_images[n]->buffer;
			for (y = 0; y < gray_images[i]->header.height; y++) {
				for (x = 0; x < gray_images[i]->header.width; x++) {
					if ((*(roibuf+(x>>3)))&(1<<(x%8))) {
						v = *(bbuf+x);
						// piecewise linear interpolation
						if (v > vmean) {
							v = (128.0-0.0)/((vmax-vmean)-0.0) * (v-vmean) + 127.0;
						} else {
							v = (-127.0-0.0)/((vmin-vmean)-0.0) * (v-vmean) + 127.0;
						}
						*(mbuf+x) = v;
					} else {
						*(mbuf+x) = 0;
					}
				}
				bbuf += gray_images[i]->header.pitch;
				mbuf += gray_matrices[i]->columns;
				roibuf += roi_images[n]->header.pitch;
			}
			matrix2bytemap(temporary_bytemap, gray_matrices[i]);
			sprintf(filename, "%s\\%d.bmp", dirname, num2wavelength[2*i+1]);
			SaveImage(filename, temporary_bytemap);
			/*
			for (j = 0; j < display_height; j++) {
				memcpy(image_buffer+j*image_info->bmiHeader.biWidth,
					temporary_bytemap->buffer+j*temporary_bytemap->header.pitch,
					temporary_bytemap->header.width);
			}
			sprintf(filename, "%s\\%d.bmp", dirname, num2wavelength[2*i+1]);
			SaveDIBitmap(filename, image_info, image_buffer);
			*/
//			display_write_gray_image(display_buffer, 0, 0, temporary_bytemap);
//			putimage(0, 0, display_buffer, COPY_PUT);
//			getch();
		}
	}
	for (i = 0; i < NR_IMAGE; i++) {
		bytemap_delete(gray_images[i]);
		matrix_delete(gray_matrices[i]);
	}

	for (n = 0; n < NR_APPLE; n++) {
		bitmap_delete(roi_images[n]);
	}

	bytemap_delete(temporary_bytemap);
	if (display_buffer) free(display_buffer);
	
	return 0;
}

