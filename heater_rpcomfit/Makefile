CC	=/usr/bin/gcc
CPP	=/usr/bin/g++
#CC	=/usr/local/FFC/bin/fcc
#CPP	=/usr/local/FFC/bin/FCC

#LIBS	= ../glacier/serial/libglacierserial.a ../libcssl-0.9.4/libcssl.a ../rpcomfit/librpcomfit.a
LIBS = ../rpcomfit/librpcomfit.a ../libtkb/export/libtkb.a
# ../debug/libglacierdebug.a

CFLAGS	=-g -Wall
INCLUDES=-I../rpcomfit/
AR      =ar
ARFLAGS =rc

OBJS = penguin_heater.o
TARGET	=libpenguinheater.a

all: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}

test: penguin_heater.o test_penheater.o
	${CPP}  -o test_penheater test_penheater.o ${OBJS} ${LIBS}

srv: penguin_heater.o srv_penheater.o
	${CPP}  -o srv_penheater srv_penheater.o ${OBJS} ${LIBS}

.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<
.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

clean:
	rm -f *.o test_penheater *~ ${TARGET}

