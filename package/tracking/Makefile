# Compiler and linker settings
CC = /usr/bin/gcc
FC = /usr/bin/gcc
CCP = /usr/bin/g++

# Flags for 32bit and 64bit
CFLAGS_32 = -m32 -O -g -Wall -Wno-format-extra-args
CFLAGS_64 = -m64 -O -g -Wall -Wno-format-extra-args
FFLAGS_32 = -m32
FFLAGS_64 = -m64

# Library and include paths
LIBS = -L../cosmoslibs_ssl2+trk/lib -ltrk -lssl2 \
       ../libtkb/export/libtkb.a \
       -L/usr/lib -lnsl -lm -lc -lpthread -lgfortran
INCLUDES = -I../cosmoslibs_ssl2+trk/inc \
           -I../libtkb/export/
LDFLAGS =

# Target names
TARGET = tracking_calculator
TARGETLIB = libpenguintracking.a

# Archive tool settings
AR      =ar
ARFLAGS =rc

# Object files
OBJS = glacier_celestialObject.o glacier_trackingInformation.o glacier_scanOffset.o penguin_tracking.o tracking_calculator.o

# Build type file
BUILD_TYPE = .build_type

.PHONY: all 32bit 64bit clean

# Default target: 32bit build
all: 32bit

# Build for 32bit
32bit: CFLAGS = $(CFLAGS_32)
32bit: FFLAGS = $(FFLAGS_32)
32bit: $(TARGET) $(TARGETLIB)
	echo "32bit" > $(BUILD_TYPE)

# Build for 64bit
64bit: CFLAGS = $(CFLAGS_64)
64bit: FFLAGS = $(FFLAGS_64)
64bit: $(TARGET) $(TARGETLIB)
	echo "64bit" > $(BUILD_TYPE)

# Create the static library
$(TARGETLIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

# Create the executable
$(TARGET): $(OBJS) compileDate.h
	$(CCP) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

# Generate compileDate.h with the current date and time
compileDate.h:
	echo "#define COMPILE_DATE \"`date +%Y/%m/%d-%H:%M:%S`\"" > compileDate.h

# Compile .cpp files to .o
%.o: %.cpp
	$(CCP) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

# Compile .c files to .o
%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

# Clean up build files
clean:
	rm -f core *.o *.BAK *.ti *.ii
	rm -rf ${TARGET} ${TARGETLIB} $(BUILD_TYPE) compileDate.h

