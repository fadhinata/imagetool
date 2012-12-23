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

#include <morphology/thin.h>
#include <pixmap/bitmap.h>
#include <pixmap/bytemap.h>
#include <convert/convert.h>
#include <geometry/point_list.h>
#include <buffering/dlink.h>

/* For 8 neighborhood system of (x, y) the index of each neighbor is like this:
 *   4 3 2
 *   5 0 1
 *   6 7 8
 * 0: (x, y), 4: (x-1, y-1), 3: (x, y-1), ... 8: (x+1, y+1) */

/* Reading The Value of Pixels in 8-neighbor of 3x3 Window
 * 'x' and 'y' are the center coordinate of 3x3 window.
 * 'p' is the binary image
 * 'values' is the array of pixel values indiced by the blow hierachy.
 *   4 3 2
 *   5 0 1
 *   6 7 8                                                             */
static inline void read_3x3_from_bitmap(int value[9], int x, int y, bitmap_t *p)
{
  memset(value, 0, 9 * sizeof(int));

  value[4] = bitmap_get_value(p, x-1, y-1);
  value[3] = bitmap_get_value(p, x,   y-1);
  value[2] = bitmap_get_value(p, x+1, y-1);

  value[5] = bitmap_get_value(p, x-1, y);
  value[0] = bitmap_get_value(p, x,   y);
  value[1] = bitmap_get_value(p, x+1, y);

  value[6] = bitmap_get_value(p, x-1, y+1);
  value[7] = bitmap_get_value(p, x, y+1);
  value[8] = bitmap_get_value(p, x+1, y+1);
}

static inline void read_3x3_from_bytemap(int value[9], int x, int y, bytemap_t *p)
{
  memset(value, 0, 9 * sizeof(int));

  value[4] = bytemap_get_value(p, x-1, y-1);
  value[3] = bytemap_get_value(p, x,   y-1);
  value[2] = bytemap_get_value(p, x+1, y-1);

  value[5] = bytemap_get_value(p, x-1, y);
  value[0] = bytemap_get_value(p, x,   y);
  value[1] = bytemap_get_value(p, x+1, y);

  value[6] = bytemap_get_value(p, x-1, y+1);
  value[7] = bytemap_get_value(p, x,   y+1);
  value[8] = bytemap_get_value(p, x+1, y+1);
}

static inline void bitmap_read_3x3(int *values, int x, int y, bitmap_t *p)
{
  memset(values, 0, 9 * sizeof(int));

  if (x > 0) {
    if (y > 0) 
      values[4] = bitmap_get_value(p, x-1, y-1);
    values[5] = bitmap_get_value(p, x-1, y);
    if (y < bitmap_get_height(p)-1)
      values[6] = bitmap_get_value(p, x-1, y+1);
  }

  if (1) {
    if (y > 0)
      values[3] = bitmap_get_value(p, x, y-1);
    values[0] = bitmap_get_value(p, x, y);
    if (y < bitmap_get_height(p)-1)
      values[7] = bitmap_get_value(p, x, y+1);
  }

  if (x < bitmap_get_width(p)-1) {
    if (y > 0)
      values[2] = bitmap_get_value(p, x+1, y-1);
    values[1] = bitmap_get_value(p, x+1, y);
    if (y < bitmap_get_height(p)-1)
      values[8] = bitmap_get_value(p, x+1, y+1);
  }
}

static inline void bytemap_read_3x3(int *values, int x, int y, bytemap_t *p)
{
  memset(values, 0, 9 * sizeof(int));

  if (x > 0) {
    if (y > 0) 
      values[4] = bytemap_get_value(p, x-1, y-1);
    values[5] = bytemap_get_value(p, x-1, y);
    if (y < bytemap_get_height(p)-1)
      values[6] = bytemap_get_value(p, x-1, y+1);
  }

  if (1) {
    if (y > 0)
      values[3] = bytemap_get_value(p, x, y-1);
    values[0] = bytemap_get_value(p, x, y);
    if (y < bytemap_get_height(p)-1)
      values[7] = bytemap_get_value(p, x, y+1);
  }

  if (x < bytemap_get_width(p)-1) {
    if (y > 0)
      values[2] = bytemap_get_value(p, x+1, y-1);
    values[1] = bytemap_get_value(p, x+1, y);
    if (y < bytemap_get_height(p)-1)
      values[8] = bytemap_get_value(p, x+1, y+1);
  }
}

static inline void left_shift_3x3(int *value)
{
  value[4] = value[3]; value[3] = value[2];
  value[5] = value[0]; value[0] = value[1];
  value[6] = value[7]; value[7] = value[8];
}

static inline void right_shift_3x3(int *value)
{
  value[2] = value[3]; value[3] = value[4];
  value[1] = value[0]; value[0] = value[5];
  value[8] = value[7]; value[7] = value[6];
}

static inline void top_shift_3x3(int *value)
{
  value[4] = value[5]; value[3] = value[0]; value[2] = value[1];
  value[5] = value[6]; value[0] = value[7]; value[1] = value[8];
}

static inline void bottom_shift_3x3(int *value)
{
  value[6] = value[5]; value[7] = value[0]; value[8] = value[1];
  value[5] = value[4]; value[0] = value[3]; value[1] = value[2];
}

static inline void read_lband_3x3_from_bitmap(int value[9], int x, int y, bitmap_t *m)
{
  value[4] = bitmap_get_value(m, x-1, y-1);
  value[5] = bitmap_get_value(m, x-1, y);
  value[6] = bitmap_get_value(m, x-1, y+1);
}

static inline void read_rband_3x3_from_bitmap(int value[9], int x, int y, bitmap_t *m)
{
  value[2] = bitmap_get_value(m, x+1, y-1);
  value[1] = bitmap_get_value(m, x+1, y);
  value[8] = bitmap_get_value(m, x+1, y+1);
}

static inline void read_tband_3x3_from_bitmap(int value[9], int x, int y, bitmap_t *m)
{
  value[4] = bitmap_get_value(m, x-1, y-1);
  value[3] = bitmap_get_value(m, x, y-1);
  value[2] = bitmap_get_value(m, x+1, y-1);
}

static inline void read_bband_3x3_from_bitmap(int value[9], int x, int y, bitmap_t *m)
{
  value[6] = bitmap_get_value(m, x-1, y+1);
  value[7] = bitmap_get_value(m, x, y+1);
  value[8] = bitmap_get_value(m, x+1, y+1);
}

static inline void read_lband_3x3_from_bytemap(int value[9], int x, int y, bytemap_t *m)
{
  value[4] = bytemap_get_value(m, x-1, y-1);
  value[5] = bytemap_get_value(m, x-1, y);
  value[6] = bytemap_get_value(m, x-1, y+1);
}

static inline void read_rband_3x3_from_bytemap(int value[9], int x, int y, bytemap_t *m)
{
  value[2] = bytemap_get_value(m, x+1, y-1);
  value[1] = bytemap_get_value(m, x+1, y);
  value[8] = bytemap_get_value(m, x+1, y+1);
}

static inline void read_tband_3x3_from_bytemap(int value[9], int x, int y, bytemap_t *m)
{
  value[4] = bytemap_get_value(m, x-1, y-1);
  value[3] = bytemap_get_value(m, x, y-1);
  value[2] = bytemap_get_value(m, x+1, y-1);
}

static inline void read_bband_3x3_from_bytemap(int value[9], int x, int y, bytemap_t *m)
{
  value[6] = bytemap_get_value(m, x-1, y+1);
  value[7] = bytemap_get_value(m, x, y+1);
  value[8] = bytemap_get_value(m, x+1, y+1);
}

static inline void bytemap_read_rband_3x3(int *value, int x, int y, bytemap_t *m)
{
  value[2] = bytemap_get_value(m, x+1, y-1);
  value[1] = bytemap_get_value(m, x+1, y);
  value[8] = bytemap_get_value(m, x+1, y+1);
}

static inline void bytemap_read_tband_3x3(int *value, int x, int y, bytemap_t *m)
{
  value[4] = bytemap_get_value(m, x-1, y-1);
  value[3] = bytemap_get_value(m, x, y-1);
  value[2] = bytemap_get_value(m, x+1, y-1);
}

static inline void bytemap_read_bband_3x3(int *value, int x, int y, bytemap_t *m)
{
  value[6] = bytemap_get_value(m, x-1, y+1);
  value[7] = bytemap_get_value(m, x, y+1);
  value[8] = bytemap_get_value(m, x+1, y+1);
}

/* Reading The Value of Pixels in 24-neighbor of 5x5 Window
 * 'x' and 'y' are the center coordinate of 3x3 window.
 * 'p' is the binary image
 * 'values' is the array of pixel values indiced by the blow hierachy.
 *  15  14  13  12  11
 *  16   4   3   2  10
 *  17   5   0   1   9
 *  18   6   7   8  24
 *  19  20  21  22  23 */
static inline void read_5x5_from_bitmap(int value[9], int x, int y, bitmap_t *m)
{
  memset(value, 0, 25 * sizeof(int));

  value[15] = bitmap_get_value(m, x-2, y-2);
  value[14] = bitmap_get_value(m, x-1, y-2);
  value[13] = bitmap_get_value(m, x,   y-2);
  value[12] = bitmap_get_value(m, x+1, y-2);
  value[11] = bitmap_get_value(m, x+2, y-2);

  value[16] = bitmap_get_value(m, x-2, y-1);
  value[4]  = bitmap_get_value(m, x-1, y-1);
  value[3]  = bitmap_get_value(m, x,   y-1);
  value[2]  = bitmap_get_value(m, x+1, y-1);
  value[10] = bitmap_get_value(m, x+2, y-1);

  value[17] = bitmap_get_value(m, x-2, y);
  value[5]  = bitmap_get_value(m, x-1, y);
  value[0]  = bitmap_get_value(m, x,   y);
  value[1]  = bitmap_get_value(m, x+1, y);
  value[9]  = bitmap_get_value(m, x+2, y);

  value[18] = bitmap_get_value(m, x-2, y+1);
  value[6]  = bitmap_get_value(m, x-1, y+1);
  value[7]  = bitmap_get_value(m, x,   y+1);
  value[8]  = bitmap_get_value(m, x+1, y+1);
  value[24] = bitmap_get_value(m, x+2, y+1);

  value[19] = bitmap_get_value(m, x-2, y+2);
  value[20] = bitmap_get_value(m, x-1, y+2);
  value[21] = bitmap_get_value(m, x,   y+2);
  value[22] = bitmap_get_value(m, x+1, y+2);
  value[23] = bitmap_get_value(m, x+2, y+2);
}

