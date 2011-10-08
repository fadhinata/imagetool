#include <stdio.h>
#include <math.h>
#include <variance.h>
#undef NDEBUG
#include <assert.h>

#undef min
#define min(x, y) ((x) > (y) ? (y) : (x))

#define GENERAL_MAP_GET_VARIANCE(type, var, im, mean) { \
	int n; \
	int i, j, pitch; \
	type *buf; \
	double tmp; \
	assert(var); \
	assert(im); \
	*var = 0.0, n = 0; \
	buf = im->buffer, pitch = im->header.pitch/sizeof(type); \
	for (i = 0; i < im->header.height; i++) { \
		for (j = 0; j < im->header.width; j++) { \
			tmp = (double)(*(buf+j))-mean; \
			*var += tmp*tmp; \
			n++; \
		} \
		buf += pitch; \
	} \
	if (n > 0) *var /= (double)n; \
	return n; \
}

#define GENERAL_MAP_GET_VARIANCE_ON_ROI(type, var, im, mean, roi) { \
	int n; \
	int i, j, pitch; \
	type *buf; \
	unsigned char *roibuf; \
	double tmp; \
	assert(var); \
	assert(im); \
	assert(roi); \
	assert(im->header.width == roi->header.width); \
	assert(im->header.height == roi->header.height); \
	*var = 0.0, n = 0; \
	buf = im->buffer, pitch = im->header.pitch/sizeof(type); \
	roibuf = roi->buffer; \
	for (i = 0; i < im->header.height; i++) { \
		for (j = 0; j < im->header.width; j++) { \
			if ((*(roibuf+(j>>3)))&(1<<(j%8))) { \
				tmp = (double)(*(buf+j))-mean; \
				*var += tmp*tmp; \
				n++; \
			} \
		} \
		buf += pitch; \
		roibuf += roi->header.pitch; \
	} \
	if (n > 0) *var /= n; \
	return n; \
}

#define GENERAL_MAP_GET_VARIANCE_ON_REGION(type, var, im, mean, x, y, dx, dy) { \
	int n; \
	int i, j, width, height, pitch; \
	type *buf; \
	double tmp; \
	assert(var); \
	assert(im); \
	assert(x >= 0 && x < im->header.width); \
	assert(y >= 0 && y < im->header.height); \
	assert(dx > 0); \
	assert(dy > 0); \
	*var = 0.0, n = 0; \
	width = min(x+dx, im->header.width); \
	height = min(y+dy, im->header.height); \
	pitch = im->header.pitch/sizeof(type); \
	buf = im->buffer+y*pitch; \
	for (i = y; i < height; i++) { \
		for (j = x; j < width; j++) { \
			tmp = (double)(*(buf+j))-mean; \
			*var += tmp*tmp; \
			n++; \
		} \
		buf += pitch; \
	} \
	if (n > 0) *var /= n; \
	return n; \
}

int bytemap_get_variance(double *var, bytemap_t *im, double mean) { GENERAL_MAP_GET_VARIANCE(unsigned char, var, im, mean); }
int wordmap_get_variance(double *var, wordmap_t *im, double mean) { GENERAL_MAP_GET_VARIANCE(unsigned short, var, im, mean); }
int dwordmap_get_variance(double *var, dwordmap_t *im, double mean) { GENERAL_MAP_GET_VARIANCE(unsigned long, var, im, mean); }
int floatmap_get_variance(double *var, floatmap_t *im, double mean) { GENERAL_MAP_GET_VARIANCE(float, var, im, mean); }
int bytemap_get_variance_on_roi(double *var, bytemap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_VARIANCE_ON_ROI(unsigned char, var, im, mean, roi); }
int wordmap_get_variance_on_roi(double *var, wordmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_VARIANCE_ON_ROI(unsigned short, var, im, mean, roi); }
int dwordmap_get_variance_on_roi(double *var, dwordmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_VARIANCE_ON_ROI(unsigned long, var, im, mean, roi); }
int floatmap_get_variance_on_roi(double *var, floatmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_VARIANCE_ON_ROI(float, var, im, mean, roi); }
int bytemap_get_variance_on_region(double *var, bytemap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_VARIANCE_ON_REGION(unsigned char, var, im, mean, x, y, dx, dy); }
int wordmap_get_variance_on_region(double *var, wordmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_VARIANCE_ON_REGION(unsigned short, var, im, mean, x, y, dx, dy); }
int dwordmap_get_variance_on_region(double *var, dwordmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_VARIANCE_ON_REGION(unsigned long, var, im, mean, x, y, dx, dy); }
int floatmap_get_variance_on_region(double *var, floatmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_VARIANCE_ON_REGION(float, var, im, mean, x, y, dx, dy); }

