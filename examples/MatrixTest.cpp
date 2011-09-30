
void MatrixTest(void)
{
	int i, j;
	
	matrix_t *a, *b, *c, *d;
	vector_t *v;
	
	a = matrix_create(4, 4, true);
	b = matrix_create(4, 4, true);
	c = matrix_create(4, 4, true);
	d = matrix_create(3, 3, true);
	v = vector_create(4, true);
	
	srand((unsigned)time(NULL));

	printf("Test - matrix clear\n");
	cmatrix_fill_randomly(a);
	matrix_dump(a);
	printf("rmatrix_bezero\n");
	matrix_be_zero(a, 0, 0, 4, 4);
	matrix_dump(a);
	printf("imatrix_bezero\n");
	imatrix_be_zero(a, 0, 0, 4, 4);
	matrix_dump(a);
	cmatrix_fill_randomly(a);
	matrix_dump(a);
	printf("matrix_bezero\n");
	matrix_be_zero(a, 0, 0, 4, 4);
	matrix_dump(a);
	
	printf("\nTest - matrix set\n");
	printf("rmatrix_fill\n");
	matrix_fill(a, 0, 0, 2, 2, 123.456);
	matrix_dump(a);
	printf("imatrix_fill\n");
	imatrix_fill(a, 0, 0, 2, 2, 123.456);
	matrix_dump(a);
	printf("matrix_fill\n");
	matrix_fill(a, 2, 2, 2, 2, 123.456);
	imatrix_fill(a, 2, 2, 2, 2, 123.456);
	matrix_dump(a);
	
	printf("Test - trimming matrix\n");
	cmatrix_fill_randomly(a);
	matrix_dump(a);
	printf("rmatrix_trim\n");
	matrix_trim(a, 10.0, 20.0);
	matrix_dump(a);
	printf("imatrix_trim\n");
	imatrix_trim(a, 10.0, 20.0);
	matrix_dump(a);

	printf("Test - identifing matrix\n");
	cmatrix_fill_randomly(a);
	matrix_dump(a);
	printf("matrix_be_identity\n");
	matrix_be_identity(a);
	matrix_dump(a);

	printf("\nTest - compare between matrix and matrix\n");
	cmatrix_fill_randomly(a);
	cmatrix_fill_randomly(b);
	printf("rmatrix_compare_rmatrix\n");
	matrix_dump(a);
	matrix_dump(b);
	printf(" -------> %d\n", matrix_compare_matrix(a, b));
	matrix_copy_matrix(a, 0, 0, b, 0, 0, 4, 4);
	matrix_dump(a);
	matrix_dump(b);
	printf(" -------> %d\n", matrix_compare_matrix(a, b));
	printf("rmatrix_compare_imatrix\n");
	matrix_dump(a);
	matrix_dump(b);
	printf(" -------> %d\n", matrix_compare_imatrix(a, b));
	imatrix_copy_matrix(b, 0, 0, a, 0, 0, 4, 4);
	matrix_dump(a);
	matrix_dump(b);
	printf(" -------> %d\n", matrix_compare_imatrix(a, b));
	printf("imatrix_compare_rmatrix\n");
	matrix_dump(a);
	matrix_dump(b);
	printf(" -------> %d\n", imatrix_compare_matrix(a, b));
	imatrix_copy_matrix(a, 0, 0, b, 0, 0, 4, 4);
	matrix_dump(a);
	matrix_dump(b);
	printf(" -------> %d\n", imatrix_compare_matrix(a, b));
	printf("imatrix_compare_imatrix\n");
	imatrix_be_zero(b, 0, 0, 4, 4);
	matrix_dump(a);
	matrix_dump(b);
	printf(" -------> %d\n", imatrix_compare_imatrix(a, b));
	imatrix_copy_imatrix(b, 0, 0, a, 0, 0, 4, 4);
	matrix_dump(a);
	matrix_dump(b);
	printf(" -------> %d\n", imatrix_compare_imatrix(a, b));
	printf("matrix_compare_matrix\n");
	matrix_dump(a);
	matrix_dump(b);
	printf(" -------> %d\n", cmatrix_compare_cmatrix(a, b));
	
	printf("\nTest - transpose matrix\n");
	printf("rmatrix_copy_rmatrix_transpose\n");
	cmatrix_fill_randomly(a);
	cmatrix_fill_randomly(b);
	matrix_dump(b);
	matrix_copy_matrix_transpose(a, b);
	matrix_dump(a);
	printf("imatrix_copy_imatrix_transpose\n");
	matrix_dump(b);
	imatrix_copy_imatrix_transpose(a, b);
	matrix_dump(a);
	printf("matrix_copy_matrix_transpose\n");
	matrix_dump(a);
	cmatrix_copy_cmatrix_transpose(b, a);
	matrix_dump(b);
	
	printf("\nTest - matrix copy row vector\n");
	printf("rmatrix_copy_row_rvector\n");
	cmatrix_fill_randomly(a);
	cvector_fill_randomly(v);
	matrix_dump(a);
	vector_dump(v);
	matrix_copy_row_vector(a, 0, 0, v);
	matrix_dump(a);
	
	printf("\nTest - matrix copy column vector\n");
	printf("rmatrix_copy_column_rvector\n");
	matrix_dump(a);
	vector_dump(v);
	matrix_copy_column_vector(a, 0, 0, v);
	matrix_dump(a);

	printf("\nTest - matrix minor\n");
	printf("rmatrix_copy_rmatrix_minor\n");
	cmatrix_fill_randomly(a);
	matrix_dump(a);
	matrix_copy_matrix_minor(d, a, 2, 2);
	matrix_dump(d);
	
	printf("\nTest - addition between matrix and scalar\n");
	printf("rmatrix_copy_rmatrix_add_scalar\n");
	cmatrix_fill_randomly(a);
	matrix_dump(a);
	matrix_copy_matrix_add_scalar(b, a, 100.0);
	matrix_dump(b);
	
	printf("\nTest - addition between matrix and matrix\n");
	printf("rmatrix_copy_rmatrix_add_rmatrix\n");
	matrix_be_zero(a, 0, 0, 4, 4);
	matrix_add_scalar(a, 1.0);
	matrix_be_zero(b, 0, 0, 4, 4);
	matrix_add_scalar(b, 1.0);
	matrix_dump(a);
	matrix_dump(b);
	matrix_copy_matrix_add_matrix(c, a, b);
	matrix_dump(c);
	printf("imatrix_copy_imatrix_add_imatrix\n");
	imatrix_be_zero(a, 0, 0, 4, 4);
	imatrix_add_scalar(a, 1.0);
	imatrix_be_zero(b, 0, 0, 4, 4);
	imatrix_add_scalar(b, 1.0);
	matrix_dump(a);
	matrix_dump(b);
	imatrix_copy_imatrix_add_imatrix(c, a, b);
	matrix_dump(c);
	printf("matrix_copy_matrix_add_matrix\n");
	matrix_dump(a);
	matrix_dump(b);
	cmatrix_copy_cmatrix_add_cmatrix(c, a, b);
	matrix_dump(c);
	
	printf("\nTest - subtraction between matrix and matrix\n");
	printf("rmatrix_copy_rmatrix_subtract_rmatrix\n");
	matrix_dump(a);
	matrix_dump(b);
	matrix_copy_matrix_subtract_matrix(c, a, b);
	matrix_dump(c);
	printf("imatrix_copy_imatrix_subtract_imatrix\n");
	matrix_dump(a);
	matrix_dump(b);
	imatrix_copy_imatrix_subtract_imatrix(c, a, b);
	matrix_dump(c);
	printf("matrix_copy_matrix_subtract_matrix\n");
	matrix_dump(a);
	matrix_dump(b);
	cmatrix_copy_cmatrix_subtract_cmatrix(c, a, b);
	matrix_dump(c);
	
	printf("\nTest - multiplication between matrix and matrix\n");
	printf("rmatrix_copy_rmatrix_multiply_rmatrix\n");
	cmatrix_fill_randomly(a);
	matrix_be_identity(b);
	matrix_dump(a);
	matrix_dump(b);
	matrix_copy_matrix_multiply_matrix(c, a, b);
	matrix_dump(c);
	printf("imatrix_copy_imatrix_multiply_rmatrix\n");
	matrix_dump(a);
	matrix_dump(b);
	imatrix_copy_imatrix_multiply_matrix(c, a, b);
	matrix_dump(c);
	printf("matrix_copy_matrix_multiply_matrix\n");
	matrix_dump(a);
	matrix_dump(b);
	cmatrix_copy_cmatrix_multiply_cmatrix(c, a, b);
	matrix_dump(c);
	
	printf("\nTest - piecewise multiplication between matrix and matrix\n");
	printf("rmatrix_copy_rmatrix_piecewise_multiply_rmatrix\n");
	cmatrix_fill_randomly(a);
	matrix_be_identity(b);
	matrix_dump(a);
	matrix_dump(b);
	matrix_copy_matrix_piecewise_multiply_matrix(c, a, b);
	matrix_dump(c);
	printf("imatrix_copy_imatrix_piecewise_multiply_rmatrix\n");
	matrix_dump(a);
	matrix_dump(b);
	imatrix_copy_imatrix_piecewise_multiply_matrix(c, a, b);
	matrix_dump(c);
	printf("matrix_copy_matrix_piecewise_multiply_matrix\n");
	matrix_dump(a);
	matrix_dump(b);
	cmatrix_copy_cmatrix_piecewise_multiply_cmatrix(c, a, b);
	matrix_dump(c);
	
	
	printf("machine epsilon at 0 is %G\n", machine_eps(0.0));
	printf("machine epsilon at 1 is %G\n", machine_eps(1.0));
	printf("machine epsilon at 2 is %G\n", machine_eps(2.0));
	printf("machine epsilon at 3 is %G\n", machine_eps(3.0));
	printf("machine epsilon at 4 is %G\n", machine_eps(4.0));
	
	matrix_delete(a);
	matrix_delete(b);
	matrix_delete(c);
}