static inline void read_5x5_from_bytemap(int value[9], int x, int y, bytemap_t *m)
{
  memset(value, 0, 25 * sizeof(int));

  value[15] = bytemap_get_value(m, x-2, y-2);
  value[14] = bytemap_get_value(m, x-1, y-2);
  value[13] = bytemap_get_value(m, x,   y-2);
  value[12] = bytemap_get_value(m, x+1, y-2);
  value[11] = bytemap_get_value(m, x+2, y-2);

  value[16] = bytemap_get_value(m, x-2, y-1);
  value[4]  = bytemap_get_value(m, x-1, y-1);
  value[3]  = bytemap_get_value(m, x,   y-1);
  value[2]  = bytemap_get_value(m, x+1, y-1);
  value[10] = bytemap_get_value(m, x+2, y-1);

  value[17] = bytemap_get_value(m, x-2, y);
  value[5]  = bytemap_get_value(m, x-1, y);
  value[0]  = bytemap_get_value(m, x,   y);
  value[1]  = bytemap_get_value(m, x+1, y);
  value[9]  = bytemap_get_value(m, x+2, y);

  value[18] = bytemap_get_value(m, x-2, y+1);
  value[6]  = bytemap_get_value(m, x-1, y+1);
  value[7]  = bytemap_get_value(m, x,   y+1);
  value[8]  = bytemap_get_value(m, x+1, y+1);
  value[24] = bytemap_get_value(m, x+2, y+1);

  value[19] = bytemap_get_value(m, x-2, y+2);
  value[20] = bytemap_get_value(m, x-1, y+2);
  value[21] = bytemap_get_value(m, x,   y+2);
  value[22] = bytemap_get_value(m, x+1, y+2);
  value[23] = bytemap_get_value(m, x+2, y+2);
}

static inline void bitmap_read_5x5(int *values, int x, int y, bitmap_t *p)
{
  memset(values, 0, 25*sizeof(int));

  if (x > 1) {
    if (y > 1) {
      values[15] = bitmap_get_value(p, x-2, y-2);
      values[14] = bitmap_get_value(p, x-1, y-2);
      values[16] = bitmap_get_value(p, x-2, y-1);
      values[4]  = bitmap_get_value(p, x-1, y-1);
    } else if (y > 0) {
      values[16] = bitmap_get_value(p, x-2, y-1);
      values[4]  = bitmap_get_value(p, x-1, y-1);
    }
    if (1) {
      values[17] = bitmap_get_value(p, x-2, y);
      values[5]  = bitmap_get_value(p, x-1, y);
    }
    if (y < bitmap_get_height(p)-2) {
      values[19] = bitmap_get_value(p, x-2, y+2);
      values[20] = bitmap_get_value(p, x-1, y+2);
      values[18] = bitmap_get_value(p, x-2, y+1);
      values[6]  = bitmap_get_value(p, x-1, y+1);
    } else if (y < bitmap_get_height(p)-1) {
      values[18] = bitmap_get_value(p, x-2, y+1);
      values[6]  = bitmap_get_value(p, x-1, y+1);
    }
  } else if (x > 0) {
    if (y > 1) {
      values[14] = bitmap_get_value(p, x-1, y-2);
      values[4]  = bitmap_get_value(p, x-1, y-1);
    } else if (y > 0) {
      values[4]  = bitmap_get_value(p, x-1, y-1);
    }
    if (1) {
      values[5]  = bitmap_get_value(p, x-1, y);
    }
    if (y < bitmap_get_height(p)-2) {
      values[20] = bitmap_get_value(p, x-1, y+2);
      values[6]  = bitmap_get_value(p, x-1, y+1);
    } else if (y < p->header.height-1) {
      values[6]  = bitmap_get_value(p, x-1, y+1);
    }
  }

  if (1) {
    if (y > 1) {
      values[13] = bitmap_get_value(p, x,   y-2);
      values[3]  = bitmap_get_value(p, x,   y-1);
    } else if (y > 0) {
      values[3]  = bitmap_get_value(p, x,   y-1);
    }
    if (1) {
      values[0]  = bitmap_get_value(p, x,   y);
    }
    if (y < bitmap_get_height(p)-2) {
      values[21] = bitmap_get_value(p, x,   y+2);
      values[7]  = bitmap_get_value(p, x,   y+1);
    } else if (y < p->header.height-1) {
      values[7]  = bitmap_get_value(p, x,   y+1);
    }
  }
	
  if (x < bitmap_get_width(p)-2) {
    if (y > 1) {
      values[11] = bitmap_get_value(p, x+2, y-2);
      values[12] = bitmap_get_value(p, x+1, y-2);
      values[10] = bitmap_get_value(p, x+2, y-1);
      values[2]  = bitmap_get_value(p, x+1, y-1);
    } else if (y > 0) {
      values[10] = bitmap_get_value(p, x+2, y-1);
      values[2]  = bitmap_get_value(p, x+1, y-1);
    }
    if (1) {
      values[9]  = bitmap_get_value(p, x+2, y);
      values[1]  = bitmap_get_value(p, x+1, y);
    }
    if (y < bitmap_get_height(p)-2) {
      values[23] = bitmap_get_value(p, x+2, y+2);
      values[22] = bitmap_get_value(p, x+1, y+2);
      values[24] = bitmap_get_value(p, x+2, y+1);
      values[8]  = bitmap_get_value(p, x+1, y+1);
    } else if (y < p->header.height-1) {
      values[24] = bitmap_get_value(p, x+2, y+1);
      values[8]  = bitmap_get_value(p, x+1, y+1);
    }
  } else if (x < bitmap_get_width(p)-1) {
    if (y > 1) {
      values[12] = bitmap_get_value(p, x+1, y-2);
      values[2]  = bitmap_get_value(p, x+1, y-1);
    } else if (y > 0)
      values[2]  = bitmap_get_value(p, x+1, y-1);
    if (1)
      values[1]  = bitmap_get_value(p, x+1, y);
    if (y < bitmap_get_height(p)-2) {
      values[22] = bitmap_get_value(p, x+1, y+2);
      values[8]  = bitmap_get_value(p, x+1, y+1);
    } else if (y < p->header.height-1)
      values[8]  = bitmap_get_value(p, x+1, y+1);
  }
}

static inline void bytemap_read_5x5(int *values, int x, int y, bytemap_t *p)
{
  memset(values, 0, 25*sizeof(int));

  if (x > 1) {
    if (y > 1) {
      values[15] = bytemap_get_value(p, x-2, y-2);
      values[14] = bytemap_get_value(p, x-1, y-2);
      values[16] = bytemap_get_value(p, x-2, y-1);
      values[4]  = bytemap_get_value(p, x-1, y-1);
    } else if (y > 0) {
      values[16] = bytemap_get_value(p, x-2, y-1);
      values[4]  = bytemap_get_value(p, x-1, y-1);
    }
    if (1) {
      values[17] = bytemap_get_value(p, x-2, y);
      values[5]  = bytemap_get_value(p, x-1, y);
    }
    if (y < bytemap_get_height(p)-2) {
      values[19] = bytemap_get_value(p, x-2, y+2);
      values[20] = bytemap_get_value(p, x-1, y+2);
      values[18] = bytemap_get_value(p, x-2, y+1);
      values[6]  = bytemap_get_value(p, x-1, y+1);
    } else if (y < bytemap_get_height(p)-1) {
      values[18] = bytemap_get_value(p, x-2, y+1);
      values[6]  = bytemap_get_value(p, x-1, y+1);
    }
  } else if (x > 0) {
    if (y > 1) {
      values[14] = bytemap_get_value(p, x-1, y-2);
      values[4]  = bytemap_get_value(p, x-1, y-1);
    } else if (y > 0) {
      values[4]  = bytemap_get_value(p, x-1, y-1);
    }
    if (1) {
      values[5]  = bytemap_get_value(p, x-1, y);
    }
    if (y < bytemap_get_height(p)-2) {
      values[20] = bytemap_get_value(p, x-1, y+2);
      values[6]  = bytemap_get_value(p, x-1, y+1);
    } else if (y < p->header.height-1) {
      values[6]  = bytemap_get_value(p, x-1, y+1);
    }
  }

  if (1) {
    if (y > 1) {
      values[13] = bytemap_get_value(p, x,   y-2);
      values[3]  = bytemap_get_value(p, x,   y-1);
    } else if (y > 0) {
      values[3]  = bytemap_get_value(p, x,   y-1);
    }
    if (1) {
      values[0]  = bytemap_get_value(p, x,   y);
    }
    if (y < bytemap_get_height(p)-2) {
      values[21] = bytemap_get_value(p, x,   y+2);
      values[7]  = bytemap_get_value(p, x,   y+1);
    } else if (y < p->header.height-1) {
      values[7]  = bytemap_get_value(p, x,   y+1);
    }
  }
	
  if (x < bytemap_get_width(p)-2) {
    if (y > 1) {
      values[11] = bytemap_get_value(p, x+2, y-2);
      values[12] = bytemap_get_value(p, x+1, y-2);
      values[10] = bytemap_get_value(p, x+2, y-1);
      values[2]  = bytemap_get_value(p, x+1, y-1);
    } else if (y > 0) {
      values[10] = bytemap_get_value(p, x+2, y-1);
      values[2]  = bytemap_get_value(p, x+1, y-1);
    }
    if (1) {
      values[9]  = bytemap_get_value(p, x+2, y);
      values[1]  = bytemap_get_value(p, x+1, y);
    }
    if (y < bytemap_get_height(p)-2) {
      values[23] = bytemap_get_value(p, x+2, y+2);
      values[22] = bytemap_get_value(p, x+1, y+2);
      values[24] = bytemap_get_value(p, x+2, y+1);
      values[8]  = bytemap_get_value(p, x+1, y+1);
    } else if (y < p->header.height-1) {
      values[24] = bytemap_get_value(p, x+2, y+1);
      values[8]  = bytemap_get_value(p, x+1, y+1);
    }
  } else if (x < bytemap_get_width(p)-1) {
    if (y > 1) {
      values[12] = bytemap_get_value(p, x+1, y-2);
      values[2]  = bytemap_get_value(p, x+1, y-1);
    } else if (y > 0)
      values[2]  = bytemap_get_value(p, x+1, y-1);
    if (1)
      values[1]  = bytemap_get_value(p, x+1, y);
    if (y < bytemap_get_height(p)-2) {
      values[22] = bytemap_get_value(p, x+1, y+2);
      values[8]  = bytemap_get_value(p, x+1, y+1);
    } else if (y < p->header.height-1)
      values[8]  = bytemap_get_value(p, x+1, y+1);
  }
}

