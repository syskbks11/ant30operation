.SILENT:
all:
	mkdir -p ../lib ../inc
	make -C ssl2/src
	make -C trk/src
32bit:
	mkdir -p ../lib ../inc
	make -C ssl2/src 32bit
	make -C trk/src 32bit
64bit:
	mkdir -p ../lib ../inc
	make -C ssl2/src 64bit
	make -C trk/src 64bit
clean:
	make -C ssl2/src clean
	make -C trk/src clean
