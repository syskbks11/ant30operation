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
#include <sys/ioctl.h>
#include <stdint.h> //for uint8_t

#include "penguin_motor.h"

#define BUFF_SIZE 4096
#define HEADER 0x02
#define TRAILER 0x04
#define SEP 0x3b
#define CMD_ERR "ERR"

struct penguin_motor_s{
  int id;
  int positiveSoftLimit;
  int negativeSoftLimit;
};

#define STRLEN 12
char tmpstr[STRLEN+1];

static void _buffered_write(int fd, char* buf, const unsigned int size);
static int _send_receive(int fd, char* buf, const unsigned int bufSize, char* buffer);
static int _send_receive_int_value(const int fd, char* cmd, const unsigned int cmdSize);
static int _send_receive_hexint_value(const int fd, char* cmd, const unsigned int cmdSize);

void printh(const char* data, unsigned int len) {
  for (unsigned int i=0; i<len; i++) {
    printf("%02x ", data[i]);
  }
  printf("\n");
}

void sprinth(char* buf, const char* data, unsigned int len) {
  for (unsigned int i=0; i<len; i++) {
    sprintf(buf+i*2, "%02x", data[i]);
  }
  buf[(len-1)*2] = 0x00;
}

// シリアルポートの初期化
penguin_motor_t* penguin_motor_init(const char* devName, int positiveSoftLimit, int negativeSoftLimit){

  /* open device file */
  const int fd = open(devName,O_RDWR | O_NOCTTY);//080905 in; 読み書き可能で開く、tty制御をしない。

  if(fd<0){ // if failed to open
    perror(devName);
    return NULL;
  }
  printf("%s opened as a penguin_motor.\n", devName);

  /* configure the port */
  struct termios tio, oldtio;//080905 in
  tcgetattr(fd, &oldtio); //080905 in; 現在の設定を退避

  memset(&tio,0,sizeof(tio));
  tio.c_cflag = 0;
  cfmakeraw(&tio);                    // RAWモード
  tio.c_cflag |= CREAD;               // 受信有効
  tio.c_cflag |= CLOCAL;              // ローカルライン（モデム制御なし）
  tio.c_cflag |= CS8;                 // データビット:8bit
  tio.c_cflag |= PARENB;              // パリティ:Even

  cfsetispeed( &tio, B9600 );
  cfsetospeed( &tio, B9600 );

  // tio.c_cc[VTIME] = 100;
  // tio.c_cc[VMIN] = 0;

  // デバイスに設定を行う
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&tio);

  // penguin_motor_t pm = {id, positiveSoftLimit, negativeSoftLimit};

  penguin_motor_t* pm = (penguin_motor_t*)malloc(sizeof(penguin_motor_t));
  pm->id = fd;
  pm->positiveSoftLimit = positiveSoftLimit;
  pm->negativeSoftLimit = negativeSoftLimit;
  printf("%s\n", "penguin_motor(ng)_init() returns. ");
  penguin_motor_configureInput(pm);
  return pm;
}

int penguin_motor_end(penguin_motor_t* pm){
  if(close(pm->id))
    perror("penguin_motor_end():: ERROR DURING CLOSING PROCESS: close(pm->id)");
  free(pm);
  pm = NULL;
  return 0;
}

int penguin_motor_checkDynamicBreakState(const penguin_motor_t* pm){
  const int dbState = penguin_motor_getDynamicBreakState(pm);
  printf("penguin_motor_checkDynamicBreakState()\n");
  if(dbState==0)
    printf("ON WHEN POWERED-ON and ALWAYS OFF ONCE SURVO-ON. [0]\n");
  else if(dbState==1)
    printf("ON WHEN POWERED-ON and ALERMED. [1]\n");
  else if(dbState==2)
    printf("ON WHEN POWERED-ON and SURVO-OFF. [2]\n");
  return 0;
}