static inline void left_shift_5x5(int value[25])
{
  value[15] = value[14]; value[14] = value[13]; value[13] = value[12]; value[12] = value[11];
  value[16] = value[4]; value[4] = value[3]; value[3] = value[2]; value[2] = value[10];
  value[17] = value[5]; value[5] = value[0]; value[0] = value[1]; value[1] = value[9];
  value[18] = value[6]; value[6] = value[7]; value[7] = value[8]; value[8] = value[24];
  value[19] = value[20]; value[20] = value[21]; value[21] = value[22]; value[22] = value[23];
}

static inline void right_shift_5x5(int value[25])
{
  value[11] = value[12]; value[12] = value[13]; value[13] = value[14]; value[14] = value[15];
  value[10] = value[2]; value[2] = value[3]; value[3] = value[4]; value[4] = value[16];
  value[9] = value[1]; value[1] = value[0]; value[0] = value[5]; value[5] = value[17];
  value[24] = value[8]; value[8] = value[7]; value[7] = value[6]; value[6] = value[18];
  value[23] = value[22]; value[22] = value[21]; value[21] = value[20]; value[20] = value[19];
}

static inline void top_shift_5x5(int value[25])
{
  value[15] = value[16]; value[16] = value[17]; value[17] = value[18]; value[19] = value[20];
  value[14] = value[4]; value[4] = value[5]; value[5] = value[6]; value[6] = value[20];
  value[13] = value[3]; value[3] = value[0]; value[7] = value[21];
  value[12] = value[2]; value[2] = value[1]; value[1] = value[8]; value[8] = value[22];
  value[11] = value[10]; value[10] = value[9]; value[9] = value[24]; value[24] = value[23];
}

static inline void bottom_shift_5x5(int value[9])
{
  value[19] = value[18]; value[18] = value[17]; value[17] = value[16]; value[16] = value[15];
  value[20] = value[6]; value[6] = value[5]; value[5] = value[4]; value[4] = value[14];
  value[21] = value[7]; value[7] = value[0]; value[0] = value[3]; value[3] = value[13];
  value[22] = value[8]; value[8] = value[1]; value[1] = value[2]; value[2] = value[12];
  value[23] = value[24]; value[24] = value[9]; value[9] = value[10]; value[10] = value[11];
}

static inline void read_lband_5x5_from_bitmap(int value[25], int x, int y, bitmap_t *m)
{
  value[15] = bitmap_get_value(m, x-2, y-2);
  value[16] = bitmap_get_value(m, x-2, y-1);
  value[17] = bitmap_get_value(m, x-2, y);
  value[18] = bitmap_get_value(m, x-2, y+1);
  value[19] = bitmap_get_value(m, x-2, y+2);
}

static inline void read_rband_5x5_from_bitmap(int value[25], int x, int y, bitmap_t *m)
{
  value[11] = bitmap_get_value(m, x+2, y-2);
  value[10] = bitmap_get_value(m, x+2, y-1);
  value[9] = bitmap_get_value(m, x+2, y);
  value[24] = bitmap_get_value(m, x+2, y+1);
  value[23] = bitmap_get_value(m, x+2, y+2);
}

static inline void read_tband_5x5_from_bitmap(int value[25], int x, int y, bitmap_t *m)
{
  value[15] = bitmap_get_value(m, x-2, y-2);
  value[14] = bitmap_get_value(m, x-1, y-2);
  value[13] = bitmap_get_value(m, x, y-2);
  value[12] = bitmap_get_value(m, x+1, y-2);
  value[11] = bitmap_get_value(m, x+2, y-2);
}

static inline void read_bband_5x5_from_bitmap(int value[25], int x, int y, bitmap_t *m)
{
  value[19] = bitmap_get_value(m, x-2, y+2);
  value[20] = bitmap_get_value(m, x-1, y+2);
  value[21] = bitmap_get_value(m, x, y+2);
  value[22] = bitmap_get_value(m, x+1, y+2);
  value[23] = bitmap_get_value(m, x+2, y+2);
}

static inline void read_lband_5x5_from_bytemap(int value[25], int x, int y, bytemap_t *m)
{
  value[15] = bytemap_get_value(m, x-2, y-2);
  value[16] = bytemap_get_value(m, x-2, y-1);
  value[17] = bytemap_get_value(m, x-2, y);
  value[18] = bytemap_get_value(m, x-2, y+1);
  value[19] = bytemap_get_value(m, x-2, y+2);
}

static inline void read_rband_5x5_from_bytemap(int value[25], int x, int y, bytemap_t *m)
{
  value[11] = bytemap_get_value(m, x+2, y-2);
  value[10] = bytemap_get_value(m, x+2, y-1);
  value[9] = bytemap_get_value(m, x+2, y);
  value[24] = bytemap_get_value(m, x+2, y+1);
  value[23] = bytemap_get_value(m, x+2, y+2);
}

static inline void read_tband_5x5_from_bytemap(int value[25], int x, int y, bytemap_t *m)
{
  value[15] = bytemap_get_value(m, x-2, y-2);
  value[14] = bytemap_get_value(m, x-1, y-2);
  value[13] = bytemap_get_value(m, x, y-2);
  value[12] = bytemap_get_value(m, x+1, y-2);
  value[11] = bytemap_get_value(m, x+2, y-2);
}

static inline void read_bband_5x5_from_bytemap(int value[25], int x, int y, bytemap_t *m)
{
  value[19] = bytemap_get_value(m, x-2, y+2);
  value[20] = bytemap_get_value(m, x-1, y+2);
  value[21] = bytemap_get_value(m, x, y+2);
  value[22] = bytemap_get_value(m, x+1, y+2);
  value[23] = bytemap_get_value(m, x+2, y+2);
}

/* The Number of Pixels with Given Value in 8 Neighbor of 3x3 Window
   'val' is given value for counting.
   'x' and 'y' is the center of 3x3 window.
   'p' is a binary image.
   It's return value is the number of pixels with given value */
#if 0
static int bitmap_count_value_in_8_neighbor(int val, int x, int y, bitmap_t *p)
{
  int values[9];
  int count = 0;

  bitmap_read_3x3(values, x, y, p);

  if (values[1] == val) count++;
  if (values[2] == val) count++;
  if (values[3] == val) count++;
  if (values[4] == val) count++;
  if (values[5] == val) count++;
  if (values[6] == val) count++;
  if (values[7] == val) count++;
  if (values[8] == val) count++;

  return count;
}

static int bytemap_count_value_in_8_neighbor(int val, int x, int y, bytemap_t *p)
{
  int values[9];
  int count = 0;

  bytemap_read_3x3(values, x, y, p);

  if (values[1] == val) count++;
  if (values[2] == val) count++;
  if (values[3] == val) count++;
  if (values[4] == val) count++;
  if (values[5] == val) count++;
  if (values[6] == val) count++;
  if (values[7] == val) count++;
  if (values[8] == val) count++;

  return count;
}

/* Yokoi's connectivitiy number
 * Argument 'val' is objective pixel's value.
 * It computes the connectivity in 3x3 window centered by 'x' and 'y'.
 * Return value has six cases like this;
 * 0: isolated, 1: edge, 2: connecting, 3: branching, 4: crossing, 5: interior */
static int bitmap_yokoi_connectivity_number(int x, int y, bitmap_t *p)
{
  int values[9];
  int i, num = 0;

  bitmap_read_3x3(values, x, y, p);

  for (i = 1; i <= 7; i += 2) {
    num += values[i] - values[i] * values[(i+1)%9] * values[(i+2)%9];
  }

  return num;
}
#endif

/* This Pre-smoothing before stentiford's thinning   *
 * The value of objective pixel is "1".              *
 * Pixels with others are considered as background.  */
