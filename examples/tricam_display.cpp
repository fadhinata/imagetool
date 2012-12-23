#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <SDL/SDL.h>

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

#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

int tricam_display(int argc, char *argv[])
{
  int w, h, bpp;
  bytemap_t *xxx_spot, *xxx_bruise;
  bytemap_t *xxx_spot_morph, *xxx_bruise_morph;
  bytemap_t *xxx_spot_label, *xxx_bruise_label;
  bytemap_t *up_image, *mid_image, *dn_image;
  char path[256], *fn;
  SDL_Event event;

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);
  
  up_image = bytemap_new(w, h);
  mid_image = bytemap_new(w, h);
  dn_image = bytemap_new(w, h);
  xxx_spot = bytemap_new(w, h);
  xxx_bruise = bytemap_new(w, h);
  xxx_spot_morph = bytemap_new(w, h);
  xxx_bruise_morph = bytemap_new(w, h);
  xxx_spot_label = bytemap_new(w, h);
  xxx_bruise_label = bytemap_new(w, h);

  ///////////////////////////////////////////////////////
  // Display UP image
  printf("Load and display: ");
  strcpy(path, argv[1]);
  strcat(path, "_up_normal.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(up_image, NULL, NULL);
  //write_bytemap_to_screen(up_image, up_image, up_image);
  //wait_keyhit();

  ///////////////////////////////////////////////////////////
  // Display MID Image
  printf("Load and display: ");
  strcpy(path, argv[1]);
  strcat(path, "_mid_normal.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(mid_image, NULL, NULL);
  //write_bytemap_to_screen(mid_image, mid_image, mid_image);
  //wait_keyhit();

  ///////////////////////////////////////////////////////////
  // Display DN Image
  printf("Load and display: ");
  strcpy(path, argv[1]);
  strcat(path, "_dn_normal.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(dn_image, NULL, NULL);
  //write_bytemap_to_screen(dn_image, dn_image, dn_image);
  //wait_keyhit();

  ////////////////////////////////////////////////////////
  // spot
  strcpy(path, argv[1]);
  strcat(path, "_xxx_spot.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(xxx_spot, NULL, NULL);
  //write_bytemap_to_screen(xxx_spot, xxx_spot, xxx_spot);
  //wait_keyhit();

  strcpy(path, argv[1]);
  strcat(path, "_xxx_spot_opening.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(xxx_spot_morph, NULL, NULL);
  //write_bytemap_to_screen(xxx_spot_morph, xxx_spot_morph, xxx_spot_morph);
  //wait_keyhit();

  strcpy(path, argv[1]);
  strcat(path, "_xxx_spot_labeling.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(xxx_spot_label, NULL, NULL);
  //write_bytemap_to_screen(xxx_spot_label, xxx_spot_label, xxx_spot_label);
  //wait_keyhit();

  ////////////////////////////////////////////////////////
  // bruise
  strcpy(path, argv[1]);
  strcat(path, "_xxx_bruise.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(xxx_bruise, NULL, NULL);
  //write_bytemap_to_screen(xxx_bruise, xxx_bruise, xxx_bruise);
  //wait_keyhit();

  strcpy(path, argv[1]);
  strcat(path, "_xxx_bruise_opening.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(xxx_bruise_morph, NULL, NULL);
  //write_bytemap_to_screen(xxx_bruise_morph, xxx_bruise_morph, xxx_bruise_morph);
  //wait_keyhit();

  strcpy(path, argv[1]);
  strcat(path, "_xxx_bruise_labeling.bmp");
  fn = strrchr(path, '/') + 1;
  printf("%s\n", fn);
  load_and_display_BMP(path);
  read_bytemap_in_screen(xxx_bruise_label, NULL, NULL);
  //write_bytemap_to_screen(xxx_bruise_label, xxx_bruise_label, xxx_bruise_label);
  //wait_keyhit();

  ////////////////////////////////////////////////////////
  // display
  printf("display\n");
  while (1) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_KEYDOWN:
	//printf("The %c scancode was pressed!\n", event.key.keysym.scancode);
	//printf("The %s key was pressed!\n", SDL_GetKeyName(event.key.keysym.sym));
	switch (event.key.keysym.sym) {
	case SDLK_1: // up image display
	  printf("Display up image\n");
	  write_bytemap_to_screen(up_image, up_image, up_image);
	  break;
	case SDLK_2: // mid image display
	  printf("Display mid image\n");
	  write_bytemap_to_screen(mid_image, mid_image, mid_image);
	  break;
	case SDLK_3: // dn image display
	  printf("Display dn image\n");
	  write_bytemap_to_screen(dn_image, dn_image, dn_image);
	  break;
	case SDLK_q: // xxx_spot image display
	  printf("Display xxx_spot image\n");
	  write_bytemap_to_screen(xxx_spot, NULL, NULL);
	  break;
	case SDLK_w: // xxx_spot_morph image display
	  printf("Display xxx_spot_morph image\n");
	  write_bytemap_to_screen(xxx_spot_morph, NULL, NULL);
	  break;
	case SDLK_e: // xxx_spot_label image display
	  printf("Display xxx_spot_label image\n");
	  write_bytemap_to_screen(xxx_spot_label, NULL, NULL);
	  break;
	case SDLK_a: // xxx_bruise image display
	  printf("Display xxx_bruise image\n");
	  write_bytemap_to_screen(NULL, NULL, xxx_bruise);
	  break;
	case SDLK_s: // xxx_bruise_morph image display
	  printf("Display xxx_bruise_morph image\n");
	  write_bytemap_to_screen(NULL, NULL, xxx_bruise_morph);
	  break;
	case SDLK_d: // xxx_bruise_label image display
	  printf("Display xxx_bruise_label image\n");
	  write_bytemap_to_screen(NULL, NULL, xxx_bruise_label);
	  break;
	default:
	  break;
	}
	break;
      case SDL_MOUSEMOTION:
	/*
	printf("Mouse moved by %d,%d to (%d,%d)\n", 
	       event.motion.xrel, event.motion.yrel,
	       event.motion.x, event.motion.y);
	*/
	break;
      case SDL_MOUSEBUTTONDOWN:
	/*
	printf("Mouse button %d pressed at (%d,%d)\n",
	       event.button.button, event.button.x, event.button.y);
	*/
	break;
      case SDL_QUIT:
	bytemap_destroy(xxx_bruise_label);
	bytemap_destroy(xxx_spot_label);
	bytemap_destroy(xxx_bruise_morph);
	bytemap_destroy(xxx_spot_morph);
	bytemap_destroy(xxx_bruise);
	bytemap_destroy(xxx_spot);
	bytemap_destroy(dn_image);
	bytemap_destroy(mid_image);
	bytemap_destroy(up_image);
	exit(0);
	break;
      }
    }
  }

  return 0;
}
  
