LDFLAGS =
LIBS	= ../powermeter/libpenguinpowermeter.a  -lusb220_gpib ../libtkb/export/libtkb.a -lpthread
#LIBS	= ../libtkb/export/libtkb_ffc.a

CFLAGS	=-O -g -Wall
INCLUDES=
CC	=/usr/bin/gcc
CPP	=/usr/bin/g++
#CC	=/usr/local/FFC/bin/fcc
#CPP	=/usr/local/FFC/bin/FCC

TARGET	=srvPower

OBJS	=main.o srvPower.o
 
all:	
	make ${TARGET}

${TARGET}: ${OBJS}
	g++ ${LDFLAGS} -o $@ ${OBJS} ${LIBS}


.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<


install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET}
