CC = gcc
LIBS = -lncurses
CFLAGS = -Wall -O
INCLUDES =


all: anime ptest mtest gratest diamond showtime tree graph
	
anime: anime.o
	$(CC) -o $@ $@.o $(LIBS)
ptest: ptest.o
	$(CC) -o $@ $@.o $(LIBS)
mtest: mtest.o
	$(CC) -o $@ $@.o $(LIBS)
gratest: gratest.o
	$(CC) -o $@ $@.o $(LIBS)
diamond: diamond.o
	$(CC) -o $@ $@.o $(LIBS)
showtime: showtime.o
	$(CC) -o $@ $@.o $(LIBS)
tree: tree.o
	$(CC) -o $@ $@.o $(LIBS)
graph: graph.o
	$(CC) -o $@ $@.o $(LIBS)

.c.o:
	$(CC) ${CFLAGS} ${INCLUDES} -c $<
clean:
	rm *.o
