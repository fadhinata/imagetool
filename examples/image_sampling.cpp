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


// for spectrum data
#define NR_IMAGE 24
#define NR_SAMPLE 11
#define REALLOC_LEN 50

#define NORMAL1 0
#define NORMAL2 1
#define TOP1    2
#define TOP2    3
#define TOP3    4
#define BOTTOM1 5
#define BOTTOM2 6
#define SCAB1   7
#define SCAB2   8
#define BRUISE1 9
#define BRUISE2 10

static const char *sample_name[NR_SAMPLE] = {
	"Sound Without Glaring",
	"Sound Under Glaring Region",
	"Top With Cross Section",
	"Top With Branch",
	"Top Under Funnel Region",
	"Bottom With Strong Darkness",
	"Bottom Neiborhood",
	"Scab Brown",
	"Scab Black",
	"Bruise Strong",
	"Bruise Doubtfull"
};

#define LEFT  0x004b
#define RIGHT 0x004d
#define UP    0x0048
#define DOWN  0x0050
#define ENTER 0x000d
#define STOP  0x0020  /* SPACE */
#define FUNC  0x003b  /* F1 */
#define DEL   0x0053
#define ESC   0x001b


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

static int alloc_samples[NR_SAMPLE];
static int count_samples[NR_SAMPLE];
static int *xpos_samples[NR_SAMPLE];
static int *ypos_samples[NR_SAMPLE];
// for multiple images
static bytemap_t *gray_images[NR_IMAGE];
static bytemap_t *roi_image = NULL;

// image dimension
static int display_width = 644;
static int display_height = 492;
// ??????
static int switch_display_image = 0;
static int switch_display_sample = 0;


