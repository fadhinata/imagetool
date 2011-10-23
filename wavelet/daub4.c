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
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include <wavelet/daub4.h>

#define SQRT3 1.732050808

// forward transform scaling (smoothing) coefficients
#define H0 0.4829629131  // (1 + sqrt(3) / (4 * sqrt(2))
#define H1 0.8365163036  // (3 + sqrt(3) / (4 * sqrt(2))
#define H2 0.2241438679  // (3 - sqrt(3) / (4 * sqrt(2))
#define H3 -0.1294095226 // (1 - sqrt(3) / (4 * sqrt(2))

// forward transform wavelet coefficients
#define G0 H3
#define G1 -H2
#define G2 H1
#define G3 -H0

#define IH0 H2
#define IH1 G2
#define IH2 H0
#define IH3 G0

#define IG0 H3
#define IG1 G3
#define IG2 H1
#define IG3 G1

// The Daubechies D4 Wavelet Transform
void daub4_fwt(vector_t *q, vector_t *p)
{
  int i, j, n;
  int half;
  real_t *temp;
	
  assert(q);
  assert(p);
  assert(vector_get_dimension(q) == vector_get_dimension(p));
  assert(vector_get_dimension(p) >= 4);
	
  n = vector_get_dimension(p);

  temp = (real_t *)malloc(n * sizeof(real_t));
  assert(temp);

  vector_copy(q, p);

  for (; n >= 4; n >>= 1) {
    // Forward Daubechies D4 transform
    half = n >> 1;
    i = 0;
    for (i = 0, j = 0; j < n-3; j += 2, i++) {
      temp[i]      = (vector_get_value(q, j)   * H0 + vector_get_value(q, j+1) * H1 +
		      vector_get_value(q, j+2) * H2 + vector_get_value(q, j+3) * H3);
      temp[half+i] = (vector_get_value(q, j)   * G0 + vector_get_value(q, j+1) * G1 +
		      vector_get_value(q, j+2) * G2 + vector_get_value(q, j+3) * G3);
    }
    temp[i]      = (vector_get_value(q, n-2) * H0 + vector_get_value(q, n-1) * H1 +
		    vector_get_value(q, 0)   * H2 + vector_get_value(q, 1)   * H3);
    temp[half+i] = (vector_get_value(q, n-2) * G0 + vector_get_value(q, n-1) * G1 +
		    vector_get_value(q, 0)   * G2 + vector_get_value(q, 1)   * G3);
    for (i = 0; i < n; i++) vector_put_value(temp[i], q, i);
  }
  free(temp);
}

void daub4_ifwt(vector_t *q, vector_t *p)
{
  int i, j, n;
  int half;
  real_t *temp;
	
  assert(q);
  assert(p);
  assert(vector_get_dimension(q) == vector_get_dimension(p));
  assert(vector_get_dimension(p) >= 4);
	
  n = vector_get_dimension(p);

  temp = (real_t *)malloc(n * sizeof(real_t));
  assert(temp);

  vector_copy(q, p);
	
  for (n = 4; n <= vector_get_dimension(p); n <<= 1) {
    half = n >> 1;
    // last smooth val  last coef.  first smooth  first coef
    temp[0] = (vector_get_value(q, half-1) * IH0 + vector_get_value(q, n-1)  * IH1 +
	       vector_get_value(q, 0)      * IH2 + vector_get_value(q, half) * IH3);
    temp[1] = (vector_get_value(q, half-1) * IG0 + vector_get_value(q, n-1)  * IG1 +
	       vector_get_value(q, 0)      * IG2 + vector_get_value(q, half) * IG3);

    for (i = 0, j = 2; i < half - 1; i++) {
      //     smooth val     coef. val       smooth val     coef. val
      temp[j++] = (vector_get_value(q, i)   * IH0 + vector_get_value(q, half)   * IH1 +
		   vector_get_value(q, i+1) * IH2 + vector_get_value(q, half+1) * IH3);
      temp[j++] = (vector_get_value(q, i)   * IH0 + vector_get_value(q, half)   * IH1 +
		   vector_get_value(q, i+1) * IH2 + vector_get_value(q, half+1) * IH3);
    }
    for (i = 0; i < n; i++) vector_put_value(temp[i], p, i);
 }
  free(temp);
}


