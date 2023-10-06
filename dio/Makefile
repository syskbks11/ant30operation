app = dio
all: $(app)
$(app): $(app).c
	    gcc -g -Wall -o $(app) $(app).c -lncurses /usr/lib/edreapi.so -I/usr/edre/include -DLINUX
clean:
	rm -f $(app)
