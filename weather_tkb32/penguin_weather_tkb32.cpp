#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>


#include "penguin_weather_tkb32.h"

#define BUF_LEN 100000


/*
 * singleton, since we have only one weather motor. 
 */
typedef struct penguin_weather_s{
	int id;
	char message[256];
}penguin_weather_t;

static penguin_weather_t p;

static int _receive(int id);
static int _receive_more(int id, int length);
static int _isMessageCompleted();


int penguin_weather_tkb32_init(const char* devName){
/*
	char devName[64];
	sprintf(devName, "/dev/ttyUSB%d", 0);
*/
	/* open device file */
	const int fd = open(devName,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。
	if(fd<0){
		// デバイスの open() に失敗したら
		perror(devName);
		exit(1);
	}
	printf("%s opened as a penguin_weather_tkb32.\n", devName);

	/* configure the port */
	struct termios tio, oldtio;//080905 in
//    struct termios tio;//080905 out
	tcgetattr(fd, &oldtio); //080905 in; 現在の設定を退避

	memset(&tio,0,sizeof(tio));
	tio.c_cflag = CS8 | CLOCAL | CREAD | PARENB| PARODD | CSTOPB;
	tio.c_oflag = 0;//080905 in; rawモード
	tio.c_lflag = 0;//081215 in; no echo
	//tio.c_lflag = ICANON;//080905 in;カノニカル入力
	tio.c_iflag = IGNPAR;
	//tio.c_iflag = ICRNL;//080905 in; CRをNLに変換する。

	tio.c_cc[VTIME] = 100;
	// ボーレートの設定
	cfsetispeed(&tio, B1200);
	cfsetospeed(&tio, B1200);
	// デバイスに設定を行う
	tcflush(fd, TCIFLUSH);//080905 in;モデムラインのクリア（要るか不明）
	tcflush(fd, TCOFLUSH);//080905 in;モデムラインのクリア（要るか不明）
	tcsetattr(fd,TCSANOW,&tio);

/*
	cssl_t *serial;
	const int id = glacier_serial_getIDforNewPort();

	cssl_start();

	serial = cssl_open(devName, glacier_serial_callback_CR, id, 9600, 8, 0, 1);//9600 bps, 8 bit, no parity, 1 stop bit
	if (!serial) {
		printf("%s\n",cssl_geterrormsg());
		return NULL;
	}
	glacier_serial_attach(serial, GLACIER_SERIAL_1ST_NONE);
*/

	p.id = fd;
	memset(p.message, 0, 256);
	printf("%s\n", "penguin_weather_tkb32_init() returns. ");
	return 0;
}

int penguin_weather_tkb32_end(){
	if(close(p.id))
		printf("some trouble when close.");

	return 0;
}

double* penguin_weather_tkb32_getData(){
	static double res[5];

	int len = penguin_weather_tkb32_receive();
	if(len != 28){
		printf("penguin_weather_tkb32_getData(); received data size (%d) is not expected value(28).\n", len);
		len = penguin_weather_tkb32_receive();
		if(len != 28){
			printf("penguin_weather_tkb32_getData(); received data size (%d) is not expected value(28), again.\n", len);
			return NULL;
		}
	}
	int direc;//throw it away.
	sscanf(p.message, "/%2d%4lf%4lf%6lf%3lf%6lf", &direc, res[0], res[1], res[2], res[3], res[4]);
	return res;
}

int penguin_weather_tkb32_getWindDirection(){//Please invoke it after penguin_weather_tkb32_getData(). 
	int res = 0;
	sscanf(p.message, "/%2d", &res);
	return res;
}


int penguin_weather_tkb32_receive(){
	memset(p.message, 0, 256);
	int res = _receive(p.id);
	int i = 0;//081226 in
	while(!_isMessageCompleted()){
		//printf("penguin_weather_receive(); not enough, read again.\n");//090113 in
		//fflush(stdout);//090113 in
		res = _receive_more(p.id, res);
		if(i >= 3){//090216 in
		//if(i >= 2){//090113 in
		//if(i > 20){//081226 in
			printf("penguin_weather_tkb32_receive(); timeout.\n");
			fflush(stdout);//090113 in
			break;//090113 in
		}
		usleep(1000*10);
		i++;//081226 in
	}
	return res;
}

char* penguin_weather_tkb32_getMessage(){
	return p.message;

}

int _isMessageCompleted(){
	return (int)strstr(p.message, "\x0D\x0A");
}


int _receive(int id){
	printf("_receive(%d)\n", id);
	int len;                            //  受信データ数（バイト）
	// ここで受信待ち
	len = read(id, p.message, 256);
	//printf("%s\n", "data recieved. ");
	if(len < 0){
		perror("_receive(): IO error!");
		return len;
	}
	// 受信したデータを 16進数形式で表示 

	int i;   
        for(i=0; i<len; i++){
//           printf("%02X ",p.message[i]);
           printf("%02x,",p.message[i]);
       }

	//buf[0] = 'x';
	/* 受信したデータ*/
	p.message[len] = 0;
	printf("[]%s:%d\n", p.message, len);
	return len;

/*
	glacier_serial_wait_for_massage_complete(id);
	char* tmp = (char*)glacier_serial_get_massage(id);
	int len = strlen(tmp);
	strncpy(buffer, tmp, len+1);
	return len;
*/
}

int _receive_more(int id, int length){
//	printf("_receive_more(%d, %d)\n", id, length);
	int len;                            //  受信データ数（バイト）
	// ここで受信待ち
	len = read(id, p.message+length, 256-length);
	//printf("%s\n", "data recieved. ");
	if(len < 0){
		perror("_receive_more(): IO error!");
		return len;
	}
	// 受信したデータを 16進数形式で表示 
/*
	int i;   
        for(i=0; i<len; i++){
           printf("%02X ",p.message[length+i]);
       }
*/
	//buf[0] = 'x';
	/* 受信したデータ*/
/*
	p.message[length+len] = 0;
	printf("[]%s:%d\n", p.message, len);
*/
	return length+len;

/*
	glacier_serial_wait_for_massage_complete(id);
	char* tmp = (char*)glacier_serial_get_massage(id);
	int len = strlen(tmp);
	strncpy(buffer, tmp, len+1);
	return len;
*/
}


