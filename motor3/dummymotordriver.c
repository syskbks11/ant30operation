#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//open(), O_RDWR
#include <stdlib.h>
//exit()

#include "motordriver.h"
#include "dummymotordriver.h"

#define BUFF_SIZE    4096                 // 適当


int main(int argc,char *argv[])
{
	int fd;
	// デバイスファイル（シリアルポート）オープン
    fd = open(DEV_NAME,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。
//    fd = open(DEV_NAME,O_RDWR);//080905 out; 読み書き可能で開く
    if(fd<0){
        // デバイスの open() に失敗したら
	printf("??");
        perror(argv[1]);
         exit(1);
    }
      int len = 0;                            //  受信データ数（バイト）
     char buffer[BUFF_SIZE];    // データ受信バッファ
	len=read(fd,buffer,BUFF_SIZE);
	while(len > 1){
		len=read(fd,buffer,BUFF_SIZE);
		/* 受信したデータ*/
		//printf("while:%s:%d\n",buffer, len);
		buffer[0]='0';
		buffer[1] = '\r';
		buffer[2] = 0;
		buffered_write(fd, buffer, 2);
	}
	exit(0);

}
