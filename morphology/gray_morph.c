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
#include <assert.h>

#include <gray_morph.h>

void gray_dilate(bytemap_t *q, bytemap_t *p, bytemap_t *kernel)
{
  int x, y, i, j, width, height, w, h;
  int xoff, yoff;
  int sum, max_sum;
  int qpitch, ppitch, kpitch;
  uint8_t *qbuf, *pbuf;
  uint8_t *kbuf;
	
  assert(q);
  assert(p);
  assert(kernel);
  assert(bytemap_get_width(q) == bytemap_get_width(p));
  assert(bytemap_get_height(q) == bytemap_get_height(p));
  assert(bytemap_get_width(kernel) < bytemap_get_width(p));
  assert(bytemap_get_height(kernel) < bytemap_get_height(p));

  w = bytemap_get_width(kernel);
  h = bytemap_get_height(kernel);

  width = bytemap_get_width(q);
  height = bytemap_get_height(q);

  xoff = bytemap_get_width(kernel) >> 1;
  yoff = bytemap_get_height(kernel) >> 1;

  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);
  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);
  kpitch = bytemap_get_pitch(kernel) / sizeof(*kbuf);

  // main body
  qbuf = (uint8_t *)bytemap_get_buffer(q) + yoff * qpitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      max_sum = INT_MIN;
      pbuf = (uint8_t *)bytemap_get_buffer(p) + (y - yoff) * ppitch;
      kbuf = (uint8_t *)kernel->buffer;
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
          if (sum > max_sum) max_sum = sum;
        }
        pbuf += ppitch;
        kbuf += kpitch;
      }
      *(qbuf + x) = (max_sum > UINT8_MAX ? UINT8_MAX : (max_sum < 0 ? 0 : max_sum));
    }
    qbuf += qpitch;
  }

  // left top
  qbuf = (uint8_t *)bytemap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = 0; x < xoff; x++) {
      max_sum = INT_MIN;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0/* && (y+i) < height*/) {
          // within y range
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) >= 0 /* && (x + j) < width */) {
              sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf+(j + xoff)));
              if (sum > max_sum) max_sum = sum;
            } else {
              sum = *(kbuf + (j + xoff));
              if (sum > max_sum) max_sum = sum;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum > max_sum) max_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (max_sum > UINT8_MAX ? UINT8_MAX : (max_sum < 0 ? 0 : max_sum));
    }
    qbuf += qpitch;
  }

  // upper band
  qbuf = (uint8_t *)bytemap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      max_sum = INT_MIN;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0/* && (y + i) < height*/) {
          // within y range
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
            if (sum > max_sum) max_sum = sum;
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum > max_sum) max_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (max_sum > UINT8_MAX ? UINT8_MAX : (max_sum < 0 ? 0 : max_sum));
    }
    qbuf += qpitch;
  }

  // right top
  qbuf = (uint8_t *)bytemap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = width - (w - xoff); x < width; x++) {
      max_sum = INT_MIN;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0) {
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if (/*(x+j) >= 0 && */(x+j) < width) {
              sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
              if (sum > max_sum) max_sum = sum;
            } else {
              sum = *(kbuf + (j + xoff));
              if (sum > max_sum) max_sum = sum;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum > max_sum) max_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (max_sum > UINT8_MAX ? UINT8_MAX : (max_sum < 0 ? 0 : max_sum));
    }
    qbuf += qpitch;
  }

  // right band
  qbuf = (uint8_t *)bytemap_get_buffer(q) + yoff * qpitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = width - (w - xoff); x < width; x++) {
      max_sum = INT_MIN;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      pbuf = (uint8_t *)bytemap_get_buffer(p) + (y - yoff) * ppitch;
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          if (/*(x+j) >= 0 && */(x + j) < width) {
            sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
            if (sum > max_sum) max_sum = sum;
          } else {
            sum = *(kbuf + (j + xoff));
            if (sum > max_sum) max_sum = sum;
          }
        }
        kbuf += kpitch;
        pbuf += ppitch;
      }
      *(qbuf + x) = (max_sum > UINT8_MAX ? UINT8_MAX : (max_sum < 0 ? 0 : max_sum));
    }
    qbuf += qpitch;
  }

  // right bottom
  qbuf = (uint8_t *)bytemap_get_buffer(q) + (height - (h - yoff)) * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = width - (w - xoff); x < width; x++) {
      max_sum = INT_MIN;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if (/*(y+i) >= 0 && */(y + i) < height) {
          // within y range
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) < width) {
              sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
              if (sum > max_sum) max_sum = sum;
            } else {
              sum = *(kbuf + (j + xoff));
              if (sum > max_sum) max_sum = sum;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum > max_sum) max_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (max_sum > UINT8_MAX ? UINT8_MAX : (max_sum < 0 ? 0 : max_sum));
    }
    qbuf += qpitch;
  }

  // lower band
  qbuf = (uint8_t *)bytemap_get_buffer(q) + (height - (h - yoff)) * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      max_sum = INT_MIN;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if (/*(y+i) >= 0 && */(y + i) < height) {
          // within y range
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
            if (sum > max_sum) max_sum = sum;
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum > max_sum) max_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (max_sum > UINT8_MAX ? UINT8_MAX : (max_sum < 0 ? 0 : max_sum));
    }
    qbuf += qpitch;
  }

  // left bottom
  qbuf = (uint8_t *)bytemap_get_buffer(q) + (height - (h-yoff)) * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = 0; x < xoff; x++) {
      max_sum = INT_MIN;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if (/*(y+i) >= 0 && */(y + i) < height) {
          // within y range
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) >= 0) {
              sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
              if (sum > max_sum) max_sum = sum;
            } else {
              sum = *(kbuf + (j + xoff));
              if (sum > max_sum) max_sum = sum;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum > max_sum) max_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (max_sum > UINT8_MAX ? UINT8_MAX : (max_sum < 0 ? 0 : max_sum));
    }
    qbuf += qpitch;
  }

  // left band
  qbuf = (uint8_t *)bytemap_get_buffer(q) + yoff * qpitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = 0; x < xoff; x++) {
      max_sum = INT_MIN;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      pbuf = (uint8_t *)bytemap_get_buffer(p) + (y - yoff) * ppitch;
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          if ((x + j) >= 0/* && (x+j) < p->header.width*/) {
            sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
            if (sum > max_sum) max_sum = sum;
          } else {
            sum = *(kbuf + (j + xoff));
            if (sum > max_sum) max_sum = sum;
          }
        }
        kbuf += kpitch;
        pbuf += ppitch;
      }
      *(qbuf + x) = (max_sum > UINT8_MAX ? UINT8_MAX : (max_sum < 0 ? 0 : max_sum));
    }
    qbuf += qpitch;
  }
}

