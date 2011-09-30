#include <stdio.h>

int value_3x3[256][9+2];

int main(int argc, char *argv[])
{
	int i, j, value;
	int crossing = 0, count = 0;

	for (i = 0; i < 256; i++) {
		value_3x3[i][0] = '*';
		for (j = 0; j < 8; j++) {
			if (i & (1 << j)) value_3x3[i][j+1] = '*';
			else value_3x3[i][j+1] = '=';
		}
		crossing = 0;
		count = 0;
		value = value_3x3[i][8];
		for (j = 1; j <= 8; j++) {
			if (value_3x3[i][j] != value) crossing++;
			if (value_3x3[i][j] == value_3x3[i][0]) count++;
			value = value_3x3[i][j];
		}
		value_3x3[i][9] = crossing;
		value_3x3[i][10] = count;


	}
	for (crossing = 0; crossing <= 8; crossing += 2) {
		for (count = 0; count <= 8; count++) {
			for (i = 0; i < 256; i++) {
				if (value_3x3[i][9] == crossing && value_3x3[i][10] == count) {
					printf("%c%c%c\n", value_3x3[i][4], value_3x3[i][3], value_3x3[i][2]);
					printf("%c%c%c %d %d\n", value_3x3[i][5], value_3x3[i][0], value_3x3[i][1], value_3x3[i][9], value_3x3[i][10]);
					printf("%c%c%c\n", value_3x3[i][6], value_3x3[i][7], value_3x3[i][8]);
					printf("\n");
				}
			}
		}
	}
	return 0;
}

