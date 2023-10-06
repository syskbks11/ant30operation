LDFLAGS =
LIBS	=

CFLAGS	=-O -g -Wall
INCLUDES=
CCP	=/usr/bin/gcc
CC	=/usr/bin/gcc
AR      =ar
ARFLAGS =rc

TARGET	=libpenguingauge.a
AR      =ar
ARFLAGS =rc

OBJS	=penguin_gauge.o
 
all:	
	make ${TARGET}

${TARGET}: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}

.cpp.o	:
	${CCP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: gaugeTest.o gauge.o penguin_gauge.o
	g++ -o gaugeTest gaugeTest.o penguin_gauge.o ${LIBS}
	g++ -o gauge gauge.o penguin_gauge.o ${LIBS}

install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET}
