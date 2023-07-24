CC	=/usr/bin/gcc
CPP	=/usr/bin/g++
#CC	=/usr/local/FFC/bin/fcc
#CPP	=/usr/local/FFC/bin/FCC

#LIBS	= ../glacier/serial/libglacierserial.a ../libcssl-0.9.4/libcssl.a ../rpcomfit/librpcomfit.a
LIBS = ../rpcomfit/librpcomfit.a
# ../debug/libglacierdebug.a

CFLAGS	=-g -Wall
INCLUDES=-I../rpcomfit/
AR      =ar
ARFLAGS =rc

OBJS = penguin_lakeshore.o
TARGET	=libpenguinlakeshore.a


all: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}

test: penguin_lakeshore.o test_penlakeshore.o
	${CPP}  -o test_penlakeshore test_penlakeshore.o ${OBJS} ${LIBS}

.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<
.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<


clean:
	rm -f *.o test_penlakeshore *~ ${TARGET}




