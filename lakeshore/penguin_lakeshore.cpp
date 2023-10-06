#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>


#include "penguin_lakeshore.h"

#define BUF_LEN 100000


/*
 * singleton, since we have only one chopper motor. 
 */
typedef struct penguin_lakeshore_s{
  int id;
  char message[256];
}penguin_lakeshore_t;

static penguin_lakeshore_t p;

static int _receive(int id);
//static int _send_receive(int id, char* buf, const unsigned int bufSize);
//static int _send_receive(const char* cmd, int length);
static void _buffered_write(int id, const char* buf, const unsigned int size);
static int _send_command(char const* cmd);
//static int _receive_more(int id, int length);

int penguin_lakeshore_init(){

  char devName[64];
  //sprintf(devName, "/dev/ttyUSB%d", 6);
  sprintf(devName, "/dev/tty.usbserial-FTRTKA50", 6);

  /* open device file */
  const int fd = open(devName,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。
  if(fd<0){
    // デバイスの open() に失敗したら
    perror(devName);
    exit(1);
  }
  printf("%s opened as a penguin_lakeshore.\n", devName);

  /* configure the port */
  struct termios tio, oldtio;//080905 in
  //    struct termios tio;//080905 out
  tcgetattr(fd, &oldtio); //080905 in; 現在の設定を退避

  memset(&tio,0,sizeof(tio));
  tio.c_cflag = CS7 | CLOCAL | CREAD| PARENB| PARODD;//081021 in; 7 Data, Parity odd
  tio.c_oflag = 0;//080905 in; rawモード
  tio.c_lflag = ICANON;//080905 in;カノニカル入力
  tio.c_iflag = IGNCR;//080905 in; CRを無視する。

  tio.c_cc[VTIME] = 100;
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
  //printf("%s\n", "penguin_heater_init() returns. ");
  return 0;
}

int penguin_lakeshore_end(){
  if(close(p.id))
    printf("some trouble when close.");

  return 0;
}
int penguin_lakeshore_get_identification(){
  if(penguin_lakeshore_send("*IDN?")){
    printf("penguin_lakeshore_get_identification(); problem to send.");
  }
  const int bytes_returned = penguin_lakeshore_receive();
  if (bytes_returned <= 0) {
    printf("penguin_lakeshore_get_identification(); problem to receive.");
    return 1;
  }
  return 0;//normal end
}

double penguin_lakeshore_get_temperature(int channel){
  char cmd[16];

  sprintf(cmd, "KRDG? %d", channel);
	
  if(penguin_lakeshore_send(cmd)){
    printf("penguin_lakeshore_get_temperature(); problem to send.");
  }
  const int bytes_returned = penguin_lakeshore_receive();
  if (bytes_returned <= 0) {
    printf("penguin_lakeshore_get_temperature(); problem to receive.");
    return -99999;
  }
  double rValue;
  sscanf(p.message, "%lf\n", &rValue);
  //printf("rValue=%lf\n", rValue);
  return rValue;
  /*
    double res = rValue/10.0;
    printf("res = %f\n", res);
    return res;
  */
}


int penguin_lakeshore_send(char const* cmd){
  //	printf("penguin_heater_send_command(%s, %d)\n", cmd, length);
  return _send_command(cmd);
}

int _send_command(char const* cmd){
  //	printf("_send_command(");
  char frame[256];


  int len = strlen(cmd);
  if(len >= 256){
    printf("_send_command(); too long command\n");
    return -1;
  }
  /*
    int i = 0;
    for(i = 0; i < len; i++){
    printf("%x ", cmd[i]);
    }
    printf(")\n");
  */
  strncpy(frame, cmd, len);
  frame[len]='\r';
  frame[len+1]='\n';
  frame[len+2]='\0';
  _buffered_write(p.id, frame, len+2);
  return 0;

}
/*
  int _isMessageCompleted(){
  return (int)strstr(p.message, "\x03");
  }
*/
int penguin_lakeshore_receive(){
  memset(p.message, 0, 256);
  int res = _receive(p.id);
  /*
    while(!_isMessageCompleted()){
    res = _receive_more(p.id, res);
    }
  */
  return res;
}

char* penguin_lakeshore_getMessage(){
  return p.message;

}



void _buffered_write(int id, const char* buf, const unsigned int size){
  //void _buffered_write(int id, uint8_t* buf, const unsigned int size){
  //	printf("_buffered_write(%d)\n", id);

  const char *p = buf;
  const char * const endp = p + size;
  //    const unsigned char *p = buf;
  //	const unsigned char * const endp = p + size;
  /*
    printf("_buffered_write(");
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
int _receive(int id){
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

/*
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
*/
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
  //	printf("[]%s:%d\n", p.message, len);
  return length+len;
*/
/*
  glacier_serial_wait_for_massage_complete(id);
  char* tmp = (char*)glacier_serial_get_massage(id);
  int len = strlen(tmp);
  strncpy(buffer, tmp, len+1);
  return len;
*/
/*
  }
*/
