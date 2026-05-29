# Compiler settings
CC = /usr/bin/gcc
CPP = /usr/bin/g++

# Libraries
LIBS = ../libtkb/export/libtkb.a -L/usr/lib -lpthread

# Flag settings
CFLAGS_32 = -m32 -O -g -Wall -Wno-format-extra-args
CFLAGS_64 = -m64 -O -g -Wall -Wno-format-extra-args
INCLUDES = -I../libtkb/export/

# Archive tool settings
AR = ar
ARFLAGS = rc

# Object files and target
OBJS = penguin_weather.o
TARGET = libpenguinweather.a

# File to store the build type
BUILD_TYPE = .build_type

.PHONY: all 32bit 64bit test clean

# Default target: compile in 32bit mode
all: 32bit test srv cln

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
test: test_penweather.o penguin_weather.o
	${CPP} ${CFLAGS} ${CPPFLAGS} -o test_penweather test_penweather.o ${OBJS} ${LIBS}

# Compile srv script (based on the previous build setting)
srv: CFLAGS = $(shell if [ -f $(BUILD_TYPE) ] && grep -q "64bit" $(BUILD_TYPE); then echo "$(CFLAGS_64)"; else echo "$(CFLAGS_32)"; fi)
srv: srv_penweather.o penguin_weather.o
	${CPP} ${CFLAGS} ${CPPFLAGS} -o srv_penweather srv_penweather.o ${OBJS} ${LIBS}

# Compile cln script (based on the previous build setting)
cln: CFLAGS = $(shell if [ -f $(BUILD_TYPE) ] && grep -q "64bit" $(BUILD_TYPE); then echo "$(CFLAGS_64)"; else echo "$(CFLAGS_32)"; fi)
cln: cln_penweather.o penguin_weather.o
	${CPP} ${CFLAGS} ${CPPFLAGS} -o cln_penweather cln_penweather.o ${OBJS} ${LIBS}

.cpp.o:
	${CPP} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $<
#${LIBS}

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $<
#${LIBS}

clean:
	rm -f *.o srv_penweather cln_penweather test_penweather *~ ${TARGET} $(BUILD_TYPE)

