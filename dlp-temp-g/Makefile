CC = gcc

LIBS	= ../libcssl-0.9.4/libcssl.a ../serial/libglacierserial.a ../debug/libglacierdebug.a 
CFLAGS	=-g -Wall
INCLUDES= 

OBJS = 



test: test.o
	$(CC)  -o $@ $@.o $(OBJS) $(LIBS)

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<
	

clean: 
	rm -f *.o test *~
