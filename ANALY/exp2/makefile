CC	= gcc
CFLAGS	= -O -lm 

ocsfit:	ocsfit.c
	$(CC) ocsfit.c -o $@ $(CFLAGS)

r1r2:	r1r2.c
	$(CC) r1r2.c -o $@ $(CFLAGS)

rexp:	rexp.c
	$(CC) rexp.c -o $@ $(CFLAGS)

clean:;	rm -f *.o ocsfit r1r2 rexp

###