#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <linear_algebra/matrix.h>
#include <linear_algebra/vector.h>
#include <linear_algebra/gram_schmidt.h>
#include <linear_algebra/qr_decomp.h>

#define TEST_COLUMNS 7
#define TEST_ROWS 7

void test_matrix_new_and_copy(void)
{
  matrix_t *a, *b;

  printf("Testing for \"matrix_new_and_copy(matrix_t *m)\" ... ");
  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  cmatrix_fill_randomly(a);
  //matrix_dump(a);

  b = matrix_new_and_copy(a);
  //matrix_dump(b);

  if (cmatrix_cmp(a, b) == 0) printf("Success\n");
  else printf("Failed\n");

  matrix_destroy(b);
  matrix_destroy(a);
}

void test_matrix_bezero(void)
{
  matrix_t *a;

  printf("Testing for \"(i,c)matrix_bezero\" ... ");
  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  cmatrix_fill_randomly(a);
  //matrix_dump(a);

  matrix_bezero(a, 0, 0, matrix_get_columns(a), matrix_get_rows(a));
  //matrix_dump(a);
  if (matrix_is_zero(a, 0, 0, matrix_get_columns(a), matrix_get_rows(a))) {
    imatrix_bezero(a, 0, 0, matrix_get_columns(a), matrix_get_rows(a));
    //matrix_dump(a);
    if (imatrix_is_zero(a, 0, 0, matrix_get_columns(a), matrix_get_rows(a))) {
      cmatrix_fill_randomly(a);
      cmatrix_bezero(a, 0, 0, matrix_get_columns(a), matrix_get_rows(a));
      if (cmatrix_is_zero(a, 0, 0, matrix_get_columns(a), matrix_get_rows(a)))
	printf("Success\n");
      else
	printf("Failed\n");
    } else {
      printf("Failed\n");
    }
  } else {
    printf("Failed\n");
  }

  matrix_destroy(a);
}


void test_matrix_fill(void)
{
  matrix_t *a;

  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);

  printf("Test for \"matrix_fill(_randomly)(matrix_t *m)\"\n");
  printf("\"cmatrix_fill_randomly()\"\n");
  cmatrix_fill_randomly(a);
  matrix_dump(a);

  printf("\"matrix_fill(a, 0, 0, 4, 4, 123.456)\"\n");
  matrix_fill(a, 0, 0, matrix_get_columns(a), matrix_get_rows(a), 123.456);
  matrix_dump(a);

  printf("\"imatrix_fill(a, 0, 0, 4, 4, 123.456)\"\n");
  imatrix_fill(a, 0, 0, matrix_get_columns(a), matrix_get_rows(a), 123.456);
  matrix_dump(a);

  printf("\"cmatrix_fill_randomly(a)\"\n");
  cmatrix_fill_randomly(a);
  matrix_dump(a);

  matrix_destroy(a);
}

void test_matrix_trim(void)
{
  matrix_t *a;

  printf("Test for \'matrix_trim()\' \n");
  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  cmatrix_fill_randomly(a);
  matrix_dump(a);

  printf("\'matrix_trim(a, 10000.0, 20000.0)\'\n");
  matrix_trim(a, 10000.0, 20000.0);
  matrix_dump(a);

  printf("\'imatrix_trim(a, 10000.0, 20000.0)\'\n");
  imatrix_trim(a, 10000.0, 20000.0);
  matrix_dump(a);

  matrix_destroy(a);
}

void test_matrix_identify(void)
{
  matrix_t *a;

  printf("Test for \'matrix_identify()\'\n");
  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  cmatrix_fill_randomly(a);
  matrix_dump(a);

  printf("\'matrix_identify(a)\'\n");
  matrix_identify(a);
  matrix_dump(a);

  printf("\'imatrix_identify(a)\'\n");
  cmatrix_fill_randomly(a);
  imatrix_identify(a);
  matrix_dump(a);

  printf("\'cmatrix_identify(a)\'\n");
  cmatrix_fill_randomly(a);
  cmatrix_identify(a);
  matrix_dump(a);

  matrix_destroy(a);
}

