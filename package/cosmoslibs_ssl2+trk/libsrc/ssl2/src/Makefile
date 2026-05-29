# Compiler settings
CC = gcc
FC = gcc  # Fortran can be linked with GCC if needed
INCLUDES =
CFLAGS_32 = -m32 -Wall -D_GNU_SOURCE -D_REENTRANT $(INCLUDES)
CFLAGS_64 = -m64 -Wall -D_GNU_SOURCE -D_REENTRANT $(INCLUDES)
FFLAGS_32 = -m32 -Wall -D_GNU_SOURCE -D_REENTRANT $(INCLUDES)
FFLAGS_64 = -m64 -Wall -D_GNU_SOURCE -D_REENTRANT $(INCLUDES)

# Library and source settings
LIB = ../../../lib/libssl2.a

SRCS =\
DCBRT.f \
DHOUSE.f \
DKBIC3.f \
DKBIF3.f \
DLAX.f \
DLAXR.f \
DLESQ1.f \
DLOWP.f \
DMAV.f \
DMGGM.f \
DSUM.f \
HOUSE.f \
bfunc.f \
bspl.f \
dbfunc.f \
dlfit.f \
dlubksb.f \
dludcmp.f \
gausel.f \
ifunct.f \
laeq.f \
lfit.f \
lubksb.f \
ludcmp.f \
slubksb.f \
sludcmp.f

OBJS = $(SRCS:.f=.o)
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
	/bin/rm -f *.o

# Compile Fortran source files into object files
%.o: %.F
	$(FC) $(FFLAGS) -c $< -o $@

# Clean up build files
clean:
	/bin/rm -f *.o $(LIB)

