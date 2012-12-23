#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <SDL/SDL.h>
#include <getopt.h>

#include <common.h>
#include <interface/screen.h>
#include <geometry/point.h>

static const char *program_name = NULL;
static char *path;
static int verbose = 0;
static const char *const short_options = "hva:b:d:w:e:h:l:";

static const struct option long_options[] = {
  {"help", 0, NULL, 'h'},
  {"verbose", 0, NULL, 'v'},
  {"alpha", 1, NULL, 'a'},
  {"beta", 1, NULL, 'b'},
  {"w1", 1, NULL, 'd'},
  {"w2", 1, NULL, 'w'},
  {"w3", 1, NULL, 'e'},
  {"h1", 1, NULL, 'h'},
  {"h2", 1, NULL, 'l'},
  {NULL, 0, NULL, 0}
};

static void print_usage(FILE *stream, int exit_code)
{
  fprintf(stream, "Usage : %s [ ... ]\n", program_name);
  fprintf(stream, "-h,       --help    Display this usage information.\n");
  fprintf(stream, "-v,       --verbose Print verbose message.\n");
  fprintf(stream, "-a [deg], --alpha   Set alpha(1st reflective angle).\n");
  fprintf(stream, "-b [deg], --beta    Set beta(2dn reflective angle).\n");
  fprintf(stream, "-d [mm],  --w1      Set w1(distance between the back of lens and prism).\n");
  fprintf(stream, "-w [mm],  --w2      Set w2(width of prism combination.\n");
  fprintf(stream, "-e [mm],  --w3      Set w3(post distnace between prism and image sensor.\n");
  fprintf(stream, "-h [mm],  --h1      Set h1(y value of first facing prism.\n");
  fprintf(stream, "-l [mm],  --h2      Set h2(y value of last facing prism.\n");
  exit(exit_code);
}

#define WIDTH 1024
#define HEIGHT 648
#define BPP 32

// gen_3CCD_prism -a 15 -b 45 -d 5 -w 30 -e 5 -h 20 -l 10
int gen_3CCD_prism(int argc, char *argv[])
{
  int w, h, bpp;
  real_t alpha, beta, w1, w2, w3, h1, h2;
  real_t x1, x2, r, l;
  point_t *A, *B, *C, *D, *E, *F, *G, *H, *I, *J;
  int next_option;

  program_name = argv[0];
  do {
    next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option) {
    case 'h': print_usage(stdout, 0); break;
    case 'v': verbose = 1; break;
    case 'a': alpha = strtod(optarg, NULL) * M_PI / 180.0; break;
    case 'b': beta = strtod(optarg, NULL) * M_PI / 180.0; break;
    case 'd': w1 = strtod(optarg, NULL); break;
    case 'w': w2 = strtod(optarg, NULL); break;
    case 'e': w3 = strtod(optarg, NULL); break;
    case 'h': h1 = strtod(optarg, NULL); break;
    case 'l': h2 = strtod(optarg, NULL); break;
    case '?': print_usage(stderr, 1); break;
    case -1: break;
    default: abort();
    }
  } while (next_option != -1);

  // Verify that alpha and beta are an acute angle
  assert(alpha > 0 && alpha < M_PI / 2);
  assert(beta > 0 && beta < M_PI / 2);
  assert(w1 > 0);
  assert(w2 > 0);
  assert(w3 > 0);
  assert(h1 > 0);
  assert(h2 > 0);
  assert(h1 > h2);

  w = WIDTH; h = HEIGHT; bpp = BPP;

  initialize_screen(w, h, bpp);

  wait_keyhit();

  A = point_new();
  B = point_new();
  C = point_new();
  D = point_new();
  E = point_new();
  F = point_new();
  G = point_new();
  H = point_new();
  I = point_new();
  J = point_new();

  point_set(A, w1, h1, 0.0);

  x1 = w1 + h1 * tan(alpha);
  r = h1 * tan(alpha) / cos(2 * alpha);
  l = w2 - (x1 - w1) - r;

  point_set(B, w1, -(r * cos(M_PI / 2 - 2 * alpha) + l / cos(M_PI / 2 - 2 * alpha)), 0.0);

  point_set(C, (x1 * tan(M_PI + alpha) - point_get_x(B) * tan(M_PI / 2 - 2 * alpha) + point_get_y(B)) / (tan(M_PI / 2 + alpha) - tan(M_PI / 2 - 2 * alpha)), 0.0);
  
  x2 = x1 + r;


  point_destroy(J);
  point_destroy(I);
  point_destroy(H);
  point_destroy(G);
  point_destroy(F);
  point_destroy(E);
  point_destroy(D);
  point_destroy(C);
  point_destroy(B);
  point_destroy(A);

  return 0;
}

