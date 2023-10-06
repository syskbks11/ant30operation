CC = gcc
LIBS = -lcurses
CFLAGS = -Wall -O
INCLUDES =
AR      =ar
ARFLAGS =rc


all: test_dummysource libglaciermonitor.a test_glacier
	
libglaciermonitor.a: glacier_monitor.o
	${AR} ${ARFLAGS} $@ glacier_monitor.o

test_dummysource:test_dummysource.o
	$(CC) -o $@ $@.o $(LIBS)
test_glacier: test_glacier.o glacier_monitor.o
	$(CC) -o $@ $@.o glacier_monitor.o $(LIBS)


.c.o:
	$(CC) ${CFLAGS} ${INCLUDES} -c $<
clean:
	rm *.o
