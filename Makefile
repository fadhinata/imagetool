# Makefile created by Dev-C++ 4.9.9.2
PREFIX = 
CC = $(PREFIX)gcc
CPP = $(PREFIX)g++
RM = rm -f
RMDIR = rm -f -r

GLIB = SDL
#GLIB = WINBGIM

CFLAGS = -g -Wall
ifeq ($(GLIB), WINBGIM)
CFLAGS += -DUSE_LIBBGI
CFLAGS += -LWinBGIm_Library6_0_Nov2005
endif
ifeq ($(GLIB), SDL)
CFLAGS += -DUSE_LIBSDL
CFLAGS += `sdl-config --cflags`
endif
#CFLAGS += `pkg-config --libs --cflags gtk+-2.0` -export-dynamic

LIBS = -lm
ifeq ($(GLIB), WINBGIM)
LIBS += -lstdc++ -lbgi -lgdi32 -lcomdlg32 -luuid -loleaut32 -lole32
endif
ifeq ($(GLIB), SDL)
LIBS += `sdl-config --libs`
endif

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
#LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--cref, --gc-sections
LDFLAGS = $(LIBS)

TARGET = imagetool

INCS = -I.
ifeq ($(GLIB), WINBGIM)
INCS += -IWinBGIm_Library6_0_Nov2005
endif

CSRC = bmp.c
CSRC += buffering/dlink.c
CSRC += convert/convert.c
CSRC += color/gray.c
CSRC += color/hsl.c
CSRC += color/hsv.c
CSRC += color/yuv.c
CSRC += drawing/draw_line.c
CSRC += drawing/draw_circle.c
CSRC += edge/kirsch.c
CSRC += edge/prewitt.c
CSRC += edge/roberts.c
CSRC += edge/robinson.c
CSRC += edge/scharr.c
CSRC += edge/sobel.c
CSRC += filtering/convolute.c
CSRC += histogram/histogram.c
CSRC += histogram/gamma.c
CSRC += hough/hough_line.c
CSRC += hough/hough_circle.c
CSRC += labeling/labeling.c
CSRC += labeling/chaincode.c
CSRC += linear_algebra/complex2.c
CSRC += linear_algebra/matrix.c
CSRC += linear_algebra/vector.c
CSRC += linear_algebra/vector_list.c
CSRC += linear_algebra/gauss.c
CSRC += linear_algebra/gram_schmidt.c
CSRC += linear_algebra/qr_decomp.c
CSRC += geometry/convexhull.c
CSRC += geometry/rotate.c
CSRC += geometry/point.c
CSRC += geometry/point_list.c
CSRC += geometry/line.c
CSRC += geometry/line_list.c
CSRC += geometry/triangle.c
CSRC += geometry/triangle_list.c
CSRC += geometry/polygon.c
CSRC += geometry/delaunay.c
CSRC += geometry/dt.c
CSRC += morphology/gray_morph.c
CSRC += morphology/binary_morph.c
CSRC += morphology/thin.c
CSRC += pixmap/bitmap.c
CSRC += pixmap/bytemap.c
CSRC += pixmap/wordmap.c
CSRC += pixmap/dwordmap.c
CSRC += pixmap/floatmap.c
CSRC += pixmap/doublemap.c
CSRC += region_growing/regiongrow.c
CSRC += region_growing/watershed.c
CSRC += regression/least_squares.c
CSRC += regression/pls.c
CSRC += regression/pca.c
CSRC += smoothing/mean_smooth.c
CSRC += smoothing/median_smooth.c
CSRC += spline/bezier.c
CSRC += statistics/matrix_statistic.c
CSRC += statistics/pixmap_statistic.c
CSRC += statistics/vector_statistic.c
CSRC += thresholding/threshold.c
CSRC += thresholding/backsym.c
CSRC += thresholding/dsigma.c
CSRC += thresholding/isodata.c
CSRC += thresholding/otsu.c
CSRC += thresholding/triangulate.c
CSRC += transform/dct.c
CSRC += transform/fourier.c
CSRC += wavelet/daub4.c
CSRC += wavelet/haar.c
CSRC += envi/envi.c

# Define all object files.
COBJ = $(CSRC:.c=.o)

CPPSRC = main.cpp
CPPSRC += interface/screen.cpp
CPPSRC += examples/dt_wshed.cpp
CPPSRC += examples/carrot_process.cpp
CPPSRC += examples/tricam_process.cpp
CPPSRC += examples/tricam_sample.cpp
CPPSRC += examples/tricam_diff.cpp
CPPSRC += examples/tricam_morphology.cpp
CPPSRC += examples/tricam_labeling.cpp
CPPSRC += examples/tricam_display.cpp
CPPSRC += examples/image_downsize.cpp
CPPSRC += examples/tricam_diff_div.cpp
CPPSRC += examples/tricam_citrus.cpp
CPPSRC += examples/tricam_apple_sub_div.cpp
CPPSRC += examples/tricam_apple_sub.cpp
CPPSRC += examples/matrix_test.cpp
CPPSRC += examples/FLIR_PM595.cpp

CPPOBJ = $(CPPSRC:.cpp=.o)

RES  = 
OBJS = $(COBJ)
LST = $(CSRC:.c=.lst)

#BIN  = imagetool

all: build

build: $(TARGET)

$(TARGET): $(COBJ) $(CPPOBJ)
	$(CPP) $(CFLAGS) $(COBJ) $(CPPOBJ) -o $@ $(LDFLAGS)

$(COBJ) : %.o : %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@ 

$(CPPOBJ) : %.o : %.cpp
	$(CPP) $(CFLAGS) $(INCS) -c $< -o $@

clean: clean_list

clean_list :
	$(RM) $(TARGET)
	$(RM) $(COBJ) $(CPPOBJ)
	$(RM) $(CSRC:.c=.c~) $(CPPSRC:.cpp=.cpp~)
	$(RM) $(CSRC:.c=.h~) $(CPPSRC:.cpp=.h~)
	$(RMDIR) .dep | exit 0

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# Listing of phony targets.
.PHONY : all build exe clean clean_list

