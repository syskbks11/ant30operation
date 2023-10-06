LDFLAGS =
#LIBS	= -L/usr/local/lib/ -lusb220_gpib -lpthread
LIBS	= -L/usr/local/lib/ -lusb220_gpib -lpthread ../libtkb/export/libtkb.a 

CFLAGS	=-O -g -Wall
INCLUDES=
CC	=/usr/bin/gcc
CPP	=/usr/bin/g++

TARGET	=srvPowermeter
AR      =ar
ARFLAGS =rcs

OBJS	=penguin_powermeter.o
 
all:	
	#make ${TARGET}
	make libpenguinpowermeter.a

${TARGET}: ${OBJS}
	g++ ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

libpenguinpowermeter.a: penguin_powermeter.o
	${AR} ${ARFLAGS} libpenguinpowermeter.a penguin_powermeter.o


.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: powermeterTest.o penguin_powermeter.o powermeter.o
	${CPP} -o powermeterTest powermeterTest.o penguin_powermeter.o ${LIBS}
	${CPP} -o powermeter powermeter.o penguin_powermeter.o ${LIBS}

install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET}