static int getsw(void)
{
	int ch;
	ch = getch() << 8;
	if (kbhit()) ch |= getch();
	else ch >>= 8;
	return ch;
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

// argv[1] : path of image
int ImageSampling(int argc, char *argv[])
{
	int i, j, k, n, x, y, xpos, ypos, color;
	int ch, sample_index, cascade, save_index;
	char filename[256], dirname[256], str[256];
	double *mbuf, v;
	unsigned char *display_buffer = NULL;
	FILE *fd;
	unsigned char *bbuf, *roibuf;
	int need_display, need_histogram;
	matrix_t *matrix1 = NULL;
	matrix_t *matrix2 = NULL;
	matrix_t *matrix3 = NULL;
	bytemap_t *temporary_bytemap = NULL;
	

	if (argc < 2) {
		printf("Error : No argument that is path of multispectral images!\n");
		return -1;
	}

	initwindow(display_width+10+10*NR_IMAGE+10, display_height, "ImageSampling");
	display_buffer = (unsigned char *)malloc(imagesize(0, 0, display_width-1, display_height-1));
	getimage(0, 0, display_width-1, display_height-1, display_buffer);

	// initialization varibles
	switch_display_image = 0;
	cascade = 0;
	save_index = 0;
	sample_index = 0; // sound
	need_display = 1;
	need_histogram = 0;
	for (i = 0; i < NR_SAMPLE; i++) {
		alloc_samples[i] = REALLOC_LEN;
		count_samples[i] = 0;
		xpos_samples[i] = (int *)malloc(REALLOC_LEN*sizeof(int));
		ypos_samples[i] = (int *)malloc(REALLOC_LEN*sizeof(int));
	}

	// allocation
	for (i = 0; i < NR_IMAGE; i++) {
		gray_images[i] = bytemap_create(display_width, display_height);
	}
	roi_image = bytemap_create(display_width, display_height);
	temporary_bytemap = bytemap_create(display_width, display_height);
	matrix1 = matrix_create(display_width, display_height, 0);
	matrix2 = matrix_create(display_width, display_height, 0);
	matrix3 = matrix_create(display_width, display_height, 0);
	
	// read normalized images
	for (i = 0; i < NR_IMAGE; i++) {
		sprintf(filename, "%s\\normalize by histogram and mean\\%d.bmp", argv[1], num2wavelength[2*i+1]);
		LoadImage(gray_images[i], filename);
	}
	
	// read roi image
	sprintf(filename, "%s\\roi.bmp", argv[1]);
	LoadImage(roi_image, filename);
	
	while (1) {
		if (kbhit()) {
			ch = getsw();
			printf("0x%x\n", ch);
			switch (ch) {
			case LEFT:
				if (cascade == 0) {
					switch_display_image = (switch_display_image-1);
					if (switch_display_image < 0) switch_display_image = NR_IMAGE-1;
					need_display = 1;
				}
				break;
			case RIGHT:
				if (cascade == 0) {
					switch_display_image = (switch_display_image+1)%NR_IMAGE;
					need_display = 1;
				}
				break;
			case UP:
				if (count_samples[sample_index] > 0){
					switch_display_sample = (switch_display_sample+1)%(count_samples[sample_index]);
					need_display = 1;
				}
				break;
			case DOWN:
				if (count_samples[sample_index] > 0){
					switch_display_sample = switch_display_sample-1;
					if (switch_display_sample < 0) switch_display_sample = count_samples[sample_index]-1;
					need_display = 1;
				}
				break;
			case 0x30: // '0' - sound
			case 0x31: // '1' - scab 0
			case 0x32: // '2' - scab 1
			case 0x33: // '3' - BRUISE 0
			case 0x34: // '4' - BRUISE 1
			case 0x35: // '5' - top 0
			case 0x36: // '6' - top 1
			case 0x37: // '7' - bottom 0
			case 0x38: // '8' - bottom 1
			case 0x39: // '9' - ????			
				sample_index = ch-'0';
				switch_display_sample = count_samples[sample_index]-1;
				if (switch_display_sample < 0) switch_display_sample = 0;
				need_display = 1;
				break;
			case 0x61: // 'a'
//			case 0x62: // 'b'
				sample_index = ch-'a'+10;
				switch_display_sample = count_samples[sample_index]-1;
				if (switch_display_sample < 0) switch_display_sample = 0;
				need_display = 1;
				break;
			case 0x13: // CTRL+'s'
				if ((xpos >= 0 && xpos < display_width) &&
					(ypos >= 0 && ypos < display_height) &&
					BYTEMAP_IS_EQAUL(255, roi_image, xpos, ypos)) {
					n = count_samples[sample_index];
					xpos_samples[sample_index][n] = xpos;
					ypos_samples[sample_index][n] = ypos;
					count_samples[sample_index]++;
					if (alloc_samples[sample_index] <= count_samples[sample_index]) {
						alloc_samples[sample_index] += REALLOC_LEN;
						xpos_samples[sample_index] = (int *)realloc(xpos_samples[sample_index], alloc_samples[sample_index]);
						ypos_samples[sample_index] = (int *)realloc(ypos_samples[sample_index], alloc_samples[sample_index]);
					}
					switch_display_sample = n;
					need_display = 1;
				}
				break;
			case STOP:
				// save and exit;
				////////////////////////////////////////////////
				strcpy(str, argv[1]);
				n = strlen(str)-1;
				while (*(str+n) != '\\') n--;
				printf("%s\n", (str+n+1));
				
				printf("save and exit\n");
				for (i = 0; i < NR_SAMPLE; i++) {
					sprintf(filename, "%s.txt", sample_name[i]);
					fd = fopen(filename, "a"); // write and append data
					for (j = 0; j < count_samples[i]; j++) {
						x = xpos_samples[i][j];
						y = ypos_samples[i][j];
						// save spectrum data
						fprintf(fd, "%s %03d %03d ", (str+n+1), x, y);
						for (k = 0; k < NR_IMAGE; k++) {
							fprintf(fd, "%03d ", *(gray_images[k]->buffer+y*gray_images[k]->header.pitch+x));
						}
						fprintf(fd, "\n");
					}
					fclose(fd);
				}
				goto __return__;				
				break;
			case ENTER:
				cascade = ~cascade;
				need_display = 1;
				break;
			case DEL:
				// delete sample (switch_display_sample) and drop down samples
				if (count_samples[sample_index] > 0) {
					for (i = switch_display_sample+1; i < count_samples[sample_index]; i++) {
						xpos_samples[sample_index][i-1] = xpos_samples[sample_index][i];
						ypos_samples[sample_index][i-1] = ypos_samples[sample_index][i];
					}
					count_samples[sample_index]--;
					if (switch_display_sample >= count_samples[sample_index]) {
						if (switch_display_sample > 0) switch_display_sample--;
					}
					need_display = 1;
				}
				//count_samples[sample_index]--;
				//if (count_samples[sample_index] < 0) count_samples[sample_index] = 0;
				
				break;
			case ESC:
				break;
			}
		}
		
		if (ismouseclick(WM_LBUTTONDOWN)) {
			// save position
			x = mousex(), y = mousey();
			if ((x >= 0 && x < display_width) &&
				(y >= 0 && y < display_height) &&
				BYTEMAP_IS_EQAUL(255, roi_image, x, y)) {
				xpos = x;
				ypos = y;
/*
				n = count_samples[sample_index];
				xpos_samples[sample_index][n] = x;
				ypos_samples[sample_index][n] = y;
				count_samples[sample_index]++;
				if (alloc_samples[sample_index] <= count_samples[sample_index]) {
					alloc_samples[sample_index] += REALLOC_LEN;
					xpos_samples[sample_index] = (int *)realloc(xpos_samples[sample_index], alloc_samples[sample_index]);
					ypos_samples[sample_index] = (int *)realloc(ypos_samples[sample_index], alloc_samples[sample_index]);
				}
*/
				need_display = 1;
			}
			clearmouseclick(WM_LBUTTONDOWN);
		}
        if (ismouseclick(WM_MOUSEMOVE)) {
//            cout << "Moving at " << mousex () << ", " << mousey () << endl;
            clearmouseclick(WM_MOUSEMOVE);
        }

		/* display */
		if (need_display) {
			printf("need display\n");
			clearviewport();
			// draw images
			if (cascade) {
				printf("cascaded images\n");
				setcolor(WHITE);
				outtextxy(display_width+5, 0, "cascaded images");
				for (y = 0; y < display_height; y++) {
					for (x = 0; x < display_width; x++) {
						MATRIX_PUT(0, matrix1, x, y);
						for (i = 0; i < NR_IMAGE/3; i++) {
							BYTEMAP_GET(v, gray_images[i], x, y);
							MATRIX_ADD_PUT(v/8.0, matrix1, x, y);
						}
						MATRIX_PUT(0, matrix2, x, y);
						for (; i < 2*NR_IMAGE/3; i++) {
							BYTEMAP_GET(v, gray_images[i], x, y);
							MATRIX_ADD_PUT(v/8.0, matrix2, x, y);
						}
						MATRIX_PUT(0, matrix3, x, y);
						for (; i < NR_IMAGE; i++) {
							BYTEMAP_GET(v, gray_images[i], x, y);
							MATRIX_ADD_PUT(v/8.0, matrix3, x, y);
						}
					}
				}
				matrix2bytemap(temporary_bytemap, matrix1);
				display_write_blue_image(display_buffer, 0, 0, temporary_bytemap);
				matrix2bytemap(temporary_bytemap, matrix2);
				display_write_green_image(display_buffer, 0, 0, temporary_bytemap);
				matrix2bytemap(temporary_bytemap, matrix3);
				display_write_red_image(display_buffer, 0, 0, temporary_bytemap);
				putimage(0, 0, display_buffer, COPY_PUT);
			} else {
				sprintf(filename, "%d.bmp", num2wavelength[2*switch_display_image+1]);
				setcolor(WHITE);
				outtextxy(display_width+5, 0, filename);
				printf("display image by index\n");
				display_write_gray_image(display_buffer, 0, 0, gray_images[switch_display_image]);
				putimage(0, 0, display_buffer, COPY_PUT);
//				getch();
			}
			// draw histogram
			printf("draw histogram\n");
			sprintf(filename, "%s : ", sample_name[sample_index]);
			outtextxy(display_width+5, 15, filename);
			sprintf(filename, "%d @ %d samples", switch_display_sample, count_samples[sample_index]);
			outtextxy(display_width+5, 30, filename);
			// draw base line
			setcolor(WHITE);
			moveto(display_width+5, 300);
			lineto(display_width+5+(5+10*NR_IMAGE+5), 300);
			moveto(display_width+5, 300);
			lineto(display_width+5, 150);

			if ((xpos >= 0 && xpos < display_width) &&
				(ypos >= 0 && ypos < display_height) &&
				BYTEMAP_IS_EQAUL(255, roi_image, xpos, ypos)) {
				// draw sampled spectrum
				setcolor(YELLOW);
				moveto(display_width+10, 300-((*(gray_images[0]->buffer+ypos*gray_images[0]->header.pitch+xpos))>>1));
				for (i = 1; i < NR_IMAGE; i++) {
					lineto(display_width+10+i*10, 300-((*(gray_images[i]->buffer+ypos*gray_images[i]->header.pitch+xpos))>>1));
				}
				/* display position */
				//setfillstyle(SOLID_FILL, YELLOW);
				//fillellipse(xpos, ypos, 3, 3);
				//setcolor(YELLOW);
				//circle(xpos, ypos, 3);
				putpixel(xpos-1, ypos, YELLOW);
				putpixel(xpos+1, ypos, YELLOW);
				putpixel(xpos, ypos, YELLOW);
				putpixel(xpos, ypos-1, YELLOW);
				putpixel(xpos, ypos+1, YELLOW);
			}
				
			if (count_samples[sample_index] > 0) {
				// draw saved spectrum
				n = switch_display_sample;
				
				for (i = 0; i < count_samples[sample_index]; i++) {
					x = xpos_samples[sample_index][i];
					y = ypos_samples[sample_index][i];
					
					if (i == n) color = GREEN;
					else color = RED;
					
					// draw spectrum graph
					setcolor(color);
					moveto(display_width+10, 300-((*(gray_images[0]->buffer+y*gray_images[0]->header.pitch+x))>>1));
					for (j = 1; j < NR_IMAGE; j++) {
						lineto(display_width+10+j*10, 300-((*(gray_images[j]->buffer+y*gray_images[j]->header.pitch+x))>>1));
					}
					/* display position */
					//setfillstyle(SOLID_FILL, RED);
					//fillellipse(x, y, 3, 3);
					//setcolor(RED);
					//circle(x, y, 3);
					putpixel(x-1, y, color);
					putpixel(x+1, y, color);
					putpixel(x, y, color);
					putpixel(x, y-1, color);
					putpixel(x, y+1, color);
				}

			}
			need_display = 0;
		}
	}
	
__return__:;
	printf("de-allocation\n");
	matrix_delete(matrix3);
	matrix_delete(matrix2);
	matrix_delete(matrix1);
	bytemap_delete(temporary_bytemap);
	bytemap_delete(roi_image);
	for (i = 0; i < NR_IMAGE; i++) {
		bytemap_delete(gray_images[i]);
	}
	for (i = 0; i < NR_SAMPLE; i++) {
		free(xpos_samples[i]);
		free(ypos_samples[i]);
	}
	free(display_buffer);
	
	return 0;
}
