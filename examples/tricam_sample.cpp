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

/*
#define INPUT_KEY   0x80000000
#define KEY_DOWN    0x81000000
#define KEY_UP      0x82000000
#define INPUT_MOUSE 0x40000000
#define MOUSE_LDOWN 0x41000000
#define MOUSE_RDOWN 0x42000000
#define MOUSE_LUP   0x44000000
#define MOUSE_RUP   0x48000000

typedef struct {
  int type;
  union {
    struct {
      int prefix;
      int keycode;
    } keyboard;
    struct {
      int xcoord;
      int ycoord;
    } mouse;
  }
} input_t;
*/


static SDL_Event event;

#if 0
static uint32_t input_get(void)
{
  SDL_PumpEvents();

  /* We can't return -1, just return 0 (no event) on error */
  if (SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_ALLEVENTS) <= 0) {
    // error
  }

}
if (SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_EVENTMASK(SDL_KEYDOWN))) {
 }

/*
SDL_EVENTMASK (SDL_KEYUP)
(SDL_EVENTMASK (SDL_MOUSEBUTTONDOWN) | SDL_EVENTMASK (SDL_MOUSEBUTTONUP))
SDL_ALLEVENTS
SDL_KEYUPMASK
SDL_ALLEVENTS ^ SDL_QUITMASK
*/
#endif

#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

int tricam_sample(int argc, char *argv[])
{
  int x, y, i, j, w, h, bpp, mode = 0, save = 0;
  real_t value;
  bitmap_t *bin, *roi;
  bytemap_t *up_image, *mid_image, *dn_image;
  bytemap_t *gray;
  wordmap_t *diff, *diff1, *diff2;
  char path[256], *fn, *ptr;
  FILE *fd = NULL;

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);
  
  up_image = bytemap_new(w, h);
  mid_image = bytemap_new(w, h);
  dn_image = bytemap_new(w, h);
  gray = bytemap_new(w, h);
  diff = wordmap_new(w, h);
  diff1 = wordmap_new(w, h);
  diff2 = wordmap_new(w, h);

  bin = bitmap_new(w, h);
  roi = bitmap_new(w, h);

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

  // Merging
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      value = bytemap_get_value(up_image, j, i) + bytemap_get_value(mid_image, j, i) + bytemap_get_value(dn_image, j, i);
      value = round(value / 3.0);
      if (value > 255) value = 255;
      bytemap_put_value(value, gray, j, i);
    }
  }
  write_bytemap_to_screen(gray, gray, gray);
  //wait_keyhit();

  //

  ///////////////////////////////
  printf("sampling\n");
  while (1) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_KEYDOWN:
	//printf("The %c scancode was pressed!\n", event.key.keysym.scancode);
	//printf("The %s key was pressed!\n", SDL_GetKeyName(event.key.keysym.sym));
	switch (event.key.keysym.sym) {
	case SDLK_a: // change normal mode
	  printf("Change Normal mode\n");
	  mode = 0;
	  if (fd) fclose(fd);
	  fd = fopen("normal.txt", "a+");
	  break;
	case SDLK_q: // change spot mode
	  printf("Change Spot mode\n");
	  mode = 1;
	  if (fd) fclose(fd);
	  fd = fopen("spot.txt", "a+");
	  break;
	case SDLK_w: // change pseudo-spot mode
	  printf("Change Pseudo-Spot mode\n");
	  mode = 2;
	  if (fd) fclose(fd);
	  fd = fopen("pseudo_spot.txt", "a+");
	  break;
	case SDLK_e: // change bruise mode
	  printf("Change Bruise mode\n");
	  mode = 3;
	  if (fd) fclose(fd);
	  fd = fopen("bruise.txt", "a+");
	  break;
	case SDLK_r: // change pseudo-bruise mode
	  printf("Change Pseudo-Bruise mode\n");
	  mode = 4;
	  if (fd) fclose(fd);
	  fd = fopen("pseudo_bruise.txt", "a+");
	  break;
	case SDLK_b: // change bottom mode
	  printf("Change Bottom mode\n");
	  mode = 5;
	  if (fd) fclose(fd);
	  fd = fopen("bottom.txt", "a+");
	  break;
	case SDLK_t: // change top mode
	  printf("Change Top mode\n");
	  mode = 6;
	  if (fd) fclose(fd);
	  fd = fopen("top.txt", "a+");
	  break;
	case SDLK_s:
	  if (save && fd) {
	    printf("Save Data\n");
	    fprintf(fd, "%d, %d, %d\n",
		    bytemap_get_value(up_image, x, y),
		    bytemap_get_value(mid_image, x, y),
		    bytemap_get_value(dn_image, x, y));
	    save = 0;
	  }
	  break;
	case SDLK_1: // up_image display
	  printf("Display up_image\n");
	  write_bytemap_to_screen(up_image, up_image, up_image);
	  break;
	case SDLK_2: // mid_image display
	  printf("Display mid_image\n");
	  write_bytemap_to_screen(mid_image, mid_image, mid_image);
	  break;
	case SDLK_3: // dn_image display
	  printf("Display dn_image\n");
	  write_bytemap_to_screen(dn_image, dn_image, dn_image);
	  break;
	case SDLK_4: // mixed image display
	  printf("Display mixed image\n");
	  write_bytemap_to_screen(gray, gray, gray);
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
	switch (mode) {
	case 0: printf("Normal mode\n"); break;
	case 1: printf("Spot mode\n"); break;
	case 2: printf("Pseudo-Spot mode\n"); break;
	case 3: printf("Bruise mode\n"); break;
	case 4: printf("Pseudo-Bruise mode\n"); break;
	case 5: printf("Bottom mode\n"); break;
	case 6: printf("Top mode\n"); break;
	}
	x = event.button.x;
	y = event.button.y;
	save = 1;
	printf("x, y = %d, %d\n", x, y);
	printf("%d, %d, %d\n",
	       bytemap_get_value(up_image, x, y),
	       bytemap_get_value(mid_image, x, y),
	       bytemap_get_value(dn_image, x, y));
	break;
      case SDL_QUIT:
	if (fd) fclose(fd);
	exit(0);
	break;
      }
    }
  }

  return 0;
}
  
