CC = gcc

LIBS	= -l pthread /usr/lib/edreapi.so
#LIBS	= -lncurses /usr/local/lib/edreapi.so
CFLAGS	=-O -g -Wall
INCLUDES= /usr/lib/edreapi.so -I/usr/edre/include -DLINUX
#INCLUDES= -lncurses /usr/local/lib/edreapi.so -I/usr/local/edre/include -DLINUX

OBJS =motordriver.o dio.o

# All Target
all: $(OBJS) test7 dummymotordriver

# Tool invocations
dummymotordriver:dummymotordriver.o
	$(CC)  -o $@ dummymotordriver.o motordriver.o $(LIBS)
	

test7: test7.o $(OBJS)
	$(CC)  -o $@ test7.o $(OBJS) $(LIBS)

.c.o	:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

# Other Targets
clean:
	rm -f core *.o *.BAK *.ti *.ii
	rm -f ${OBJS}
