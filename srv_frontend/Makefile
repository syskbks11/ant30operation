CC	=/usr/bin/gcc
CPP	=/usr/bin/g++
#CC	=/usr/local/FFC/bin/fcc
#CPP	=/usr/local/FFC/bin/FCC

LIBS = ../rpcomfit/librpcomfit.a ../libtkb/export/libtkb.a ../gauge_rpcomfit/libpenguingauge.a ../lakeshore_rpcomfit/libpenguinlakeshore.a -lpthread

CFLAGS	=-g -Wall
INCLUDES=-I../rpcomfit/
AR      =ar
ARFLAGS =rc

OBJS = srv_frontend.o
TARGET = srv_frontend

all:
	make ${TARGET}

${TARGET}: ${OBJS}
	${CPP} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

test: test_penfrontend.o
	${CPP} -o test_penfrontend test_penfrontend.o ${LIBS}

.cpp.o	:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<
.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

install : all

clean:
	rm -f *.o test_penfrontend srv_penfrontend *~ ${TARGET}


