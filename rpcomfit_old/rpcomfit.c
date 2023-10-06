#include "rpcomfit.h"

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
  pIoMap->baudrate =              9600;
  pIoMap->data_bit =              0;
  pIoMap->parity_bit =            2;
  pIoMap->stop_bit =              0;
  pIoMap->flow_ctrl =             0;
  pIoMap->xon_char =              0x11;
  pIoMap->xoff_char =             0x13;
  pIoMap->break_bit =             0;
  pIoMap->signal_ctrl =           0x0003;
  pIoMap->mode =                  0;//1;
  pIoMap->tx_wait =               2000;
  pIoMap->sampl_time =            0xffffffff;
  pIoMap->age_time =              1000;//30000;//0;
  pIoMap->ctrl_cmd_retx_intval =  0;//100;
  pIoMap->ctrl_cmd_retx_cnt =     0;//10;
  pIoMap->data_cmd_retx_intval =  0;//100;
  pIoMap->data_cmd_retx_cnt =     0;//10;
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



