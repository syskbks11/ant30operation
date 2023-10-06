LDFLAGS =
LIBS	= ../libtkb/export/libtkb.a ../gauge/libpenguingauge.a ../lakeshore/libpenguinlakeshore.a -lpthread

CFLAGS	=-O -g -Wall
INCLUDES=
CCP	=/usr/bin/gcc
CC	=/usr/bin/gcc

TARGET	=srvFrontEnd

OBJS	=srvFrontEnd.o
 
all:	
	make ${TARGET}

${TARGET}: ${OBJS}
	g++ ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

.cpp.o	:
	${CCP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET}
