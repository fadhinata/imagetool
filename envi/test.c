#include <stdio.h>

int main(int argc, char *argv[])
{
	FILE *fd;
	fd = fopen("test.txt", "wb");
	fputs("ENVI\n", fd);
	fputs("description = {\n", fd);
	fclose(fd);
	return 0;
}

