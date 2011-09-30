
/*********

we must transmit the quant-tables if we customize them.
This will crap out performance on small files.

----

tail smashing.

preliminary experiments on checa show that tail smashing
only helps for ratio ~ 100 , and is irrelevant around ratio ~ 1

	quality Q @ no tail smash
			equiv
	quality (Q+1) @ tail smash J *= 60

----

notes:

the "rateD" computation is *NEGATIVE* (indicating that a lower
rate gives *better* quality) and some sub-sections of the
image!  This indicates that some locally-adaptive quantization
could be smashing.

*********/

#define SMASH_SOUGHT_MIN		2
#define SMASH_DISTORTION_DIV	15
#define SMASH_SOUGHT_MULT		(0.4)
//#define SMASH_SOUGHT_MULT		(99)
//#define SMASH_SOUGHT_MULT		(tune_param/10.0)
	/** surprisingly, small changes in these params
		don't affect the R-D curves at all! **/

#define VERBOSE
#define DO_LOG
#define DEBUG

//#define FIDDLE_QUANTIZERS

//#define NO_CODING
#define VISUAL_QUANTIZERS // else uniform, ** hurts the raw numbers!
#define DPCM_ORDER_M1	// else order0, ** order(-1) is safer
//#define DPCM_ORDER_0	// else order1 : order1 is best on Lena
#define FLAT_ORDER_M1	// else jpeg : flat is best for o(-1)
//#define JPEG_ORDER_M1 // else custom 
#define USE_EOBS // ** helps about 0.02 bpb
#define CODE_SIGNS // helps on files bigger than 256x256

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <crbinc/inc.h>
#include <crbinc/fileutil.h>
#include <crbinc/cindcatr.h>
#include <crbinc/timer.h>
#include <crbinc/bbitio.h>
#include <crbinc/arithc.h>
#include <crbinc/scontext.h>
#include <crbinc/context.h>

#include "dct.h"

static const int qtable_jpeg[DCTBLOCK] =
   { 16,  11,  10,  16,  24,  40,  51,  61,
     12,  12,  14,  19,  26,  58,  60,  55,
     14,  13,  16,  24,  40,  57,  69,  56,
     14,  17,  22,  29,  51,  87,  80,  62,
     18,  22,  37,  56,  68, 109, 103,  77,
     24,  35,  55,  64,  81, 104, 113,  92,
     49,  64,  78,  87, 103, 121, 120, 101,
     72,  92,  95,  98, 112, 100, 103,  99 };

static const int qtable_jpeg_flattened[DCTBLOCK] =
   { 18,  23,  22,  28,  34,  44,  50,  55,
	 24,  24,  26,  31,  36,  53,  54,  52,
	 26,  25,  28,  34,  44,  53,  58,  52,
	 26,  29,  33,  38,  50,  65,  63,  55,
	 30,  33,  43,  52,  58,  73,  71,  61,
	 34,  41,  52,  56,  63,  71,  74,  67,
	 49,  56,  62,  65,  71,  77,  77,  70,
	 59,  67,  68,  69,  74,  70,  71,  70};

static const int qtable_semiuniform[DCTBLOCK] =
   { 16,  32,  64,  64,  64,  64,  64,  64,
     32,  64,  64,  64,  64,  64,  64,  64,
     64,  64,  64,  64,  64,  64,  64,  64,
     64,  64,  64,  64,  64,  64,  64,  64,
     64,  64,  64,  64,  64,  64,  64,  64,
     64,  64,  64,  64,  64,  64,  64,  64,
     64,  64,  64,  64,  64,  64,  64,  64,
     64,  64,  64,  64,  64,  64,  64,  64 };

static const int qtable_uniform[DCTBLOCK] =
   { 100,  100,  100,  100,  100,  100,  100,  100,
     100,  100,  100,  100,  100,  100,  100,  100,
     100,  100,  100,  100,  100,  100,  100,  100,
     100,  100,  100,  100,  100,  100,  100,  100,
     100,  100,  100,  100,  100,  100,  100,  100,
     100,  100,  100,  100,  100,  100,  100,  100,
     100,  100,  100,  100,  100,  100,  100,  100,
     100,  100,  100,  100,  100,  100,  100,  100 };

static const int qtable_none[DCTBLOCK] =
   { 1,   1,   1,   1,   1,   1,   1,   1,
     1,   1,   1,   1,   1,   1,   1,   1,
     1,   1,   1,   1,   1,   1,   1,   1,
     1,   1,   1,   1,   1,   1,   1,   1,
     1,   1,   1,   1,   1,   1,   1,   1,
     1,   1,   1,   1,   1,   1,   1,   1,
     1,   1,   1,   1,   1,   1,   1,   1,
     1,   1,   1,   1,   1,   1,   1,   1 };

static const int qtable_jpeg_squashed[DCTBLOCK] = {	/** 4*sqrt(jpeg_flat) **/
	18, 19, 19, 21, 23, 27, 28, 30,
	20, 20, 21, 22, 24, 29, 30, 29,
	21, 20, 21, 23, 27, 29, 31, 29,
	21, 22, 23, 25, 28, 32, 32, 30,
	22, 23, 26, 29, 31, 34, 34, 31,
	23, 26, 29, 30, 32, 34, 35, 33,
	28, 30, 32, 32, 34, 35, 35, 34,
	31, 33, 33, 33, 35, 34, 34, 34 };

#ifdef VISUAL_QUANTIZERS
//#define quant_table qtable_jpeg
#define quant_table qtable_jpeg_squashed
#else // UNIFORM QUANTIZERS
#define quant_table qtable_semiuniform
//#define quant_table qtable_uniform /** smashing is a clear win here
#endif

static const int zigzag[DCTBLOCK] = {
	  0,  1,  8, 16,  9,  2,  3, 10,
	 17, 24, 32, 25, 18, 11,  4,  5,
	 12, 19, 26, 33, 40, 48, 41, 34,
	 27, 20, 13,  6,  7, 14, 21, 28,
	 35, 42, 49, 56, 57, 50, 43, 36,
	 29, 22, 15, 23, 30, 37, 44, 51,
	 58, 59, 52, 45, 38, 31, 39, 46,
	 53, 60, 61, 54, 47, 55, 62, 63 };

