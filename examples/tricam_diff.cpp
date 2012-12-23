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

#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

//#undef wait_keyhit()
//#define wait_keyhit()

static const char *program_name = NULL;
static char *path;
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

// imagetool -o up(740nm) -s mid(940nm) -f -30 -t -15 -n ../Data/try13/0
int tricam_diff(int argc, char *argv[])
{
  int x, y, i, j, w, h, bpp, mode = 0, save = 0;
  real_t value, cutup = -255, cutdown = 255;
  bitmap_t *result;
  bytemap_t *up_image, *mid_image, *dn_image;
  bytemap_t *gray, *op1, *op2;
  bytemap_t *red, *green, *blue;
  char file_name[256], *fn, *ptr;
  FILE *fd = NULL;
  int next_option;
  char *token1, *token2;

  program_name = argv[0];
  do {
    next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option) {
    case 'h': print_usage(stdout, 0); break;
    case 'o': token1 = strdup(optarg); break;
    case 's': token2 = strdup(optarg); break;
    case 'f': cutup = strtod(optarg, NULL); break;
    case 't': cutdown = strtod(optarg, NULL); break;
    case 'v': verbose = 1; break;
    case 'n': path = strdup(optarg); break;
    case '?': print_usage(stderr, 1); break;
    case -1: break;
    default: abort();
    }
  } while (next_option != -1);

  printf("%s - %s, %lf, %lf, %s\n", token1, token2, cutup, cutdown, path);

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);
  
  up_image = bytemap_new(w, h);
  mid_image = bytemap_new(w, h);
  dn_image = bytemap_new(w, h);

  gray = bytemap_new(w, h);
  red = bytemap_new(w, h);
  green = bytemap_new(w, h);
  blue = bytemap_new(w, h);

  result = bitmap_new(w, h);

  // Display UP image
  printf("Load and display: ");
  strcpy(file_name, path);
  strcat(file_name, "_up_normal.bmp");
  fn = strrchr(file_name, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(file_name);
  read_bytemap_in_screen(up_image, NULL, NULL);
  write_bytemap_to_screen(up_image, up_image, up_image);
  wait_keyhit();

  // Display MID Image
  printf("Load and display: ");
  strcpy(file_name, path);
  strcat(file_name, "_mid_normal.bmp");
  fn = strrchr(file_name, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(file_name);
  read_bytemap_in_screen(mid_image, NULL, NULL);
  write_bytemap_to_screen(mid_image, mid_image, mid_image);
  wait_keyhit();

  // Display DN Image
  printf("Load and display: ");
  strcpy(file_name, path);
  strcat(file_name, "_dn_normal.bmp");
  fn = strrchr(file_name, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(file_name);
  read_bytemap_in_screen(dn_image, NULL, NULL);
  write_bytemap_to_screen(dn_image, dn_image, dn_image);
  wait_keyhit();

  // Difference
  if (strcmp(token1, "up") == 0) op1 = up_image;
  else if (strcmp(token1, "mid") == 0) op1 = mid_image;
  else if (strcmp(token1, "dn") == 0) op1 = dn_image;
  else exit(1);

  if (strcmp(token2, "up") == 0) op2 = up_image;
  else if (strcmp(token2, "mid") == 0) op2 = mid_image;
  else if (strcmp(token2, "dn") == 0) op2 = dn_image;
  else exit(1);

  // op1 - op2 + 255 for result
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = bytemap_get_value(op1, j, i);
      value -= bytemap_get_value(op2, j, i);
      if (value >= cutup && value <= cutdown) {
	bitmap_set_value(result, j, i);
      } else {
	bitmap_reset_value(result, j, i);
      }
    }
  }

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = (bytemap_get_value(op1, j, i) + bytemap_get_value(op2, j, i)) / 2.0;
      if (bitmap_isset(result, j, i)) {
	bytemap_put_value((255+value)/2.0, blue, j, i);
      } else {
	bytemap_put_value(value, blue, j, i);
      }
      bytemap_put_value(value, green, j, i);
      bytemap_put_value(value, red, j, i);
    }
  }
  write_bytemap_to_screen(red, green, blue);
  sprintf(file_name, "%s_%s_sub_%s_at_%07.0lf_%07.0lf.bmp",
	  path, token1, token2, cutup, cutdown);
  save_screen_as_BMP(file_name);
  wait_keyhit();

  dynamic_bitmap2bytemap(gray, result);
  sprintf(file_name, "%s_%s_sub_%s_at_%07.0lf_%07.0lf_result.bmp",
	  path, token1, token2, cutup, cutdown);
  save_bytemap_as_gray_BMP(gray, file_name);

  bitmap_destroy(result);
  bytemap_destroy(blue);
  bytemap_destroy(green);
  bytemap_destroy(red);
  bytemap_destroy(gray);
  bytemap_destroy(dn_image);
  bytemap_destroy(mid_image);
  bytemap_destroy(up_image);

  return 0;
}

