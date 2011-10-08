#include <stdio.h>
#include <string.h>
#include <math.h>
#include <status.h>
#include <assert.h>

#undef abs
#define abs(x) ((x) > 0 ? (x) : -(x))
#undef sqr
#define sqr(x) ((x)*(x))
#undef min
#define min(x, y) ((x) > (y) ? (y) : (x))
#undef max
#define max(x, y) ((x) > (y) ? (x) : (y))

/*
static int matrix_sum(double *sum, double *buf, int columns, int rows)
{
	int i, j, n;
	
	*sum = 0.0, n = 0;
	for (i = 0; i < rows; i++) {
		for (j = 0; j < columns; j++) {
			*sum += *(buf+j);
			n++;
		}
		buf += columns;
	}
	return n;
}

static int matrix_sum_on_roi(double *sum, double *buf, int columns, int rows, bitmap_t *roi)
{
	int i, j, n;
	unsigned char *roibuf;
	
	*sum = 0.0, n = 0;
	roibuf = roi->buffer;
	for (i = 0; i < rows; i++) {
		for (j = 0; j < columns; j++) {
			if ((*(roibuf+(j>>3)))&(1<<(j%8))) { // 1
				*sum += *(buf+j);
				n++;
			} else { // 0
			}
		}
		roibuf += roi->header.pitch;
		buf += columns;
	}
	return n;
}

static int matrix_sum_on_region(double *sum, double *buf, int columns, int rows, int c, int r, int dc, int dr)
{
	int i, j, n, pitch;

	*sum = 0.0, n = 0;
	pitch = columns;
	buf += r*pitch;
	columns = min(columns, c+dc);
	rows = min(rows, r+dr);
	for (i = r; i < rows; i++) {
		for (j = c; j < columns; j++) {
			*sum += *(buf+j);
			n++;
		}
		buf += pitch;
	}
	return n;
}

int matrix_get_sum(double *real, double *imag, matrix_t *mat)
{
	int n;
	
	assert(real || imag);
	assert(mat);
	
	n = 0;
	if (real) {
		if (!mat->real) {
			*real = 0.0;
			n = 0;
		} else {
			n = matrix_sum(real, mat->real, mat->columns, mat->rows);
		}
	}
	if (imag) {
		if (!mat->imaginary) {
			*imag = 0.0;
			n = 0;
		} else {
			n = matrix_sum(imag, mat->imaginary, mat->columns, mat->rows);
		}
	}
	return n;

}

int matrix_get_sum_on_roi(double *real, double *imag, matrix_t *mat, bitmap_t *roi)
{
	int n;
	
	assert(real || imag);
	assert(mat);
	assert(roi);
	assert(mat->columns == roi->header.width);
	assert(mat->rows == roi->header.height);
	
	if (real) {
		if (mat->real) {
			n = matrix_sum_on_roi(real, mat->real, mat->columns, mat->rows, roi);
		} else {
			*real = 0.0;
			n = 0;
		}
	}
	if (imag) {
		if (mat->imaginary) {
			n = matrix_sum_on_roi(imag, mat->imaginary, mat->columns, mat->rows, roi);
		} else {
			*imag = 0.0;
			n = 0;
		}
	}
	return n;
}

int matrix_get_sum_on_region(double *real, double *imag, matrix_t *mat, int c, int r, int dc, int dr)
{
	int n;
	
	assert(real || imag);
	assert(mat);
	assert(roi);
	assert(c >= 0 && c < mat->columns);
	assert(r >= 0 && r < mat->rows);
	assert(dc > 0);
	assert(dr > 0);
	
	if (real) {
		if (mat->real) {
			n = matrix_sum_on_region(real, mat->real, mat->columns, mat->rows, c, r, dc, dr);
		} else {
			*real = 0.0;
			n = 0;
		}
	}
	if (imag) {
		if (mat->imaginary) {
			n = matrix_sum_on_region(imag, mat->imaginary, mat->columns, mat->rows, c, r, dc, dr);
		} else {
			*imag = 0.0;
			n = 0;
		}
	}
	return n;
}

int matrix_get_mean(double *real, double *imag, matrix_t *mat)
{
	int n;
	
	assert(real || imag);
	assert(mat);
	
	if (real) {
		if (mat->real) {
			n = matrix_sum(real, mat->real, mat->columns, mat->rows);
			if (n != 0) *real /= (double)n;
		} else {
			*real = 0.0;
			n = 0;
		}
	}
	if (imag) {
		if (mat->imaginary) {
			n = matrix_sum(imag, mat->imaginary, mat->columns, mat->rows);
			if (n != 0) *imag /= (double)n;
		} else {
			*imag = 0.0;
			n = 0;
		}
	}
	return n;
}

int matrix_get_mean_on_roi(double *real, double *imag, matrix_t *mat, bitmap_t *roi)
{
	int n;
	
	assert(real || imag);
	assert(mat);
	assert(roi);
	assert(mat->columns == roi->header.width);
	assert(mat->rows == roi->header.height);
	
	if (real) {
		if (mat->real) {
			n = matrix_sum_on_roi(real, mat->real, mat->columns, mat->rows, roi);
			if (n != 0) *real /= (double)n;
		} else {
			*real = 0.0;
			n = 0;
		}
	}
	if (imag) {
		if (mat->imaginary) {
			n = matrix_sum_on_roi(imag, mat->imaginary, mat->columns, mat->rows, roi);
			if (n != 0) *imag /= (double)n;
		} else {
			*imag = 0.0;
			n = 0;
		}
	}
	return n;
}

int matrix_get_mean_on_region(double *real, double *imag, matrix_t *mat, int c, int r, int dc, int dr)
{
	int n;
	
	assert(real || imag);
	assert(mat);
	assert(roi);
	assert(c >= 0 && c < mat->columns);
	assert(r >= 0 && r < mat->rows);
	assert(dc > 0);
	assert(dr > 0);
	
	if (real) {
		if (mat->real) {
			n = matrix_sum_on_region(real, mat->real, mat->columns, mat->rows, c, r, dc, dr);
			if (n != 0) *real /= (double)n;
		} else {
			*real = 0.0;
			n = 0;
		}
	}
	if (imag) {
		if (mat->imaginary) {
			n = matrix_sum_on_region(imag, mat->imaginary, mat->columns, mat->rows, c, r, dc, dr);
			if (n != 0) *imag /= (double)n;
		} else {
			*imag = 0.0;
			n = 0;
		}
	}
	return n;
}

*/
static void get_real_mean(double *mean, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	int i, j;
	int count = 0;
	double *real;
	unsigned char *buf;

	*mean = 0.0;
	if (!m) {
		real = p->real+r*p->columns;
		for (i = r; i < (r+rows); i++) {
			for (j = c; j < (c+cols); j++) {
				*mean += *(real+j);
				count++;
			}
			real += p->columns;
		}
	} else {
		real = p->real+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				if (*(buf+(j>>3))&(1<<(j%8))) {
					*mean += *(real+j);
					count++;
				}
			}
			real += p->columns;
			buf += m->header.pitch;
		}
	}
	*mean /= (double)count;
}