/*** unzigzag : (never used)
{ 0,  1,  5,  6, 14, 15, 27, 28,  2,  4,  7, 13, 16, 26, 29, 42, 3,  8, 12, 17,
25, 30, 41, 43,  9, 11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52, 54,
20, 22, 33, 38, 46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49,
57, 58, 62, 63 };
*******/

static int zag_x[DCTBLOCK] = { 	// (zigzag & 7)
	0, 1, 0,
	0, 1, 2, 3, 2, 1, 0, 
	0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 
	0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0,
	1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2,
	3, 4, 5, 6, 7, 7, 6, 5, 4,
	5, 6, 7, 7, 6, 7 };

static int zag_y[DCTBLOCK] = {	// (zigzag>>3)
	0,
	0, 1, 2, 1, 0,
	0, 1, 2, 3, 4, 3, 2, 1, 0,
	0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0,
	0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1,
	2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3,
	4, 5, 6, 7, 7, 6, 5,
	6, 7, 7 };

#define cleanup(str) if ( 0 ) ; else { errputs(str); exit(10); }

#ifdef DO_LOG
#define LOG(x)	if (0) ; else x;
#else
#define LOG(x)	if (1) ; else x;
#endif

#define TRANS_MAX		(0xFFF)
#define TRANS_MAX_EV	(0xFFE)
#define TRANS_TOPBIT	(0x800)
#define TRANS_LOWBIT	(0x002)	// since 001 is the sign
#define VAL_MAX 		(TRANS_TOPBIT)
#define VAL_EOB			(VAL_MAX)

#define TRANS_EOB		(TRANS_MAX+1)
#define TRANS_DONT_CODE (TRANS_MAX+2)

#define data_trans(val) ( ((val) < 0 ) ? ( -1+min(TRANS_MAX_EV,(-val-val))) : (min(TRANS_MAX_EV,(val+val))) )
#define trans_data(val) (( (val) == TRANS_EOB || (val) == TRANS_DONT_CODE ) ? 0 : ( ((val) & 1 ) ? (-((val+1)>>1)) : (((val)>>1)) ))

int log_n0,log_n1,log_nm1;
int log_zero_len,log_zero_len_smashed,log_zero_len_gained;
int log_0p0,log_0p1,log_0pbig;
int log_nsign_noc,log_nsign_same,log_nsign_diff;
int log_nsign,log_nsign_dpcm;
int log_max_RoDs5,log_min_RoDs5;

#define RESET_LOG() if ( 0 ) ; else { log_n0=log_n1=log_nm1=log_zero_len=log_zero_len_smashed=log_zero_len_gained=log_0p0=log_0p1=log_0pbig=log_nsign_noc=log_nsign_same=log_nsign_diff=log_nsign=log_nsign_dpcm=log_max_RoDs5=log_min_RoDs5=0; };

#define CODE_CONTEXTS		9
static const int context_buckets[CODE_CONTEXTS] =
	{ 0,1,2,3,4,8,16,32, TRANS_MAX + 10 };	// doesn't make too much difference

#define ORDER1_TOTMAX		6000
#define ORDER1_ALPHABET		30		// infinity is optimum, but slow for scontext
#ifdef USE_EOBS
#define ORDER1_ESCAPE		(ORDER1_ALPHABET-2)
#define ORDER1_EOB			(ORDER1_ALPHABET-1)
#else
#define ORDER1_ESCAPE		(ORDER1_ALPHABET-1)
#endif // USE_EOBS
#define ORDER1_INC			15		//   and 30 is only 0.001 bpb worse than infinity

#define ORDER0_TOTMAX		2500
#define ORDER0_ALPHABET		(VAL_MAX + 2 - ORDER1_ALPHABET)

#ifdef CODE_SIGNS
#define SIGN_CONTEXTS		3
#define SIGNORDER0_TOTMAX	1000
#define SIGNORDER0_INC		5
#endif

#define FLAT_ORDER_M1_ESCAPE	(200)

#define M1_THRESH_1 8
#define M1_THRESH_2 24
#define M1_THRESH_3 40
#define M1_THRESH_4 100

#define PSNR_MAX 	(48.165)	//(10*log10(256^2))

int tune_param = 1;

ubyte **raw = NULL,**rawout = NULL;
uword **trans = NULL;
ubyte *comp = NULL,*signcomp = NULL,*rawcomp = NULL;
FILE *rawF = NULL;
struct FAI * FAI = NULL;
struct FAI * signFAI = NULL;
struct FAI * rawFAI = NULL;
struct BitIOInfo * BII = NULL;
struct BitIOInfo * signBII = NULL;
struct BitIOInfo * rawBII = NULL;
context *order0 = NULL;
#ifdef CODE_SIGNS
scontext **signorder0 = NULL;
#endif
scontext **order1 = NULL;
int num_planes;

void ExitFunc(void);
void encode(int sym,int context);
void encode_m1_jpeg(int sym);
void encode_m1_flat(int sym);
void encode_m1_custom(int sym);
void encodeSign(bool sym,int prev);

int code_image(ubyte **raw,ubyte **rawout,int width,int height,int num_planes,
	int * qtable,int J_sought_s5,int max_distortion,
	int * comp_dpcm_len_ptr,int *comp_ac_len_ptr,int *comp_sign_len_ptr,int *comp_raw_len_ptr);

void dct_image(ubyte **raw,uword **trans,int width,int height,int num_planes,
		int * quant_table);
void idct_image(ubyte **rawout,uword **trans,int width,int height,int num_planes,
		int * quant_table);

void TheImageAnalyzer(ubyte **original,ubyte **vs,
					int num_planes,int width,int height,
					float ratio,FILE *sio);

void init_allocs(int num_planes,int rawsize,int complen);
void init_coders(void);
int done_coders(void);
void free_coders(void);