void test_matrix_exchange_row(void)
{
  int i, j;
  matrix_t *a;

  printf("Test for \'matrix_exchange_row()\'\n");
  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  for (i = 0; i < matrix_get_rows(a); i++) {
    for (j = 0; j < matrix_get_columns(a); j++) {
      matrix_put_value(i, a, j, i);
      imatrix_put_value(j, a, j, i);
    }
  }

  matrix_exchange_row(0, matrix_get_rows(a) - 1, a);
  matrix_dump(a);

  imatrix_exchange_row(0, matrix_get_rows(a) - 1, a);
  matrix_dump(a);

  cmatrix_exchange_row(0, matrix_get_rows(a) - 1, a);
  matrix_dump(a);

  matrix_destroy(a);
}

void test_matrix_exchange_column(void)
{
  int i, j;
  matrix_t *a;

  printf("Test for \'matrix_exchange_row()\'\n");
  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  for (i = 0; i < matrix_get_rows(a); i++) {
    for (j = 0; j < matrix_get_columns(a); j++) {
      matrix_put_value(i, a, j, i);
      imatrix_put_value(j, a, j, i);
    }
  }

  matrix_exchange_column(0, matrix_get_columns(a) - 1, a);
  matrix_dump(a);

  imatrix_exchange_column(0, matrix_get_columns(a) - 1, a);
  matrix_dump(a);

  cmatrix_exchange_column(0, matrix_get_columns(a) - 1, a);
  matrix_dump(a);

  matrix_destroy(a);
}

void test_matrix_transpose(void)
{
  int i, j;
  matrix_t *a, *b;

  printf("Test for \'matrix_transpose()\'\n");
  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  for (i = 0; i < matrix_get_rows(a); i++) {
    for (j = 0; j < matrix_get_columns(a); j++) {
      matrix_put_value(i, a, j, i);
      imatrix_put_value(j, a, j, i);
    }
  }

  b = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  for (i = 0; i < matrix_get_rows(b); i++) {
    for (j = 0; j < matrix_get_columns(b); j++) {
      matrix_put_value(i, b, j, i);
      imatrix_put_value(j, b, j, i);
    }
  }

  printf("\'matrix_copy_rmatrix_transpose\'\n");
  matrix_copy_matrix_transpose(a, b);
  //matrix_dump(b);
  matrix_dump(a);

  printf("\'imatrix_copy_imatrix_transpose\'\n");
  imatrix_copy_imatrix_transpose(a, b);
  //matrix_dump(b);
  matrix_dump(a);

  printf("\'cmatrix_copy_matrix_transpose\'\n");
  cmatrix_copy_cmatrix_transpose(a, b);
  matrix_dump(a);
  //matrix_dump(b);

  printf("\'cmatrix_transpose\'\n");
  cmatrix_transpose(a);
  matrix_dump(a);

  matrix_destroy(b);
  matrix_destroy(a);
}

void test_matrix_copy(void)
{
  int i, j;
  matrix_t *a, *b;

  printf("Test for \'matrix_copy\'\n");
  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  /*
  for (i = 0; i < matrix_get_rows(a); i++) {
    for (j = 0; j < matrix_get_columns(a); j++) {
      matrix_put_value(i, a, j, i);
      imatrix_put_value(j, a, j, i);
    }
  }
  */

  b = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  for (i = 0; i < matrix_get_rows(b); i++) {
    for (j = 0; j < matrix_get_columns(b); j++) {
      matrix_put_value(i, b, j, i);
      imatrix_put_value(j, b, j, i);
    }
  }

  printf("\'matrix_copy()\'\n");
  matrix_copy(a, b);
  matrix_dump(a);

  printf("\'imatrix_copy()\'\n");
  imatrix_copy(a, b);
  matrix_dump(a);

  printf("\'cmatrix_copy()\'\n");
  cmatrix_clear(a);
  cmatrix_copy(a, b);
  matrix_dump(a);

  printf("\'imatrix_copy_matrix()\'\n");
  cmatrix_clear(a);
  imatrix_copy_matrix(a, 0, 0, b, 0, 0, matrix_get_columns(b), matrix_get_rows(b));
  matrix_dump(a);

  printf("\'matrix_copy_imatrix()\'\n");
  cmatrix_clear(a);
  matrix_copy_imatrix(a, 0, 0, b, 0, 0, matrix_get_columns(b), matrix_get_rows(b));
  matrix_dump(a);

  matrix_destroy(b);
  matrix_destroy(a);
}

