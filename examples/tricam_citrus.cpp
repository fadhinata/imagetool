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

// This is for try12

#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

#undef wait_keyhit()
#define wait_keyhit()

/////////////////////////////////////////
#define UP_DX 2
#define UP_DY -4

#define UP_FROM 30
#define UP_TO 255

#define UP_MIN 16.952
#define UP_MEAN 108.028
#define UP_MAX 182.222

/////////////////////////////////////////
#define MID_DX 0
#define MID_DY 0

#define MID_FROM 30
#define MID_TO 255

#define MID_MIN 18.143
#define MID_MEAN 85.361
#define MID_MAX 144.111

/////////////////////////////////////////
#define DN_DX 10
#define DN_DY -23

#define DN_FROM 17
#define DN_TO 255

#define DN_MIN 8.397
#define DN_MEAN 42.504
#define DN_MAX 71.048

/////////////////////////////////////////
#define STEM_FROM 300.0
#define STEM_TO 350.0
#define STEM_OP1 up_image
#define STEM_OP2 dn_image

#define SOUR_FROM 287.0
#define SOUR_TO 512.0
#define SOUR_OP1 up_image
#define SOUR_OP2 mid_image

#define GRAY_FROM 270.0
#define GRAY_TO 512.0
#define GRAY_OP1 mid_image
#define GRAY_OP2 up_image

/////////////////////////////////////////
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

