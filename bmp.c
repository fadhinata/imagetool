/*
 * Windows BMP file functions for OpenGL.
 *
 * Written by Michael Sweet.
 */

#include <bmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <pixmap/bytemap.h>

#ifdef WIN32
/*
 * 'LoadDIBitmap()' - Load a DIB/BMP file from disk.
 *
 * Returns a pointer to the bitmap if successful, NULL otherwise...
 */
unsigned char *                    /* O - Bitmap data */
LoadDIBitmap(const char *filename, /* I - File to load */
             BITMAPINFO **info)    /* O - Bitmap information */
{
    FILE             *fp;          /* Open file pointer */
    unsigned char    *bits;        /* Bitmap pixel bits */
    int              bitsize;      /* Size of bitmap */
    int              infosize;     /* Size of header information */
    BITMAPFILEHEADER header;       /* File header */


    /* Try opening the file; use "rb" mode to read this *binary* file. */
    if ((fp = fopen(filename, "rb")) == NULL)
        return (NULL);

    /* Read the file header and any following bitmap information... */
    if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
        /* Couldn't read the file header - return NULL... */
        fclose(fp);
        return (NULL);
    }
    
    /* Check for BM reversed... */
    if (header.bfType != 'MB') {
        /* Not a bitmap file - return NULL... */
        fclose(fp);
        return (NULL);
    }

    infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
    if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL) {
        /* Couldn't allocate memory for bitmap info - return NULL... */
        fclose(fp);
        return (NULL);
    }

    if (fread(*info, 1, infosize, fp) < infosize) {
        /* Couldn't read the bitmap header - return NULL... */
        free(*info);
        fclose(fp);
        return (NULL);
    }

    /* Now that we have all the header info read in, allocate memory for *
     * the bitmap and read *it* in...                                    */
    if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
        bitsize = ((*info)->bmiHeader.biWidth *
                   (*info)->bmiHeader.biBitCount + 7) / 8 *
  	           abs((*info)->bmiHeader.biHeight);

    if ((bits = (unsigned char *)malloc(bitsize)) == NULL) {
        /* Couldn't allocate memory - return NULL! */
        free(*info);
        fclose(fp);
        return (NULL);
    }

    if (fread(bits, 1, bitsize, fp) < bitsize) {
        /* Couldn't read bitmap - free memory and return NULL! */
        free(*info);
        free(bits);
        fclose(fp);
        return (NULL);
    }

    /* OK, everything went fine - return the allocated bitmap... */
    fclose(fp);
    return (bits);
}


/*
 * 'SaveDIBitmap()' - Save a DIB/BMP file to disk.
 *
 * Returns 0 on success or -1 on failure...
 */

int                                /* O - 0 = success, -1 = failure */
SaveDIBitmap(const char *filename, /* I - File to load */
             BITMAPINFO *info,     /* I - Bitmap information */
	     unsigned char *bits)  /* I - Bitmap data */
{
    FILE             *fp;          /* Open file pointer */
    int              size,         /* Size of file */
                     infosize,     /* Size of bitmap info */
                     bitsize;      /* Size of bitmap pixels */
    BITMAPFILEHEADER header;       /* File header */


    /* Try opening the file; use "wb" mode to write this *binary* file. */
    if ((fp = fopen(filename, "wb")) == NULL)
        return (-1);

    /* Figure out the bitmap size */
    if (info->bmiHeader.biSizeImage == 0)
	bitsize = (info->bmiHeader.biWidth *
        	   info->bmiHeader.biBitCount + 7) / 8 *
		  abs(info->bmiHeader.biHeight);
    else
	bitsize = info->bmiHeader.biSizeImage;

    /* Figure out the header size */
    infosize = sizeof(BITMAPINFOHEADER);
    switch (info->bmiHeader.biCompression) {
	case BI_BITFIELDS :
            infosize += 12; /* Add 3 RGB doubleword masks */
            if (info->bmiHeader.biClrUsed == 0)
	      break;
	case BI_RGB :
            if (info->bmiHeader.biBitCount > 8 &&
        	info->bmiHeader.biClrUsed == 0)
	      break;
	case BI_RLE8 :
	case BI_RLE4 :
            if (info->bmiHeader.biClrUsed == 0)
              infosize += (1 << info->bmiHeader.biBitCount) * 4;
	    else
              infosize += info->bmiHeader.biClrUsed * 4;
	    break;
    }

    size = sizeof(BITMAPFILEHEADER) + infosize + bitsize;

    /* Write the file header, bitmap information, and bitmap pixel data... */
    header.bfType      = 'MB'; /* Non-portable... sigh */
    header.bfSize      = size;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits   = sizeof(BITMAPFILEHEADER) + infosize;

    if (fwrite(&header, 1, sizeof(BITMAPFILEHEADER), fp) < sizeof(BITMAPFILEHEADER)) {
        /* Couldn't write the file header - return... */
        fclose(fp);
        return (-1);
    }

    if (fwrite(info, 1, infosize, fp) < infosize) {
        /* Couldn't write the bitmap header - return... */
        fclose(fp);
        return (-1);
    }

    if (fwrite(bits, 1, bitsize, fp) < bitsize) {
        /* Couldn't write the bitmap - return... */
        fclose(fp);
        return (-1);
    }

    /* OK, everything went fine - return... */
    fclose(fp);
    return (0);
}