int penguin_motor_checkDOState(const penguin_motor_t* pm){
  const int ecrs = penguin_motor_getDOState(pm);
  printf("penguin_motor_checkDOState()\n");
  printf("OUTPUT BITS\n");
  printf("");
  if(ecrs & 128) printf("1");
  else printf("0");
  if(ecrs &  64) printf("1");
  else printf("0");
  if(ecrs &  32) printf("1");
  else printf("0");
  if(ecrs &  16) printf("1");
  else printf("0");
  if(ecrs &   8) printf("1");
  else printf("0");
  if(ecrs &   4) printf("1");
  else printf("0");
  if(ecrs &   2) printf("1");
  else printf("0");
  if(ecrs &   1) printf("1");
  else printf("0");
  printf("\n");
  const int ecrs_in = (ecrs>>(4*4)) & 0xFFFF;
  printf("INPUT BITS\n");
  printf("H --------------- L\n");
  if(ecrs_in & 16384) printf("1");
  else printf("0");
  if(ecrs_in &  8192) printf("1");
  else printf("0");
  if(ecrs_in &  4096) printf("1");
  else printf("0");
  if(ecrs_in &  2048) printf("1");
  else printf("0");
  if(ecrs_in &  1024) printf("1");
  else printf("0");
  if(ecrs_in &   512) printf("1");
  else printf("0");
  if(ecrs_in &   256) printf("1");
  else printf("0");
  if(ecrs_in &   128) printf("1");
  else printf("0");
  if(ecrs_in &    64) printf("1");
  else printf("0");
  if(ecrs_in &    32) printf("1");
  else printf("0");
  if(ecrs_in &    16) printf("1");
  else printf("0");
  if(ecrs_in &     8) printf("1");
  else printf("0");
  if(ecrs_in &     4) printf("1");
  else printf("0");
  if(ecrs_in &     2) printf("1");
  else printf("0");
  if(ecrs_in &     1) printf("1");
  else printf("0");
  printf("\n");
  return 0;
}

int penguin_motor_checkErrorCorrectionState(const penguin_motor_t* pm){
  const int ecrs = penguin_motor_getErrorCorrectionState(pm);
  printf("penguin_motor_checkDynamicBreakState()\n");
  int res = 0;
  if(ecrs == 0){
    printf("penguin_motor_checkErrorCorrectionState(%d) POSITION NOT CORRECTED\n", pm->id);
    res = 1;
  }
  else if(ecrs == 1){
    res = 0;
  }
  else{
    printf("penguin_motor_checkErrorCorrectionState(%d) INVALID STATUS %d\n", pm->id, ecrs);
    res = -1;
  }
  penguin_motor_checkAlermState(pm);
  return res;
}

int penguin_motor_checkAlermState(const penguin_motor_t* pm){
  const int alerm = penguin_motor_getAlermState(pm);
  if(alerm != 0){
    printf("!!!ALERM ISSUED!!! penguin_motor_checkAlermState()\n");
    printf("Check the motor dirver, consulting with its manual!\n");
    printf("H -------------- L\n");
    printf(" ");
    if(alerm & 4096) printf("1");
    else printf("0");
    if(alerm & 2048) printf("1");
    else printf("0");
    if(alerm & 1024) printf("1");
    else printf("0");
    if(alerm &  512) printf("1");
    else printf("0");
    if(alerm &  256) printf("1");
    else printf("0");
    if(alerm &  128) printf("1");
    else printf("0");
    if(alerm &   64) printf("1");
    else printf("0");
    if(alerm &   32) printf("1");
    else printf("0");
    if(alerm &   16) printf("1");
    else printf("0");
    if(alerm &    8) printf("1");
    else printf("0");
    if(alerm &    4) printf("1");
    else printf("0");
    if(alerm &    2) printf("1");
    else printf("0");
    if(alerm &    1) printf("1");
    else printf("0");
    printf("\n");
  }
  return alerm;
}

void penguin_motor_moveToPositiveLimit(const penguin_motor_t* pm){
  penguin_motor_setPulse(pm, pm->positiveSoftLimit);
}

void penguin_motor_moveToNegativeLimit(const penguin_motor_t* pm){
  penguin_motor_setPulse(pm, pm->negativeSoftLimit);
}

