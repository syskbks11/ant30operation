.SUFFIXES :	.o .c

SRCS	=	diff_lag.c	\
		inverse.c	\
		sweepout.c	\
		diff_sp.c	\
		lagrange.c	\
		spline.c	\
		fft.c		\
		fft2.c

OBJS	=	diff_lag.o	\
		inverse.o	\
		sweepout.o	\
		diff_sp.o	\
		lagrange.o	\
		spline.o	\
		fft.o		\
		fft2.o


LIBS	=	nucalc.a

CFLAGS	=	-O -I../h -D_NO_PROTO

CC	= 	gcc

$(LIBS):	$(OBJS)
	ar ruv $@ $?
	ranlib $@

.c.o:
	$(CC) -c $< $(CFLAGS) $(PROTO) -o $*.o

clean:;		rm -f $(OBJS) *.a

###
