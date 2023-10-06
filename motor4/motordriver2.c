/*!
¥file motordriver.c
¥author NAGAI Makoto
¥date 2008.12.08
¥brief antenna motor driver control for 30-cm telescope
* 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

#include "motordriver.h"

#define BUFF_SIZE    4096                 // 適当


struct motordriver_s{
	int id;
	int positiveSoftLimit;
	int negativeSoftLimit;
};


#define STRLEN 12
char tmpstr[STRLEN+1];


void setGain(int fd, int velocity);
void setPositionLoopConstant(int fd, int velocity);
void setVelocityLoopConstant(int fd, int velocity);



//static struct termios old_set,new_set;

//void init_keyboard(){
//    tcgetattr(0,&old_set);
//    new_set=old_set;
//    new_set.c_lflag &=~ICANON;
//    new_set.c_lflag &=~ECHO;
//    new_set.c_lflag &=~ISIG;
//    new_set.c_cc[VMIN] = 1;
//    new_set.c_cc[VTIME] = 0;
//    tcsetattr(0,TCSANOW,&new_set);
//}

//void close_keyboard(){
//    tcsetattr(0,TCSANOW,&old_set);
//}


// シリアルポートの初期化
int serial_init(char* devName, int id){
	cssl_t *serial;

	int id=0;

	cssl_start();

	serial=cssl_open(devName,glacier_serial_callback_CR,id,9600,8,0,1);
	if (!serial) {
		printf("%s\n",cssl_geterrormsg());
		return -1;
	}

	printf("%s\n", "serial_init() returns. ");

	return 0;//normal end
}

// シリアルポートの終了
int serial_end(int id){
	glacier_serial_dettach(id);
	cssl_close(serial);
	cssl_stop();
	return 0;//normal end
}

void buffered_write(int id, char* buf, const unsigned int size){
	glacier_serial_putdata(id, buf, size);
}

int send_receive(int id, char* buf, const unsigned int bufSize, char* buffer){
	buffered_write(id, buf, bufSize);
	glacier_serial_wait_for_massage_complete(id);
	char* tmp = glacier_serial_get_massage(id);
	int len = strlen(tmp);
	strncpy(buffer, tmp, len+1);
}

int getPreviousCommand(const int fd, char* buffer)
{
	int len;
	len = send_receive(fd,"RRX\r", 4, buffer);
	if(len < 0){
		perror("Can't get previous command. ");
		len = 0;
	}
	buffer[len] = 0;
	return len;
}

int send_receive_int_value(const int fd, char* cmd, const unsigned int cmdSize)
{
	char* rest;
	char buffer[BUFF_SIZE];    // データ受信バッファ
	int len=send_receive(fd, cmd, cmdSize, buffer);
	if(len == 0){
		perror("No data received. ");
		return -1;		
		//exit(1);
	}
	//buffer[len] = 0;
	//double res = atof(buffer);
	int resint = strtod(buffer, &rest);
	return resint;
}

int getAlermState(int fd)
{
	int resint = send_receive_int_value(fd, "ALST\r", 5);
	if(resint != ALST_OK){
		//perror("!");
		//printf("error: ALST=%d\n", resint);
	}
	return resint;
}

int getPulseActual(int fd)
{
	return send_receive_int_value(fd, "PACT\r", 5);
}

int getVelocityActual(int fd)
{
	return send_receive_int_value(fd, "VACT\r", 5);
}

int getVelocity(int fd)
{
	return send_receive_int_value(fd, "V\r", 2);
}

int getDOState(int fd)
{
	return send_receive_int_value(fd, "DOST\r", 5);
}

int getJogVelocity(int fd)
{
	return send_receive_int_value(fd, "#23\r", 4);
}

int getGain(int fd)
{
	return send_receive_int_value(fd, "#19\r", 4);
}


int getVelocityLoopConstant(int fd)
{
	return send_receive_int_value(fd, "#18\r", 4);
}

int getPositionLoopConstant(int fd)
{
	return send_receive_int_value(fd, "#17\r", 4);
}

int getAcceralationFilter(int fd)
{
	return send_receive_int_value(fd, "#39\r", 4);
}
void clearAlerm(int fd)
{
    buffered_write(fd, "RES\r", 4);
}


void setVelocity(int fd, int velocity)
{
	char tmp[BUFF_SIZE];
	//char buffer[BUFF_SIZE];    // データ受信バッファ
	sprintf(tmp, "V=%d\r", velocity);
	int pos = 2;
	int i;	
	for(i=2; i < BUFF_SIZE; i++){
		if(tmp[i] == '\0'){
			pos = i;
			break;
		}
	}
	buffered_write(fd, tmp, pos);

	//return resint;
	
}


void setJogVelocity(int fd, int velocity)
{
	char tmp[BUFF_SIZE];
	//char buffer[BUFF_SIZE];    // データ受信バッファ
	sprintf(tmp, "#23=%d\r", velocity);
	int pos = 3;
	int i;	
	for(i=3; i < BUFF_SIZE; i++){
		if(tmp[i] == '\0'){
			pos = i;
			break;
		}
	}
	buffered_write(fd, tmp, pos);

	//return resint;
	
}

void setGain(int fd, int velocity)
{
	char tmp[BUFF_SIZE];
	//char buffer[BUFF_SIZE];    // データ受信バッファ
	sprintf(tmp, "#19=%d\r", velocity);
	int pos = 3;
	int i;	
	for(i=3; i < BUFF_SIZE; i++){
		if(tmp[i] == '\0'){
			pos = i;
			break;
		}
	}
	buffered_write(fd, tmp, pos);

	//return resint;
	
}

void setPositionLoopConstant(int fd, int velocity)
{
	char tmp[BUFF_SIZE];
	//char buffer[BUFF_SIZE];    // データ受信バッファ
	sprintf(tmp, "#17=%d\r", velocity);
	int pos = 3;
	int i;	
	for(i=3; i < BUFF_SIZE; i++){
		if(tmp[i] == '\0'){
			pos = i;
			break;
		}
	}
	buffered_write(fd, tmp, pos);

	//return resint;
	
}

void setVelocityLoopConstant(int fd, int velocity)
{
	char tmp[BUFF_SIZE];
	//char buffer[BUFF_SIZE];    // データ受信バッファ
	sprintf(tmp, "#18=%d\r", velocity);
	int pos = 3;
	int i;	
	for(i=3; i < BUFF_SIZE; i++){
		if(tmp[i] == '\0'){
			pos = i;
			break;
		}
	}
	buffered_write(fd, tmp, pos);

	//return resint;
	
}

void setPulse(int fd, int velocity)
{
//void setPulse(int fd, int pulse){
	char tmp[BUFF_SIZE];
	//char buffer[BUFF_SIZE];    // データ受信バッファ
	sprintf(tmp, "P=%d\r", velocity);
	int pos = 2;
	int i;	
	for(i=2; i < BUFF_SIZE; i++){
		if(tmp[i] == '\0'){
			pos = i;
			break;
		}
	}
	buffered_write(fd, tmp, pos);

	//return resint;
	
}

void setSERVOFF(int fd)
{
    buffered_write(fd, "SVOF\r", 5);
}
void stop(int fd)
{
    buffered_write(fd, "S\r", 2);
}
void start(int fd)
{
    buffered_write(fd, "G\r", 2);
}

void goOriginPositive(int fd){
	buffered_write(fd, "HZ+\r", 4);
}

void goOriginNegative(int fd){
	buffered_write(fd, "HZ-\r", 4);
}