void stentiford_pre_smooth_for_thinning(bitmap_t *q, bitmap_t *p)
{
  int x, y, w, h;
  int i, i1, i2, values[9];
  int ones, connectivity;
  bytemap_t *temp;
	
  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  w = bitmap_get_width(q);
  h = bitmap_get_height(q);

  temp = bytemap_new(w, h);
  bitmap2bytemap(temp, p);

  bitmap_clear(q);
  for (y = 1; y < h-1; y++) {
    read_3x3_from_bytemap(values, 1, y, temp);
    for (x = 1; x < w-1; x++) {
      read_rband_3x3_from_bytemap(values, x, y, temp);
      if (values[0] == 1) {
	// counting pixel with one value and exclude pixels more than 3-ones
	for (ones = 0, i = 1; i < 9; i++)
	  if (values[i] == 1) ones++;
	if (ones < 3) {
	  // yokoi connectivity number
	  for (connectivity = 0, i = 1; i <= 7; i += 2) {
	    i1 = i+1; if (i1 > 8) i1 -= 8;
	    i2 = i+2; if (i2 > 8) i2 -= 8;
	    connectivity += (values[i] != 1) - (values[i] != 1) * (values[i1] != 1) * (values[i2] != 1);
	  }
	  //2:connecting, 3:branching, 4:crossing, and 5:interior it is excluded
	  if (connectivity < 2)
	    bytemap_put_value(2, temp, x, y);
	}
      }
      left_shift_3x3(values);
    }
  }
  for (y = 0; y < q->header.height; y++) {
    for (x = 0; x < q->header.width; x++) {
      if (bytemap_get_value(temp, x, y) == 2)
	bytemap_put_value(0, temp, x, y);
    }
  }
  bytemap2bitmap(q, temp);
  bytemap_destroy(temp);
}

#define MATCH_DU_D1(v)							\
  (v[15] == 1 && v[14] == 1 && v[13] == 0 && v[12] == 1 && v[11] == 1 && \
   v[16] == 1 && v[4]  == 1 && v[3]  == 0 && v[2]  == 1 && v[10] == 1 && \
   v[17] == 1 && v[5]  == 1 && v[0]  == 1 && v[1]  == 1 && v[9]  == 1 && \
   v[18] == 1 && v[6]  == 1 && v[7]  == 1 && v[8]  == 1 && v[24] == 1 && \
                 v[20] == 1 && v[21] == 1 && v[22] == 1)
	
#define MATCH_DU_D2(v)							\
  (v[15] == 1 && v[14] == 0 && v[13] == 0 && v[12] == 1 && v[11] == 1 && \
   v[16] == 1 && v[4]  == 1 && v[3]  == 0 && v[2]  == 1 && v[10] == 1 && \
   v[17] == 1 && v[5]  == 1 && v[0]  == 1 && v[1]  == 1 && v[9]  == 1 && \
   v[18] == 1 && v[6]  == 1 && v[7]  == 1 && v[8]  == 1 && v[24] == 1 && \
                 v[20] == 1 && v[21] == 1 && v[22] == 1)

#define MATCH_DU_D3(v)							\
  (v[15] == 1 && v[14] == 1 && v[13] == 0 && v[12] == 0 && v[11] == 1 && \
   v[16] == 1 && v[4]  == 1 && v[3]  == 0 && v[2]  == 1 && v[10] == 1 && \
   v[17] == 1 && v[5]  == 1 && v[0]  == 1 && v[1]  == 1 && v[9]  == 1 && \
   v[18] == 1 && v[6]  == 1 && v[7]  == 1 && v[8]  == 1 && v[24] == 1 && \
                 v[20] == 1 && v[21] == 1 && v[22] == 1)
	
#define MATCH_DU_D4(v)			\
  (v[15] == 1 && v[14] == 0 && v[13] == 0 && v[12] == 1 && v[11] == 1 && \
   v[16] == 1 && v[4]  == 0 && v[3]  == 0 && v[2]  == 1 && v[10] == 1 && \
   v[17] == 1 && v[5]  == 1 && v[0]  == 1 && v[1]  == 1 && v[9]  == 1 && \
   v[18] == 1 && v[6]  == 1 && v[7]  == 1 && v[8]  == 1 && v[24] == 1 && \
                 v[20] == 1 && v[21] == 1 && v[22] == 1)

#define MATCH_DU_D5(v)							\
  (v[15] == 1 && v[14] == 1 && v[13] == 0 && v[12] == 0 && v[11] == 1 && \
   v[16] == 1 && v[4]  == 1 && v[3]  == 0 && v[2]  == 0 && v[10] == 1 && \
   v[17] == 1 && v[5]  == 1 && v[0]  == 1 && v[1]  == 1 && v[9]  == 1 && \
   v[18] == 1 && v[6]  == 1 && v[7]  == 1 && v[8]  == 1 && v[24] == 1 && \
                 v[20] == 1 && v[21] == 1 && v[22] == 1)

#define MATCH_DU_U1(v)							\
  (v[19] == 1 && v[20] == 1 && v[21] == 0 && v[22] == 1 && v[23] == 1 && \
   v[18] == 1 && v[6]  == 1 && v[7]  == 0 && v[8]  == 1 && v[24] == 1 && \
   v[17] == 1 && v[5]  == 1 && v[0]  == 1 && v[1]  == 1 && v[9]  == 1 && \
   v[16] == 1 && v[4]  == 1 && v[3]  == 1 && v[2]  == 1 && v[10] == 1 && \
                 v[14] == 1 && v[13] == 1 && v[12] == 1)

#define MATCH_DU_U2(v)							\
  (v[19] == 1 && v[20] == 0 && v[21] == 0 && v[22] == 1 && v[23] == 1 && \
   v[18] == 1 && v[6]  == 1 && v[7]  == 0 && v[8]  == 1 && v[24] == 1 && \
   v[17] == 1 && v[5]  == 1 && v[0]  == 1 && v[1]  == 1 && v[9]  == 1 && \
   v[16] == 1 && v[4]  == 1 && v[3]  == 1 && v[2]  == 1 && v[10] == 1 && \
                 v[14] == 1 && v[13] == 1 && v[12] == 1)

#define MATCH_DU_U3(v)							\
  (v[19] == 1 && v[20] == 1 && v[21] == 0 && v[22] == 0 && v[23] == 1 && \
   v[18] == 1 && v[6]  == 1 && v[7]  == 0 && v[8]  == 1 && v[24] == 1 && \
   v[17] == 1 && v[5]  == 1 && v[0]  == 1 && v[1]  == 1 && v[9]  == 1 && \
   v[16] == 1 && v[4]  == 1 && v[3]  == 1 && v[2]  == 1 && v[10] == 1 && \
                 v[14] == 1 && v[13] == 1 && v[12] == 1)

#define MATCH_DU_U4(v)							\
  (v[19] == 1 && v[20] == 0 && v[21] == 0 && v[22] == 1 && v[23] == 1 && \
   v[18] == 1 && v[6]  == 0 && v[7]  == 0 && v[8]  == 1 && v[24] == 1 && \
   v[17] == 1 && v[5]  == 1 && v[0]  == 1 && v[1]  == 1 && v[9]  == 1 && \
   v[16] == 1 && v[4]  == 1 && v[3]  == 1 && v[2]  == 1 && v[10] == 1 && \
                 v[14] == 1 && v[13] == 1 && v[12] == 1)
	
#define MATCH_DU_U5(v)							\
  (v[19] == 1 && v[20] == 1 && v[21] == 0 && v[22] == 0 && v[23] == 1 && \
   v[18] == 1 && v[6]  == 1 && v[7]  == 0 && v[8]  == 0 && v[24] == 1 && \
   v[17] == 1 && v[5]  == 1 && v[0]  == 1 && v[1]  == 1 && v[9]  == 1 && \
   v[16] == 1 && v[4]  == 1 && v[3]  == 1 && v[2]  == 1 && v[10] == 1 && \
                 v[14] == 1 && v[13] == 1 && v[12] == 1)

void stentiford_acute_angle_emphasis_for_thinning(bitmap_t *q, bitmap_t *p)
{
  int x, y, k, w, h;
  int again;
  int values[25];
  bytemap_t *del;
	
  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  w = bitmap_get_width(q);
  h = bitmap_get_height(q);

  del = bytemap_new(w, h);
  bitmap2bytemap(del, p);

  for (k = 5; k >= 1; k -= 2) {
    for (y = 2; y < h-2; y++) {
      read_5x5_from_bytemap(values, 2, y, del);
      for (x = 2; x < w-2; x++) {
	read_rband_5x5_from_bytemap(values, x, y, del);
	if (values[0] == 1) {
	  if (MATCH_DU_D1(values))
	    bytemap_put_value(2, del, x, y);
	  else if (k >= 2 && MATCH_DU_D2(values))
	    bytemap_put_value(2, del, x, y);
	  else if (k >= 3 && MATCH_DU_D3(values))
	    bytemap_put_value(2, del, x, y);
	  else if (k >= 4 && MATCH_DU_D4(values))
	    bytemap_put_value(2, del, x, y);
	  else if (k >= 5 && MATCH_DU_D5(values))
	    bytemap_put_value(2, del, x, y);

	  if (MATCH_DU_U1(values))
	    bytemap_put_value(2, del, x, y);
	  else if (k >= 2 && MATCH_DU_U2(values))
	    bytemap_put_value(2, del, x, y);
	  else if (k >= 3 && MATCH_DU_U3(values))
	    bytemap_put_value(2, del, x, y);
	  else if (k >= 4 && MATCH_DU_U4(values))
	    bytemap_put_value(2, del, x, y);
	  else if (k >= 5 && MATCH_DU_U5(values))
	    bytemap_put_value(2, del, x, y);
	}
	left_shift_5x5(values);
      }
    }
    again = 0;
    for (y = 0; y < h; y++) {
      for (x = 0; x < w; x++) {
	if (bytemap_get_value(del, x, y) == 2) {
	  bytemap_put_value(0, del, x, y);
	  again = 1;
	}
      }
    }
    if (again == 0) break;
  }
  bytemap2bitmap(q, del);
  bytemap_destroy(del);
}

