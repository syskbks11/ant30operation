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

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned int  DWORD;
typedef unsigned long DWORD64;
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

#define IP_ADDRESS  "192.168.10.3"
#define HEADER 0x00
#define TRAILER 0x00
#define VERBOSE 1
#define BUFFERSIZE 1500
#define WORDSIZE 100

void DataSendReceive(int deviceid, char* dataWord, char* recvWord);
void _DataSend(int deviceid, char* dataWord, const BYTE ack);
void DataSend(int deviceid, char* dataWord);
void DataRecv(int deviceid, char* recvWord);
void FitOpen(int deviceid);
void FitStatus(int deviceid);
void FitReset(int deviceid);
void FitClose(int deviceid);
void FitStrmTrap(int deviceid, int event);

struct sockaddr_in SocketIn;
int Socket = 0;

BYTE  TxBuffer[BUFFERSIZE];
BYTE  RxBuffer[BUFFERSIZE];
WORD  SeqNo    = 0;
WORD  ReplyId  = 0;
DWORD StrmOffs = 0;

char recvDummyWord[WORDSIZE];

void printv(const char* word){
  if(!VERBOSE) return;
  printf("%s",word);
  printf("\n");
  return;
}

void printv_withhex(const char* headword, const char* word){
  //if(!VERBOSE) return;
  printf("%s \"%s\" --> Hex: ",headword,word);
  for(unsigned int i=0; i<(int)(strlen(word)); i++)
    printf("%02x ", word[i]);
  printf(" (size = %d) \n",(int)(strlen(word)));
  return;
}

void printv_svhead(PSVHEAD pSVHead){
  if(!VERBOSE) return;
  unsigned int i = 0;
  printf("- SVHEAD -------------------------\n");
  printf(" SVH Id            = 0x%08x\n",pSVHead->wSVHId);             /* 識別子 */
  printf(" SVH Ver           = 0x%08x\n",pSVHead->bSVHVer);            /* ﾍｯﾀﾞ構造のﾊﾞｰｼﾞｮﾝ */
  printf(" SVH Command       = 0x%08x\n",pSVHead->bSVHCommand);        /* ｺﾏﾝﾄﾞ */
  printf(" SVH SeqNo         = 0x%08x\n",pSVHead->wSVHSeqNo);          /* ｼｰｹﾝｽ番号 */
  printf(" SVH ReplyId       = 0x%08x\n",pSVHead->wSVHReplyId);        /* 応答識別子 */
  printf(" SVH VAddr         = 0x%08x\n",pSVHead->dwSVHVAddr);         /* 仮想ｱﾄﾞﾚｽ */
  printf(" SVH AccessSize    = 0x%08x\n",pSVHead->wSVHAccessSize);     /* ｱｸｾｽｻｲｽﾞ */
  printf(" SVH Status        = 0x%08x\n",pSVHead->wSVHStatus);         /* ｽﾃｰﾀｽ */
  printf(" SVH AccInd        = 0x");
  for(i=0;i<8;i++)
    printf("%01x",pSVHead->bSVHAccInd[i]);      /* ｱｸｾｽ識別子 */
  printf("\n");
  printf(" SVH RemMacAddr    = 0x00");
  for(i=0;i<6;i++)
    printf("%01x",pSVHead->bSVHRemMacAddr[i]);  /* 相手のMACｱﾄﾞﾚｽ */
  printf("\n");
  printf(" SVH Resv          = 0x00");
  for(i=0;i<6;i++)
    printf("%01x",pSVHead->bSVHResv[i]);  /* 予約 */
  printf("\n");
  printf("----------------------------------\n");
}

void printv_strmhead(PSTRMHEAD pSTRMHead){
  if(!VERBOSE) return;
  printf("- STRMHEAD -----------------------\n");
  printf(" STRM Ver     = 0x%08x\n",pSTRMHead->bSTRMVer);           /* ﾍｯﾀﾞ構造のﾊﾞｰｼﾞｮﾝ */
  printf(" STRM Command = 0x%08x\n",pSTRMHead->bSTRMCommand);       /* ｺﾏﾝﾄﾞ */
  printf(" STRM Handle  = 0x%08x\n",pSTRMHead->bSTRMHandle);        /* ﾊﾝﾄﾞﾙ番号 */
  printf(" STRM Offset  = 0x%08x\n",pSTRMHead->dwSTRMOffset);       /* ｵﾌｾｯﾄ */
  printf(" STRM Size    = 0x%08x\n",pSTRMHead->wSTRMSize);          /* ﾃﾞｰﾀ数 */
  printf(" STRM Resv    = 0x%08x\n",pSTRMHead->wSVHResv);           /* 予約 */
  printf("----------------------------------\n");
}

