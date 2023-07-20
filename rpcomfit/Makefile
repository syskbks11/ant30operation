#CC	=/usr/local/FFC/bin/fcc
#CPP	=/usr/local/FFC/bin/FCC
CC = gcc
CPP = gcc

LIBS	= 
CFLAGS	=-g -Wall
INCLUDES=
AR      =ar
ARFLAGS =rc

OBJS = rpcomfit.o fitprot.o
TARGET = librpcomfit.a

all: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}


ha770: sample_ha770.o
	$(CPP)  -o $@ sample_$@.o $(OBJS) $(LIBS)

.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<
.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

clean:
	rm -f *.o ha770 librpcomfit.a *~
