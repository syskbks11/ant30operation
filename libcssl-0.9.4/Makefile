# Copyright 2003 Marcin Siennicki <m.siennicki@cloos.pl>
# see COPYING file for details 


CC	=/usr/local/FFC/bin/fcc
CPP	=/usr/local/FFC/bin/FCC
#CC = gcc
#CPP = gcc

LIBS	=
CFLAGS	=-g -D_GNU_SOURCE
#CFLAGS	=-g -Wall -D_GNU_SOURCE
INCLUDES= 
AR      =ar
ARFLAGS =rc

OBJS = cssl.o
TARGET	=libcssl.a

all: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}


test: test.o cssl.o
	$(CC)  -o $@ $@.o $(OBJS) $(LIBS)

.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<
.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<
	

clean: 
	rm -f *.o test *~
