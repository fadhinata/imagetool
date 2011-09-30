#include <math.h>
#include <assert.h>
#include <complex2.h>
#include <common.h>

typedef double (*function_t)(double);
typedef complex_t (*cfunction_t)(complex_t);

/* condition : the function is continuous */
/* find a root of equation f(x) = 0 on [a, b] */
/* reference from wikipedia */
int find_root_using_bisection_method(double *x, function_t *f, double a, double b, int maxtries)
{
  double lo, hi;

  assert(x);
  assert(f);
  assert(f(a) * f(b) < 0);
  assert(maxtries > 0);

  if (!(f(a) * f(b) < 0)) return -1;

  if (f(a) <= 0) {
    lo = a;
    hi = b;
  } else {
    lo = b;
    hi = a;
  }

  do {
    *x = lo + (hi - lo) / 2.0;
    if (f(*x) < 0) lo = *x;
    else hi = *x;
  } while ((abs(f(*x)) < DBL_EPSILON ||
	    abs(*x - lo) < DBL_EPSILON ||
	    abs(*x - hi) < DBL_EPSILON) &&
	   (--maxtries));

  if (maxtries <= 0) return -1;

  return 0;
}

/* condition : the function f(x) is continuous and f(a)*f(b) <= 0 */
/* find a root of equation f(x) = 0 on [a, b] */
int find_root_using_false_position_method(double *x, function_t *f, double a, double b, int maxtries)
{
  double lo, hi;

  assert(x);
  assert(f);
  assert(f(a) * f(b) < 0);
  assert(maxtries > 0);

  if (!(f(a) * f(b) < 0)) return -1;

  if (f(a) <= 0) {
    lo = a;
    hi = b;
  } else {
    lo = b;
    hi = a;
  }

  do {
    *x = lo - (hi - lo) / (f(hi) - f(lo)) * f(lo);
    if (f(*x) < 0) lo = *x;
    else hi = *x;
  } while ((abs(f(*x)) < DBL_EPSILON || 
	    abs(*x - lo) < DBL_EPSILON ||
	    abs(*x - hi) < DBL_EPSILON) && 
	   (--maxtries));

  if (maxtries <= 0) return -1;

  return 0;
}

/* condition : the function f(x) is continuous and f(a)*f(b) <= 0 */
/* find a root of equation f(x) = 0 on [a, b] */
int find_root_using_modified_false_position_method(double *x, function_t *f, double a, double b, int maxtries)
{
  double lo, hi;

  assert(x);
  assert(f);
  assert(f(a) * f(b) < 0);
  assert(maxtries > 0);

  if (!(f(a)*f(b) < 0)) return -1;

  if (f(a) <= 0) {
    lo = a;
    hi = b;
  } else {
    lo = b;
    hi = a;
  }

  *x = lo - (hi - lo) / (f(hi) - f(lo)) * f(lo);

  do {	
    if (f(*x) < 0) {
      lo = *x;
      *x = lo - (hi - lo) / (f(hi) / 2.0 - f(lo)) * f(lo);
    } else {
      hi = *x;
      *x = lo - (hi - lo) / (f(hi) - f(lo) / 2.0) * f(lo);
    }

    if (abs(f(*x)) < DBL_EPSILON ||
	abs(*x - lo) < DBL_EPSILON ||
	abs(*x - hi) < DBL_EPSILON)
      break;
  } while (--maxtries);

  if (maxtries <= 0) return -1;

  return 0;
}

int find_root_using_newton_method(double *x, function_t *f, function_t *fdiv, double a, int maxtries)
{
  double tmp;

  assert(x);
  assert(f);
  assert(maxtries > 0);

  do {
    tmp = f(*x) / fdiv(*x);
    *x = *x - tmp;
    //*x -= f(*x)/fdiv(*x);
  } while (abs(tmp) > DBL_EPSILON && --maxtries);

  if (maxtries <= 0) return -1;

  return 0;
}

int find_croot_using_newton_method(complex_t *x, cfunction_t *f, cfunction_t *fdiv, complex_t a, int maxtries)
{
  double v;
  complex_t fx, fdivx, tmp;

  assert(x);
  assert(f);
  assert(maxtries > 0);

  do {
    fx = f(*x);
    fdivx = fdiv(*x);
    complex_copy_complex_divide_complex(&tmp, &fx, &fdivx);
    complex_subtract_complex(x, &tmp);
    //*x -= f(*x)/fdiv(*x);
    complex_abs(&v, &tmp);
  } while (v > DBL_EPSILON && --maxtries);

  if (maxtries <= 0) return -1;

  return 0;
}

int find_root_using_secant_method(double *x, function_t *f, double a, double b, int maxtries)
{
  double xprev, d;

  assert(x);
  assert(f);

  *x = b - (b - a) / (f(b) - f(a)) * f(b);
  xprev = b;

  do {
    d = ((*x) - xprev) / (f((*x)) - f(xprev)) * f((*x));
    if (abs(d) < DBL_EPSILON) return 0;
    xprev = *x;
    *x = *x-d;
  } while (--maxtries);

  return -1;
}

