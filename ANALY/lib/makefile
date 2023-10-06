.SUFFIXES :	.o .c

OBJ1	= ProfileIO.o ProfileProc.o Index.o GPlib.o
LIB1	= libstar.a

SRC2	= StdFileExt.c StdBakFile.c DividePrmRec.c
OBJ2	= StdFileExt.o StdBakFile.o DividePrmRec.o
LIB2	= libStarFile.a

SRC3	= libStarFits.c
OBJ3	= libStarFits.o
LIB3	= libStarFits.a

CFLAGS	= -O -I../h -D_NO_PROTO

CC	= gcc

$(LIB1):	$(OBJ1)
	ar ruv $@ $?
	ranlib $@

$(LIB2):	$(SRC2) $(OBJ2)
	ar ruv $@ $?
	ranlib $@

$(LIB3):	$(SRC3) $(OBJ3)
	ar ruv $@ $?
	ranlib $@

.c.o:
	$(CC) -c $< $(CFLAGS) $(PROTO) -o $*.o

all:	$(LIB1) $(LIB2) $(LIB3)
clean:;		rm -f $(OBJ1) $(OBJ2) $(OBJ3)

###
