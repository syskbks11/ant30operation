#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>


#include "penguin_chopper.h"

#define BUF_LEN 100000


/*
 * singleton, since we have only one chopper motor.
 */
typedef struct penguin_chopper_s{
  int id;
  char message[256];
  char command[256];
}penguin_chopper_t;

static penguin_chopper_t p;

static int currentPulse;

static int _receive(int id);
static int _send_receive(const char* cmd);
static void _buffered_write(int id, const char* buf, const unsigned int size);
static int _send_command(char const* cmd);
static int _receive_more(int id, int length);
static int _isMessageCompleted();

char penguin_chopper_getStatus(){
  if(penguin_chopper_askStatus()){
    printf("penguin_chopper_getStatus(); penguin_chopper_askStatus() failed.\n");
    return '.';
  }
  return p.message[0];
}
int penguin_chopper_getSensorStatus(){
  if(penguin_chopper_askSensorStatus()){
    printf("penguin_chopper_getSensorStatus(); penguin_chopper_askSensorStatus() failed.\n");
    return -1;
  }
  int res = strtod(p.message, NULL);
  return res;
}

int penguin_chopper_getCurrentPulse(){
  return currentPulse;
}

int penguin_chopper_reset(){
  _send_command("RESET:");
  return 0;//normal end
}

int penguin_chopper_open(){
  return _send_receive("ABS X 0");
}

int penguin_chopper_close(){
  return _send_receive("ABS X 1000000");
}

int penguin_chopper_stop(){
  return _send_receive("SST X");
}

int penguin_chopper_isMoving(){
  if(penguin_chopper_askStatus()){
    printf("penguin_chopper_isMoving(), penguin_chopper_askStatus() failed.\n");
    return 1;
  }
  return p.message[0]=='D';
}
int penguin_chopper_isAtOpen(){
  int pulse = penguin_chopper_getPulse();
  if(pulse == -10000){
    printf("penguin_chopper_isAtOpen(); penguin_chopper_getPulse() failed.\n");
    return 0;
  }
  if(pulse < 100){
    return 1;//open
  }
  return 0;//not open
}
int penguin_chopper_isAtClose(){
  int pulse = penguin_chopper_getPulse();
  if(pulse == -10000){
    printf("penguin_chopper_isAtClose(); penguin_chopper_getPulse() failed.\n");
    return 0;
  }
  if(pulse > 20000){//20090202 at Sphinx
    //	if(pulse > 30000){//20090201 at Sphinx
    //	if(pulse > 20000){//at Univ. Tsukuba
    return 1;//close
  }
  return 0;//not close
}

int penguin_chopper_askStatus(){
  int ret = _send_receive("RDR X");
  if(ret < 0){
    printf("penguin_chopper_askStatus(), failed\n");
    return -1;
  }
  if(ret == 0){
    printf("penguin_chopper_askStatus(), Something wrong.\n");
    return -1;
  }
  if(ret != 3){
    printf("penguin_chopper_askStatus(), received message, expected 3 but %d.\n", ret);
    return -1;
  }
  return 0;//normal end
}

int penguin_chopper_askSensorStatus(){
  int ret = _send_receive("I:A");
  if(ret < 0){
    printf("penguin_chopper_askSensorStatus(), failed\n");
    return -1;
  }
  if(ret == 0){
    printf("penguin_chopper_askSensorStatus(), Something wrong.\n");
    return -1;
  }
  if(ret != 6){
    printf("penguin_chopper_askSensorStatus(), received message, expected 6 but %d.\n", ret);
    return -1;
  }
  return 0;
}

int penguin_chopper_getPulse(){
  int ret = _send_receive("Q:A1");
  if(ret < 0){
    printf("penguin_chopper_getPulse(), failed\n");
    return -10000;
  }
  if(ret == 0){
    printf("penguin_chopper_getPulse(), Something wrong.\n");
    return -10000;
  }
  int res = strtod(p.message, NULL);
  currentPulse = res;
  return res;
}

int penguin_chopper_setPulse(int pulse){
  char cmd[256];
  memset(cmd, 0, 256);
  sprintf(cmd, "MGO:A%d", pulse);
  _send_receive(cmd);
  return 0;
}


int penguin_chopper_getParameter(int num){
  char cmd[256];
  memset(cmd, 0, 256);
  const int num1 = (int)(num/10);
  const int num2 = num-num1*10;
  sprintf(cmd, "P:%d%dR", num1, num2);
  _send_receive(cmd);
  return 0;
}

int penguin_chopper_setParameter(int num, int value){
  char cmd[256];
  memset(cmd, 0, 256);
  const int num1 = (int)(num/10);
  const int num2 = num-num1*10;
  sprintf(cmd, "P:%d%dA%d", num1, num2, value);
  int ret = _send_receive(cmd);
  if(ret < 0){
    printf("penguin_chopper_setParameter(), failed\n");
    return -1;
  }
  if(ret == 0){
    printf("penguin_chopper_setParameter(), Something wrong.\n");
    return -1;
  }
  int res = strtod(p.message, NULL);
  return res;
}

/**
 * Please invoke this function after penguin_chopper_init() ends successfully.
 */
