LIBS	= ../lib/libStarFits.a ../lib/libstar.a ../nucalc/nucalc.a

CFLAGS	= -O -I../h -I./ -D_NO_PROTO -lm 
CCFLAGS	= -O -I../h -I./ -D_NO_PROTO
XFLAGS	= -lXm -lXt -lXlc -lX11

CC	= gcc

gfit	: gfit.c gsfit.o 	
	$(CC) $@.c gsfit.o $(LIBS) -o $@ $(CFLAGS)
#	\mv $@ ../bin 

gsfit.o	: gsfit.c
	$(CC) -c gsfit.c $(CCFLAGS)

glfit	: glfit.c 	
	$(CC) %@.c $(LIBS) -o $@ $(CFLAGS)
#	\mv $@ ../bin 

Fgfit   : Fgfit.c gsfit.o $(LIBS)
	$(CC) $@.c gsfit.o $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

#Fvint   : Fvint.c $(LIBS)
#	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
#	\mv $@ ../bin 

Fwing   : Fwing.c gsfit.o $(LIBS)
	$(CC) $@.c gsfit.o $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

#Fint   : Fint.c $(LIBS)
#	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
#	\mv $@ ../bin 

Frad   : Frad.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

#Fint2   : Fint2.c $(LIBS)
#	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
#	\mv $@ ../bin 

Ftest   : Ftest.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

all:	gfit glfit Fgfit Fwing Frad Ftest

clean:;	rm -f *.o

	

###
