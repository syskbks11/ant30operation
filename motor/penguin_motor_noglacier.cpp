/*!
  ¥file motordriver.cpp
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
//#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <stdint.h>//for uint8_t

#include "penguin_motor.h"
//#include "../glacier/serial/glacier_serial.h"

#define BUFF_SIZE    4096                 // 適当


struct penguin_motor_s{
  int id;
  int positiveSoftLimit;
  int negativeSoftLimit;
};


#define STRLEN 12
char tmpstr[STRLEN+1];


static void _buffered_write(int fd, char* buf, const unsigned int size);
//static void _buffered_write(int fd, uint8_t* buf, const unsigned int size);
static int _send_receive(int fd, char* buf, const unsigned int bufSize, char* buffer);
static int _send_receive_int_value(const int fd, char* cmd, const unsigned int cmdSize);

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
penguin_motor_t* penguin_motor_init(const char* devName, int positiveSoftLimit, int negativeSoftLimit){

  /* open device file */
  const int fd = open(devName,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。
  //    fd = open(DEV_NAME,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。
  //    fd = open(DEV_NAME,O_RDWR);//080905 out; 読み書き可能で開く
  if(fd<0){
    // デバイスの open() に失敗したら
    perror(devName);
    exit(1);
  }
  printf("%s opened as a penguin_motor.\n", devName);

  /* configure the port */
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
  cfsetispeed(&tio, 9600);
  cfsetospeed(&tio, 9600);
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
  //	penguin_motor_t pm = {id, positiveSoftLimit, negativeSoftLimit};

  penguin_motor_t* pm = (penguin_motor_t*)malloc(sizeof(penguin_motor_t));
  pm->id = fd;
  pm->positiveSoftLimit = positiveSoftLimit;
  pm->negativeSoftLimit = negativeSoftLimit;
  printf("%s\n", "penguin_motor(ng)_init() returns. ");
  return pm;
}

int penguin_motor_end(penguin_motor_t* pm){
  if(close(pm->id))
    printf("some trouble when close.");
  /*
    int res = glacier_serial_dettach(pm->id);
  */
  free(pm);
  pm = NULL;
  return 0;
  //	return res;
  //cssl_stop();
}

void penguin_motor_buffered_write(const penguin_motor_t* pm, char* buf, const unsigned int size){
  _buffered_write(pm->id, buf, size);
  //	glacier_serial_putdata(pm->id, (uint8_t*)buf, size);
}

