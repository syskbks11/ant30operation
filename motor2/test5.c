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
#include <edrapi.h>
#include <query.h>
#include <boards.h>
#include <errors.h>


//#define DEV_NAME    "/dev/pts/1"        // デバイスファイル名
#define DEV_NAME    "/dev/ttyUSB0"        // デバイスファイル名
#define BAUD_RATE    B9600                // RS232C通信ボーレート
#define BUFF_SIZE    4096                 // 適当

#define ALST_OK	0
#define ECRS_NO	0
#define ECRS_WAIT	2
#define ECRS_YES	3

#define PULSE_360DEG	2097152

#define STRLEN 12
char tmpstr[STRLEN+1];


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

void ShowError(long err)
{
    char s[40];
    EDRE_StrError(err,s);
    printf("\n%s\n",s);
}


unsigned long init_DIO(){

	char s[40];
	int bn, di, i;
	unsigned long sn;

    di=EDRE_Query(0,APINUMDEV,0);
    printf("Devices installed = %d\n",di);
    if(di==0) exit(0);
    
    for(i=0;i<di;i++)
    {
	//get serial number of each installed device
	sn=EDRE_Query(0,BRDSERIALNO,i);
	//get text name of each device 
	EDRE_StrBoardName(sn,s);
	printf("%d) %ld - %s\n",i,sn,s);    	
    }
    
    do
    {
	bn = 0;	
	printf("board %d is used: ", bn);
	//fgets(tmpstr,STRLEN,stdin);
	//bn=atoi(tmpstr);
	//if(bn>=di) printf("Device number invalid, try again.\n");
    }while(bn>=di);
    sn=EDRE_Query(0,BRDSERIALNO,bn);

	return sn;

}

unsigned long Read(unsigned long sn)
{
    unsigned long port,value;    
    long err;    
    //printf("\n");
    //printf("Read from a Port\n");
    //printf("----------------\n");
    //printf("Port: ");
    //fgets(tmpstr,STRLEN,stdin);
    //port=atoi(tmpstr);
	port = 0;
	//printf("Read from Port %d\n", port);
    err=EDRE_DioRead(sn,port,&value);
    if(err<EDRE_OK) ShowError(err);
    else {
	//printf("Value = %ld\n",value);
	}
	return value;
}


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

int getPreviousCommand(const int fd, char* buffer){
	int len;
	len = send_receive(fd,"RRX\r", 4, buffer);
	if(len < 0){
		perror("Can't get previous command. ");
		len = 0;
	}
	buffer[len] = 0;
	return len;
}

