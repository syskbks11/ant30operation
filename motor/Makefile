#CC	=/usr/bin/gcc
#CPP	=/usr/bin/g++
CC	=/usr/local/FFC/bin/fcc
CPP	=/usr/local/FFC/bin/FCC

LIBS	= ../glacier/serial/libglacierserial.a ../libcssl-0.9.4/libcssl.a
# ../debug/libglacierdebug.a

CFLAGS	=-g -Wall
INCLUDES= 
AR      =ar
ARFLAGS =rc

OBJS = penguin_motor.o fitprot.o
TARGET	=libpenguinmotor.a


all: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}


test: test.o penguin_motor.o
	$(CPP)  -o $@ $@.o $(OBJS) $(LIBS)

.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<
.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<


clean: 
	rm -f *.o test *~ ${TARGET}

