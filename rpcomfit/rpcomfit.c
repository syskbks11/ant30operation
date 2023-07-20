#ifndef INCLUDE_GUARD_UUID_E6E53248_B00E_4ED7_81C4_934605EF139F
#define INCLUDE_GUARD_UUID_E6E53248_B00E_4ED7_81C4_934605EF139F
/*--------------------------------------------------------------------------*/
/*  FactoryIT RP-COM(FIT), FX-DS110-COM,RP-COM(FIT)H                        */
/*  仮想COMﾓｰﾄﾞ ﾌﾟﾛｸﾞﾗﾐﾝｸﾞ ｻﾝﾌﾟﾙ ｿｰｽｺｰﾄﾞ    Ver1.02 (2022-07-14)            */
/*                                                                          */
/*  機能:               FactoryIT機器への送信                               */
/*                                                                          */
/*  作成方法:           Windows の場合                                      */
/*                      Microsoft Visual Studio 6.0 を使用                  */
/*                          cl -c -DWIN32 tx.c                              */
/*                          link tx.obj wsock32.lib                         */
/*                      Linux の場合                                        */
/*                          cc tx.c -o tx                                   */
/*                                                                          */
/*                                                      CONTEC Co.,Ltd.     */
/*--------------------------------------------------------------------------*/

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

#include "rpcomfit.h"

char dammyWord[FIT_WORDSIZE];

