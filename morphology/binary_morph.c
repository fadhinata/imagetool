/*
 Copyright 2011 Hoyoung Yi.

 This program is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, please visit www.gnu.org.
*/
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

#include <bitmap.h>
#include <bytemap.h>
#include <binary_morph.h>
#include <assert.h>
#include <common.h>

void binary_dilate(bitmap_t *q, bitmap_t *p, bytemap_t *kernel)
{
  int x, y, i, j, hit, w, h, width, height;
  int xoff, yoff, qpitch, ppitch, kpitch;
  uint8_t *qbuf, *pbuf;
  int8_t *kbuf;
	
  assert(q);
  assert(p);
  assert(kernel);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));
  assert(bytemap_get_width(kernel) < bitmap_get_width(p));
  assert(bytemap_get_height(kernel) < bitmap_get_height(p));

  width = bitmap_get_width(q);
  height = bitmap_get_height(q);

  w = bytemap_get_width(kernel);
  h = bytemap_get_height(kernel);

  xoff = w >> 1;
  yoff = h >> 1;

  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);
  ppitch = bitmap_get_pitch(p) / sizeof(*pbuf);
  kpitch = bytemap_get_pitch(kernel) / sizeof(*kbuf);

  // main body	
  qbuf = bitmap_get_buffer(q) + yoff * qpitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      hit = 0;
      pbuf = bitmap_get_buffer(p) + (y - yoff) * ppitch;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          switch (*(kbuf + (j + xoff))) {
          case 0:
            if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
              hit++;
            break;
          case 1:
            if ((*(pbuf + ((x+ j) >> 3))) & (1 << ((x + j) % 8)))
              hit++;
            break;
          case -1: // don't care
            break;
          default:
            assert(0);
            break;
          }
        }
        pbuf += ppitch;
        kbuf += kpitch;
      }
      if (hit > 0) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // left top block
  qbuf = bitmap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = 0; x < xoff; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0) {
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) >= 0) {
              switch (*(kbuf + (j + xoff))) {
              case 0:
                if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
                  hit++;
                break;
              case 1:
                if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))
                  hit++;
                break;
              case -1: // don't care
                break;
              default:
                assert(0);
                break;
              }
            } else {
              // out of x range
              if ((*(kbuf + (j + xoff))) == 0) hit++;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0) hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit > 0) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // upper band
  qbuf = bitmap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0 /*&& (y+i) < p->header.height*/) {
          // within y range
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            switch (*(kbuf + (j + xoff))) {
            case 0:
              if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
                hit++;
              break;
            case 1:
              if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))
                hit++;
              break;
            case -1: // don't care
              break;
            default:
              assert(0);
              break;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0) hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit > 0) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // right top
  qbuf = bitmap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = width - (w - xoff); x < width; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0 /*&& (y+i) < p->header.height*/) {
          // within y range
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) < width) {
              switch (*(kbuf + (j + xoff))) {
              case 0:
                if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
                  hit++;
                break;
              case 1:
                if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))
                  hit++;
                break;
              case -1: // don't care
                break;
              default:
                assert(0);
                break;
              }
            } else {
              // out of x range
              if ((*(kbuf + (j + xoff))) == 0) hit++;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0) hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit > 0) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // right band
  qbuf = bitmap_get_buffer(q) + yoff * qpitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = width - (w - xoff); x < width; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      pbuf = bitmap_get_buffer(p) + (y - yoff) * ppitch;
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          if (/*(x+j) >= 0 && */(x + j) < width) {
            switch (*(kbuf + (j + xoff))) {
            case 0:
              if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))) hit++;
              break;
            case 1:
              if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))) hit++;
              break;
            case -1: // don't care
              break;
            default:
              assert(0);
              break;
            }
          } else {
            if (*(kbuf + (j + xoff)) == 0) hit++;
          }
        }
        kbuf += kpitch;
        pbuf += ppitch;
      }
      if (hit > 0) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // right bottom
  qbuf = bitmap_get_buffer(q) + yoff * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = width - (w - xoff); x < width; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) < height) {
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if (/*(x+j) >= 0 && */(x + j) < width) {
              switch (*(kbuf + (j + xoff))) {
              case 0:
                if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))) hit++;
                break;
              case 1:
                if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))) hit++;
                break;
              case -1: // don't care
                break;
              default:
                assert(0);
                break;
              }
            } else {
              if (*(kbuf + (j + xoff)) == 0) hit++;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0)
              hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit > 0) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // lower band
  qbuf = bitmap_get_buffer(q) + (height - (h - yoff)) * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if (/*(y+i) >= 0 && */(y + i) < height) {
          // within y range
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            switch (*(kbuf + (j + xoff))) {
            case 0:
              if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
                hit++;
              break;
            case 1:
              if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))
                hit++;
              break;
            case -1: // don't care
              break;
            default:
              assert(0);
              break;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0)
              hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit > 0) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // left bottom
  qbuf = bitmap_get_buffer(q) + (height - (h - yoff)) * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = 0; x < xoff; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if (/*(y+i) >= 0 && */(y + i) < height) {
          // within y range
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) >= 0) {
              switch (*(kbuf + (j + xoff))) {
              case 0:
                if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
                  hit++;
                break;
              case 1:
                if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))
                  hit++;
                break;
              case -1: // don't care
                break;
              default:
                assert(0);
                break;
              }
            } else {
              // out of x range
              if ((*(kbuf + (j + xoff))) == 0)
                hit++;
            }
          }

        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0)
              hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit > 0) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // left band
  qbuf = q->buffer+yoff*q->header.pitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = 0; x < xoff; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      pbuf = bitmap_get_buffer(p) + (y - yoff) * ppitch;
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          if ((x + j) >= 0 /*&& (x+j) < p->header.width*/) {
            switch (*(kbuf + (j + xoff))) {
            case 0:
              if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))) hit++;
              break;
            case 1:
              if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))) hit++;
              break;
            case -1: // don't care
              break;
            default:
              assert(0);
              break;
            }
          } else {
            if (*(kbuf + (j + xoff)) == 0) hit++;
          }
        }
        kbuf += kpitch;
        pbuf += ppitch;
      }
      if (hit > 0) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }
}