#else /* !WIN32 */
/*
 * Functions for reading and writing 16- and 32-bit little-endian integers.
 */

static unsigned short read_word(FILE *fp);
static unsigned int   read_dword(FILE *fp);
static int            read_long(FILE *fp);

static int            write_word(FILE *fp, unsigned short w);
static int            write_dword(FILE *fp, unsigned int dw);
static int            write_long(FILE *fp, int l);


/*
 * 'LoadDIBitmap()' - Load a DIB/BMP file from disk.
 *
 * Returns a pointer to the bitmap if successful, NULL otherwise...
 */

unsigned char *                    /* O - Bitmap data */
LoadDIBitmap(const char *filename, /* I - File to load */
             BITMAPINFO **info)    /* O - Bitmap information */
{
    FILE             *fp;          /* Open file pointer */
    unsigned char    *bits;        /* Bitmap pixel bits */
    unsigned char    *ptr;         /* Pointer into bitmap */
    unsigned char    temp;         /* Temporary variable to swap red and blue */
    int              x, y;         /* X and Y position in image */
    int              length;       /* Line length */
    int              bitsize;      /* Size of bitmap */
    int              infosize;     /* Size of header information */
    BITMAPFILEHEADER header;       /* File header */

    /* Try opening the file; use "rb" mode to read this *binary* file. */
    if ((fp = fopen(filename, "rb")) == NULL)
        return (NULL);

    /* Read the file header and any following bitmap information... */
    header.bfType      = read_word(fp); // 2 bytes
    header.bfSize      = read_dword(fp); // 4 bytes
    header.bfReserved1 = read_word(fp); // 2 bytes
    header.bfReserved2 = read_word(fp); // 2 bytes
    header.bfOffBits   = read_dword(fp); // 4 bytes

    /* Check for BM reversed... */
    if (header.bfType != BF_TYPE) {
        /* Not a bitmap file - return NULL... */
        fclose(fp);
        return (NULL);
    }

    infosize = header.bfOffBits - 14; //sizeof(BITMAPFILEHEADER)/*18*/;
    if ((*info = (BITMAPINFO *)malloc(sizeof(BITMAPINFO))) == NULL) {
        /* Couldn't allocate memory for bitmap info - return NULL... */
        fclose(fp);
        return (NULL);
    }

    (*info)->bmiHeader.biSize          = read_dword(fp); // 4
    (*info)->bmiHeader.biWidth         = read_long(fp); // 4
    (*info)->bmiHeader.biHeight        = read_long(fp); // 4
    (*info)->bmiHeader.biPlanes        = read_word(fp); // 2
    (*info)->bmiHeader.biBitCount      = read_word(fp); // 2
    (*info)->bmiHeader.biCompression   = read_dword(fp); // 4
    (*info)->bmiHeader.biSizeImage     = read_dword(fp); // 4
    (*info)->bmiHeader.biXPelsPerMeter = read_long(fp); // 4
    (*info)->bmiHeader.biYPelsPerMeter = read_long(fp); // 4
    (*info)->bmiHeader.biClrUsed       = read_dword(fp); // 4
    (*info)->bmiHeader.biClrImportant  = read_dword(fp); // 4

    if (infosize > sizeof(BITMAPINFOHEADER)) {
	if (fread((*info)->bmiColors, infosize - sizeof(BITMAPINFOHEADER), 1, fp) < 1) {
            /* Couldn't read the bitmap header - return NULL... */
            free(*info);
            fclose(fp);
            return (NULL);
        }
    }

    //if (bmpinfo->bmiHeader.biBitCount == 8);

    /* Now that we have all the header info read in, allocate memory for *
     * the bitmap and read *it* in...                                    */
    if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
        bitsize = ((*info)->bmiHeader.biWidth *
                   (*info)->bmiHeader.biBitCount + 7) / 8 *
  	           abs((*info)->bmiHeader.biHeight);

    printf("bitsize:%d\n", bitsize);
    if ((bits = (unsigned char *)malloc(bitsize)) == NULL) {
        /* Couldn't allocate memory - return NULL! */
      printf("Couldn't allocate memory - return NULL!\n");
      free(*info);
        fclose(fp);
        return (NULL);
    }

    if (fread(bits, 1, bitsize, fp) < bitsize) {
        /* Couldn't read bitmap - free memory and return NULL! */
        free(*info);
        free(bits);
        fclose(fp);
        return (NULL);
    }

#if 0
    /* Swap red and blue */
    length = ((*info)->bmiHeader.biWidth * 3 + 3) & ~3;
    for (y = 0; y < (*info)->bmiHeader.biHeight; y ++) {
	ptr = bits + y * length;
        for (x = (*info)->bmiHeader.biWidth; x > 0; x --, ptr += 3) {
	    temp   = ptr[0];
	    ptr[0] = ptr[2];
	    ptr[2] = temp;
	}
    }
#endif
    /* OK, everything went fine - return the allocated bitmap... */
    fclose(fp);
    return (bits);
}

