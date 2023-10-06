LIBS	= ../lib/libstar.a ../lib/libStarFile.a 

CFLAGS	= -O -lm -I../h -D_NO_PROTO
XFLAGS	= -lXm -lXt -lXlc -lX11

CC	= gcc

Brot : Brot.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)

Rext : Rext.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)

LBext : LBext.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)

Vsum : Vsum.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)

Mint:		Mint.c
		$(CC) $@.c -o $@ $(CFLAGS)
		mv $@ ../bin

MtoC:		MtoC.c
		$(CC) $@.c -o $@ $(CFLAGS)
		mv $@ ../bin


clean:;	rm -f *.o

	

###
