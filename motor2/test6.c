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

#include "commands.h"
#include "dio.h"

#define BUFF_SIZE    4096                 // 適当

//#define DEV_NAME    "/dev/pts/1"        // デバイスファイル名
//#define DEV_NAME    "/dev/ttyUSB0"        // デバイスファイル名
#define DEV_NAME    "/dev/ttyUSB1"        // デバイスファイル名


#define DIO_CH1 0x1
#define DIO_CH2 0x2
#define DIO_CH3 0x4
#define DIO_CH4 0x8
#define DIO_CH5 0x10

#define AZ_CW DIO_CH1
#define AZ_CCW DIO_CH2
#define EL_LOW DIO_CH3
#define EL_HIGH DIO_CH5

#define LABEL_ON "on"
#define LABEL_OFF "off"

#define STRLEN 12
char tmpstr[STRLEN+1];

int limitState(int fd, unsigned long sn)
{
	static int state;
	unsigned long state_switches;

	state_switches = Read(sn);
	printf("%ld\t", state_switches);
	//printf("%ld\t", (state_switches & DIO_CH1 ));
	printf("%s\t", (state_switches & AZ_CW )?LABEL_ON:LABEL_OFF);
	printf("%s\t", (state_switches & AZ_CCW )?LABEL_ON:LABEL_OFF);
	printf("%s\t", (state_switches & EL_LOW )?LABEL_ON:LABEL_OFF);
	printf("%s\n", (state_switches & EL_HIGH )?LABEL_ON:LABEL_OFF);
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

int moveToPositiveLimit(int fd, unsigned long sn){
	int var, res;
	unsigned long state_switches = Read(sn);

	state_switches = Read(sn);

	while(!(state_switches & EL_HIGH )){
//	while(!(state_switches & AZ_CCW )){
		res = getPulseActual(fd);
		printf("[PACT]%d\n",res);
		
		setPulse(fd, 300);
		usleep(2*1000);

		res = getPulseActual(fd);
		printf("[PACT]%d\n",res);

		limitState(fd, sn);

		state_switches = Read(sn);
		
	}
	return 0;
}
int moveToNegativeLimit(int fd, unsigned long sn){
	int var, res;
	unsigned long state_switches = Read(sn);

	state_switches = Read(sn);

	while(!(state_switches & EL_LOW )){
//	while(!(state_switches & AZ_CW )){
		res = getPulseActual(fd);
		printf("[PACT]%d\n",res);
		
		setPulse(fd, -300);
		usleep(2*1000);

		res = getPulseActual(fd);
		printf("[PACT]%d\n",res);

		limitState(fd, sn);

		state_switches = Read(sn);
		
	}
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
	limitState(fd, sn);
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
	case '+':
		moveToPositiveLimit(fd, sn);
		break;
	case '-':
		moveToNegativeLimit(fd, sn);
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
			pos = -10*PULSE_360DEG/360;
			break;
		case '+':
		default:
			pos = 10*PULSE_360DEG/360;
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
/*	case 'l':
		setVelocityLoopConstant(fd, 100);
		len=getPreviousCommand(fd, buffer);
		buffer[len] = 0;
		printf("[#18=]%s",buffer);
		res = getVelocityLoopConstant(fd);
		printf("[#18]%d\n",res);
		break;*/
/*	case 'j':
		setGain(fd, 1);
		len=getPreviousCommand(fd, buffer);
		buffer[len] = 0;
		printf("[P=%d]%s",pos, buffer);
		break;*/
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

