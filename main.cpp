#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#include <examples/examples.h>

int main(int argc, char *argv[])
{
  //image_downsize(argc, argv);
  //carrot_process(argc, argv);
  //tricam_process(argc, argv);
  //tricam_diff_div(argc, argv);
  //tricam_apple_sub_div(argc, argv);
  //tricam_apple_sub(argc, argv);
  //tricam_diff(argc, argv);
  //tricam_morphology(argc, argv);
  //tricam_labeling(argc, argv);
  //tricam_display(argc, argv);
  //tricam_citrus(argc, argv);
  //matrix_test();
  FLIR_PM595_viewer(argc, argv);

  /*
  FILE *fd;
  int i, j;
  double d;

  fd = fopen("log.txt", "w");
  for (i = 0; i <= 2550000; i += 10000) {
    for (j = 1; j <= 256; j++) {
      fprintf(fd, "%lf, ", (double)i / (double)j);
    }
    fprintf(fd, "\n");
  }
  fclose(fd);
  */

  return 0;
}
