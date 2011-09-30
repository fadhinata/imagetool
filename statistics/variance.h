#ifndef __VARIANCE_H__
#define __VARIANCE_H__

#include <bitmap.h>
#include <bytemap.h>
#include <wordmap.h>
#include <dwordmap.h>

#ifdef __cplusplus
extern "C" {
#endif

int bytemap_get_variance(double *var, bytemap_t *im, double mean);
int wordmap_get_variance(double *var, wordmap_t *im, double mean);
int dwordmap_get_variance(double *var, dwordmap_t *im, double mean);
int floatmap_get_variance(double *var, floatmap_t *im, double mean);
int bytemap_get_variance_on_roi(double *var, bytemap_t *im, double mean, bitmap_t *roi);
int wordmap_get_variance_on_roi(double *var, wordmap_t *im, double mean, bitmap_t *roi);
int dwordmap_get_variance_on_roi(double *var, dwordmap_t *im, double mean, bitmap_t *roi);
int floatmap_get_variance_on_roi(double *var, floatmap_t *im, double mean, bitmap_t *roi);
int bytemap_get_variance_on_region(double *var, bytemap_t *im, double mean, int x, int y, int dx, int dy);
int wordmap_get_variance_on_region(double *var, wordmap_t *im, double mean, int x, int y, int dx, int dy);
int dwordmap_get_variance_on_region(double *var, dwordmap_t *im, double mean, int x, int y, int dx, int dy);
int floatmap_get_variance_on_region(double *var, floatmap_t *im, double mean, int x, int y, int dx, int dy);

int bytemap_get_standard_deviation(double *std, bytemap_t *im, double mean);
int wordmap_get_standard_deviation(double *std, wordmap_t *im, double mean);
int dwordmap_get_standard_deviation(double *std, dwordmap_t *im, double mean);
int floatmap_get_standard_deviation(double *std, floatmap_t *im, double mean);
int bytemap_get_standard_deviation_on_roi(double *std, bytemap_t *im, double mean, bitmap_t *roi);
int wordmap_get_standard_deviation_on_roi(double *std, wordmap_t *im, double mean, bitmap_t *roi);
int dwordmap_get_standard_deviation_on_roi(double *std, dwordmap_t *im, double mean, bitmap_t *roi);
int floatmap_get_standard_deviation_on_roi(double *std, floatmap_t *im, double mean, bitmap_t *roi);
int bytemap_get_standard_deviation_on_region(double *std, bytemap_t *im, double mean, int x, int y, int dx, int dy);
int wordmap_get_standard_deviation_on_region(double *std, wordmap_t *im, double mean, int x, int y, int dx, int dy);
int dwordmap_get_standard_deviation_on_region(double *std, dwordmap_t *im, double mean, int x, int y, int dx, int dy);
int floatmap_get_standard_deviation_on_region(double *std, floatmap_t *im, double mean, int x, int y, int dx, int dy);

#ifdef __cplusplus
}
#endif

#endif /* __VARIANCE_H__ */