int tricam_citrus(int argc, char *argv[])
{
  int x, y, i, j, w, h, bpp, mode = 0, save = 0;
  real_t value, cutup = -255, cutdown = 255;
  bitmap_t *stem_end, *sour_rot, *gray_rot;
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

  up_roi = bitmap_new(w, h);
  mid_roi = bitmap_new(w, h);
  dn_roi = bitmap_new(w, h);

  bin = bitmap_new(w, h);
  tmp = bitmap_new(w, h);
  roi = bitmap_new(w, h);

  stem_end = bitmap_new(w, h);
  sour_rot = bitmap_new(w, h);
  gray_rot = bitmap_new(w, h);

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
  printf("for stem end\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = bytemap_get_value(STEM_OP1, j, i) - bytemap_get_value(STEM_OP2, j, i) + 255.0;
      if (value >= STEM_FROM && value <= STEM_TO) {
	bitmap_set_value(stem_end, j, i);
      } else {
	bitmap_reset_value(stem_end, j, i);
      }
    }
  }
  binary_dilate(bin, stem_end, se);
  binary_dilate(stem_end, bin, se);
  //binary_closing(stem_end, bin, se);
  //write_bitmap_to_screen(NULL, stem_end, NULL);
  //wait_keyhit();

  printf("for sour rot\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = bytemap_get_value(SOUR_OP1, j, i) - bytemap_get_value(SOUR_OP2, j, i) + 255.0;
      if (value >= SOUR_FROM && value <= SOUR_TO && bytemap_get_value(dn_image, j, i) < 100) {
	bitmap_set_value(sour_rot, j, i);
      } else {
	bitmap_reset_value(sour_rot, j, i);
      }
    }
  }
  //write_bitmap_to_screen(sour_rot, NULL, NULL);
  //wait_keyhit();

  printf("for gray rot\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = bytemap_get_value(GRAY_OP1, j, i) - bytemap_get_value(GRAY_OP2, j, i) + 255.0;
      if (value >= GRAY_FROM && value <= GRAY_TO) {
	bitmap_set_value(gray_rot, j, i);
      } else {
	bitmap_reset_value(gray_rot, j, i);
      }
    }
  }
  write_bitmap_to_screen(sour_rot, stem_end, gray_rot);
  wait_keyhit();

  /*
  labelmap = dwordmap_new(w, h);
  label_info = label_info_new();
  labeling(labelmap, label_info, stem_end, NEIGHBOR_8);
  bitmap_clear(stem_end);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 40) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(stem_end, j, i);
	  }
	}
      }
    }
  }
  label_info_destroy(label_info);
  dwordmap_destroy(labelmap);
  */

  printf("Deletation on stem_end\n");
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(sour_rot, j, i) && bitmap_isset(stem_end, j, i))
	bitmap_reset_value(sour_rot, j, i);
      if (bitmap_isset(gray_rot, j, i) && bitmap_isset(stem_end, j, i))
	bitmap_reset_value(gray_rot, j, i);
    }
  }
  write_bitmap_to_screen(sour_rot, NULL, gray_rot);
  wait_keyhit();

  printf("Determination for Sour Rot\n");
  labelmap = dwordmap_new(w, h);
  label_info = label_info_new();
  labeling(labelmap, label_info, sour_rot, NEIGHBOR_8);
  bitmap_clear(sour_rot);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 200) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(sour_rot, j, i);
	  }
	}
      }
    }
  }
  /*
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(sour_rot, j, i) && bitmap_isset(stem_end, j, i)) {
	label = dwordmap_get_value(labelmap, j, i);
	for (y = 0; y < h; y++) {
	  for (x = 0; x < w; x++) {
	    if (dwordmap_get_value(labelmap, x, y) == label) {
	      bitmap_reset_value(sour_rot, x, y);
	    }
	  }
	}
      }
    }
  }
  */
  label_info_destroy(label_info);
  dwordmap_destroy(labelmap);

  printf("Determination for Gray Rot\n");

  labelmap = dwordmap_new(w, h);
  label_info = label_info_new();

  labeling(labelmap, label_info, gray_rot, NEIGHBOR_8);
  bitmap_clear(gray_rot);
  for (label = 0; label < label_info_get_count(label_info); label++) {
    label_info_glimpse(&area, label, label_info);
    //printf("area:%d\n", area);
    if (area > 200) {
      for (i = 0; i < h; i++) {
	for (j = 0; j < w; j++) {
	  if (dwordmap_get_value(labelmap, j, i) == label) {
	    bitmap_set_value(gray_rot, j, i);
	  }
	}
      }
    }
  }
  /*
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (bitmap_isset(gray_rot, j, i) && bitmap_isset(stem_end, j, i)) {
	label = dwordmap_get_value(labelmap, j, i);
	for (y = 0; y < h; y++) {
	  for (x = 0; x < w; x++) {
	    if (dwordmap_get_value(labelmap, x, y) == label) {
	      bitmap_reset_value(gray_rot, x, y);
	    }
	  }
	}
      }
    }
  }
  */
  label_info_destroy(label_info);
  dwordmap_destroy(labelmap);

  write_bitmap_to_screen(sour_rot, NULL, gray_rot);
  wait_keyhit();

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = (bytemap_get_value(up_image, j, i) + bytemap_get_value(mid_image, j, i) + bytemap_get_value(dn_image, j, i)) / 3.0;
      if (bitmap_isset(sour_rot, j, i))
	bytemap_put_value(255, red, j, i);
      else
	bytemap_put_value(value, red, j, i);
      bytemap_put_value(value, green, j, i);
      bytemap_put_value(value, blue, j, i);
    }
  }
  write_bytemap_to_screen(red, green, blue);
  sprintf(filename, "%s_sour_rot_at_%03.0lf_%03.0lf.bmp",
	  path, SOUR_FROM, SOUR_TO);
  save_screen_as_BMP(filename);
  wait_keyhit();

  dynamic_bitmap2bytemap(gray, sour_rot);
  sprintf(filename, "%s_sour_rot.bmp", path);
  save_bytemap_as_gray_BMP(gray, filename);

  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = (bytemap_get_value(up_image, j, i) + bytemap_get_value(mid_image, j, i) + bytemap_get_value(dn_image, j, i)) / 3.0;
      if (bitmap_isset(gray_rot, j, i))
	bytemap_put_value(255, blue, j, i);
      else
	bytemap_put_value(value, blue, j, i);
      bytemap_put_value(value, green, j, i);
      bytemap_put_value(value, red, j, i);
    }
  }
  write_bytemap_to_screen(red, green, blue);
  sprintf(filename, "%s_gray_rot_at_%03.0lf_%03.0lf.bmp",
	  path, GRAY_FROM, GRAY_TO);
  save_screen_as_BMP(filename);
  wait_keyhit();

  dynamic_bitmap2bytemap(gray, gray_rot);
  sprintf(filename, "%s_gray_rot.bmp", path);
  save_bytemap_as_gray_BMP(gray, filename);

  bitmap_destroy(sour_rot);
  bitmap_destroy(gray_rot);
  bitmap_destroy(stem_end);
  bytemap_destroy(gray);
  bytemap_destroy(dn_image);
  bytemap_destroy(mid_image);
  bytemap_destroy(up_image);

  return 0;
}