void test_matrix_copy_column_vector(void)
{
  int i;
  matrix_t *a;
  vector_t *v;

  printf("Test for \'matrix_copy_column_vector()\'\n");
  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  v = vector_new(TEST_ROWS, true);
  for (i = 0; i < vector_get_dimension(v); i++) {
    vector_put_value(i, v, i);
    ivector_put_value(i*10, v, i);
  }
  vector_dump(v);

  printf("\'matrix_copy_column_vector\'\n");
  matrix_clear(a);
  for (i = 0; i < matrix_get_columns(a); i++)
    matrix_copy_column_vector(a, i, 0, v);
  matrix_dump(a);

  printf("\'imatrix_copy_column_vector\'\n");
  matrix_clear(a);
  for (i = 0; i < matrix_get_columns(a); i++)
    imatrix_copy_column_vector(a, i, 0, v);
  matrix_dump(a);

  vector_destroy(v);
  matrix_destroy(a);
}

void test_matrix_copy_row_vector(void)
{
  int i;
  matrix_t *a;
  vector_t *v;

  printf("Test for \'matrix_copy_row_vector()\'\n");
  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  v = vector_new(TEST_COLUMNS, true);
  for (i = 0; i < vector_get_dimension(v); i++) {
    vector_put_value(i, v, i);
    ivector_put_value(i*10, v, i);
  }
  vector_dump(v);

  printf("\'matrix_copy_row_vector\'\n");
  matrix_clear(a);
  for (i = 0; i < matrix_get_rows(a); i++)
    matrix_copy_row_vector(a, 0, i, v);
  matrix_dump(a);

  printf("\'imatrix_copy_row_vector\'\n");
  matrix_clear(a);
  for (i = 0; i < matrix_get_rows(a); i++)
    imatrix_copy_row_vector(a, 0, i, v);
  matrix_dump(a);

  vector_destroy(v);
  matrix_destroy(a);
}

void test_matrix_copy_matrix_minor(void)
{
  int i;
  matrix_t *a, *d;

  a = matrix_new(TEST_COLUMNS, TEST_ROWS, true);
  d = matrix_new(TEST_COLUMNS-1, TEST_ROWS-1, true);

  printf("\nTest - matrix minor\n");
  printf("rmatrix_copy_rmatrix_minor\n");
  cmatrix_fill_randomly(a);
  matrix_dump(a);
  matrix_copy_matrix_minor(d, a, 2, 2);
  matrix_dump(d);

  matrix_destroy(d);
  matrix_destroy(a);
}


void matrix_copy_matrix_add_scalar_test(void)
{
  matrix_t *a, *b;

  a = matrix_new(4, 4, true);
  b = matrix_new(4, 4, true);

  printf("\nTest - addition between matrix and scalar\n");
  printf("rmatrix_copy_rmatrix_add_scalar\n");
  cmatrix_fill_randomly(a);
  matrix_dump(a);
  matrix_copy_matrix_add_scalar(b, a, 100.0);
  matrix_dump(b);

  matrix_destroy(b);
  matrix_destroy(a);
}

void matrix_copy_matrix_add_matrix_test(void)
{
  matrix_t *a, *b, *c;

  a = matrix_new(4, 4, true);
  b = matrix_new(4, 4, true);
  c = matrix_new(4, 4, true);

  printf("\nTest - addition between matrix and matrix\n");
  printf("rmatrix_copy_rmatrix_add_rmatrix\n");
  matrix_bezero(a, 0, 0, 4, 4);
  matrix_add_scalar(a, 1.0);
  matrix_bezero(b, 0, 0, 4, 4);
  matrix_add_scalar(b, 1.0);
  matrix_dump(a);
  matrix_dump(b);
  matrix_copy_matrix_add_matrix(c, a, b);
  matrix_dump(c);

  printf("imatrix_copy_imatrix_add_imatrix\n");
  imatrix_bezero(a, 0, 0, 4, 4);
  imatrix_add_scalar(a, 1.0);
  imatrix_bezero(b, 0, 0, 4, 4);
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

  matrix_destroy(c);
  matrix_destroy(b);
  matrix_destroy(a);
}

/*
 * A = 12  -51   4
 *      6  167 -68
 *     -4   24 -41
 *
 * Q =  6/7 -69/175 -58/175
 *      3/7 158/175   6/175
 *     -1/7    6/35  -33/35
 */
