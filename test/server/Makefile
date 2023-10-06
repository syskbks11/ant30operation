CC = gcc
CCP	=gcc
#CCP	=/usr/local/FFC/bin/FCC

LIBS	=-L/usr/lib -lnsl -lm -lc ../../libtkb/export/libtkb.a
CFLAGS	= -O -g -Wall
INCLUDES	= -I/usr/include -I/usr/local/include

OBJS = server.o

# All Target
all: ${OBJS} server

# Tool invocations
server:server.o
	${CCP}  -o $@ server.o ${LIBS}
	
.cpp.o	:
	${CCP} ${CFLAGS} ${INCLUDES} -c $<
.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

# Other Targets
clean:
	rm -f core *.o *.BAK *.ti *.ii
	rm -f ${OBJS}
