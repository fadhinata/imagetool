
#include "dct.h"

void dct_init(int * qtbl);
void dct(RAWDATA * in_data, DCTDATA * out_data );

static double divisors[DCTSIZE2];

void dct(RAWDATA * in_data, DCTDATA * out_data )
{
double tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
double tmp10, tmp11, tmp12, tmp13;
double z1, z2, z3, z4, z5, z11, z13;
double *dataptr;
double data[DCTSIZE2];
int ctr,i;

	/* step 0 : read in doubles , make data signed */

	dataptr = data;
	for(i=0;i<DCTSIZE;i++) {
		*dataptr++ = (double)( *in_data++ - RAWCENTER  );
		*dataptr++ = (double)( *in_data++ - RAWCENTER );
		*dataptr++ = (double)( *in_data++ - RAWCENTER );
		*dataptr++ = (double)( *in_data++ - RAWCENTER );
		*dataptr++ = (double)( *in_data++ - RAWCENTER );
		*dataptr++ = (double)( *in_data++ - RAWCENTER );
		*dataptr++ = (double)( *in_data++ - RAWCENTER );
		*dataptr++ = (double)( *in_data++ - RAWCENTER );
	}

  /* Pass 1: process rows. */

  dataptr = data;
  for (ctr = DCTSIZE-1; ctr >= 0; ctr--) {
    tmp0 = dataptr[0] + dataptr[7];
    tmp7 = dataptr[0] - dataptr[7];
    tmp1 = dataptr[1] + dataptr[6];
    tmp6 = dataptr[1] - dataptr[6];
    tmp2 = dataptr[2] + dataptr[5];
    tmp5 = dataptr[2] - dataptr[5];
    tmp3 = dataptr[3] + dataptr[4];
    tmp4 = dataptr[3] - dataptr[4];
    
    /* Even part */
    
    tmp10 = tmp0 + tmp3;	/* phase 2 */
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    dataptr[0] = tmp10 + tmp11; /* phase 3 */
    dataptr[4] = tmp10 - tmp11;
    
    z1 = (tmp12 + tmp13) * ((double) 0.707106781); /* c4 */
    dataptr[2] = tmp13 + z1;	/* phase 5 */
    dataptr[6] = tmp13 - z1;
    
    /* Odd part */

    tmp10 = tmp4 + tmp5;	/* phase 2 */
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    /* The rotator is modified from fig 4-8 to avoid extra negations. */
    z5 = (tmp10 - tmp12) * ((double) 0.382683433); /* c6 */
    z2 = ((double) 0.541196100) * tmp10 + z5; /* c2-c6 */
    z4 = ((double) 1.306562965) * tmp12 + z5; /* c2+c6 */
    z3 = tmp11 * ((double) 0.707106781); /* c4 */

    z11 = tmp7 + z3;		/* phase 5 */
    z13 = tmp7 - z3;

    dataptr[5] = z13 + z2;	/* phase 6 */
    dataptr[3] = z13 - z2;
    dataptr[1] = z11 + z4;
    dataptr[7] = z11 - z4;

    dataptr += DCTSIZE;		/* advance pointer to next row */
  }

  /* Pass 2: process columns. */

  dataptr = data;
  for (ctr = DCTSIZE-1; ctr >= 0; ctr--) {
    tmp0 = dataptr[DCTSIZE*0] + dataptr[DCTSIZE*7];
    tmp7 = dataptr[DCTSIZE*0] - dataptr[DCTSIZE*7];
    tmp1 = dataptr[DCTSIZE*1] + dataptr[DCTSIZE*6];
    tmp6 = dataptr[DCTSIZE*1] - dataptr[DCTSIZE*6];
    tmp2 = dataptr[DCTSIZE*2] + dataptr[DCTSIZE*5];
    tmp5 = dataptr[DCTSIZE*2] - dataptr[DCTSIZE*5];
    tmp3 = dataptr[DCTSIZE*3] + dataptr[DCTSIZE*4];
    tmp4 = dataptr[DCTSIZE*3] - dataptr[DCTSIZE*4];
    
    /* Even part */
    
    tmp10 = tmp0 + tmp3;	/* phase 2 */
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    dataptr[DCTSIZE*0] = tmp10 + tmp11; /* phase 3 */
    dataptr[DCTSIZE*4] = tmp10 - tmp11;
    
    z1 = (tmp12 + tmp13) * ((double) 0.707106781); /* c4 */
    dataptr[DCTSIZE*2] = tmp13 + z1; /* phase 5 */
    dataptr[DCTSIZE*6] = tmp13 - z1;
    
    /* Odd part */

    tmp10 = tmp4 + tmp5;	/* phase 2 */
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    /* The rotator is modified from fig 4-8 to avoid extra negations. */
    z5 = (tmp10 - tmp12) * ((double) 0.382683433); /* c6 */
    z2 = ((double) 0.541196100) * tmp10 + z5; /* c2-c6 */
    z4 = ((double) 1.306562965) * tmp12 + z5; /* c2+c6 */
    z3 = tmp11 * ((double) 0.707106781); /* c4 */

    z11 = tmp7 + z3;		/* phase 5 */
    z13 = tmp7 - z3;

    dataptr[DCTSIZE*5] = z13 + z2; /* phase 6 */
    dataptr[DCTSIZE*3] = z13 - z2;
    dataptr[DCTSIZE*1] = z11 + z4;
    dataptr[DCTSIZE*7] = z11 - z4;

    dataptr++;			/* advance pointer to next column */
  }

  /* step 3: Quantize/descale the coefficients, and store into coef_blocks[] */
      for (i = 0; i < DCTSIZE2; i++) {
		/* Apply the quantization and scaling factor */
		out_data[i] = (DCTDATA) (data[i] * divisors[i]);
	
		/* Round to nearest integer.
		 * Since C does not specify the direction of rounding for negative
		 * quotients, we have to force the dividend positive for portability.
		 * The maximum coefficient size is +-16K (for 12-bit data), so this
		 * code should work for either 16-bit or 32-bit ints.
		 */
		/** out_data[i] = (DCTDATA) ((int) (data[i] * divisors[i] + (double) 16384.5)
		 *		 - 16384); 
		**/
       }

}

void dct_init(int * qtbl)
{
int row, col,i;
static const double aanscalefactor[DCTSIZE] = {
  1.0, 1.387039845, 1.306562965, 1.175875602,
  1.0, 0.785694958, 0.541196100, 0.275899379
};

	/* For double AA&N IDCT method, divisors are equal to quantization
	 * coefficients scaled by scalefactor[row]*scalefactor[col], where
	 *   scalefactor[0] = 1
	 *   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
	 * We apply a further scale factor of 8.
	 * What's actually stored is 1/divisor so that the inner loop can
	 * use a multiplication rather than a division.
	 */

	i = 0;
	for (row = 0; row < DCTSIZE; row++) {
	  for (col = 0; col < DCTSIZE; col++) {
	    divisors[i] = (double)
	      (1.0 / (((double) qtbl[i] * aanscalefactor[row] * aanscalefactor[col] * 8.0)));
	    i++;
	  }
	}

}

