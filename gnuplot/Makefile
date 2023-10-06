LDFLAGS =
LIBS	=

CFLAGS	=-O -g -Wall
INCLUDES=
CCP	=/usr/bin/gcc
CC	=/usr/bin/gcc

TARGET	=stdin_gnuplot

OBJS	=stdin_gnuplot.o
 
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
