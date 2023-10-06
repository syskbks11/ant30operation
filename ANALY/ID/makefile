LIBS	= ../lib/libStarFits.a

CFLAGS	= -O -lm -I../h -D_NO_PROTO
XFLAGS	= -lXm -lXt -lXlc -lX11

CC	= gcc


Fsms : Fsms.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

cloudID : cloudID.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

cloudPRM : cloudPRM.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

simID : simID.c $(LIBS)
	$(CC) $@.c $(LIBS) -o $@ $(CFLAGS)
	\mv $@ ../bin 

all:	Fsms cloudID cloudPRM simID

clean:;	rm -f *.o cloudID 

	

###
