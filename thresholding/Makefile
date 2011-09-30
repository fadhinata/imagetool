PREFIX = /mingw/bin/

CC = $(PREFIX)gcc
CPP = $(PREFIX)g++
AR = $(PREFIX)ar
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size
NM = $(PREFIX)nm

REMOVE = rm -f
REMOVEDIR = rm -f -r
COPY = cp

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options for C here
CDEFS = 

# Place -I options here
CINCS =

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
#OPT = s
OPT = 0

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
EXTRAINCDIRS = ./ ../pixmap ../histogram

# List any extra directories to look for library files here.
#     Each directory must be seperated by a space.
EXTRA_LIBDIRS = ./ ../pixmap ../histogram

#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
#
# Flags for C and C++ (arm-elf-gcc/arm-elf-g++)
CFLAGS = $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
CFLAGS += -Wall -Wcast-align -Wimplicit 
CFLAGS += -Wpointer-arith -Wswitch
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wredundant-decls -Wreturn-type -Wshadow -Wunused
CFLAGS += -Wa,-adhlns=$(subst $(suffix $<),.lst,$<) 
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--cref,--gc-sections
LDFLAGS += -lc -lgcc -lm -lpixmap
LDFLAGS += $(CPLUSPLUS_LIB)
LDFLAGS += $(patsubst %,-L%,$(EXTRA_LIBDIRS))
LDFLAGS += $(patsubst %,-l%,$(EXTRA_LIBS))

TARGET = libthreshold.a

SRC = threshold.c isodata.c dsigma.c backsym.c triangle.c otsu.c

# Define all object files.
COBJ = $(SRC:.c=.o) 

# Define all listing files.
LST = $(SRC:.c=.lst)

# Compiler flags to generate dependency files.
### GENDEPFLAGS = -Wp,-M,-MP,-MT,$(*F).o,-MF,.dep/$(@F).d
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS  = -I. $(CFLAGS) $(GENDEPFLAGS)

# Default target.
all: library

library: $(TARGET)

# Compile: create object files from C source files. ARM/Thumb
$(COBJ) : %.o : %.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 

$(TARGET) : $(COBJ)
	$(AR) -rv $@ $(COBJ)

# Target: clean project.
clean: clean_list

clean_list :
	$(REMOVE) $(TARGET)
	$(REMOVE) $(COBJ)
	$(REMOVE) $(LST)
	$(REMOVEDIR) .dep | exit 0

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# Listing of phony targets.
.PHONY : all library clean clean_list $(TARGET)
