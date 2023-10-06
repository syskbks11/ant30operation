CC = gcc

LIBS	= -lncurses /usr/lib/edreapi.so
CFLAGS	=-O -g -Wall
INCLUDES= -lncurses /usr/lib/edreapi.so -I/usr/edre/include -DLINUX

OBJS =motordriver.o dio.o

# All Target
all: $(OBJS) test6 dummymotordriver

# Tool invocations
dummymotordriver:dummymotordriver.o
	$(CC)  -o $@ dummymotordriver.o motordriver.o $(LIBS)
	

test6: test6.o $(OBJS)
	$(CC)  -o $@ test6.o $(OBJS) $(LIBS)

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

# Other Targets
clean:
	rm -f core *.o *.BAK *.ti *.ii
	rm -f ${OBJS}