void gray_erode(bytemap_t *q, bytemap_t *p, bytemap_t *kernel)
{
  int x, y, i, j, width, height, w, h;
  int xoff, yoff;
  int sum, min_sum;
  int qpitch, ppitch, kpitch;
  uint8_t *qbuf, *pbuf;
  uint8_t *kbuf;
	
  assert(q);
  assert(p);
  assert(kernel);
  assert(bytemap_get_width(q) == bytemap_get_width(p));
  assert(bytemap_get_height(q) == bytemap_get_height(p));
  assert(bytemap_get_width(kernel) < bytemap_get_width(p));
  assert(bytemap_get_height(kernel) < bytemap_get_height(p));

  w = bytemap_get_width(kernel);
  h = bytemap_get_height(kernel);

  width = bytemap_get_width(q);
  height = bytemap_get_height(q);

  xoff = bytemap_get_width(kernel) >> 1;
  yoff = bytemap_get_height(kernel) >> 1;

  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);
  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);
  kpitch = bytemap_get_pitch(kernel) / sizeof(*kbuf);

  // main body
  qbuf = (uint8_t *)bytemap_get_buffer(q) + yoff * qpitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      min_sum = INT_MAX;
      pbuf = (uint8_t *)bytemap_get_buffer(p) + (y - yoff) * ppitch;
      kbuf = (uint8_t *)kernel->buffer;
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
          if (sum < min_sum) min_sum = sum;
        }
        pbuf += ppitch;
        kbuf += kpitch;
      }
      *(qbuf + x) = (min_sum > UINT8_MAX ? UINT8_MAX : (min_sum < 0 ? 0 : min_sum));
    }
    qbuf += qpitch;
  }

  // left top
  qbuf = (uint8_t *)bytemap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = 0; x < xoff; x++) {
      min_sum = INT_MAX;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0/* && (y+i) < height*/) {
          // within y range
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) >= 0 /* && (x + j) < width */) {
              sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf+(j + xoff)));
              if (sum < min_sum) min_sum = sum;
            } else {
              sum = *(kbuf + (j + xoff));
              if (sum < min_sum) min_sum = sum;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum < min_sum) min_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (min_sum > UINT8_MAX ? UINT8_MAX : (min_sum < 0 ? 0 : min_sum));
    }
    qbuf += qpitch;
  }

  // upper band
  qbuf = (uint8_t *)bytemap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      min_sum = INT_MAX;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0/* && (y + i) < height*/) {
          // within y range
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
            if (sum < min_sum) min_sum = sum;
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum < min_sum) min_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (min_sum > UINT8_MAX ? UINT8_MAX : (min_sum < 0 ? 0 : min_sum));
    }
    qbuf += qpitch;
  }

  // right top
  qbuf = (uint8_t *)bytemap_get_buffer(q);
  for (y = 0; y < yoff; y++) {
    for (x = width - (w - xoff); x < width; x++) {
      min_sum = INT_MAX;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if ((y + i) >= 0) {
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if (/*(x+j) >= 0 && */(x+j) < width) {
              sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
              if (sum < min_sum) min_sum = sum;
            } else {
              sum = *(kbuf + (j + xoff));
              if (sum < min_sum) min_sum = sum;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum < min_sum) min_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (min_sum > UINT8_MAX ? UINT8_MAX : (min_sum < 0 ? 0 : min_sum));
    }
    qbuf += qpitch;
  }

  // right band
  qbuf = (uint8_t *)bytemap_get_buffer(q) + yoff * qpitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = width - (w - xoff); x < width; x++) {
      min_sum = INT_MAX;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      pbuf = (uint8_t *)bytemap_get_buffer(p) + (y - yoff) * ppitch;
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          if (/*(x+j) >= 0 && */(x + j) < width) {
            sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
            if (sum < min_sum) min_sum = sum;
          } else {
            sum = *(kbuf + (j + xoff));
            if (sum < min_sum) min_sum = sum;
          }
        }
        kbuf += kpitch;
        pbuf += ppitch;
      }
      *(qbuf + x) = (min_sum > UINT8_MAX ? UINT8_MAX : (min_sum < 0 ? 0 : min_sum));
    }
    qbuf += qpitch;
  }

  // right bottom
  qbuf = (uint8_t *)bytemap_get_buffer(q) + (height - (h - yoff)) * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = width - (w - xoff); x < width; x++) {
      min_sum = INT_MAX;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if (/*(y+i) >= 0 && */(y + i) < height) {
          // within y range
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) < width) {
              sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
              if (sum < min_sum) min_sum = sum;
            } else {
              sum = *(kbuf + (j + xoff));
              if (sum < min_sum) min_sum = sum;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum < min_sum) min_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (min_sum > UINT8_MAX ? UINT8_MAX : (min_sum < 0 ? 0 : min_sum));
    }
    qbuf += qpitch;
  }

  // lower band
  qbuf = (uint8_t *)bytemap_get_buffer(q) + (height - (h - yoff)) * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = xoff; x < width - (w - xoff); x++) {
      min_sum = INT_MAX;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if (/*(y+i) >= 0 && */(y + i) < height) {
          // within y range
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
            if (sum < min_sum) min_sum = sum;
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum < min_sum) min_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (min_sum > UINT8_MAX ? UINT8_MAX : (min_sum < 0 ? 0 : min_sum));
    }
    qbuf += qpitch;
  }

  // left bottom
  qbuf = (uint8_t *)bytemap_get_buffer(q) + (height - (h-yoff)) * qpitch;
  for (y = height - (h - yoff); y < height; y++) {
    for (x = 0; x < xoff; x++) {
      min_sum = INT_MAX;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      for (i = -yoff; i < h - yoff; i++) {
        if (/*(y+i) >= 0 && */(y + i) < height) {
          // within y range
          pbuf = (uint8_t *)bytemap_get_buffer(p) + (y + i) * ppitch;
          for (j = -xoff; j < w - xoff; j++) {
            if ((x + j) >= 0) {
              sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
              if (sum < min_sum) min_sum = sum;
            } else {
              sum = *(kbuf + (j + xoff));
              if (sum < min_sum) min_sum = sum;
            }
          }
        } else {
          // out of y range
          for (j = -xoff; j < w - xoff; j++) {
            sum = *(kbuf + (j + xoff));
            if (sum < min_sum) min_sum = sum;
          }
        }
        kbuf += kpitch;
      }
      *(qbuf + x) = (min_sum > UINT8_MAX ? UINT8_MAX : (min_sum < 0 ? 0 : min_sum));
    }
    qbuf += qpitch;
  }

  // left band
  qbuf = (uint8_t *)bytemap_get_buffer(q) + yoff * qpitch;
  for (y = yoff; y < height - (h - yoff); y++) {
    for (x = 0; x < xoff; x++) {
      min_sum = INT_MAX;
      kbuf = (uint8_t *)bytemap_get_buffer(kernel);
      pbuf = (uint8_t *)bytemap_get_buffer(p) + (y - yoff) * ppitch;
      for (i = -yoff; i < h - yoff; i++) {
        for (j = -xoff; j < w - xoff; j++) {
          if ((x + j) >= 0/* && (x+j) < p->header.width*/) {
            sum = (int)(*(pbuf + (x + j))) + (int)(*(kbuf + (j + xoff)));
            if (sum < min_sum) min_sum = sum;
          } else {
            sum = *(kbuf + (j + xoff));
            if (sum < min_sum) min_sum = sum;
          }
        }
        kbuf += kpitch;
        pbuf += ppitch;
      }
      *(qbuf + x) = (min_sum > UINT8_MAX ? UINT8_MAX : (min_sum < 0 ? 0 : min_sum));
    }
    qbuf += qpitch;
  }
}

