LDFLAGS =
LIBS	= ../libtkb/export/libtkb.a

CFLAGS	=-O -g -Wall
INCLUDES=
CC	=/usr/bin/gcc
CPP	=/usr/bin/g++

TARGET	=srvWeather2
AR      =ar
ARFLAGS =rc

OBJS	=penguin_weather2.o srvWeather2.o
 
all:	
	make ${TARGET}
	make libpenguinweather2.a

${TARGET}: ${OBJS}
	g++ ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

libpenguinweather2.a: penguin_weather2.o
	${AR} ${ARFLAGS} libpenguinweather2.a penguin_weather2.o


.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: weather2Test.o penguin_weather2.o
	${CPP} -o weather2Test weather2Test.o penguin_weather2.o ${LIBS}

install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET}
