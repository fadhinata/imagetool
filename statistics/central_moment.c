/*
 Copyright 2011 Hoyoung Yi.

 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, please visit www.gnu.org.
*/
#include <stdio.h>
#include <math.h>
#include <central_moment.h>
#undef NDEBUG
#include <assert.h>

#undef min
#define min(x, y) ((x) > (y) ? (y) : (x))

#define GENERAL_MAP_GET_CENTRAL_MOMENTS(type, m2, m3, m4, im, mean) { \
	int n; \
	int i, j, pitch; \
	type *buf; \
	double m1, tmp; \
	assert(m2); \
	assert(m3); \
	assert(m4); \
	assert(im); \
	*m2 = *m3 = *m4 = 0.0, n = 0; \
	buf = im->buffer, pitch = im->header.pitch/sizeof(type); \
	for (i = 0; i < im->header.height; i++) { \
		for (j = 0; j < im->header.width; j++) { \
			m1 = (double)(*(buf+j))-mean; \
			tmp = m1*m1; \
			*m2 += tmp; \
			tmp *= m1; \
			*m3 += tmp; \
			tmp *= m1; \
			*m4 += tmp; \
			n++; \
		} \
		buf += pitch; \
	} \
	if (n > 0) { \
		*m2 /= (double)n; \
		*m3 /= (double)n; \
		*m4 /= (double)n; \
	} \
	return n; \
}

#define GENERAL_MAP_GET_CENTRAL_MOMENTS_ON_ROI(type, m2, m3, m4, im, mean, roi) { \
	int n; \
	int i, j, pitch; \
	type *buf; \
	unsigned char *roibuf; \
	double m1, tmp; \
	assert(m2); \
	assert(m3); \
	assert(m4); \
	assert(im); \
	assert(roi); \
	assert(im->header.width == roi->header.width); \
	assert(im->header.height == roi->header.height); \
	*m2 = *m3 = *m4 = 0.0, n = 0; \
	buf = im->buffer, pitch = im->header.pitch/sizeof(type); \
	roibuf = roi->buffer; \
	for (i = 0; i < im->header.height; i++) { \
		for (j = 0; j < im->header.width; j++) { \
			if ((*(roibuf+(j>>3)))&(1<<(j%8))) { \
				m1 = (double)(*(buf+j))-mean; \
				tmp = m1*m1; \
				*m2 += tmp; \
				tmp *= m1; \
				*m3 += tmp; \
				tmp *= m1; \
				*m4 += tmp; \
				n++; \
			} \
		} \
		buf += pitch; \
		roibuf += roi->header.pitch; \
	} \
	if (n > 0) { \
		*m2 /= (double)n; \
		*m3 /= (double)n; \
		*m4 /= (double)n; \
	} \
	return n; \
}

#define GENERAL_MAP_GET_CENTRAL_MOMENTS_ON_REGION(type, m2, m3, m4, im, mean, x, y, dx, dy) { \
	int n; \
	int i, j, width, height, pitch; \
	type *buf; \
	double tmp, m1; \
	assert(m2); \
	assert(m3); \
	assert(m4); \
	assert(im); \
	assert(x >= 0 && x < im->header.width); \
	assert(y >= 0 && y < im->header.height); \
	assert(dx > 0); \
	assert(dy > 0); \
	*m2 = *m3 = *m4 = 0.0, n = 0; \
	width = min(x+dx, im->header.width); \
	height = min(y+dy, im->header.height); \
	pitch = im->header.pitch/sizeof(type); \
	buf = im->buffer+y*pitch; \
	for (i = y; i < height; i++) { \
		for (j = x; j < width; j++) { \
			m1 = (double)(*(buf+j))-mean; \
			tmp = m1*m1; \
			*m2 += tmp; \
			tmp *= m1; \
			*m3 += tmp; \
			tmp *= m1; \
			*m4 += tmp; \
			n++; \
		} \
		buf += pitch; \
	} \
	if (n > 0) { \
		*m2 /= (double)n; \
		*m3 /= (double)n; \
		*m4 /= (double)n; \
	} \
	return n; \
}

