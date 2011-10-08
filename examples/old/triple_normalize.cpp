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
#include <median_smooth.h>
#include <convert.h>
#include <otsu.h>
#include <isodata.h>
#include <common.h>

#define NR_APPLE 32

// image dimension
static int display_width = 640;
static int display_height = 480;
// for multiple images
static bytemap_t *gray_image = NULL;
static bitmap_t *roi_image = NULL;
static unsigned long histograms[3][256];
static int min_value[3], max_value[3];
static double mean_value[3];
static unsigned char *display_buffer = NULL;

//#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)

static void LoadImage(bytemap_t *image, const char *filename)
{
	int i, j, widthbytes;
	unsigned char *image_buffer = NULL;
	BITMAPINFO *image_info = NULL;
	

	//if (image_info) free(image_info);
	image_buffer = LoadDIBitmap(filename, &image_info);

	assert(image_info->bmiHeader.biWidth == display_width);
	assert(abs(image_info->bmiHeader.biHeight) == display_height);

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
	if (image_info->bmiHeader.biBitCount == 8) {
		// read to bytemap
		for (j = 0; j < image->header.height; j++) {
			memcpy(image->buffer+j*image->header.pitch,
				image_buffer+j*image_info->bmiHeader.biWidth,
				image_info->bmiHeader.biWidth);
		}
	} else if (image_info->bmiHeader.biBitCount == 24) {
		widthbytes = WIDTHBYTES(image_info->bmiHeader.biBitCount*image_info->bmiHeader.biWidth);
		for (i = 0; i < image->header.height; i++) {
			for (j = 0; j < image->header.width; j++) {
				if (image_info->bmiHeader.biHeight < 0) {
					*(image->buffer+i*(image->header.pitch)+j) = *(image_buffer+i*widthbytes+(3*j+0)); // red
					//*(image->buffer+i*(image->header.pitch)+j) = *(image_buffer+i*widthbytes+(3*j+1)); // green
					//*(image->buffer+i*(image->header.pitch)+j) = *(image_buffer+i*widthbytes+(3*j+2)); // blue
				} else {
					*(image->buffer+i*(image->header.pitch)+j) = *(image_buffer+(abs(image_info->bmiHeader.biHeight)-1-i)*widthbytes+(3*j+0)); // red
					//*(image->buffer+i*(image->header.pitch)+j) = *(image_buffer+i*widthbytes+(3*j+1)); // green
					//*(image->buffer+i*(image->header.pitch)+j) = *(image_buffer+i*widthbytes+(3*j+2)); // blue
				}
			}
		}
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
	image_info->bmiHeader.biHeight = -(gray->header.height);
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
	//bytemap_isodata_threshold(bin, gray);
	label_pickup_by_largest_area(roi, bin);
	//display_write_mono_image(display_buffer, 0, 0, roi);
	//putimage(0, 0, display_buffer, COPY_PUT);
	//getch();
	bytemap_fill(se, 0, 0, se->header.width, se->header.height, 1);
	binary_erode(bin, roi, se);
	binary_erode(roi, bin, se);
	label_pickup_by_largest_area(bin, roi);
	label_fill_holes(roi, bin);
	//bitmap_delete_shell(bin, 3);
	// add for more performance
	
	//label_fill_holes(roi, bin);
	bitmap_delete_shell(roi, 3);
	

	bytemap_delete(se);
	bitmap_delete(bin);
}


int TripleNormalize(int argc, char *argv[])
{
	int i, j, k, n, x, y, m;
	char filename[256], dirname[256];
	double v, sum;
	double *mbuf;
	unsigned char *bbuf, *roibuf;
	bytemap_t *bytemap;
	
	if (argc < 2) {
		printf("Error : No argument that is path of multispectral images!\n");
		return -1;
	}

	initwindow(display_width, display_height, "ImageNormalize");
	display_buffer = (unsigned char *)malloc(imagesize(0, 0, display_width-1, display_height-1));
	getimage(0, 0, display_width-1, display_height-1, display_buffer);

	roi_image = bitmap_create(display_width, display_height);
	gray_image = bytemap_create(display_width, display_height);
	bytemap = bytemap_create(display_width, display_height);
	
	for (i = 0; i < 256; i++) {
		histograms[0][i] = histograms[1][i] = histograms[2][i] = 0;
	}
	
	sprintf(dirname, "%s\\normalize", argv[1]);
	mkdir(dirname);
	
	printf("Statistics:\n");
	for (n = 0; n < NR_APPLE; n++) {
		for (m = 0; m < 3; m++) {
			printf("%d-%d\n", n+1, m+1);
			// roi image
			sprintf(filename, "%s\\%d-%dCam3.bmp", argv[1], n+1, m+1);
			//readimagefile(filename, 0, 0, display_width-1, display_height-1);
			//getimage(0, 0, display_width-1, display_height-1, display_buffer);
			//display_read_red_image(gray_image, display_buffer);
			LoadImage(gray_image, filename);
			median_smooth(bytemap, gray_image, 3);
			GetROIImage(roi_image, bytemap);
			//GetROIImage(roi_image, gray_image);
			
			display_write_mono_image(display_buffer, 0, 0, roi_image);
			putimage(0, 0, display_buffer, COPY_PUT);
			//getch();
			sprintf(filename, "%s\\%d-%dROI.bmp", dirname, n+1, m+1);
			//writeimagefile(filename, 0, 0, display_width-1, display_height-1);
			bitmap2bytemap(bytemap, roi_image);
			SaveImage(filename, bytemap);
			
			// for cam1 image
			sprintf(filename, "%s\\%d-%dCam1.bmp", argv[1], n+1, m+1);
			LoadImage(gray_image, filename);
			bbuf = gray_image->buffer;
			roibuf = roi_image->buffer;
			for (y = 0; y < gray_image->header.height; y++) {
				for (x = 0; x < gray_image->header.width; x++) {
					if ((*(roibuf+(x>>3)))&(1<<(x%8))) {
						histograms[0][*(bbuf+x)]++;
					}
				}
				bbuf += gray_image->header.pitch;
				roibuf += roi_image->header.pitch;
			}

			// for cam2 image
			sprintf(filename, "%s\\%d-%dCam2.bmp", argv[1], n+1, m+1);
			LoadImage(gray_image, filename);
			bbuf = gray_image->buffer;
			roibuf = roi_image->buffer;
			for (y = 0; y < gray_image->header.height; y++) {
				for (x = 0; x < gray_image->header.width; x++) {
					if ((*(roibuf+(x>>3)))&(1<<(x%8))) {
						histograms[1][*(bbuf+x)]++;
					}
				}
				bbuf += gray_image->header.pitch;
				roibuf += roi_image->header.pitch;
			}
			// for cam3 image
			sprintf(filename, "%s\\%d-%dCam3.bmp", argv[1], n+1, m+1);
			LoadImage(gray_image, filename);
			bbuf = gray_image->buffer;
			roibuf = roi_image->buffer;
			for (y = 0; y < gray_image->header.height; y++) {
				for (x = 0; x < gray_image->header.width; x++) {
					if ((*(roibuf+(x>>3)))&(1<<(x%8))) {
						histograms[2][*(bbuf+x)]++;
					}
				}
				bbuf += gray_image->header.pitch;
				roibuf += roi_image->header.pitch;
			}
		}
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 256; j++) {
			if (histograms[i][j] != 0) break;
		}
		min_value[i] = j;
		
		for (j = 255; j >= 0; j--) {
			if (histograms[i][j] != 0) break;
		}
		max_value[i] = j;

		mean_value[i] = 0.0;
		sum = 0.0;
		for (j = min_value[i]; j <= max_value[i]; j++) {
			mean_value[i] += (double)j*(double)histograms[i][j];
			sum += (double)histograms[i][j];
		}
		mean_value[i] /= (double)sum;
	}
	
	FILE *fd;
	sprintf(filename, "%s\\Statistics.txt", dirname);
	fd = fopen(filename, "wb");
	fprintf(fd, "min(%d) mean(%lf) max(%d) for Cam1\n", min_value[0], mean_value[0], max_value[0]);
	fprintf(fd, "min(%d) mean(%lf) max(%d) for Cam2\n", min_value[1], mean_value[1], max_value[1]);
	fprintf(fd, "min(%d) mean(%lf) max(%d) for Cam3\n", min_value[2], mean_value[2], max_value[2]);
	fclose(fd);
	
	// normalization
	printf("normalization:\n");
	for (n = 0; n < NR_APPLE; n++) {
		for (m = 0; m < 3; m++) {
			// roi image
			printf("%d-%d\n", n+1, m+1);
			sprintf(filename, "%s\\%d-%dROI.bmp", dirname, n+1, m+1);
			LoadImage(bytemap, filename);
			bytemap2bitmap(roi_image, bytemap);
		
			// for cam1 image
			sprintf(filename, "%s\\%d-%dCam1.bmp", argv[1], n+1, m+1);
			LoadImage(gray_image, filename);
			bbuf = gray_image->buffer;
			roibuf = roi_image->buffer;
			for (y = 0; y < gray_image->header.height; y++) {
				for (x = 0; x < gray_image->header.width; x++) {
					if ((*(roibuf+(x>>3)))&(1<<(x%8))) {
						v = *(bbuf+x);
						if (v > mean_value[0]) {
							v = (128.0-0.0)/((max_value[0]-mean_value[0])-0.0) * (v-mean_value[0]) + 127.0;
						} else {
							v = (-127.0-0.0)/((min_value[0]-mean_value[0])-0.0) * (v-mean_value[0]) + 127.0;
						}
						*(bbuf+x) = (unsigned char)v;
					} else {
						*(bbuf+x) = 0;
					}
				}
				bbuf += gray_image->header.pitch;
				roibuf += roi_image->header.pitch;
			}
			display_write_gray_image(display_buffer, 0, 0, gray_image);
			putimage(0, 0, display_buffer, COPY_PUT);
			//getch();
			sprintf(filename, "%s\\%d-%dCam1.bmp", dirname, n+1, m+1);
			//writeimagefile(filename, 0, 0, display_width-1, display_height-1);
			SaveImage(filename, gray_image);
			
			// for cam2 image
			sprintf(filename, "%s\\%d-%dCam2.bmp", argv[1], n+1, m+1);
			LoadImage(gray_image, filename);
			bbuf = gray_image->buffer;
			roibuf = roi_image->buffer;
			for (y = 0; y < gray_image->header.height; y++) {
				for (x = 0; x < gray_image->header.width; x++) {
					if ((*(roibuf+(x>>3)))&(1<<(x%8))) {
						v = *(bbuf+x);
						if (v > mean_value[1]) {
							v = (128.0-0.0)/((max_value[1]-mean_value[1])-0.0) * (v-mean_value[1]) + 127.0;
						} else {
							v = (-127.0-0.0)/((min_value[1]-mean_value[1])-0.0) * (v-mean_value[1]) + 127.0;
						}
						*(bbuf+x) = (unsigned char)v;
					} else {
						*(bbuf+x) = 0;
					}
				}
				bbuf += gray_image->header.pitch;
				roibuf += roi_image->header.pitch;
			}
			display_write_gray_image(display_buffer, 0, 0, gray_image);
			putimage(0, 0, display_buffer, COPY_PUT);
			//getch();
			sprintf(filename, "%s\\%d-%dCam2.bmp", dirname, n+1, m+1);
			//writeimagefile(filename, 0, 0, display_width-1, display_height-1);
			SaveImage(filename, gray_image);
			
			// for cam3 image
			sprintf(filename, "%s\\%d-%dCam3.bmp", argv[1], n+1, m+1);
			LoadImage(gray_image, filename);
			bbuf = gray_image->buffer;
			roibuf = roi_image->buffer;
			for (y = 0; y < gray_image->header.height; y++) {
				for (x = 0; x < gray_image->header.width; x++) {
					if ((*(roibuf+(x>>3)))&(1<<(x%8))) {
						v = *(bbuf+x);
						if (v > mean_value[2]) {
							v = (128.0-0.0)/((max_value[2]-mean_value[2])-0.0) * (v-mean_value[2]) + 127.0;
						} else {
							v = (-127.0-0.0)/((min_value[2]-mean_value[2])-0.0) * (v-mean_value[2]) + 127.0;
						}
						*(bbuf+x) = (unsigned char)v;
					} else {
						*(bbuf+x) = 0;
					}
				}
				bbuf += gray_image->header.pitch;
				roibuf += roi_image->header.pitch;
			}
			display_write_gray_image(display_buffer, 0, 0, gray_image);
			putimage(0, 0, display_buffer, COPY_PUT);
			//getch();
			sprintf(filename, "%s\\%d-%dCam3.bmp", dirname, n+1, m+1);
			//writeimagefile(filename, 0, 0, display_width-1, display_height-1);
			SaveImage(filename, gray_image);
		}
	}
	
	bytemap_delete(bytemap);
	bytemap_delete(gray_image);
	bitmap_delete(roi_image);
	free(display_buffer);
	
	return 0;
}

