LIBS	= ../lib/libstar.a ../lib/libStarFile.a 

CFLAGS	= -O -lm -I../h -D_NO_PROTO
XFLAGS	= -lXm -lXt -lXlc -lX11

CC	= cc

grid : grid.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv grid ../bin

clean:;	rm -f *.o

	

###
