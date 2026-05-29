# Compiler settings
CC = /usr/bin/gcc
CPP = /usr/bin/g++

# Libraries
LIBS_32 = -L/usr/lib/i386-linux-gnu/ -ltinfo -lcurses
LIBS_64 = -lcurses

# Flag settings
CFLAGS_32 = -m32 -g -Wall
CFLAGS_64 = -m64 -g -Wall
INCLUDES_32 = -I/usr/include/i386-linux-gnu
INCLUDES_64 =

# Archive tool settings
AR = ar
ARFLAGS = rc

# Object files and target
OBJS = glacier_monitor.o
TARGET = libglaciermonitor.a

# File to store the build type
BUILD_TYPE = .build_type

.PHONY: all 32bit 64bit test clean

# Default target: compile in 32bit mode
all: 32bit

# Compile in 32bit mode
32bit: CFLAGS = $(CFLAGS_32)
32bit: LIBS = $(LIBS_32)
32bit: INCLUDES = $(INCLUDES_32)
32bit: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}
	echo "32bit" > $(BUILD_TYPE)

# Compile in 64bit mode
64bit: CFLAGS = $(CFLAGS_64)
64bit: LIBS = $(LIBS_64)
64bit: INCLUDES = $(INCLUDES_64)
64bit: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}
	echo "64bit" > $(BUILD_TYPE)

# Compile test script (based on the previous build setting)
test: CFLAGS = $(shell if [ -f $(BUILD_TYPE) ] && grep -q "64bit" $(BUILD_TYPE); then echo "$(CFLAGS_64)"; else echo "$(CFLAGS_32)"; fi)
test: LIBS = $(shell if [ -f $(BUILD_TYPE) ] && grep -q "64bit" $(BUILD_TYPE); then echo "$(LIBS_64)"; else echo "$(LIBS_32)"; fi)
test: INCLUDES = $(shell if [ -f $(BUILD_TYPE) ] && grep -q "64bit" $(BUILD_TYPE); then echo "$(INCLUDES_64)"; else echo "$(INCLUDES_32)"; fi)
test: test_glacier.o glacier_monitor.o test_dummysource.o
	${CPP} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -o test_dummysource test_dummysource.o ${OBJS} ${LIBS}
	${CPP} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -o test_glacier test_glacier.o ${OBJS} ${LIBS}

.cpp.o:
	${CPP} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $<

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $<

clean:
	rm -f *.o test_dummysource test_glacier *~ ${TARGET} $(BUILD_TYPE)

