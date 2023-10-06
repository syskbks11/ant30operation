LDFLAGS =
LIBS	=../libvxi11/libvxi11.a ../libtkb/export/libtkb.a

CFLAGS	=-O -g -Wall
INCLUDES=
CCP	=/usr/bin/gcc
CC	=/usr/bin/gcc

TARGET	=srvSignalG

OBJS	=srvSignalG.o penguin_signalG.o
 
all:	
	make ${TARGET}

${TARGET}: ${OBJS}
	g++ ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

.cpp.o	:
	${CCP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: vxi11Test.o signalGTest.o penguin_signalG.o
	g++ -o vxi11Test vxi11Test.o ${LIBS}
	g++ -o signalGTest signalGTest.o penguin_signalG.o ${LIBS}

install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET} ${MAKESCH}
