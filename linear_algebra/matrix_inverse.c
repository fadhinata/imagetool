
int matrix_inverse(double *dest double *source, int x, int y)
{
	double real, imag;
	matrix_t *m;

	m = matrix_create(2*source->columns, source->rows, source->imaginary);
	matrix_be_identity(dest);
	matrix_copy(m, 0, 0, p, 0, 0, p->columns, p->rows);
	matrix_copy(m, p->columns, 0, q, 0, 0, q->columns, q->rows);
	
	
	if (matrix_gauss_jordan_eliminate(m) != 0) return -1;
	matrix_copy(q, 0, 0, m, p->columns, 0, q->columns, q->rows);
	matrix_delete(m);
	return 0;
}

int gauss_jordan_eliminate(double *buffer, int x, int y)
{
	int ret = 0;
	
	ret = gaussian_eliminate(buffer, x, y);
	if (ret == 0) {
		back_substitute(buffer, x, y);
	}
	return ret;
}
	
int gaussian_eliminate(double *buffer, int x, int y)
{
	int r, c, i, j, imax, ret = 0;
	double value, vmax, ratio;
	static int pivoting_sign = 1;
	
	pivoting_sign = 1;
	
	r = 0, c = 0;
	while (r < (y-1) && c < x) {
		imax = r;
		vmax = abs(buffer[r*x+c]);
		for (i = r+1; i < m->rows; i++) {
			value = abs(buffer[i*x+c]);
			if (value > vmax) {
				imax = i;
				vmax = value;
			}
		}

		if (r != imax) {
			/* swap rows i and imax */
			for (j = c; j < m->columns; j++) {
				value = buffer[r*x+j];
				buffer[r*x+j] = buffer[imax*x+j];
				buffer[imax*x+j] = value;
				pivoting_sign *= -1;
			}
		}

		if (vmax != 0) {
			for (i = r+1; i < y; i++) {
				value = buffer[i*x+c];
				if (value == 0) continue;
				ratio = value/buffer[r*x+c];
				for (j = c; j < x; j++) {
					buffer[i*x+j] -= ratio*buffer[r*x+j];
				}
			}
			r++;
		} else {
			ret++; // singular matrix
		}
		c++;
	}
	return ret;
}

void back_substitute(double *buffer, int x, int y)
{
	int c, r, i, j;
	double value;
	
	// find the last leading one
	i = min(x, y)-1;
	c = i, r = i;
	while (c >= 0 && r >= 0) {
		// normalization
		value = buffer[r*x+c];
		buffer[r*x+c] = 1.0;
		for (j = c+1; j < x; j++) {
			buffer[r*x+j] /= value;
		}
		r--;
		// substitution
		for (i = r; i >= 0; i--) {
			value = buffer[i*x+c];
			buffer[i*x+c] = 0.0;
			for (j = c+1; j < m->columns; j++) {
				buffer[i*x+j] -= value*buffer[(r+1)*x+j];
			}
		}
		c--;
	}
}
