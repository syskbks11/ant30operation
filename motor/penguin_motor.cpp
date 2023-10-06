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
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_addr

#include "penguin_motor.h"
#include "fitprot.h"

#define IP_ADDRESS  "192.168.10.3" // for RPCOM device
#define BUFF_SIZE 4096
#define FITBUFF_SIZE 1500
#define HEADER 0x02
#define TRAILER 0x04
#define SEP 0x3b
#define CMD_ERR "ERR"

#define LIMIT 2000000

unsigned int MotorCmdCount;
unsigned int StreamOffset;

struct penguin_motor_s{
  int id;
  int socket;
  struct sockaddr_in socketin;
  int positiveSoftLimit;
  int negativeSoftLimit;
};

#define STRLEN 12
char tmpstr[STRLEN+1];

static void _buffered_write(int fd, char* buf, const unsigned int size);
static int _send_receive(int fd, char* buf, const unsigned int bufSize, char* buffer);
static int _send_receive_int_value(const int fd, char* cmd, const unsigned int cmdSize);
static int _send_receive_hexint_value(const int fd, char* cmd, const unsigned int cmdSize);
static void _fitClose(const penguin_motor_t* pm);
static void _fitOpen(const penguin_motor_t* pm);
static void _fitDataSendReceive(const penguin_motor_t* pm, const char* dataWord, const unsigned int dataSize, char* recvWord);

static void FitStatus(const penguin_motor_t* pm);

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

// initialization for FitProt
penguin_motor_t* penguin_motor_init(const unsigned int devId, int positiveSoftLimit, int negativeSoftLimit){

  /* open device file */
  const int sock = socket( AF_INET, SOCK_DGRAM, 0 );
  if(sock==-1){ // if failed to open
    printf("ERROR:: Invalid socket.");
    return NULL;
  }
  struct timeval tv;
  tv.tv_sec = 3;
  tv.tv_usec = 0;
  if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0) {
    printf("ERROR::setsockopt");
    return NULL;
  }

  printf("RPCOM device %d opened as a penguin_motor.\n", devId);

  penguin_motor_t* pm = (penguin_motor_t*)malloc(sizeof(penguin_motor_t));
  pm->id = devId;
  pm->socket = sock;
  pm->positiveSoftLimit = positiveSoftLimit;
  pm->negativeSoftLimit = negativeSoftLimit;
  MotorCmdCount = 0;
  StreamOffset = 0;
  printf("%s\n", "penguin_motor(ng)_init() returns. ");
  _fitClose(pm);
  _fitOpen(pm);
  //FitStatus(pm);
  penguin_motor_configureInput(pm);
  _fitClose(pm);
  return pm;

}

int penguin_motor_end(penguin_motor_t* pm){
  _fitClose(pm);
  //if(close(pm->id))
  //perror("penguin_motor_end():: ERROR DURING CLOSING PROCESS: close(pm->id)");
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
  // ----------------------------------------------------
  param = 0x7;
  printf("==============================================\n");
  penguin_motor_setParameter(pm, 50, param);
  printf("==============================================\n");
  if(penguin_motor_getParameter(pm, 50) == param) res += 3;
  // ----------------------------------------------------
  // PaRameteR 50
  // Settings for nominal state of input pins
  // [00]: NormalOpen / [01]: NormalClose
  // BIT14--BIT0 (check PRR60-63 for definitions)
  // BIT1,2 set as 1 for outrange limit signals
  // ----------------------------------------------------
  //param = -2576223.5;
  //printf("==============================================\n");
  //penguin_motor_setParameter(pm, 24, param);
  //printf("==============================================\n");
  //if(penguin_motor_getParameter(pm, 24) == param) res += 4;
  // ----------------------------------------------------
  // PaRameteR 50
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
  _fitDataSendReceive(pm,buf,size,buffer);
}

void _buffered_write(int id, char* buf, const unsigned int size){
  printf("ERROR:: Not use in this version!\n");
  return;
}