int send_receive_int_value(const int fd, char* cmd, const unsigned int cmdSize){
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

int getAlermState(int fd){
	int resint = send_receive_int_value(fd, "ALST\r", 5);
	if(resint != ALST_OK){
		perror("!");
		printf("error: ALST=%d\n", resint);
	}
	return resint;
}

int getPulseActual(int fd){
	return send_receive_int_value(fd, "PACT\r", 5);
}

int getVelocityActual(int fd){
	return send_receive_int_value(fd, "VACT\r", 5);
}

int getVelocity(int fd){
	return send_receive_int_value(fd, "V\r", 2);
}

int getDOState(int fd){
	return send_receive_int_value(fd, "DOST\r", 5);
}

int getJogVelocity(int fd){
	return send_receive_int_value(fd, "#23\r", 4);
}

int getGain(int fd){
	return send_receive_int_value(fd, "#19\r", 4);
}


int getVelocityLoopConstant(int fd){
	return send_receive_int_value(fd, "#18\r", 4);
}

int getPositionLoopConstant(int fd){
	return send_receive_int_value(fd, "#17\r", 4);
}

int getAcceralationFilter(int fd){
	return send_receive_int_value(fd, "#39\r", 4);
}
void clearAlerm(int fd){
    buffered_write(fd, "RES\r", 4);
}


void setVelocity(int fd, int velocity){
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


void setJogVelocity(int fd, int velocity){
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

void setGain(int fd, int velocity){
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

void setPositionLoopConstant(int fd, int velocity){
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

void setVelocityLoopConstant(int fd, int velocity){
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

void setPulse(int fd, int velocity){
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

void setSERVOFF(int fd){
    buffered_write(fd, "SVOF\r", 5);
}
void stop(int fd){
    buffered_write(fd, "S\r", 2);
}
void start(int fd){
    buffered_write(fd, "G\r", 2);
}


int testMonitorState(int fd){
      int len;                            //  受信データ数（バイト）
     char buffer[BUFF_SIZE];    // データ受信バッファ
		char* rest;
		int resint;
		int res;

	
	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	resint = strtod(buffer, &rest);
	printf("[ECRS]%d\n",res);

	res = getPulseActual(fd);
	printf("[PACT]%d\n",res);

	res = getVelocityActual(fd);
	printf("[VACT]%d\n",res);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

    len=send_receive(fd, "DBONS\r", 6, buffer);
	buffer[len] = 0;
	printf("[DBONS]%s",buffer);

	return 0;

}

int testPhase2(int fd){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	int resint;
	int res;

   
    buffered_write(fd, "MASK\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[MASK]%s",buffer);

    //usleep(100*1000);

	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);

    buffered_write(fd, "DBOF\r", 5);
	len=getPreviousCommand(fd, buffer);
//	buffer[len] = 0;
//	len=send_receive(fd, "RRX\r", 4, buffer);
	printf("[DBOF]%s",buffer);

    buffered_write(fd, "SVON\r", 5);
	len=getPreviousCommand(fd, buffer);
//	len=send_receive(fd, "RRX\r", 4, buffer);
//	buffer[len] = 0;
	printf("[SVON]%s",buffer);

    usleep(4000*1000);

//    len=send_receive(fd, "SRDYS\r", 6, buffer);
//	buffer[len] = 0;
//	printf("[SRDYS]%s",buffer);

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	printf("[ECRS]%s",buffer);

	res = getPulseActual(fd);
	printf("[PACT]%d\n",res);

	res = getVelocityActual(fd);
	printf("[VACT]%d\n",res);


	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}


	setSERVOFF(fd);
//	buffered_write(fd, "SVOF\r", 5);
    
    usleep(200*1000);
    
	len=getPreviousCommand(fd, buffer);
//    len=send_receive(fd,"RRX\r", 4, buffer);
//	buffer[len] = 0;
	printf("[SVOF]%s",buffer);

    usleep(100*1000);

	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}


	resint = getDOState(fd);
	printf("[DOST]%d\n",res);
//    len=send_receive(fd, "DOST\r", 5, buffer);
//	buffer[len] = 0;
//	printf("[DOST]%s",buffer);
	
	//readSTDINtoABORT(fd);

	return 0;

}

int jogPositive(int fd, int runTime, int waitBeforeStart, int checkTimes){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	int resint;
	int res;
	
	
	printf("Motor will move toward positive after %d seconds.\n", waitBeforeStart);
	usleep(waitBeforeStart*1000*1000);

	//buffered_write(fd, "J+\r", 3);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[J+]%s",buffer);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	printf("Motor will stop after %d seconds.\n ", runTime);
	int var;
	for (var = 0; var < runTime; ++var) {
	    len=send_receive(fd, "PACT\r", 5, buffer);
		//buffer[len] = 0;
		printf("[PACT]%s",buffer);
	
	res = getVelocityActual(fd);
	printf("[VACT]%d\n",res);
		usleep(1000*1000);
	}
	//usleep(runTime*1000*1000);

    buffered_write(fd, "S\r", 2);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[S]%s",buffer);

	for (var = 0; var < checkTimes; ++var) {
	res = getPulseActual(fd);
	printf("[PACT]%d\n",res);
	
	res = getVelocityActual(fd);
	printf("[VACT]%d\n",res);
		usleep(100*1000);
	}

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);



	buffered_write(fd, "PUL\r", 4);
	len=getPreviousCommand(fd, buffer);
//	len=send_receive(fd, "RRX\r", 4, buffer);
//	buffer[len] = 0;
	printf("[PUL]%s",buffer);

	return 0;


}

int jogNegative(int fd, int runTime, int waitBeforeStart, int checkTimes){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	int resint;
	int res;
	
	
	printf("Motor will move toward negative after %d seconds.\n", waitBeforeStart);
	usleep(waitBeforeStart*1000*1000);

	//buffered_write(fd, "J-\r", 3);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[J-]%s",buffer);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	printf("Motor will stop after %d seconds.\n ", runTime);
	int var;
	for (var = 0; var < runTime; ++var) {
	    len=send_receive(fd, "PACT\r", 5, buffer);
		//buffer[len] = 0;
		printf("[PACT]%s",buffer);
	
	res = getVelocityActual(fd);
	printf("[VACT]%d\n",res);
		usleep(1000*1000);
	}
	//usleep(runTime*1000*1000);

    buffered_write(fd, "S\r", 2);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[S]%s",buffer);

	for (var = 0; var < checkTimes; ++var) {
	res = getPulseActual(fd);
	printf("[PACT]%d\n",res);
	
	res = getVelocityActual(fd);
	printf("[VACT]%d\n",res);
		usleep(100*1000);
	}

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);


	buffered_write(fd, "PUL\r", 4);
	len=getPreviousCommand(fd, buffer);
//	len=send_receive(fd, "RRX\r", 4, buffer);
//	buffer[len] = 0;
	printf("[PUL]%s",buffer);

	return 0;


}

int testJogPositive(int fd){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	int resint;
	int res;
	
   

    buffered_write(fd, "MASK\r", 5);
	len=getPreviousCommand(fd, buffer);
//	len=send_receive(fd, "RRX\r", 4, buffer);
//	buffer[len] = 0;
	printf("[MASK]%s",buffer);

    //usleep(100*1000);

	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);

    buffered_write(fd, "SVON\r", 5);
	len=getPreviousCommand(fd, buffer);
//	len=send_receive(fd, "RRX\r", 4, buffer);
//	buffer[len] = 0;
	printf("[SVON]%s",buffer);

    usleep(4000*1000);


	resint = getDOState(fd);
	printf("[DOST]%d\n",res);

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	printf("[ECRS]%s",buffer);



	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	setJogVelocity(fd, 2);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[#23=2]%s",buffer);
	resint = getJogVelocity(fd);
	printf("[#23]%d\n",resint);
	
	int waitBeforeStart = 3;
	int runTime = 2;
	int checkTimes = 50;

	jogPositive(fd, runTime, waitBeforeStart, checkTimes);

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);

	setSERVOFF(fd);
//    buffered_write(fd, "SVOF\r", 5);
    
    usleep(200*1000);
    
	len=getPreviousCommand(fd, buffer);
//    len=send_receive(fd,"RRX\r", 4, buffer);
//	buffer[len] = 0;
	printf("[SVOF]%s",buffer);

    usleep(100*1000);

	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}


	resint = getDOState(fd);
	printf("[DOST]%d\n",res);

	return 0;


}

