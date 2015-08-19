# Introduction #

To use in windows system using WinBGIm is better than using libSDL. It is reason that libSDL is some bug of fprintf function; it is not reported but I experienced at fprintf(f, "%lf", val). So to use this in windows system it needs to install MingW and WinBGIm.

# Details #

Install MingW

  * See http://hoyoung2.blogspot.com/2011/09/install-mingw.html

Install WinBGIm

  * Goto http://winbgim.codecutter.org/ and download "the WinBGIm 6.0 library for MingW 5.x.x"
  * Extract ".a" into $(MingW directory)/lib/
  * Extract ".h" into $(MingW directory)/include/