int bytemap_get_central_moments(double *m2, double *m3, double *m4, bytemap_t *im, double mean) { GENERAL_MAP_GET_CENTRAL_MOMENTS(unsigned char, m2, m3, m4, im, mean); }
int wordmap_get_central_moments(double *m2, double *m3, double *m4, wordmap_t *im, double mean) { GENERAL_MAP_GET_CENTRAL_MOMENTS(unsigned short, m2, m3, m4, im, mean); }
int dwordmap_get_central_moments(double *m2, double *m3, double *m4, dwordmap_t *im, double mean) { GENERAL_MAP_GET_CENTRAL_MOMENTS(unsigned long, m2, m3, m4, im, mean); }
int floatmap_get_central_moments(double *m2, double *m3, double *m4, floatmap_t *im, double mean) { GENERAL_MAP_GET_CENTRAL_MOMENTS(float, m2, m3, m4, im, mean); }
int bytemap_get_central_moments_on_roi(double *m2, double *m3, double *m4, bytemap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_CENTRAL_MOMENTS_ON_ROI(unsigned char, m2, m3, m4, im, mean, roi); }
int wordmap_get_central_moments_on_roi(double *m2, double *m3, double *m4, wordmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_CENTRAL_MOMENTS_ON_ROI(unsigned short, m2, m3, m4, im, mean, roi); }
int dwordmap_get_central_moments_on_roi(double *m2, double *m3, double *m4, dwordmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_CENTRAL_MOMENTS_ON_ROI(unsigned long, m2, m3, m4, im, mean, roi); }
int floatmap_get_central_moments_on_roi(double *m2, double *m3, double *m4, floatmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_CENTRAL_MOMENTS_ON_ROI(float, m2, m3, m4, im, mean, roi); }
int bytemap_get_central_moments_on_region(double *m2, double *m3, double *m4, bytemap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_CENTRAL_MOMENTS_ON_REGION(unsigned char, m2, m3, m4, im, mean, x, y, dx, dy); }
int wordmap_get_central_moments_on_region(double *m2, double *m3, double *m4, wordmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_CENTRAL_MOMENTS_ON_REGION(unsigned short, m2, m3, m4, im, mean, x, y, dx, dy); }
int dwordmap_get_central_moments_on_region(double *m2, double *m3, double *m4, dwordmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_CENTRAL_MOMENTS_ON_REGION(unsigned long, m2, m3, m4, im, mean, x, y, dx, dy); }
int floatmap_get_central_moments_on_region(double *m2, double *m3, double *m4, floatmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_CENTRAL_MOMENTS_ON_REGION(float, m2, m3, m4, im, mean, x, y, dx, dy); }

#define GENERAL_MAP_GET_SKEWNESS(maptype, skewness, im, mean) { \
	int n; \
	double m2, m3, m4, std, tmp; \
	assert(skewness); \
	assert(im); \
	n = maptype##_get_central_moments(&m2, &m3, &m4, im, mean); \
	if (m2 != 0.0) { \
		std = sqrt(m2); \
		tmp = std; \
		tmp *= std; \
		tmp *= std; \
		*skewness = m3/tmp; \
	} \
	return n; \
}

#define GENERAL_MAP_GET_SKEWNESS_ON_ROI(maptype, skewness, im, mean, roi) {\
	int n; \
	double m2, m3, m4, std, tmp; \
	assert(skewness); \
	assert(im); \
	assert(roi); \
	assert(im->header.width == roi->header.width); \
	assert(im->header.height == roi->header.height); \
	n = maptype##_get_central_moments_on_roi(&m2, &m3, &m4, im, mean, roi); \
	if (m2 != 0.0) { \
		std = sqrt(m2); \
		tmp = std; \
		tmp *= std; \
		tmp *= std; \
		*skewness = m3/tmp; \
	} \
	return n; \
}

#define GENERAL_MAP_GET_SKEWNESS_ON_REGION(maptype, skewness, im, mean, x, y, dx, dy) {\
	int n; \
	double m2, m3, m4, std, tmp; \
	assert(skewness); \
	assert(im); \
	assert(x >= 0 && x < im->header.width); \
	assert(y >= 0 && y < im->header.height); \
	assert(dx > 0); \
	assert(dy > 0); \
	n = maptype##_get_central_moments_on_region(&m2, &m3, &m4, im, mean, x, y, dx, dy); \
	if (m2 != 0.0) { \
		std = sqrt(m2); \
		tmp = std; \
		tmp *= std; \
		tmp *= std; \
		*skewness = m3/tmp; \
	} \
	return n; \
}

int bytemap_get_skewness(double *skewness, bytemap_t *im, double mean) { GENERAL_MAP_GET_SKEWNESS(bytemap, skewness, im, mean); }
int wordmap_get_skewness(double *skewness, wordmap_t *im, double mean) { GENERAL_MAP_GET_SKEWNESS(wordmap, skewness, im, mean); }
int dwordmap_get_skewness(double *skewness, dwordmap_t *im, double mean) { GENERAL_MAP_GET_SKEWNESS(dwordmap, skewness, im, mean); }
int floatmap_get_skewness(double *skewness, floatmap_t *im, double mean) { GENERAL_MAP_GET_SKEWNESS(floatmap, skewness, im, mean); }
int bytemap_get_skewness_on_roi(double *skewness, bytemap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_SKEWNESS_ON_ROI(bytemap, skewness, im, mean, roi); }
int wordmap_get_skewness_on_roi(double *skewness, wordmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_SKEWNESS_ON_ROI(wordmap, skewness, im, mean, roi); }
int dwordmap_get_skewness_on_roi(double *skewness, dwordmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_SKEWNESS_ON_ROI(dwordmap, skewness, im, mean, roi); }
int floatmap_get_skewness_on_roi(double *skewness, floatmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_SKEWNESS_ON_ROI(floatmap, skewness, im, mean, roi); }
int bytemap_get_skewness_on_region(double *skewness, bytemap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_SKEWNESS_ON_REGION(bytemap, skewness, im, mean, x, y, dx, dy); }
int wordmap_get_skewness_on_region(double *skewness, wordmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_SKEWNESS_ON_REGION(wordmap, skewness, im, mean, x, y, dx, dy); }
int dwordmap_get_skewness_on_region(double *skewness, dwordmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_SKEWNESS_ON_REGION(dwordmap, skewness, im, mean, x, y, dx, dy); }
int floatmap_get_skewness_on_region(double *skewness, floatmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_SKEWNESS_ON_REGION(floatmap, skewness, im, mean, x, y, dx, dy); }