int main(int argc,char *argv[])
{
char *t1,*t2;
int width,height;
int rawsize,complen,totsize;
int comp_tot_len,comp_dpcm_len,comp_ac_len,comp_sign_len,comp_raw_len,i;
clock_t startclock,stopclock;
int quality,quantizer,J_sought_s5,max_distortion;
float J_sought;
int qtable[DCTBLOCK];

	errputs("dct v0.5, (c) 1998 by Charles Bloom");

	if ( argc <= 3 ) {
		errputs("dct");
		errputs("dct <raw file> <width> <height> [planes] [quality] [tune param]");
		exit(10);
	}

	if ( atexit(ExitFunc) )
	  cleanup("Couldn't install exit trap!");

	if ( ( rawF = fopen(argv[1],"rb") ) == NULL )
		cleanup("open raw failed");

	width = atol(argv[2]);
	height = atol(argv[3]);

	if ( ((width/DCTLINE)*DCTLINE) != width )
		cleanup("size must be divable by 8");
	if ( ((height/DCTLINE)*DCTLINE) != height )
		cleanup("size must be divable by 8");

	if ( argc > 4 )
		num_planes = atol(argv[4]);
	else
		num_planes = 1;

	if ( argc > 5 )
		quality = min(max(1,atol(argv[5])),100);
	else
		quality = 50;

	if ( argc > 6 )
		tune_param = atol(argv[6]);
	else
		tune_param = 1;

	rawsize = width * height;
	totsize = rawsize*num_planes;
	complen = (totsize*9)>>3;

	init_allocs(num_planes,rawsize,complen);

	for(i=0;i<num_planes;i++) {
		if ( !FReadOk(rawF,raw[i],rawsize) )
			errputs("fread short! continuing..");
	}
	fclose(rawF); rawF = NULL;

	if ( quality >= 50 )	quantizer = (100 - quality)*2;
	else					quantizer = 5000/quality;

	for(i=0;i<DCTBLOCK;i++) {
		qtable[i] = (quant_table[i]*quantizer + 40)/100;
		if ( qtable[i] < 1 ) qtable[i] = 1;

		zag_y[i] *= width;
	}

	/**** measure the R_D slope for a change of "quality" ****/
#ifdef USE_EOBS // otherwise this is irrelevant
	do {
	int qtn[DCTBLOCK];
	int base_len,new_len,base_err,new_err,i,j,d;
	double dRoD;

		if ( quality > 99 )
			{ J_sought_s5 = 9999; max_distortion=0; break; }

		if ( quality >= 50 )	quantizer = (100 - (quality-1))*2;
		else					quantizer = 5000/(quality-1);

		for(i=0;i<DCTBLOCK;i++) {
			qtn[i] = (quant_table[i]*quantizer + 40)/100;
			if ( qtn[i] < 1 ) qtn[i] = 1;
		}

		base_len = code_image(raw,rawout,width,height,num_planes,qtable,1000,0,
			NULL,NULL,NULL,NULL);

		base_err=0;
		for(i=0;i<num_planes;i++) 
			for(j=0;j<rawsize;j++) {
				d = raw[i][j] - rawout[i][j];
				base_err += d*d;
		}

		new_len = code_image(raw,rawout,width,height,num_planes,qtn,1000,0,
			NULL,NULL,NULL,NULL);

		new_err=0;
		for(i=0;i<num_planes;i++) 
			for(j=0;j<rawsize;j++) {
				d = raw[i][j] - rawout[i][j];
				new_err += d*d;
		}

		max_distortion = base_err - new_err;
		if ( max_distortion == 0 ) max_distortion++;
		if ( max_distortion < 0 ) max_distortion = - max_distortion;

		dRoD = 8*(double)(new_len - base_len)/sqrt((double)max_distortion);
		if ( dRoD < 0 ) dRoD = - dRoD ;

#ifdef VERBOSE
		/*#*/{
		float base_rmse,new_rmse;
		base_rmse= sqrt((float)base_err/totsize);
		new_rmse= sqrt((float)new_err/totsize);
		printf("base: len= %d , err= %.2f (rmse=%.2f) , new: len= %d , err= %.2f (%.2f)\n",base_len,(float)base_err/totsize,base_rmse,new_len,(float)new_err/totsize,new_rmse);
		printf("dRoD = %f , perfo(rmse): base =%.3f, new=%.3f \n",dRoD,((float)totsize/base_len)/base_rmse,((float)totsize/new_len)/new_rmse);
		/*#*/}
#endif

		J_sought_s5 = (dRoD*32.0*SMASH_SOUGHT_MULT);

		if ( J_sought_s5 < SMASH_SOUGHT_MIN ) J_sought_s5 = SMASH_SOUGHT_MIN;

		max_distortion /= SMASH_DISTORTION_DIV;

	} while(0);
#endif // USE_EOBS

	/*** fiddle quantizers ***/
#ifdef FIDDLE_QUANTIZERS
	do {
	int b,d,j;
	int r_c,r_p,r_m,d_c,d_p,d_m,inc;
	int tot_wiggle=0,num_wiggles=0;
	bool active[DCTBLOCK];
	int nactive=64;

		/** make free changes which don't affect distortion,
			but decrease the rate ;
		very slow and helps just a tad. (Ratio/RMSE += 0.01) **/

		for(b=0;b<DCTBLOCK;b++)
			active[b] = true;

	while(nactive>0) {
		for(b=0;b<DCTBLOCK;b++) {
			if ( ! active[b] ) continue;
#ifdef VERBOSE
			fprintf(stderr,"%d\r",b); fflush(stderr);
#endif
			if ( qtable[b] == 1 )
				{ nactive--; active[b] = false; continue; } //** do this better

			inc = qtable[b]>>4; if ( inc < 1 ) inc = 1;

			r_c = code_image(raw,rawout,width,height,num_planes,qtable,1000,0, NULL,NULL,NULL,NULL);
			d_c=0; for(i=0;i<num_planes;i++) { ubyte *rptr,*vptr; rptr = raw[i]; vptr = rawout[i];
			for(j=rawsize;j--;) { d = *rptr++ - *vptr++; 
			d_c += d*d; } }
			d_c = sqrt((float)d_c);

			qtable[b] += inc;
			r_p = code_image(raw,rawout,width,height,num_planes,qtable,1000,0, NULL,NULL,NULL,NULL);
			d_p=0; for(i=0;i<num_planes;i++) { ubyte *rptr,*vptr; rptr = raw[i]; vptr = rawout[i];
			for(j=rawsize;j--;) { d = *rptr++ - *vptr++; 
			d_p += d*d; } }
			d_p = sqrt((float)d_p);

			qtable[b] -= (inc<<1);
			r_m = code_image(raw,rawout,width,height,num_planes,qtable,1000,0, NULL,NULL,NULL,NULL);
			d_m=0; for(i=0;i<num_planes;i++) { ubyte *rptr,*vptr; rptr = raw[i]; vptr = rawout[i];
			for(j=rawsize;j--;) { d = *rptr++ - *vptr++; 
			d_m += d*d; } }
			d_m = sqrt((float)d_m);

			qtable[b] += inc;

			r_p -= r_c;
			d_p -= d_c;
			r_m -= r_c;
			d_m -= d_c;

			if ( d_p <= 0 && r_p < 0  ) { // go plus
				qtable[b] += inc;
				tot_wiggle++;
				num_wiggles++;
#ifdef VERBOSE
				printf("  p: %d/%d , m: %d/%d\n",r_p,d_p,r_m,d_m);
#endif
			} else if ( d_m <= 0 && r_m < 0 ) {
				qtable[b] -= inc;
				tot_wiggle--;
				num_wiggles++;
#ifdef VERBOSE
				printf("  p: %d/%d , m: %d/%d\n",r_p,d_p,r_m,d_m);
#endif
			} else {
				active[b] = false;
				nactive--;
			}
		}
	}
#ifdef VERBOSE
		errputs("");
#endif

#ifdef VERBOSE
		printf("tot_wiggle = %d, num_wiggles = %d\n",tot_wiggle,num_wiggles);
#endif

	} while(0);
#endif // FIDDLE_QUANTIZERS

	RESET_LOG();

	startclock = clock();

	code_image(raw,rawout,width,height,num_planes,qtable,J_sought_s5,max_distortion,
		&comp_dpcm_len,&comp_ac_len,&comp_sign_len,&comp_raw_len);

	stopclock = clock();

	comp_tot_len = comp_sign_len + comp_ac_len + comp_raw_len + comp_dpcm_len;

	printf("%-13s :",FilePart(argv[1]));
	TheCompressionIndicator(totsize,comp_tot_len,stdout);
	printf(", %6ld byps\n",NumPerSec(totsize,stopclock-startclock));

#ifdef VERBOSE
	printf("%-13s :","DC dpcm");TheCompressionIndicator((totsize>>6),comp_dpcm_len,stdout); puts("");
	printf("%-13s :","AC order-0-1");  TheCompressionIndicator(totsize-(totsize>>6),comp_ac_len,stdout); puts("");
	LOG( printf("%-13s :","AC order(-1)");TheCompressionIndicator(log_nm1,comp_raw_len,stdout); puts(""); );
	LOG( printf("%-13s :","signs"); TheCompressionIndicator((log_nsign - log_nsign_dpcm)/8,comp_sign_len,stdout); puts(""); );
#endif

	TheImageAnalyzer(raw,rawout,num_planes,width,height,((float)totsize/comp_tot_len),stdout);

#ifdef DO_LOG
	printf("n0 = %f %% , n1 = %f %%\n",(float)log_n0*100/(float)(totsize),(float)log_n1*100/(float)(totsize));
	printf("zero_len = %d , av = %f , smashed = %d, av gain = %f\n",log_zero_len,(float)log_zero_len/(totsize>>6),log_zero_len_smashed,(float)log_zero_len_gained/log_zero_len_smashed);
	printf("0p0 = %f %% , 0p1 = %f %% , 0pbig = %f %%\n",(float)log_0p0*100/(float)(log_0p0+log_0p1+log_0pbig),(float)log_0p1*100/(float)(log_0p0+log_0p1+log_0pbig),(float)log_0pbig*100/(float)(log_0p0+log_0p1+log_0pbig));
#ifdef CODE_SIGNS
	printf("nsign = %.4f %% , same = %.4f %% , diff = %.4f %% , noc  = %.4f %%\n",(float)log_nsign*100/(float)(totsize),(float)log_nsign_same*100/(float)(log_nsign),(float)log_nsign_diff*100/(float)(log_nsign),(float)log_nsign_noc *100/(float)(log_nsign) );
#endif

	if ( log_zero_len_smashed > 0 )
		printf("max RoD = %.4f , min RoD = %.4f\n",log_max_RoDs5/32.0,log_min_RoDs5/32.0);

	if ( ( rawF = fopen("out.raw","wb") ) == NULL )
		cleanup("open rawout failed");
	for(i=0;i<num_planes;i++) {
		if ( !FWriteOk(rawF,rawout[i],rawsize) )
			errputs("fwrote short! continuing..");
	}
	fclose(rawF); rawF = NULL;
#endif // DO_LOG

return 0;
}

