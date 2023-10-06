/*
 * シリアルポート受信サンプルプログラム
 * Version 1.0  2006.10.19  Y.Ebihara (SiliconLinux)
 *
 * このプログラムはシリアルポートをopenして、データを16進数表示する
 * サンプルプログラムです。
 *   i386パソコン、およびCAT760で動作検証をしています。
 *
 * test-machine: TSR-V2, i386PC, CAT760
 * 
 * 
 * URL: http://www.si-linux.co.jp/wiki/cat/index.php?%A5%B7%A5%EA%A5%A2%A5%EB%A5%DD%A1%BC%A5%C8%A5%D7%A5%ED%A5%B0%A5%E9%A5%DF%A5%F3%A5%B0
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
#include <term.h>

#include "commands.h"

#define BUFF_SIZE    4096                 // 適当


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
void serial_init(int fd)
{
    printf("%s\n", "serial_init()");
    struct termios tio, oldtio;//080905 in
//    struct termios tio;//080905 out
	tcgetattr(fd, &oldtio); //080905 in; 現在の設定を退避
	
    memset(&tio,0,sizeof(tio));
    tio.c_cflag = CS8 | CLOCAL | CREAD;
    tio.c_oflag = 0;//080905 in; rawモード
    tio.c_lflag = ICANON;//080905 in;カノニカル入力
    tio.c_iflag = ICRNL;//080905 in; CRをNLに変換する。
    
    tio.c_cc[VTIME] = 100;
    // ボーレートの設定
    cfsetispeed(&tio,BAUD_RATE);
    cfsetospeed(&tio,BAUD_RATE);
    // デバイスに設定を行う
    tcflush(fd, TCIFLUSH);//080905 in;モデムラインのクリア（要るか不明）
    tcsetattr(fd,TCSANOW,&tio);
    printf("%s\n", "serial_init() returns. ");
}

void buffered_write(int fd, char* buf, const unsigned int size)
//void buffered_write(int fd, unsigned char* buf, const unsigned int size)
{
    const char *p = buf;
	const char * const endp = p + size;
//    const unsigned char *p = buf;
//	const unsigned char * const endp = p + size;

	while (p < endp) {
		int num_bytes = write(fd, p, endp - p);
		if (num_bytes < 0) {
 			perror("write failed");
    		break;
		}
		p += num_bytes;
  	}

	
}

int send_receive(int fd, char* buf, const unsigned int bufSize, char* buffer)
//int send_receive(int fd, unsigned char* buf, const unsigned int bufSize, unsigned char* buffer)
{
	buffered_write(fd, buf, bufSize);
     int len;                            //  受信データ数（バイト）
     //unsigned char buffer[BUFF_SIZE];    // データ受信バッファ
    //int i;
    // ここで受信待ち
    len=read(fd,buffer,BUFF_SIZE);
    //printf("%s\n", "data recieved. ");
    if(len < 0){
    	perror("send_receive(): IO error!");
    	return len;
    }
    // 受信したデータを 16進数形式で表示    
//        for(i=0; i<len; i++){
//           printf("%02X ",buffer[i]);
//       }
	//buf[0] = 'x';
	/* 受信したデータ*/
	buffer[len] = 0;
//	printf("[%s]%s:%d\n",buf, buffer, len);
	return len;
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
		perror("!");
		printf("error: ALST=%d\n", resint);
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

