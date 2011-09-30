#ifndef __STATUS_H__
#define __STATUS_H__

#include <matrix.h>
#include <bitmap.h>

#ifdef __cplusplus
extern "C" {
#endif

int matrix_get_mean(double *real, double *imag, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);
int matrix_get_rmean(double *mean, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);
int matrix_get_imean(double *mean, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);

int matrix_get_variance(double *var, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);
int matrix_get_rvariance(double *var, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);
int matrix_get_ivariance(double *var, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);

int matrix_get_std(double *std, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);
int matrix_get_rstd(double *std, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);
int matrix_get_istd(double *std, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);

int matrix_get_kurtosis(double *kur, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);
int matrix_get_rkurtosis(double *kur, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);
int matrix_get_ikurtosis(double *kur, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);

int matrix_get_skewness(double *skew, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);
int matrix_get_rskewness(double *skew, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);
int matrix_get_iskewness(double *skew, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);

int matrix_get_status(double *rmean, double *imean, double *var, double *std, double *kur, double *skew,
						matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows);

#ifdef __cplusplus
}
#endif

#endif /* __STATUS_H__ */