void test_gram_schmidt_process(void)
{
  int i, n;
  matrix_t *a, *b;
  vector_t *q[3], *p[3];

  printf("test_gram_schmidt_process\n");

  a = matrix_new(3, 3, false);

  matrix_put_value(12, a, 0, 0); matrix_put_value(-51, a, 1, 0); matrix_put_value(4, a, 2, 0);
  matrix_put_value(6, a, 0, 1); matrix_put_value(167, a, 1, 1); matrix_put_value(-68, a, 2, 1);
  matrix_put_value(-4, a, 0, 2); matrix_put_value(24, a, 1, 2); matrix_put_value(-41, a, 2, 2);
  matrix_dump(a);

  b = matrix_new_and_gram_schmidt_process(a);
  matrix_dump(b);

  return;

  for (i = 0; i < 3; i++) {
    p[i] = vector_new(3, false);
    q[i] = vector_new(3, false);
  }

  vector_put_value(1.0, p[0], 0);
  vector_put_value(1.0, p[0], 1);
  vector_put_value(1.0, p[0], 2);
  vector_put_value(0.0, p[1], 0);
  vector_put_value(1.0, p[1], 1);
  vector_put_value(1.0, p[1], 2);
  vector_put_value(0.0, p[2], 0);
  vector_put_value(0.0, p[2], 1);
  vector_put_value(1.0, p[2], 2);

  for (i = 0; i < 3; i++) {
    vector_dump(p[i]);
  }

  printf("gram-schmidt processing\n");
  //  n = vector_gram_schmidt_process(q, p, 3);

  for (i = 0; i < n; i++) {
    vector_dump(q[i]);
  }

  for (i = 0; i < 3; i++) {
    vector_destroy(p[i]);
    vector_destroy(q[i]);
  }
}

/*
 * A = 12  -51   4
 *      6  167 -68
 *     -4   24 -41
 *
 * Q =  6/7 -69/175 -58/175
 *      3/7 158/175   6/175
 *     -1/7    6/35  -33/35
 *
 * R = 14  21 -14
 *      0 175 -70
 *      0   0  35
 */
void test_qr_decomposition(void)
{
  int i, n;
  matrix_t *q, *r, *p;

  p = matrix_new(3, 3, false);

  matrix_put_value(12, p, 0, 0); matrix_put_value(-51, p, 1, 0); matrix_put_value(4, p, 2, 0);
  matrix_put_value(6, p, 0, 1); matrix_put_value(167, p, 1, 1); matrix_put_value(-68, p, 2, 1);
  matrix_put_value(-4, p, 0, 2); matrix_put_value(24, p, 1, 2); matrix_put_value(-41, p, 2, 2);
  matrix_dump(p);

  printf("QR decomposition\n");

  matrix_new_and_qr_decomposition(&q, &r, p);

  matrix_dump(q);
  matrix_dump(r);

  matrix_destroy(q);
  matrix_destroy(r);
  matrix_destroy(p);
}

void matrix_test(void)
{
  int i, j;
  matrix_t *a, *b, *c, *d;
  vector_t *v;
	
  a = matrix_new(4, 4, true);
  b = matrix_new(4, 4, true);
  c = matrix_new(4, 4, true);
  d = matrix_new(3, 3, true);

  v = vector_new(4, true);
	
  srand((unsigned)time(NULL));

  //test_matrix_new_and_copy();
  //test_matrix_bezero();
  //test_matrix_fill();
  //test_matrix_trim();
  //test_matrix_identify();
  //test_matrix_exchange_row();
  //test_matrix_exchange_column();
  //test_matrix_transpose();
  //test_matrix_copy();
  ///////////test_matrix_copy_column_vector();
  //test_gram_schmidt_process();
  test_qr_decomposition();

  return;
	
	
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
  matrix_identify(b);
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
  matrix_identify(b);
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
	
  /*
  printf("machine epsilon at 0 is %G\n", machine_eps(0.0));
  printf("machine epsilon at 1 is %G\n", machine_eps(1.0));
  printf("machine epsilon at 2 is %G\n", machine_eps(2.0));
  printf("machine epsilon at 3 is %G\n", machine_eps(3.0));
  printf("machine epsilon at 4 is %G\n", machine_eps(4.0));
  */
	
  matrix_destroy(a);
  matrix_destroy(b);
  matrix_destroy(c);
}