typedef struct { int RoDs5,x,y,len,zlen,D; } smash_data;
int smash_data_cmp(const void *v1,const void *v2)
{
return( (((*(smash_data **)v1))->RoDs5) - (((*(smash_data **)v2))->RoDs5) );
}

int code_image(ubyte **raw,ubyte **rawout,int width,int height,int num_planes,
	int * qtable,int J_sought_s5,int max_distortion,
	int * comp_dpcm_len_ptr,int *comp_ac_len_ptr,int *comp_sign_len_ptr,int *comp_raw_len_ptr)
{
int coded_len,comp_dpcm_len,comp_ac_len,comp_sign_len,comp_raw_len;
int pnum;
bool smash_tail;

if ( J_sought_s5 > 999 || max_distortion < 10 ) smash_tail = false;
else smash_tail = true;

	init_coders();

	dct_image(raw,trans,width,height,num_planes,qtable);

#define TRANS_ZAG(z) (transline[zag_x[z] + x + zag_y[z]])
#define TRANS_ZAG_PREVB(z) (transline[zag_x[z] + x-DCTLINE + zag_y[z]])

#ifdef USE_EOBS
	/* look for zero-tail ; this is a transform, not coding */
if ( smash_tail ) {

	LOG(log_max_RoDs5=0; log_min_RoDs5=99999;);

	for(pnum=0;pnum<num_planes;pnum++) {
		uword *transline,*transplane;
		int x,y,z,val,i;
		int totR,totD;
		int nblocks,b;
		smash_data * sd;
		smash_data ** sdp;

		nblocks = (width*height)/DCTBLOCK;

		if ( (sd = malloc(nblocks*sizeof(smash_data))) == NULL )
			cleanup("malloc smash_data failed!");
		if ( (sdp = malloc(nblocks*sizeofpointer)) == NULL ) {
			free(sd); cleanup("malloc smash_data failed!");
		}
		for(b=0;b<nblocks;b++) {
			sdp[b] = &sd[b];
		}

		transplane = trans[pnum];

		b= -1;

		/** first count R/D for all blocks **/
		for(y=0;y<height;y += DCTLINE) {
			transline = transplane + y*width;
			for(x=0;x<width;x += DCTLINE) {
				b++;

				sd[b].x = x;
				sd[b].y = y;

				z = 63;
				while( z > 0 && TRANS_ZAG(z) == 0 ) {
					z--;
				} //zag(z) is not zero
				sd[b].zlen = (63-z);
				if ( z == 0 ) {
					sd[b].RoDs5 = 0;
					sd[b].len = 63;
					sd[b].D = 0;
					continue;
				}

				totD = 0; totR = 0;
				do {
					int gain,s,R,D;
					s = z;
					val = TRANS_ZAG(s);
					D = (val*val*qtable[zigzag[z]]*qtable[zigzag[z]])>>2;
					s--;
					while( s > 0 && TRANS_ZAG(s) == 0 ) s--;
					gain = (z-s);

					if ( val == 1 )	R = gain + 2;
					else R = gain + intlog2(val+1) + intlog2(intlog2(val+1)) + 1;

					/* J = R/D , J > J_quant ? */

					if ( R > ((J_sought_s5 * intsqrt(D))>>5) ) {
						z = s; // points at nonzero one
						totR += R;
						totD += D;
					} else {
						break;
					}
				} while( z > 0 );
				sd[b].len = (63-z);
				if ( sd[b].len == 0 || totD == 0 ) { 
					sd[b].D = 0; sd[b].RoDs5 = 0;
				} else {
					sd[b].D = totD;
					sd[b].RoDs5 = (totR<<5)/intsqrt(totD);
				}
			}
		}

		/** sort blocks Id's by R/D **/

		qsort(sdp,nblocks,sizeofpointer,smash_data_cmp);

		/** do smashing in order from top R/D **/
		totD = 0;
		for(b=nblocks-1;b>=0;b--) {
			if ( sdp[b]->len == 0 ) continue;

			y = sdp[b]->y; x = sdp[b]->x;
			transline = transplane + y*width;

			if ( totD < max_distortion && sdp[b]->D > 0 ) {
				LOG( if ( sdp[b]->RoDs5 > log_max_RoDs5 ) log_max_RoDs5 = sdp[b]->RoDs5; );
				LOG( if ( sdp[b]->RoDs5 < log_min_RoDs5 ) log_min_RoDs5 = sdp[b]->RoDs5; );

				z = 63;
				for(i=(sdp[b]->len);i--;) {
					LOG( if ( TRANS_ZAG(z) > 0 ) log_zero_len_smashed ++; );
					TRANS_ZAG(z) = TRANS_DONT_CODE;
					z--;
				}
#ifdef DEBUG
				if ( z < 0 || z > 62 ) cleanup("z out of range, you doof");
#endif
				TRANS_ZAG(z+1) = TRANS_EOB;
				totD += sdp[b]->D;
				LOG( log_zero_len += (sdp[b]->len) );
				LOG( log_zero_len_gained += (sdp[b]->len - sdp[b]->zlen); );
			} else if ( (i = (sdp[b]->zlen)) > 0 ) {
				for(z=63;i--;) {
					TRANS_ZAG(z) = TRANS_DONT_CODE;
					z--;
				}
				TRANS_ZAG(z+1) = TRANS_EOB;
				LOG(log_zero_len += (sdp[b]->zlen) );
			}
		}

#ifdef VERBOSE
		printf("max_distortion = %d , totd = %d\n",max_distortion,totD);
#endif

		free(sd);
		free(sdp);
	}
} else { /** don't smash tails **/
	uword *transline,*transplane;
	int x,y,z,val;

	for(pnum=0;pnum<num_planes;pnum++) {
		transplane = trans[pnum];
		for(y=0;y<height;y += DCTLINE) {
			transline = transplane + y*width;
			for(x=0;x<width;x += DCTLINE) {
				if ( TRANS_ZAG(63) > 0 ) continue;
				TRANS_ZAG(63) = TRANS_DONT_CODE;
				z = 62;
				while( z > 0 && TRANS_ZAG(z) == 0 ) {
					TRANS_ZAG(z) = TRANS_DONT_CODE;
					z--;
				}
				TRANS_ZAG(z+1) = TRANS_EOB;

				LOG(log_zero_len += (63-z) );
			}
		}
	}
}
#endif // USE_EOBS

#ifndef NO_CODING // *{*

	arithEncode(FAI,0,1,100); /** send the "quality" factor **/

	/* DPCM the 0ths */
	/*#*/ {
	uword *transline,*transplane,*prevline,sign,context;
	int x,y,pred,prev_sign,val;

	prev_sign = 2;

	for(pnum=0;pnum<num_planes;pnum++) {
		transplane = trans[pnum];
		for(y=0;y<height;y += DCTLINE) {
			prevline  = transline;
			transline = transplane + y*width;
			for(x=0;x<width;x += DCTLINE) {

				val = trans_data(transline[x]);

				if ( x == 0 && y == 0 ) {	pred = 0;
				} else if ( y == 0 ) {		pred = trans_data(transline[x-DCTLINE]);
				} else if ( x == 0 ) {		pred = trans_data(prevline[x]);
				} else { int a,b;
					a = trans_data(transline[x-DCTLINE]);
					b = trans_data(prevline[x]);
					pred = (a + b)>>1;
				}

				val -= pred;

				if ( val == 0 ) sign = 2;
				else if ( val < 0 ) { sign = 1; val = -val; }
				else sign = 0;

				/** warning : val can now be up to 2* order0_alphabet ! ;
					encode_m1 is the safest way to go. **/

#ifdef DPCM_ORDER_M1
				encode_m1_custom(val);
#else
#ifdef DPCM_ORDER_0
				if ( ! contextEncode(order0,val) )
					encode_m1_custom(val);
#else
				encode(val,(CODE_CONTEXTS-1));
					// using context=0 here hurts a huge amount!
#endif
#endif //orders

				if ( val > 0 ) encodeSign(sign,prev_sign);

#ifdef CODE_SIGNS
				prev_sign = sign;
#endif
			}
		}
	}
	/*#*/ }

	comp_dpcm_len = BitIO_GetPos(BII) + BitIO_GetPos(rawBII) + BitIO_GetPos(signBII);
	BitIO_ResetArray(BII,comp); BitIO_ResetArray(rawBII,rawcomp); BitIO_ResetArray(signBII,signcomp);
	LOG( log_nsign_dpcm = log_nsign;);

	/* encode the frequencies (AC) */
	/*#*/ {
	uword *transline,*transplane,val,sign,context;
	int prev_val,prev_sign,prev;
	int x,y,pnum,i,t;

	for(i=1;i<DCTBLOCK;i++) {
		for(pnum=0;pnum<num_planes;pnum++) {
			transplane = trans[pnum];
			for(y=0;y<height;y += DCTLINE) {
				transline = transplane + y*width;
				for(x=0;x<width;x += DCTLINE) {

					/** we scan through all spacial locations of
						a certain frequency (so arithcoders adapt)
						but use the previous zigzag as context **/

					if ( (t = TRANS_ZAG(i)) == TRANS_DONT_CODE )
						continue;

					prev = TRANS_ZAG(i-1);

					sign = t & 1; val = (t>>1) + sign;
					if ( val == 0 || t == TRANS_EOB ) sign = 2;

					prev_sign = prev & 1; prev_val = (prev>>1) + prev_sign;
					if ( prev_val == 0 ) prev_sign = 2;

				LOG( if ( val == 0 ) log_n0 ++; else if ( val == 1 ) log_n1 ++; );
				LOG( if ( prev_val == 0 ) { if (val == 0) log_0p0 ++; else if (val==1) log_0p1++; else log_0pbig++; } );

					for(context=0;;context++) //** could use direct lookup table
						if ( prev_val <= context_buckets[context] )	
							break;

					encode(val,context);
					if ( sign != 2 ) encodeSign(sign,prev_sign);
				}
			}
		}
	}
	/*#*/ }

#endif // *}* ALL CODING

	idct_image(rawout,trans,width,height,num_planes,qtable);

	coded_len = done_coders() + comp_dpcm_len;

	comp_ac_len = BitIO_GetPos(BII) - 2;
	comp_sign_len = BitIO_GetPos(signBII) - 2;
	comp_raw_len = BitIO_GetPos(rawBII) - 2;

	if ( comp_dpcm_len_ptr ) *comp_dpcm_len_ptr = comp_dpcm_len;
	if ( comp_ac_len_ptr ) *comp_ac_len_ptr = comp_ac_len;
	if ( comp_sign_len_ptr ) *comp_sign_len_ptr = comp_sign_len;
	if ( comp_raw_len_ptr ) *comp_raw_len_ptr = comp_raw_len;

	free_coders();

return coded_len;
}

