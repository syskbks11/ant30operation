CC = gcc
LIBS = -lcurses ../glacier/monitor/libglaciermonitor.a
CFLAGS = -Wall -O
INCLUDES =


all: heaterMonitor
	
heaterMonitor: heaterMonitor.o
	$(CC) -o $@ $@.o $(LIBS)

.c.o:
	$(CC) ${CFLAGS} ${INCLUDES} -c $<
clean:
	rm *.o
