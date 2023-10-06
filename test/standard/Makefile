TARGET = cat logtest
LIBS = -lncurses /usr/lib/edreapi.so -I/usr/edre/include -DLINUX
OPTS = -g -Wall

all: $(TARGET)
$(TARGET): 
	    gcc $(OPTS) -o $@ $@.c $(LIBS)
clean:
	rm -f $(TARGET)
