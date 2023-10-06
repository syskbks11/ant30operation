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

#define DEV_NAME    "/dev/ttyUSB0"        // デバイスファイル名
#define BAUD_RATE    B9600                // RS232C通信ボーレート
#define BUFF_SIZE    4096                 // 適当

#define ALST_OK	0
#define ECRS_NO	0
#define ECRS_WAIT	2
#define ECRS_YES	3

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

int monitorAlermState(int fd){
	char* rest;
	char buffer[BUFF_SIZE];    // データ受信バッファ
	int len=send_receive(fd, "ALST\r", 5, buffer);
	if(len == 0){
		perror("No data received. ");
		exit(1);
	}
	//buffer[len] = 0;
	//double res = atof(buffer);
	//printf("[ALST]%f",res);
	int resint = strtod(buffer, &rest);
	if(resint != ALST_OK){
		printf("error: ALST=%d\n", resint);
		return resint;
	}
	printf("[ALST]%d %s",resint, rest);
	//printf("[ALST]%s",buffer);
	return resint;
}

int testMonitorState(int fd){
      int len;                            //  受信データ数（バイト）
     char buffer[BUFF_SIZE];    // データ受信バッファ
		char* rest;
		int resint;
		int res;

	
	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	resint = strtod(buffer, &rest);
	printf("[ECRS]%s",buffer);

    len=send_receive(fd, "PACT\r", 5, buffer);
	buffer[len] = 0;
	printf("[PACT]%s",buffer);

    len=send_receive(fd, "VACT\r", 5, buffer);
	buffer[len] = 0;
	printf("[VACT]%s",buffer);

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

	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

    buffered_write(fd, "DBOF\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	//buffer[len] = 0;
	resint = atoi(buffer);
	printf("[DBOF]%s, %d",buffer, resint);

    buffered_write(fd, "SVON\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[SVON]%s",buffer);

    usleep(4000*1000);

//    len=send_receive(fd, "SRDYS\r", 6, buffer);
//	buffer[len] = 0;
//	printf("[SRDYS]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	printf("[ECRS]%s",buffer);

    len=send_receive(fd, "PACT\r", 5, buffer);
	buffer[len] = 0;
	printf("[PACT]%s",buffer);

    len=send_receive(fd, "VACT\r", 5, buffer);
	buffer[len] = 0;
	printf("[VACT]%s",buffer);


	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}


    buffered_write(fd, "SVOF\r", 5);
    
    usleep(200*1000);
    
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[SVOF]%s",buffer);

    usleep(100*1000);

	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

//    len=send_receive(fd, "SRDYS\r", 6, buffer);
//	buffer[len] = 0;
//	printf("[SRDYS]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

	return 0;

}

int testJogPositive(int fd){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	//int resint;
	int res;
	
   

    buffered_write(fd, "MASK\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[MASK]%s",buffer);

    //usleep(100*1000);

	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

    buffered_write(fd, "SVON\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[SVON]%s",buffer);

    usleep(4000*1000);

//    len=send_receive(fd, "SRDYS\r", 6, buffer);
//	buffer[len] = 0;
//	printf("[SRDYS]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	printf("[ECRS]%s",buffer);



	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);
	
	int waitBeforeStart = 3;
	printf("Motor will move toward positive after %d seconds.\n", waitBeforeStart);
	usleep(waitBeforeStart*1000*1000);

	buffered_write(fd, "J+\r", 3);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[J+]%s",buffer);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	int runTime = 10;
	printf("Motor will stop after %d seconds.\n ", runTime);
	int var;
	for (var = 0; var < runTime; ++var) {
	    len=send_receive(fd, "PACT\r", 5, buffer);
		//buffer[len] = 0;
		printf("[PACT]%s",buffer);
	
	    len=send_receive(fd, "VACT\r", 5, buffer);
		//buffer[len] = 0;
		printf("[VACT]%s",buffer);
		usleep(1000*1000);
	}
	//usleep(runTime*1000*1000);

    buffered_write(fd, "S\r", 2);
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[S]%s",buffer);

	for (var = 0; var < 50; ++var) {
	    len=send_receive(fd, "PACT\r", 5, buffer);
		//buffer[len] = 0;
		printf("[PACT]%s",buffer);
	
	    len=send_receive(fd, "VACT\r", 5, buffer);
		//buffer[len] = 0;
		printf("[VACT]%s",buffer);
		usleep(100*1000);
	}

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

