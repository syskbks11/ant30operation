LDFLAGS =
LIBS	= ../libtkb/export/libtkb.a
#LIBS	= ../libtkb/export/libtkb_ffc.a

CFLAGS	=-O -g -Wall
INCLUDES=
CC	=/usr/bin/g++
CPP	=/usr/bin/g++
#CC	=/usr/local/FFC/bin/fcc
#CPP	=/usr/local/FFC/bin/FCC

TARGET	=srvThermocon
AR      =ar
ARFLAGS =rc

OBJS	=penguin_thermocon.o
 
all:	
	make ${TARGET}
	make libpenguinthermocon.a

${TARGET}: ${OBJS} srvThermocon.o
	g++ ${LDFLAGS} -o $@ $@.o ${OBJS} ${LIBS}

libpenguinthermocon.a: penguin_thermocon.o
	${AR} ${ARFLAGS} libpenguinthermocon.a penguin_thermocon.o


.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: thermoconTest.o penguin_thermocon.o thermocon.o
	${CPP} -o thermoconTest thermoconTest.o penguin_thermocon.o ${LIBS}
	${CPP} -o thermocon thermocon.o penguin_thermocon.o ${LIBS}

install : all

clean   :
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET}