static void get_imag_mean(double *mean, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	int i, j;
	int count = 0;
	double *imag;
	unsigned char *buf;

	*mean = 0.0;
	if (!m) {
		imag = p->imaginary+r*p->columns;
		for (i = r; i < (r+rows); i++) {
			for (j = c; j < (c+cols); j++) {
				*mean += *(imag+j);
				count++;
			}
			imag += p->columns;
		}
	} else {
		imag = p->imaginary+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				if (*(buf+(j>>3))&(1<<(j%8))) {
					*mean += *(imag+j);
					count++;
				}
			}
			imag += p->columns;
			buf += m->header.pitch;
		}
	}	
	*mean /= (double)count;
}

static void get_complex_mean(double *rmean, double *imean, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	int i, j;
	int count = 0;
	double *real, *imag;
	unsigned char *buf;

	*rmean = *imean = 0.0;
	if (!m) {
		real = p->real+r*p->columns;
		imag = p->imaginary+r*p->columns;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				*rmean += *(real+j);
				*imean += *(imag+j);
				count++;
			}
			real += p->columns;
			imag += p->columns;
		}
	} else {
		real = p->real+r*p->columns;
		imag = p->imaginary+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				if (*(buf+(j>>3))&(1<<(j%8))) {
					*rmean += *(real+j);
					*imean += *(imag+j);
					count++;
				}
			}
			real += p->columns;
			imag += p->columns;
			buf += m->header.pitch;
		}
	}
	*rmean /= (double)count;
	*imean /= (double)count;
}