//	    len=send_receive(fd,"#12\r", 4, buffer);
//		buffer[len] = 0;
//		printf("[#12=]%s",buffer);



	buffered_write(fd, "PUL\r", 4);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[PUL]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

//	    len=send_receive(fd,"#12\r", 4, buffer);
//		buffer[len] = 0;
//		printf("[#12=]%s",buffer);

    buffered_write(fd, "SVOF\r", 5);
    
    usleep(200*1000);
    
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[SVOF]%s",buffer);

    usleep(100*1000);

	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

//    len=send_receive(fd, "SRDYS\r", 6, buffer);
//	buffer[len] = 0;
//	printf("[SRDYS]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

	return 0;


}

int testJogNegative(int fd){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	//int resint;
	int res;
	
   

    buffered_write(fd, "MASK\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[MASK]%s",buffer);

    //usleep(100*1000);

	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

    buffered_write(fd, "SVON\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[SVON]%s",buffer);

    usleep(4000*1000);

//    len=send_receive(fd, "SRDYS\r", 6, buffer);
//	buffer[len] = 0;
//	printf("[SRDYS]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	printf("[ECRS]%s",buffer);



	res = monitorAlermState(fd);
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
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[J-]%s",buffer);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	int runTime = 10;
	printf("Motor will stop after %d seconds.\n ", runTime);
	int var;
	for (var = 0; var < runTime; ++var) {
	    len=send_receive(fd, "PACT\r", 5, buffer);
		//buffer[len] = 0;
		printf("[PACT]%s",buffer);
	
	    len=send_receive(fd, "VACT\r", 5, buffer);
		//buffer[len] = 0;
		printf("[VACT]%s",buffer);
		usleep(1000*1000);
	}
	//usleep(runTime*1000*1000);

    buffered_write(fd, "S\r", 2);
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[S]%s",buffer);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	for (var = 0; var < 50; ++var) {
	    len=send_receive(fd, "PACT\r", 5, buffer);
		//buffer[len] = 0;
		printf("[PACT]%s",buffer);
	
	    len=send_receive(fd, "VACT\r", 5, buffer);
		//buffer[len] = 0;
		printf("[VACT]%s",buffer);
		usleep(100*1000);
	}

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

//	    len=send_receive(fd,"#12\r", 4, buffer);
//		buffer[len] = 0;
//		printf("[#12=]%s",buffer);



	buffered_write(fd, "PUL\r", 4);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[PUL]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

//	    len=send_receive(fd,"#12\r", 4, buffer);
//		buffer[len] = 0;
//		printf("[#12=]%s",buffer);

    buffered_write(fd, "SVOF\r", 5);
    
    usleep(200*1000);
    
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[SVOF]%s",buffer);

    usleep(100*1000);

	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

//    len=send_receive(fd, "SRDYS\r", 6, buffer);
//	buffer[len] = 0;
//	printf("[SRDYS]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

	return 0;


}

