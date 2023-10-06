LIBS	= ../lib/libStarFits.a
LIBS1	= ../lib/libStarFits.a ../lib/libstar.a
#LIBS2	= ../lib/libStarFits.a ../../ASTRO/lib/libAstro.a
LIBS2   = ../lib/libStarFits.a ../lib/libAstro.a
#LIBS3	= ../lib/libStarFits.a ../../lib/nucalc.a
LIBS3  = ../lib/libStarFits.a ../nucalc/nucalc.a
XLIB	= -L/usr/X11R6/lib -lX11
PGLIB	= -L/usr/local/pgplot -lcpgplot -lpgplot

PGINC	= /usr/local/pgplot

CFLAGS	= -O -I../h -I./ -D_NO_PROTO -lm
#XFLAGS	= -lXm -lXt -lXlc -lX11

CC	= gcc
FC	= g77


Fplot2.o :	Fplot2.c 
	$(CC) -c Fplot2.c -I$(PGINC) -O -I../h -I./ -D_NO_PROTO

Fplot2 :	Fplot2.o $(LIBS)
	$(FC) -o $@ $@.o $(LIBS) $(PGLIB) $(XLIB) -I$(PGINC)
	\mv $@ ../bin 

Fplot.o :	Fplot.c 
	$(CC) -c Fplot.c -I$(PGINC) -O -I../h -I./ -D_NO_PROTO

Fplot :	Fplot.o $(LIBS)
	$(FC) -o $@ $@.o $(LIBS) $(PGLIB) $(XLIB) -I$(PGINC)
	\mv $@ ../bin 

WtoF   : WtoF.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fprs   : Fprs.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fflip   : Fflip.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fbase   : Fbase.c $(LIBS3)
	$(CC) $@.c $(LIBS3) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fvint   : Fvint.c $(LIBS1)
	$(CC) $@.c $(LIBS1) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Frot   : Frot.c $(LIBS1)
	$(CC) $@.c $(LIBS1) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fsft   : Fsft.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fcorr   : Fcorr.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fpp   : Fpp.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fblnk	: Fblnk.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS) 
	\mv $@ ../bin 

Flbrd	: Flbrd.c $(LIBS)
	$(CC) $@.c $(LIBS2) -o $@ $(CFLAGS) -I../../ASTRO/h
	\mv $@ ../bin 

Feqx   : Feqx.c $(LIBS)
	$(CC) $@.c $(LIBS2) -o $@ $(CFLAGS) -I../../ASTRO/h
	\mv $@ ../bin 

Fpk   : Fpk.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fpv:	Fpv.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

Fval   : Fval.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fdv   : Fdv.c $(LIBS1)
	$(CC) $@.c $(LIBS1) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fdisp   : Fdisp.c $(LIBS1)
	$(CC) $@.c $(LIBS1) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fsdisp   : Fsdisp.c $(LIBS1)
	$(CC) $@.c $(LIBS1) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fcut   : Fcut.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fclip   : Fclip.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fsmv   : Fsmv.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

FJtoF   : FJtoF.c $(LIBS2)
	$(CC) $@.c $(LIBS2) -o $@ $(CFLAGS)
	\mv $@ ../bin 

FCfAtoF   : FCfAtoF.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

F45toF   : F45toF.c $(LIBS1)
	$(CC) $@.c $(LIBS1) -o $@ $(CFLAGS)
	\mv $@ ../bin 

FNMAtoF   : FNMAtoF.c $(LIBS1)
	$(CC) $@.c $(LIBS1) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fsm   : Fsm.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fsm1   : Fsm1.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

FtoH   : FtoH.c $(LIBS1)
	$(CC) $@.c $(LIBS1) -o $@ $(CFLAGS)
	\mv $@ ../bin 

Fff   : Fff.c $(LIBS1)
	$(CC) $@.c $(LIBS1) -o $@ $(CFLAGS)
	\mv $@ ../bin 
	
FdivF : FdivF.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

F2toF3 : F2toF3.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

F3toF2 : F3toF2.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

FtoM : FtoM.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

FtoC: FtoC.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

FtoCx: FtoCx.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

F2toC: F2toC.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

Fspec:	Fspec.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

Fxy:	Fxy.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

Fsc:	Fsc.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

Fint:	Fint.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

Fsint:	Fsint.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

Fcint:	Fcint.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

Ftest:	Ftest.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	mv $@ ../bin

MtoC:	MtoC.c
	$(CC) $@.c -o $@ $(CFLAGS)
	mv $@ ../bin

MtoF:	MtoF.c $(LIBS) 
	$(CC) $@.c -o $@ $(LIBS) $(CFLAGS)
	mv $@ ../bin

Mint:	Mint.c
	$(CC) $@.c -o $@ $(CFLAGS)
	mv $@ ../bin

all:	Fplot2 Fplot WtoF Fspec Fprs Fflip Fbase Frot Fsft Fcorr Fblnk Fpp Fpv Fpk Fval F45toF FNMAtoF Fint Fdv Fdisp Fcut Fsmv Fsm FtoH Fff FdivF F2toF3 F3toF2 FtoM FtoC FtoCx F2toC Fxy Fsc Fint MtoC MtoF Mint 
#all:	Fplot2 Fplot WtoF Fspec Fprs Fflip Fbase Frot Fsft Fcorr Fblnk Feqx Flbrd Fpp Fpv Fpk Fval F45toF FNMAtoF Fint Fdv Fdisp Fcut Fsmv FJtoF Fsm FtoH Fff FdivF F2toF3 F3toF2 FtoM FtoC FtoCx F2toC Fxy Fsc Fint MtoC MtoF Mint 

clean:;	rm -f *.o

	

###