void printv_iomap(PSER_DEVCTL pIoMap){
  if(!VERBOSE) return;
  printf("- SER_DEVCTL -------------------------------\n");
  printf(" IOMAP version              = 0x%08x\n",pIoMap->version);                /*   ﾊﾞｰｼﾞｮﾝ番号 (R) */
  printf(" IOMAP resv0[ 1 ]           \n");
  printf(" IOMAP name                 = 0x");                                      /*   ﾃﾞﾊﾞｲｽ名    (R) */
  unsigned int i = 0;
  for(i=0;i<4;i++)
    printf("%02x",pIoMap->name[i]);
  printf("\n");
  printf(" IOMAP ctrl_reg             = 0x%08x\n",pIoMap->ctrl_reg);               /*   制御ﾚｼﾞｽﾀ   (R/W) */
  printf(" IOMAP baudrate             = 0x%08x\n",pIoMap->baudrate);               /*   ﾎﾞｰﾚｰﾄ      (R/W) */
  printf(" IOMAP data_bit             = 0x%08x\n",pIoMap->data_bit);               /*   ﾃﾞｰﾀﾋﾞｯﾄ    (R/W) */
  printf(" IOMAP parity_bit           = 0x%08x\n",pIoMap->parity_bit);             /*   ﾊﾟﾘﾃｨﾋﾞｯﾄ   (R/W) */
  printf(" IOMAP stop_bit             = 0x%08x\n",pIoMap->stop_bit);               /*   ｽﾄｯﾌﾟﾋﾞｯﾄ   (R/W) */
  printf(" IOMAP flow_ctrl            = 0x%08x\n",pIoMap->flow_ctrl);              /*   ﾌﾛｰ制御     (R/W) */
  printf(" IOMAP xon_char             = 0x%08x\n",pIoMap->xon_char);               /*   XON文字     (R/W) */
  printf(" IOMAP xoff_char            = 0x%08x\n",pIoMap->xoff_char);              /*   XOFF文字    (R/W) */
  printf(" IOMAP break_bit            = 0x%08x\n",pIoMap->break_bit);              /*   ﾌﾞﾚｰｸﾋﾞｯﾄ   (R/W) */
  printf(" IOMAP signal_ctrl          = 0x%08x\n",pIoMap->signal_ctrl);            /*   信号制御    (R/W) */
  printf(" IOMAP model_ctrl           = 0x%08x\n",pIoMap->mode);                   /*   動作ﾓｰﾄﾞ    (R/W) */
  printf(" IOMAP tx_wait              = 0x%08x\n",pIoMap->tx_wait);                /*   送信待ち時間(R/W) */
  printf(" IOMAP sampl_time           = 0x%08x\n",pIoMap->sampl_time);             /*   ｽﾃｰﾀｽ変化のｻﾝﾌﾟﾘﾝｸﾞ時間 (R/W) */
  printf(" IOMAP age_time             = 0x%08x\n",pIoMap->age_time);               /*   生存時間    (R/W) */
  printf(" IOMAP ctrl_cmd_retx_intval = 0x%08x\n",pIoMap->ctrl_cmd_retx_intval);   /*   再送間隔    (R/W) */
  printf(" IOMAP ctrl_cmd_retx_cnt    = 0x%08x\n",pIoMap->ctrl_cmd_retx_cnt);      /*   再送回数    (R/W) */
  printf(" IOMAP resv1[ 1 ]           \n");
  printf(" IOMAP data_cmd_retx_intval = 0x%08x\n",pIoMap->data_cmd_retx_intval);   /*   再送間隔    (R/W) */
  printf(" IOMAP data_cmd_retx_cnt    = 0x%08x\n",pIoMap->data_cmd_retx_cnt);      /*   再送回数    (R/W) */
  printf(" IOMAP resv2[ 1 ]           \n");
  printf(" IOMAP ip_addr              = 0x%08x\n",pIoMap->ip_addr);                /*   IPｱﾄﾞﾚｽ     (R/W) */
  printf(" IOMAP port_no              = 0x%08x\n",pIoMap->port_no);                /*   ﾎﾟｰﾄ番号    (R/W) */
  printf(" IOMAP resv3[ 1 ]           \n");
  printf(" IOMAP stm_ip_addr          = 0x%08x\n",pIoMap->stm_ip_addr);            /*   IPｱﾄﾞﾚｽ     (R/W) */
  printf(" IOMAP stm_port_no          = 0x%08x\n",pIoMap->stm_port_no);            /*   ﾎﾟｰﾄ番号    (R/W) */
  printf(" IOMAP resv4[ 1 ]           \n");
  printf(" IOMAP host_tx_resum        = 0x%08x\n",pIoMap->host_tx_resum);          /*   ﾎｽﾄからのﾃﾞｰﾀ送信を再開する割合 (R/W) */
  printf(" IOMAP host_tx_pause        = 0x%08x\n",pIoMap->host_tx_pause);          /*   ﾎｽﾄからのﾃﾞｰﾀ送信を中断する割合 (R/W) */
  printf(" IOMAP rs232c_rx_resum      = 0x%08x\n",pIoMap->rs232c_rx_resum);        /*   RS-232Cからの受信したﾃﾞｰﾀの送信を再開する割合 (R/W) */
  printf(" IOMAP re232c_rx_pause      = 0x%08x\n",pIoMap->re232c_rx_pause);        /*   RS-232Cからの受信したﾃﾞｰﾀの送信を中断する割合 (R/W) */
  printf(" IOMAP resv5[ 24 ]          \n");
  printf(" IOMAP status_reg           = 0x%08x\n",pIoMap->status_reg);             /*   ｽﾃｰﾀｽﾚｼﾞｽﾀ  (R) */
  printf(" IOMAP handle               = 0x%08x\n",pIoMap->handle);                 /*   ﾊﾝﾄﾞﾙ番号   (R) */
  printf(" IOMAP lsr                  = 0x%08x\n",pIoMap->lsr);                    /*   LSRﾚｼﾞｽﾀ    (R) */
  printf(" IOMAP msr                  = 0x%08x\n",pIoMap->msr);                    /*   ﾓﾃﾞﾑｽﾃｰﾀｽ   (R) */
  printf(" IOMAP resv6[ 11 ]          \n");
  printf(" IOMAP parity_errors        = 0x%08x\n",pIoMap->parity_errors);          /*     ﾊﾟﾘﾃｨｴﾗｰ */
  printf(" IOMAP frame_errors         = 0x%08x\n",pIoMap->frame_errors);           /*     ﾌﾚｰﾑｴﾗｰ */
  printf(" IOMAP overrun_errors       = 0x%08x\n",pIoMap->overrun_errors);         /*     ｵｰﾊﾞｰﾗﾝｴﾗｰ */
  printf(" IOMAP resv7[ 26 ]          \n");
  printf("--------------------------------------------\n");
}

