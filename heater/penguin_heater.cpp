#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>


#include "penguin_heater.h"

#define BUF_LEN 100000

/*
 * singleton, since we have only one chopper motor. 
 */
typedef struct penguin_heater_s{
  int id;
  char message[256];
}penguin_heater_t;

static penguin_heater_t p;

static int _receive(int id);
//static int _send_receive(int id, char* buf, const unsigned int bufSize);
static int _send_receive(const char* cmd, int length);
static void _buffered_write(int id, const char* buf, const unsigned int size);
static int _send_command(char const* cmd, int length);
static int _receive_more(int id, int length);

int penguin_heater_init(){

  char devName[64];
  sprintf(devName, "/dev/tty.usbserial-FTRTKA50");

  /* open device file */
  const int fd = open(devName,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。
  if(fd<0){
    // デバイスの open() に失敗したら
    perror(devName);
    exit(1);
  }
  printf("%s opened as a penguin_heater.\n", devName);

  /* configure the port */
  struct termios tio, oldtio;//080905 in
  //    struct termios tio;//080905 out
  tcgetattr(fd, &oldtio); //080905 in; 現在の設定を退避

  memset(&tio,0,sizeof(tio));
  tio.c_cflag = CS8 | CSTOPB | CLOCAL | CREAD;
  tio.c_oflag = 0;//080905 in; rawモード
  //	tio.c_lflag = ICANON;//080905 in;カノニカル入力
  tio.c_lflag = 0;//081215 csslにならう
  //tio.c_iflag = ICRNL;//080905 in; CRをNLに変換する。

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


double penguin_heater_get_temperature(int addr){
  //printf("penguin_heater_get_temperature(); invoked.\n");
  if(penguin_heater_send_get_command(addr, "PV1")){
    printf("penguin_heater_get_temperature(); problem to send.\n");
  }
  const int bytes_returned = penguin_heater_receive();
  if (bytes_returned <= 0) {
    printf("penguin_heater_get_temperature(); problem to receive.\n");
    return -99999;
  }else if(bytes_returned != 13){//090206 in
    printf("penguin_heater_get_temperature(); received bytes expected 13 but (%d).\n", bytes_returned);
    return -99999;
  }
  int rAddr, rValue;
  sscanf(p.message, "\x02%d\x06PV1%d\x03", &rAddr, &rValue);
  //	printf("rAddr=%d, rValue=%d\n", rAddr, rValue);
  if(addr != rAddr){
    printf("penguin_heater_get_temperature(); Awful!!\n");
  }
  //printf("penguin_heater_getData(); returns.\n");
  return rValue/10.0;
  /*
    double res = rValue/10.0;
    printf("res = %f\n", res);
    return res;
  */
}


/**
 * addr: 1, 2, 3, ..., or 9.
 * cmd: three characters.
 */
int penguin_heater_send_get_command(int addr, char const* cmd){
  char frame[256];
  sprintf(frame, "\x02%.2dR%c%c%c\x03", addr, cmd[0], cmd[1], cmd[2]);
  /*
    cmd[0]=0x02;
    cmd[1]='0';
    cmd[2]='1';
    cmd[3]='R';
    cmd[4]='P';
    cmd[5]='V';
    cmd[6]='1';
    cmd[7]=0x03;
  */
  return penguin_heater_send_frame(frame, 8);
}

int penguin_heater_send_set_command(int addr, char const* cmd, int value){
  char frame[256];
  sprintf(frame, "\x02%.2dW%c%c%c%.5d\x03", addr, cmd[0], cmd[1], cmd[2], value);
  return penguin_heater_send_frame(frame, 13);
}

int penguin_heater_send_frame(char const* cmd, int length){
  //	printf("penguin_heater_send_command(%s, %d)\n", cmd, length);
  return _send_command(cmd, length);
}

int _send_command(char const* cmd, int length){
  //	printf("_send_command(");
  int i = 0;
  /*
    for(i = 0; i < length; i++){
    printf("%x ", cmd[i]);
    }
    printf(")\n");
  */
  //int len = strlen(cmd);
  _buffered_write(p.id, cmd, length);
  return 0;

}

char* _isMessageCompleted(){
  return strstr(p.message, "\x03");
}

int penguin_heater_receive(){
  int res = _receive(p.id);
  int i=0;
  while(_isMessageCompleted()!=NULL){
    //printf("penguin_heater_receive(); not enough, read again.\n");
    if(i >= 7){//090205 in
      //		if(i >= 2){
      printf("penguin_heater_receive(); timeout.\n");
      fflush(stdout);//090113 in
      break;//090113 in
    }
    res = _receive_more(p.id, res);
    i++;
  }
  return res;
}

char* penguin_heater_getMassage(){
  return p.message;

}



void _buffered_write(int id, const char* buf, const unsigned int size){
  //void _buffered_write(int id, uint8_t* buf, const unsigned int size){
  //	printf("_buffered_write(%d)\n", id);

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

int _send_receive(const char* cmd, int length){
  /*
    printf("_send_receive(%s)\n", cmd);
  */

  int ret = _send_command(cmd, length);
  if(ret){
    printf("_send_receive(); send failed.\n");
    return ret;
  }
  return _receive(p.id);
}

int _receive(int id){
  //	printf("_receive(%d)\n", id);
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
    printf("%02X ",p.message[i]);
    }
  */
  //buf[0] = 'x';
  /* 受信したデータ*/
  p.message[len] = 0;
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

