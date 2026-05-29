LDFLAGS = -m32 
LIBS = ../libvxi11/libvxi11.a ../libtkb/export/libtkb.a -ltirpc -lstdc++

CFLAGS = -O -g -Wall -m32 -I/usr/include/tirpc -fPIC
INCLUDES =
CCP = /usr/bin/g++
CC = /usr/bin/gcc

TARGET = srvSignalG
LIBRARY = libpenguin_signalG.a

OBJS = srvSignalG.o penguin_signalG.o

all: ${TARGET} ${LIBRARY}

${TARGET}: ${OBJS}
	${CCP} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

${LIBRARY}: penguin_signalG.o
	ar rcs $@ penguin_signalG.o

.cpp.o:
	${CCP} ${CFLAGS} ${INCLUDES} -c $<

.c.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: vxi11Test.o signalGTest.o ${LIBRARY}
	${CCP} ${LDFLAGS} -o vxi11Test vxi11Test.o ${LIBS}
	${CCP} ${LDFLAGS} -o signalGTest signalGTest.o ${LIBRARY} ${LIBS}

srv: srvSignalG.o ${LIBRARY}
	${CCP} ${LDFLAGS} -o srvSignalG srvSignalG.o ${LIBRARY} ${LIBS}

install: all

clean:
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET} ${LIBRARY}
