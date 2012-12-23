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

// This is for try11 of apple

#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

#undef wait_keyhit()
#define wait_keyhit()

/////////////////////////////////////////
#define UP_DX 7
#define UP_DY 20

#define UP_FROM 60
#define UP_TO 255

#define UP_MIN 26.397
#define UP_MEAN 107.599
#define UP_MAX 200.073

/////////////////////////////////////////
#define MID_DX 0
#define MID_DY 0

#define MID_FROM 30
#define MID_TO 255

#define MID_MIN 27.943
#define MID_MEAN 66.330
#define MID_MAX 132.206

/////////////////////////////////////////
#define DN_DX 15
#define DN_DY 21

#define DN_FROM 60
#define DN_TO 255

#define DN_MIN 32.58
#define DN_MEAN 118.678
#define DN_MAX 201.543

/////////////////////////////////////////
#define SPOT_FROM 270.0
#define SPOT_TO 350.0
#define SPOT_OP1 mid_image
#define SPOT_OP2 up_image

#define BRUISE_FROM 295.0
#define BRUISE_TO 512.0
#define BRUISE_OP1 dn_image
#define BRUISE_OP2 mid_image

/////////////////////////////////////////
static const char *program_name = NULL;
static char *path;
static int verbose = 0;

static const struct option long_options[] = {
  {"help", 0, NULL, 'h'},
  {"op", 1, NULL, 'o'},
  {"sub", 1, NULL, 's'},
  {"from", 1, NULL, 'f'},
  {"to", 1, NULL, 't'},
  {"verbose", 0, NULL, 'v'},
  {"name", 1, NULL, 'n'},
  {NULL, 0, NULL, 0}
};

static const char *const short_options = "ho:s:f:t:vn:";

static void print_usage(FILE *stream, int exit_code)
{
  fprintf(stream, "Usage : %s [ ... ]\n", program_name);
  fprintf(stream, "-h,           --help    Display this usage information.\n");
  fprintf(stream, "-o,           --op      Set operand for subtracting.\n");
  fprintf(stream, "-s,           --sub     Set subtract for subtracting.\n");
  fprintf(stream, "-f [Cutup],   --from    Set Cutup value to threshold.\n");
  fprintf(stream, "-t [Cutdown], --to      Set Cutdown value to threhold.\n");
  fprintf(stream, "-v,           --verbose Print verbose message.\n");
  fprintf(stream, "-n [image],   --name    Set input file.\n");
  exit(exit_code);
}