int testInclimentalMode(int fd){
	int len;                            //  受信データ数（バイト）
	char buffer[BUFF_SIZE];    // データ受信バッファ
	//char* rest;
	//int resint;
	int res;

    buffered_write(fd, "MASK\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[MASK]%s",buffer);

    //usleep(100*1000);

	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

    buffered_write(fd, "SVON\r", 5);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[SVON]%s",buffer);

    usleep(4000*1000);

//    len=send_receive(fd, "SRDYS\r", 6, buffer);
//	buffer[len] = 0;
//	printf("[SRDYS]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

    len=send_receive(fd, "ECRS\r", 5, buffer);
	buffer[len] = 0;
	printf("[ECRS]%s",buffer);



	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

	buffered_write(fd, "INC\r", 4);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[INC]%s",buffer);

    len=send_receive(fd, "M\r", 2, buffer);
	buffer[len] = 0;
	printf("[M]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

//	    len=send_receive(fd,"#12\r", 4, buffer);
//		buffer[len] = 0;
//		printf("[#12=]%s",buffer);

	usleep(3000*1000);

    len=send_receive(fd,"#22\r", 4, buffer);
	buffer[len] = 0;
	printf("[#22=]%s",buffer);

    buffered_write(fd, "V=5\r", 4);
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[V=5]%s",buffer);

    len=send_receive(fd,"#22\r", 4, buffer);
	buffer[len] = 0;
	printf("[#22=]%s",buffer);

    len=send_receive(fd, "PACT\r", 5, buffer);
	buffer[len] = 0;
	printf("[PACT]%s",buffer);

    len=send_receive(fd, "VACT\r", 5, buffer);
	buffer[len] = 0;
	printf("[VACT]%s",buffer);

    buffered_write(fd, "P=10000\r", 8);
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[P=10000]%s",buffer);
	
    len=send_receive(fd, "VACT\r", 5, buffer);
	buffer[len] = 0;
	printf("[VACT]%s",buffer);
	//double v=(buffer);
	while(buffer[0] != '0'){
//		while(v != 0){
	    len=send_receive(fd, "PACT\r", 5, buffer);
		buffer[len] = 0;
		printf("[PACT]%s",buffer);
	
	    len=send_receive(fd, "VACT\r", 5, buffer);
		buffer[len] = 0;
		printf("[VACT]%s",buffer);
		
		//v=atod(buffer);
	}
	int var;
	for (var = 0; var < 140; ++var) {
	    len=send_receive(fd, "PACT\r", 5, buffer);
		buffer[len] = 0;
		printf("[PACT]%s",buffer);
	
	    len=send_receive(fd, "VACT\r", 5, buffer);
		buffer[len] = 0;
		printf("[VACT]%s",buffer);
		usleep(100*1000);
	}

    buffered_write(fd, "S\r", 2);
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[S]%s",buffer);


	buffered_write(fd, "PUL\r", 4);
	len=send_receive(fd, "RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[PUL]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

//	    len=send_receive(fd,"#12\r", 4, buffer);
//		buffer[len] = 0;
//		printf("[#12=]%s",buffer);

    buffered_write(fd, "SVOF\r", 5);
    
    usleep(200*1000);
    
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[SVOF]%s",buffer);

    usleep(100*1000);

	res = monitorAlermState(fd);
	if(res != 0){
		printf("%s\n", "Test failed with error. ");
		return res;
	}

//    len=send_receive(fd, "SRDYS\r", 6, buffer);
//	buffer[len] = 0;
//	printf("[SRDYS]%s",buffer);

    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);

	return 0;
}


int test_by_sequence(int fd, int testPhase){
      int len;                            //  受信データ数（バイト）
     char buffer[BUFF_SIZE];    // データ受信バッファ
		//char* rest;
		//int resint;
		int res;

	printf("%s\n", "Test start. ");
	if(testPhase > 0){
		printf("%s\n", "[Phase 1]");
		testMonitorState(fd);
		printf("%s\n", "[Phase 1] CLEAR!");		
	}
	if(testPhase > 1){
		printf("%s\n", "[Phase 2]");
		testPhase2(fd);
		printf("%s\n", "[Phase 2] CLEAR!");		
	}
	if(testPhase > 2){
		printf("%s\n", "[Phase 3]");
		testJogPositive(fd);
		printf("%s\n", "[Phase 3] CLEAR!");		
	}
	if(testPhase > 3){
		printf("%s\n", "[Phase 4]");
		testJogNegative(fd);
		printf("%s\n", "[Phase 4] CLEAR!");		
	}
	if(testPhase > 4){
		printf("%s\n", "[Phase 5]");
		testInclimentalMode(fd);
		printf("%s\n", "[Phase 5] CLEAR!");		
	}
	if(testPhase > 5){
		printf("%s\n", "[Phase 6]");
	
		buffered_write(fd, "MASK\r", 5);
		len=send_receive(fd, "RRX\r", 4, buffer);
		buffer[len] = 0;
		printf("[MASK]%s",buffer);
	
	    //usleep(100*1000);

		res = monitorAlermState(fd);
		if(res != 0){
			printf("%s\n", "Test failed with error. ");
			return res;
		}
	
	    len=send_receive(fd, "DOST\r", 5, buffer);
		buffer[len] = 0;
		printf("[DOST]%s",buffer);

	    buffered_write(fd, "SVON\r", 5);
		len=send_receive(fd, "RRX\r", 4, buffer);
		buffer[len] = 0;
		printf("[SVON]%s",buffer);
	
	    usleep(4000*1000);

//    len=send_receive(fd, "SRDYS\r", 6, buffer);
//	buffer[len] = 0;
//	printf("[SRDYS]%s",buffer);

	    len=send_receive(fd, "DOST\r", 5, buffer);
		buffer[len] = 0;
		printf("[DOST]%s",buffer);
	
	    len=send_receive(fd, "ECRS\r", 5, buffer);
		buffer[len] = 0;
		printf("[ECRS]%s",buffer);
	


		res = monitorAlermState(fd);
		if(res != 0){
			printf("%s\n", "Test failed with error. ");
			return res;
		}

		buffered_write(fd, "INC\r", 4);
		len=send_receive(fd, "RRX\r", 4, buffer);
		buffer[len] = 0;
		printf("[INC]%s",buffer);

	    len=send_receive(fd, "DOST\r", 5, buffer);
		buffer[len] = 0;
		printf("[DOST]%s",buffer);

	    len=send_receive(fd,"#30\r", 4, buffer);
		buffer[len] = 0;
		printf("[#30]%s",buffer);
		buffered_write(fd, "#30=1\r", 6);
		len=send_receive(fd, "RRX\r", 4, buffer);
		buffer[len] = 0;
		printf("[#30=1]%s",buffer);
	    len=send_receive(fd,"#30\r", 4, buffer);
		buffer[len] = 0;
		printf("[#30]%s",buffer);


		usleep(3000*1000);
		
		buffered_write(fd, "HZ-\r", 4);
		len=send_receive(fd, "RRX\r", 4, buffer);
		buffer[len] = 0;
		printf("[HZ-]%s",buffer);
//		buffered_write(fd, "HZ+\r", 4);
//		len=send_receive(fd, "RRX\r", 4, buffer);
//		buffer[len] = 0;
//		printf("[HZ+]%s",buffer);

		int var;
		for (var = 0; var < 300; ++var) {
		    len=send_receive(fd, "PACT\r", 5, buffer);
			buffer[len] = 0;
			printf("[PACT]%s",buffer);
		
		    len=send_receive(fd, "VACT\r", 5, buffer);
			buffer[len] = 0;
			printf("[VACT]%s",buffer);
			usleep(100*1000);
		}

    buffered_write(fd, "S\r", 2);
    len=send_receive(fd,"RRX\r", 4, buffer);
	buffer[len] = 0;
	printf("[S]%s",buffer);

		buffered_write(fd, "PUL\r", 4);
		len=send_receive(fd, "RRX\r", 4, buffer);
		buffer[len] = 0;
		printf("[PUL]%s",buffer);

	    len=send_receive(fd, "DOST\r", 5, buffer);
		buffer[len] = 0;
		printf("[DOST]%s",buffer);

//	    len=send_receive(fd,"#12\r", 4, buffer);
//		buffer[len] = 0;
//		printf("[#12=]%s",buffer);

	    buffered_write(fd, "SVOF\r", 5);
	    
	    usleep(200*1000);
	    
	    len=send_receive(fd,"RRX\r", 4, buffer);
		buffer[len] = 0;
		printf("[SVOF]%s",buffer);
	
	    usleep(100*1000);
	
		res = monitorAlermState(fd);
		if(res != 0){
			printf("%s\n", "Test failed with error. ");
			return res;
		}
	
	//    len=send_receive(fd, "SRDYS\r", 6, buffer);
	//	buffer[len] = 0;
	//	printf("[SRDYS]%s",buffer);
	
	    len=send_receive(fd, "DOST\r", 5, buffer);
		buffer[len] = 0;
		printf("[DOST]%s",buffer);

		printf("%s\n", "[Phase 6] CLEAR!");		
	}
	printf("%s\n", "Test end. ");

	return 0;
	
}

/* --------------------------------------------------------------------- */
/* メイン                                                                */
/* --------------------------------------------------------------------- */

int main(int argc,char *argv[])
{
    int fd;
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
 
      int len;                            //  受信データ数（バイト）
     char buffer[BUFF_SIZE];    // データ受信バッファ
    
	test_by_sequence(fd, 1);
	
    len=send_receive(fd, "DOST\r", 5, buffer);
	buffer[len] = 0;
	printf("[DOST]%s",buffer);
	
	while(len > 1){
    len=read(fd,buffer,BUFF_SIZE);
	/* 受信したデータ*/
	buffer[len] = 0;
	printf("while:%s:%d\n",buffer, len);
	}
	exit(0);
}