void encode(int sym,int context)
{

#ifdef DEBUG
if ( sym >= ORDER0_ALPHABET ) dbf();
if ( context >= CODE_CONTEXTS ) dbf();
#endif

	if ( order1[context] == NULL )
		if ( (order1[context] = scontextCreate(FAI,ORDER1_ALPHABET,0,
				ORDER1_TOTMAX,ORDER1_INC,true)) == NULL )
			cleanup("context creation failed!");

#ifdef USE_EOBS
	if ( sym == VAL_EOB ) {
		scontextEncode(order1[context],ORDER1_EOB);
	} else 
#endif
		if ( sym < ORDER1_ESCAPE ) {
		scontextEncode(order1[context],sym);
	} else {
		scontextEncode(order1[context],ORDER1_ESCAPE);
		sym -= ORDER1_ESCAPE;

		if ( ! contextEncode(order0,sym) ) {
#ifdef FLAT_ORDER_M1
			encode_m1_flat(sym);
#else
#ifdef JPEG_ORDER_M1
			encode_m1_jpeg(sym);
#else
			encode_m1_custom(sym);
#endif
#endif
			LOG(log_nm1++);
		}
	}

}

void encode_m1_flat(int sym) // order -1
{
	while( sym >= FLAT_ORDER_M1_ESCAPE ) {
		arithEncode(rawFAI,FLAT_ORDER_M1_ESCAPE,FLAT_ORDER_M1_ESCAPE+1,FLAT_ORDER_M1_ESCAPE+1);
		sym -= FLAT_ORDER_M1_ESCAPE;
	}
arithEncode(rawFAI,sym,sym+1,(FLAT_ORDER_M1_ESCAPE+1));
}