int tricam_apple_sub(int argc, char *argv[])
{
  int x, y, i, j, w, h, bpp, mode = 0, save = 0;
  real_t value, cutup = -255, cutdown = 255;
  bitmap_t *spot, *bruise;
  bitmap_t *up_roi, *mid_roi, *dn_roi, *tmp, *roi, *bin;
  bytemap_t *up_image, *mid_image, *dn_image;
  bytemap_t *gray, *se, *red, *green, *blue;
  char filename[256], *fn, *ptr;
  FILE *fd = NULL;
  dwordmap_t *labelmap;
  label_info_t *label_info;
  int label, area;

  bytemap_t *op1, *op2, *op3;
  char *path;
  int next_option;
  char *token1, *token2, *token3;

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

  printf("(%s - %s), %lf, %lf, %s\n",
	 token1, token2, cutup, cutdown, path);

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);
  
  up_image = bytemap_new(w, h);
  mid_image = bytemap_new(w, h);
  dn_image = bytemap_new(w, h);

  gray = bytemap_new(w, h);
  red = bytemap_new(w, h);
  green = bytemap_new(w, h);
  blue = bytemap_new(w, h);

  up_roi = bitmap_new(w, h);
  mid_roi = bitmap_new(w, h);
  dn_roi = bitmap_new(w, h);

  bin = bitmap_new(w, h);
  tmp = bitmap_new(w, h);
  roi = bitmap_new(w, h);

  spot = bitmap_new(w, h);
  bruise = bitmap_new(w, h);

  se = bytemap_new(3, 3);
  bytemap_fill(se, 0, 0, 3, 3, 1);

  labelmap = dwordmap_new(w, h);

  // Up display image ////////////////////
  sprintf(filename, "%s_up.bmp", path);
  fn = strrchr(filename, '/') + 1;
  printf("Filename: %s\n", fn);
  printf("Display image\n");
  load_and_display_BMP(filename);
  read_bytemap_in_screen(up_image, NULL, NULL);
  bytemap_clear(gray);
  bytemap_copy(gray, UP_DX, UP_DY, up_image, 0, 0, w, h);
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  printf("Median filtering \n");
  mean_smooth(up_image, gray, 3);
  write_bytemap_to_screen(up_image, up_image, up_image);
  wait_keyhit();
  ////////////////////////////////////////

  // MID display image ///////////////////
  sprintf(filename, "%s_mid.bmp", path);
  fn = strrchr(filename, '/') + 1;
  printf("Filename: %s\n", fn);
  printf("Display image\n");
  load_and_display_BMP(filename);
  read_bytemap_in_screen(mid_image, NULL, NULL);
  bytemap_clear(gray);
  bytemap_copy(gray, MID_DX, MID_DY, mid_image, 0, 0, w, h);
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  printf("Median filtering \n");
  mean_smooth(mid_image, gray, 3);
  write_bytemap_to_screen(mid_image, mid_image, mid_image);
  wait_keyhit();
  /////////////////////////////////////////

  // DN display image /////////////////////
  sprintf(filename, "%s_dn.bmp", path);
  fn = strrchr(filename, '/') + 1;
  printf("filename: %s\n", fn);
  printf("Display image\n");
  load_and_display_BMP(filename);
  read_bytemap_in_screen(dn_image, NULL, NULL);
  bytemap_clear(gray);
  bytemap_copy(gray, DN_DX, DN_DY, dn_image, 0, 0, w, h);
  write_bytemap_to_screen(gray, gray, gray);
  wait_keyhit();

  printf("Median filtering \n");
  mean_smooth(dn_image, gray, 3);
  write_bytemap_to_screen(dn_image, dn_image, dn_image);
  wait_keyhit();
  ///////////////////////////////////////

  printf("Image thresholding\n");
  bytemap_threshold(bin, up_image, UP_FROM, UP_TO);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Big blobs greping\n");
  bitmap_clear(roi);
  label_info = label_info_new();
  labeling(labelmap, label_info, bin, NEIGHBOR_8);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 3000) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(roi, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);
  write_bitmap_to_screen(roi, roi, roi);
  wait_keyhit();

  printf("Fill holes\n");
  bitmap_complement(bin, roi);
  labeling_grep_big_blob(roi, bin);
  bitmap_complement(bin, roi);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Image Morphology\n");
  binary_closing(tmp, bin, se);
  binary_opening(up_roi, tmp, se);
  write_bitmap_to_screen(up_roi, up_roi, up_roi);
  wait_keyhit();

  ///////////////////////////////////////
  printf("Image thresholding\n");
  bytemap_threshold(bin, mid_image, MID_FROM, MID_TO);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Big blobs greping\n");
  bitmap_clear(roi);
  label_info = label_info_new();
  labeling(labelmap, label_info, bin, NEIGHBOR_8);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 3000) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(roi, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);
  write_bitmap_to_screen(roi, roi, roi);
  wait_keyhit();

  printf("Fill holes\n");
  bitmap_complement(bin, roi);
  labeling_grep_big_blob(roi, bin);
  bitmap_complement(bin, roi);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Image Morphology\n");
  binary_closing(tmp, bin, se);
  binary_opening(mid_roi, tmp, se);
  write_bitmap_to_screen(mid_roi, mid_roi, mid_roi);
  wait_keyhit();

  ///////////////////////////////////////////////////
  printf("Image thresholding\n");
  bytemap_threshold(bin, dn_image, DN_FROM, DN_TO);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Big blobs greping\n");
  bitmap_clear(roi);

  label_info = label_info_new();
  labeling(labelmap, label_info, bin, NEIGHBOR_8);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 3000) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(roi, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);
  write_bitmap_to_screen(roi, roi, roi);
  wait_keyhit();

  printf("Fill holes\n");
  bitmap_complement(bin, roi);
  labeling_grep_big_blob(roi, bin);
  bitmap_complement(bin, roi);
  write_bitmap_to_screen(bin, bin, bin);
  wait_keyhit();

  printf("Image Morphology\n");
  binary_closing(tmp, bin, se);
  binary_opening(dn_roi, tmp, se);
  write_bitmap_to_screen(dn_roi, dn_roi, dn_roi);
  wait_keyhit();
  ///////////////////////////////////////////////

  bitmap_clear(roi);
  bitmap_or(roi, up_roi);
  bitmap_or(roi, mid_roi);
  bitmap_or(roi, dn_roi);
  write_bitmap_to_screen(up_roi, mid_roi, dn_roi);
  wait_keyhit();
  write_bitmap_to_screen(roi, roi, roi);
  wait_keyhit();

  // Up image normalization
  printf("Up-Image global normalization\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = bytemap_get_value(up_image, j, i);
	if (value < UP_MEAN) {
	  value = round(128.0 / (UP_MEAN - UP_MIN) * (value - UP_MIN));
	} else {
	  value = round(128.0 / (UP_MAX - UP_MEAN) * (value - UP_MEAN) + 128.0);
	}
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, up_image, j, i);
      } else {
	bytemap_put_value(0, up_image, j, i);
      }
    }
  }
  write_bytemap_to_screen(up_image, up_image, up_image);
  //strcpy(path, argv[1]);
  //strcat(path, "_up_normal.bmp");
  //save_bytemap_as_gray_BMP(up_image, path);
  wait_keyhit();
  //////////////////////////////////////////

  // Mid image normalization
  printf("MID-Image global normalization\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = bytemap_get_value(mid_image, j, i);
	if (value < MID_MEAN) {
	  value = round(128.0 / (MID_MEAN - MID_MIN) * (value - MID_MIN));
	} else {
	  value = round(128.0 / (MID_MAX - MID_MEAN) * (value - MID_MEAN) + 128.0);
	}
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, mid_image, j, i);
      } else {
	bytemap_put_value(0, mid_image, j, i);
      }
    }
  }
  write_bytemap_to_screen(mid_image, mid_image, mid_image);
  //strcpy(path, argv[1]);
  //strcat(path, "_mid_normal.bmp");
  //save_bytemap_as_gray_BMP(mid_image, path);
  wait_keyhit();
  ///////////////////////////////////////////////

  // Dn image normalization
  printf("Dn image global normalization\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(roi, j, i)) {
	value = bytemap_get_value(dn_image, j, i);
	if (value < DN_MEAN) {
	  value = round(128.0 / (DN_MEAN - DN_MIN) * (value - DN_MIN));
	} else {
	  value = round(128.0 / (DN_MAX - DN_MEAN) * (value - DN_MEAN) + 128.0);
	}
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	bytemap_put_value(value, dn_image, j, i);
      } else {
	bytemap_put_value(0, dn_image, j, i);
      }
    }
  }
  write_bytemap_to_screen(dn_image, dn_image, dn_image);
  //strcpy(path, argv[1]);
  //strcat(path, "_dn_normal.bmp");
  //save_bytemap_as_gray_BMP(dn_image, path);
  wait_keyhit();
  ////////////////////////////////////////////

  // Difference
  // up - dn for stem end
  printf("for spot\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = bytemap_get_value(SPOT_OP1, j, i) - bytemap_get_value(SPOT_OP2, j, i) + 255.0;
      if ((value >= SPOT_FROM && value <= SPOT_TO) && bytemap_get_value(SPOT_OP2, j, i) < 100) {
	bitmap_set_value(spot, j, i);
      } else {
	bitmap_reset_value(spot, j, i);
      }
    }
  }
  //binary_closing(stem_end, bin, se);
  //write_bitmap_to_screen(NULL, stem_end, NULL);
  //wait_keyhit();

  printf("for bruise\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = bytemap_get_value(BRUISE_OP1, j, i) - bytemap_get_value(BRUISE_OP2, j, i) + 255.0;
      if ((value >= BRUISE_FROM && value <= BRUISE_TO) && bytemap_get_value(BRUISE_OP2, j, i) < 100) {
	bitmap_set_value(bruise, j, i);
      } else {
	bitmap_reset_value(bruise, j, i);
      }
    }
  }
  //write_bitmap_to_screen(sour_rot, NULL, NULL);
  //wait_keyhit();
  write_bitmap_to_screen(spot, NULL, bruise);
  wait_keyhit();

  label_info = label_info_new();
  labeling(labelmap, label_info, spot, NEIGHBOR_8);
  bitmap_clear(spot);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 30) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(spot, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = (bytemap_get_value(up_image, j, i) + bytemap_get_value(mid_image, j, i) + bytemap_get_value(dn_image, j, i)) / 3.0;
      if (bitmap_isset(spot, j, i))
	bytemap_put_value(255, red, j, i);
      else
	bytemap_put_value(value, red, j, i);
      bytemap_put_value(value, green, j, i);
      bytemap_put_value(value, blue, j, i);
    }
  }
  write_bytemap_to_screen(red, green, blue);
  sprintf(filename, "%s_spot_at_%03.0lf_%03.0lf.bmp",
	  path, SPOT_FROM, SPOT_TO);
  save_screen_as_BMP(filename);
  wait_keyhit();

  dynamic_bitmap2bytemap(gray, spot);
  sprintf(filename, "%s_spot.bmp", path);
  save_bytemap_as_gray_BMP(gray, filename);

  label_info = label_info_new();
  labeling(labelmap, label_info, bruise, NEIGHBOR_8);
  bitmap_clear(bruise);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 30) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(bruise, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = (bytemap_get_value(up_image, j, i) + bytemap_get_value(mid_image, j, i) + bytemap_get_value(dn_image, j, i)) / 3.0;
      if (bitmap_isset(bruise, j, i))
	bytemap_put_value(255, blue, j, i);
      else
	bytemap_put_value(value, blue, j, i);
      bytemap_put_value(value, green, j, i);
      bytemap_put_value(value, red, j, i);
    }
  }
  write_bytemap_to_screen(red, green, blue);
  sprintf(filename, "%s_bruise_at_%03.0lf_%03.0lf.bmp",
	  path, BRUISE_FROM, BRUISE_TO);
  save_screen_as_BMP(filename);
  wait_keyhit();

  dynamic_bitmap2bytemap(gray, bruise);
  sprintf(filename, "%s_bruise.bmp", path);
  save_bytemap_as_gray_BMP(gray, filename);

  dwordmap_destroy(labelmap);
  bitmap_destroy(bruise);
  bitmap_destroy(spot);
  bytemap_destroy(gray);
  bytemap_destroy(dn_image);
  bytemap_destroy(mid_image);
  bytemap_destroy(up_image);

  return 0;
}
