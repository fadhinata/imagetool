#ifndef CRB_DCT_H
#define CRB_DCT_H

typedef unsigned char	RAWDATA;
typedef int				DCTDATA;

#define RAWCENTER	0x80
#define RAWMAX		0xFF
#define RAWMIN		0

#define DCTSIZE2 64
#define DCTSIZE	 8

#define DCTLINE  DCTSIZE
#define DCTBLOCK DCTSIZE2

/** these sizes are really hard-coded, we just try to use
	them for clarity **/

extern void dct_init(int * qtbl);
extern void dct(RAWDATA * in_data, DCTDATA * out_data );
extern void idct(DCTDATA * coef_block, RAWDATA * output_buf);
extern void idct_init(int * qtbl);

#define cap_data(val) min(RAWMAX,max(RAWMIN,(val)))

#define unroll_dctline(op) if ( 0 ) ; else { op; op; op; op; op; op; op; op; }

#endif //CRB_DCT_H

