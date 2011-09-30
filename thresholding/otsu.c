/* Copyright 2011 Hoyoung Yi. This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with this program; if not, please visit www.gnu.org.*/#include <stdio.h>#include <malloc.h>#include <limits.h>#include <histogram.h>#include <threshold.h>#include <triangle.h>#include <assert.h>#if 0/* calculation of probability density */for(int i=0; i<MAXVAL; i++){  pdf[i] = (real_t)hist[i] / (image_width * image_height); }    /* cdf & myu generation */cdf[0] = pdf[0];myu[0] = 0.0;       /* 0.0 times prob[0] equals zero */for(int i=1; i<MAXVAL; i++){  cdf[i] = cdf[i-1] + pdf[i];  myu[i] = myu[i-1] + i*pdf[i]; }  int threshold = 0;max_sigma = 0.0;for(int i=0; i<MAXVAL-1; i++){  if(cdf[i] != 0.0 && cdf[i] != 1.0){    real_t p1p2 = cdf[i]*(1.0 - cdf[i]);    real_t mu1mu2diff = myu[MAXVAL-1]*cdf[i]-myu[i];    sigma[i] = mu1mu2diff * mu1mu2diff / p1p2;  }  else    sigma[i] = 0.0;  if(sigma[i] > max_sigma){    max_sigma = sigma[i];    threshold = i;  } }#endifint otsu_level(int *hist, int n){  int i, tlevel;  real_t *pdf, *cdf, *mean, pixels;  real_t meandiff, prob, sigma, maxval;	  assert(hist);  assert(n > 0);  pdf = (real_t *)malloc(n*sizeof(*pdf));  //cdf = (real_t *)malloc(n*sizeof(*cdf));  mean = (real_t *)malloc(n*sizeof(*mean));  assert(pdf);  //assert(cdf);  assert(mean);  pixels = 0.0;  for (i = 0; i < n; i++) pixels += hist[i];	  pdf[0] = (real_t)hist[0]/pixels;  //cdf[0] = pdf[0];  mean[0] = 0.0;  for (i = 1; i < n; i++) {    pdf[i] = (real_t)hist[i]/pixels; // probability density function    //cdf[i] += pdf[i]; // Cumulative distribution function    mean[i] += mean[i-1]+(real_t)(i+1)*pdf[i];  }      /* sigma maximization     sigma stands for inter-class variance      and determines optimal threshold value */  tlevel = 0;  prob = 0.0;  maxval = 0.0;  for (i = 0; i < n; i++) {    prob += pdf[i];    meandiff = mean[i]-mean[n-1]*prob;    sigma = meandiff*meandiff/(prob*(1.0-prob));    if (sigma > maxval) {      maxval = sigma;      tlevel = i;    }  }	  free(mean);  //free(cdf);  free(pdf);	  return tlevel;}void bytemap_otsu_threshold(bitmap_t *bin, bytemap_t *im){  int tlevel;  int hist[UINT8_MAX+1];  assert(bin);  assert(im);  assert(bytemap_get_width(im) == bitmap_get_width(bin));  assert(bytemap_get_height(im) == bitmap_get_height(bin));	  bytemap_histogram((int *)hist, im);  tlevel = otsu_level((int *)hist, UINT8_MAX+1);  bytemap_threshold(bin, im, tlevel, UINT8_MAX);}void bytemap_otsu_threshold_on_roi(bitmap_t *bin, bytemap_t *im, bitmap_t *roi){  int tlevel;  int hist[UINT8_MAX+1];  assert(bin);  assert(im);  assert(bytemap_get_width(im) == bitmap_get_width(bin));  assert(bytemap_get_height(im) == bitmap_get_height(bin));  assert(bytemap_get_width(im) == bitmap_get_width(roi));  assert(bytemap_get_height(im) == bitmap_get_height(roi));	  bytemap_histogram_on_roi((int *)hist, im, roi);  tlevel = otsu_level((int *)hist, UINT8_MAX+1);  bytemap_threshold_on_roi(bin, im, tlevel, UINT8_MAX, roi);}void bytemap_otsu_threshold_on_region(bitmap_t *bin, bytemap_t *im, int x, int y, int dx, int dy){  int tlevel;  int hist[UINT8_MAX+1];  assert(bin);  assert(im);  assert(bytemap_get_width(im) == bitmap_get_width(bin));  assert(bytemap_get_height(im) == bitmap_get_height(bin));  assert(x >= 0 && x < bitmap_get_width(bin));  assert(y >= 0 && y < bitmap_get_height(bin));  assert(dx > 0);  assert(dy > 0);	  bytemap_histogram_on_region((int *)hist, im, x, y, dx, dy);  tlevel = otsu_level((int *)hist, UINT8_MAX+1);  bytemap_threshold_on_region(bin, im, tlevel, UINT8_MAX, x, y, dx, dy);}void wordmap_otsu_threshold(bitmap_t *bin, wordmap_t *im){  int tlevel;  int hist[UINT16_MAX+1];  assert(bin);  assert(im);  assert(wordmap_get_width(im) == bitmap_get_width(bin));  assert(wordmap_get_height(im) == bitmap_get_height(bin));	  wordmap_histogram((int *)hist, im);  tlevel = otsu_level((int *)hist, UINT16_MAX+1);  wordmap_threshold(bin, im, tlevel, UINT16_MAX);}void wordmap_otsu_threshold_on_roi(bitmap_t *bin, wordmap_t *im, bitmap_t *roi){  int tlevel;  int hist[UINT16_MAX+1];  assert(bin);  assert(im);  assert(wordmap_get_width(im) == bitmap_get_width(bin));  assert(wordmap_get_height(im) == bitmap_get_height(bin));  assert(wordmap_get_width(im) == bitmap_get_width(roi));  assert(wordmap_get_height(im) == bitmap_get_height(roi));	  wordmap_histogram_on_roi((int *)hist, im, roi);  tlevel = otsu_level((int *)hist, UINT16_MAX+1);  wordmap_threshold_on_roi(bin, im, tlevel, UINT16_MAX, roi);}void wordmap_otsu_threshold_on_region(bitmap_t *bin, wordmap_t *im, int x, int y, int dx, int dy){  int tlevel;  int hist[UINT16_MAX+1];  assert(bin);  assert(im);  assert(wordmap_get_width(im) == bitmap_get_width(bin));  assert(wordmap_get_height(im) == bitmap_get_height(bin));  assert(x >= 0 && x < bitmap_get_width(bin));  assert(y >= 0 && y < bitmap_get_height(bin));  assert(dx > 0);  assert(dy > 0);	  wordmap_histogram_on_region((int *)hist, im, x, y, dx, dy);  tlevel = otsu_level((int *)hist, UINT16_MAX+1);  wordmap_threshold_on_region(bin, im, tlevel, UINT16_MAX, x, y, dx, dy);}