int main(int argc, char *argv[]) {
  if(argc!=2){
    printf("Invalid command format. \n --> %s [DEVICE-ID: 1-7] \n",argv[0]);
    return -1;
  }
  unsigned int i = 0;
  char dataWord[WORDSIZE];
  char recvWord[WORDSIZE];
  int devid =  atoi(argv[1]);

  //printv("FitStatus ========");
  //FitStatus(devid);
  /* start socket */
  printv("SocketOpen ========");
  Socket = socket( AF_INET, SOCK_DGRAM, 0 );
  // timeout setting
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  if(setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0) {
    printf("ERROR::setsockopt\n");
    return -1;
  }
  if( Socket != -1 ) {
    /* open RP-COM(FIT)/FX-DS110-COM */
    printv("FitClose =========");
    FitClose(devid);
    printv("FitOpen  =========");
    FitOpen(devid);

    //getchar();
    FitStatus(devid);
    //getchar();
    //FitReset(devid);

    /* set data */
    //if(VERBOSE) printf("++DataRegistration\n");
    // if(HEADER!=0x00 && TRAILER!=0x00)
    // if(HEADER==0x00 && TRAILER!=0x00)
    //   sprintf(dataWord,"%s%c", argv[2],TRAILER);
    // if(HEADER!=0x00 && TRAILER==0x00)
    //   sprintf(dataWord,"%c%s", HEADER,argv[2]);
    // if(HEADER==0x00 && TRAILER==0x00)
    //sprintf(dataWord,"%s ", argv[2]);

    /* send data */
    memset(dataWord, 0x00, strlen(dataWord));
    printv("DataSendReceive ===========");
    for(int a =0; a<1; a++){
      memset(dataWord, 0x00, WORDSIZE);
      sprintf(dataWord,"ABS X -2147483646%c", TRAILER);
      DataSendReceive(devid,dataWord,recvWord);
      sleep(10);
      memset(dataWord, 0x00, WORDSIZE);
      sprintf(dataWord,"ABS X 2147483646%c", TRAILER);
      DataSendReceive(devid,dataWord,recvWord);
      //sleep(1);

      // printv("TRAP PAUSE ==============");
      // FitStrmTrap(devid,STRM_EVENT_PAUSE);
      // sleep(1);
      // printv("FitStatus ========");
      // FitStatus(devid);
      // printv("DATA SENDING 1 ==========");
      // memset(dataWord, 0x00, WORDSIZE);
      // sprintf(dataWord,"%cVF=10%c", HEADER,TRAILER);
      // //sprintf(dataWord,"VF=1 ");
      // _DataSend(devid,dataWord,1);
      // //DataSend(devid,dataWord);
      // //sleep(1);
      // printv("DATA SENDING 2 ==========");
      // memset(dataWord, 0x00, WORDSIZE);
      // sprintf(dataWord,"%cP=0%c", HEADER,TRAILER);
      // //sprintf(dataWord,"P=100000 ");
      // _DataSend(devid,dataWord,1);
      // //DataSend(devid,dataWord);
      // //sleep(1);
      // //printv("DATA SENDING 3 ==========");
      // //memset(dataWord, 0x00, WORDSIZE);
      // //sprintf(dataWord,"%cP=0%c", HEADER,TRAILER);
      // //sprintf(dataWord,"P=0 ");
      // //printv_withhex("SEND:: ",dataWord);
      // //_DataSend(devid,dataWord,1);
      // //DataSend(devid,dataWord);
      // //sleep(1);
      // printv("TRAP RESUME =============");
      // FitStrmTrap(devid,STRM_EVENT_RESUME);
      // sleep(1);
      //printv("FitStatus ========");
      //FitStatus(devid);
    }

    // for(i=0; i<3; i++){
    //   printf("DATA RECVING %d ==========\n",i);
    //   DataRecv(devid,recvWord);
    //   //printv("FitStatus ========");
    //   //FitStatus(devid);
    // }

    /* close RP-COM(FIT)/FX-DS110-COM */
    printv("FitClose =========");
    FitClose(devid);

    /* end socket */
    printv("SocketClose ======");
    close( Socket );
  }
  return 0;
}