void gray_opening(bytemap_t *q, bytemap_t *p, bytemap_t *kernel)
{
  bytemap_t *temp;
	
  assert(q);
  assert(p);
  assert(kernel);
  assert(bytemap_get_width(q) == bytemap_get_width(p));
  assert(bytemap_get_height(q) == bytemap_get_height(p));
  assert(bytemap_get_width(kernel) < bytemap_get_width(q));
  assert(bytemap_get_height(kernel) < bytemap_get_height(q));

  temp = bytemap_clone(p);

  gray_erode(temp, p, kernel);
  gray_dilate(q, temp, kernel);
  bytemap_destroy(temp);
}

void gray_closing(bytemap_t *q, bytemap_t *p, bytemap_t *kernel)
{
  bytemap_t *temp;
	
  assert(q);
  assert(p);
  assert(kernel);
  assert(bytemap_get_width(q) == bytemap_get_width(p));
  assert(bytemap_get_height(q) == bytemap_get_height(p));
  assert(bytemap_get_width(kernel) < bytemap_get_width(q));
  assert(bytemap_get_height(kernel) < bytemap_get_height(q));

  temp = bytemap_clone(p);

  gray_dilate(temp, p, kernel);
  gray_erode(q, temp, kernel);
  bytemap_destroy(temp);
}

