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
#include <common.h>

// image dimension
static int display_width = 640;
static int display_height = 480;
// for multiple images
static bytemap_t *gray_images[3] = NULL;
static bitmap_t *roi_image = NULL;
static unsigned char *display_buffer = NULL;

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
/* argv[0] : imagetool.exe
 * argv[1] : path of images
 * argv[2] : the name of apple
 * argv[3] : the number of a denominator
 * argv[4] : the number of a numerator
 * argv[5] : cutup threshold
 * argv[6] : cutdown threshold
 */
int TripleDivide(int argc, char *argv[])
{
	int i, j, k, n, x, y, m;
	char filename[256], dirname[256];
	double v, sum;
	double *mbuf;
	unsigned char *bbuf, *roibuf;
	bytemap_t *bytemap;
	
	if (argc < 7) {
		printf("Error : No argument that is path of multispectral images!\n");
		return -1;
	}

	initwindow(display_width, display_height, "ImageNormalize");
	display_buffer = (unsigned char *)malloc(imagesize(0, 0, display_width-1, display_height-1));
	getimage(0, 0, display_width-1, display_height-1, display_buffer);

	roi_image = bitmap_create(display_width, display_height);
	for (i = 0; i < 3; i++) gray_images[i] = bytemap_create(display_width, display_height);
	bytemap = bytemap_create(display_width, display_height);
	
	sprintf(filename, "%s\\%sROI.bmp", argv[1], argv[2]);
	readimagefile(filename);
	getimage(0, 0,  display_width-1, display_height-1, display_buffer);
	display_read_read_image(bytemap, display_buffer);
	getch();
	
	bytemap_delete(bytemap);
	for (i = 0; i < 3; i++) bytemap_delete(gray_image[i]);
	bitmap_delete(roi_image);
	free(display_buffer);
	
	return 0;
}