int penguin_motor_send_receive(const penguin_motor_t* pm, char* buf, const unsigned int bufSize, char* buffer){
  _fitDataSendReceive(pm,buf,bufSize,buffer);
  return strlen(buffer);
}

int _send_receive(int id, char* buf, const unsigned int bufSize, char* buffer){
  printf("ERROR:: Not use in this version!\n");
  return -1;
}

int penguin_motor_getPreviousCommand(const penguin_motor_t* pm, char* buffer)
{
  perror("penguin_motor_getPreviousCommand() This commnand is not supported anymore.");
  int len = 0;
  buffer[len] = 0;
  return len;
}

int penguin_motor_send_receive_int_value(const penguin_motor_t* pm, char* cmd, const unsigned int cmdSize){
  char buffer[BUFF_SIZE];
  _fitDataSendReceive(pm,cmd,cmdSize,buffer);
  const int len = strlen(buffer);
  if(len == 0){
    perror("No data received.\n");
    return -1;
  }
  const int resint = strtod(buffer, NULL);
  return resint;
}

int penguin_motor_send_receive_hexint_value(const penguin_motor_t* pm, char* cmd, const unsigned int cmdSize){
  char buffer[BUFF_SIZE];
  _fitDataSendReceive(pm,cmd,cmdSize,buffer);
  const int len = strlen(buffer);
  if(len == 0){
    perror("No data received.\n");
    return -1;
  }
  char hexbuffer[BUFF_SIZE];
  sprintf(hexbuffer,"0x%s",buffer);
  const int resint = strtod(hexbuffer, NULL);
  //printf("HEXINT CONVERT %s, %s, %d",buffer,hexbuffer,resint);
  return resint;
}

int _send_receive_hexint_value(const int fd, char* cmd, const unsigned int cmdSize)
{
  printf("ERROR:: Not use in this version!\n");
  return -1;
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
  if(velocity>pm->positiveSoftLimit)
    sprintf(tmp, "P=%d", pm->positiveSoftLimit);
  else if(velocity<pm->negativeSoftLimit)
    sprintf(tmp, "P=%d", pm->negativeSoftLimit);
  else
    sprintf(tmp, "P=%d", velocity);
  int pos = strlen(tmp);
  penguin_motor_buffered_write(pm, tmp, pos);
}

void penguin_motor_setParameter(const penguin_motor_t* pm, int param, int value)
{
  char tmp[BUFF_SIZE];
  sprintf(tmp, "PRW%d=%d", param, value);
  int pos = strlen(tmp);
  printf("%s, %d\n",tmp,pos);
  penguin_motor_buffered_write(pm, tmp, pos);
}