/* A dark grayscale image with some small bright regions.
 * To identify those regions, apply the tophat, using a SE that is larger than the extent of the regions.
 * The opening is a Min operation that removes those bright regions that are smaller in dimension than the SE used in the opening.
 * Then, subtracting this image with the thin peaks cut off from the original image gives you just those peaks, plus some low amplitude noise.
 * The tophat is typically followed by a thresholding operation on the peaks.
 * We've just described the white tophat.
 *
 * There is a black tophat that is a dual to the white tophat, and it subtracts the original image from the closing with a SE.
 * The black tophat gives large pixel values in the result where there were small dark regions in the original image
 */

/* Difference gray and gray opening of image in size x size window */
void white_tophat(bytemap_t *q, bytemap_t *p, bytemap_t *kernel)
{
  int x, y, w, h, qpitch, ppitch;
  int diff;
  uint8_t *qbuf, *pbuf;
	
  assert(q);
  assert(p);
  assert(kernel);
  assert(bytemap_get_width(q) == bytemap_get_width(p));
  assert(bytemap_get_height(q) == bytemap_get_height(p));
  assert(bytemap_get_width(kernel) < bytemap_get_width(p));
  assert(bytemap_get_height(kernel) < bytemap_get_width(p));

  gray_opening(q, p, kernel);

  w = bytemap_get_width(q);
  h = bytemap_get_height(q);

  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);
  qbuf = (uint8_t *)bytemap_get_buffer(q);

  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);
  pbuf = (uint8_t *)bytemap_get_buffer(p);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      diff = *(pbuf + x);
      diff -= *(qbuf + x);
      *(qbuf + x) = (diff < 0 ? 0 : diff);
    }
    qbuf += qpitch;
    pbuf += ppitch;
  }
}