void encode_m1_jpeg(int sym) // order -1
{
int topbit,top;

	topbit = 0;
	top = sym;
	while(top)
		{ topbit++; top>>=1; }

	if ( topbit == 0 ) {
		BitIO_WriteZeroBit(rawBII);
		BitIO_WriteZeroBit(rawBII);
	} else {
		top = 1<<(topbit-1);
		sym -= top;
		if ( topbit < 3 ) {
			BitIO_WriteZeroBit(rawBII);
			BitIO_WriteBit(rawBII,1);
			BitIO_WriteBit(rawBII,topbit&1);
		} else {
			BitIO_WriteBit(rawBII,1);
			topbit -= 3;
			if ( topbit >= 7 ) {
				BitIO_WriteBit(rawBII,1);
				BitIO_WriteBit(rawBII,1);
				BitIO_WriteBit(rawBII,1);
				topbit -= 7;
				while(topbit) {	BitIO_WriteBit(rawBII,1); topbit--; }
				BitIO_WriteZeroBit(rawBII);
			} else {
				BitIO_WriteBit(rawBII,(topbit&4));
				BitIO_WriteBit(rawBII,(topbit&2));
				BitIO_WriteBit(rawBII,(topbit&1));
			}
		}
#ifdef DEBUG
		if ( sym >= top ) cleanup(" sym >= top , you blew it ");
#endif
		arithEncode(rawFAI,sym,sym+1,top);
	}
}