int testJogNegative(int fd){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	int resint;
	int res;
	
   

    buffered_write(fd, "MASK\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[MASK]%s",buffer);

    //usleep(100*1000);

	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);

    buffered_write(fd, "SVON\r", 5);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[SVON]%s",buffer);

    usleep(4000*1000);


	resint = getDOState(fd);
	printf("[DOST]%d\n",res);

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	printf("[ECRS]%s",buffer);



	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);
	
	int waitBeforeStart = 3;
	printf("Motor will move toward negative after %d seconds.\n", waitBeforeStart);
	usleep(waitBeforeStart*1000*1000);

	buffered_write(fd, "J-\r", 3);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[J-]%s",buffer);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	int runTime = 10;
	printf("Motor will stop after %d seconds.\n ", runTime);
	int var;
	for (var = 0; var < runTime; ++var) {
		res = getPulseActual(fd);
		printf("[PACT]%d\n",res);
	
		res = getVelocityActual(fd);
		printf("[VACT]%d\n",res);
		usleep(1000*1000);
	}
	//usleep(runTime*1000*1000);
	
	stop(fd);
//    buffered_write(fd, "S\r", 2);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[S]%s",buffer);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	for (var = 0; var < 50; ++var) {
		res = getPulseActual(fd);
		printf("[PACT]%d\n",res);
	
		res = getVelocityActual(fd);
		printf("[VACT]%d\n",res);

		usleep(100*1000);
	}

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);



	buffered_write(fd, "PUL\r", 4);
	len=getPreviousCommand(fd, buffer);
	//len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[PUL]%s",buffer);

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);

	setSERVOFF(fd);
