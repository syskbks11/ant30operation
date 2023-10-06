#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <stdint.h>

#include "penguin_thermocon.h"

#define BUF_LEN 100000


/*
 * singleton, since we have only one chopper motor. 
 */
typedef struct penguin_thermocon_s{
	int id;
	char message[256];
}penguin_thermocon_t;

static penguin_thermocon_t p;

static int _receive(int id);
//static int _send_receive(int id, char* buf, const unsigned int bufSize);
//static int _send_receive(const char* cmd, int length);
static void _buffered_write(int id, const char* buf, const unsigned int size);
static int _send_frame(char const* cmd, int length);
static int _receive_more(int id, int length);
static int _isMessageCompleted();

int penguin_thermocon_init(const char* devName){
	/* open device file */
	const int fd = open(devName,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。
	if(fd<0){
		// デバイスの open() に失敗したら
		perror(devName);
		exit(1);
	}
	printf("%s opened as a penguin_thermocon.\n", devName);

	/* configure the port */
	struct termios tio, oldtio;//080905 in
//    struct termios tio;//080905 out
	tcgetattr(fd, &oldtio); //080905 in; 現在の設定を退避

	memset(&tio,0,sizeof(tio));
	tio.c_cflag = CS8 | CLOCAL | CREAD;
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
/*
	printf("%s\n", "penguin_weather_init() returns. ");
*/
	return 0;
}

int penguin_thermocon_end(){
	if(close(p.id))
		printf("some trouble when close.");

	return 0;
}


int penguin_thermocon_getPresetTemperature(){
	if(penguin_thermocon_send_get_command(0x31)){
		printf("penguin_thermocon_getPresetTemperature(); send_get_command failed.\n");
		return NULL;
	}

	int len = penguin_thermocon_receive();
	if(p.message[0] != 0x02){
		printf("penguin_thermocon_getPresetTemperature(); received data (%2X) is not expected value(02).\n", p.message[0]);
		return NULL;
	}
	if(p.message[1] != 0x31){
		printf("penguin_thermocon_getPresetTemperature(); received data (%2X) is not expected value(31).\n", p.message[1]);
		return NULL;
	}
	if(len != 10){
		printf("penguin_thermocon_getPresetTemperature(); received data size (%d) is not expected value(10).\n", len);
		if(len < 6)
			return NULL;
	}
	int res, n;
	n = sscanf(p.message+2, "%4d0x03", &res);
	if(n <= 0){
		printf("penguin_thermocon_getPresetTemperature(); received data do not represents number.\n");
		return NULL;
	}
	return res;
}
int penguin_thermocon_getInsideTemperature(){
	if(penguin_thermocon_send_get_command(0x32)){
		printf("penguin_thermocon_getInsideTemperature(); send_get_command failed.\n");
		return NULL;
	}

	int len = penguin_thermocon_receive();
	if(p.message[0] != 0x02){
		printf("penguin_thermocon_getInsideTemperature(); received data (%2X) is not expected value(02).\n", p.message[0]);
		return NULL;
	}
	if(p.message[1] != 0x32){
		printf("penguin_thermocon_getInsideTemperature(); received data (%2X) is not expected value(32).\n", p.message[1]);
		return NULL;
	}
	if(len != 10){
		printf("penguin_thermocon_getInsideTemperature(); received data size (%d) is not expected value(10).\n", len);
		if(len < 6)
			return NULL;
	}
	int res, n;
	n = sscanf(p.message+2, "%4d0x03", &res);
	if(n <= 0){
		printf("penguin_thermocon_getInsideTemperature(); received data do not represents number.\n");
		return NULL;
	}
	return res;
}
int penguin_thermocon_getOutsideTemperature(){
	if(penguin_thermocon_send_get_command(0x33)){
		printf("penguin_thermocon_getOutsideTemperature(); send_get_command failed.\n");
		return NULL;
	}

	int len = penguin_thermocon_receive();
	if(p.message[0] != 0x02){
		printf("penguin_thermocon_getOutsideTemperature(); received data (%2X) is not expected value(02).\n", p.message[0]);
		return NULL;
	}
	if(p.message[1] != 0x33){
		printf("penguin_thermocon_getOutsideTemperature(); received data (%2X) is not expected value(33).\n", p.message[1]);
		return NULL;
	}
	if(len != 10){
		printf("penguin_thermocon_getOutsideTemperature(); received data size (%d) is not expected value(10).\n", len);
		if(len < 7)
			return NULL;
	}
	int res, n;
	n = sscanf(p.message+2, "%4d0x03", &res);
	if(n <= 0){
		printf("penguin_thermocon_getOutsideTemperature(); received data do not represents number.\n");
		return NULL;
	}
	return res;
}
int penguin_thermocon_getAlerm(){
	if(penguin_thermocon_send_get_command(0x34)){
		printf("penguin_thermocon_getAlerm(); send_get_command failed.\n");
		return NULL;
	}

	int len = penguin_thermocon_receive();
	if(p.message[0] != 0x02){
		printf("penguin_thermocon_getAlerm(); received data (%2X) is not expected value(02).\n", p.message[0]);
		return NULL;
	}
	if(p.message[1] != 0x34){
		printf("penguin_thermocon_getAlerm(); received data (%2X) is not expected value(34).\n", p.message[1]);
		return NULL;
	}
	if(len != 9){
		printf("penguin_thermocon_getAlerm(); received data size (%d) is not expected value(9).\n", len);
		if(len < 6)
			return NULL;
	}
	int res, n;
	n = sscanf(p.message+2, "%3d0x03", &res);
	if(n <= 0){
		printf("penguin_thermocon_getAlerm(); received data do not represents number.\n");
		return NULL;
	}
	return res;
}



int penguin_thermocon_send_get_command(char cmd){
	char frame[256];
	frame[0] = 0x05;
	frame[1] = cmd;
	frame[2] = 0x33;
	frame[3] = cmd;
	frame[4] = 0x0D;
	frame[5] = 0x00;

	return _send_frame(frame, 5);
}

int penguin_thermocon_send_set_command(char cmd, int value10){
	char frame[256];
	frame[0] = 0x02;
	frame[1] = cmd;
	sprintf(frame+2, "%3d", value10);
	frame[5] = 0x30;
	frame[6] = 0x03;
	unsigned char sum = 0;
	int i;
	for(i = 1; i < 6; i++){
		sum += frame[i];
	}
	printf("sum=%d", sum);
	sprintf(frame+7, "%2X", sum);
	if(frame[7] > 0x40){
		frame[7] -= 7;
	}
	if(frame[8] > 0x40){
		frame[8] -= 7;
	}
	frame[9] = 0x0D;

	printf("set_command(");
	for(i = 0; i < 10; i++){
		printf("%x ", frame[i]);
	}
	printf(")\n");

	return _send_frame(frame, 10);
}

int penguin_thermocon_send_frame(const char* cmd, int length){
	//printf("penguin_weather_send_command(%s, %d)\n", cmd, length);
	return _send_frame(cmd, length);
}

int _send_frame(const char* cmd, int length){
/*
	printf("_send_command(");
	int i = 0;
	for(i = 0; i < length; i++){
		printf("%x ", cmd[i]);
	}
	printf(")\n");
*/
	//int len = strlen(cmd);
	_buffered_write(p.id, cmd, length);
	return 0;

}


int penguin_thermocon_receive(){
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
			printf("penguin_thermocon_receive(); timeout.\n");
			fflush(stdout);//090113 in
			break;//090113 in
		}
		usleep(1000*50);
		i++;//081226 in
	}
	return res;
}

