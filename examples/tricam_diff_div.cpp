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

static const struct option long_options[] = {
  {"help", 0, NULL, 'h'},
  {"op", 1, NULL, 'o'},
  {"sub", 1, NULL, 's'},
  {"div", 1, NULL, 'd'},
  {"from", 1, NULL, 'f'},
  {"to", 1, NULL, 't'},
  {"verbose", 0, NULL, 'v'},
  {"name", 1, NULL, 'n'},
  {NULL, 0, NULL, 0}
};

static const char *const short_options = "ho:s:d:f:t:vn:";

static void print_usage(FILE *stream, int exit_code)
{
  fprintf(stream, "Usage : %s [ ... ]\n", program_name);
  fprintf(stream, "-h,           --help    Display this usage information.\n");
  fprintf(stream, "-o,           --op      Set operand for subtracting.\n");
  fprintf(stream, "-s,           --sub     Set subtract for subtracting.\n");
  fprintf(stream, "-d,           --div     Set subtract for subtracting.\n");
  fprintf(stream, "-f [Cutup],   --from    Set Cutup value to threshold.\n");
  fprintf(stream, "-t [Cutdown], --to      Set Cutdown value to threhold.\n");
  fprintf(stream, "-v,           --verbose Print verbose message.\n");
  fprintf(stream, "-n [image],   --name    Set input file.\n");
  exit(exit_code);
}

int tricam_diff_div(int argc, char *argv[])
{
  int x, y, i, j, w, h, bpp, mode = 0, save = 0;
  real_t value;
  bitmap_t *result;
  bytemap_t *up_image, *mid_image, *dn_image;
  bytemap_t *gray, *red, *green, *blue;
  char *fn, *ptr, file_name[256];
  FILE *fd = NULL;

  bytemap_t *op1, *op2, *op3;
  char *path;
  real_t cutup, cutdown;
  int next_option;
  char *token1, *token2, *token3;

  program_name = argv[0];
  do {
    next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option) {
    case 'h': print_usage(stdout, 0); break;
    case 'o': token1 = strdup(optarg); break;
    case 's': token2 = strdup(optarg); break;
    case 'd': token3 = strdup(optarg); break;
    case 'f': cutup = strtod(optarg, NULL); break;
    case 't': cutdown = strtod(optarg, NULL); break;
    case 'v': verbose = 1; break;
    case 'n': path = strdup(optarg); break;
    case '?': print_usage(stderr, 1); break;
    case -1: break;
    default: abort();
    }
  } while (next_option != -1);

  printf("(%s - %s) / %s, %lf, %lf, %s\n",
	 token1, token2, token3, cutup, cutdown, path);

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
  sprintf(file_name, "%s_up_normal.bmp", path);
  fn = strrchr(file_name, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(file_name);
  read_bytemap_in_screen(up_image, NULL, NULL);
  write_bytemap_to_screen(up_image, up_image, up_image);
  wait_keyhit();

  // Display MID Image
  printf("Load and display: ");
  sprintf(file_name, "%s_mid_normal.bmp", path);
  fn = strrchr(file_name, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(file_name);
  read_bytemap_in_screen(mid_image, NULL, NULL);
  write_bytemap_to_screen(mid_image, mid_image, mid_image);
  wait_keyhit();

  // Display DN Image
  printf("Load and display: ");
  sprintf(file_name, "%s_dn_normal.bmp", path);
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(file_name);
  read_bytemap_in_screen(dn_image, NULL, NULL);
  write_bytemap_to_screen(dn_image, dn_image, dn_image);
  wait_keyhit();

  if (strcmp(token1, "up") == 0) op1 = up_image;
  else if (strcmp(token1, "mid") == 0) op1 = mid_image;
  else if (strcmp(token1, "dn") == 0) op1 = dn_image;
  else exit(1);

  if (strcmp(token2, "up") == 0) op2 = up_image;
  else if (strcmp(token2, "mid") == 0) op2 = mid_image;
  else if (strcmp(token2, "dn") == 0) op2 = dn_image;
  else exit(1);

  if (strcmp(token3, "up") == 0) op3 = up_image;
  else if (strcmp(token3, "mid") == 0) op3 = mid_image;
  else if (strcmp(token3, "dn") == 0) op3 = dn_image;
  else exit(1);

  real_t value1, value2, value3;

  // 10000.0 * (op1 - op2 + 255) / (op3 + 1.0)
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value1 = bytemap_get_value(op1, j, i);
      value2 = bytemap_get_value(op2, j, i);
      value3 = bytemap_get_value(op3, j, i);
      value = 10000.0 * (value1 - value2) / (value3 + 1.0);
      if (value >= cutup && value <= cutdown)
	bitmap_set_value(result, j, i);
      else
	bitmap_reset_value(result, j, i);
    }
  }

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = (bytemap_get_value(up_image, j, i) + bytemap_get_value(mid_image, j, i) + bytemap_get_value(dn_image, j, i)) / 3.0;
      if (bitmap_isset(result, j, i))
	bytemap_put_value(255, red, j, i);
      else
	bytemap_put_value(value, red, j, i);
      bytemap_put_value(value, green, j, i);
      bytemap_put_value(value, blue, j, i);
    }
  }
  write_bytemap_to_screen(red, green, blue);
  sprintf(file_name, "%s_%s_sub_%s_div_%s_at_%07.0lf_%07.0lf.bmp",
	  path, token1, token2, token3, cutup, cutdown);
  save_screen_as_BMP(file_name);
  wait_keyhit();

  dynamic_bitmap2bytemap(gray, result);
  sprintf(file_name, "%s_%s_sub_%s_div_%s_at_%07.0lf_%07.0lf_result.bmp",
	  path, token1, token2, token3, cutup, cutdown);
  save_bytemap_as_gray_BMP(gray, file_name);
  //save_bytemap_as_BMP(gray, path);

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
  
