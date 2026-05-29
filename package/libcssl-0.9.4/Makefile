# Compiler settings
CC = /usr/bin/gcc
CPP = /usr/bin/g++

# Libraries
LIBS =

# Flag settings
CFLAGS_32 = -m32 -g -Wall -D_GNU_SOURCE -Wno-format-extra-args
CFLAGS_64 = -m64 -g -Wall -D_GNU_SOURCE -Wno-format-extra-args
INCLUDES =

# Archive tool settings
AR = ar
ARFLAGS = rc

# Object files and target
OBJS = cssl.o
TARGET =libcssl.a

# File to store the build type
BUILD_TYPE = .build_type

.PHONY: all 32bit 64bit test clean

# Default target: compile in 32bit mode
all: 32bit

# Compile in 32bit mode
32bit: CFLAGS = $(CFLAGS_32)
32bit: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}
	echo "32bit" > $(BUILD_TYPE)

# Compile in 64bit mode
64bit: CFLAGS = $(CFLAGS_64)
64bit: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}
	echo "64bit" > $(BUILD_TYPE)

# Compile test script (based on the previous build setting)
test: CFLAGS = $(shell if [ -f $(BUILD_TYPE) ] && grep -q "64bit" $(BUILD_TYPE); then echo "$(CFLAGS_64)"; else echo "$(CFLAGS_32)"; fi)
test: test_cssl.o cssl.o
	${CPP} ${CFLAGS} ${CPPFLAGS} -o test_cssl test_cssl.o ${OBJS} ${LIBS}

.cpp.o:
	${CPP} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $<

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $<

clean:
	rm -f *.o test_cssl *~ ${TARGET} $(BUILD_TYPE)

