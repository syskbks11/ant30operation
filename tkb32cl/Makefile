LIBS	=../libtkb/export/libtkb.a \
	-L/usr/lib -lnsl -lm -lc -lpthread -lcurses
CFLAGS	=-O -g
INCLUDES=-I/usr/include -I/usr/local/include
CC	=/usr/bin/gcc
CCP	=/usr/bin/g++
AR      =ar
ARFLAGS =rc

TARGET	=tkb32clTest
TARGET2	=libtkb32cl.a

OBJS	=schedule.o obstablePath.o tkb32clObservationFunc.o tkb32clFunc.o tkb32clPort.o errno.o


all:	
	echo "#define COMPILE_DATE "\""`date +%Y/%m/%d-%H:%M:%S`"\" > compileDate.h
	make ${TARGET}
	make ${TARGET2}
${TARGET2}: ${OBJS}
	${AR} ${ARFLAGS} $@ ${OBJS}

${TARGET}: tkb32clTest.o ${OBJS} compileDate.h
	${CCP} -o $@ $@.o ${OBJS} ${LIBS}

.cpp.o	:
	${CCP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<


clean   :
	/bin/rm -f compileDate.h
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET} ${MAKESCH}