#define GENERAL_MAP_GET_KURTOSIS(maptype, kurtosis, im, mean) { \
	int n; \
	double m2, m3, m4, tmp; \
	assert(kurtosis); \
	assert(im); \
	n = maptype##_get_central_moments(&m2, &m3, &m4, im, mean); \
	if (m2 != 0.0) { \
		tmp = m2; \
		tmp *= m2; \
		*kurtosis = m4/tmp; \
	} \
	return n; \
}

#define GENERAL_MAP_GET_KURTOSIS_ON_ROI(maptype, kurtosis, im, mean, roi) { \
	int n; \
	double m2, m3, m4, tmp; \
	assert(kurtosis); \
	assert(im); \
	assert(roi); \
	assert(im->header.width == roi->header.width); \
	assert(im->header.height == roi->header.height); \
	n = maptype##_get_central_moments_on_roi(&m2, &m3, &m4, im, mean, roi); \
	if (m2 != 0.0) { \
		tmp = m2; \
		tmp *= m2; \
		*kurtosis = m4/tmp; \
	} \
	return n; \
}

#define GENERAL_MAP_GET_KURTOSIS_ON_REGION(maptype, kurtosis, im, mean, x, y, dx, dy) { \
	int n; \
	double m2, m3, m4, tmp; \
	assert(kurtosis); \
	assert(im); \
	assert(x >= 0 && x < im->header.width); \
	assert(y >= 0 && y < im->header.height); \
	assert(dx > 0); \
	assert(dy > 0); \
	n = maptype##_get_central_moments_on_region(&m2, &m3, &m4, im, mean, x, y, dx, dy); \
	if (m2 != 0.0) { \
		tmp = m2; \
		tmp *= m2; \
		*kurtosis = m4/tmp; \
	} \
	return n; \
}

int bytemap_get_kurtosis(double *kurtosis, bytemap_t *im, double mean) { GENERAL_MAP_GET_KURTOSIS(bytemap, kurtosis, im, mean); }
int wordmap_get_kurtosis(double *kurtosis, wordmap_t *im, double mean) { GENERAL_MAP_GET_KURTOSIS(bytemap, kurtosis, im, mean); }
int dwordmap_get_kurtosis(double *kurtosis, dwordmap_t *im, double mean) { GENERAL_MAP_GET_KURTOSIS(dwordmap, kurtosis, im, mean); }
int floatmap_get_kurtosis(double *kurtosis, floatmap_t *im, double mean) { GENERAL_MAP_GET_KURTOSIS(floatmap, kurtosis, im, mean); }
int bytemap_get_kurtosis_on_roi(double *kurtosis, bytemap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_KURTOSIS_ON_ROI(bytemap, kurtosis, im, mean, roi); }
int wordmap_get_kurtosis_on_roi(double *kurtosis, wordmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_KURTOSIS_ON_ROI(wordmap, kurtosis, im, mean, roi); }
int dwordmap_get_kurtosis_on_roi(double *kurtosis, dwordmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_KURTOSIS_ON_ROI(dwordmap, kurtosis, im, mean, roi); }
int floatmap_get_kurtosis_on_roi(double *kurtosis, floatmap_t *im, double mean, bitmap_t *roi) { GENERAL_MAP_GET_KURTOSIS_ON_ROI(floatmap, kurtosis, im, mean, roi); }
int bytemap_get_kurtosis_on_region(double *kurtosis, bytemap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_KURTOSIS_ON_REGION(bytemap, kurtosis, im, mean, x, y, dx, dy); }
int wordmap_get_kurtosis_on_region(double *kurtosis, wordmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_KURTOSIS_ON_REGION(wordmap, kurtosis, im, mean, x, y, dx, dy); }
int dwordmap_get_kurtosis_on_region(double *kurtosis, dwordmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_KURTOSIS_ON_REGION(dwordmap, kurtosis, im, mean, x, y, dx, dy); }
int floatmap_get_kurtosis_on_region(double *kurtosis, floatmap_t *im, double mean, int x, int y, int dx, int dy) { GENERAL_MAP_GET_KURTOSIS_ON_REGION(floatmap, kurtosis, im, mean, x, y, dx, dy); }
