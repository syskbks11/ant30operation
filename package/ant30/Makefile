# Compiler settings
CC =/usr/bin/gcc
CCP =/usr/bin/g++

# Libraries
LIBS = ../tracking/libpenguintracking.a \
       -L../cosmoslibs_ssl2+trk/lib -ltrk -lssl2 \
       ../libtkb/export/libtkb.a \
       ../motor_rpcomfit/libpenguinmotor.a \
       ../rpcomfit/librpcomfit.a \
       ../weather/libpenguinweather.a \
       ../glacier/serial/libglacierserial.a \
       ../libcssl-0.9.4/libcssl.a \
       ../signalG/libpenguin_signalG.a \
       ../libvxi11/libvxi11.a \
       -L/usr/lib -lnsl -lm -lc -lpthread -lgfortran -ltirpc -lstdc++

# Flag settings
CFLAGS_32 = -m32 -O -g -Wno-format-extra-args
CFLAGS_64 = -m64 -O -g -Wall -Wno-format-extra-args
LDFLAGS =
INCLUDES = -I../cosmoslibs_ssl2+trk/inc \
           -I../rpcomfit \
           -I/usr/include

# Object files and target
OBJS = fitprot.o errno.o weath.o acu.o celestialObject.o scanTable.o instrument.o calcBoth.o trk.o chop.o fe.o if.o be.o tkb32Func.o controlBoard.o tkb32Parser.o tkb32.o
TARGET = tkb32
MAKESCH = makeSch

# File to store the build type
BUILD_TYPE = .build_type

.PHONY: all 32bit 64bit test clean

# Default target: compile in 32bit mode
all: 32bit

# Compile in 32bit mode
32bit: CFLAGS = $(CFLAGS_32)
32bit: $(TARGET) $(MAKESCH)
	echo "32bit" > $(BUILD_TYPE)

# Compile in 64bit mode
64bit: CFLAGS = $(CFLAGS_64)
64bit: $(TARGET) $(MAKESCH)
	echo "64bit" > $(BUILD_TYPE)

# Create the executable
$(TARGET): compileDate.h $(OBJS)
	${CCP} ${CFLAGS} ${CPPFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

$(MAKESCH): compileDate.h $(OBJS) ${MAKESCH}.o
	${CCP} ${CFLAGS} ${CPPFLAGS} -o ${MAKESCH} ${MAKESCH}.o ${LIBS}

# Generate compileDate.h with the current date and time
compileDate.h:
	echo "#define COMPILE_DATE \"`date +%Y/%m/%d-%H:%M:%S`\"" > compileDate.h

.cpp.o:
	${CCP} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $<

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} ${INCLUDES} -c $<

clean:
	rm -f *.o test_penmotor *~ ${TARGET} $(BUILD_TYPE)

test: CFLAGS = $(shell if [ -f $(BUILD_TYPE) ] && grep -q "64bit" $(BUILD_TYPE); then echo "$(CFLAGS_64)"; else echo "$(CFLAGS_32)"; fi)
test: testAcu.o instrumentTest.o acu.o instrument.o
	${CCP} ${CFLAGS} ${CPPFLAGS} -o testAcu testAcu.o acu.o ${LIBS}
	${CCP} ${CFLAGS} ${CPPFLAGS} -o instrumentTest instrumentTest.o instrument.o ${LIBS}

install: ${TARGET} ${MAKESCH}
	mkdir -p ../../tcs01/bin
	mkdir -p ../../tcs01/log/ant30
	cp ${TARGET} ../../tcs01/bin/ant30
	cp ${MAKESCH} ../../tcs01/bin/.

clean:
	/bin/rm -f compileDate.h
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET} ${MAKESCH}
