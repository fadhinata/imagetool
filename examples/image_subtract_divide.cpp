#include <stdio.h>
#include <malloc.h>
#include <dir.h>
#include <assert.h>
#include <bmp.h>
#include <bytemap.h>
#include <winbgim_io.h>

// for spectrum data
#define NR_IMAGE 24
#define NR_SAMPLE 11

static const int wavelength[NR_IMAGE] = {
	530, 550, 570, 590, 610, 630, 650, 700, 720, 740,
	760, 780, 800, 820, 840, 860, 880, 900, 920, 940,
	960, 980, 1000, 1050
};

static bytemap_t *CreateImage(const char *filename)
{
	int i, j;
	bytemap_t *image = NULL;
	unsigned char *image_buffer = NULL;
	BITMAPINFO *image_info = NULL;

	if (image_info) free(image_info);
	image_buffer = LoadDIBitmap(filename, &image_info);
	assert(image_buffer);
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
#endif
	// read to bytemap
	image = bytemap_create(image_info->bmiHeader.biWidth, image_info->bmiHeader.biHeight);
	for (j = 0; j < image->header.height; j++) {
		memcpy(image->buffer+(image->header.height-1-j)*image->header.pitch,
			image_buffer+j*image_info->bmiHeader.biWidth,
			image_info->bmiHeader.biWidth);
	}
	free(image_info);
	free(image_buffer);
	return image;
}

static void LoadImage(bytemap_t *image, const char *filename)
{
	int i, j;
	unsigned char *image_buffer = NULL;
	BITMAPINFO *image_info = NULL;

	if (image_info) free(image_info);
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
#endif
	// read to bytemap
	for (j = 0; j < image->header.height; j++) {
		memcpy(image->buffer+(image->header.height-1-j)*image->header.pitch,
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

// argv[0] : program name
// argv[1] : path of image
// argv[2] : op image's wavelength
// argv[3] : op image's wavelength
// argv[4] : op image's wavelength
// argv[5] : cutup threshold value
// argv[6] : cutdown threshold value
int ImageSubtractDivide(int argc, char *argv[])
{
	int i, j;
	char filename[256], dirname[256];
	bytemap_t *roi;
	bytemap_t *op1;
	bytemap_t *op2;
	bytemap_t *op3;
	bytemap_t *result;
	int display_width, display_height;
	int value, cutup, cutdown;
	unsigned char *display_buffer;
	
	if (argc < 7) {
		printf("Error : No argument that is path of multispectral images!\n");
		return -1;
	}
	
	printf("Load roi image\n");
	sprintf(filename, "%s\\roi.bmp", argv[1]);
	roi = CreateImage(filename);
	
	printf("Load opand 1 image\n");
	sprintf(filename, "%s\\%s.bmp", argv[1], argv[2]);
	op1 = CreateImage(filename);
	
	printf("Load opand 2 image\n");
	sprintf(filename, "%s\\%s.bmp", argv[1], argv[3]);
	op2 = CreateImage(filename);

	printf("Load opand 3 image\n");
	sprintf(filename, "%s\\%s.bmp", argv[1], argv[4]);
	op3 = CreateImage(filename);
	
	assert(roi->header.width == op1->header.width);
	assert(roi->header.height == op1->header.height);
	assert(op1->header.width == op2->header.width);
	assert(op1->header.height == op2->header.height);
	assert(op2->header.width == op3->header.width);
	assert(op2->header.height == op3->header.height);
	
	printf("Read threshold value (i.e. cutup and cutdown level)\n");
	cutup = atoi(argv[5]);
	cutdown = atoi(argv[6]);
	
	printf("Create display buffer\n");
	display_width = roi->header.width;
	display_height = roi->header.height;
	initwindow(display_width, display_height, "ImageSubtract");
	display_buffer = (unsigned char *)malloc(imagesize(0, 0, display_width-1, display_height-1));
	assert(display_buffer);
	getimage(0, 0, display_width-1, display_height-1, display_buffer);
	
	printf("Display ROI image\n");
	display_write_gray_image(display_buffer, 0, 0, roi);
	putimage(0, 0, display_buffer, COPY_PUT);
	getch();
	
	printf("Display Operand 1 image\n");
	display_write_gray_image(display_buffer, 0, 0, op1);
	putimage(0, 0, display_buffer, COPY_PUT);
	getch();

	printf("Display Operand 2 image\n");
	display_write_gray_image(display_buffer, 0, 0, op2);
	putimage(0, 0, display_buffer, COPY_PUT);
	getch();

	printf("Display Operand 3 image\n");
	display_write_gray_image(display_buffer, 0, 0, op3);
	putimage(0, 0, display_buffer, COPY_PUT);
	getch();

	result = bytemap_create(display_width, display_height);
	
	/* image subtraction */
	for (i = 0; i < display_height; i++) {
		for (j = 0; j < display_width; j++) {
			if (*(roi->buffer+i*(roi->header.pitch)+j) != 0) {
				value = (int)(*(op1->buffer+i*(op1->header.pitch)+j))-(int)(*(op2->buffer+i*(op2->header.pitch)+j));
				value = (int)(1000.0*(double)value/(double)((*(op3->buffer+i*(op3->header.pitch)+j)+1.0)));
				if (((cutup < cutdown) && ((cutup <= value) && (value <= cutdown))) ||
					((cutup > cutdown) && ((value <= cutdown) && (cutup <= value)))) {
					*(result->buffer+i*(result->header.pitch)+j) = 255;
				} else {
					*(result->buffer+i*(result->header.pitch)+j) = 0;
				}
			} else {
				*(result->buffer+i*(result->header.pitch)+j) = 0;
			}
		}
	}
	
	printf("Display Result image\n");
	display_write_gray_image(display_buffer, 0, 0, result);
	putimage(0, 0, display_buffer, COPY_PUT);
	getch();

	sprintf(dirname, "%s\\%snm - %snm over %snm between %s and %s", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
	mkdir(dirname);
	sprintf(filename, "%s\\result.bmp", dirname);
	SaveImage(filename, result);	

	display_write_gray_image(display_buffer, 0, 0, op1);
	display_write_red_image(display_buffer, 0, 0, result);
	putimage(0, 0, display_buffer, COPY_PUT);
	getch();
	sprintf(filename, "%s\\result on %snm.bmp", dirname, argv[2]);
	writeimagefile(filename);

	display_write_gray_image(display_buffer, 0, 0, op2);
	display_write_red_image(display_buffer, 0, 0, result);
	putimage(0, 0, display_buffer, COPY_PUT);
	getch();
	sprintf(filename, "%s\\result on %snm.bmp", dirname, argv[3]);
	writeimagefile(filename);
	
	bytemap_delete(result);
	free(display_buffer);
	bytemap_delete(op3);
	bytemap_delete(op2);
	bytemap_delete(op1);
	bytemap_delete(roi);
	
	return 0;
}
