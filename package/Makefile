# 32bit コンパイル用の共通フラグ
CFLAGS=-m32
LDFLAGS=-m32

all:
	make 32bit

32bit:
	make -C libtkb/
	make -C libcssl-0.9.4/
	make -C glacier/serial/
	make -C cosmoslibs_ssl2+trk/libsrc/
	make -C tracking/
	make -C weather/
	make -C weather/ test
	make -C weather/ srv
	make -C signalG/
	make -C signalG/ test
	make -C signalG/ srv
	make -C rpcomfit/
	make -C motor_rpcomfit/
	make -C motor_rpcomfit/ test
	make -C ant30/
	rm -f 64bit
	touch 32bit
	mkdir -p ../tcs01/bin
	mkdir -p ../tcs01/log/ant30
	cp motor_rpcomfit/test_penmotor ../tcs01/bin/
	cp weather/test_penweather ../tcs01/bin/
	cp weather/srv_penweather ../tcs01/bin/
	cp signalG/signalGTest ../tcs01/bin/
	cp signalG/srvSignalG ../tcs01/bin/
	cp ./ant30/tkb32 ../tcs01/bin/ant30
	cp ./ant30/makeSch ../tcs01/bin/makeSch
	cp 32bit ../tcs01/bin/32bit
	rm -rf ../tcs01/bin/64bit

clean:
	make -C libtkb/ clean
	make -C libcssl-0.9.4/ clean
	make -C glacier/serial/ clean
	make -C cosmoslibs_ssl2+trk/libsrc/ clean
	make -C tracking/ clean
	make -C weather/ clean
	make -C signalG/ clean
	make -C rpcomfit/ clean
	make -C motor_rpcomfit/ clean
	make -C ant30/ clean
	rm -f 32bit
	rm -f 64bit

64bit:
	CFLAGS=-m64 LDFLAGS=-m64 make -C libtkb/
	CFLAGS=-m64 LDFLAGS=-m64 make -C libcssl-0.9.4/
	CFLAGS=-m64 LDFLAGS=-m64 make -C glacier/serial/
	CFLAGS=-m64 LDFLAGS=-m64 make -C cosmoslibs_ssl2+trk/libsrc/
	CFLAGS=-m64 LDFLAGS=-m64 make -C tracking/
	CFLAGS=-m64 LDFLAGS=-m64 make -C weather/
	CFLAGS=-m64 LDFLAGS=-m64 make -C signalG/
	CFLAGS=-m64 LDFLAGS=-m64 make -C rpcomfit/
	CFLAGS=-m64 LDFLAGS=-m64 make -C motor_rpcomfit/
	CFLAGS=-m64 LDFLAGS=-m64 make -C motor_rpcomfit/ test
	CFLAGS=-m64 LDFLAGS=-m64 make -C ant30/
	rm -f 32bit
	touch 64bit
	mkdir -p ../tcs01/bin
	mkdir -p ../tcs01/log/ant30
	cp motor_rpcomfit/test_penmotor ../tcs01/bin/test_penmotor
	cp ./ant30/tkb32 ../tcs01/bin/ant30
	cp ./ant30/makeSch ../tcs01/bin/makeSch
	cp 64bit ../tcs01/bin/64bit
	rm -rf ../tcs01/bin/32bit