// Template M1 - scan top -> bottom, left -> right
static void stentiford_M1_templating(bytemap_t *del)
{
  int x, y, w, h;
  int i, i1, i2;
  int values[9], ones, connectivity;

  w = bytemap_get_width(del);
  h = bytemap_get_height(del);

  for (y = 1; y < h-1; y++) {
    read_3x3_from_bytemap(values, 1, y, del);
    for (x = 1; x < w-1; x++) {
      read_rband_3x3_from_bytemap(values, x, y, del);
      /* X  0  X
	 X  1  X
	 X !0  X */
      if (values[3] == 0 && values[0] == 1 && values[7] != 0) {
	for (ones = 0, i = 1; i < 9; i++)
	  if (values[i] == 1) ones++;
	if (ones != 1) {
	  // edge
	  for (connectivity = 0, i = 1; i <= 7; i += 2) {
	    i1 = i+1; if (i1 > 8) i1 -= 8;
	    i2 = i+2; if (i2 > 8) i2 -= 8;
	    connectivity += (values[i] != 1) - (values[i] != 1) * (values[i1] != 1) * (values[i2] != 1);
	  }
	  if (connectivity == 1)
	    bytemap_put_value(2, del, x, y);
	}
      }
      left_shift_3x3(values);
    }
  }
}

// Template M2 - scan bottom -> top, left -> right
static void stentiford_M2_templating(bytemap_t *del)
{
  int x, y, w, h;
  int i, i1, i2;
  int values[9], ones, connectivity;

  w = bytemap_get_width(del);
  h = bytemap_get_height(del);

  for (x = 1; x < w-1; x++) {
    read_3x3_from_bytemap(values, x, h-2, del);
    for (y = h-2; y >= 1; y--) {
      read_tband_3x3_from_bytemap(values, x, y, del);
      /* X  X  X
	 0  1 !0
	 X  X  X */
      if (values[5] == 0 && values[0] == 1 && values[1] != 0) {
	for (ones = 0, i = 1; i < 9; i++)
	  if (values[i] == 1) ones++;
	if (ones != 1) {
	  // edge
	  for (connectivity = 0, i = 1; i <= 7; i += 2) {
	    i1 = i+1; if (i1 > 8) i1 -= 8;
	    i2 = i+2; if (i2 > 8) i2 -= 8;
	    connectivity += (values[i] != 1) - (values[i] != 1) * (values[i1] != 1) * (values[i2] != 1);
	  }
	  if (connectivity == 1)
	    bytemap_put_value(2, del, x, y);
	}
      }
      bottom_shift_3x3(values);
    }
  }
}

// Template M3 - scan right -> left, bottom -> top
static void stentiford_M3_templating(bytemap_t *del)
{
  int x, y, w, h;
  int i, i1, i2;
  int values[9], ones, connectivity;

  w = bytemap_get_width(del);
  h = bytemap_get_height(del);

  for (y = h-2; y >= 1; y--) {
    read_3x3_from_bytemap(values, w-2, y, del);
    for (x = w-2; x >= 1; x--) {
      read_lband_3x3_from_bytemap(values, x, y, del);
      /* X !0  X
	 X  1  X
	 X  0  X */
      if (values[3] != 0 && values[0] == 1 && values[7] == 0) {
	for (ones = 0, i = 1; i < 9; i++)
	  if (values[i] == 1) ones++;
	if (ones != 1) {
	  // edge
	  for (connectivity = 0, i = 1; i <= 7; i += 2) {
	    i1 = i+1; if (i1 > 8) i1 -= 8;
	    i2 = i+2; if (i2 > 8) i2 -= 8;
	    connectivity += (values[i] != 1) - (values[i] != 1) * (values[i1] != 1) * (values[i2] != 1);
	  }
	  if (connectivity == 1)
	    bytemap_put_value(2, del, x, y);
	}
      }
      right_shift_3x3(values);
    }
  }
}

// Template M4 
static void stentiford_M4_templating(bytemap_t *del)
{
  int x, y, w, h;
  int i, i1, i2;
  int values[9], ones, connectivity;

  w = bytemap_get_width(del);
  h = bytemap_get_height(del);

  for (x = w-2; x >= 1; x--) {
    read_3x3_from_bytemap(values, x, 1, del);
    for (y = 1; y < h-1; y++) {
      read_bband_3x3_from_bytemap(values, x, y, del);
      /* X  X  X
	!0  1  0
	 X  X  X */
      if (values[5] != 0 && values[0] == 1 && values[1] == 0) {
	// end point
	for (ones = 0, i = 1; i < 9; i++)
	  if (values[i] == 1) ones++;
	if (ones != 1) {
	  // edge
	  for (connectivity = 0, i = 1; i <= 7; i += 2) {
	    i1 = i+1; if (i1 > 8) i1 -= 8;
	    i2 = i+2; if (i2 > 8) i2 -= 8;
	    connectivity += (values[i] != 1) - (values[i] != 1) * (values[i1] != 1) * (values[i2] != 1);
	  }
	  if (connectivity == 1)
	    bytemap_put_value(2, del, x, y);
	}
      }
      top_shift_3x3(values);
    }
  }
}

void stentiford_thinning(bitmap_t *q, bitmap_t *p)
{
  int x, y, w, h;
  int not_done;
  bytemap_t *del;
  //  int i, i1, i2, values[9], ones, connectivity;
	
  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  w = bitmap_get_width(p);
  h = bitmap_get_height(p);

  del = bytemap_new(w, h);
  bitmap2bytemap(del, p);

  not_done = 1;
  while (not_done) {
    stentiford_M1_templating(del);
    stentiford_M2_templating(del);
    stentiford_M3_templating(del);
    stentiford_M4_templating(del);
    not_done = 0;
    /* Delete pixels that are marked (== 2) */
    for (y = 0; y < bytemap_get_height(del); y++) {
      for (x = 0; x < bytemap_get_width(del); x++) {
	if (bytemap_get_value(del, x, y) == 2) {
	  bytemap_put_value(0, del, x, y);
	  not_done = 1;
	}
      }
    }
  }
  bytemap2bitmap(q, del);
  bytemap_destroy(del);
}

/* Zhang & Suen's thinning algorithm */
void zhang_suen_thinning(bitmap_t *q, bitmap_t *p)
{
  int x, y, i, w, h;
  int not_done, values[9], ones, connectivity;
  bitmap_t *del;
  bytemap_t *temp;
	
  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  w = bitmap_get_width(q);
  h = bitmap_get_height(q);
  del = bitmap_new(w, h);

  temp = bytemap_new(w, h);
  bitmap2bytemap(temp, p);

  not_done = 1;
  while (not_done) {
    // Second sub-iteration
    for (y = 1; y < h-1; y++) {
      read_3x3_from_bytemap(values, 1, y, temp);
      for (x = 1; x < w-1; x++) {
	read_rband_3x3_from_bytemap(values, x, y, temp);
	if (values[0] == 1) {
	  for (ones = 0, i = 1; i < 9; i++)
	    if (values[i] == 1) ones++;
	  if (ones >= 2 && ones <= 6) {
	    connectivity = 0;
	    if (values[1] == 1 && values[2] == 0) connectivity++;
	    if (values[2] == 1 && values[3] == 0) connectivity++;
	    if (values[3] == 1 && values[4] == 0) connectivity++;
	    if (values[4] == 1 && values[5] == 0) connectivity++;
	    if (values[5] == 1 && values[6] == 0) connectivity++;
	    if (values[6] == 1 && values[7] == 0) connectivity++;
	    if (values[7] == 1 && values[8] == 0) connectivity++;
	    if (values[8] == 1 && values[1] == 0) connectivity++;
	    if (connectivity == 1) {
	      if (values[1]*values[3]*values[5] == 0 &&
		  values[3]*values[7]*values[5] == 0) {
		bitmap_set_value(del, x, y);
	      }
	    }
	  }
	}
	left_shift_3x3(values);
      }
    }
    not_done = 0;
    // delete the marked pixels
    for (y = 0; y < h; y++) {
      for (x = 0; x < w; x++) {
	if (bitmap_isset(del, x, y)) {
	  bitmap_reset_value(del, x, y);
	  bytemap_put_value(0, temp, x, y);
	  not_done = 1;
	}
      }
    }
    if (not_done == 0) break;
    // First sub-iteration
    for (y = 1; y < h-1; y++) {
      read_3x3_from_bytemap(values, 1, y, temp);
      for (x = 1; x < w-1; x++) {
	read_rband_3x3_from_bytemap(values, x, y, temp);
	if (values[0] == 1) {
	  for (ones = 0, i = 1; i < 9; i++)
	    if (values[i] == 1) ones++;
	  if (ones >= 2 && ones <= 6) {
	    connectivity = 0;
	    if (values[1] == 1 && values[2] == 0) connectivity++;
	    if (values[2] == 1 && values[3] == 0) connectivity++;
	    if (values[3] == 1 && values[4] == 0) connectivity++;
	    if (values[4] == 1 && values[5] == 0) connectivity++;
	    if (values[5] == 1 && values[6] == 0) connectivity++;
	    if (values[6] == 1 && values[7] == 0) connectivity++;
	    if (values[7] == 1 && values[8] == 0) connectivity++;
	    if (values[8] == 1 && values[1] == 0) connectivity++;
	    if (connectivity == 1) {
	      if (values[3]*values[1]*values[7] == 0 &&
		  values[1]*values[7]*values[5] == 0) {
		bitmap_set_value(del, x, y);
	      }
	    }
	  }
	}
	left_shift_3x3(values);
      }
    }
    // delete the marked pixels
    not_done = 0;
    for (y = 0; y < h; y++) {
      for (x = 0; x < w; x++) {
	if (bitmap_isset(del, x, y)) {
	  not_done = 1;
	  bitmap_reset_value(del, x, y);
	  bytemap_put_value(0, temp, x, y);
	}
      }
    }
  }
  bytemap2bitmap(q, temp);
  bytemap_destroy(temp);
  bitmap_destroy(del);
}