void binary_erode(bitmap_t *q, bitmap_t *p, bytemap_t *kernel)
{
  int x, y, i, j, hit, count, w, h, width, height;
  int xoff, yoff, qpitch, ppitch, kpitch;
  uint8_t *qbuf, *pbuf;
  int8_t *kbuf;
	
  assert(q);
  assert(p);
  assert(kernel);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));
  assert(bytemap_get_width(kernel) < bitmap_get_width(p));
  assert(bytemap_get_height(kernel) < bitmap_get_height(p));

  width = bitmap_get_width(q);
  height = bitmap_get_height(q);

  w = bytemap_get_width(kernel);
  h = bytemap_get_height(kernel);

  xoff = w >> 1;
  yoff = h >> 1;

  qpitch = bitmap_get_pitch(q) / sizeof(*qbuf);
  ppitch = bitmap_get_pitch(p) / sizeof(*pbuf);
  kpitch = bytemap_get_pitch(kernel) / sizeof(*kbuf);

  kbuf = (int8_t *)bytemap_get_buffer(kernel);
  count = 0;
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      if (*(kbuf + j) == 0 || *(kbuf + j) == 1) count++;
    }
    kbuf += kpitch;
  }

  // main body	
  qbuf = bitmap_get_buffer(q) + yoff * qpitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      hit = 0;
      pbuf = bitmap_get_buffer(p) + (y - yoff) * ppitch;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          switch (*(kbuf + (j + xoff))) {
          case 0:
            if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
              hit++;
            break;
          case 1:
            if ((*(pbuf + ((x+ j) >> 3))) & (1 << ((x + j) % 8)))
              hit++;
            break;
          case -1: // don't care
            break;
          default:
            assert(0);
            break;
          }
        }
        pbuf += ppitch;
        kbuf += kpitch;
      }
      if (hit == count) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // left top block
  qbuf = bitmap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = 0; x < xoff; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0) {
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) >= 0) {
              switch (*(kbuf + (j + xoff))) {
              case 0:
                if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
                  hit++;
                break;
              case 1:
                if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))
                  hit++;
                break;
              case -1: // don't care
                break;
              default:
                assert(0);
                break;
              }
            } else {
              // out of x range
              if ((*(kbuf + (j + xoff))) == 0) hit++;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0) hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit == count) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // upper band
  qbuf = bitmap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0 /*&& (y+i) < p->header.height*/) {
          // within y range
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            switch (*(kbuf + (j + xoff))) {
            case 0:
              if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
                hit++;
              break;
            case 1:
              if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))
                hit++;
              break;
            case -1: // don't care
              break;
            default:
              assert(0);
              break;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0) hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit == count) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // right top
  qbuf = bitmap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = width - (w - xoff); x < width; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0 /*&& (y+i) < p->header.height*/) {
          // within y range
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) < width) {
              switch (*(kbuf + (j + xoff))) {
              case 0:
                if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
                  hit++;
                break;
              case 1:
                if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))
                  hit++;
                break;
              case -1: // don't care
                break;
              default:
                assert(0);
                break;
              }
            } else {
              // out of x range
              if ((*(kbuf + (j + xoff))) == 0) hit++;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0) hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit == count) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // right band
  qbuf = bitmap_get_buffer(q) + yoff * qpitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = width - (w - xoff); x < width; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      pbuf = bitmap_get_buffer(p) + (y - yoff) * ppitch;
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          if (/*(x+j) >= 0 && */(x + j) < width) {
            switch (*(kbuf + (j + xoff))) {
            case 0:
              if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))) hit++;
              break;
            case 1:
              if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))) hit++;
              break;
            case -1: // don't care
              break;
            default:
              assert(0);
              break;
            }
          } else {
            if (*(kbuf + (j + xoff)) == 0) hit++;
          }
        }
        kbuf += kpitch;
        pbuf += ppitch;
      }
      if (hit == count) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // right bottom
  qbuf = bitmap_get_buffer(q) + yoff * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = width - (w - xoff); x < width; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) < height) {
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if (/*(x+j) >= 0 && */(x + j) < width) {
              switch (*(kbuf + (j + xoff))) {
              case 0:
                if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))) hit++;
                break;
              case 1:
                if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))) hit++;
                break;
              case -1: // don't care
                break;
              default:
                assert(0);
                break;
              }
            } else {
              if (*(kbuf + (j + xoff)) == 0) hit++;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0)
              hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit == count) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // lower band
  qbuf = bitmap_get_buffer(q) + (height - (h - yoff)) * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if (/*(y+i) >= 0 && */(y + i) < height) {
          // within y range
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            switch (*(kbuf + (j + xoff))) {
            case 0:
              if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
                hit++;
              break;
            case 1:
              if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))
                hit++;
              break;
            case -1: // don't care
              break;
            default:
              assert(0);
              break;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0)
              hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit == count) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // left bottom
  qbuf = bitmap_get_buffer(q) + (height - (h - yoff)) * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = 0; x < xoff; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if (/*(y+i) >= 0 && */(y + i) < height) {
          // within y range
          pbuf = bitmap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) >= 0) {
              switch (*(kbuf + (j + xoff))) {
              case 0:
                if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))))
                  hit++;
                break;
              case 1:
                if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))
                  hit++;
                break;
              case -1: // don't care
                break;
              default:
                assert(0);
                break;
              }
            } else {
              // out of x range
              if ((*(kbuf + (j + xoff))) == 0)
                hit++;
            }
          }

        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            if ((*(kbuf + (j + xoff))) == 0)
              hit++;
          }
        }
        kbuf += kpitch;
      }
      if (hit == count) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }

  // left band
  qbuf = q->buffer+yoff*q->header.pitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = 0; x < xoff; x++) {
      hit = 0;
      kbuf = (int8_t *)bytemap_get_buffer(kernel);
      pbuf = bitmap_get_buffer(p) + (y - yoff) * ppitch;
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          if ((x + j) >= 0 /*&& (x+j) < p->header.width*/) {
            switch (*(kbuf + (j + xoff))) {
            case 0:
              if (!((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8)))) hit++;
              break;
            case 1:
              if ((*(pbuf + ((x + j) >> 3))) & (1 << ((x + j) % 8))) hit++;
              break;
            case -1: // don't care
              break;
            default:
              assert(0);
              break;
            }
          } else {
            if (*(kbuf + (j + xoff)) == 0) hit++;
          }
        }
        kbuf += kpitch;
        pbuf += ppitch;
      }
      if (hit == count) *(qbuf + (x >> 3)) |= (1 << (x % 8));
      else *(qbuf + (x >> 3)) &= ~(1 << (x % 8));
    }
    qbuf += qpitch;
  }
}

