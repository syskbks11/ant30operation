LIBS	= ../../star/lib/libStarFits.a
CC	= gcc
BIN	= ../../star/bin
CFLAGS	= -O -I../../star/h -I./ -D_NO_PROTO -lm 

tau.o:
	$(CC) -c tau.c -o tau.o -O

tex.o:
	$(CC) -c tex.c -o tex.o -O

taut:	taut.c tau.o
	$(CC) taut.c tau.o -o taut $(CFLAGS)

text:	text.c tex.o
	$(CC) text.c tex.o -o text $(CFLAGS)

tex:	tex.c
	$(CC) $@.c -o $@ $(CFLAGS)

Ftex:	Ftex.c tau.o tex.o
	$(CC) $@.c tau.o tex.o $(LIBS) -o $@ $(CFLAGS)
	mv $@ $(BIN)

Fcol:	Fcol.c tau.o tex.o
	$(CC) $@.c tau.o tex.o $(LIBS) -o $@ $(CFLAGS)
	mv $@ $(BIN)

Ftau:	Ftau.c tau.o
	$(CC) $@.c tau.o $(LIBS) -o $@ $(CFLAGS)
	mv $@ $(BIN)

Fnn:	Fnn.c
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ $(BIN)

N:	N.c
	$(CC) $@.c -o $@ $(CFLAGS)

Ns:	Ns.c  tau.o tex.o
	$(CC) $@.c tau.o tex.o -o $@ $(CFLAGS)

tmb:	tmb.c
	$(CC) $@.c -o $@ $(CFLAGS)	

error:	error.c
	$(CC) $@.c -o $@ $(CFLAGS)

all:	taut text Ftex Fcol Ftau N error

clean:;	rm -f *.o taut text  Ftex Fcol Ftau N tmb error

###