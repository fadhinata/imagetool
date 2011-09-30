
#include "dct.h"

/* Dequantize a coefficient by multiplying it by the multiplier-table
 * entry; produce a double result.
 */

void idct(DCTDATA * coef_block, RAWDATA * output_buf);
void idct_init(int * qtbl);

#define DEQUANTIZE(coef,quantval)  (((double) (coef)) * (quantval))

/*
 * Perform dequantization and inverse DCT on one block of coefficients.
 */

static double dct_table[DCTSIZE2];

void idct(DCTDATA * coef_block, RAWDATA * output_buf)
{
double tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
double tmp10, tmp11, tmp12, tmp13;
double z5, z10, z11, z12, z13;
DCTDATA * inptr;
RAWDATA * outptr;
double * quantptr;
double * wsptr;
int ctr;
double workspace[DCTSIZE2]; /* buffers data between passes */

  /* Pass 1: process columns from input, store into work array. */

  quantptr = dct_table;
  inptr = coef_block;
  wsptr = workspace;
  for (ctr = DCTSIZE; ctr > 0; ctr--) {
    /* Due to quantization, we will usually find that many of the input
     * coefficients are zero, especially the AC terms.  We can exploit this
     * by short-circuiting the IDCT calculation for any column in which all
     * the AC terms are zero.  In that case each output is equal to the
     * DC coefficient (with scale factor as needed).
     * With typical images and quantization tables, half or more of the
     * column DCT calculations can be simplified this way.
     */
    
    if ((inptr[DCTSIZE*1] | inptr[DCTSIZE*2] | inptr[DCTSIZE*3] |
	 inptr[DCTSIZE*4] | inptr[DCTSIZE*5] | inptr[DCTSIZE*6] |
	 inptr[DCTSIZE*7]) == 0) {
      /* AC terms all zero */
      double dcval = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
      
      wsptr[DCTSIZE*0] = dcval;
      wsptr[DCTSIZE*1] = dcval;
      wsptr[DCTSIZE*2] = dcval;
      wsptr[DCTSIZE*3] = dcval;
      wsptr[DCTSIZE*4] = dcval;
      wsptr[DCTSIZE*5] = dcval;
      wsptr[DCTSIZE*6] = dcval;
      wsptr[DCTSIZE*7] = dcval;
      
      inptr++;			/* advance pointers to next column */
      quantptr++;
      wsptr++;
      continue;
    }
    
    /* Even part */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    tmp2 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    tmp3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    tmp10 = tmp0 + tmp2;	/* phase 3 */
    tmp11 = tmp0 - tmp2;

    tmp13 = tmp1 + tmp3;	/* phases 5-3 */
    tmp12 = (tmp1 - tmp3) * ((double) 1.414213562) - tmp13; /* 2*c4 */

    tmp0 = tmp10 + tmp13;	/* phase 2 */
    tmp3 = tmp10 - tmp13;
    tmp1 = tmp11 + tmp12;
    tmp2 = tmp11 - tmp12;
    
    /* Odd part */

    tmp4 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    tmp5 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    tmp6 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    tmp7 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    z13 = tmp6 + tmp5;		/* phase 6 */
    z10 = tmp6 - tmp5;
    z11 = tmp4 + tmp7;
    z12 = tmp4 - tmp7;

    tmp7 = z11 + z13;		/* phase 5 */
    tmp11 = (z11 - z13) * ((double) 1.414213562); /* 2*c4 */

    z5 = (z10 + z12) * ((double) 1.847759065); /* 2*c2 */
    tmp10 = ((double) 1.082392200) * z12 - z5; /* 2*(c2-c6) */
    tmp12 = ((double) -2.613125930) * z10 + z5; /* -2*(c2+c6) */

    tmp6 = tmp12 - tmp7;	/* phase 2 */
    tmp5 = tmp11 - tmp6;
    tmp4 = tmp10 + tmp5;

    wsptr[DCTSIZE*0] = tmp0 + tmp7;
    wsptr[DCTSIZE*7] = tmp0 - tmp7;
    wsptr[DCTSIZE*1] = tmp1 + tmp6;
    wsptr[DCTSIZE*6] = tmp1 - tmp6;
    wsptr[DCTSIZE*2] = tmp2 + tmp5;
    wsptr[DCTSIZE*5] = tmp2 - tmp5;
    wsptr[DCTSIZE*4] = tmp3 + tmp4;
    wsptr[DCTSIZE*3] = tmp3 - tmp4;

    inptr++;			/* advance pointers to next column */
    quantptr++;
    wsptr++;
  }
  
  /* Pass 2: process rows from work array, store into output array. */
  /* Note that we must descale the results by a factor of 8 == 2**3. */

  wsptr = workspace;
  outptr = output_buf;
  for (ctr = 0; ctr < DCTSIZE; ctr++) {
    
    /* Even part */

    tmp10 = wsptr[0] + wsptr[4];
    tmp11 = wsptr[0] - wsptr[4];

    tmp13 = wsptr[2] + wsptr[6];
    tmp12 = (wsptr[2] - wsptr[6]) * ((double) 1.414213562) - tmp13;

    tmp0 = tmp10 + tmp13;
    tmp3 = tmp10 - tmp13;
    tmp1 = tmp11 + tmp12;
    tmp2 = tmp11 - tmp12;

    /* Odd part */

    z13 = wsptr[5] + wsptr[3];
    z10 = wsptr[5] - wsptr[3];
    z11 = wsptr[1] + wsptr[7];
    z12 = wsptr[1] - wsptr[7];

    tmp7 = z11 + z13;
    tmp11 = (z11 - z13) * ((double) 1.414213562);

    z5 = (z10 + z12) * ((double) 1.847759065); /* 2*c2 */
    tmp10 = ((double) 1.082392200) * z12 - z5; /* 2*(c2-c6) */
    tmp12 = ((double) -2.613125930) * z10 + z5; /* -2*(c2+c6) */

    tmp6 = tmp12 - tmp7;
    tmp5 = tmp11 - tmp6;
    tmp4 = tmp10 + tmp5;

    /* Final output stage: scale down by a factor of 8 and range-limit */

#define DESCALE(x,n)  (((x) + ((int)1 << ((n)-1)))>>(n))

    outptr[0] = cap_data((int) DESCALE((int) (tmp0 + tmp7), 3) + RAWCENTER );
    outptr[7] = cap_data((int) DESCALE((int) (tmp0 - tmp7), 3) + RAWCENTER  );
    outptr[1] = cap_data((int) DESCALE((int) (tmp1 + tmp6), 3) + RAWCENTER  );
    outptr[6] = cap_data((int) DESCALE((int) (tmp1 - tmp6), 3) + RAWCENTER  );
    outptr[2] = cap_data((int) DESCALE((int) (tmp2 + tmp5), 3) + RAWCENTER  );
    outptr[5] = cap_data((int) DESCALE((int) (tmp2 - tmp5), 3) + RAWCENTER  );
    outptr[4] = cap_data((int) DESCALE((int) (tmp3 + tmp4), 3) + RAWCENTER  );
    outptr[3] = cap_data((int) DESCALE((int) (tmp3 - tmp4), 3) + RAWCENTER  );
    
	outptr += DCTSIZE;
    wsptr += DCTSIZE;		/* advance pointer to next row */
  }
}

void idct_init(int * qtbl)
{
int i;
int row, col;
static const double aanscalefactor[DCTSIZE] = {
	1.0, 1.387039845, 1.306562965, 1.175875602,
	1.0, 0.785694958, 0.541196100, 0.275899379
};

	/* For float AA&N IDCT method, multipliers are equal to quantization
	 * coefficients scaled by scalefactor[row]*scalefactor[col], where
	 *   scalefactor[0] = 1
	 *   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
	 */

	i = 0;
	for (row = 0; row < DCTSIZE; row++) {
	  for (col = 0; col < DCTSIZE; col++) {
	    dct_table[i] = ((double) qtbl[i] * aanscalefactor[row] * aanscalefactor[col]);
	    i++;
	  }
	}
}

