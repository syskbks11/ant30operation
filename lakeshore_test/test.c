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

#define DEV_NAME    "/dev/tty.usbserial-FTRTKA50"        // デバイスファイル名
#define BAUD_RATE    B9600                // RS232C通信ボーレート
#define BUFF_SIZE    4096                 // 適当

// シリアルポートの初期化
void serial_init(int fd)
{
    printf("%s\n", "serial_init()");
    struct termios tio, oldtio;//080905 in
//    struct termios tio;//080905 out
	tcgetattr(fd, &oldtio); //080905 in; 現在の設定を退避
	
    memset(&tio,0,sizeof(tio));
    tio.c_cflag = CS7 | CLOCAL | CREAD| PARODD;//081021 in; 7 Data, Parity odd
    tio.c_oflag = 0;//080905 in; rawモード
    tio.c_lflag = ICANON;//080905 in;カノニカル入力
    tio.c_iflag = IGNCR;//080905 in; CRを無視する。
    //tio.c_iflag = ICRNL;//080905 in; CRをNLに変換する。
    
    tio.c_cc[VTIME] = 100;
    // ボーレートの設定
    cfsetispeed(&tio,BAUD_RATE);
    cfsetospeed(&tio,BAUD_RATE);
    // デバイスに設定を行う
    tcflush(fd, TCIFLUSH);//080905 in;モデムラインのクリア（要るか不明）
    tcsetattr(fd,TCSANOW,&tio);
    printf("%s\n", "serial_init() returns. ");
}

int send_receive(int fd, unsigned char* buf, const unsigned int bufSize)
{
	write(fd, buf, bufSize);
         int len;                            //  受信データ数（バイト）
         unsigned char buffer[BUFF_SIZE];    // データ受信バッファ
    	printf("%s\n", "while(). ");
        //int i;
        // ここで受信待ち
        len=read(fd,buffer,BUFF_SIZE);
        printf("%s\n", "data recieved. ");
        if(len < 0){
        	return len;
        }
        // 受信したデータを 16進数形式で表示    
//        for(i=0; i<len; i++){
//           printf("%02X ",buffer[i]);
//       }
		/* 受信したデータ*/
		buffer[len] = 0;
		printf("%s:%d\n",buffer, len);
		return len;
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


/* --------------------------------------------------------------------- */
/* メイン                                                                */
/* --------------------------------------------------------------------- */

int main(int argc,char *argv[]){
    int fd;

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
    
    buffered_write(fd, "*CLS\r\n", 6);
    buffered_write(fd, "*IDN?\r\n", 7);
    //write(fd, "RRX", 6);
    //usleep(800*1000);
    //write(fd, "SVON\r\n", 7);
    //usleep(3000*1000);
    //write(fd, "SRDYS\r\n", 8);
//         int len;                            //  受信データ数（バイト）
//         unsigned char buffer[BUFF_SIZE];    // データ受信バッファ
//         // ここで受信待ち
//        len=read(fd,buffer,BUFF_SIZE);
//		/* 受信したデータ*/
//		buffer[len] = 0;
//		printf("%s:%d\n",buffer, len);

    // メインの無限ループ
    while(1){
         int len;                            //  受信データ数（バイト）
         unsigned char buffer[BUFF_SIZE];    // データ受信バッファ
    	printf("%s\n", "while(). ");
        //int i;
        // ここで受信待ち
        len=read(fd,buffer,BUFF_SIZE);
        printf("%s\n", "data recieved. ");
        if(len==0){
            // read()が0を返したら、end of file
            // 通常は正常終了するのだが今回は無限ループ
            continue;
        }
        if(len<0){
            printf("%s: ERROR\n",argv[0]);
            // read()が負を返したら何らかのI/Oエラー
            perror("");
            exit(2);
        }
        // read()が正を返したら受信データ数

        // 受信したデータを 16進数形式で表示    
//        for(i=0; i<len; i++){
//           printf("%02X ",buffer[i]);
//       }
		/* 受信したデータ*/
		buffer[len] = 0;
		printf("%s:%d\n",buffer, len);
        printf("\n");
    }
}
