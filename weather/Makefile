LDFLAGS =
LIBS	= ../libtkb/export/libtkb_ffc.a

CFLAGS	=-O -g -Wall
INCLUDES=
CC	=/usr/local/FFC/bin/fcc
CPP	=/usr/local/FFC/bin/FCC

TARGET	=srvWeather
AR      =ar
ARFLAGS =rc

OBJS	=penguin_weather.o
 
all:	
	#make ${TARGET}
	make libpenguinweather.a

${TARGET}: ${OBJS}
	g++ ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

libpenguinweather.a: penguin_weather.o
	${AR} ${ARFLAGS} libpenguinweather.a penguin_weather.o


.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: weatherTest.o penguin_weather.o
	${CPP} -o weatherTest weatherTest.o penguin_weather.o ${LIBS}

install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET}