void test_gram_schmidt_process(void)
{
	int i, n;
	vector_t *q[3], *p[3];
	for (i = 0; i < 3; i++) {
		p[i] = vector_create(3, 0);
		q[i] = vector_create(3, 0);
	}
	VECTOR_PUT(1.0, p[0], 0); VECTOR_PUT(1.0, p[0], 1); VECTOR_PUT(1.0, p[0], 2);
	VECTOR_PUT(0.0, p[1], 0); VECTOR_PUT(1.0, p[1], 1); VECTOR_PUT(1.0, p[1], 2);
	VECTOR_PUT(0.0, p[2], 0); VECTOR_PUT(0.0, p[2], 1); VECTOR_PUT(1.0, p[2], 2);
	for (i = 0; i < 3; i++) {
		vector_dump(p[i]);
	}
	printf("gram-schmidt processing\n");
	n = vector_gram_schmidt_process(q, p, 3);
	for (i = 0; i < n; i++) {
		vector_dump(q[i]);
	}
	for (i = 0; i < 3; i++) {
		vector_delete(p[i]);
		vector_delete(q[i]);
	}
}

void test_qr_decomposition(void)
{
	int i, n;
	matrix_t *q, *r, *p;
	p = matrix_create(3, 3, 1);
	MATRIX_PUT(1.0, p, 0, 0); MATRIX_PUT(0.0, p, 1, 0); MATRIX_PUT(0.0, p, 2, 0);
	MATRIX_PUT(1.0, p, 0, 1); MATRIX_PUT(1.0, p, 1, 1); MATRIX_PUT(0.0, p, 2, 1);
	MATRIX_PUT(1.0, p, 0, 2); MATRIX_PUT(1.0, p, 1, 2); MATRIX_PUT(1.0, p, 2, 2);
	matrix_dump(p);
	printf("QR decomposition\n");
	cmatrix_qr_decomposition(&q, &r, p);
	matrix_dump(q);
	matrix_dump(r);
	cmatrix_copy_cmatrix_multiply_cmatrix(p, q, r);
	matrix_dump(p);
	matrix_delete(q);
	matrix_delete(r);
	matrix_delete(p);
}
