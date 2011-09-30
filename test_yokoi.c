#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define calculate_yokoi() { \
	for (num = 0, i = 1; i <= 7; i += 2) { \
		i1 = i+1; if (i1 > 8) i1 -= 8; \
		i2 = i+2; if (i2 > 8) i2 -= 8; \
		num += (values[i] == 0) - (values[i] == 0) * (values[i1] == 0) * (values[i2] == 0); \
	} \
}
	
int main(int argc, char *argv[])
{
	int i, i1, i2, j, values[9], num;

	// 0
	values[4] = 0, values[3] = 0, values[2] = 0;
	values[5] = 0, values[0] = 1, values[1] = 0;
	values[6] = 0, values[7] = 0, values[8] = 0;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	// 1
	values[4] = 1, values[3] = 0, values[2] = 1;
	values[5] = 1, values[0] = 1, values[1] = 1;
	values[6] = 1, values[7] = 1, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 0, values[3] = 1, values[2] = 1;
	values[5] = 1, values[0] = 1, values[1] = 1;
	values[6] = 1, values[7] = 1, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 1, values[3] = 1, values[2] = 1;
	values[5] = 0, values[0] = 1, values[1] = 1;
	values[6] = 1, values[7] = 1, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 1, values[3] = 1, values[2] = 1;
	values[5] = 1, values[0] = 1, values[1] = 1;
	values[6] = 0, values[7] = 1, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 1, values[3] = 1, values[2] = 1;
	values[5] = 1, values[0] = 1, values[1] = 1;
	values[6] = 1, values[7] = 0, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 1, values[3] = 1, values[2] = 1;
	values[5] = 1, values[0] = 1, values[1] = 1;
	values[6] = 1, values[7] = 1, values[8] = 0;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 1, values[3] = 1, values[2] = 1;
	values[5] = 1, values[0] = 1, values[1] = 0;
	values[6] = 1, values[7] = 1, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 1, values[3] = 1, values[2] = 0;
	values[5] = 1, values[0] = 1, values[1] = 1;
	values[6] = 1, values[7] = 1, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	// 2
	values[4] = 1, values[3] = 0, values[2] = 1;
	values[5] = 1, values[0] = 1, values[1] = 1;
	values[6] = 1, values[7] = 0, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 0, values[3] = 1, values[2] = 1;
	values[5] = 1, values[0] = 1, values[1] = 1;
	values[6] = 1, values[7] = 1, values[8] = 0;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 1, values[3] = 1, values[2] = 1;
	values[5] = 0, values[0] = 1, values[1] = 0;
	values[6] = 1, values[7] = 1, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 1, values[3] = 1, values[2] = 0;
	values[5] = 1, values[0] = 1, values[1] = 1;
	values[6] = 0, values[7] = 1, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	// 3
	values[4] = 1, values[3] = 0, values[2] = 1;
	values[5] = 0, values[0] = 1, values[1] = 0;
	values[6] = 0, values[7] = 1, values[8] = 0;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 1, values[3] = 0, values[2] = 0;
	values[5] = 0, values[0] = 1, values[1] = 1;
	values[6] = 1, values[7] = 0, values[8] = 0;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 0, values[3] = 1, values[2] = 0;
	values[5] = 0, values[0] = 1, values[1] = 0;
	values[6] = 1, values[7] = 0, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 0, values[3] = 0, values[2] = 1;
	values[5] = 1, values[0] = 1, values[1] = 0;
	values[6] = 0, values[7] = 0, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	// 4
	values[4] = 1, values[3] = 0, values[2] = 1;
	values[5] = 0, values[0] = 1, values[1] = 0;
	values[6] = 1, values[7] = 0, values[8] = 1;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

	values[4] = 0, values[3] = 1, values[2] = 0;
	values[5] = 1, values[0] = 1, values[1] = 1;
	values[6] = 0, values[7] = 1, values[8] = 0;
	printf("%d %d %d\n", values[4], values[3], values[2]);
	printf("%d %d %d\n", values[5], values[0], values[1]);
	printf("%d %d %d\n", values[6], values[7], values[8]);
	calculate_yokoi();
	printf("yokoi number:%d\n", num);

/*
	srand((unsigned)time(NULL));
	for (j = 0; j < 20; j++) {
		for (i = 0; i < 9; i++) {
			values[i] = rand()%2;
		}
		values[0] = 1;
		printf("%d %d %d\n", values[4], values[3], values[2]);
		printf("%d %d %d\n", values[5], values[0], values[1]);
		printf("%d %d %d\n", values[6], values[7], values[8]);
		for (num = 0, i = 1; i <= 7; i += 2) {
			num += values[i] - values[i] * values[(i+1)%9] * values[(i+2)%9];
		}
		printf("yokoi number:%d\n", num);
	}
*/
	return 0;
}

