CC = gcc

LIBS	= -lncurses /usr/lib/edreapi.so
CFLAGS	=-O -g -Wall
INCLUDES= -lncurses /usr/lib/edreapi.so -I/usr/edre/include -DLINUX

OBJS =commands.o dio.o

# All Target
all: $(OBJS) test6

# Tool invocations

test6: test6.o commands.o dio.o
	$(CC)  -o $@ test6.o $(OBJS) $(LIBS)

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

# Other Targets
clean:
	rm -f core *.o *.BAK *.ti *.ii
	rm -f ${OBJS}
