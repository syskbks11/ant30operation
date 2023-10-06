#include <stdio.h>

#include "fitprot.h"

void print_svhead(PSVHEAD pSVHead);
void print_strmhead(PSTRMHEAD pSTRMHead);
void print_iomap(PSER_DEVCTL pIoMap);

void print_svhead(PSVHEAD pSVHead){
  int i;
  i = 0;
  printf("= SVHEAD =========================\n");
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
  printf("==================================\n");
}

void print_strmhead(PSTRMHEAD pSTRMHead){
  printf("= STRMHEAD =======================\n");
  printf(" STRM Ver     = 0x%08x\n",pSTRMHead->bSTRMVer);           /* ﾍｯﾀﾞ構造のﾊﾞｰｼﾞｮﾝ */
  printf(" STRM Command = 0x%08x\n",pSTRMHead->bSTRMCommand);       /* ｺﾏﾝﾄﾞ */
  printf(" STRM Handle  = 0x%08x\n",pSTRMHead->bSTRMHandle);        /* ﾊﾝﾄﾞﾙ番号 */
  printf(" STRM Offset  = 0x%08x\n",pSTRMHead->dwSTRMOffset);       /* ｵﾌｾｯﾄ */
  printf(" STRM Size    = 0x%08x\n",pSTRMHead->wSTRMSize);          /* ﾃﾞｰﾀ数 */
  printf(" STRM Resv    = 0x%08x\n",pSTRMHead->wSVHResv);           /* 予約 */
  printf("==================================\n");
}