/*
 * 'SaveDIBitmap()' - Save a DIB/BMP file to disk.
 *
 * Returns 0 on success or -1 on failure...
 */

int                                /* O - 0 = success, -1 = failure */
SaveDIBitmap(const char *filename, /* I - File to load */
             BITMAPINFO *info,     /* I - Bitmap information */
	         unsigned char *bits)  /* I - Bitmap data */
{
    FILE *fp;                      /* Open file pointer */
    int  size,                     /* Size of file */
         infosize,                 /* Size of bitmap info */
         bitsize;                  /* Size of bitmap pixels */


    /* Try opening the file; use "wb" mode to write this *binary* file. */
    if ((fp = fopen(filename, "wb")) == NULL)
        return (-1);

    /* Figure out the bitmap size */
    if (info->bmiHeader.biSizeImage == 0)
	bitsize = (info->bmiHeader.biWidth *
        	   info->bmiHeader.biBitCount + 7) / 8 *
		  abs(info->bmiHeader.biHeight);
    else
	bitsize = info->bmiHeader.biSizeImage;

    /* Figure out the header size */
    infosize = sizeof(BITMAPINFOHEADER);
    switch (info->bmiHeader.biCompression) {
	case BI_BITFIELDS :
            infosize += 12; /* Add 3 RGB doubleword masks */
            if (info->bmiHeader.biClrUsed == 0)
	      break;
	case BI_RGB :
            if (info->bmiHeader.biBitCount > 8 &&
        	info->bmiHeader.biClrUsed == 0)
	      break;
	case BI_RLE8 :
	case BI_RLE4 :
            if (info->bmiHeader.biClrUsed == 0)
              infosize += (1 << info->bmiHeader.biBitCount) * 4;
	    else
              infosize += info->bmiHeader.biClrUsed * 4;
	    break;
    }
    
    size = sizeof(BITMAPFILEHEADER) + infosize + bitsize;

    /* Write the file header, bitmap information, and bitmap pixel data... */
    write_word(fp, BF_TYPE);        /* bfType */
    write_dword(fp, size);          /* bfSize */
    write_word(fp, 0);              /* bfReserved1 */
    write_word(fp, 0);              /* bfReserved2 */
    write_dword(fp, 14 + infosize); /* bfOffBits */

    write_dword(fp, info->bmiHeader.biSize);
    write_long(fp, info->bmiHeader.biWidth);
    write_long(fp, info->bmiHeader.biHeight);
    write_word(fp, info->bmiHeader.biPlanes);
    write_word(fp, info->bmiHeader.biBitCount);
    write_dword(fp, info->bmiHeader.biCompression);
    write_dword(fp, info->bmiHeader.biSizeImage);
    write_long(fp, info->bmiHeader.biXPelsPerMeter);
    write_long(fp, info->bmiHeader.biYPelsPerMeter);
    write_dword(fp, info->bmiHeader.biClrUsed);
    write_dword(fp, info->bmiHeader.biClrImportant);

    if (infosize > sizeof(BITMAPINFOHEADER)) {
      if (fwrite(info->bmiColors, infosize - sizeof(BITMAPINFOHEADER), 1, fp) < 1) {
            /* Couldn't write the bitmap header - return... */
            fclose(fp);
            return (-1);
        }
    }

    if (fwrite(bits, 1, bitsize, fp) < bitsize) {
        /* Couldn't write the bitmap - return... */
        fclose(fp);
        return (-1);
    }

    /* OK, everything went fine - return... */
    fclose(fp);
    return (0);
}

