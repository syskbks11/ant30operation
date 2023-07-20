#ifndef INCLUDE_GUARD_UUID_22096767_664D_4684_B6CD_88BE761180E2
#define INCLUDE_GUARD_UUID_22096767_664D_4684_B6CD_88BE761180E2
#include <stdio.h>
#include <string.h>

#include "fitprot.h"

#define VERBOSE 1

void sprintf_withhex(char* buf, const char* const data, const unsigned int len) {
  unsigned int i;
  for(i=0; i<len; i++) {
    sprintf(buf+i*2, "%02x", data[i]);
  }
  buf[(len-1)*2] = 0x00;
}

void sprintf_withbinary(char* buf, const unsigned int value, const unsigned int maxbit){
  unsigned int bit = 1 << maxbit;
  unsigned int i;
  for(i=0; i<maxbit; i++) {
    if(value & bit) sprintf(buf+i, "%d", 1);
    else sprintf(buf+i, "%d", 0);
    bit = bit >> 1;
  }
}

void printv(const char* const word){
  /* print function with ending /n                                */
  /* enabled if VERBOSE in fitprot.c is defined as 1              */
  if(!VERBOSE) return;
  printf("%s",word);
  printf("\n");
  return;
}

void printv_withhex(const char* const headword, const char* const word){
  /* print function with [headword] "[word as string]" --> Hex: [word as Hexadecimal] \n */
  /* enabled if VERBOSE in fitprot.c is defined as 1              */
  if(!VERBOSE) return;
  printf("%s \"%s\" --> Hex: ",headword,word);
  unsigned int i;
  for(i=0; i<(unsigned int)(strlen(word)); i++)
    printf("%02x ", word[i]);
  printf("\n");
  //printf(" (size = %d) \n",(unsigned int)(strlen(word)));
  return;
}

void printv_withbinary(const char* const headword, const unsigned int value, const unsigned int maxbit){
  /* print function with [headword] "[value]" --> Binary: [value as Binary] \n */
  /* enabled if VERBOSE in fitprot.c is defined as 1              */
  if(!VERBOSE) return;
  printf("%s %07d --> Binary: ",headword,value);
  unsigned int bit = 1 << maxbit;
  unsigned int i;
  for(i=0; i<maxbit; i++) {
    if(value & bit) printf("1");
    else printf("0");
    bit = bit >> 1;
  }
  printf("\n");
  return;
}

void printv_svhead(const SVHEAD* const pSVHead){
  /* print function for SVHEAD                                    */
  /* enabled if VERBOSE in fitprot.c is defined as 1              */
  if(!VERBOSE) return;
  unsigned int i = 0;
  printf("- SVHEAD -----------------------------------\n");
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
  printf("--------------------------------------------\n");
}

void printv_strmhead(const STRMHEAD* const pSTRMHead){
  /* print function for STRMHEAD                                  */
  /* enabled if VERBOSE in fitprot.c is defined as 1              */
  if(!VERBOSE) return;
  printf("- STRMHEAD ---------------------------------\n");
  printf(" STRM Ver     = 0x%08x\n",pSTRMHead->bSTRMVer);           /* ﾍｯﾀﾞ構造のﾊﾞｰｼﾞｮﾝ */
  printf(" STRM Command = 0x%08x\n",pSTRMHead->bSTRMCommand);       /* ｺﾏﾝﾄﾞ */
  printf(" STRM Handle  = 0x%08x\n",pSTRMHead->bSTRMHandle);        /* ﾊﾝﾄﾞﾙ番号 */
  printf(" STRM Offset  = 0x%08x\n",pSTRMHead->dwSTRMOffset);       /* ｵﾌｾｯﾄ */
  printf(" STRM Size    = 0x%08x\n",pSTRMHead->wSTRMSize);          /* ﾃﾞｰﾀ数 */
  printf(" STRM Resv    = 0x%08x\n",pSTRMHead->wSVHResv);           /* 予約 */
  printf("--------------------------------------------\n");
}

void printv_iomap(const SER_DEVCTL* const pIoMap){
  /* print function for IOMAP /n                                  */
  /* enabled if VERBOSE in fitprot.c is defined as 1              */
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

#endif

