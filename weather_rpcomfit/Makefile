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
	make libpenguinweather.a

${TARGET}: ${OBJS}
	g++ ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

libpenguinweather.a: penguin_weather.o
	${AR} ${ARFLAGS} libpenguinweather.a penguin_weather.o


.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: test_penweath.o penguin_weather.o
	${CPP} -o test_penweath test_penweath.o ${OBJS} ${LIBS}

install : all

clean   :
	/bin/rm -f test_penweath core *.o *.BAK *.ti *.ii *~ ${TARGET}