void encode_m1_custom(int sym) // order -1
{
	if ( sym < M1_THRESH_1 ) {
		BitIO_WriteZeroBit(rawBII);
		arithEncode(rawFAI,sym,sym+1,M1_THRESH_1);
	} else {
		BitIO_WriteBit(rawBII,1); sym -= M1_THRESH_1;
		if ( sym < M1_THRESH_2 ) {
			BitIO_WriteZeroBit(rawBII);
			arithEncode(rawFAI,sym,sym+1,M1_THRESH_2);
		} else {
			BitIO_WriteBit(rawBII,1); sym -= M1_THRESH_2;
			if ( sym < M1_THRESH_3 ) {
				BitIO_WriteZeroBit(rawBII);
				arithEncode(rawFAI,sym,sym+1,M1_THRESH_3);
			} else {
				BitIO_WriteBit(rawBII,1); sym -= M1_THRESH_3;
				while( sym >= (M1_THRESH_4) ) {
					arithEncode(rawFAI,M1_THRESH_4,M1_THRESH_4+1,M1_THRESH_4+1);
					sym -= (M1_THRESH_4);
				}
				arithEncode(rawFAI,sym,sym+1,M1_THRESH_4+1);
			}
		}
	}
}

void encodeSign(bool sign,int parent)
{
LOG( log_nsign++; );
#ifdef CODE_SIGNS
LOG( if ( parent == 2 ) log_nsign_noc++; else if ( parent == sign ) log_nsign_same++; else log_nsign_diff++; );
	
	scontextEncode(signorder0[parent],sign);
#else
	BitIO_WriteBit(signBII,sign);
#endif
}

void ExitFunc(void)
{
int i;

free_coders();

smartfree(comp);
smartfree(signcomp);
smartfree(rawcomp);

if ( raw ) {
	for(i=0;i<num_planes;i++) {
		smartfree(raw[i]);
	}
	free(raw);
}
if ( rawout ) {
	for(i=0;i<num_planes;i++) {
		smartfree(rawout[i]);
	}
	free(rawout);
}
if ( trans ) {
	for(i=0;i<num_planes;i++) {
		smartfree(trans[i]);
	}
	free(trans);
}


if ( rawF ) fclose(rawF);

}


void TheImageAnalyzer(ubyte **original,ubyte **im2,
					int num_planes,int width,int height,
					float ratio,FILE *sio)
{
int diffs[256];
int diff,i,tot,totsq,max,pnum,j;
int rawsize,totsize;
float mse,me,rmse,mse_percep;

rawsize = width*height;
totsize = width*height*num_planes;

	MemClear(diffs,256*sizeof(int));

	for(pnum=0;pnum<num_planes;pnum++) {
		ubyte *rptr,*vptr;
		rptr = original[pnum]; vptr = im2[pnum];
		for(i=rawsize;i--;) {
			diff = *rptr++ - *vptr++;
			if ( diff < 0 ) diff = -diff;
			diffs[diff] ++;
		}
	}

	tot = totsq = max = 0;
	for(i=1;i<256;i++) {
		if ( diffs[i] > 0 ) {
			max = i;
			tot += i * diffs[i];
			totsq += i*i * diffs[i];
		}
	}

	me = (float)tot/totsize;
	mse = (float)totsq/totsize;
	rmse = sqrt(mse);

	for(pnum=0;pnum<num_planes;pnum++) {
		int x,y,av1,av2;
		ulong totds;
		ubyte *line1,*pline1,*nline1;
		ubyte *line2,*pline2,*nline2;
		line1 = original[pnum]; nline1 = line1+width;
		line2 = im2[pnum]; nline2 = line2+width;
		totds = 0;
		for(y=1;y<(width-1);y++) {
			pline1 = line1; line1 = nline1; nline1 += width;
			pline2 = line2; line2 = nline2; nline2 += width;
			for(x=1;x<(width-1);x++) {
				av1 = line1[x] + line1[x-1] + line1[x+1] + pline1[x] + nline1[x];
				av2 = line2[x] + line2[x-1] + line2[x+1] + pline2[x] + nline2[x];
				diff = (av1 - av2); diff *= diff;
				if ( abs( line1[x-1] - line1[x+1] ) < 5 &&
 					 abs( pline1[x] - nline1[x] ) < 5 ) totds += diff + diff;
				else totds += diff;
			}
		}

		mse_percep = (float)totds/(25.0*totsize);
	}

#ifdef VERBOSE
	fprintf(sio,"error: av= %.2f,max= %d,mse= %.3f,rmse= %.2f,psnr= %.2f,perc= %.2f\n",me,max,mse,rmse,(PSNR_MAX - 10*log10(mse)),mse_percep);
	fprintf(sio,"performance, MSE = %f , RMSE = %f , percep = %f\n",(ratio/mse),(ratio/rmse),(ratio/mse_percep));
		/** use MSE in high error regime, RMS in low error **/
#else
	fprintf(sio,"RMSE = %f ,PSNR = %f, performanse(rmse) = %f\n",rmse,(PSNR_MAX - 10*log10(mse)),(ratio/rmse));;
#endif

}