void FitStatus(int deviceid) {

  if (deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return;
  }

  PSVHEAD     pSVHead;
  size_t sendSize;

  pSVHead = (PSVHEAD)(TxBuffer);
  sendSize = ( (DWORD64)sizeof(SVHEAD) );

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->bSVHCommand    = SV_READ;
  pSVHead->wSVHSeqNo      = (++SeqNo);
  pSVHead->wSVHReplyId    = (++SeqNo);
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (deviceid-1) * SV_VA_IO_CHSPACE;
  pSVHead->wSVHAccessSize = sizeof( SER_DEVCTL );

  /* send data */
  //memset( (char*)&SocketIn, 0, sizeof( (char*)&SocketIn ) );
  memset( &SocketIn, 0, sizeof( SocketIn ) );
  SocketIn.sin_family = AF_INET;
  SocketIn.sin_port = htons( 0x5007 );
  SocketIn.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  printv(" + SENT CONTENTS");
  printv_svhead(pSVHead);

  sendto(Socket,
         TxBuffer,
         sendSize,
         0,
         (struct sockaddr*)&SocketIn,
         sizeof( SocketIn )
         );

  //return;
  char recvWord[WORDSIZE];
  DataRecv(deviceid,recvWord);
}

void FitReset(int deviceid) {

  if (deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return;
  }

  PSVHEAD     pSVHead;
  size_t sendSize;

  pSVHead = (PSVHEAD)(TxBuffer);
  sendSize = ( (DWORD64)sizeof(SVHEAD) );

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->bSVHCommand    = SV_RESET;
  pSVHead->wSVHSeqNo      = (++SeqNo);
  pSVHead->wSVHReplyId    = (++SeqNo);
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (deviceid-1) * SV_VA_IO_CHSPACE;
  pSVHead->wSVHAccessSize = sizeof( SER_DEVCTL );

  /* send data */
  //memset( (char*)&SocketIn, 0, sizeof( (char*)&SocketIn ) );
  memset( &SocketIn, 0, sizeof( SocketIn ) );
  SocketIn.sin_family = AF_INET;
  SocketIn.sin_port = htons( 0x5007 );
  SocketIn.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  sendto(Socket,
         TxBuffer,
         sendSize,
         0,
         (struct sockaddr*)&SocketIn,
         sizeof( SocketIn )
         );

  PSTRMHEAD   pStrmHead;
  BYTE*       pRxData;
  PSER_DEVCTL pIoMap;
  unsigned int len;
  DWORD rcvTimeout = 1L;    /* [ms] */

  WORD dataSize = 0;
  int n = 0;
  while(1) {
    n = recvfrom(Socket, RxBuffer, sizeof( RxBuffer ), 0, (struct sockaddr*)&SocketIn, &len );
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

  char recvWord[100];
  pSVHead = (PSVHEAD)(RxBuffer);
#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__)   /* 64bit OS */
  //pStrmHead = (PSTRMHEAD)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) );
  //pIoMap = (PSER_DEVCTL)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
  pIoMap = (PSER_DEVCTL)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) );
  pRxData = (BYTE*)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD));