static int t00, t01, t01s, t11;

// all '0'
// at least two '1'
// at least one '1'
#define check(v1, v2, v3) {				\
    if ((v2 == 0) && ((v1 == 0) || (v3 == 0))) t00 = 1; \
    if (v2 && (v1 || v3)) t11 = 1;			\
    if (((v1 == 0) && v2) || ((v2 == 0) && v3)) {	\
      t01s = t01;					\
      t01 = 1;						\
    }							\
  }

static inline int edge_3x3(int value[9])
{
  if (value[0] == 0) return 0;
  t00 = 0; t01 = 0; t01s = 0; t11 = 0;

  check(value[4], value[3], value[2]);
  check(value[2], value[1], value[8]);
  check(value[8], value[7], value[6]);
  check(value[6], value[5], value[4]);

  return t00 && t11 && (t01s == 0);
}

static inline int edge_5x5(int xoff, int yoff, int values[25])
{
  int v[9];

  if (xoff < 0 && yoff < 0) {
    v[4] = values[15]; v[3] = values[14]; v[2] = values[13];
    v[5] = values[16]; v[0] = values[4]; v[1] = values[3];
    v[6] = values[17]; v[7] = values[5]; v[8] = values[0];
  } else if (xoff == 0 && yoff < 0) {
    v[4] = values[14]; v[3] = values[13]; v[2] = values[12];
    v[5] = values[4]; v[0] = values[3]; v[1] = values[2];
    v[6] = values[5]; v[7] = values[0]; v[8] = values[1];
  } else if (xoff > 0 && yoff < 0) {
    v[4] = values[13]; v[3] = values[12]; v[2] = values[11];
    v[5] = values[3]; v[0] = values[2]; v[1] = values[10];
    v[6] = values[0]; v[7] = values[1]; v[8] = values[9];
  } else if (xoff < 0 && yoff == 0) {
    v[4] = values[16]; v[3] = values[4]; v[2] = values[3];
    v[5] = values[17]; v[0] = values[5]; v[1] = values[0];
    v[6] = values[18]; v[7] = values[6]; v[8] = values[7];
  } else if (xoff == 0 && yoff == 0) {
    v[4] = values[4]; v[3] = values[3]; v[2] = values[2];
    v[5] = values[5]; v[0] = values[0]; v[1] = values[1];
    v[6] = values[6]; v[7] = values[7]; v[8] = values[8];
  } else if (xoff > 0 && yoff == 0) {
    v[4] = values[3]; v[3] = values[2]; v[2] = values[10];
    v[5] = values[0]; v[0] = values[1]; v[1] = values[9];
    v[6] = values[7]; v[7] = values[8]; v[8] = values[24];
  } else if (xoff < 0 && yoff > 0) {
    v[4] = values[17]; v[3] = values[5]; v[2] = values[0];
    v[5] = values[18]; v[0] = values[6]; v[1] = values[7];
    v[6] = values[19]; v[7] = values[20]; v[8] = values[21];
  } else if (xoff == 0 && yoff > 0) {
    v[4] = values[5]; v[3] = values[0]; v[2] = values[1];
    v[5] = values[6]; v[0] = values[7]; v[1] = values[8];
    v[6] = values[20]; v[7] = values[21]; v[8] = values[22];
  } else if (xoff > 0 && yoff > 0) {
    v[4] = values[0]; v[3] = values[1]; v[2] = values[9];
    v[5] = values[7]; v[0] = values[8]; v[1] = values[24];
    v[6] = values[21]; v[7] = values[22]; v[8] = values[23];
  }

  return edge_3x3(v);
}

// CACM Feb 1987 Vol 20 No1 Holt et al
void holt_et_al_thinning(bitmap_t *q, bitmap_t *p)
{
  int x, y, k, w, h;
  int again = 1;
  bytemap_t *temp;
  int val, value[25];

  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  w = bitmap_get_width(q);
  h = bitmap_get_height(q);

  temp = bytemap_new(w, h);
  bitmap2bytemap(temp, p);

  while (again) {
    again = 0;
    // Sub-iteration 1:
    for (y = 2; y < h-2; y++) {
      read_5x5_from_bytemap(value, 2, y, temp);
      for (x = 2; x < w-2; x++) {
	read_rband_5x5_from_bytemap(value, x, y, temp);
	if (value[0] != 0) {
	  k = (!edge_5x5(0, 0, value) ||
	       (edge_5x5(1, 0, value) && value[3] && value[7]) ||
	       (edge_5x5(0, 1, value) && value[5] && value[1]) ||
	       (edge_5x5(1, 0, value) && edge_5x5(1, 1, value) && edge_5x5(0, 1, value)));
	  if (!k) {
	    edge_5x5(0, 0, value);
	    if (t01) bytemap_put_value(value[0] | 4, temp, x, y);
	    bytemap_put_value(value[0] | 2, temp, x, y);
	    again = 1;
	  }
	}
	left_shift_5x5(value);
      }
    }
    for (y = 0; y < h; y++) {
      for (x = 0; x < w; x++) {
	if (bytemap_get_value(temp, x, y) & 0x02)
	  bytemap_put_value(0, temp, x, y);
      }
    }
    // Sub-iteration 2:
    for (y = 2; y < h-2; y++) {
      read_5x5_from_bytemap(value, 2, y, temp);
      for (x = 2; x < w-2; x++) {
	read_rband_5x5_from_bytemap(value, x, y, temp);
	if (value[0] != 0) {
	  k = (!edge_5x5(0, 0, value) ||
	       (value[0] & 0x04) == 0 ||
	       (edge_5x5(0, 1, value) && edge_5x5(-1, 0, value) && edge_5x5(1, 0, value)) ||
	       (edge_5x5(1, 0, value) && edge_5x5(0, -1, value) && edge_5x5(0, 1, value)) ||
	       (edge_5x5(0, 1, value) && edge_5x5(1, 0, value) && edge_5x5(1, 1, value)));
	  if (!k) {
	    bytemap_put_value(value[0] | 0x02, temp, x, y);
	    again = 1;
	  }
	}
	left_shift_5x5(value);
      }
    }
    for (y = 0; y < h; y++) {
      for (x = 0; x < w; x++) {
	val = bytemap_get_value(temp, x, y);
	if (val & 0x02)
	  bytemap_put_value(0, temp, x, y);
	else if (val > 0)
	  bytemap_put_value(1, temp, x, y);
      }
    }
  }
  // Staircase removal; northward bias
  for (y = 1; y < h-1; y++) {
    read_3x3_from_bytemap(value, 1, y, temp);
    for (x = 1; x < w-1; x++) {
      read_rband_3x3_from_bytemap(value, x, y, temp);
      if (value[0] != 0) {
	k = !(value[3] &&
	      ((value[1] && !value[2] && !value[6] && (!value[5] || !value[7])) ||
	       (value[5] && !value[4] && !value[8] && (!value[1] || !value[7]))));
	if (!k) {
	  bytemap_put_value(val | 0x02, temp, x, y);
	}
      }
      left_shift_3x3(value);
    }
  }
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      val = bytemap_get_value(temp, x, y);
      if (val & 0x02) bytemap_put_value(0, temp, x, y);
      else if (val > 0) bytemap_put_value(1, temp, x, y);
    }
  }
  // Southward bias
  for (y = 1; y < h-1; y++) {
    read_3x3_from_bytemap(value, 1, y, temp);
    for (x = 1; x < w-1; x++) {
      read_rband_3x3_from_bytemap(value, x, y, temp);
      if (value[0] != 0) {
	k = !(value[7] &&
	      ((value[1] && !value[8] && !value[4] && (!value[5] || !value[3])) ||
	       (value[5] && !value[6] && !value[2] && (!value[1] || !value[3]))));
	if (!k) {
	  bytemap_put_value(val | 0x02, temp, x, y);
	}
      }
      left_shift_3x3(value);
    }
  }
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      val = bytemap_get_value(temp, x, y);
      if (val & 0x02) bytemap_put_value(0, temp, x, y);
      else if (val > 0) bytemap_put_value(1, temp, x, y);
    }
  }
  bytemap2bitmap(q, temp);
  bytemap_destroy(temp);
}

