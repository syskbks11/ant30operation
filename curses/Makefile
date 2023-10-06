CC = gcc
LIBS = -lcurses
CFLAGS = -Wall -O
INCLUDES =
AR      =ar
ARFLAGS =rc


all: demo1 demo2
	make demo1
	make demo2
	make demo3
	make demo4
	make demo5

demo1: demo1.o
	$(CC) -o $@ $@.o $(LIBS)
demo2: demo2.o
	$(CC) -o $@ $@.o $(LIBS)
demo3: demo3.o
	$(CC) -o $@ $@.o $(LIBS)
demo4: demo4.o
	$(CC) -o $@ $@.o $(LIBS)
demo5: demo5.o
	$(CC) -o $@ $@.o $(LIBS) -lm


.c.o:
	$(CC) ${CFLAGS} ${INCLUDES} -c $<
clean:
	rm *.o
