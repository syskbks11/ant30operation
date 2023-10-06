CC = gcc

LIBS	=
CFLAGS	=-g -Wall
INCLUDES= 
AR      =ar
ARFLAGS =rc

OBJS = glacier_debug.o
TARGET	=libglacierdebug.a

all: ${OBJS}
	${AR} ${ARFLAGS} ${TARGET} ${OBJS}

test: glacier_debug.o
	$(CC)  -o $@ $@.o $(OBJS) $(LIBS)

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<
	

clean: 
	rm -f *.o test *~