int matrix_get_mean(double *real, double *imag, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	assert(p);
	assert(real);
	assert(m == NULL || p->columns == m->header.width);
	assert(m == NULL || p->rows == m->header.height);
	assert(c >= 0 && c < p->columns);
	assert(r >= 0 && r < p->rows);
	
	cols = min(c+cols, p->columns)-c;
	rows = min(r+rows, p->rows)-r;
	
	if (!p->imaginary) {
		get_real_mean(real, p, m, c, r, cols, rows);
		if (imag) *imag = 0.0;
	} else {
		if (!imag) return -1;
		get_complex_mean(real, imag, p, m, c, r, cols, rows);
	}
	return 0;
}

int matrix_get_rmean(double *mean, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	assert(p);
	assert(mean);
	assert(m == NULL || p->columns == m->header.width);
	assert(m == NULL || p->rows == m->header.height);
	assert(c >= 0 && c < p->columns);
	assert(r >= 0 && r < p->rows);
	
	cols = min(c+cols, p->columns)-c;
	rows = min(r+rows, p->rows)-r;
	
	get_real_mean(mean, p, m, c, r, cols, rows);
	return 0;
}

int matrix_get_imean(double *mean, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	assert(p);
	assert(p->imaginary);
	assert(mean);
	assert(m == NULL || p->columns == m->header.width);
	assert(m == NULL || p->rows == m->header.height);
	assert(c >= 0 && c < p->columns);
	assert(r >= 0 && r < p->rows);
	
	cols = min(c+cols, p->columns)-c;
	rows = min(r+rows, p->rows)-r;
	
	get_imag_mean(mean, p, m, c, r, cols, rows);
	return 0;
}

static void get_real_variance(double *var, matrix_t *p, double mean, bitmap_t *m, int c, int r, int cols, int rows)
{
	int i, j;
	int count = 0;
	double val;
	double *real;
	unsigned char *buf;
	
	*var = 0.0;
	if (!m) {
		real = p->real+r*p->columns;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				val = *(real+j)-mean;
				*var += val*val;
				count++;
			}
			real += p->columns;
		}
	} else {
		real = p->real+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				if (*(buf+(j>>3))&(1<<(j%8))) {
					val = *(real+j)-mean;
					*var += val*val;
					count++;
				}
			}
			real += p->columns;
			buf += m->header.pitch;
		}
	}
	*var /= (double)count;
}

static void get_imag_variance(double *var, matrix_t *p, double mean, bitmap_t *m, int c, int r, int cols, int rows)
{
	int i, j;
	int count = 0;
	double val;
	double *imag;
	unsigned char *buf;
	
	*var = 0.0;
	if (!m) {
		imag = p->imaginary+r*p->columns;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				val = *(imag+j)-mean;
				*var += val*val;
				count++;
			}
			imag += p->columns;
		}
	} else {
		imag = p->imaginary+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				if (*(buf+(j>>3))&(1<<(j%8))) {
					val = *(imag+j)-mean;
					*var += val*val;
					count++;
				}
			}
			imag += p->columns;
			buf += m->header.pitch;
		}
	}
	*var /= (double)count;
}

static void get_complex_variance(double *var, matrix_t *p, double rmean, double imean, bitmap_t *m, int c, int r, int cols, int rows)
{
	int i, j;
	int count = 0;
	double rval, ival;
	double *real, *imag;
	unsigned char *buf;

	*var = 0.0;
	if (!m) {
		real = p->real+r*p->columns;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				rval = *(real+j)-rmean;
				ival = *(imag+j)-imean;
				*var += sqr(rval)+sqr(ival);
			}
			real += p->columns;
			imag += p->columns;
		}
	} else {
		real = p->real+r*p->columns;
		imag = p->imaginary+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				if (*(buf+(j>>3))&(1<<(j%8))) {
					rval = *(real+j)-rmean;
					ival = *(imag+j)-imean;
					*var += sqr(rval)+sqr(ival);
				}
			}
			real += p->columns;
			imag += p->columns;
			buf += m->header.pitch;
		}
	}
	*var /= (double)count;
}

int matrix_get_variance(double *var, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	double rmean, imean;
	
	assert(p);
	assert(var);
	assert(m == NULL || p->columns == m->header.width);
	assert(m == NULL || p->rows == m->header.height);
	assert(c >= 0 && c < p->columns);
	assert(r >= 0 && r < p->rows);
	
	cols = min(c+cols, p->columns)-c;
	rows = min(r+rows, p->rows)-r;
	
	if (!p->imaginary) {
		get_real_mean(&rmean, p, m, c, r, cols, rows);
		get_real_variance(var, p, rmean, m, c, r, cols, rows);
	} else {
		get_complex_mean(&rmean, &imean, p, m, c, r, cols, rows);
		get_complex_variance(var, p, rmean, imean, m, c, r, cols, rows);
	}
	return 0;
}

