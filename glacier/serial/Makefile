CC	=/usr/local/FFC/bin/fcc
CPP	=/usr/local/FFC/bin/FCC
#CC = gcc
#CPP = gcc

LIBS	= ../glacier/serial/libglacierserial.a ../libcssl-0.9.4/libcssl.a ../debug/libglacierdebug.a
#LIBS	= ../libcssl-0.9.4/libcssl.la ../debug/libglacierdebug.a
CFLAGS	=-g -Wall
INCLUDES=
AR      =ar
ARFLAGS =rc

OBJS = glacier_serial.o
TARGET	=libglacierserial.a

all: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}


test: test.o glacier_serial.o
	$(CPP)  -o $@ $@.o $(OBJS) $(LIBS)
#	libtool --mode=link $(CC)  -o $@ $@.o $(OBJS) $(LIBS)

.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<
.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<
	

clean: 
	rm -f *.o test *~
