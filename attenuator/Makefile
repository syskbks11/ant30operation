CC = gcc

LIBS	= -L/usr/local/lib/ -lusb220_gpib -lpthread
CFLAGS	=-O -g -Wall
INCLUDES= 

OBJS = glacier_att.o



test: test.o glacier_att.o
	$(CC)  -o $@ $@.o $(OBJS) $(LIBS)

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<
	

clean: 
	rm -f *.o gpibctrl *~
