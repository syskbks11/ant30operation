LDFLAGS =
LIBS	= ../libtkb/export/libtkb.a

CFLAGS	=-O -g -Wall
INCLUDES=
CCP	=/usr/bin/gcc
CC	=/usr/bin/gcc

TARGET	=srvChopper

OBJS	=srvChopper.o penguin_chopper.o

all:
	make ${TARGET}

${TARGET}: ${OBJS}
	g++ ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

.cpp.o	:
	${CCP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: chopper.o chopperTest.o penguin_chopper.o autoChopper.o
	g++ -o chopperTest chopperTest.o penguin_chopper.o ${LIBS}
	g++ -o chopper chopper.o penguin_chopper.o ${LIBS}
	g++ -o autoChopper autoChopper.o penguin_chopper.o ${LIBS}

install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET}