int penguin_motor_configureInput(const penguin_motor_t* pm){
  int res = 0;
  int param = 0;
  param = 0x055;
  printf("====== penguin_motor_configureInput() ==========\n");
  penguin_motor_getParameter(pm, 69);
  printf("==============================================\n");
  penguin_motor_setParameter(pm, 69, param);
  printf("==============================================\n");
  if(penguin_motor_getParameter(pm, 69) == param) res += 1;
  printf("==============================================\n");
  // ----------------------------------------------------
  // PaRameteR 69
  // [00]: external input / [01]: serial communication
  // - lowest  byte 5 = [01][01] = [JOG/TGO][SURBO]
  // - middle  byte 5 = [01][01] = [ORIGIN][START]
  // - highest byte 5 = [00][00] = [POINT][PULSE]
  // ----------------------------------------------------
  param = 0x0006;
  printf("==============================================\n");
  penguin_motor_setParameter(pm, 67, param);
  printf("==============================================\n");
  if(penguin_motor_getParameter(pm, 67) == param) res += 2;
  // ----------------------------------------------------
  // PaRameteR 67
  // Settings for nominal state of input pins
  // [00]: NormalOpen / [01]: NormalClose
  // BIT14--BIT0 (check PRR60-63 for definitions)
  // BIT1,2 set as 1 for outrange limit signals
  printf("==============================================\n");

  return res;
}

int penguin_motor_checkPreviousCommand(const penguin_motor_t* pm, const char* cmd){
  char buffer[256];
  int len=penguin_motor_getPreviousCommand(pm, buffer);
  if(len==0){
    printf("penguin_motor_checkPreviousCommand(); Specified command was [%s], but received [%s].\n", cmd, buffer);
    return 1;
  }
  if(buffer[len-1] != '\n')
    printf("penguin_motor_checkPreviousCommand(), ??\n");
  buffer[len-1]='\0';
  if(strcmp(cmd, buffer) != 0){
    printf("penguin_motor_checkPreviousCommand(); Specified command was [%s], but received [%s].\n", cmd, buffer);
    return 1;
  }
  return 0;//normal end
}

void penguin_motor_buffered_write(const penguin_motor_t* pm, char* buf, const unsigned int size){
  char buffer[BUFF_SIZE];
  _send_receive(pm->id, buf, size, buffer);
  //_buffered_write(pm->id, buf, size);
}

void _buffered_write(int id, char* buf, const unsigned int size){
  const unsigned int sendsize = size + 2;
  char cmd[BUFF_SIZE]; // [HEADER]+[CMD]+[TRAILER]
  sprintf(cmd,"%c%s%c", HEADER,buf,TRAILER);
  cmd[sendsize-1] = TRAILER; // for case if size of `buf` > `size`
  //cmd[sendsize-1] = '\n';
  printf("CMD = %s\n",cmd); // debug SH
  //printh(cmd,sendsize); // debug SH

  const char *p = cmd;
  const char * const endp = cmd + sendsize;
  while (p < endp) {
    int num_bytes = write(id, p, endp - p);
    if (num_bytes < 0) {
      perror("write failed");
      break;
    }
    p += num_bytes;
  }

}

int penguin_motor_send_receive(const penguin_motor_t* pm, char* buf, const unsigned int bufSize, char* buffer){
  //printf("penguin_motor_send_receive() : %d\n", pm->id);
  return _send_receive(pm->id, buf, bufSize, buffer);
}