#if 0
class Daubechies {
 private:
  /** forward transform scaling coefficients */
  double h0, h1, h2, h3;
  /** forward transform wave coefficients */
  double g0, g1, g2, g3;

  double Ih0, Ih1, Ih2, Ih3;
  double Ig0, Ig1, Ig2, Ig3;

  /**
     Forward Daubechies D4 transform
  */
  void transform( double* a, const int n )
  {
    if (n >= 4) {
      int i, j;
      const int half = n >> 1;
         
      double* tmp = new double[n];

      for (i = 0, j = 0; j < n-3; j += 2, i++) {
	tmp[i]      = a[j]*h0 + a[j+1]*h1 + a[j+2]*h2 + a[j+3]*h3;
	tmp[i+half] = a[j]*g0 + a[j+1]*g1 + a[j+2]*g2 + a[j+3]*g3;
      }

      tmp[i]      = a[n-2]*h0 + a[n-1]*h1 + a[0]*h2 + a[1]*h3;
      tmp[i+half] = a[n-2]*g0 + a[n-1]*g1 + a[0]*g2 + a[1]*g3;

      for (i = 0; i < n; i++) {
	a[i] = tmp[i];
      }
      delete [] tmp;
    }
  }

  /**
     Inverse Daubechies D4 transform
  */
  void invTransform( double* a, const int n )
  {
    if (n >= 4) {
      int i, j;
      const int half = n >> 1;
      const int halfPls1 = half + 1;

      double* tmp = new double[n];

      //      last smooth val  last coef.  first smooth  first coef
      tmp[0] = a[half-1]*Ih0 + a[n-1]*Ih1 + a[0]*Ih2 + a[half]*Ih3;
      tmp[1] = a[half-1]*Ig0 + a[n-1]*Ig1 + a[0]*Ig2 + a[half]*Ig3;
      for (i = 0, j = 2; i < half-1; i++) {
	//     smooth val     coef. val       smooth val    coef. val
	tmp[j++] = a[i]*Ih0 + a[i+half]*Ih1 + a[i+1]*Ih2 + a[i+halfPls1]*Ih3;
	tmp[j++] = a[i]*Ig0 + a[i+half]*Ig1 + a[i+1]*Ig2 + a[i+halfPls1]*Ig3;
      }
      for (i = 0; i < n; i++) {
	a[i] = tmp[i];
      }
      delete [] tmp;
    }
  }


 public:

  Daubechies() 
    {
      const double sqrt_3 = sqrt( 3 );
      const double denom = 4 * sqrt( 2 );

      //
      // forward transform scaling (smoothing) coefficients
      //
      h0 = (1 + sqrt_3)/denom;
      h1 = (3 + sqrt_3)/denom;
      h2 = (3 - sqrt_3)/denom;
      h3 = (1 - sqrt_3)/denom;
      //
      // forward transform wavelet coefficients
      //
      g0 =  h3;
      g1 = -h2;
      g2 =  h1;
      g3 = -h0;

      Ih0 = h2;
      Ih1 = g2;  // h1
      Ih2 = h0;
      Ih3 = g0;  // h3

      Ig0 = h3;
      Ig1 = g3;  // -h0
      Ig2 = h1;
      Ig3 = g1;  // -h2
    }

  void daubTrans( double* ts, int N )
  {
    int n;
    for (n = N; n >= 4; n >>= 1) {
      transform( ts, n );
    }
  }


  void invDaubTrans( double* coef, int N )
  {
    int n;
    for (n = 4; n <= N; n <<= 1) {
      invTransform( coef, n );
    }
  }

}; // Daubechies

#endif

