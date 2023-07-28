CC	=/usr/bin/gcc
CPP	=/usr/bin/g++
#CC	=/usr/local/FFC/bin/fcc
#CPP	=/usr/local/FFC/bin/FCC

LIBS = ../rpcomfit/librpcomfit.a ../libtkb/export/libtkb.a ../powermeter_rpcomfit/libpenguinpowermeter.a -lpthread

CFLAGS	=-g -Wall
INCLUDES=-I../rpcomfit/
AR      =ar
ARFLAGS =rc

OBJS = srv_powermeter.o
TARGET = srv_powermeter

all:
	make ${TARGET}

${TARGET}: ${OBJS}
	${CPP} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<
.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

install : all

clean:
	rm -f *.o srv_penpowermeter *~ ${TARGET}