void print_iomap(PSER_DEVCTL pIoMap){
  int i;
  i = 0;
  printf("= SER_DEVCTL ===============================\n");
  printf(" version              = 0x%08x\n",pIoMap->version);                /*   ﾊﾞｰｼﾞｮﾝ番号 (R) */
  printf(" resv0[ 1 ]           \n");
  printf(" name                 = 0x");                                      /*   ﾃﾞﾊﾞｲｽ名    (R) */
  for(i=0;i<4;i++)
    printf("%02x",pIoMap->name[i]);
  printf("\n");
  printf(" ctrl_reg             = 0x%08x\n",pIoMap->ctrl_reg);               /*   制御ﾚｼﾞｽﾀ   (R/W) */
  printf(" baudrate             = 0x%08x\n",pIoMap->baudrate);               /*   ﾎﾞｰﾚｰﾄ      (R/W) */
  printf(" data_bit             = 0x%08x\n",pIoMap->data_bit);               /*   ﾃﾞｰﾀﾋﾞｯﾄ    (R/W) */
  printf(" parity_bit           = 0x%08x\n",pIoMap->parity_bit);             /*   ﾊﾟﾘﾃｨﾋﾞｯﾄ   (R/W) */
  printf(" stop_bit             = 0x%08x\n",pIoMap->stop_bit);               /*   ｽﾄｯﾌﾟﾋﾞｯﾄ   (R/W) */
  printf(" flow_ctrl            = 0x%08x\n",pIoMap->flow_ctrl);              /*   ﾌﾛｰ制御     (R/W) */
  printf(" xon_char             = 0x%08x\n",pIoMap->xon_char);               /*   XON文字     (R/W) */
  printf(" xoff_char            = 0x%08x\n",pIoMap->xoff_char);              /*   XOFF文字    (R/W) */
  printf(" break_bit            = 0x%08x\n",pIoMap->break_bit);              /*   ﾌﾞﾚｰｸﾋﾞｯﾄ   (R/W) */
  printf(" signal_ctrl          = 0x%08x\n",pIoMap->signal_ctrl);            /*   信号制御    (R/W) */
  printf(" model_ctrl           = 0x%08x\n",pIoMap->mode);                   /*   動作ﾓｰﾄﾞ    (R/W) */
  printf(" tx_wait              = 0x%08x\n",pIoMap->tx_wait);                /*   送信待ち時間(R/W) */
  printf(" sampl_time           = 0x%08x\n",pIoMap->sampl_time);             /*   ｽﾃｰﾀｽ変化のｻﾝﾌﾟﾘﾝｸﾞ時間 (R/W) */
  printf(" age_time             = 0x%08x\n",pIoMap->age_time);               /*   生存時間    (R/W) */
  printf(" ctrl_cmd_retx_intval = 0x%08x\n",pIoMap->ctrl_cmd_retx_intval);   /*   再送間隔    (R/W) */
  printf(" ctrl_cmd_retx_cnt    = 0x%08x\n",pIoMap->ctrl_cmd_retx_cnt);      /*   再送回数    (R/W) */
  printf(" resv1[ 1 ]           \n");
  printf(" data_cmd_retx_intval = 0x%08x\n",pIoMap->data_cmd_retx_intval);   /*   再送間隔    (R/W) */
  printf(" data_cmd_retx_cnt    = 0x%08x\n",pIoMap->data_cmd_retx_cnt);      /*   再送回数    (R/W) */
  printf(" resv2[ 1 ]           \n");
  printf(" ip_addr              = 0x%08x\n",pIoMap->ip_addr);                /*   IPｱﾄﾞﾚｽ     (R/W) */
  printf(" port_no              = 0x%08x\n",pIoMap->port_no);                /*   ﾎﾟｰﾄ番号    (R/W) */
  printf(" resv3[ 1 ]           \n");
  printf(" stm_ip_addr          = 0x%08x\n",pIoMap->stm_ip_addr);            /*   IPｱﾄﾞﾚｽ     (R/W) */
  printf(" stm_port_no          = 0x%08x\n",pIoMap->stm_port_no);            /*   ﾎﾟｰﾄ番号    (R/W) */
  printf(" resv4[ 1 ]           \n");
  printf(" host_tx_resum        = 0x%08x\n",pIoMap->host_tx_resum);          /*   ﾎｽﾄからのﾃﾞｰﾀ送信を再開する割合 (R/W) */
  printf(" host_tx_pause        = 0x%08x\n",pIoMap->host_tx_pause);          /*   ﾎｽﾄからのﾃﾞｰﾀ送信を中断する割合 (R/W) */
  printf(" rs232c_rx_resum      = 0x%08x\n",pIoMap->rs232c_rx_resum);        /*   RS-232Cからの受信したﾃﾞｰﾀの送信を再開する割合 (R/W) */
  printf(" re232c_rx_pause      = 0x%08x\n",pIoMap->re232c_rx_pause);        /*   RS-232Cからの受信したﾃﾞｰﾀの送信を中断する割合 (R/W) */
  printf(" resv5[ 24 ]          \n");
  printf(" status_reg           = 0x%08x\n",pIoMap->status_reg);             /*   ｽﾃｰﾀｽﾚｼﾞｽﾀ  (R) */
  printf(" handle               = 0x%08x\n",pIoMap->handle);                 /*   ﾊﾝﾄﾞﾙ番号   (R) */
  printf(" lsr                  = 0x%08x\n",pIoMap->lsr);                    /*   LSRﾚｼﾞｽﾀ    (R) */
  printf(" msr                  = 0x%08x\n",pIoMap->msr);                    /*   ﾓﾃﾞﾑｽﾃｰﾀｽ   (R) */
  printf(" resv6[ 11 ]          \n");
  printf(" parity_errors        = 0x%08x\n",pIoMap->parity_errors);          /*     ﾊﾟﾘﾃｨｴﾗｰ */
  printf(" frame_errors         = 0x%08x\n",pIoMap->frame_errors);           /*     ﾌﾚｰﾑｴﾗｰ */
  printf(" overrun_errors       = 0x%08x\n",pIoMap->overrun_errors);         /*     ｵｰﾊﾞｰﾗﾝｴﾗｰ */
  printf(" resv7[ 26 ]          \n");
  printf("============================================\n");
}