#else /* 32bit OS */
  //pStrmHead = (PSTRMHEAD)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) );
  //pIoMap = (PSER_DEVCTL)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
  pIoMap = (PSER_DEVCTL)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) );
  pRxData = (BYTE*)( (DWORD64)(RxBuffer) + sizeof(SVHEAD)+ sizeof(STRMHEAD) );
#endif

  printf("RESET +++++ \n");
  printv_svhead(pSVHead);
  //printv_strmhead(pStrmHead);
  printv_iomap(pIoMap);

  sprintf(recvWord,(char*)pRxData,100);
  printf("RECV:: \"%s\" \n",recvWord);
}

void FitOpen(int deviceid) {

  if (deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return;
  }

  PSVHEAD     pSVHead;
  PSER_DEVCTL pIoMap;

  size_t sendSize;

  pSVHead = (PSVHEAD)(TxBuffer);
#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__) /* 64bit OS */
  pIoMap = (PSER_DEVCTL)( (DWORD64)(TxBuffer)  + sizeof(SVHEAD) );
  sendSize = ( (DWORD64)sizeof(SVHEAD) + (DWORD64)sizeof(SER_DEVCTL) );
#else /* 32bit OS */
  pIoMap = (PSER_DEVCTL)( (DWORD64)(TxBuffer)    + sizeof(SVHEAD) );
  sendSize = ( (DWORD)sizeof(SVHEAD) + (DWORD64)sizeof(SER_DEVCTL) );
#endif

  /* virtual MAP setup */
  memset( pIoMap, 0x00, sizeof( SER_DEVCTL ) );
  pIoMap->version  =              1;
  pIoMap->ctrl_reg =              SER_CR_START;
  pIoMap->baudrate =              115200;
  pIoMap->data_bit =              0;
  pIoMap->parity_bit =            0;
  pIoMap->stop_bit =              0;
  pIoMap->flow_ctrl =             0;
  pIoMap->xon_char =              0x11;
  pIoMap->xoff_char =             0x13;
  pIoMap->break_bit =             0;
  pIoMap->signal_ctrl =           0x0003;
  pIoMap->mode =                  0;//1;
  pIoMap->tx_wait =               0;//2000;
  pIoMap->sampl_time =            2000;//0xffffffff;
  pIoMap->age_time =              1000;//30000;//0;
  pIoMap->ctrl_cmd_retx_intval =  0;//100;
  pIoMap->ctrl_cmd_retx_cnt =     0;//10;
  pIoMap->data_cmd_retx_intval =  100;
  pIoMap->data_cmd_retx_cnt =     10;
  pIoMap->host_tx_resum =         55;
  pIoMap->host_tx_pause =         45;
  pIoMap->rs232c_rx_resum =       55;
  pIoMap->re232c_rx_pause =       50;
  pIoMap->status_reg      =       0x090d07;
  pIoMap->handle =                0x0002;

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->bSVHCommand    = SV_WRITE | SV_NO_ACK_FLAG;
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (deviceid-1) * SV_VA_IO_CHSPACE;
  pSVHead->wSVHAccessSize = sizeof( SER_DEVCTL );
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->wSVHSeqNo      = (++SeqNo);
  pSVHead->wSVHReplyId    = (++SeqNo);
  pSVHead->wSVHStatus     = 0;

  /* send data */
  memset( &SocketIn, 0, sizeof( SocketIn ) );
  SocketIn.sin_family = AF_INET;
  SocketIn.sin_port = htons( 0x5007 );
  SocketIn.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  sendto(Socket,
         TxBuffer,
         sendSize,
         0,
         (struct sockaddr*)&SocketIn,
         sizeof( SocketIn )
         );

  printv("  + SENT CONTENTS");
  printv_svhead(pSVHead);
  printv_iomap(pIoMap);

  //DataRecv(deviceid,recvDummyWord);

}


