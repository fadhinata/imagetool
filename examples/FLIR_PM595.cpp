#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <SDL/SDL.h>
#include <getopt.h>

#include <common.h>
#include <bmp.h>
#include <smoothing/mean_smooth.h>
#include <interface/screen.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <pixmap/wordmap.h>
#include <thresholding/threshold.h>
#include <morphology/binary_morph.h>
#include <labeling/labeling.h>
#include <statistics/pixmap_statistic.h>
#include <convert/convert.h>

#define WIDTH 326
#define HEIGHT 244
#define BPP 32

static const char *program_name = NULL;
static char *file_name = NULL;
static int verbose = 0;

static const char *const short_options = "ho:s:f:t:vn:";

static const struct option long_options[] = {
  {"help", 0, NULL, 'h'},
  {"operand", 1, NULL, 'o'},
  {"subtract", 1, NULL, 's'},
  {"from", 1, NULL, 'f'},
  {"to", 1, NULL, 't'},
  {"verbose", 0, NULL, 'v'},
  {"name", 1, NULL, 'n'},
  {NULL, 0, NULL, 0}
};

static void print_usage(FILE *stream, int exit_code)
{
  fprintf(stream, "Usage : %s [ ... ]\n", program_name);
  fprintf(stream, "-h,           --help    Display this usage information.\n");
  fprintf(stream, "-o,           --operand Set operand for subtracting.\n");
  fprintf(stream, "-s,           --sub     Set subtract for subtracting.\n");
  fprintf(stream, "-f [Cutup],   --from    Set Cutup value to threshold.\n");
  fprintf(stream, "-t [Cutdown], --to      Set Cutdown value to threhold.\n");
  fprintf(stream, "-v,           --verbose Print verbose message.\n");
  fprintf(stream, "-n [image],   --name    Set the path of input file.\n");

  exit(exit_code);
}

int FLIR_PM595_viewer(int argc, char *argv[])
{
  int next_option;
  FILE *f;
  int file_size, header_size, image_size;
  char buffer[256];

  program_name = argv[0];
  do {
    next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option) {
    case 'h': print_usage(stdout, 0); break;
      /*
	case 'o': token1 = strdup(optarg); break;
	case 's': token2 = strdup(optarg); break;
	case 'f': cutup = strtod(optarg, NULL); break;
	case 't': cutdown = strtod(optarg, NULL); break;
	case 'v': verbose = 1; break;

	case '?': print_usage(stderr, 1); break;
      */
    case 'n': file_name = strdup(optarg); break;
    case -1: break;
    default: abort();
    }
  } while (next_option != -1);

  if (!file_name) {
    print_usage(stderr, -1);
    return -1;
  }
  
  if (!(f = fopen(file_name, "rb"))) {
    printf("Invalid file name !\n");
    return -1;
  }

  fseek(f, 0, SEEK_END);
  file_size = ftell(f);
  image_size = 2 * (326 * 1) * (244 + 1);
  header_size = file_size - image_size;

  printf("file size: %d bytes\n", file_size);
  printf("image size: %d bytes\n", image_size);
  printf("header size: %d bytes\n", header_size);

  rewind(f);
  while (header_size > 0) {
    fgets(buffer, 80, f);
    printf("%d: %s\n", strlen(buffer), buffer);
    header_size -= 80;
  }
  
  fclose(f);

  //printf("%s - %s, %lf, %lf, %s\n", token1, token2, cutup, cutdown, path);

  initialize_screen(WIDTH, HEIGHT, BPP);
  
  wait_keyhit();

  deinitialize_screen();
  return 0;
}