void dct_image(ubyte **raw,uword **trans,int width,int height,int num_planes,
		int * quant_table)
{
ubyte *plane,*line,*lptr;
uword *transline,*transplane,*tptr;
RAWDATA rawblock[DCTBLOCK],*rptr;
DCTDATA dctblock[DCTBLOCK],*bptr;
int x,y,pnum,i;

dct_init(quant_table);

	for(pnum=0;pnum<num_planes;pnum++) {
		plane = raw[pnum];
		transplane = trans[pnum];
		for(y=0;y<height;y += DCTLINE) {
			line = plane + y*width;
			transline = transplane + y*width;
			for(x=0;x<width;x += DCTLINE) {

				rptr = rawblock;
				for(i=0;i<DCTLINE;i++) {
					lptr = line + x + i*width;
					unroll_dctline(*rptr++ = *lptr++);
				}
	
				dct(rawblock,dctblock);

				bptr = dctblock;
				for(i=0;i<DCTLINE;i++) {
					tptr = transline + x + i*width;
					unroll_dctline(*tptr++ = data_trans(*bptr); bptr++);
				}
			}
		}
	}


}


void idct_image(ubyte **rawout,uword **trans,int width,int height,int num_planes,
		int * quant_table)
{
ubyte *plane,*line,*lptr;
uword *transline,*transplane,*tptr;
RAWDATA rawblock[DCTBLOCK],*rptr;
DCTDATA dctblock[DCTBLOCK],*bptr;
int x,y,pnum,i;

	idct_init(quant_table);

	for(pnum=0;pnum<num_planes;pnum++) {
		plane = rawout[pnum];
		transplane = trans[pnum];
		for(y=0;y<height;y += DCTLINE) {
			line = plane + y*width;
			transline = transplane + y*width;
			for(x=0;x<width;x += DCTLINE) {

				bptr = dctblock;
				for(i=0;i<DCTLINE;i++) {
					tptr = transline + x + i*width;
					unroll_dctline(*bptr++ = trans_data(*tptr); tptr++;);
				}

				idct(dctblock,rawblock);

				rptr = rawblock;
				for(i=0;i<DCTLINE;i++) {
					lptr = line + x + i*width;
					unroll_dctline(*lptr++ = *rptr++);
				}
	
			}
		}
	}

}

void init_allocs(int num_planes,int rawsize,int complen)
{
int i;
	if ( (raw = malloc(sizeofpointer*num_planes)) == NULL )
		cleanup("malloc failed");
	if ( (rawout = malloc(sizeofpointer*num_planes)) == NULL )
		cleanup("malloc failed");
	if ( (trans = malloc(sizeofpointer*num_planes)) == NULL )
		cleanup("malloc failed");

	for(i=0;i<num_planes;i++) {
		if ( (raw[i] = malloc(rawsize)) == NULL )
			cleanup("malloc failed");
		if ( (rawout[i] = malloc(rawsize)) == NULL )
			cleanup("malloc failed");
		if ( (trans[i] = malloc(sizeof(uword)*rawsize)) == NULL )
			cleanup("malloc failed");
	}

	if ( (comp = malloc(complen + 1024)) == NULL )
		cleanup("malloc failed");
	if ( (signcomp = malloc(complen + 1024)) == NULL )
		cleanup("malloc failed");
	if ( (rawcomp = malloc(complen + 1024)) == NULL )
		cleanup("malloc failed");
}

void init_coders(void)
{
int i;
	if ( (BII = BitIO_Init(comp)) == NULL )
		cleanup("BitIOInit failed!");
	if ( (FAI = FastArithInit(BII)) == NULL )
		cleanup("FastArithInit failed!");

	if ( (signBII = BitIO_Init(signcomp)) == NULL )
		cleanup("BitIOInit failed!");
	if ( (signFAI = FastArithInit(signBII)) == NULL )
		cleanup("FastArithInit failed!");

	if ( (rawBII = BitIO_Init(rawcomp)) == NULL )
		cleanup("BitIOInit failed!");
	if ( (rawFAI = FastArithInit(rawBII)) == NULL )
		cleanup("FastArithInit failed!");

	FastArithEncodeCInit(FAI);
	FastArithEncodeCInit(signFAI);
	FastArithEncodeCInit(rawFAI);

	if ( (order0 = contextCreateMax(FAI,ORDER0_ALPHABET,ORDER0_TOTMAX)) == NULL )
		cleanup("Order0_Init failed!");

	if ( (order1 = AllocMem(CODE_CONTEXTS*sizeofpointer,MEMF_CLEAR)) == NULL )
		cleanup("Order1_Init failed!");

#ifdef CODE_SIGNS
	if ( (signorder0 = AllocMem(SIGN_CONTEXTS*sizeofpointer,MEMF_CLEAR)) == NULL )
		cleanup("Order1_Init failed!");
	for(i=0;i<SIGN_CONTEXTS;i++) {
		if ( (signorder0[i] = scontextCreate(signFAI,2,
				0,SIGNORDER0_TOTMAX,SIGNORDER0_INC, true)) == NULL )
			cleanup("Order0_Init failed!");
	}
#endif
}

int done_coders(void)
{
int complen;
	FastArithEncodeCDone(FAI);
	FastArithEncodeCDone(signFAI);
	FastArithEncodeCDone(rawFAI);
	complen = 0;
	complen += BitIO_FlushWrite(BII) - 2;
	complen += BitIO_FlushWrite(signBII) - 2;
	complen += BitIO_FlushWrite(rawBII) - 2;
return complen;
}

void free_coders(void)
{
int i;

if ( order1 ) {
	for(i=0;i<CODE_CONTEXTS;i++)
		if ( order1[i] ) scontextFree(order1[i]);
	free(order1);
	order1 = NULL;
}
#ifdef CODE_SIGNS
if ( signorder0 ) {
	for(i=0;i<SIGN_CONTEXTS;i++)
		if ( signorder0[i] ) scontextFree(signorder0[i]);
	free(signorder0);
	signorder0 = NULL;
}
#endif
if ( order0 ) contextFree(order0); order0 = NULL;

if ( FAI ) FastArithCleanUp(FAI); FAI = NULL;
if ( BII ) BitIO_CleanUp(BII); BII = NULL;
if ( signFAI ) FastArithCleanUp(signFAI); signFAI = NULL;
if ( signBII ) BitIO_CleanUp(signBII); signBII = NULL;
if ( rawFAI ) FastArithCleanUp(rawFAI); rawFAI = NULL;
if ( rawBII ) BitIO_CleanUp(rawBII); rawBII = NULL;

}