int penguin_motor_getParameter(const penguin_motor_t* pm, int param){
  char tmp[BUFF_SIZE];
  sprintf(tmp, "PRR%d", param);
  int pos = strlen(tmp);
  printf("%s, %d\n",tmp,pos);
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
    perror("No data received.\n");
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
    perror("No data received.\n");
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

void _fitOpen(const penguin_motor_t* pm) {

  if (pm->id<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",pm->id);
    return;
  }

  PSVHEAD     pSVHead;
  PSER_DEVCTL pIoMap;
  size_t sendSize;
  BYTE  txBuffer[FITBUFF_SIZE];

  pSVHead = (PSVHEAD)(txBuffer);
#if defined( _M_AMD64) || defined(__amd64__) /* 64bit OS */
  pIoMap = (PSER_DEVCTL)( (DWORD64)(txBuffer)  + sizeof(SVHEAD) );
  sendSize = ( (DWORD64)sizeof(SVHEAD) + (DWORD64)sizeof(SER_DEVCTL) );
#else /* 32bit OS */
  pIoMap = (PSER_DEVCTL)( (DWORD64)(txBuffer)    + sizeof(SVHEAD) );
  sendSize = ( (DWORD)sizeof(SVHEAD) + (DWORD64)sizeof(SER_DEVCTL) );
#endif

  /* virtual MAP setup */
  memset( pIoMap, 0x00, sizeof( SER_DEVCTL ) );
  pIoMap->version  =              1;
  pIoMap->ctrl_reg =              SER_CR_START;
  pIoMap->baudrate =              9600;
  pIoMap->data_bit =              0;
  pIoMap->parity_bit =            2;
  pIoMap->stop_bit =              0;
  pIoMap->flow_ctrl =             0;
  pIoMap->xon_char =              0x11;
  pIoMap->xoff_char =             0x13;
  pIoMap->break_bit =             0;
  pIoMap->signal_ctrl =           0x0003;
  pIoMap->mode =                  1;
  pIoMap->tx_wait =               2000;
  pIoMap->sampl_time =            0xffffffff;
  pIoMap->age_time =              0;
  pIoMap->ctrl_cmd_retx_intval =  0;
  pIoMap->ctrl_cmd_retx_cnt =     0;
  pIoMap->data_cmd_retx_intval =  0;
  pIoMap->data_cmd_retx_cnt =     0;
  pIoMap->host_tx_resum =         55;
  pIoMap->host_tx_pause =         45;
  pIoMap->rs232c_rx_resum =       55;
  pIoMap->re232c_rx_pause =       50;
  pIoMap->status_reg      =       0x090d07;
  pIoMap->handle =                0x0002;

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->bSVHCommand    = SV_WRITE | SV_NO_ACK_FLAG;
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (pm->id-1) * SV_VA_IO_CHSPACE;
  pSVHead->wSVHAccessSize = sizeof( SER_DEVCTL );
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->wSVHSeqNo      = (++(MotorCmdCount));
  pSVHead->wSVHReplyId    = (++(MotorCmdCount));
  pSVHead->wSVHStatus     = 0;

  /* send data */
  struct sockaddr_in socketin;
  memset( &socketin, 0, sizeof( socketin ) );
  socketin.sin_family = AF_INET;
  socketin.sin_port = htons( 0x5007 );
  socketin.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  sendto(pm->socket,
         txBuffer,
         sendSize,
         0,
         (struct sockaddr*)&(socketin),
         sizeof( socketin )
         );

}

void _fitClose(const penguin_motor_t* pm) {

  if (pm->id<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",pm->id);
    return;
  }

  PSVHEAD pSVHead;
  size_t sendSize;
  BYTE  txBuffer[FITBUFF_SIZE];

  /* close device */
#if defined( _M_AMD64) || defined(__amd64__)  /* 64bit OS */
  *(DWORD64*)(&txBuffer[sizeof(SVHEAD)]) = SER_CR_INIT;
#else /* 32bit OS */
  *(DWORD*)(&txBuffer[sizeof(SVHEAD)]) = SER_CR_INIT;
#endif

  sendSize = (sizeof(SVHEAD) + sizeof(SV_VA_SETCTRL_SIZE));
  pSVHead  = (PSVHEAD)(txBuffer);

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->bSVHCommand    = SV_WRITE | SV_NO_ACK_FLAG;
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (pm->id -1) * SV_VA_IO_CHSPACE + SV_VA_SETCTRL_OFFS;
  pSVHead->wSVHAccessSize = SV_VA_SETCTRL_SIZE;
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->wSVHSeqNo      = (++(MotorCmdCount));
  pSVHead->wSVHReplyId    = (++(MotorCmdCount));
  pSVHead->wSVHStatus     = 0;

  /* send data */
  struct sockaddr_in socketin;
  memset( &socketin, 0, sizeof( socketin ) );
  socketin.sin_family = AF_INET;
  socketin.sin_port = htons( 0x5007 );
  socketin.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  sendto(pm->socket,
         txBuffer,
         sendSize,
         0,
         (struct sockaddr*)&(socketin),
         sizeof( socketin )
         );

}

void _fitDataSendReceive(const penguin_motor_t* pm, const char* dataWord, const unsigned int dataSize, char* recvWord){

  _fitClose(pm);
  _fitOpen(pm);

  PSVHEAD     pTxSVHead;
  PSVHEAD     pRxSVHead;
  PSTRMHEAD   pTxStrmHead;
  PSTRMHEAD   pRxStrmHead;
  BYTE*       pTxData;
  BYTE*       pRxData;

  size_t sendSize;
  unsigned int i =0;

  BYTE  txBuffer[FITBUFF_SIZE];
  BYTE  rxBuffer[FITBUFF_SIZE];

  sendSize =  sizeof(SVHEAD) + sizeof(STRMHEAD) + dataSize+2;
  pTxSVHead = (PSVHEAD)(txBuffer);
#if defined( _M_AMD64) || defined(__amd64__)   /* 64bit OS */
  pTxData = (BYTE*)( (DWORD64)(txBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
  pTxStrmHead = (PSTRMHEAD)( (DWORD64)(txBuffer) + sizeof(SVHEAD) );
#else /* 32bit OS */
  pTxData = (BYTE*)( (DWORD64)(txBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
  pTxStrmHead = (PSTRMHEAD)( (DWORD64)(txBuffer) + sizeof(SVHEAD) );
#endif

  sprintf((char*)pTxData, "%c%s%c", HEADER,dataWord,TRAILER);

  /* stream header */
  memset( pTxStrmHead, 0x00, sizeof( STRMHEAD ) );
  pTxStrmHead->bSTRMVer = STRM_VER;
  pTxStrmHead->bSTRMCommand = STRM_SEND;
  pTxStrmHead->bSTRMHandle = pm->id;
  pTxStrmHead->dwSTRMOffset = StreamOffset;
  pTxStrmHead->wSTRMSize = dataSize+2;

  StreamOffset += dataSize+2;

  /* FactoryIT protcol header */
  memset( pTxSVHead, 0x00, sizeof( SVHEAD ) );
  pTxSVHead->bSVHCommand    = SV_DATA_STREAM | SV_NO_ACK_FLAG;
  pTxSVHead->dwSVHVAddr     = 0;
  pTxSVHead->wSVHAccessSize = sizeof(STRMHEAD) + dataSize+2;
  pTxSVHead->wSVHId         = SV_ID;
  pTxSVHead->bSVHVer        = SV_VER;
  pTxSVHead->wSVHSeqNo      = (++(MotorCmdCount));
  pTxSVHead->wSVHReplyId    = (++(MotorCmdCount));
  pTxSVHead->wSVHStatus     = 0;

  //print_svhead(pTxSVHead);
  //print_strmhead(pTxStrmHead);
  //print_iomap(pTxIoMap);

  /* send data */
  struct sockaddr_in socketin;
  memset( &socketin, 0, sizeof( socketin ) );
  socketin.sin_family = AF_INET;
  socketin.sin_port = htons( 0x5007 );
  socketin.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  sendto(pm->socket,
         txBuffer,
         sendSize,
         0,
         (struct sockaddr*)&(socketin),
         sizeof( socketin )
         );

  //sleep(0.1);
  int n = 0;
  unsigned int len = 0;
  printf("SEND:: \"%s\" --> ",(char*)pTxData);
  while(1) {
    n = recvfrom(pm->socket, rxBuffer, sizeof( rxBuffer ), 0, (struct sockaddr*)&(pm->socketin), &len );
    if(n < 0) {
      if(errno == EINTR) {
        continue;
      }
      if(errno == EAGAIN) {
        printf("Hex: ");
        for(i=0; i<dataSize+2; i++)
          printf("%02x ", pTxData[i]);
        printf(" (size = %d) \n",dataSize+2);
        printf("ERROR::Timeout\n");
        return;
      }
      else {
        printf("Hex: ");
        for(i=0; i<dataSize+2; i++)
          printf("%02x ", pTxData[i]);
        printf(" (size = %d) \n",dataSize+2);
        printf("ERROR:: Invalid Received Data... \n");
      }
    }
    break;
  }

  pRxSVHead = (PSVHEAD)(rxBuffer);
#if defined( _M_AMD64) || defined(__amd64__)   /* 64bit OS */
  pRxStrmHead = (PSTRMHEAD)( (DWORD64)(rxBuffer) + sizeof(SVHEAD) );
  pRxData = (BYTE*)( (DWORD64)(rxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
#else /* 32bit OS */
  pRxStrmHead = (PSTRMHEAD)( (DWORD64)(rxBuffer) + sizeof(SVHEAD) );
  pRxData = (BYTE*)( (DWORD64)(rxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
#endif

  if((pTxSVHead->bSVHCommand & SV_COMMAND_MASK) != (pRxSVHead->bSVHCommand & SV_COMMAND_MASK ))
    printf("ERROR:: Invalid SVHCommand: Tx: %d <--> Rx: %d\n",
           (pTxSVHead->bSVHCommand & SV_COMMAND_MASK),(pRxSVHead->bSVHCommand & SV_COMMAND_MASK));
  if(pTxSVHead->wSVHId != pRxSVHead->wSVHId)
    printf("ERROR:: Invalid SVHId: Tx: %d <--> Rx: %d\n",
           pTxSVHead->wSVHId, pRxSVHead->wSVHId);
  if(pTxSVHead->bSVHVer != pRxSVHead->bSVHVer)
    printf("ERROR:: Invalid SVHVer: Tx: %d <--> Rx: %d\n",
           pTxSVHead->bSVHVer, pRxSVHead->bSVHVer);

  if(pTxStrmHead->bSTRMVer != pRxStrmHead->bSTRMVer )
    printf("ERROR:: Invalid STRMVer: Tx: %d <--> Rx: %d\n",
           pTxStrmHead->bSTRMVer, pRxStrmHead->bSTRMVer);
  if((pTxStrmHead->bSTRMCommand & STRM_COMMAND_MASK) != (pRxStrmHead->bSTRMCommand & STRM_COMMAND_MASK))
    printf("ERROR:: Invalid STRMCommand: Tx: %d <--> Rx: %d\n",
           (pTxStrmHead->bSTRMCommand & STRM_COMMAND_MASK), (pRxStrmHead->bSTRMCommand & STRM_COMMAND_MASK));

  sprintf(recvWord,(char*)pRxData,pRxStrmHead->wSTRMSize);
  printf("RECV:: \"%s\" --> Hex: ",recvWord);
  for(i=0; i<(int)(strlen(recvWord)); i++)
    printf("%02x ", recvWord[i]);
  printf(" (size = %d) \n",(int)(strlen(recvWord)));

  sprintf(recvWord,(char*)pRxData+1,3);
  if(recvWord==CMD_ERR){
    sprintf(recvWord,(char*)pRxData+5,2);
    int errcode = atoi(recvWord);
    if(errcode==0x01) printf("ERROR:: Error code 0x01 returned: [invalid command]\n");
    if(errcode==0x03) printf("ERROR:: Error code 0x03 returned: [input value is outrange]\n");
    if(errcode==0x04) printf("ERROR:: Error code 0x04 returned: [option not selected]\n");
    if(errcode==0x06) printf("ERROR:: Error code 0x06 returned: [reset failed]\n");
    if(errcode==0x07) printf("ERROR:: Error code 0x07 returned: [failed survo-on]\n");
    if(errcode==0x0b) printf("ERROR:: Error code 0x0B returned: [invalid data length]\n");
    if(errcode==0x0d) printf("ERROR:: Error code 0x0D returned: [failed command execution]\n");
    recvWord[0] = 0x00;
    return;
  }

  sprintf(recvWord,(char*)pRxData,pRxStrmHead->wSTRMSize);
  *(recvWord + pRxStrmHead->wSTRMSize) = 0x00;

  if( *(recvWord + pRxStrmHead->wSTRMSize-1) != TRAILER ) {
    printf("ERROR:: Invalid Trailer: 0x%02x (should be 0x%02x)\n", *(recvWord + pRxStrmHead->wSTRMSize-1), TRAILER);
  }
  if( *(recvWord) != HEADER ) {
    printf("ERROR:: Invalid Header: 0x%02x (should be 0x%02x)\n", *(recvWord), HEADER);
  }

  int retlen = pRxStrmHead->wSTRMSize -3 -dataSize; // RXDATA - (HEADER,TRAILER,COMMAND,";")
  if(retlen>0){
    strncpy(recvWord, (char*)pRxData+2+dataSize, retlen); // remove HEADER,CMD,SEP,TRAILER
    recvWord[retlen] = 0x00;
  }
  else{
    //printf("ERROR:: Data recognition failed. \"%s\" Empty data returned.\n",recvWord);
    retlen = 0;
    //strncpy(recvWord, (char*)pRxData+2+dataSize, retlen); // remove HEADER,CMD,SEP,TRAILER
    recvWord[retlen] = 0x00;
  }
  _fitClose(pm);
  //sleep(0.5);
}

///////////////

void FitStatus(const penguin_motor_t* pm) {

  int deviceid = pm->id;
  if (deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return;
  }

  _fitClose(pm);
  _fitOpen(pm);

  PSVHEAD     pSVHead;
  size_t sendSize;
  BYTE  txBuffer[FITBUFF_SIZE];

  pSVHead = (PSVHEAD)(txBuffer);
  sendSize = ( (DWORD64)sizeof(SVHEAD) );

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->bSVHCommand    = SV_READ;
  pSVHead->wSVHSeqNo      = (++(MotorCmdCount));
  pSVHead->wSVHReplyId    = (++(MotorCmdCount));
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (deviceid-1) * SV_VA_IO_CHSPACE;
  pSVHead->wSVHAccessSize = sizeof( SER_DEVCTL );

  /* send data */
  //memset( (char*)&SocketIn, 0, sizeof( (char*)&SocketIn ) );
  // memset( &pm->socketin, 0, sizeof( pm->socketin ) );
  // pm->socketin.sin_family = AF_INET;
  // pm->socketin.sin_port = htons( 0x5007 );
  // pm->socketin.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  struct sockaddr_in socketin;
  memset( &socketin, 0, sizeof( socketin ) );
  socketin.sin_family = AF_INET;
  socketin.sin_port = htons( 0x5007 );
  socketin.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  sendto(pm->socket,
         txBuffer,
         sendSize,
         0,
         (struct sockaddr*)&(socketin),
         sizeof( socketin )
         );

  PSTRMHEAD   pStrmHead;
  BYTE*       pRxData;
  PSER_DEVCTL pIoMap;
  unsigned int len;
  DWORD rcvTimeout = 1L;    /* [ms] */
  BYTE rxBuffer[FITBUFF_SIZE];

  WORD dataSize = 0;
  int n = 0;
  while(1) {
    n = recvfrom(pm->socket, rxBuffer, sizeof( rxBuffer ), 0, (struct sockaddr*)&(pm->socketin), &len );
    if(n < 0) {
      if(errno == EINTR) {
        continue;
      }
      if(errno == EAGAIN) {
        printf("ERROR::Timeout\n");
        return;
      }
      else {
        printf("ERROR:: Invalid Received Data... \n");
      }
    }
    break;
  }

  pSVHead = (PSVHEAD)(rxBuffer);
#if defined( _M_AMD64) || defined(__amd64__)   /* 64bit OS */
  //pStrmHead = (PSTRMHEAD)( (DWORD64)(rxBuffer) + sizeof(SVHEAD) );
  //pIoMap = (PSER_DEVCTL)( (DWORD64)(rxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
  pIoMap = (PSER_DEVCTL)( (DWORD64)(rxBuffer) + sizeof(SVHEAD) );
#else /* 32bit OS */
  //pStrmHead = (PSTRMHEAD)( (DWORD64)(rxBuffer) + sizeof(SVHEAD) );
  //pIoMap = (PSER_DEVCTL)( (DWORD64)(rxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
  pIoMap = (PSER_DEVCTL)( (DWORD64)(rxBuffer) + sizeof(SVHEAD) );
#endif

  //print_svhead(pSVHead);
  //print_strmhead(pStrmHead);
  //print_iomap(pIoMap);
  _fitClose(pm);
}