void SocketOpen(rpcomfitctrl* pctrl, char* ip_address){
  pctrl->socket = socket( AF_INET, SOCK_DGRAM, 0 );
  // timeout setting
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  if(setsockopt(pctrl->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0) {
    printf("ERROR::setsockopt\n");
  }

  memset( &(pctrl->socketin), 0, sizeof( pctrl->socketin ) );
  pctrl->socketin.sin_family = AF_INET;
  pctrl->socketin.sin_port = htons( 0x5007 );
  pctrl->socketin.sin_addr.s_addr = inet_addr( ip_address );

  memset( pctrl->txbuffer, 0x00, sizeof( BYTE ) * FIT_BUFFSIZE );
  memset( pctrl->rxbuffer, 0x00, sizeof( BYTE ) * FIT_BUFFSIZE );

  pctrl->strmoff = 0;
  pctrl->seqno   = 0;
}

void SocketClose(rpcomfitctrl* pctrl){
  close( pctrl->socket );
}

void FitOpen(rpcomfitctrl* pctrl, DWORD baudrate, WORD data_bit, WORD parity_bit, WORD stop_bit) {

  int deviceid = pctrl->deviceid;
  if(deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return;
  }

  SVHEAD*     pSVHead;
  pSVHead = (SVHEAD*)(pctrl->txbuffer);

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->bSVHCommand    = SV_WRITE | SV_NO_ACK_FLAG;
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (deviceid-1) * SV_VA_IO_CHSPACE;
  pSVHead->wSVHAccessSize = sizeof( SER_DEVCTL );
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->wSVHSeqNo      = (++(pctrl->seqno));
  pSVHead->wSVHReplyId    = (++(pctrl->seqno));
  pSVHead->wSVHStatus     = 0;

  SER_DEVCTL* pIoMap;
#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__) /* 64bit OS */
  pIoMap = (SER_DEVCTL*)( (DWORD64)(pctrl->txbuffer)  + sizeof(SVHEAD) );
#else /* 32bit OS */
  pIoMap = (SER_DEVCTL*)( (DWORD64)(pctrl->txbuffer)    + sizeof(SVHEAD) );
#endif

  /* virtual MAP setup */
  memset( pIoMap, 0x00, sizeof( SER_DEVCTL ) );
  pIoMap->version  =              1;
  pIoMap->ctrl_reg =              SER_CR_START;
  pIoMap->baudrate =              baudrate;
  pIoMap->data_bit =              data_bit;
  pIoMap->parity_bit =            parity_bit;
  pIoMap->stop_bit =              stop_bit;
  pIoMap->flow_ctrl =             0;
  pIoMap->xon_char =              0x11;
  pIoMap->xoff_char =             0x13;
  pIoMap->break_bit =             0;
  pIoMap->signal_ctrl =           0x0003;
  pIoMap->mode =                  0;
  pIoMap->tx_wait =               2000;
  pIoMap->sampl_time =            0xffffffff;
  pIoMap->age_time =              1000;
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

  /* send data */
  sendto(pctrl->socket,
         pctrl->txbuffer,
         (sizeof(SVHEAD)+sizeof(SER_DEVCTL)),
         0,
         (struct sockaddr*)&(pctrl->socketin),
         sizeof( pctrl->socketin )
         );

}

/**
   RP-COM(FIT)/FX-DS110-COM の ｸﾛｰｽﾞ
*/
void FitClose(rpcomfitctrl* pctrl) {

  int deviceid = pctrl->deviceid;
  if(deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return;
  }

  SVHEAD* pSVHead;
  pSVHead  = (SVHEAD*)(pctrl->txbuffer);

#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__)  /* 64bit OS */
  *(DWORD64*)(&pctrl->txbuffer[sizeof(SVHEAD)]) = SER_CR_INIT;
#else /* 32bit OS */
  *(DWORD*)(&pctrl->txbuffer[sizeof(SVHEAD)]) = SER_CR_INIT;
#endif

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->bSVHCommand    = SV_WRITE | SV_NO_ACK_FLAG;
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (deviceid-1) * SV_VA_IO_CHSPACE + SV_VA_SETCTRL_OFFS;
  pSVHead->wSVHAccessSize = SV_VA_SETCTRL_SIZE;
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->wSVHSeqNo      = (++(pctrl->seqno));
  pSVHead->wSVHReplyId    = (++(pctrl->seqno));
  pSVHead->wSVHStatus     = 0;

  /* send data */
  sendto(pctrl->socket,
         pctrl->txbuffer,
         (sizeof(SVHEAD) + sizeof(SV_VA_SETCTRL_SIZE)),
         0,
         (struct sockaddr*)&(pctrl->socketin),
         sizeof( pctrl->socketin )
         );

}

void FitStatus(rpcomfitctrl* pctrl, SER_DEVCTL* recvdata) {

  int deviceid = pctrl->deviceid;
  if(deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return;
  }

  memset( pctrl->txbuffer, 0x00, sizeof( BYTE ) * FIT_BUFFSIZE );

  SVHEAD* pSVHead;
  pSVHead = (SVHEAD*)(pctrl->txbuffer);

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->bSVHCommand    = SV_READ;
  pSVHead->wSVHSeqNo      = (++(pctrl->seqno));
  pSVHead->wSVHReplyId    = (++(pctrl->seqno));
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (deviceid-1) * SV_VA_IO_CHSPACE;
  pSVHead->wSVHAccessSize = sizeof( SER_DEVCTL );

  /* send data */
  sendto(pctrl->socket,
         pctrl->txbuffer,
         (sizeof(SVHEAD)),
         0,
         (struct sockaddr*)&(pctrl->socketin),
         sizeof( pctrl->socketin )
         );

  DataRecv(pctrl,dammyWord);
  memset( recvdata, 0x00, sizeof( SER_DEVCTL ) );
  recvdata = (SER_DEVCTL*)( (DWORD64)(pctrl->rxbuffer) + sizeof(SVHEAD) );
  return;

}

void FitReset(rpcomfitctrl* pctrl) {

  int deviceid = pctrl->deviceid;
  if (deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return;
  }

  memset( pctrl->txbuffer, 0x00, sizeof( BYTE ) * FIT_BUFFSIZE );

  SVHEAD* pSVHead;
  pSVHead = (SVHEAD*)(pctrl->txbuffer);

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->bSVHCommand    = SV_RESET;
  pSVHead->wSVHSeqNo      = (++(pctrl->seqno));
  pSVHead->wSVHReplyId    = (++(pctrl->seqno));
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (deviceid-1) * SV_VA_IO_CHSPACE;
  pSVHead->wSVHAccessSize = sizeof( SER_DEVCTL );

  /* send data */
  sendto(pctrl->socket,
         pctrl->txbuffer,
         (sizeof(SVHEAD)),
         0,
         (struct sockaddr*)&(pctrl->socketin),
         sizeof( pctrl->socketin )
         );

  return;

}

void FitStrmTrap(rpcomfitctrl* pctrl, unsigned int event){

  int deviceid = pctrl->deviceid;
  if(deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return;
  }

  char dataWord[10];
  sprintf(dataWord, "%c%c", event, 0);

  SVHEAD* pSVHead;
  pSVHead = (SVHEAD*)(pctrl->txbuffer);

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->bSVHCommand    = SV_DATA_STREAM;
  pSVHead->dwSVHVAddr     = 0;
  pSVHead->wSVHAccessSize = sizeof(STRMHEAD) + strlen(dataWord);
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->wSVHSeqNo      = (++(pctrl->seqno));
  pSVHead->wSVHReplyId    = (++(pctrl->seqno));
  pSVHead->wSVHStatus     = 0;

  STRMHEAD* pStrmHead;
  BYTE* pTxData;

#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__) || defined(__arm64__)  /* 64bit OS */
  pStrmHead = (STRMHEAD*)( (DWORD64)(pctrl->txbuffer) + sizeof(SVHEAD) );
  pTxData = (BYTE*)( (DWORD64)(pctrl->txbuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
#else /* 32bit OS */
  pStrmHead = (STRMHEAD*)( (DWORD64)(pctrl->txbuffer) + sizeof(SVHEAD) );
  pTxData = (BYTE*)( (DWORD64)(pctrl->txbuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
#endif

  /* stream header */
  memset( pStrmHead, 0x00, sizeof( STRMHEAD ) );
  pStrmHead->bSTRMVer = STRM_VER;
  pStrmHead->bSTRMCommand = STRM_TRAP;
  pStrmHead->bSTRMHandle = deviceid;
  pStrmHead->dwSTRMOffset = pctrl->strmoff;
  pStrmHead->wSTRMSize = strlen(dataWord);

  pctrl->strmoff+=strlen(dataWord);

  /* data */
  sprintf((char*)pTxData, "%s", dataWord);

  /* send data */
  sendto(pctrl->socket,
         pctrl->txbuffer,
         (sizeof(SVHEAD) + sizeof(STRMHEAD) + strlen(dataWord)),
         0,
         (struct sockaddr*)&(pctrl->socketin),
         sizeof( pctrl->socketin )
         );

  return;

}

int DataSendReceive(rpcomfitctrl* pctrl, const char* const dataWord, char* recvWord){
  DataSendVerbose(pctrl,dataWord,1,false);
  return DataRecvVerbose(pctrl,recvWord,false);
}

int DataSendReceiveVerbose(rpcomfitctrl* pctrl, const char* const dataWord, char* recvWord, const unsigned int verbose){
  DataSendVerbose(pctrl,dataWord,1,verbose);
  return DataRecvVerbose(pctrl,recvWord,verbose);
}

int DataSend(rpcomfitctrl* pctrl, const char* const dataWord){
  return DataSendVerbose(pctrl, dataWord, 0, false);
}

int DataSendVerbose(rpcomfitctrl* pctrl, const char* const dataWord, const BYTE ack, const unsigned int verbose){

  int deviceid = pctrl->deviceid;
  if(deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return -1;
  }

  memset(pctrl->txbuffer, 0x00, FIT_BUFFSIZE);

  size_t sendSize;
  //unsigned int i =0;

  sendSize =  sizeof(SVHEAD) + sizeof(STRMHEAD) + strlen(dataWord);

  if(verbose>1) printv(" + SENT CONTENTS");

  SVHEAD*   pTxSVHead;
  pTxSVHead = (SVHEAD*)(pctrl->txbuffer);
  /* FactoryIT protcol header */
  memset( pTxSVHead, 0x00, sizeof( SVHEAD ) );
  pTxSVHead->bSVHCommand    = SV_DATA_STREAM;
  if(ack)
    pTxSVHead->bSVHCommand = pTxSVHead->bSVHCommand | SV_NO_ACK_FLAG;
  pTxSVHead->dwSVHVAddr     = 0;
  pTxSVHead->wSVHAccessSize = sizeof(STRMHEAD) + strlen(dataWord);
  pTxSVHead->wSVHId         = SV_ID;
  pTxSVHead->bSVHVer        = SV_VER;
  pTxSVHead->wSVHSeqNo      = (++(pctrl->seqno));
  pTxSVHead->wSVHReplyId    = (++(pctrl->seqno));
  pTxSVHead->wSVHStatus     = 0;

  STRMHEAD* pTxStrmHead;
  BYTE*     pTxData;
#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__) || defined(__arm64__)  /* 64bit OS */
  pTxStrmHead = (STRMHEAD*)( (DWORD64)(pctrl->txbuffer) + sizeof(SVHEAD) );
  pTxData = (BYTE*)( (DWORD64)(pctrl->txbuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
#else /* 32bit OS */
  pTxStrmHead = (STRMHEAD*)( (DWORD64)(pctrl->txbuffer) + sizeof(SVHEAD) );
  pTxData = (BYTE*)( (DWORD64)(pctrl->txbuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
#endif

  sprintf((char*)pTxData, "%s", dataWord);

  /* stream header */
  memset( pTxStrmHead, 0x00, sizeof( STRMHEAD ) );
  pTxStrmHead->bSTRMVer = STRM_VER;
  pTxStrmHead->bSTRMCommand = STRM_SEND;
  pTxStrmHead->bSTRMHandle = deviceid;
  pTxStrmHead->dwSTRMOffset = pctrl->strmoff;
  pTxStrmHead->wSTRMSize = strlen(dataWord);

  pctrl->strmoff += pTxStrmHead->wSTRMSize;

  if(verbose>1) printv("- STREAMDATA -------------------------------");
  if(verbose>1) printv_svhead(pTxSVHead);
  if(verbose>1) printv_strmhead(pTxStrmHead);
  if(verbose) printv_withhex("SEND:: ",dataWord);

  /* send data */
  sendto(pctrl->socket,
         pctrl->txbuffer,
         sendSize,
         0,
         (struct sockaddr*)&(pctrl->socketin),
         sizeof( pctrl->socketin )
         );

  return 0;

}

int DataRecv(rpcomfitctrl* pctrl, char* recvWord){
  return DataRecvVerbose(pctrl, recvWord, false);
}

int DataRecvVerbose(rpcomfitctrl* pctrl, char* recvWord, const unsigned int verbose){

  memset(pctrl->rxbuffer, 0x00, FIT_BUFFSIZE);
  memset(recvWord, 0x00, strlen(recvWord));
  SVHEAD*   pRxSVHead;
  STRMHEAD* pRxStrmHead;
  BYTE*     pRxData;
  SER_DEVCTL* pRxIoMap;
  size_t wordSize;

  if(verbose>1) printv(" + RECV CONTENTS");

  int n = 0;
  unsigned int len = 0;
  while(1) {
    n = recvfrom(pctrl->socket, pctrl->rxbuffer, sizeof( pctrl->rxbuffer ), 0, (struct sockaddr*)&(pctrl->socketin), &len );
    if(n < 0) {
      if(errno == EINTR) {
        continue;
      }
      if(errno == EAGAIN) {
        printf("ERROR:: DataRecv Timeout: Empty recvWord will be returned. \n");
        return -2;
      }
      else {
        printf("ERROR:: Invalid Received Data: Empty recvWord will be returned. \n");
        return -3;
      }
    }
    break;
  }

  pRxSVHead = (SVHEAD*)(pctrl->rxbuffer);

#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__)   /* 64bit OS */
  if((pRxSVHead->bSVHCommand & SV_COMMAND_MASK) == SV_DATA_STREAM){
    pRxStrmHead = (STRMHEAD*)( (DWORD64)(pctrl->rxbuffer) + sizeof(SVHEAD) );
    pRxData = (BYTE*)( (DWORD64)(pctrl->rxbuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
    if(verbose>1) printv("- STREAMDATA -------------------------------");
    if(verbose>1) printv_svhead(pRxSVHead);
    if(verbose>1) printv_strmhead(pRxStrmHead);
    wordSize = pRxStrmHead->wSTRMSize;
  }
  else{
    pRxIoMap = (SER_DEVCTL*)( (DWORD64)(pctrl->rxbuffer) + sizeof(SVHEAD) );
    pRxData = (BYTE*)( (DWORD64)(pctrl->rxbuffer) + sizeof(SVHEAD) + sizeof(SER_DEVCTL));
    if(verbose>1) printv("- FITCOMDATA -------------------------------");
    if(verbose>1) printv_svhead(pRxSVHead);
    if(verbose>1) printv_iomap(pRxIoMap);
    wordSize = 0;
  }
#else /* 32bit OS */
  pRxStrmHead = (STRMHEAD*)( (DWORD64)(pctrl->rxbuffer) + sizeof(SVHEAD) );
  pRxData = (BYTE*)( (DWORD64)(pctrl->rxbuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
#endif

  if(wordSize>0){
    sprintf(recvWord,(char*)pRxData,pRxStrmHead->wSTRMSize);
    *(recvWord + pRxStrmHead->wSTRMSize) = 0x00;
    if(verbose) printv_withhex("RECV:: ",recvWord);
  }

  return 0;
}


#endif