/*
 * 'read_word()' - Read a 16-bit unsigned integer.
 */

static unsigned short     /* O - 16-bit unsigned integer */
read_word(FILE *fp)       /* I - File to read from */
{
    unsigned char b0, b1; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);

    return ((b1 << 8) | b0);
}


/*
 * 'read_dword()' - Read a 32-bit unsigned integer.
 */

static unsigned int               /* O - 32-bit unsigned integer */
read_dword(FILE *fp)              /* I - File to read from */
{
    unsigned char b0, b1, b2, b3; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


/*
 * 'read_long()' - Read a 32-bit signed integer.
 */

static int                        /* O - 32-bit signed integer */
read_long(FILE *fp)               /* I - File to read from */
{
    unsigned char b0, b1, b2, b3; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


/*
 * 'write_word()' - Write a 16-bit unsigned integer.
 */

static int                     /* O - 0 on success, -1 on error */
write_word(FILE           *fp, /* I - File to write to */
           unsigned short w)   /* I - Integer to write */
{
    putc(w, fp);
    return (putc(w >> 8, fp));
}


/*
 * 'write_dword()' - Write a 32-bit unsigned integer.
 */

static int                    /* O - 0 on success, -1 on error */
write_dword(FILE         *fp, /* I - File to write to */
            unsigned int dw)  /* I - Integer to write */
{
    putc(dw, fp);
    putc(dw >> 8, fp);
    putc(dw >> 16, fp);
    return (putc(dw >> 24, fp));
}


/*
 * 'write_long()' - Write a 32-bit signed integer.
 */

static int           /* O - 0 on success, -1 on error */
write_long(FILE *fp, /* I - File to write to */
           int  l)   /* I - Integer to write */
{
    putc(l, fp);
    putc(l >> 8, fp);
    putc(l >> 16, fp);
    return (putc(l >> 24, fp));
}
#endif /* WIN32 */

void save_bytemap_as_gray_BMP(bytemap_t *m, const char *fn)
{
  int i, w, h;
  BITMAPINFO *info;

  assert(m);

  w = bytemap_get_width(m);
  h = bytemap_get_height(m);

  info = (BITMAPINFO *)malloc(sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 255);
  info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  info->bmiHeader.biWidth = w;
  info->bmiHeader.biHeight = -h;
  info->bmiHeader.biPlanes = 1;
  info->bmiHeader.biBitCount = 8;
  info->bmiHeader.biCompression = BI_RGB;
  info->bmiHeader.biSizeImage = ((w * 8 + 31) & ~31) / 8 * h;
  info->bmiHeader.biXPelsPerMeter = 72;
  info->bmiHeader.biYPelsPerMeter = 72;
  info->bmiHeader.biClrUsed = 256;
  info->bmiHeader.biClrImportant = 256;

  for (i = 0; i < 256; i++) {
    info->bmiColors[i].rgbRed = i;
    info->bmiColors[i].rgbGreen = i;
    info->bmiColors[i].rgbBlue = i;
  }

  SaveDIBitmap(fn, info, bytemap_get_buffer(m));

  free(info);
}