//    buffered_write(fd, "SVOF\r", 5);
    
    usleep(200*1000);
    
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[SVOF]%s",buffer);

    usleep(100*1000);

	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}


	resint = getDOState(fd);
	printf("[DOST]%d\n",res);

	return 0;
}

int prepareInclimental(int fd){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	int resint;
	int res;
    buffered_write(fd, "MASK\r", 5);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[MASK]%s",buffer);

    //usleep(100*1000);

	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);
//    len=send_receive(fd, "DOST\r", 5, buffer);
//	buffer[len] = 0;
//	printf("[DOST]%s",buffer);

    buffered_write(fd, "SVON\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[SVON]%s",buffer);

    usleep(4000*1000);


	resint = getDOState(fd);
	printf("[DOST]%d\n",res);
//    len=send_receive(fd, "DOST\r", 5, buffer);
//	buffer[len] = 0;
//	printf("[DOST]%s",buffer);

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	printf("[ECRS]%s",buffer);



	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	buffered_write(fd, "INC\r", 4);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[INC]%s",buffer);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);
//    len=send_receive(fd, "DOST\r", 5, buffer);
//	buffer[len] = 0;
//	printf("[DOST]%s",buffer);


	usleep(3000*1000);

    len=send_receive(fd,"#22\r", 4, buffer);
	buffer[len] = 0;
	printf("[#22=]%s",buffer);

	setVelocity(fd, 1);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[V=2]%s",buffer);

    len=send_receive(fd,"#22\r", 4, buffer);
	buffer[len] = 0;
	printf("[#22=]%s",buffer);
	
	resint = getVelocity(fd);
	printf("[V=]%d\r",resint);

	res = getPulseActual(fd);
	printf("[PACT]%d\n",res);

	res = getVelocityActual(fd);
	printf("[VACT]%d\n",res);

	return 0;

}

int prepareAbsolute(int fd){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	int resint;
	int res;
    buffered_write(fd, "MASK\r", 5);
	len=getPreviousCommand(fd, buffer);
	//buffer[len] = 0;
	printf("[MASK]%s",buffer);

    //usleep(100*1000);

	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);
//    len=send_receive(fd, "DOST\r", 5, buffer);
//	buffer[len] = 0;
//	printf("[DOST]%s",buffer);

    buffered_write(fd, "SVON\r", 5);
	len=getPreviousCommand(fd, buffer);
//	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[SVON]%s",buffer);

    usleep(4000*1000);


	resint = getDOState(fd);
	printf("[DOST]%d\n",res);
//    len=send_receive(fd, "DOST\r", 5, buffer);
//	buffer[len] = 0;
//	printf("[DOST]%s",buffer);

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	printf("[ECRS]%s",buffer);



	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	buffered_write(fd, "ABS\r", 4);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[ABS]%s",buffer);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	resint = getDOState(fd);
	printf("[DOST]%d\n",res);
