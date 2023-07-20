#ifndef INCLUDE_GUARD_UUID_16CE064E_6FE3_4ADE_8664_8AF5DF9F32FC
#define INCLUDE_GUARD_UUID_16CE064E_6FE3_4ADE_8664_8AF5DF9F32FC
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

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

#include "fitprot.h"

#define FIT_BUFFSIZE 1500
#define FIT_WORDSIZE 100

typedef struct _RPCOMFIT_CTRL {
  BYTE  txbuffer[FIT_BUFFSIZE];
  BYTE  rxbuffer[FIT_BUFFSIZE];
  int     socket;
  struct sockaddr_in socketin;
  int     deviceid;           /*   ﾃﾞﾊﾞｲｽﾊﾞﾝｺﾞｳ      */
  WORD    seqno;              /*   通信番号          */
  unsigned int strmoff;
}rpcomfitctrl;

void SocketOpen(rpcomfitctrl* ctrl, char* ip_address);
void SocketClose(rpcomfitctrl* ctrl);

void FitOpen(rpcomfitctrl* ctrl, DWORD baudrate, WORD data_bit, WORD parity_bit, WORD stop_bit);
void FitClose(rpcomfitctrl* ctrl);
void FitStatus(rpcomfitctrl* ctrl, SER_DEVCTL* recvdata);
void FitReset(rpcomfitctrl* ctrl);
void FitStrmTrap(rpcomfitctrl* ctrl, unsigned int event);

int DataSendReceive(rpcomfitctrl* ctrl, const char* const dataWord, char* recvWord);
int DataSendReceiveVerbose(rpcomfitctrl* ctrl, const char* const dataWord, char* recvWord, const unsigned int verbose);

int DataSend(rpcomfitctrl* ctrl, const char* const dataWord);
int DataSendVerbose(rpcomfitctrl* ctrl, const char* const dataWord, const BYTE ack, const unsigned int verbose);

int DataRecv(rpcomfitctrl* ctrl, char* recvWord);
int DataRecvVerbose(rpcomfitctrl* ctrl, char* recvWord, const unsigned int verbose);

#endif

