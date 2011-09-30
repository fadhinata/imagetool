#include <stdio.h>

static unsigned char image[9];
// 0 : isolated, 1: edge, 2: connecting, 3: branching, 4: crossing, 5: interior
static inline int yokoi_number(int x, int y, unsigned char *im)
{
	int nbr[9];
	int i, k, i1, i2;

	nbr[0] = im[y*3+x];
	nbr[1] = im[y*3+(x+1)];
	nbr[2] = im[(y-1)*3+(x+1)];
	nbr[3] = im[(y-1)*3+x];
	nbr[4] = im[(y-1)*3+(x-1)];
	nbr[5] = im[y*3+(x-1)];
	nbr[6] = im[(y+1)*3+(x-1)];
	nbr[7] = im[(y+1)*3+x];
	nbr[8] = im[(y+1)*3+(x+1)];

	k = 0;
	for (i = 1; i <= 7; i += 2) {
		i1 = i+1; if (i1 > 8) i1 -= 8;
		i2 = i+2; if (i2 > 8) i2 -= 8;
		k += (nbr[i]==0) - (nbr[i]==0)*(nbr[i1]==0)*(nbr[i2]==0);
	}

	return k;
}

#define N 16

void main(void)
{
    int i, j, t, k, yokio, num;
    int n, i2;
    int a[N];

    srand(time(NULL));

    n = N;
    for (i = 0; i < n; i++) {
        a[i] = i;
    }



    i2 = n >> 1;
    j = 0;
    for (i=0;i<n-1;i++) {
      if (i < j) {
         t = a[i];
         a[i] = a[j];
         a[j] = t;
      }
            printf("i(%d) j(%d) k(%d)\n", i, j, k);
      k = i2;
      while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
      //printf("i(%d) j(%d) k(%d)\n", i, j, k);
   }
   for (i = 0; i < n; i++) printf("%d ", a[i]);
    /*
    for (i = 0; i < 9; i++) {
        num = rand()&1;
        image[i] = num;
    }
    image[4] = 0;
    */
#if 0
    image[0] = 1, image[1] = 0, image[2] = 1;
    image[3] = 0, image[4] = 1, image[5] = 0;
    image[6] = 1, image[7] = 0, image[8] = 1;

    yokio = yokoi_number(1, 1, image);

    printf("%d %d %d\n", image[0], image[1], image[2]);
    printf("%d %d %d\n", image[3], image[4], image[5]);
    printf("%d %d %d\n", image[6], image[7], image[8]);
    printf("yokio_number %d\n", yokio);
#endif
}