//    len=send_receive(fd, "DOST\r", 5, buffer);
//	buffer[len] = 0;
//	printf("[DOST]%s",buffer);


	usleep(3000*1000);

    len=send_receive(fd,"#22\r", 4, buffer);
	buffer[len] = 0;
	printf("[#22=]%s",buffer);

	setVelocity(fd, 1);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[V=1]%s",buffer);

    len=send_receive(fd,"#22\r", 4, buffer);
	buffer[len] = 0;
	printf("[#22=]%s",buffer);
	
	resint = getVelocity(fd);
	printf("[V=]%d\r",resint);

	res = getPulseActual(fd);
	printf("[PACT]%d\n",res);

	res = getVelocityActual(fd);
	printf("[VACT]%d\n",res);

	return 0;

}

int endProcedure(int fd){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	int resint;
	int res;

    buffered_write(fd, "S\r", 2);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[S]%s",buffer);


	buffered_write(fd, "PUL\r", 4);
	len=getPreviousCommand(fd, buffer);
	buffer[len] = 0;
	printf("[PUL]%s",buffer);

	resint = getDOState(fd);
	printf("[DOST]%d\n",resint);
//    len=send_receive(fd, "DOST\r", 5, buffer);
//	buffer[len] = 0;
//	printf("[DOST]%s",buffer);

	setSERVOFF(fd);
	//buffered_write(fd, "SVOF\r", 5);
    
    usleep(200*1000);
    
	len=getPreviousCommand(fd, buffer);
//	buffer[len] = 0;
	printf("[SVOF]%s",buffer);

    usleep(100*1000);

	res = getAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}


	resint = getDOState(fd);
	printf("[DOST]%d\n",res);
//    len=send_receive(fd, "DOST\r", 5, buffer);
//	buffer[len] = 0;
//	printf("[DOST]%s",buffer);

	return 0;

}

int testInclimentalMode(int fd){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	//int resint;
	int res;

	prepareInclimental(fd);
	    
	setPulse(fd, 10*PULSE_360DEG/360);	
	//buffered_write(fd, "P=100000\r", 8);
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[P=10000]%s",buffer);
	
	res = getVelocityActual(fd);
	printf("[VACT]%d\n",res);

	//double v=(buffer);
	while(buffer[0] != '0'){
//		while(v != 0){
		res = getPulseActual(fd);
		printf("[PACT]%d\n",res);
	
		res = getVelocityActual(fd);
		printf("[VACT]%d\n",res);
		
		//v=atod(buffer);
	}
	int var;
	for (var = 0; var < 140; ++var) {
		res = getPulseActual(fd);
		printf("[PACT]%d\n",res);
	
		res = getVelocityActual(fd);
		printf("[VACT]%d\n",res);
		usleep(100*1000);
	}

	 endProcedure(fd);
	return 0;
}

