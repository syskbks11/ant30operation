LDFLAGS =
LIBS	= ../libtkb/export/libtkb.a

CFLAGS	=-O -g -Wall
INCLUDES=
CCP	=/usr/bin/gcc
CC	=/usr/bin/gcc

TARGET	=libpenguinlakeshore.a
AR      =ar
ARFLAGS =rc

OBJS	=penguin_lakeshore.o
 
all:	
	make ${TARGET}

${TARGET}: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}

.cpp.o	:
	${CCP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: lakeshoreTest.o penguin_lakeshore.o test.o lakeshore.o
	g++ -o lakeshoreTest lakeshoreTest.o penguin_lakeshore.o ${LIBS}
	g++ -o lakeshore lakeshore.o penguin_lakeshore.o
	g++ -o test test.o

install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET}
