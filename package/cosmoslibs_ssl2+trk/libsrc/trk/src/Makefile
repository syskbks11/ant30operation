# Compiler settings
CC = gcc
FC = gcc  # Fortran can be linked with GCC if needed
INCLUDES = -I../inc
CFLAGS_32 = -m32 -Wall $(INCLUDES)
CFLAGS_64 = -m64 -Wall $(INCLUDES)
FFLAGS_32 = -m32 -Wall $(INCLUDES)
FFLAGS_64 = -m64 -Wall $(INCLUDES)

# Library and source settings
LIB = ../../../lib/libtrk.a
CSRCS =\
tjcd2m.c \
tjm2cd.c \
trk_00.c \
trk_02.c \
trk_03_cmt.c \
trk_10.c \
trk_20.c \
trk_20_cmt.c \
trk_30.c \
trk_antvl.c

FSRCS =\
EI_305.F \
EI_320.F \
EI_330.F \
EI_350.F \
EI_360.F \
EI_370.F \
EI_371.F \
EI_375.F \
EI_381.F \
EI_382.F \
O_MSG2.F \
R_0000.F \
R_1200.F \
R_1300.F \
R_2200.F \
R_3100.F \
R_3200.F \
R_3210.F \
R_3300.F \
R_3310.F \
R_3320.F \
R_3330.F \
R_3340.F \
R_3400.F \
R_3500.F \
R_4100.F \
R_4200.F \
R_4300.F \
R_4400.F \
R_4500.F \
R_4500_cmt.F \
R_4600.F \
R_5100.F \
R_5110.F \
R_5200.F \
R_5300.F \
R_5400.F \
R_5410.F \
R_6100.F \
R_6200.F \
R_6300.F \
R_6400.F \
R_7100.F \
R_8100.F \
trk_01.F \
trk_11.F \
trk_12.F \
trk_21.F \
trk_22lb.F \
trk_22rd.F \
trk_23.F \
trk_24.F \
trk_26.F \
trk_31.F \
trk_ant.F \
trk_frq.F \
trk_velo.F

COBJS = $(CSRCS:.c=.o)
FOBJS = $(FSRCS:.F=.o)
OBJS = $(COBJS) $(FOBJS)

.PHONY: all 32bit 64bit clean

# Default target: compile in 32bit mode
all: 32bit

# Compile in 32bit mode
32bit: CFLAGS = $(CFLAGS_32)
32bit: FFLAGS = $(FFLAGS_32)
32bit: $(LIB)

# Compile in 64bit mode
64bit: CFLAGS = $(CFLAGS_64)
64bit: FFLAGS = $(FFLAGS_64)
64bit: $(LIB)

# Create the library
$(LIB): $(OBJS)
	ar r $@ $(OBJS)
	cp -r ../inc/*.h ../../../inc/
	/bin/rm -f *.o

# Compile C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Fortran source files into object files
%.o: %.F
	$(FC) $(FFLAGS) -c $< -o $@

# Clean up build files
clean:
	/bin/rm -f *.o $(LIB)