static inline void zhangsuen_holt_edge_finding(bitmap_t *edge, bitmap_t *q)
{
  int x, y, w, h;
  int values[9];
#if 1
  w = bitmap_get_width(q);
  h = bitmap_get_height(q);

  for (y = 1; y < h-1; y++) {
    read_3x3_from_bitmap(values, 1, y, q);
    for (x = 1; x < w-1; x++) {
      read_rband_3x3_from_bitmap(values, x, y, q);
      if (edge_3x3(values)) bitmap_set_value(edge, x, y);
      left_shift_3x3(values);
    }
  }
#else
  /* edge finding */
  // left-bottom
  x = 0;
  y = 0;
  values[4] = 0; values[3] = 0; values[2] = 0;
  values[5] = 0; values[0] = bitmap_get_value(x, y, q); values[1] = bitmap_get_value(x+1, y, q);
  values[6] = 0; values[7] = bitmap_get_value(x, y+1, q); values[8] = bitmap_get_value(x+1, y+1, q);
  if (edge_3x3(values)) bitmap_set_value(x, y, edge);
  x++;
  // bottom-band
  for (; x < q->header.width-1; x++) {
    /* values[4] = 0; values[3] = 0; values[2] = 0; */
    values[5] = values[0]; values[0] = values[1]; values[1] = bitmap_get_value(x+1, y, q);
    values[6] = values[7]; values[7] = values[8]; values[8] = bitmap_get_value(x+1, y+1, q);
    if (edge_3x3(values)) bitmap_set_value(x, y, edge);
  }
  // right-bottom
  /* values[4] = 0; values[3] = 0; values[2] = 0; */
  values[5] = values[0]; values[0] = values[1]; values[1] = 0;
  values[6] = values[7]; values[7] = values[8]; values[8] = 0;
  if (edge_3x3(values)) bitmap_set_value(x, y, edge);
  y++;
  // body
  for (; y < q->header.height-1; y++) {
    // left-band
    x = 0;
    values[4] = bitmap_get_value(x-1, y-1, q); values[3] = bitmap_get_value(x, y-1, q); values[2] = bitmap_get_value(x+1, y-1, q);
    values[5] = bitmap_get_value(x-1, y, q); values[0] = bitmap_get_value(x, y, q); values[1] = bitmap_get_value(x+1, y, q);
    values[6] = bitmap_get_value(x-1, y+1, q); values[7] = bitmap_get_value(x, y+1, q); values[8] = bitmap_get_value(x+1, y+1, q);
    if (edge_3x3(values)) bitmap_set_value(x, y, edge);
    x++;
    // normal-body
    for (; x < q->header.width-1; x++) {
      values[4] = values[3]; values[3] = values[2]; values[2] = bitmap_get_value(x+1, y-1, q);
      values[5] = values[0]; values[0] = values[1]; values[1] = bitmap_get_value(x+1, y, q);
      values[6] = values[7]; values[7] = values[8]; values[8] = bitmap_get_value(x+1, y+1, q);
      if (edge_3x3(values)) bitmap_set_value(x, y, edge);
    }
    // right-band
    values[4] = values[3]; values[3] = values[2]; values[2] = 0;
    values[5] = values[0]; values[0] = values[1]; values[1] = 0;
    values[6] = values[7]; values[7] = values[8]; values[8] = 0;
    if (edge_3x3(values)) bitmap_set_value(x, y, edge);
  }
  // left-top
  x = 0;
  values[4] = 0; values[3] = bitmap_get_value(x, y-1, q); values[2] = bitmap_get_value(x+1, y-1, q);
  values[5] = 0; values[0] = bitmap_get_value(x, y, q); values[1] = bitmap_get_value(x+1, y, q);
  values[6] = 0; values[7] = 0; values[8] = 0;
  if (edge_3x3(values)) bitmap_set_value(x, y, edge);
  x++;
  // top-band
  for (; x < q->header.width-1; x++) {
    values[4] = values[3]; values[3] = values[2]; values[2] = bitmap_get_value(x+1, y-1, q);
    values[5] = values[0]; values[0] = values[1]; values[1] = bitmap_get_value(x+1, y, q);
    /* values[6] = 0; values[7] = 0; values[8] = 0; */
    if (edge_3x3(values)) bitmap_set_value(x, y, edge);
  }
  // right-top
  values[4] = values[3]; values[3] = values[2]; values[2] = 0;
  values[5] = values[0]; values[0] = values[1]; values[1] = 0;
  /* values[6] = 0; values[7] = 0; values[8] = 0; */
  if (edge_3x3(values)) bitmap_set_value(x, y, edge);
  /* the end of edge finding */
#endif
}

static int zhangsuen_holt_mark_delete(bitmap_t *del, bitmap_t *q, bitmap_t *edge)
{
  int x, y, w, h;
  int edges[9], qs[9];
  int not_done = 0;


#if 1
  w = bitmap_get_width(q);
  h = bitmap_get_height(q);

  for (y = 1; y < h-1; y++) {
    read_3x3_from_bitmap(edges, 1, y, edge);
    read_3x3_from_bitmap(qs, 1, y, q);
    for (x = 1; x < w-1; x++) {
      read_rband_3x3_from_bitmap(edges, x, y, edge);
      read_rband_3x3_from_bitmap(qs, x, y, q);
      if (qs[0] == 1) {
	if (!(edges[0] == 0 ||
	      (edges[1] == 1 && qs[3] == 1 && qs[7] == 1) ||
	      (edges[7] == 1 && qs[5] == 1 && qs[1] == 1) ||
	      (edges[1] == 1 && edges[8] == 1 && edges[7] == 1))) {
	  bitmap_set_value(del, x, y);
	  not_done = 1;
	}
      }
      left_shift_3x3(edges);
      left_shift_3x3(qs);
    }
  }
#else
  // left-bottom
  x = 0;
  y = 0;
  if (bitmap_isset(x, y, q)) {
    if ((bitmap_isreset(x, y, edge)) ||
	(bitmap_isset(x+1, y, edge) && bitmap_isset(x+1, y+1, edge) && bitmap_isset(x, y+1, edge))) {
      //BITMAP_RESET(x, y, del);
    } else {
      bitmap_set_value(x, y, del);
      not_done = 1;
    }
  }
  x++;
  // bottom-band
  for (; x < q->header.width-1; x++) {
    if (bitmap_isset(x, y, q)) {
      if ((bitmap_isreset(x, y, edge)) ||
	  (bitmap_isset(x, y+1, edge) && bitmap_isset(x-1, y, q) && bitmap_isset(x+1, y, q)) ||
	  (bitmap_isset(x+1, y, edge) && bitmap_isset(x+1, y+1, edge) && bitmap_isset(x, y+1, edge))) {
	//BITMAP_RESET(x, y, del);
      } else {
	bitmap_set_value(x, y, del);
	not_done = 1;
      }
    }
  }
  // right-bottom
  if (bitmap_isset(x, y, q)) {
    if (bitmap_isreset(x, y, edge)) {
      //BITMAP_RESET(x, y, del);
    } else {
      bitmap_set_value(x, y, del);
      not_done = 1;
    }
  }
  y++;
  // body
  for (y = 1; y < q->header.height-1; y++) {
    // left-band
    x = 0;
    if (bitmap_isset(x, y, q)) {
      if ((bitmap_isreset(x, y, edge)) ||
	  (bitmap_isset(x+1, y, edge) && bitmap_isset(x, y-1, q) && bitmap_isset(x, y+1, q)) ||
	  (bitmap_isset(x+1, y, edge) && bitmap_isset(x+1, y+1, edge) && bitmap_isset(x, y+1, edge))) {
	//BITMAP_RESET(x, y, del);
      } else {
	bitmap_set_value(x, y, del);
	not_done = 1;
      }
    }
    x++;
    // normal-body
    for (; x < q->header.width-1; x++) {
      if (bitmap_isset(x, y, q)) {
	if ((bitmap_isreset(x, y, edge)) ||
	    (bitmap_isset(x+1, y, edge) && bitmap_isset(x, y-1, q) && bitmap_isset(x, y+1, q)) ||
	    (bitmap_isset(x, y+1, edge) && bitmap_isset(x-1, y, q) && bitmap_isset(x+1, y, q)) ||
	    (bitmap_isset(x+1, y, edge) && bitmap_isset(x+1, y+1, edge) && bitmap_isset(x, y+1, edge))) {
	  //BITMAP_RESET(x, y, del);
	} else {
	  bitmap_set_value(x, y, del);
	  not_done = 1;
	}
      }
    }
    // right-band
    if (bitmap_isset(x, y, q)) {
      if (bitmap_isreset(x, y, edge)) {
	//BITMAP_RESET(x, y, del);
      } else {
	bitmap_set_value(x, y, del);
	not_done = 1;
      }
    }
  }
  // left-top, top-band, right-top
  for (x = 0; x < q->header.width; x++) {
    if (bitmap_isset(x, y, q)) {
      if (bitmap_isreset(x, y, edge)) {
	//BITMAP_RESET(x, y, del);
      } else {
	bitmap_set_value(x, y, del);
	not_done = 1;
      }
    }
  }
#endif
  return not_done;
}

static inline void zhangsuen_holt_delete(bitmap_t *q, bitmap_t *del)
{
  int x, y;
	
  for (y = 0; y < del->header.height; y++) {
    for (x = 0; x < del->header.width; x++) {
      if (bitmap_isset(del, x, y)) {
	bitmap_reset_value(del, x, y);
	bitmap_reset_value(q, x, y);
      }
    }
  }
}

static void zhangsuen_mark_stair(bitmap_t *del, bitmap_t *p, int north_south)
{
  int x, y, w, h;
  int value[9];

  w = bitmap_get_width(p);
  h = bitmap_get_height(p);

  if (north_south) { // north
    for (y = 1; y < h-1; y++) {
      read_3x3_from_bitmap(value, 1, y, p);
      for (x = 1; x < w-1; x++) {
	read_rband_3x3_from_bitmap(value, x, y, p);
	if (value[0] &&
	    !(value[3] && 
	      ((value[1] && !value[2] && !value[6] && (!value[5] || !value[7])) || 
	       (value[5] && !value[4] && !value[8] && (!value[1] || !value[7]))))) {
	  //BITMAP_RESET(x, y, del);		/* Survives */
	} else {
	  bitmap_set_value(del, x, y);
	}
	left_shift_3x3(value);
      }
    }
  } else { // south
    for (y = 1; y < h-1; y++) {
      read_3x3_from_bitmap(value, 1, y, p);
      for (x = 1; x < w-1; x++) {
	read_rband_3x3_from_bitmap(value, x, y, p);
	if (value[0] &&
	    !(value[7] && 
	      ((value[1] && !value[8] && !value[4] && (!value[5] || !value[3])) || 
	       (value[5] && !value[6] && !value[2] && (!value[1] || !value[3]))))) {
	  //BITMAP_RESET(x, y, del);		/* Survives */
	} else {
	  bitmap_set_value(del, x, y);
	}
	left_shift_3x3(value);
      }
    }
  }
}

