LIBS	= ../star/lib/libStarFits.a
CC	= gcc
BIN	= ../star/bin
CFLAGS	= -O -I../star/h -I./ -D_NO_PROTO -lm 


err:	err.c
	$(CC) $@.c -o $@ $(CFLAGS)

tau:	tau.c
	$(CC) $@.c -o $@ $(CFLAGS)

N:	N.c
	$(CC) $@.c -o $@ $(CFLAGS)

Nt:	Nt.c
	$(CC) $@.c -o $@ $(CFLAGS)

ci:	ci.c
	$(CC) $@.c -o $@ $(CFLAGS)

cl:	cl.c
	$(CC) $@.c -o $@ $(CFLAGS)

F:	F.c
	$(CC) $@.c -o $@ $(CFLAGS)

all:	err tau N ci cl Nt F

clean:;	rm -f *.o tau N err ci cl Nt F

###