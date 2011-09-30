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
static bytemap_t *gray_images[3];
static bitmap_t *roi_image = NULL;
static unsigned char *display_buffer = NULL;
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
				*(image->buffer+i*(image->header.pitch)+j) = *(image_buffer+i*widthbytes+(3*j+0)); // red
				//*(image->buffer+i*(image->header.pitch)+j) = *(image_buffer+i*widthbytes+(3*j+1)); // green
				//*(image->buffer+i*(image->header.pitch)+j) = *(image_buffer+i*widthbytes+(3*j+2)); // blue
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
	double *mbuf;
	unsigned char *bbuf, *roibuf, *binbuf;
	unsigned char *abuf, *cbuf;
	bytemap_t *bytemap;
	bitmap_t *bin;
	bytemap_t *se;
	
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
	bin = bitmap_create(display_width, display_height);
	
	printf("Load ROI image\n");
	sprintf(filename, "%s\\%sROI.bmp", argv[1], argv[2]);
	LoadImage(bytemap, filename);
	bytemap2bitmap(roi_image, bytemap);

	int denominator, numerator;
	
	denominator = atoi(argv[3]);
	sprintf(filename, "%s\\%sCam%d.bmp", argv[1], argv[2], denominator);
	LoadImage(gray_images[0], filename);
	
	numerator = atoi(argv[4]);
	sprintf(filename, "%s\\%sCam%d.bmp", argv[1], argv[2], numerator);
	LoadImage(gray_images[1], filename);
	
	int cutup, cutdown;
	cutup = atoi(argv[5]);
	cutdown = atoi(argv[6]);
	printf("cutup (%d) cutdown (%d)\n", cutup, cutdown);
	abuf = gray_images[0]->buffer;
	bbuf = gray_images[1]->buffer;
	cbuf = gray_images[2]->buffer;
	roibuf = roi_image->buffer;
	binbuf = bin->buffer;
	for (i = 0; i < display_height; i++) {
		for (j = 0; j < display_width; j++) {
			if ((*(roibuf+(j>>3)))&(1<<(j%8))) {
				m = (int)(1000.0*(double)(*(abuf+j))/(double)((int)(*(bbuf+j))+1));
				//printf("%d ", m);
				if ((cutup <= m) && (m <= cutdown)) {
					*(cbuf+j) = 255;
					*(binbuf+(j>>3)) |= (1<<(j%8));
				} else {
					*(cbuf+j) = 0;
					*(binbuf+(j>>3)) &= ~(1<<(j%8));
				}
			} else {
				*(cbuf+j) = 0;
				*(binbuf+(j>>3)) &= ~(1<<(j%8));
			}
		}
		abuf += gray_images[0]->header.pitch;
		bbuf += gray_images[1]->header.pitch;
		cbuf += gray_images[2]->header.pitch;
		roibuf += roi_image->header.pitch;
		binbuf += bin->header.pitch;
	}
	
	display_write_gray_image(display_buffer, 0, 0, gray_images[0]);
	se = bytemap_create(3, 3);
	bytemap_fill(se, 0, 0, se->header.width, se->header.height, 1);
	//binary_erode(bin, roi, se);
	binary_opening(roi_image, bin, se);
	display_write_mono_image_at_red_part(display_buffer, 0, 0, roi_image);
	//display_write_red_image(display_buffer, 0, 0, gray_images[2]);
	putimage(0, 0, display_buffer, COPY_PUT);
	sprintf(dirname, "%s\\result", argv[1]);
	mkdir(dirname);
	sprintf(filename, "%s\\%s_%dDiv%d_%d_%d.bmp", dirname, argv[2], denominator, numerator, cutup, cutdown);
	writeimagefile(filename, 0, 0, display_width-1, display_height-1);
	//getch();
	bitmap_delete(bin);
	bytemap_delete(bytemap);
	for (i = 0; i < 3; i++) bytemap_delete(gray_images[i]);
	bitmap_delete(roi_image);
	free(display_buffer);
	
	return 0;
}

