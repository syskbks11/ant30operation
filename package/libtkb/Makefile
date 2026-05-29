# Compiler and linker settings
CC = gcc
CCP = g++
CFLAGS_32 = -m32 -O -g -Wall -Wno-format-extra-args
CFLAGS_64 = -m64 -O -g -Wall -Wno-format-extra-args
INCLUDES = -I/usr/include -I/usr/local/include
LDFLAGS = 
LIBS = -L/usr/lib -lnsl -lm -lc -lpthread -L/usr/local/lib -lstdc++

# Object files and directories
OBJS = u.o conf.o conf2.o rs232c.o netsv.o netcl.o netu.o tm.o thrd.o
LIBDIR = ./export
SRCDIR = ./src
TARGET = libtkb.a

# Archive tool settings
AR = ar
ARFLAGS = rc

# File to store the build type
BUILD_TYPE = .build_type

.PHONY: all 32bit 64bit clean

# Default target: compile in 32bit mode
all: 32bit

# Compile in 32bit mode
32bit: CFLAGS = $(CFLAGS_32)
32bit: ${OBJS}
	mkdir -p ${LIBDIR}
	make comp
	${AR} ${ARFLAGS} ${LIBDIR}/${TARGET} ${OBJS}
	echo "32bit" > $(BUILD_TYPE)
#	/bin/rm -f core *.o *.BAK *.ti *.ii
	cp src/libtkb.h ${LIBDIR}/.

# Compile in 64bit mode
64bit: CFLAGS = $(CFLAGS_64)
64bit: ${OBJS}
	mkdir -p ${LIBDIR}
	make comp
	${AR} ${ARFLAGS} ${LIBDIR}/${TARGET} ${OBJS}
	echo "64bit" > $(BUILD_TYPE)
#	/bin/rm -f core *.o *.BAK *.ti *.ii
	cp src/libtkb.h ${LIBDIR}/.

# Compile objects based on the build type
comp: ${OBJS}

# Compile object files
#${OBJS}: ${SRCDIR}/%/*.cpp
#	${CCP} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $< -o $@

%.o: ${SRCDIR}/%/*.cpp
	${CCP} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $< -o $@

#.cpp.o:
#	${CPP} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $<

#.c.o:
#	${CC} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $<


# Clean up build files
clean:
	rm -f core *.o *.BAK *.ti *.ii
	rm -rf ${LIBDIR} $(BUILD_TYPE)

#LIBDIR	=export

#all	:
#	make -f Makefile.gcc
#	make -f Makefile.ffc
#clean   :
#	/bin/rm -f core *.o *.BAK *.ti *.ii
#	/bin/rm -fr ${LIBDIR}/lib*