int matrix_get_rvariance(double *var, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	double rmean;
	
	assert(p);
	assert(var);
	assert(m == NULL || p->columns == m->header.width);
	assert(m == NULL || p->rows == m->header.height);
	assert(c >= 0 && c < p->columns);
	assert(r >= 0 && r < p->rows);
	
	cols = min(c+cols, p->columns)-c;
	rows = min(r+rows, p->rows)-r;
	
	get_real_mean(&rmean, p, m, c, r, cols, rows);
	get_real_variance(var, p, rmean, m, c, r, cols, rows);
	return 0;
}

int matrix_get_ivariance(double *var, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	double imean;
	
	assert(p);
	assert(var);
	assert(m == NULL || p->columns == m->header.width);
	assert(m == NULL || p->rows == m->header.height);
	assert(c >= 0 && c < p->columns);
	assert(r >= 0 && r < p->rows);
	
	cols = min(c+cols, p->columns)-c;
	rows = min(r+rows, p->rows)-r;
	
	get_imag_mean(&imean, p, m, c, r, cols, rows);
	get_imag_variance(var, p, imean, m, c, r, cols, rows);
	return 0;
}

static void get_real_moments(double *m4, double *m3, double *m2, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	int i, j;
	int count = 0;
	double mean, val, val2, val3, val4;
	double *real;
	unsigned char *buf;

	mean = 0.0;
	if (!m) {
		real = p->real+r*p->columns;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < (c+cols); j++) {
				mean += *(real+j);
				count++;
			}
			real += p->columns;
		}
		mean /= (double)count;
		real = p->real+r*p->columns;
		val2 = val3 = val4 = 0.0;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				val = *(real+j)-mean;
				val2 = val*val;
				val3 = val2*val;
				val4 = val3*val;
			}
			real += p->columns;
		}
		if (m2) *m2 = val2/(double)count;
		if (m3) *m3 = val3/(double)count;
		if (m4) *m4 = val4/(double)count;
	} else {
		real = p->real+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				if (*(buf+(j>>3))&(1<<(j%8))) {
					mean += *(real+j);
					count++;
				}
			}
			real += p->columns;
			buf += m->header.pitch;
		}
		mean /= (double)count;
		real = p->real+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		val2 = val3 = val4 = 0.0;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				val = *(real+j)-mean;
				val2 = val*val;
				val3 = val2*val;
				val4 = val3*val;
			}
			real += p->columns;
			buf += m->header.pitch;
		}
		if (m2) *m2 = val2/(double)count;
		if (m3) *m3 = val3/(double)count;
		if (m4) *m4 = val4/(double)count;
	}
	mean /= (double)count;
}

static void get_imag_moments(double *m4, double *m3, double *m2, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	int i, j;
	int count = 0;
	double mean, val, val2, val3, val4;
	double *imag;
	unsigned char *buf;

	mean = 0.0;
	if (!m) {
		imag = p->imaginary+r*p->columns;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < (c+cols); j++) {
				mean += *(imag+j);
				count++;
			}
			imag += p->columns;
		}
		mean /= (double)count;
		imag = p->imaginary+r*p->columns;
		val2 = val3 = val4 = 0.0;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				val = *(imag+j)-mean;
				val2 = val*val;
				val3 = val2*val;
				val4 = val3*val;
			}
			imag += p->columns;
		}
		if (m2) *m2 = val2/(double)count;
		if (m3) *m3 = val3/(double)count;
		if (m4) *m4 = val4/(double)count;
	} else {
		imag = p->imaginary+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				if (*(buf+(j>>3))&(1<<(j%8))) {
					mean += *(imag+j);
					count++;
				}
			}
			imag += p->columns;
			buf += m->header.pitch;
		}
		mean /= (double)count;
		imag = p->imaginary+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		val2 = val3 = val4 = 0.0;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				val = *(imag+j)-mean;
				val2 = val*val;
				val3 = val2*val;
				val4 = val3*val;
			}
			imag += p->columns;
			buf += m->header.pitch;
		}
		if (m2) *m2 = val2/(double)count;
		if (m3) *m3 = val3/(double)count;
		if (m4) *m4 = val4/(double)count;
	}
}

