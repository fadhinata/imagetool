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
#include <daub4.h>
#include <assert.h>

#define SQRT3 1.732050808

#define H0 0.4829629131
#define H1 0.8365163036
#define H2 0.2241438679
#define H3 -0.1294095226
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

/* The Daubechies D4 Wavelet Transform */
void d4fwt(vector_t *q, vector_t *p)
{
  int i, j, n;
  int half;
  double *temp;
	
  assert(q);
  assert(p);
  assert(q->length == p->length);
  assert(p->length >= 4);
	
  n = p->length;
  temp = (double *)malloc(n*sizeof(double));
  assert(temp);

  memcpy(q->real, p->real, n*sizeof(double));

  for (; n >= 4; n >>= 1) {
    half = n>>1;
    i = 0;
    for (i = 0, j = 0; j < n-3; j += 2, i++) {
      temp[i]      = *(q->real+j)*H0+*(q->real+j+1)*H1+*(q->real+j+2)*H2+*(q->real+j+3)*H3;
      temp[i+half] = *(q->real+j)*G0+*(q->real+j+1)*G1+*(q->real+j+2)*G2+*(q->real+j+3)*G3;
    }
    temp[i]      = *(q->real+n-2)*H0+*(q->real+n-1)*H1+*(q->real)*H2+*(q->real+1)*H3;
    temp[i+half] = *(q->real+n-2)*G0+*(q->real+n-1)*G1+*(q->real)*G2+*(q->real+1)*G3;
    for (i = 0; i < n; i++) *(q->real+i) = temp[i];
  }
  free(temp);
}

void id4fwt(vector_t *q, vector_t *p)
{
  int i, j, n;
  int half;
  double *temp;
	
  assert(q);
  assert(p);
  assert(q->length == p->length);
  assert(p->length >= 4);
	
  n = p->length;
  temp = (double *)malloc(n*sizeof(double));
  assert(temp);

  memcpy(q->real, p->real, n*sizeof(double));
	
  for (n = 4; n <= p->length; n <<= 1) {
    half = n>>1;
    //      last smooth val  last coef.  first smooth  first coef
    temp[0] = *(p->real+half-1)*IH0 + *(p->real+n-1)*IH1 + *(p->real)*IH2 + *(p->real+half)*IH3;
    temp[1] = *(p->real+half-1)*IG0 + *(p->real+n-1)*IG1 + *(p->real)*IG2 + *(p->real+half)*IG3;
    for (i = 0, j = 2; i < half-1; i++) {
      //     smooth val     coef. val       smooth val     coef. val
      temp[j++] = *(p->real+i)*IH0 + *(p->real+i+half)*IH1 + *(p->real+i+1)*IH2 + *(p->real+i+half+1)*IH3;
      temp[j++] = *(p->real+i)*IG0 + *(p->real+i+half)*IG1 + *(p->real+i+1)*IG2 + *(p->real+i+half+1)*IG3;
    }
    for (i = 0; i < n; i++) *(p->real+i) = temp[i];
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

