LDFLAGS =
LIBS	=-L/usr/local/trk45/lib -ltrk \
	../libtkb/export/libtkb_ffc.a \
	-L/usr/lib -lnsl -lm -lc -lpthread \
	-L/usr/local/FFC/lib -lssl2 -lstd6b
CFLAGS	=-O
INCLUDES=-I/usr/local/FFC/include \
	-I/usr/local/trk45/include
FC	=/usr/local/FFC/bin/frt
CC	=/usr/local/FFC/bin/fcc
CCP	=/usr/local/FFC/bin/FCC
AR      =ar
ARFLAGS =rc

TARGET	=tracking_caluculator
TARGET2	=libpenguintracking.a

OBJS	=glacier_celestialObject.o glacier_trackingInformation.o glacier_scanOffset.o penguin_tracking.o tracking_calculator.o


all:	
	echo "#define COMPILE_DATE "\""`date +%Y/%m/%d-%H:%M:%S`"\" > compileDate.h
	make ${TARGET}
	make ${TARGET2}
${TARGET2}: ${OBJS}
	${AR} ${ARFLAGS} $@ glacier_celestialObject.o glacier_trackingInformation.o glacier_scanOffset.o penguin_tracking.o

${TARGET}: ${OBJS} compileDate.h
	${FC} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

.cpp.o	:
	${CCP} ${CFLAGS} ${INCLUDES} -c $<

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<


clean   :
	/bin/rm -f compileDate.h
	/bin/rm -f core *.o *.BAK *.ti *.ii
	/bin/rm -f ${TARGET} ${MAKESCH}