/**
   RP-COM(FIT)/FX-DS110-COM の ｸﾛｰｽﾞ
*/
void FitClose(int deviceid) {

  if(deviceid<1){
    printf("ERROR:: Invalid device ID (%d): This should be >=1. \n",deviceid);
    return;
  }

  PSVHEAD pSVHead;
  size_t  sendSize;

  /* close device */
#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__)  /* 64bit OS */
  *(DWORD64*)(&TxBuffer[sizeof(SVHEAD)]) = SER_CR_INIT;
#else /* 32bit OS */
  *(DWORD*)(&TxBuffer[sizeof(SVHEAD)]) = SER_CR_INIT;
#endif

  sendSize = (sizeof(SVHEAD) + sizeof(SV_VA_SETCTRL_SIZE));
  pSVHead  = (PSVHEAD)(TxBuffer);

  /* FactoryIT protcol header */
  memset( pSVHead, 0x00, sizeof( SVHEAD ) );
  pSVHead->bSVHCommand    = SV_WRITE | SV_NO_ACK_FLAG;
  pSVHead->dwSVHVAddr     = SV_VA_IO_CTRL + (deviceid-1) * SV_VA_IO_CHSPACE + SV_VA_SETCTRL_OFFS;
  pSVHead->wSVHAccessSize = SV_VA_SETCTRL_SIZE;
  pSVHead->wSVHId         = SV_ID;
  pSVHead->bSVHVer        = SV_VER;
  pSVHead->wSVHSeqNo      = (++SeqNo);
  pSVHead->wSVHReplyId    = (++SeqNo);
  pSVHead->wSVHStatus     = 0;

  /* send data */
  memset( &SocketIn, 0, sizeof( SocketIn ) );
  SocketIn.sin_family      = AF_INET;
  SocketIn.sin_port        = htons( 0x5007 );
  SocketIn.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  sendto(Socket,
         TxBuffer,
         sendSize,
         0,
         (struct sockaddr*)&SocketIn,
         sizeof( SocketIn )
         );

  printv("  + SENT CONTENTS");
  printv_svhead(pSVHead);

  //DataRecv(deviceid,recvDummyWord);
}

