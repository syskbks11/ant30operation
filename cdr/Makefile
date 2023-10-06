CC = gcc
LIBS = -lncurses
CFLAGS = -Wall -O
INCLUDES =


all: mushi char fsca search sequence map color
	
mushi: mushi.o
	$(CC) -o $@ $@.o $(LIBS)
char: char.o
	$(CC) -o $@ $@.o $(LIBS)
map: map.o
	$(CC) -o $@ $@.o $(LIBS)
color: color.o
	$(CC) -o $@ $@.o $(LIBS)
fsca: fsca.o
	$(CC) -o $@ $@.o $(LIBS)
search: search.o
	$(CC) -o $@ $@.o $(LIBS)
sequence: sequence.o
	$(CC) -o $@ $@.o $(LIBS)

.c.o:
	$(CC) ${CFLAGS} ${INCLUDES} -c $<
clean:
	rm *.o