void binary_closing(bitmap_t *q, bitmap_t *p, bytemap_t *kernel)
{
  bitmap_t *tmp;

  assert(q);
  assert(p);
  assert(kernel);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));
  assert(bytemap_get_width(kernel) < bitmap_get_width(p));
  assert(bytemap_get_height(kernel) < bitmap_get_height(p));

  tmp = bitmap_new(bitmap_get_width(p), bitmap_get_height(p));
  binary_dilate(tmp, p, kernel);
  binary_erode(q, tmp, kernel);
  bitmap_destroy(tmp);
}

void binary_opening(bitmap_t *q, bitmap_t *p, bytemap_t *kernel)
{
  bitmap_t *tmp;

  assert(q);
  assert(p);
  assert(kernel);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));
  assert(bytemap_get_width(kernel) < bitmap_get_width(p));
  assert(bytemap_get_height(kernel) < bitmap_get_height(p));

  tmp = bitmap_new(bitmap_get_width(p), bitmap_get_height(p));
  binary_erode(tmp, p, kernel);
  binary_dilate(q, tmp, kernel);
  bitmap_destroy(tmp);
}

void hit_and_miss(bitmap_t *q, bitmap_t *p, bytemap_t *hit_kernel, bytemap_t *miss_kernel)
{
  int i, j, w, h;
  int hpitch, mpitch;
  bool alloc = false;
  bitmap_t *not_p, *tmp;
  int8_t *hbuf, *mbuf;
	
  assert(q);
  assert(p);
  assert(hit_kernel);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));
  assert(bytemap_get_width(hit_kernel) < bitmap_get_width(p));
  assert(bytemap_get_height(hit_kernel) < bitmap_get_height(p));
  assert(bytemap_get_width(miss_kernel) < bitmap_get_width(p));
  assert(bytemap_get_height(miss_kernel) < bitmap_get_height(p));

  w = bytemap_get_width(hit_kernel);
  h = bytemap_get_height(hit_kernel);

  tmp = bitmap_clone(p);
  not_p = bitmap_clone(p);

  bitmap_copy_not_bitmap(not_p, p);

  if (miss_kernel == NULL) {
    miss_kernel = bytemap_clone(hit_kernel);
    hpitch = bytemap_get_pitch(hit_kernel) / sizeof(*hbuf);
    hbuf = (int8_t *)bytemap_get_buffer(hit_kernel);
    mpitch = bytemap_get_pitch(miss_kernel) / sizeof(*mbuf);
    mbuf = (int8_t *)bytemap_get_buffer(miss_kernel);
    for (i = 0; i < h; i++) {
      for (j = 0; j < w; j++) {
	switch (*(hbuf + j)) {
	case 0: *(mbuf + j) = 1; break;
	case 1: *(mbuf + j) = 0; break;
	case -1: *(mbuf + j) = -1; break;
	default: assert(0); break;
	}
      }
      hbuf += hpitch;
      mbuf += mpitch;
    }
    alloc = true;
  }

  /* p erode with hit_kernel */
  binary_erode(tmp, p, hit_kernel);
  /* not_p erode with miss_kernel */
  binary_erode(q, not_p, miss_kernel);
  bitmap_and(q, tmp);

  if (alloc) bytemap_destroy(miss_kernel);
  bitmap_destroy(not_p);
  bitmap_destroy(tmp);
}

