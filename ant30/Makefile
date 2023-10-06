LDFLAGS =
LIBS	=../tracking/libpenguintracking.a \
	-L/usr/local/trk45/lib -ltrk \
	../libtkb/export/libtkb_ffc.a \
	../weather/libpenguinweather.a \
	../glacier/serial/libglacierserial.a \
	../libcssl-0.9.4/libcssl.a \
	../motor/libpenguinmotor.a \
	-L/usr/lib -lnsl -lm -lc -lpthread \
	-L/usr/local/FFC/lib -lssl2 -lstd6b
#	../libtrk/trk45/lib/libtrk.a 
#LIBS	=-L/usr/trk45/lib -ltrk \
#	-L/usr/lib -lnsl -lm -lc -lpthread \
#	-L/usr/FFC/lib -lssl2 -lstd6b \
#	../../import/libtkb_ffc.a
CFLAGS	=-O
INCLUDES=-I/usr/local/FFC/include \
	-I/usr/local/trk45/include \
	-I/usr/include
#INCLUDES=-I/usr/FFC/include \
#	-I/usr/trk45/include
FC	=/usr/local/FFC/bin/frt
CC	=/usr/local/FFC/bin/fcc
CCP	=/usr/local/FFC/bin/FCC
#FC	=/usr/bin/gcc
#CC	=/usr/bin/gcc
#CCP	=/usr/bin/gcc
TARGET	=tkb32
OBJS	=fitprot.o errno.o weath.o weatherMonitor.o acu.o celestialObject.o scanTable.o instrument.o calcBoth.o trk.o chop.o fe.o if.o be.o tkb32Func.o controlBoard.o tkb32Parser.o tkb32.o
#OBJS	=errno.o weath.o weatherMonitor.o acu.o celestialObject.o scanTable.o instrument.o calc.o trk.o chop.o fe.o calc2.o if.o be.o tkb32Func.o tkb32Parser.o tkb32.o
MAKESCH = makeSch

all:
	echo "#define COMPILE_DATE "\""`date +%Y/%m/%d-%H:%M:%S`"\" > compileDate.h
	make ${MAKESCH}
	make ${TARGET}

${MAKESCH}: ${MAKESCH}.o
	${CCP} -o ${MAKESCH} ${MAKESCH}.o ${LIBS}

${TARGET}: ${OBJS} compileDate.h
	${FC} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}
.cpp.o	:
	${CCP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

test: all testAcu.o instrumentTest.o
	${CCP} -o testAcu testAcu.o acu.o ${LIBS}
	${CCP} -o instrumentTest instrumentTest.o instrument.o ${LIBS}

install : all
	mkdir -p ../../tcs01/bin
	mkdir -p ../../tcs01/log
	cp ${TARGET} ../../tcs01/bin/ant30
	cp ${MAKESCH} ../../tcs01/bin/.
	cp instrumentTest ../../tcs01/bin/.

clean   :
	/bin/rm -f compileDate.h
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET} ${MAKESCH}