static void get_complex_moments(double *m4, double *m3, double *m2, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	int i, j;
	int count = 0;
	double rmean, imean, rval, ival, val, val2, val3, val4;
	double *real;
	double *imag;
	unsigned char *buf;

	rmean = imean = 0.0;
	if (!m) {
		real = p->real+r*p->columns;
		imag = p->imaginary+r*p->columns;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < (c+cols); j++) {
				rmean += *(real+j);
				imean += *(imag+j);
				count++;
			}
			real += p->columns;
			imag += p->columns;
		}
		rmean /= (double)count;
		imean /= (double)count;
		real = p->real+r*p->columns;
		imag = p->imaginary+r*p->columns;
		val2 = val3 = val4 = 0.0;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				rval = *(real+j);
				ival = *(imag+j);
				val = sqrt(sqr(rval-rmean)+sqr(ival-imean));
				if (abs(rval)+abs(ival) < abs(rmean)+abs(imean)) val = -val;
				val2 = val*val;
				val3 = val2*val;
				val4 = val3*val;
			}
			real += p->columns;
			imag += p->columns;
		}
		if (m2) *m2 = val2/(double)count;
		if (m3) *m3 = val3/(double)count;
		if (m4) *m4 = val4/(double)count;
	} else {
		real = p->real+r*p->columns;
		imag = p->imaginary+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				if (*(buf+(j>>3))&(1<<(j%8))) {
					rmean += *(real+j);
					imean += *(imag+j);
					count++;
				}
			}
			real += p->columns;
			imag += p->columns;
			buf += m->header.pitch;
		}
		rmean /= (double)count;
		imean /= (double)count;
		real = p->real+r*p->columns;
		imag = p->imaginary+r*p->columns;
		buf = m->buffer+r*m->header.pitch;
		val2 = val3 = val4 = 0.0;
		for (i = r; i < r+rows; i++) {
			for (j = c; j < c+cols; j++) {
				rval = *(real+j);
				ival = *(imag+j);
				val = sqrt(sqr(rval-rmean)+sqr(ival-imean));
				if (abs(rval)+abs(ival) < abs(rmean)+abs(imean)) val = -val;
				val2 = val*val;
				val3 = val2*val;
				val4 = val3*val;
			}
			real += p->columns;
			imag += p->columns;
			buf += m->header.pitch;
		}
		if (m2) *m2 = val2/(double)count;
		if (m3) *m3 = val3/(double)count;
		if (m4) *m4 = val4/(double)count;
	}
}

int matrix_get_std(double *std, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	double rmean, imean;
	
	assert(p);
	assert(std);
	assert(m == NULL || p->columns == m->header.width);
	assert(m == NULL || p->rows == m->header.height);
	assert(c >= 0 && c < p->columns);
	assert(r >= 0 && r < p->rows);
	
	cols = min(c+cols, p->columns)-c;
	rows = min(r+rows, p->rows)-r;
	
	if (!p->imaginary) {
		get_real_mean(&rmean, p, m, c, r, cols, rows);
		get_real_variance(std, p, rmean, m, c, r, cols, rows);
		*std = sqrt(*std);
	} else {
		get_complex_mean(&rmean, &imean, p, m, c, r, cols, rows);
		get_complex_variance(std, p, rmean, imean, m, c, r, cols, rows);
		*std = sqrt(*std);
	}
	return 0;
}

int matrix_get_rstd(double *std, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	double rmean;
	
	assert(p);
	assert(std);
	assert(m == NULL || p->columns == m->header.width);
	assert(m == NULL || p->rows == m->header.height);
	assert(c >= 0 && c < p->columns);
	assert(r >= 0 && r < p->rows);
	
	cols = min(c+cols, p->columns)-c;
	rows = min(r+rows, p->rows)-r;

	get_real_mean(&rmean, p, m, c, r, cols, rows);
	get_real_variance(std, p, rmean, m, c, r, cols, rows);
	*std = sqrt(*std);
	return 0;
}

int matrix_get_istd(double *std, matrix_t *p, bitmap_t *m, int c, int r, int cols, int rows)
{
	double imean;
	
	assert(p);
	assert(std);
	assert(m == NULL || p->columns == m->header.width);
	assert(m == NULL || p->rows == m->header.height);
	assert(c >= 0 && c < p->columns);
	assert(r >= 0 && r < p->rows);
	
	cols = min(c+cols, p->columns)-c;
	rows = min(r+rows, p->rows)-r;
	
	get_imag_mean(&imean, p, m, c, r, cols, rows);
	get_imag_variance(std, p, imean, m, c, r, cols, rows);
	*std = sqrt(*std);
	return 0;
}