int _send_receive(int id, char* buf, const unsigned int bufSize, char* buffer){

  _buffered_write(id, buf, bufSize);

  int len=0;                            //  受信データ数（バイト）
  // ここで受信待ち
  char readbuf[BUFF_SIZE];
  unsigned int n_zeroout = 0;
  //printf("RECEIVE\n"); // debug SH
  while(true){
    len += read(id, readbuf+len, BUFF_SIZE);
    //printf("READING... (%d) = ",len);
    //printh(readbuf,len);
    if(n_zeroout==10){
      printf("ERROR penguin_motor _send_receive():: No data recieved 10 times without TRAILER byte \"%02x\". \n", TRAILER);
      break;
    }
    if(len==0) {
      n_zeroout++;
      continue;
    }
    else {
      n_zeroout=0;
    }
    if(readbuf[len-1] == TRAILER) break;
  }
  if(len>2){
    char word[len-1];
    strncpy(word, readbuf+1, len-2);
    word[len-2]=0x00;
    printf("RET = %s (%d)\n",word,len-2);
  }
  else{
    printf("Invalid RET = %s (%d)\n",readbuf,len);
  }
  //printh(readbuf,len); // debug SH

  if(len < 0){
    perror("send_receive(): IO error!");
    return len;
  }
  if(readbuf[len-1]!=TRAILER){
    perror("send_receive(): Invalid trailer!");
  }
  char cmd3[3];
  strncpy(cmd3, readbuf+1, 3); // remove HEADER,CMD,SEP
  if(strcmp(cmd3,CMD_ERR)==0){
    printf("send_receive(): ERROR returned %s \n",readbuf);
    printh(cmd3,3);
  }

  int retlen = len-2-bufSize-1;
  if(retlen>0){
    retlen = len-2-bufSize-1;
    strncpy(buffer, readbuf+2+bufSize, retlen); // remove HEADER,CMD,SEP,TRAILER
    buffer[len-2-bufSize-1]   = 0x00;
  }
  else{
    retlen = 0;
  }
  return retlen;
}

int penguin_motor_getPreviousCommand(const penguin_motor_t* pm, char* buffer)
{
  perror("penguin_motor_getPreviousCommand() This commnand is not supported anymore.");
  int len = 0;
  buffer[len] = 0;
  return len;
}

int penguin_motor_send_receive_int_value(const penguin_motor_t* pm, char* cmd, const unsigned int cmdSize)
{
  return _send_receive_int_value(pm->id, cmd, cmdSize);
}

int _send_receive_int_value(const int fd, char* cmd, const unsigned int cmdSize)
{
  char buffer[BUFF_SIZE];
  const int len = _send_receive(fd, cmd, cmdSize, buffer);
  if(len == 0){
    perror("No data received.");
    return -1;
  }
  const int resint = strtod(buffer, NULL);
  return resint;
}

int penguin_motor_send_receive_hexint_value(const penguin_motor_t* pm, char* cmd, const unsigned int cmdSize)
{
  return _send_receive_hexint_value(pm->id, cmd, cmdSize);
}

int _send_receive_hexint_value(const int fd, char* cmd, const unsigned int cmdSize)
{
  char buffer[BUFF_SIZE];
  const int len = _send_receive(fd, cmd, cmdSize, buffer);
  if(len == 0){
    perror("No data received.");
    return -1;
  }
  char hexbuffer[len+2];
  sprintf(hexbuffer,"0x%s",buffer);
  const int resint = strtod(hexbuffer, NULL);
  //printf("HEXINT CONVERT %s, %s, %d",buffer,hexbuffer,resint);
  return resint;
}

int penguin_motor_getID(const penguin_motor_t* pm){
  return pm->id;
}

void penguin_motor_setVelocity(const penguin_motor_t* pm, int velocity)
{
  char tmp[BUFF_SIZE];
  sprintf(tmp, "V=%d", velocity);
  int pos = strlen(tmp);
  penguin_motor_buffered_write(pm, tmp, pos);
}

void penguin_motor_setJogVelocity(const penguin_motor_t* pm, int value)
{
  penguin_motor_setParameter(pm, 30, value);
}

void penguin_motor_setGain(const penguin_motor_t* pm, int value)
{
  penguin_motor_setParameter(pm,  4, value);
}

void penguin_motor_setPositionLoopConstant(const penguin_motor_t* pm, int value)
{
  penguin_motor_setParameter(pm,  2, value);
}

void penguin_motor_setVelocityLoopConstant(const penguin_motor_t* pm, int value)
{
  penguin_motor_setParameter(pm,  3, value);
}

void penguin_motor_setAbsoluteMode(const penguin_motor_t* pm)
{
  penguin_motor_setParameter(pm, 17, 0);
  printf("penguin_motor_setAbsoluteMode(): PLEASE RESTART SURVO DRIVER TO APPLY THIS CHANGE.\n");
}