int test_by_manual(int fd, unsigned long sn){
      int len;                            //  受信データ数（バイト）
     char buffer[BUFF_SIZE];    // データ受信バッファ
		//char* rest;
		//int resint;
		int res, pact, vact;
	unsigned long state_switches;
	//char ch;
	//int key;
	printf("%s\n", "Test start. ");
	int n = 0;
	
	int cont = 1;    
	do{
	n++;
	printf("[test: %d] > ", n);
	fflush(stdout);
	//printf("Esc");	
	//init_keyboard();
	
	fgets(tmpstr,STRLEN,stdin);
	//key = fgetc(stdin)
	//read(0,&ch,1);
	//close_keyboard();
	pact = getPulseActual(fd);
	//printf("[PACT]%d\n",res);
	vact = getVelocityActual(fd);
	//printf("[VACT]%d\n",res);
	state_switches = Read(sn);
	printf("PACT, VACT, SW = %d %d %d\n",pact, vact, state_switches);
	printf("cmd:%s", tmpstr);
	
	long pos;
	char *token;
	switch(tmpstr[0]){
	case 'q':
	case 'Q':
		setSERVOFF(fd);
		len=getPreviousCommand(fd, buffer);
		buffer[len] = 0;
		printf("[SVOF]%s",buffer);
		break;
	case 's':
	case 'S':
		stop(fd);
		len=getPreviousCommand(fd, buffer);
		buffer[len] = 0;
		printf("[S]%s",buffer);
		break;
	case 'g':
	case 'G':
		start(fd);
		len=getPreviousCommand(fd, buffer);
		buffer[len] = 0;
		printf("[G]%s",buffer);
		break;
	case '1':
		testMonitorState(fd);
		break;
	case '2':
		testPhase2(fd);
		break;
	case 'x':
		token=strtok(tmpstr," ");
		if(token == NULL)break;
		printf("token%s", token);
		token=strtok(NULL," ");
		if(token == NULL)break;
		pos = atoi(token);
		setPulse(fd, pos);
		len=getPreviousCommand(fd, buffer);
		//buffer[len] = 0;
		printf("[P=%ld]%s",pos, buffer);

		break;
	case 'X':
		switch(tmpstr[1]){
		case '-':
			pos = -90*PULSE_360DEG/360*4;
			break;
		case '+':
		default:
			pos = 90*PULSE_360DEG/360*4;
			break;
		}
		setPulse(fd, pos);
		len=getPreviousCommand(fd, buffer);
		buffer[len] = 0;
		printf("[P=%ld]%s",pos, buffer);

		break;
	case 'v':
		setVelocity(fd, 1);
		break;
	case 'l':
		setVelocityLoopConstant(fd, 100);
		len=getPreviousCommand(fd, buffer);
		buffer[len] = 0;
		printf("[#18=]%s",buffer);
		res = getVelocityLoopConstant(fd);
		printf("[#18]%d\n",res);
		break;
	case 'j':
		setGain(fd, 1);
		len=getPreviousCommand(fd, buffer);
		buffer[len] = 0;
		printf("[P=%d]%s",pos, buffer);
		break;
	case 'a':
		prepareAbsolute(fd);
		break;
	case 'i':
		prepareInclimental(fd);
		break;
	case 'c':
		clearAlerm(fd);
		break;
	case 'e':
		cont = 0;
		endProcedure(fd);
		break;
	case '#':
		res = getPositionLoopConstant(fd);
		printf("[#17]%d\n",res);
		res = getVelocityLoopConstant(fd);
		printf("[#18]%d\n",res);
		res = getGain(fd);
		printf("[#19]%d\n",res);
		res = getAcceralationFilter(fd);
		printf("[#39]%d\n",res);
		break;

	default:break;	
	}
		
    }while(cont);



	printf("%s\n", "Test end. ");

	return 0;
	
}

/* --------------------------------------------------------------------- */
/* メイン                                                                */
/* --------------------------------------------------------------------- */

int main(int argc,char *argv[])
{
    int fd;
	int resint;
    //unsigned char *cmd;

    printf("%s\n", "main()");
    // デバイスファイル（シリアルポート）オープン
    fd = open(DEV_NAME,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。
//    fd = open(DEV_NAME,O_RDWR);//080905 out; 読み書き可能で開く
    if(fd<0){
        // デバイスの open() に失敗したら
        perror(argv[1]);
         exit(1);
    }

    // シリアルポートの初期化
    serial_init(fd);

	unsigned long sn = init_DIO();
 
      int len = 0;                            //  受信データ数（バイト）
     char buffer[BUFF_SIZE];    // データ受信バッファ
    
	test_by_manual(fd, sn);
	
	resint = getDOState(fd);
	printf("[DOST]%d\n",resint);
//    len=send_receive(fd, "DOST\r", 5, buffer);
//	buffer[len] = 0;
//	printf("[DOST]%s",buffer);
	
	while(len > 1){
		len=read(fd,buffer,BUFF_SIZE);
	/* 受信したデータ*/
	buffer[len] = 0;
	printf("while:%s:%d\n",buffer, len);
	}
	exit(0);
}

