#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <getopt.h>

#include <common.h>
#include <bmp.h>
#include <smoothing/mean_smooth.h>
#include <smoothing/median_smooth.h>
#include <interface/screen.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <thresholding/threshold.h>
#include <morphology/binary_morph.h>
#include <labeling/labeling.h>
#include <statistics/pixmap_statistic.h>

// This is for .../Data/try11/*.bmp

/////////////////////////////////////////
#define UP_DX -11
#define UP_DY -46

/////////////////////////////////////////
#define MID_DX 0
#define MID_DY 0

/////////////////////////////////////////
#define DN_DX 0
#define DN_DY 0

/////////////////////////////////////////
#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

//#undef wait_keyhit()
//#define wait_keyhit()

static const char *program_name = NULL;
static char *path;
static int verbose = 0;

static const struct option long_options[] = {
  {"help", 0, NULL, 'h'},
  {"verbose", 0, NULL, 'v'},
  {"name", 1, NULL, 'n'},
  {"dx", 1, NULL, 'x'},
  {"dy", 1, NULL, 'y'},
  {NULL, 0, NULL, 0}
};

static const char *const short_options = "hvn:";

static void print_usage(FILE *stream, int exit_code)
{
  fprintf(stream, "Usage : %s [ ... ]\n", program_name);
  fprintf(stream, "-h,           --help    Display this usage information.\n");
  fprintf(stream, "-v,           --verbose Print verbose message.\n");
  fprintf(stream, "-n [image],   --name    Set input file.\n");
  fprintf(stream, "-x [xshift],  --dx      Set the displacement of x.\n");
  fprintf(stream, "-y [yshift],  --dy      Set the displacement of y.\n");
  exit(exit_code);
}

int tricam_move(int argc, char *argv[])
{
  int i, j, w, h, bpp;
  bytemap_t *image;
  bytemap_t *gray;

  char *fn, *ptr;
  real_t value;

  FILE *fd;

  char *path;
  int next_option;
  int dx, dy;

  program_name = argv[0];
  do {
    next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option) {
    case 'h': print_usage(stdout, 0); break;
    case 'x': dx = strtod(optarg, NULL); break;
    case 'y': dy = strtod(optarg, NULL); break;
    case 'v': verbose = 1; break;
    case 'n': path = strdup(optarg); break;
    case '?': print_usage(stderr, 1); break;
    case -1: break;
    default: abort();
    }
  } while (next_option != -1);

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);
  
  up_image = bytemap_new(w, h);
  mid_image = bytemap_new(w, h);
  dn_image = bytemap_new(w, h);

  gray = bytemap_new(w, h);

  // Up display image ////////////////////
  strcpy(path, argv[1]);
  fn = strrchr(path, '/') + 1;
  printf("Filename: %s\n", fn);
  printf("Display image\n");
  load_and_display_BMP(path);
  read_bytemap_in_screen(up_image, NULL, NULL);
  bytemap_clear(gray);
  bytemap_copy(gray, UP_DX, UP_DY, up_image, 0, 0, w, h);
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  printf("Mean filtering \n");
  mean_smooth(up_image, gray, 3);
  write_bytemap_to_screen(up_image, up_image, up_image);
  wait_keyhit();

  strcpy(path, argv[1]);
  strcat(path, "normal.bmp");
  save_bytemap_as_gray_BMP(up_image, path);
  wait_keyhit();
  //////////////////////////////////////////

  bytemap_destroy(gray);
  bytemap_destroy(image);

  return 0;
}
