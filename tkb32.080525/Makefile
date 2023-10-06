LDFLAGS =
LIBS	=-L/usr/local/trk45/lib -ltrk \
	-L/usr/lib -lnsl -lm -lc -lpthread \
	-L/usr/local/FFC/lib -lssl2 -lstd6b \
	../libtkb/export/libtkb_ffc.a
CFLAGS	=-O
INCLUDES=-I/usr/local/FFC/include \
	-I/usr/local/trk45/include
FC	=/usr/local/FFC/bin/frt
CC	=/usr/local/FFC/bin/fcc
CCP	=/usr/local/FFC/bin/FCC
TARGET	=tkb32
OBJS	=errno.o weath.o trk.o chop.o fe.o if.o be.o tkb32Func.o tkb32.o
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

install : all
	mkdir -p ../../bin
	mkdir -p ../../log
	cp ${TARGET} ../../bin/.
	cp ${MAKESCH} ../../bin/.

clean   :
	/bin/rm -f compileDate.h
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET} ${MAKESCH}
