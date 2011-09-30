#include <stdio.h>
#include <math.h>

void test_type_cast(void)
{
	int i;
	short val;
	unsigned char v[8];
	for (i = 0; i < 8; i++) v[i] = 255;
	val = 5*v[0]+5*v[1]+5*v[2]+5*v[3]+5*v[4]+5*v[5]+5*v[6]+5*v[7];
	printf("%d", val);
}

void make_table_sin_of_acos(void)
{
	int i;
	FILE *fd = fopen("sin_acos.tbl", "wb");
	fprintf(fd, "const signed long SIN_OF_ACOS[1024] = {");
	for (i = 0; i < 1024; i++) {                /* create the sin(arccos(x)) table. */
		if ((i%32) == 0) fprintf(fd, "\n\t");
		fprintf(fd, "%d, ", sin(acos((float)i/1024))*0x10000L);
	}
	fprintf(fd, "\n};\n");
	fclose(fd);
}

void eps_test(void)
{
    double machEps = 1.0;
 
    printf( "current Epsilon, 1 + current Epsilon\n" );
    do {
       printf( "%G\t%.20f\n", machEps, (1.0 + machEps) );
       machEps /= 2.0f;
       // If next epsilon yields 1, then break, because current
       // epsilon is the machine epsilon.
    }
    while ((double)(1.0 + (machEps/2.0)) != 1.0);
 
    printf( "\nCalculated Machine epsilon: %G\n", machEps );
}

int main( int argc, char **argv )
{
	//make_table_sin_of_acos();
	test_type_cast();
	return 0;
}
