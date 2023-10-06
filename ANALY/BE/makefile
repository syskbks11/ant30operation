LIBS	= ../../star/lib/libStarFits.a ../../lib/nucalc.a
CC	= gcc
BIN	= ../../star/bin
CFLAGS	= -O -I./ -I../../lib -D_NO_PROTO -lm 


column:	column.c
	$(CC) $@.c -o $@ $(CFLAGS) 

polyfit:	polyfit.c
	$(CC) $@.c -o $@ $(CFLAGS) $(LIBS)

poly:	poly.c
	$(CC) $@.c -o $@ $(CFLAGS)

r2R:	r2R.c
	$(CC) $@.c -o $@ $(CFLAGS)

all:	poly r2R

clean:;	rm -f *.o poly r2R polyfit column

###