void _buffered_write(int id, char* buf, const unsigned int size){
  //void _buffered_write(int id, uint8_t* buf, const unsigned int size){
  printf("_buffered_write(%d)\n", id);

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

int penguin_motor_send_receive(const penguin_motor_t* pm, char* buf, const unsigned int bufSize, char* buffer){
  printf("penguin_motor_send_receive() : %d\n", pm->id);
  return _send_receive(pm->id, buf, bufSize, buffer);
}

int _send_receive(int id, char* buf, const unsigned int bufSize, char* buffer){
  printf("_send_receive(%d)\n", id);
  _buffered_write(id, buf, bufSize);


  int len;                            //  受信データ数（バイト）
  // ここで受信待ち
  len = read(id, buffer, BUFF_SIZE);
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

  /*
    glacier_serial_wait_for_massage_complete(id);
    char* tmp = (char*)glacier_serial_get_massage(id);
    int len = strlen(tmp);
    strncpy(buffer, tmp, len+1);
    return len;
  */
}

int penguin_motor_getPreviousCommand(const penguin_motor_t* pm, char* buffer)
{
  int len;
  len = _send_receive(pm->id, "RRX\r", 4, buffer);
  if(len < 0){
    perror("Can't get previous command. ");
    len = 0;
  }
  buffer[len] = 0;
  return len;
}

int penguin_motor_send_receive_int_value(const penguin_motor_t* pm, char* cmd, const unsigned int cmdSize)
{
  printf("penguin_motor_send_receive_int_value(%d)\n", pm->id);
  return _send_receive_int_value(pm->id, cmd, cmdSize);
}

int _send_receive_int_value(const int fd, char* cmd, const unsigned int cmdSize)
{
  printf("_send_receive_int_value(%d)\n", fd);
  //char rest[BUFF_SIZE];
  char buffer[BUFF_SIZE];    // データ受信バッファ
  const int len = _send_receive(fd, cmd, cmdSize, buffer);
  if(len == 0){
    perror("No data received. ");
    return -1;		
    //exit(1);
  }
  //buffer[len] = 0;
  //double res = atof(buffer);
  const int resint = strtod(buffer, NULL);
  return resint;
}

int penguin_motor_getAlermState(const penguin_motor_t* pm)
{
  int resint = _send_receive_int_value(pm->id, "ALST\r", 5);
  if(resint != ALST_OK){
    //perror("!");
    //printf("error: ALST=%d\n", resint);
  }
  return resint;
}
/*
  int penguin_motor_getVelocityActual(const penguin_motor_t* pm)
  {
  printf("b:pm->id = %d\n", pm->id);
  const int res =_send_receive_int_value(pm->id, "VACT\r", 5);
  printf("a:pm->id = %d\n", pm->id);
  return res;

  //	return _send_receive_int_value(pm->id, "VACT\r", 5);
  }

  int penguin_motor_getPulseActual(const penguin_motor_t* pm)
  {
  printf("b:pm->id = %d\n", pm->id);
  printf("pm=%p\n", pm);
  printf("id=%d\n", penguin_motor_getID(pm));
  const int res =_send_receive_int_value(pm->id, "PACT\r", 5);
  printf("a:pm->id = %d\n", pm->id);
  printf("pm=%p\n", pm);
  printf("id=%d\n", penguin_motor_getID(pm));
  return res;

  //	return _send_receive_int_value(pm->id, "PACT\r", 5);
  }
*/
int penguin_motor_getID(const penguin_motor_t* pm){
  return pm->id;
}
/*
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
  return _send_receive_int_value(fd, "#18\r", 4);
  }

  int getPositionLoopConstant(int fd)
  {
  return _send_receive_int_value(fd, "#17\r", 4);
  }

  int getAcceralationFilter(int fd)
  {
  return _send_receive_int_value(fd, "#39\r", 4);
  }

  void clearAlerm(int fd)
  {
  _buffered_write(fd, "RES\r", 4);
  }
*/


void penguin_motor_setVelocity(const penguin_motor_t* pm, int velocity)
{
  char tmp[BUFF_SIZE];
  //char buffer[BUFF_SIZE];    // データ受信バッファ
  sprintf(tmp, "V=%d\r", velocity);
  int pos = strlen(tmp);
  /*
    int pos = 2;
    int i;	
    for(i=2; i < BUFF_SIZE; i++){
    if(tmp[i] == '\0'){
    pos = i;
    break;
    }
    }
  */
  _buffered_write(pm->id, tmp, pos);

  //return resint;
	
}


void penguin_motor_setJogVelocity(const penguin_motor_t* pm, int value)
{
  penguin_motor_setParameter(pm, 23, value);
  /*
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
    _buffered_write(fd, tmp, pos);
  */
  //return resint;
	
}

void penguin_motor_setGain(const penguin_motor_t* pm, int value)
{
  penguin_motor_setParameter(pm, 19, value);
  /*
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
    _buffered_write(fd, tmp, pos);
  */
  //return resint;
	
}

void penguin_motor_setPositionLoopConstant(const penguin_motor_t* pm, int value)
{
  penguin_motor_setParameter(pm, 17, value);
  /*
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
    _buffered_write(fd, tmp, pos);
  */
  //return resint;
	
}

void penguin_motor_setVelocityLoopConstant(const penguin_motor_t* pm, int value)
{
  penguin_motor_setParameter(pm, 18, value);
  /*
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
    _buffered_write(fd, tmp, pos);
  */
  //return resint;
	
}

void penguin_motor_setPulse(const penguin_motor_t* pm, int velocity)
{
  //void setPulse(int fd, int pulse){
  char tmp[BUFF_SIZE];
  //char buffer[BUFF_SIZE];    // データ受信バッファ
  sprintf(tmp, "P=%d\r", velocity);
  int pos = strlen(tmp);
  /*
    int pos = 2;
    int i;	
    for(i=2; i < BUFF_SIZE; i++){
    if(tmp[i] == '\0'){
    pos = i;
    break;
    }
    }
  */
  _buffered_write(pm->id, tmp, pos);

  //return resint;
	
}
/*
  void setSERVOFF(int fd)
  {
  _buffered_write(fd, "SVOF\r", 5);
  }
  void stop(int fd)
  {
  _buffered_write(fd, "S\r", 2);
  }
  void start(int fd)
  {
  _buffered_write(fd, "G\r", 2);
  }

  void goOriginPositive(int fd){
  _buffered_write(fd, "HZ+\r", 4);
  }

  void goOriginNegative(int fd){
  _buffered_write(fd, "HZ-\r", 4);
  }
*/
void penguin_motor_setParameter(const penguin_motor_t* pm, int param, int value)
{
  char tmp[BUFF_SIZE];
  //char buffer[BUFF_SIZE];    // データ受信バッファ
  sprintf(tmp, "#%d=%d\r", param, value);
  int pos = strlen(tmp);
  /*
    int pos = 3;
    int i;	
    for(i=3; i < BUFF_SIZE; i++){
    if(tmp[i] == '\0'){
    pos = i;
    break;
    }
    }
  */
  _buffered_write(pm->id, tmp, pos);

  //return resint;
	
}
int penguin_motor_getParameter(penguin_motor_t* pm, int param){
  char tmp[BUFF_SIZE];
  //char buffer[BUFF_SIZE];    // データ受信バッファ
  sprintf(tmp, "#%d\r", param);
  int pos = strlen(tmp);
  /*
    int pos = 1;
    int i;	
    for(i=1; i < BUFF_SIZE; i++){
    if(tmp[i] == '\0'){
    pos = i;
    break;
    }
    }
  */
  return _send_receive_int_value(pm->id, tmp, pos);
}

