#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>


#include "penguin_gauge.h"

#define BUF_LEN 100000


/*
 * singleton, since we have only one chopper motor. 
 */
typedef struct penguin_gauge_s{
	int id;
	char message[256];
}penguin_gauge_t;

static penguin_gauge_t p;

static int _gauge_receive(int id);
//static int _send_receive(int id, char* buf, const unsigned int bufSize);
//static int _send_receive(const char* cmd, int length);
static void _gauge_buffered_write(int id, const char* buf, const unsigned int size);
static int _gauge_send_command(char const* cmd);
static int _gauge_receive_more(int id, int length);
static int _gauge_isMessageCompleted();

int penguin_gauge_init(){

	char devName[64];
	sprintf(devName, "/dev/ttyUSB%d", 5);

	/* open device file */
	const int fd = open(devName,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。
	if(fd<0){
		// デバイスの open() に失敗したら
		perror(devName);
		exit(1);
	}
	printf("%s opened as a penguin_gauge.\n", devName);

	/* configure the port */
	struct termios tio, oldtio;//080905 in
//    struct termios tio;//080905 out
	tcgetattr(fd, &oldtio); //080905 in; 現在の設定を退避

	memset(&tio,0,sizeof(tio));
	tio.c_cflag = CS8 | CLOCAL | CREAD;// 8 Data, no Parity, 1 stop bit
	tio.c_oflag = 0;//080905 in; rawモード
	tio.c_lflag = 0;
	tio.c_iflag = 0;

	tio.c_cc[VTIME] = 10;
	// ボーレートの設定
	cfsetispeed(&tio, B9600);
	cfsetospeed(&tio, B9600);
	// デバイスに設定を行う
	tcflush(fd, TCIFLUSH);//080905 in;モデムラインのクリア（要るか不明）
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
	printf("%s\n", "penguin_gauge_init() returns. ");
	return 0;
}

int penguin_gauge_end(){
	if(close(p.id))
		printf("some trouble when close.\n");

	return 0;
}
int penguin_gauge_get_errorStatus(){
	if(penguin_gauge_send("ERR")){
		printf("penguin_gauge_get_errorStatus(); problem to send.\n");
	}

	if(penguin_gauge_receiveAcknowledge()){
		printf("penguin_gauge_get_errorStatus(); penguin_gauge_receiveAcknowledge() failed.\n");
	}

	if(penguin_gauge_sendEnquery()){
		printf("penguin_gauge_get_errorStatus(); penguin_gauge_sendEnquery() failed.\n");
	}
	const int bytes_returned = penguin_gauge_receive();
	if (bytes_returned <= 0) {
		printf("penguin_gauge_get_errorStatus(); problem to receive.\n");
		return 99999;
	}
	int status = 0;//no error
	if(p.message[0]){//controller error
		status += 8;
		printf("penguin_gauge_get_errorStatus(); CONTROLLER ERROR! PLEASE CHECK THE FRONT PANEL!\n");
	}
	if(p.message[2]){//no hardware
		status += 4;
	}
	if(p.message[3]){//inadmissible parameter
		status += 2;
	}
	if(p.message[4]){//syntax error
		status += 1;
	}

	printf("error status = %d\n", status);
	return status;
}

double penguin_gauge_get_pressure(int channel){
	char cmd[16];

	sprintf(cmd, "PR%d", channel);
	
	if(penguin_gauge_send(cmd)){
		printf("penguin_gauge_get_pressure(); problem to send.\n");
	}

	if(penguin_gauge_receiveAcknowledge()){
		printf("penguin_gauge_get_pressure(); penguin_gauge_receiveAcknowledge() failed.\n");
	}

	if(penguin_gauge_sendEnquery()){
		printf("penguin_gauge_get_pressure(); penguin_gauge_sendEnquery() failed.\n");
	}
	const int bytes_returned = penguin_gauge_receive();
	if (bytes_returned <= 0) {
		printf("penguin_gauge_get_pressure(); problem to receive.\n");
		return 99999;
	}
	int status;
	double rValue;
	sscanf(p.message, "%d,%lf\r\n", &status, &rValue);
	//printf("status = %d, rValue = %e\n", status, rValue);
	return rValue;
/*
	double res = rValue/10.0;
	printf("res = %f\n", res);
	return res;
*/
}

int penguin_gauge_get_identification(){
	if(penguin_gauge_send("TID")){
		printf("penguin_gauge_get_identification(); problem to send.\n");
		return 1;
	}

	if(penguin_gauge_receiveAcknowledge()){
		printf("penguin_gauge_get_identification(); penguin_gauge_receiveAcknowledge() failed.\n");
		return 1;
	}

	if(penguin_gauge_sendEnquery()){
		printf("penguin_gauge_get_identification(); penguin_gauge_sendEnquery() failed.\n");
		return 1;
	}
	const int bytes_returned = penguin_gauge_receive();
	if (bytes_returned <= 0) {
		printf("penguin_gauge_get_identification(); problem to receive.\n");
		return 1;
	}
	if(bytes_returned != 11) {
		printf("penguin_gauge_get_identification(); receive bytes, expected 11 but %d.\n", bytes_returned);
		return 1;
	}
	if(p.message[0] != 'P' || p.message[0] != 'K'|| p.message[0] != 'R'){
		printf("penguin_gauge_get_identification(); expected PKP,noSen but %s.\n", p.message);
		return 1;
	}
	return 0;//normal end
}


int penguin_gauge_receiveAcknowledge(){
	const int bytes_returned = penguin_gauge_receive();
	if (bytes_returned <= 0) {
		printf("penguin_gauge_receiveAcknowledge(); problem to receive.\n");
		return -1;
	}
	if(p.message[0] != 6){
		if(p.message[0] == 0x15){
			printf("penguin_gauge_receiveAcknowledge(); received Negative.\n");
			return 1;
		}
		printf("penguin_gauge_receiveAcknowledge(); received %02X.\n", p.message[0]);
		return -2;
	}
	return 0;//normal end
}

int penguin_gauge_sendEnquery(){
	return _gauge_send_command("\x05");
}


int penguin_gauge_send(char const* cmd){
//	printf("penguin_heater_send_command(%s, %d)\n", cmd, length);
	return _gauge_send_command(cmd);
}

int _gauge_send_command(char const* cmd){
//	printf("_send_command(");
	char frame[256];


	int len = strlen(cmd);
	if(len >= 256){
		printf("_gauge_send_command(); too long command\n");
		return -1;
	}
/*
	int i = 0;
	for(i = 0; i < len; i++){
		printf("%x ", cmd[i]);
	}
*/
	//printf(")\n");

	strncpy(frame, cmd, len);
	frame[len]='\r';
	frame[len+1]='\n';
	frame[len+2]='\0';
	_gauge_buffered_write(p.id, frame, len+2);
	return 0;

}

int _gauge_isMessageCompleted(){
	return (int)strstr(p.message, "\r\n");
}


int penguin_gauge_receive(){
	memset(p.message, 0, 256);
	int res = _gauge_receive(p.id);
	if(res < 0)
		return res;
	int i = 0;
	while(!_gauge_isMessageCompleted() && i < 20){
		res = _gauge_receive_more(p.id, res);
		i++;
		//printf("penguin_gauge_receive(): %d", i);
		usleep(1000*5);
	}

	return res;
}

char* penguin_gauge_getMessage(){
	return p.message;

}



void _gauge_buffered_write(int id, const char* buf, const unsigned int size){
//void _buffered_write(int id, uint8_t* buf, const unsigned int size){
//	printf("_buffered_write(%d)\n", id);

	const char *p = buf;
	const char * const endp = p + size;
//    const unsigned char *p = buf;
//	const unsigned char * const endp = p + size;

	//printf("_gauge_buffered_write(");
/*
	int i = 0;
	for(i = 0; i < size; i++){
		printf("%x ", buf[i]);
	}
	printf(")\n");
*/
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
int _send_receive(const char* cmd){

	printf("_send_receive(%s)\n", cmd);


	int ret = _send_command(cmd);
	if(ret){
		printf("_send_receive(); send failed.\n");
		return ret;
	}
	return _receive(p.id);
}
*/
int _gauge_receive(int id){
//	printf("_receive(%d)\n", id);
	int len;                            //  受信データ数（バイト）
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
           printf("%02X ",p.message[i]);
       }
*/
	//buf[0] = 'x';
	/* 受信したデータ*/
	//p.message[len] = 0;
//	printf("[]%s:%d\n", p.message, len);
	return len;

/*
	glacier_serial_wait_for_massage_complete(id);
	char* tmp = (char*)glacier_serial_get_massage(id);
	int len = strlen(tmp);
	strncpy(buffer, tmp, len+1);
	return len;
*/
}

int _gauge_receive_more(int id, int length){
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
	p.message[length+len] = 0;
//	printf("[]%s:%d\n", p.message, len);
	return length+len;

/*
	glacier_serial_wait_for_massage_complete(id);
	char* tmp = (char*)glacier_serial_get_massage(id);
	int len = strlen(tmp);
	strncpy(buffer, tmp, len+1);
	return len;
*/
}