void FitStrmTrap(int deviceid, int event){

  PSVHEAD   pTxSVHead;
  PSTRMHEAD pTxStrmHead;
  BYTE*     pTxData;
  BYTE*     pRxData;

  size_t sendSize;
  unsigned int i =0;

  sendSize =  sizeof(SVHEAD) + sizeof(STRMHEAD) + 2;
  pTxSVHead = (PSVHEAD)(TxBuffer);
#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__) || defined(__arm64__)  /* 64bit OS */
  pTxData = (BYTE*)( (DWORD64)(TxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
  pTxStrmHead = (PSTRMHEAD)( (DWORD64)(TxBuffer) + sizeof(SVHEAD) );
#else /* 32bit OS */
  pTxData = (BYTE*)( (DWORD64)(TxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
  pTxStrmHead = (PSTRMHEAD)( (DWORD64)(TxBuffer) + sizeof(SVHEAD) );
#endif

  sprintf((char*)pTxData, "%c%c", event,0);

  /* stream header */
  memset( pTxStrmHead, 0x00, sizeof( STRMHEAD ) );
  pTxStrmHead->bSTRMVer = STRM_VER;
  pTxStrmHead->bSTRMCommand = STRM_TRAP;
  pTxStrmHead->bSTRMHandle = deviceid;
  pTxStrmHead->dwSTRMOffset = StrmOffs;
  pTxStrmHead->wSTRMSize = 2;

  StrmOffs+= 2;

  /* FactoryIT protcol header */
  memset( pTxSVHead, 0x00, sizeof( SVHEAD ) );
  pTxSVHead->bSVHCommand    = SV_DATA_STREAM;
  pTxSVHead->dwSVHVAddr     = 0;
  pTxSVHead->wSVHAccessSize = sizeof(STRMHEAD) + 2;
  pTxSVHead->wSVHId         = SV_ID;
  pTxSVHead->bSVHVer        = SV_VER;
  pTxSVHead->wSVHSeqNo      = (++SeqNo);
  pTxSVHead->wSVHReplyId    = (++SeqNo);
  pTxSVHead->wSVHStatus     = 0;

  /* send data */
  memset( &SocketIn, 0, sizeof( SocketIn ) );
  SocketIn.sin_family = AF_INET;
  SocketIn.sin_port = htons( 0x5007 );
  SocketIn.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  sendto(Socket,
         TxBuffer,
         sendSize,
         0,
         (struct sockaddr*)&SocketIn,
         sizeof( SocketIn )
         );

  printf("  + SENT CONTENTS\n");
  printv_svhead(pTxSVHead);
  printv_strmhead(pTxStrmHead);

  DataRecv(deviceid,recvDummyWord);

}

void DataSendReceive(int deviceid, char* dataWord, char* recvWord){

  _DataSend(deviceid,dataWord,1);
  DataRecv(deviceid,recvWord);

  // PSVHEAD   pTxSVHead;
  // PSTRMHEAD pTxStrmHead;

  // PSVHEAD   pRxSVHead;
  // PSTRMHEAD pRxStrmHead;

  // if((pTxSVHead->bSVHCommand & SV_COMMAND_MASK) != (pRxSVHead->bSVHCommand & SV_COMMAND_MASK ))
  //   printf("ERROR:: Invalid SVHCommand: Tx: %d <--> Rx: %d\n",
  //          (pTxSVHead->bSVHCommand & SV_COMMAND_MASK),(pRxSVHead->bSVHCommand & SV_COMMAND_MASK));
  // if(pTxSVHead->wSVHId != pRxSVHead->wSVHId)
  //   printf("ERROR:: Invalid SVHId: Tx: %d <--> Rx: %d\n",
  //          pTxSVHead->wSVHId, pRxSVHead->wSVHId);
  // if(pTxSVHead->bSVHVer != pRxSVHead->bSVHVer)
  //   printf("ERROR:: Invalid SVHVer: Tx: %d <--> Rx: %d\n",
  //          pTxSVHead->bSVHVer, pRxSVHead->bSVHVer);

  // if(pTxStrmHead->bSTRMVer != pRxStrmHead->bSTRMVer )
  //   printf("ERROR:: Invalid STRMVer: Tx: %d <--> Rx: %d\n",
  //          pTxStrmHead->bSTRMVer, pRxStrmHead->bSTRMVer);
  // if((pTxStrmHead->bSTRMCommand & STRM_COMMAND_MASK) != (pRxStrmHead->bSTRMCommand & STRM_COMMAND_MASK))
  //   printf("ERROR:: Invalid STRMCommand: Tx: %d <--> Rx: %d\n",
  //          (pTxStrmHead->bSTRMCommand & STRM_COMMAND_MASK), (pRxStrmHead->bSTRMCommand & STRM_COMMAND_MASK));

  // memset(recvWord, 0x00, pRxStrmHead->wSTRMSize+10);
  // sprintf(recvWord,(char*)pRxData,pRxStrmHead->wSTRMSize);
  // *(recvWord + pRxStrmHead->wSTRMSize) = 0x00;

  // if( TRAILER != 0x00 && *(recvWord + pRxStrmHead->wSTRMSize-1) != TRAILER ) {
  //   printf("ERROR:: Invalid Trailer: 0x%02x (should be 0x%02x)\n", *(recvWord + pRxStrmHead->wSTRMSize-1), TRAILER);
  // }
  // if( HEADER != 0x00 && *(recvWord) != HEADER ) {
  //   printf("ERROR:: Invalid Header: 0x%02x (should be 0x%02x)\n", *(recvWord), HEADER);
  // }
}

void DataSend(int deviceid, char* dataWord){
  _DataSend(deviceid, dataWord, 0);
}

void _DataSend(int deviceid, char* dataWord, const BYTE ack){

  memset(TxBuffer, 0x00, BUFFERSIZE);
  PSVHEAD   pTxSVHead;
  PSTRMHEAD pTxStrmHead;
  BYTE*     pTxData;

  size_t sendSize;
  unsigned int i =0;

  sendSize =  sizeof(SVHEAD) + sizeof(STRMHEAD) + strlen(dataWord);
  pTxSVHead = (PSVHEAD)(TxBuffer);
#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__) || defined(__arm64__)  /* 64bit OS */
  pTxData = (BYTE*)( (DWORD64)(TxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
  pTxStrmHead = (PSTRMHEAD)( (DWORD64)(TxBuffer) + sizeof(SVHEAD) );
#else /* 32bit OS */
  pTxData = (BYTE*)( (DWORD64)(TxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
  pTxStrmHead = (PSTRMHEAD)( (DWORD64)(TxBuffer) + sizeof(SVHEAD) );
#endif

  sprintf((char*)pTxData, "%s", dataWord);

  /* stream header */
  memset( pTxStrmHead, 0x00, sizeof( STRMHEAD ) );
  pTxStrmHead->bSTRMVer = STRM_VER;
  pTxStrmHead->bSTRMCommand = STRM_SEND;
  pTxStrmHead->bSTRMHandle = deviceid;
  pTxStrmHead->dwSTRMOffset = StrmOffs;
  pTxStrmHead->wSTRMSize = strlen(dataWord);

  StrmOffs += pTxStrmHead->wSTRMSize;

  /* FactoryIT protcol header */
  memset( pTxSVHead, 0x00, sizeof( SVHEAD ) );
  pTxSVHead->bSVHCommand    = SV_DATA_STREAM;
  if(ack)
    pTxSVHead->bSVHCommand = pTxSVHead->bSVHCommand | SV_NO_ACK_FLAG;
  pTxSVHead->dwSVHVAddr     = 0;
  pTxSVHead->wSVHAccessSize = sizeof(STRMHEAD) + strlen(dataWord);
  pTxSVHead->wSVHId         = SV_ID;
  pTxSVHead->bSVHVer        = SV_VER;
  pTxSVHead->wSVHSeqNo      = (++SeqNo);
  pTxSVHead->wSVHReplyId    = (++SeqNo);
  pTxSVHead->wSVHStatus     = 0;

  /* send data */
  memset( &SocketIn, 0, sizeof( SocketIn ) );
  SocketIn.sin_family = AF_INET;
  SocketIn.sin_port = htons( 0x5007 );
  SocketIn.sin_addr.s_addr = inet_addr( IP_ADDRESS );

  printv(" + SENT CONTENTS");
  printv_svhead(pTxSVHead);
  printv_strmhead(pTxStrmHead);
  printv_withhex("SEND:: ",dataWord);

  sendto(Socket,
         TxBuffer,
         sendSize,
         0,
         (struct sockaddr*)&SocketIn,
         sizeof( SocketIn )
         );

  return;
}

void DataRecv(int deviceid, char* recvWord){

  memset(RxBuffer, 0x00, BUFFERSIZE);
  memset(recvWord, 0x00, WORDSIZE);
  PSVHEAD   pRxSVHead;
  PSTRMHEAD pRxStrmHead;
  BYTE*     pRxData;
  PSER_DEVCTL pRxIoMap;
  size_t wordSize;

  int n = 0;
  unsigned int len = 0;
  while(1) {
    n = recvfrom(Socket, RxBuffer, sizeof( RxBuffer ), 0, (struct sockaddr*)&SocketIn, &len );
    if(n < 0) {
      if(errno == EINTR) {
        continue;
      }
      if(errno == EAGAIN) {
        printf("ERROR::Timeout\n");
        //return;
        //continue;
        break;
      }
      else {
        printf("ERROR:: Invalid Received Data... \n");
      }
    }
    break;
  }

  pRxSVHead = (PSVHEAD)(RxBuffer);

  printv(" + RECV CONTENTS");
#if defined( _M_AMD64) || defined(__amd64__) || defined(__arm64__)   /* 64bit OS */
  if((pRxSVHead->bSVHCommand & SV_COMMAND_MASK) == SV_DATA_STREAM){
    pRxStrmHead = (PSTRMHEAD)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) );
    pRxData = (BYTE*)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
    printv("- STREAMDATA ------");
    printv_svhead(pRxSVHead);
    printv_strmhead(pRxStrmHead);
    wordSize = pRxStrmHead->wSTRMSize;
  }
  else{
    pRxIoMap = (PSER_DEVCTL)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) );
    pRxData = (BYTE*)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) + sizeof(SER_DEVCTL));
    printv("- FITCOMDATA ------");
    printv_svhead(pRxSVHead);
    printv_iomap(pRxIoMap);
    wordSize = 0;
  }
#else /* 32bit OS */
  pRxStrmHead = (PSTRMHEAD)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) );
  pRxData = (BYTE*)( (DWORD64)(RxBuffer) + sizeof(SVHEAD) + sizeof(STRMHEAD) );
#endif

  if(wordSize>0){
    sprintf(recvWord,(char*)pRxData,pRxStrmHead->wSTRMSize);
    *(recvWord + pRxStrmHead->wSTRMSize) = 0x00;
    printv_withhex("RECV:: ",recvWord);
    if( TRAILER != 0x00 && *(recvWord + pRxStrmHead->wSTRMSize-1) != TRAILER ) {
      printf("ERROR:: Invalid Trailer: 0x%02x (should be 0x%02x)\n", *(recvWord + pRxStrmHead->wSTRMSize-1), TRAILER);
    }
    if( HEADER != 0x00 && *(recvWord) != HEADER ) {
      printf("ERROR:: Invalid Header: 0x%02x (should be 0x%02x)\n", *(recvWord), HEADER);
    }
  }

}