// valley detector
void black_tophat(bytemap_t *q, bytemap_t *p, bytemap_t *kernel)
{
  int x, y, w, h, qpitch, ppitch;
  int diff;
  uint8_t *qbuf, *pbuf;
	
  assert(q);
  assert(p);
  assert(kernel);
  assert(bytemap_get_width(q) == bytemap_get_width(p));
  assert(bytemap_get_height(q) == bytemap_get_height(p));
  assert(bytemap_get_width(kernel) < bytemap_get_width(p));
  assert(bytemap_get_height(kernel) < bytemap_get_width(p));
	
  gray_closing(q, p, kernel);

  w = bytemap_get_width(q);
  h = bytemap_get_height(q);
	
  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);
  qbuf = (uint8_t *)bytemap_get_buffer(q);

  ppitch = bytemap_get_pitch(p) / sizeof(*pbuf);
  pbuf = (uint8_t *)bytemap_get_buffer(p);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      diff = *(pbuf + x);
      diff -= *(qbuf + x);
      *(qbuf + x) = (diff < 0 ? 0 : diff);
    }
    qbuf += qpitch;
    pbuf += ppitch;
  }
}

void morphology_gradient_magnitude(bytemap_t *q, bytemap_t *p, bytemap_t *kernel)
{
  int x, y, w, h, qpitch, pitch1, pitch2;
  int diff;
  bytemap_t *erode, *dilate;
  unsigned char *qbuf, *buf1, *buf2;
	
  assert(q);
  assert(p);
  assert(kernel);
  assert(bytemap_get_width(q) == bytemap_get_width(p));
  assert(bytemap_get_height(q) == bytemap_get_height(p));
  assert(bytemap_get_width(kernel) < bytemap_get_width(p));
  assert(bytemap_get_height(kernel) < bytemap_get_width(p));
	
  erode = bytemap_clone(p);
  dilate = bytemap_clone(p);

  gray_dilate(dilate, p, kernel);	
  gray_erode(erode, p, kernel);
	
  w = bytemap_get_width(q);
  h = bytemap_get_height(q);
	
  qpitch = bytemap_get_pitch(q) / sizeof(*qbuf);
  qbuf = (uint8_t *)bytemap_get_buffer(q);

  pitch1 = bytemap_get_pitch(dilate) / sizeof(*buf1);
  buf1 = (uint8_t *)bytemap_get_buffer(dilate);

  pitch2 = bytemap_get_pitch(erode) / sizeof(*buf2);
  buf2 = (uint8_t *)bytemap_get_buffer(erode);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      diff = *(buf1 + x);
      diff -= *(buf2 + x);
      *(qbuf + x) = (diff < 0 ? 0 : diff);
    }
    qbuf += qpitch;
    buf1 += pitch1;
    buf2 += pitch2;
  }
  bytemap_destroy(erode);
  bytemap_destroy(dilate);
}