/* filling hole nested by white region */
void region_filling(bitmap_t *q, bitmap_t *p, int xhole, int yhole)
{
  int x, y, i, j;
  int hit, not_done; //, count;
  int left, right, bottom, top;
  int new_left, new_right, new_bottom, new_top;
  bytemap_t *kernel;
  bitmap_t *not_p, *tmp1, *tmp2;
  int8_t *kbuf;

  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));
  assert(xhole >= 0 && xhole < bitmap_get_width(p));
  assert(yhole >= 0 && yhole < bitmap_get_height(p));

  // kernel setting
  kernel = bytemap_new(3, 3);
  //  count = 5;
  kbuf = (int8_t *)bytemap_get_buffer(kernel);
  *(kbuf) = -1; *(kbuf+1) = 1; *(kbuf+2) = -1; kbuf += bytemap_get_pitch(kernel);
  *(kbuf) =  1; *(kbuf+1) = 1; *(kbuf+2) =  1; kbuf += bytemap_get_pitch(kernel);
  *(kbuf) = -1; *(kbuf+1) = 1; *(kbuf+2) = -1;
	
  not_p = bitmap_clone(p);
  tmp1 = bitmap_clone(p);
  tmp2 = bitmap_clone(p);

  // start pixel for dilation
  bitmap_set_value(xhole, yhole, tmp1);

  left = max(0, xhole - 1);
  right = min(bitmap_get_width(p) - 1, xhole + 1);
  bottom = max(0, yhole - 1);
  top = min(bitmap_get_height(p) - 1, yhole + 1);

  // compliment of p
  bitmap_copy_not_bitmap(not_p, p);
  do {
    // locally dilation
    new_left = left;
    new_right = right;
    new_bottom = bottom;
    new_top = top;
    for (y = bottom; y <= top; y++) {
      for (x = left; x <= right; x++) {
	hit = 0;
	kbuf = (int8_t *)bytemap_get_buffer(kernel);
	for (i = -1; i <= 1; i++) {
	  if ((y + i) >= 0 && (y + i) < bitmap_get_height(p)) {
            for (j = -1; j <= 1; j++) {
              if ((x + j) >= 0 && (x + j) < bitmap_get_width(p)) {
                switch (*(kbuf + (j + 1))) {
                case 0:
                  if (bitmap_isreset(x + j, y + i, tmp1)) hit++;
                  break;
                case 1:
                  if (bitmap_isset(x + j, y + i, tmp1)) hit++;
                  break;
                case -1: // don't care
                  break;
                default:
                  assert(0);
                  break;
                }
              }
            }
          }
	  kbuf += bytemap_get_pitch(kernel);
	}
	if (hit > 0) {
	  bitmap_set_value(x, y, tmp2);
	  if ((x-1) < new_left) new_left = max(x-1, 0);
	  if ((x+1) > new_right) new_right = min(x+1, bitmap_get_width(p)-1);
	  if ((y-1) < new_bottom) new_bottom = max(y-1, 0);
	  if ((y+1) > new_top) new_top = min(y+1, bitmap_get_height(p)-1);
	}
      }
    }
		
    not_done = 0;
    left = new_left, right = new_right;
    bottom = new_bottom, top = new_top;
    for (y = new_bottom; y <= new_top; y++) {
      for (x = new_left; x <= new_right; x++) {
	// and operation between tmp2 and not_p
	// check equality
	if (bitmap_isset(x, y, tmp2)) {
	  if (bitmap_isset(x, y, not_p)) {
	    bitmap_set_value(x, y, tmp1);
	    if ((x-1) < left) left = max(x-1, 0);
	    if ((x+1) > right) right = min(x+1, bitmap_get_width(p)-1);
	    if ((y-1) < bottom) bottom = max(y-1, 0);
	    if ((y+1) > top) top = min(y+1, bitmap_get_height(p)-1);
	  } else { // not_p[y][x] '0'
	    bitmap_reset_value(x, y, tmp1);
	    not_done = 1;
	  }
	} else { // tmp2[y][x] '0'
	  bitmap_reset_value(x, y, tmp1);
	}
      }
    }
  } while (not_done);
	
  bitmap_copy_bitmap_or_bitmap(q, tmp1, p);
	
  bitmap_destroy(tmp2);
  bitmap_destroy(tmp1);
  bitmap_destroy(not_p);
  bytemap_destroy(kernel);
}

void morphology_boundary(bitmap_t *q, bitmap_t *p)
{
  bytemap_t *kernel;
  bitmap_t *tmp;
  int8_t *kbuf;
	
  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));
	
  // kernel create
  kernel = bytemap_new(3, 3);
  kbuf = (int8_t *)bytemap_get_buffer(kernel);
  *(kbuf) = 1; *(kbuf+1) = 1; *(kbuf+2) = 1; kbuf += bytemap_get_pitch(kernel);
  *(kbuf) = 1; *(kbuf+1) = 1; *(kbuf+2) = 1; kbuf += bytemap_get_pitch(kernel);
  *(kbuf) = 1; *(kbuf+1) = 1; *(kbuf+2) = 1;
	
  tmp = bitmap_new(bitmap_get_width(p), bitmap_get_height(p));
  // (p) intercept ( inverse ( (p) erode (kernel) ))
  binary_erode(tmp, p, kernel);
  bitmap_not(tmp);
  bitmap_copy_bitmap_and_bitmap(q, p, tmp);
	
  bitmap_destroy(tmp);
  bytemap_destroy(kernel);
}