void penguin_motor_setIncrementalMode(const penguin_motor_t* pm)
{
  penguin_motor_setParameter(pm,  17, 1);
  printf("penguin_motor_setIncrementalMode(): PLEASE RESTART SURVO DRIVER TO APPLY THIS CHANGE.\n");
}

void penguin_motor_setDynamicBreakOFF(const penguin_motor_t* pm)
{
  penguin_motor_setParameter(pm, 56, 1);
  printf("penguin_motor_setDynamicBreakOFF(): PLEASE CONFIRM THIS WITH MANUAL. DB IS ON WHEN POWERED-ON OR ALERMED. [PR56 SET AS 1]\n");
}

void penguin_motor_setDynamicBreakON(const penguin_motor_t* pm)
{
  penguin_motor_setParameter(pm, 56, 2);
  printf("penguin_motor_setDynamicBreakON(): PLEASE CONFIRM THIS WITH MANUAL. DB IS ON WHEN POWERED-ON OR SURVO-OFF. [PR56 SET AS 2]\n");
}

void penguin_motor_setPulse(const penguin_motor_t* pm, int velocity)
{
  char tmp[BUFF_SIZE];
  sprintf(tmp, "P=%d", velocity);
  int pos = strlen(tmp);
  penguin_motor_buffered_write(pm, tmp, pos);
}

void penguin_motor_setParameter(const penguin_motor_t* pm, int param, int value)
{
  char tmp[BUFF_SIZE];
  sprintf(tmp, "PRW%d=%d", param, value);
  int pos = strlen(tmp);
  penguin_motor_buffered_write(pm, tmp, pos);
}

int penguin_motor_getParameter(const penguin_motor_t* pm, int param){
  char tmp[BUFF_SIZE];
  sprintf(tmp, "PRR%d", param);
  int pos = strlen(tmp);
  return penguin_motor_send_receive_int_value(pm, tmp, pos);
}

int penguin_motor_getDynamicBreakState(const penguin_motor_t* pm){
  return penguin_motor_getParameter(pm, 56);
}

int penguin_motor_getJogVelocity(const penguin_motor_t* pm){
  return penguin_motor_getParameter(pm, 30);
}

int penguin_motor_getGain(const penguin_motor_t* pm){
  return penguin_motor_getParameter(pm, 4);
}

int penguin_motor_getVelocityLoopConstant(const penguin_motor_t* pm){
  return penguin_motor_getParameter(pm, 3);
}

int penguin_motor_getPositionLoopConstant(const penguin_motor_t* pm){
  return penguin_motor_getParameter(pm, 2);
}

int penguin_motor_getAlermHistory(const penguin_motor_t* pm, const unsigned int number){
  if(number>8){
    printf("penguin_motor_getAlermHistory(): ERROR:: Invalid history number %d <-- should be 0-8.",number);
    return -1;
  }
  char tmp[BUFF_SIZE];
  const int len = penguin_motor_send_receive(pm, "ALM", 3, tmp);
  if(len == 0){
    perror("No data received.");
    return -1;
  }
  char reschar[4];
  strncpy(reschar, tmp+number*5, 4);
  const int resint = strtod(reschar, NULL);
  return resint;
}

int penguin_motor_getAlermState(const penguin_motor_t* pm){
  return penguin_motor_getAlermHistory(pm, 0);
}

int penguin_motor_getModeState(const penguin_motor_t* pm){
  char tmp[BUFF_SIZE];
  const int len = penguin_motor_send_receive(pm, "M", 1, tmp);
  if(len == 0){
    perror("No data received.");
    return -1;
  }
  if(strcmp(tmp, "ABS") !=0){
    return 0;
  }
  else if(strcmp(tmp, "INC") !=0){
    return 1;
  }
  else{
    printf("penguin_motor_getModeState(): ERROR:: INVALID MODE RETURNED (%s). PLEASE IMPLEMENET THIS IN THE CODE.",tmp);
    return -1;
  }
}
