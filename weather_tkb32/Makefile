LDFLAGS =
LIBS	= ../libtkb/export/libtkb_ffc.a

CFLAGS	=-O -g -Wall
INCLUDES=
CC	=/usr/local/FFC/bin/fcc
CPP	=/usr/local/FFC/bin/FCC


TARGET	=srvWeather
AR      =ar
ARFLAGS =rc

OBJS	=srvWeather.o penguin_weather_tkb32.o
 
all:	
	make ${TARGET}
	make libpenguinweather_tkb32.a


${TARGET}: ${OBJS}
	${CPP} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

libpenguinweather_tkb32.a: penguin_weather_tkb32.o
	${AR} ${ARFLAGS} libpenguinweather_tkb32.a penguin_weather_tkb32.o


.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: weatherTest.o penguin_weather_tkb32.o
	g++ -o weatherTest weatherTest.o penguin_weather_tkb32.o ${LIBS}

install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET}