void zhangsuen_holt_thinning(bitmap_t *q, bitmap_t *p, int stair_remove)
{
  int not_done, w, h;
  bitmap_t *del, *edge;
	
  assert(q);
  assert(p);
  assert(bitmap_get_width(q) == bitmap_get_width(p));
  assert(bitmap_get_height(q) == bitmap_get_height(p));

  w = bitmap_get_width(p);
  h = bitmap_get_height(p);
  bitmap_copy(q, 0, 0, p, 0, 0, w, h);
  del = bitmap_new(w, h);
  edge = bitmap_new(w, h);

  do {
    bitmap_clear(edge);
    zhangsuen_holt_edge_finding(edge, q);
    not_done = zhangsuen_holt_mark_delete(del, q, edge);
    zhangsuen_holt_delete(q, del);
  } while (not_done);

  /* Staircase removal */
  if (stair_remove) {
    zhangsuen_mark_stair(del, q, 1);
    zhangsuen_holt_delete(q, del);
    zhangsuen_mark_stair(del, q, 0);
    zhangsuen_holt_delete(q, del);
  }

  bitmap_destroy(edge);
  bitmap_destroy(del);
}

static inline int crossing_number(int value[9])
{
  int i, val, count;
  
  val = value[8];
  for (i = 1; i < 9; i++) {
    if (value[i] != val)
      count++;
    val = value[i];
  }

  return count;
}

static inline int is_end_in_3x3(int value[9])
{
  int i, val;
  int crossing = 0, count = 0;

  val = value[8];
  for (i = 1; i < 9; i++) {
    if (value[i] != val) crossing++;
    if (value[i] == value[0]) count++;
    val = value[i];
  }

  if (crossing == 2 && count < 4) return 1;

  return 0;
}

static inline int is_bifurcation_in_3x3(int value[9])
{
  int i, val;
  int crossing = 0;

  val = value[8];
  for (i = 1; i < 9; i++) {
    if (value[i] != val) crossing++;
    val = value[i];
  }

  if (crossing >= 6) return 1;

  return 0;
}

static void edge_dump(dlist_t *edge)
{
  int i;
  dlink_t *link;
  dlist_t *branch;

  printf("edge: %d\n", dlist_get_count(edge));
  link = dlist_glimpse(dlist_get_count(edge) - 1, edge);
  if (link->spare) {
    branch = (dlist_t *)link->spare;
    printf("branch: %d\n", dlist_get_count(branch));
    for (i = 0; i < dlist_get_count(branch); i++) {
      link = dlist_glimpse(i, branch);
      edge = (dlist_t *)link->object;
      edge_dump(edge);
    }
  }
}

void edge_tree_dump(dlist_t *edge_tree)
{
  assert(edge_tree);
  edge_dump(edge_tree);
}

static void edge_del(dlist_t *edge)
{
  int i;
  dlist_t *list, *branch;
  point_t *p;
  dlink_t *link;

  while (dlist_get_count(edge) > 0) {
    link = dlist_pop(edge);
    branch = (dlist_t *)link->spare;
    p = (point_t *)link->object;
    point_dec_ref(p);
    point_destroy(p);
    dlink_destroy(link);
  }

  if (branch) {
    dlist_dec_ref(branch);
    for (i = 0; i < dlist_get_count(branch); i++) {
      link = dlist_pop(branch);
      list = (dlist_t *)link->object;
      dlist_dec_ref(list);
      edge_del(list);
      dlist_destroy(list);
    }
    dlist_destroy(branch);
  }
}

void edge_tree_delete(dlist_t *edge_tree)
{
  dlist_t *edge;

  assert(edge_tree);

  edge = edge_tree;
  edge_del(edge);
}

static dlist_t *edge_tree_find_max(dlist_t *edge, dlist_t *max)
{
  int i;
  dlink_t *link;
  dlist_t *branch; //, *new_max;

  if (dlist_get_count(edge) > dlist_get_count(max)) max = edge;

  //printf("edge: %d\n", dlist_get_count(edge));
  link = dlist_glimpse(dlist_get_count(edge) - 1, edge);
  if (link->spare) {
    branch = (dlist_t *)link->spare;
    //printf("branch: %d\n", dlist_get_count(branch));
    for (i = 0; i < dlist_get_count(branch); i++) {
      link = dlist_glimpse(i, branch);
      edge = (dlist_t *)link->object;
      max = edge_tree_find_max(edge, max);
    }
  }
  return max;
}

dlist_t *edge_tree_get_long_edge(dlist_t *edge_tree)
{
  dlist_t *max;

  assert(edge_tree);
  max = edge_tree_find_max(edge_tree, edge_tree);
  return max;
}

void thinning_to_edge_tree(dlist_t *edge_tree, bitmap_t *bin)
{
  int x, y, w, h;
  int i, j; //val, pitch;
  //unsigned char *buf;
  point_t *p;
  point_list_t *edge;
  dlink_t *link;
  dlist_t *edges, *branch;
  bitmap_t *marker;
  bool found;

  /*+---+---+---+
    | 3 | 2 | 1 |
    +---+---+---+
    | 4 |   | 0 |
    +---+---+---+
    | 5 | 6 | 7 |
    +---+---+---+*/
  int dx[8] = {+1, +1,  0, -1, -1, -1,  0, +1};
  int dy[8] = { 0, -1, -1, -1,  0, +1, +1, +1};

  int value[9];

  //assert(p);
  assert(edge_tree);
  assert(bin);

  w = bitmap_get_width(bin);
  h = bitmap_get_height(bin);
  //buf = bitmap_get_buffer(bin);
  //pitch = bitmap_get_pitch(bin) / sizeof(*buf);

  // collect starting end point
  p = NULL;
  for (x = 1; x < w - 1; x++) {
    read_3x3_from_bitmap(value, x, 1, bin);
    for (y = 1; y < h - 1; y++) {
      read_bband_3x3_from_bitmap(value, x, y, bin);
      if ((value[0] == 1) &&
	  (is_end_in_3x3(value) || is_bifurcation_in_3x3(value))) {
	p = point_new_and_set(x, y, 0);
	goto __start_found;
      }
      top_shift_3x3(value);
    }
  }
 __start_found:
  if (p == NULL) return;
  //printf("Start point found\n");

  marker = bitmap_new(w, h);
  bitmap_clear(marker);

  // Allocate the edge containing the start end point.
  point_inc_ref(p);
  link = dlink_new(); link->object = (void *)p;
  dlist_insert(link, edge_tree);

  // Allocate the edge list for tracing connected points.
  edges = dlist_new();
  // Insert the edge containing the start end point.
  dlist_inc_ref(edge_tree);
  link = dlink_new(); link->object = (void *)edge_tree;
  dlist_insert(link, edges);

  while (dlist_get_count(edges) > 0) {
    link = dlist_pop(edges);
    edge = (dlist_t *)link->object;
    dlink_destroy(link);
    dlist_dec_ref(edge);

    link = dlist_glimpse(0, edge);
    p = (point_t *)link->object;
    bitmap_set_value(marker, point_get_x(p), point_get_y(p));

    while (1) {
      found = false;
      for (i = 0; i < 8; i++) {
	x = point_get_x(p) + dx[i]; y = point_get_y(p) + dy[i];

	if (bitmap_isset(marker, x, y) || bitmap_isreset(bin, x, y)) continue;

	read_3x3_from_bitmap(value, x, y, bin);

	if (is_bifurcation_in_3x3(value)) {
	  p = point_new_and_set(x, y, 0);
	  point_inc_ref(p);
	  link = dlink_new(); link->object = (void *)p;
	  dlist_insert(link, edge);
	  bitmap_set_value(marker, x, y);

	  branch = dlist_new();
	  dlist_inc_ref(branch);
	  link->spare = (void *)branch;
	  for (j = 0; j < 8; j++) {
	    if (bitmap_isreset(marker, x + dx[j], y + dy[j]) && value[j + 1]) {
	      // allocate new edge
	      edge = dlist_new();
	      p = point_new_and_set(x + dx[j], y + dy[j], 0);
	      point_inc_ref(p);
	      link = dlink_new(); link->object = (void *)p;
	      dlist_insert(link, edge);
	      // insert a new edge into branch
	      dlist_inc_ref(edge);
	      link = dlink_new(); link->object = (void *)edge;
	      dlist_insert(link, branch);
	      // insert a new edge into edges
	      dlist_inc_ref(edge);
	      link = dlink_new(); link->object = (void *)edge;
	      dlist_insert(link, edges);
	    }
	  }
	  found = true;
	  break;
	}
      }

      if (found) break;

      found = false;
      for (i = 0; i < 8; i++) {
	x = point_get_x(p) + dx[i]; y = point_get_y(p) + dy[i];

	if (bitmap_isset(marker, x, y) || bitmap_isreset(bin, x, y)) continue;

	read_3x3_from_bitmap(value, x, y, bin);

	if (is_end_in_3x3(value)) {
	  //printf("An end point is detected!\n");
	  // Terminate edge finding
	  p = point_new_and_set(x, y, 0);
	  point_inc_ref(p);
	  link = dlink_new(); link->object = (void *)p;
	  dlist_insert(link, edge);
	  bitmap_set_value(marker, x, y);

	  found = true;
	  break;
	}
      }

      if (found) break;

      found = false;
      for (i = 0; i < 8; i++) {
	x = point_get_x(p) + dx[i]; y = point_get_y(p) + dy[i];

	if (bitmap_isset(marker, x, y) || bitmap_isreset(bin, x, y)) continue;

	//printf("An end point is detected!\n");
	// Terminate edge finding
	p = point_new_and_set(x, y, 0);
	point_inc_ref(p);
	link = dlink_new(); link->object = (void *)p;
	dlist_insert(link, edge);
	bitmap_set_value(marker, x, y);

	found = true;
	break;
      }

      if (!found) break;
    }
  }
  bitmap_destroy(marker);
}