#define GENERAL_MAP_GET_STANDARD_DEVIATION(maptype, std, im, mean) {\
	int n; \
	assert(std); \
	assert(im); \
	n = maptype##_get_variance(std, im, mean); \
	if (n > 0) *std = sqrt(*std); \
	return n; \
}

#define GENERAL_MAP_GET_STANDARD_DEVIATION_ON_ROI(maptype, std, im, mean, roi) { \
	int n; \
	assert(std); \
	assert(im); \
	assert(roi); \
	n = maptype##_get_variance_on_roi(std, im, mean, roi); \
	if (n > 0) *std = sqrt(*std); \
	return n; \
}

#define GENERAL_MAP_GET_STANDARD_DEVIATION_ON_REGION(maptype, std, im, mean, x, y, dx, dy) {\
	int n; \
	assert(std); \
	assert(im); \
	assert(x >= 0 && x < im->header.width); \
	assert(y >= 0 && y < im->header.height); \
	assert(dx > 0); \
	assert(dy > 0); \
	n = maptype##_get_variance_on_region(std, im, mean, x, y, dx, dy); \
	if (n > 0) *std = sqrt(*std); \
	return n; \
}

int bytemap_get_standard_deviation(double *std, bytemap_t *im, double mean) { GENERAL_MAP_GET_STANDARD_DEVIATION(bytemap, std, im, mean); }
int wordmap_get_standard_deviation(double *std, wordmap_t *im, double mean) { GENERAL_MAP_GET_STANDARD_DEVIATION(wordmap, std, im, mean); }
int dwordmap_get_standard_deviation(double *std, dwordmap_t *im, double mean) { GENERAL_MAP_GET_STANDARD_DEVIATION(dwordmap, std, im, mean); }
int floatmap_get_standard_deviation(double *std, floatmap_t *im, double mean) { GENERAL_MAP_GET_STANDARD_DEVIATION(floatmap, std, im, mean); }
int bytemap_get_standard_deviation_on_roi(double *std, bytemap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_STANDARD_DEVIATION_ON_ROI(bytemap, std, im, mean, roi); }
int wordmap_get_standard_deviation_on_roi(double *std, wordmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_STANDARD_DEVIATION_ON_ROI(wordmap, std, im, mean, roi); }
int dwordmap_get_standard_deviation_on_roi(double *std, dwordmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_STANDARD_DEVIATION_ON_ROI(dwordmap, std, im, mean, roi); }
int floatmap_get_standard_deviation_on_roi(double *std, floatmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_STANDARD_DEVIATION_ON_ROI(floatmap, std, im, mean, roi); }
int bytemap_get_standard_deviation_on_region(double *std, bytemap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_STARNDARD_DEVIATION_ON_REGION(bytemap, std, im, mean); }
int wordmap_get_standard_deviation_on_region(double *std, wordmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_STARNDARD_DEVIATION_ON_REGION(wordmap, std, im, mean); }
int dwordmap_get_standard_deviation_on_region(double *std, dwordmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_STARNDARD_DEVIATION_ON_REGION(dwordmap, std, im, mean); }
int floatmap_get_standard_deviation_on_region(double *std, floatmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_STARNDARD_DEVIATION_ON_REGION(floatmap, std, im, mean); }