int penguin_chopper_configure_settings(){
  _send_receive("X:1");
  penguin_chopper_setParameter(6, 1);
  penguin_chopper_setParameter(18, 0);
  penguin_chopper_setParameter(5, 0);
  penguin_chopper_getParameter(6);
  penguin_chopper_getParameter(18);
  penguin_chopper_getParameter(5);
  _send_receive("S:A01");
  _send_receive("D:A1000P10000P100");
  _send_receive("P:11A10000");
  return 0;
}

/**
 * if you want to use penguin_chopper_close() and penguin_chopper_open(), 
 * invoke this function after penguin_chopper_configure_settings(). 
 */
int penguin_chopper_configure_origin(){
  _send_receive("JGO:A-");
  int time = 0;
  while(penguin_chopper_isMoving()){
    usleep(1000*1000);
    time++;
    if(time > 40){
      penguin_chopper_stop();
      printf("penguin_chopper_configure_origin(); SERIOUS PROBLEM!! CHECK THE HARDWARE!!\n");
      return -1;
    }
  }
  penguin_chopper_askSensorStatus();
  if(p.message[0] != '0')
    printf("penguin_chopper_configure_origin(); penguin_chopper_askSensorStatus() [0] 0 expected but %c\n", p.message[0]);
  if(p.message[1] != '1')
    printf("penguin_chopper_configure_origin(); penguin_chopper_askSensorStatus() [1] 1 expected but %c\n", p.message[1]);
  if(p.message[2] != '1')
    printf("penguin_chopper_configure_origin(); penguin_chopper_askSensorStatus() [2] 1 expected but %c\n", p.message[2]);
  if(p.message[3] != '0')
    printf("penguin_chopper_configure_origin(); penguin_chopper_askSensorStatus() [3] 0 expected but %c\n", p.message[3]);
  _send_receive("R:A0");	
  return 0;//normal end
}

int penguin_chopper_init(const char* devName){

  /* open device file */
  const int fd = open(devName,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。

  if(fd<0){ // if failed to open
    perror(devName);
    exit(1);
  }
  printf("%s opened as a penguin_chopper.\n", devName);

  /* configure the port */
  struct termios tio, oldtio;//080905 in
  tcgetattr(fd, &oldtio); //080905 in; 現在の設定を退避

  memset(&tio,0,sizeof(tio));
  tio.c_cflag = 0;
  cfmakeraw(&tio);                    // RAWモード
  tio.c_cflag |= CREAD;               // 受信有効
  tio.c_cflag |= CLOCAL;              // ローカルライン（モデム制御なし）
  tio.c_cflag |= CS8;                 // データビット:8bit
  tio.c_cflag &= ~PARENB;             // パリティ:None
  tio.c_cflag &= ~PARODD;             // パリティ:None

  cfsetispeed( &tio, B115200 );
  cfsetospeed( &tio, B115200 );

  // tio.c_cc[VTIME] = 100;
  // tio.c_cc[VMIN] = 0;

  // デバイスに設定を行う
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&tio);

  p.id = fd;
  memset(p.message, 0, 256);
  memset(p.command, 0, 256);
  printf("%s\n", "penguin_chopper_init() returns. ");
  return 0;
}

int penguin_chopper_end(){
  if(close(p.id))
    printf("some trouble when close.\n");
  return 0;
}

int penguin_chopper_send_command(char const* cmd){
  printf("penguin_chopper_send_command(%s)\n", cmd);
  return _send_command(cmd);
}

int _send_command(char const* cmd){
  int len = strlen(cmd);
  if(len > 256){
    printf("command is too long (%d)\n", len);
    return -1;
  }
  memcpy(p.command, cmd, len);
  p.command[len] = 0x00;
  //p.command[len+1] = '\n';
  //_buffered_write(p.id, p.command, len+2);
  _buffered_write(p.id, p.command, len+1);
  memset(p.command, 0, 256);
  return 0;
}

int penguin_chopper_receive(){
  int res = _receive(p.id);
  int i=0;
  while(!_isMessageCompleted()){
    //printf("penguin_chopper_receive(); not enough, read again.\n");
    if(i >= 9){//090218 in
      //		if(i >= 7){//090205 in
      //		if(i >= 2){
      printf("penguin_chopper_receive(); timeout.\n");
      fflush(stdout);//090113 in
      break;//090113 in
    }
    res = _receive_more(p.id, res);
    i++;
  }
  printf("[]%s:%d\n", p.message, res);
  return res;
}


char* penguin_chopper_getMassage(){
  return p.message;

}


void _buffered_write(int id, const char* buf, const unsigned int size){

  const char *p = buf;
  const char * const endp = p + size;

  while (p < endp) {
    int num_bytes = write(id, p, endp - p);
    if (num_bytes < 0) {
      perror("write failed");
      break;
    }
    p += num_bytes;
  }

}

int _send_receive(const char* cmd){
  printf("_send_receive(%s)\n", cmd);
  int ret = _send_command(cmd);
  if(ret){
    printf("_send_receive(); send failed.\n");
    return ret;
  }
  return penguin_chopper_receive();
  //	return _receive(p.id);
}

int _isMessageCompleted(){
  if(strstr(p.message, "\r")){
    if(strstr(p.message, "\n")){
      return 1;
    }
  }
  return 0;
}

int _receive(int id){
  p.message[0] = 0;
  //printf("_receive(%d)\n", id);
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
  p.message[len] = 0;
  //printf("[]%s:%d\n", p.message, len);
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