int find_root_using_muller_method(double *x, function_t *f, double x1, double x3, int maxtries)
{
  double x2, xprev;
  double a, b, c;
  double m1, m3;

  assert(x);
  assert(f);

  if (x1 > x3) {
    x2 = x1;
    x1 = x3;
    x3 = x2;
  }
	
  xprev = x2 = (x1 + x3) / 2.0; // x2

  do {
    m1 = (f(x2) - f(x1)) / (x2 - x1);
    m3 = (f(x3) - f(x2)) / (x3 - x2);
    a = (m3 - m1) / (x3 - x1);
    b = m1 + a * (x1 - x2);
    c = f(x1);
		
    if (b > 0) *x = x1 - 2.0 * c / (b + sqrt(b * b - 4.0 * a * c));
    else *x = x1 - 2.0 * c / (b - sqrt(b * b - 4.0 * a * c));
		
    // termination ?
    if (abs((*x) - xprev) < DBL_EPSILON) return 0;
		
    // rearrange
    if ((*x) < x1) {
      x3 = x2;
      x2 = x1;
      x1 = *x;
    } else if ((*x) < x2) {
      x3 = x2;
      x2 = *x;
    } else if ((*x) < x3) {
      x3 = *x;
    } else {
      x1 = x2;
      x2 = x3;
      x3 = *x;			
    }
    xprev = *x;
  } while (--maxtries);

  return -1;
}

int find_croot_using_muller_method(complex_t *x, cfunction_t *f, complex_t x1, complex_t x3, int maxtries)
{
  complex_t x2, fx3, fx2, fx1;
  complex_t xprev;
  complex_t num; // numerator
  complex_t denom; // denominator
  complex_t tmp;
  complex_t a, b, c;
  complex_t m1, m3;
  double q, p;
  assert(x);
  assert(f);

  //	if (x1 > x3) {
  //		x2 = x1; x1 = x3; x3 = x2;
  //	}

  if (complex_cmp(&x1, &x3) > 0) {
    x2 = x1; x1 = x3; x3 = x2;
  }

  complex_copy_complex_add_complex(&x2, &x1, &x3);
  x2.real /= 2.0, x2.imag /= 2.0;
  xprev = x2;
  //xprev = x2 = (x1+x3)/2.0; // x2

  do {
    fx1 = f(x1), fx2 = f(x2), fx3 = f(x3);
    complex_copy_complex_subtract_complex(&num, &fx2, &fx1);
    complex_copy_complex_subtract_complex(&denom, &x2, &x1);
    complex_copy_complex_divide_complex(&m1, &num, &denom);
    complex_copy_complex_subtract_complex(&num, &fx3, &fx2);
    complex_copy_complex_subtract_complex(&denom, &x3, &x2);
    complex_copy_complex_divide_complex(&m3, &num, &denom);
    complex_copy_complex_subtract_complex(&num, &m3, &m1);
    complex_copy_complex_subtract_complex(&denom, &x3, &x1);
    complex_copy_complex_divide_complex(&a, &num, &denom);
    complex_copy_complex_subtract_complex(&num, &x1, &x2);
    complex_copy_complex_multiply_complex(&denom, &a, &num);
    complex_copy_complex_add_complex_(&b, &m1, &denom);
    c = f(x1);
		
    //m1 = (f(x2)-f(x1))/(x2-x1);
    //m3 = (f(x3)-f(x2))/(x3-x2);
    //a = (m3-m1)/(x3-x1);
    //b = m1+a*(x1-x2);
    //c = f(x1);

    complex_copy_complex_multiply_complex(&num, &b, &b);
    complex_copy_complex_multiply_complex(&denom, &a, &c);
    //complex_multiply_real(&denom, 4.0);
    denom.real *= 4.0, denom.imag *= 4.0;
    complex_subtract_complex(&num, &denom);
    complex_copy_complex_sqrt(&tmp, &num);
		
    if (complex_abs(&b) > 0) {
      complex_copy_complex_add_complex(&denom, &b, &tmp);
      num.real = c.real*2.0, num.imag = c.imag*2.0;
      complex_copy_complex_divide_complex(&tmp, &num, &denom);
      complex_copy_complex_subtract_complex(x, &x1, &tmp);
    } else {
      complex_copy_complex_subtract_complex(&denom, &b, &tmp);
      num.real = c.real*2.0, num.imag = c.imag*2.0;
      complex_copy_complex_divide_complex(&tmp, &num, &denom);
      complex_copy_complex_subtract_complex(x, &x1, &tmp);
    }
    //if (b > 0) *x = x1-2.0*c/(b+sqrt(b*b-4.0*a*c));
    //else *x = x1-2.0*c/(b-sqrt(b*b-4.0*a*c));
		
    // termination ?
    complex_copy_complex_subtract_complex(&tmp, x, &xprev);
    if (complex_abs(&tmp) < DOUBLE_EPSILON) return 0;
    //if (abs((*x)-xprev) < DOUBLE_EPSILON) return 0;
		
    // rearrange
    if (complex_cmp(x, &x1) < 0) {
      //if ((*x) < x1) {
      x3 = x2;
      x2 = x1;
      x1 = *x;
    } else if (complex_cmp(x, &x2) < 0) {
      //} else if ((*x) < x2) {
      x3 = x2;
      x2 = *x;
    } else if (complex_cmp(x, &x3) < 0) {
      //} else if ((*x) < x3) {
      x3 = *x;
    } else {
      x1 = x2;
      x2 = x3;
      x3 = *x;
    }
    xprev = *x;
  } while (--maxtries);
  return -1;
}

//int find_root_using_bairstow_method(double *x, function_t *f, 