char* penguin_thermocon_getMassage(){
	return p.message;

}

int _isMessageCompleted(){
	return (int)strstr(p.message, "\x0D");
}

/*
void penguin_weather_awake(){
	_buffered_write(p.id, "\x00", 1);
}
*/

void _buffered_write(int id, const char* buf, const unsigned int size){
//void _buffered_write(int id, uint8_t* buf, const unsigned int size){
	//printf("_buffered_write(%d)\n", id);

	const char *p = buf;
	const char * const endp = p + size;
//    const unsigned char *p = buf;
//	const unsigned char * const endp = p + size;

	while (p < endp) {
		int num_bytes = write(id, p, endp - p);
		if (num_bytes < 0) {
 			perror("write failed");
    			break;
		}
		p += num_bytes;
  	}

/*
	uint8_t* tmp = (uint8_t*)malloc(sizeof(uint8_t) * size);
	memcpy(tmp, buf, size);
	glacier_serial_putdata(id, tmp, size);
	free(tmp);
*/
	//glacier_serial_putdata(id, (uint8_t*)buf, size);
}

/*
int _send_receive(const char* cmd, int length){

//	printf("_send_receive(%s)\n", cmd);


	int ret = _send_frame(cmd, length);
	if(ret){
		printf("_send_receive(); send failed.\n");
		return ret;
	}
	return _receive(p.id);
}
*/

int _receive(int id){
	//printf("_receive(%d)\n", id);
	int len;                            //  受信データ数（バイト）
	p.message[0] = 0;//090218 in
	// ここで受信待ち
	len = read(id, p.message, 256);
	//printf("%s\n", "data recieved. ");
	if(len < 0){
		perror("send_receive(): IO error!");
		return len;
	}
	// 受信したデータを 16進数形式で表示 
/*
	int i;   
        for(i=0; i<len; i++){
//           printf("%02X ",p.message[i]);
           printf("%02x,",p.message[i]);
       }
*/
	//buf[0] = 'x';
	/* 受信したデータ*/
/*
	p.message[len] = 0;
	printf("[]%s:%d\n", p.message, len);
*/
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
		perror("send_receive(): IO error!");
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

