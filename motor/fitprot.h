/**
   FactoryIT ﾌﾟﾛﾄｺﾙﾍｯﾀﾞ
*/

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned int  DWORD;
typedef unsigned long DWORD64;

/*--------------------------------------------------------------*/
/*  通信ｻｰﾊﾞ関連                                                */
/*--------------------------------------------------------------*/
/* 共通ﾍｯﾀﾞ */
#ifdef  WIN32   /* Windows */
#pragma pack(1)
#endif
typedef struct  _SVHEAD {
  WORD    wSVHId;             /* 識別子 */
  BYTE    bSVHVer;            /* ﾍｯﾀﾞ構造のﾊﾞｰｼﾞｮﾝ */
  BYTE    bSVHCommand;        /* ｺﾏﾝﾄﾞ */
  WORD    wSVHSeqNo;          /* ｼｰｹﾝｽ番号 */
  WORD    wSVHReplyId;        /* 応答識別子 */
  DWORD   dwSVHVAddr;         /* 仮想ｱﾄﾞﾚｽ */
  WORD    wSVHAccessSize;     /* ｱｸｾｽｻｲｽﾞ */
  WORD    wSVHStatus;         /* ｽﾃｰﾀｽ */
  BYTE    bSVHAccInd[8];      /* ｱｸｾｽ識別子 */
  BYTE    bSVHRemMacAddr[6];  /* 相手のMACｱﾄﾞﾚｽ */
  BYTE    bSVHResv[6];        /* 予約 */
  /* ﾃﾞｰﾀ */
}SVHEAD,*PSVHEAD;
#ifdef  WIN32   /* Windows */
#pragma pack()
#endif

#define SV_ID                       ( 0x5653 )      /* 識別子('SV') */
#define SV_VER                      ( 1 )           /* ﾍｯﾀﾞ構造のﾊﾞｰｼﾞｮﾝ */
#define SV_DATA_SIZE                ( 1436 )        /* ﾃﾞｰﾀｻｲｽﾞ */

/* ﾄﾗｯﾌﾟｺﾏﾝﾄﾞ時のﾃﾞｰﾀ部の構造 */
#ifdef  WIN32   /* Windows */
#pragma pack(1)
#endif
typedef struct _SVHTRAPDATA {
  WORD    wTrapCommand;       /* ｺﾏﾝﾄﾞ番号 */
  WORD    wTrapDataLen;       /* ﾃﾞｰﾀ数 */
  DWORD   dwTrapFragment;     /* ﾌﾗｸﾞﾒﾝﾄﾃﾞｰﾀ */
  /* ﾃﾞｰﾀ */
}SVHTRAPDATA, *PSVHTRAPDATA;
#ifdef  WIN32   /* Windows */
#pragma pack()
#endif

/* ｺﾏﾝﾄﾞ番号 */
#define SV_READ                     ( 1 )           /* 仮想ｱﾄﾞﾚｽの読込み */
#define SV_WRITE                    ( 2 )           /* 仮想ｱﾄﾞﾚｽへの書込み */
#define SV_MESSAGE                  ( 3 )           /* ﾒｯｾｰｼﾞ送信 */
#define SV_TRAP                     ( 4 )           /* ﾄﾗｯﾌﾟ */
#define SV_RESET                    ( 5 )           /* ﾘｾｯﾄ */
#define SV_DATA_STREAM              ( 6 )           /* ﾃﾞｰﾀｽﾄﾘｰﾑ */
#define SV_ACK_FLAG                 ( 0x80 )        /* 応答ﾌﾚｰﾑ(ACK)ﾌﾗｸﾞ */
#define SV_NO_ACK_FLAG              ( 0x40 )        /* 応答不要ﾌﾚｰﾑ(NoACK)ﾌﾗｸﾞ */
#define SV_COMMAND_MASK             ( 0x3f )        /* ｺﾏﾝﾄﾞﾋﾞｯﾄ */

/* ｽﾃｰﾀｽ */
#define SV_OK                       ( 0x0000 )      /* 正常終了 */
#define SV_AS_NO                    ( 0x0001 )      /* ｱｸｾｽ権違反： Read Only の領域に書き込みを行なった場合 */
#define SV_AREA_ERR                 ( 0x0002 )      /* 領域ｴﾗｰ： 機器で定義されていない領域にｱｸｾｽした場合 */
#define SV_AS_SIZE_ERR              ( 0x0003 )      /* ｱｸｾｽｻｲｽﾞｴﾗｰ： 1452ﾊﾞｲﾄ以上のｱｸｾｽを要求した場合 */
#define SV_PARM_ERR                 ( 0x0004 )      /* ﾊﾟﾗﾒｰﾀｴﾗｰ： ﾊﾟﾗﾒｰﾀの内容が不正である。ｻﾎﾟｰﾄ外のｺﾏﾝﾄﾞ等を受信した等 */
#define SV_LENGTH_ERR               ( 0x0005 )      /* ﾚﾝｸﾞｽｴﾗｰ： 送信長が不正である。UDP/IPから算出されるﾃﾞｰﾀ数と整合しない等 */
#define SV_RES_FL                   ( 0x0006 )      /* ﾘｿｰｽ不足： ACKﾃﾞｰﾀ待ちのﾀｽｸが多く、ﾘｿｰｽ不足となった場合。ﾀｲﾑｱｳﾄにより開放され */
#define SV_WRITE_ERR                ( 0x0007 )      /* 書き込みｴﾗｰ : ｽﾄﾘｰﾑのﾃﾞｰﾀ送信などで正常に書き込みができない場合 */
#define SV_TIMEOUT_ERR              ( 0x0008 )      /* ﾀｲﾑｱｳﾄｴﾗｰ : ｽﾄﾘｰﾑのﾃﾞｰﾀ送信などでﾀｲﾑｱｳﾄが発生した場合 */


/*--------------------------------------------------------------*/
/*  ｽﾄﾘｰﾑｺﾏﾝﾄﾞ関連                                              */
/*--------------------------------------------------------------*/
/* ｽﾄﾘｰﾑﾍｯﾀﾞ */
#ifdef  WIN32   /* Windows */
#pragma pack(1)
#endif
typedef struct _STRMHEAD {
  BYTE    bSTRMVer;           /* ﾍｯﾀﾞ構造のﾊﾞｰｼﾞｮﾝ */
  BYTE    bSTRMCommand;       /* ｺﾏﾝﾄﾞ */
  WORD    bSTRMHandle;        /* ﾊﾝﾄﾞﾙ番号 */
  DWORD   dwSTRMOffset;       /* ｵﾌｾｯﾄ */
  WORD    wSTRMSize;          /* ﾃﾞｰﾀ数 */
  WORD    wSVHResv;           /* 予約 */
  /* ﾃﾞｰﾀ */
}STRMHEAD, *PSTRMHEAD;
#ifdef  WIN32   /* Windows */
#pragma pack()
#endif

#define STRM_VER                    ( 1 )           /* ﾍｯﾀﾞ構造のﾊﾞｰｼﾞｮﾝ */
#define STRM_DATA_SIZE              ( 1424 )        /* ﾃﾞｰﾀｻｲｽﾞ */


/* ｽﾄﾘｰﾑｺﾏﾝﾄﾞ番号 */
#define STRM_RESV                   ( 0 )           /* 予約 */
#define STRM_OPEN                   ( 1 )           /* ｵｰﾌﾟﾝ */
#define STRM_CLOSE                  ( 2 )           /* ｸﾛｰｽﾞ */
#define STRM_SEND                   ( 3 )           /* ﾃﾞｰﾀ送信 */
#define STRM_BST_SEND               ( 4 )           /* ﾃﾞｰﾀ送信(ﾊﾞｰｽﾄ) */
#define STRM_TRAP                   ( 5 )           /* ﾄﾗｯﾌﾟ */
#define STRM_COMMAND_MASK           ( 0x0f )        /* ｺﾏﾝﾄﾞﾋﾞｯﾄ */

/* ｽﾄﾘｰﾑｺﾏﾝﾄﾞの事象番号 */
#define STRM_EVENT_CANSEL           ( 1 )           /* 送信のｷｬﾝｾﾙ */
#define STRM_EVENT_PAUSE            ( 2 )           /* 送信の一時停止 */
#define STRM_EVENT_RESUME           ( 3 )           /* 送信の再開 */
#define STRM_EVENT_STA_CHANGE       ( 4 )           /* ｽﾃｰﾀｽ変化 */


/*--------------------------------------------------------------*/
/*  仮想MAP関連                                                 */
/*--------------------------------------------------------------*/
#define SV_VA_IO_SPACE1     ( 0x00300000L )         /* I/O空間1(64Kﾊﾞｲﾄ) */
#define SV_VA_IO_CHSPACE    ( 0x00010000 )         /* I/O空間(64Kﾊﾞｲﾄ) */
#define SV_VA_IO_CTRL       ( SV_VA_IO_SPACE1 )     /* I/O操作(SER_DEVCTL構造体の格納ｱﾄﾞﾚｽ) */

#define SV_VA_SETCTRL_OFFS  ( 0x00000020 )          /* ﾃﾞﾊﾞｲｽ制御の位置 */
#define SV_VA_SETCTRL_SIZE  ( 4 )                   /* ﾃﾞﾊﾞｲｽ制御のｻｲｽﾞ */

#define SV_VA_SETSTATE_OFFS ( 0x00000024 )          /* ﾎﾞｰﾚｰﾄの位置 */
#define SV_VA_SETSTATE_SIZE ( 20 )                  /* ﾎﾞｰﾚｰﾄ～信号制御 までのｻｲｽﾞ */

#define SV_VA_GETMSTAT_OFFS ( 0x000000a8 )          /* ﾓﾃﾞﾑｽﾃｰﾀｽの位置 */
#define SV_VA_GETMSTAT_SIZE ( 4  )                  /* ﾓﾃﾞﾑｽﾃｰﾀｽのｻｲｽﾞ */

#define SV_VA_SET_XON_OFFS  ( 0x00000030 )          /* XON文字の位置 */
#define SV_VA_SET_XON_SIZE  ( 2  )                  /* XOFF文字のｻｲｽﾞ */

#define SV_VA_SET_XOFF_OFFS ( 0x00000032 )          /* ﾌﾞﾚｰｸﾋﾞｯﾄの位置 */
#define SV_VA_SET_XOFF_SIZE ( 2  )                  /* ﾌﾞﾚｰｸﾋﾞｯﾄのｻｲｽﾞ */

#define SV_VA_SET_BRK_OFFS  ( 0x00000034 )          /* ﾌﾞﾚｰｸﾋﾞｯﾄの位置 */
#define SV_VA_SET_BRK_SIZE  ( 2  )                  /* ﾌﾞﾚｰｸﾋﾞｯﾄのｻｲｽﾞ */


#define SV_VA_SET_SIG_OFFS  ( 0x00000036 )          /* 信号制御の位置 */
#define SV_VA_SET_SIG_SIZE  ( 2  )                  /* 信号制御のｻｲｽﾞ */

#define SV_VA_GETCNT_OFFS   ( 0x000000c0 )          /* ｶｳﾝﾀ情報の位置 */
#define SV_VA_GETCNT_SIZE   ( 4 + 4 + 4  )          /* ｶｳﾝﾀ情報のｻｲｽﾞ */


#ifdef  WIN32   /* Windows */
#pragma pack(1)
#endif
typedef struct  _SER_DEVCTL {           /* ﾃﾞﾊﾞｲｽ制御情報 */
  WORD    version;                /*   ﾊﾞｰｼﾞｮﾝ番号 (R) */
  WORD    resv0[ 7 ];             /*   予約        (R/W) */
  BYTE    name[ 16 ];             /*   ﾃﾞﾊﾞｲｽ名    (R) */
  DWORD   ctrl_reg;               /*   制御ﾚｼﾞｽﾀ   (R/W) */
  DWORD   baudrate;               /*   ﾎﾞｰﾚｰﾄ      (R/W) */
  WORD    data_bit;               /*   ﾃﾞｰﾀﾋﾞｯﾄ    (R/W) */
  WORD    parity_bit;             /*   ﾊﾟﾘﾃｨﾋﾞｯﾄ   (R/W) */
  WORD    stop_bit;               /*   ｽﾄｯﾌﾟﾋﾞｯﾄ   (R/W) */
  WORD    flow_ctrl;              /*   ﾌﾛｰ制御     (R/W) */
  WORD    xon_char;               /*   XON文字     (R/W) */
  WORD    xoff_char;              /*   XOFF文字    (R/W) */
  WORD    break_bit;              /*   ﾌﾞﾚｰｸﾋﾞｯﾄ   (R/W) */
  WORD    signal_ctrl;            /*   信号制御    (R/W) */
  DWORD   mode;                   /*   動作ﾓｰﾄﾞ    (R/W) */
  DWORD   tx_wait;                /*   送信待ち時間(R/W) */
  DWORD   sampl_time;             /*   ｽﾃｰﾀｽ変化のｻﾝﾌﾟﾘﾝｸﾞ時間 (R/W) */
  DWORD   age_time;               /*   生存時間    (R/W) */

  /* 制御ｺﾏﾝﾄﾞ用再送情報 */
  DWORD   ctrl_cmd_retx_intval;   /*   再送間隔    (R/W) */
  WORD    ctrl_cmd_retx_cnt;      /*   再送回数    (R/W) */
  WORD    resv1[ 1 ];             /*   予約 */

  /* ﾃﾞｰﾀ用再送情報 */
  DWORD   data_cmd_retx_intval;   /*   再送間隔    (R/W) */
  WORD    data_cmd_retx_cnt;      /*   再送回数    (R/W) */
  WORD    resv2[ 1 ];             /*   予約 */

  /* 管理者用ｱﾄﾞﾚｽ情報 */
  DWORD   ip_addr;                /*   IPｱﾄﾞﾚｽ     (R/W) */
  WORD    port_no;                /*   ﾎﾟｰﾄ番号    (R/W) */
  WORD    resv3[ 1 ];             /*   予約 */

  /* ｽﾄﾘｰﾑｺﾏﾝﾄﾞ用ｱﾄﾞﾚｽ情報 */
  DWORD   stm_ip_addr;            /*   IPｱﾄﾞﾚｽ     (R/W) */
  WORD    stm_port_no;            /*   ﾎﾟｰﾄ番号    (R/W) */
  WORD    resv4[ 1 ];             /*   予約 */

  WORD    host_tx_resum;          /*   ﾎｽﾄからのﾃﾞｰﾀ送信を再開する割合 (R/W) */
  WORD    host_tx_pause;          /*   ﾎｽﾄからのﾃﾞｰﾀ送信を中断する割合 (R/W) */
  WORD    rs232c_rx_resum;        /*   RS-232Cからの受信したﾃﾞｰﾀの送信を再開する割合 (R/W) */
  WORD    re232c_rx_pause;        /*   RS-232Cからの受信したﾃﾞｰﾀの送信を中断する割合 (R/W) */
  WORD    resv5[ 24 ];            /*   予約 */

  DWORD   status_reg;             /*   ｽﾃｰﾀｽﾚｼﾞｽﾀ  (R) */
  WORD    handle;                 /*   ﾊﾝﾄﾞﾙ番号   (R) */
  WORD    lsr;                    /*   LSRﾚｼﾞｽﾀ    (R) */
  WORD    msr;                    /*   ﾓﾃﾞﾑｽﾃｰﾀｽ   (R) */
  WORD    resv6[ 11 ];            /*   予約 */

  /*   ｴﾗｰｶｳﾝﾀ関連 (R/W) */
  DWORD   parity_errors;          /*     ﾊﾟﾘﾃｨｴﾗｰ */
  DWORD   frame_errors;           /*     ﾌﾚｰﾑｴﾗｰ */
  DWORD   overrun_errors;         /*     ｵｰﾊﾞｰﾗﾝｴﾗｰ */
  WORD    resv7[ 26 ];            /*   予約 */
}SER_DEVCTL, *PSER_DEVCTL;
#ifdef  WIN32   /* Windows */
#pragma pack()
#endif

#define SER_FITDEV_VERSION      1                   /* ﾊﾞｰｼﾞｮﾝ番号 */
#define SER_FITDRV_NAME         "COM1"              /* ﾃﾞﾊﾞｲｽ名 */
                                                    /* 制御ﾚｼﾞｽﾀ (R/W) */
#define SER_CR_INIT             0x00000000          /*   初期状態 */
#define SER_CR_START            0x00000001          /*   開始 */
#define SER_CR_HOST_RXBUFFULL   0x00000002          /*   ﾎｽﾄPC側の受信ﾊﾞｯﾌｧが一杯 */
                                                    /* ｽﾃｰﾀｽﾚｼﾞｽﾀ (R) */
#define SER_SR_NOTUSE           0x00000000          /*   未使用 */
#define SER_SR_ACTIVE           0x00000001          /*   動作中 */
#define SER_SR_STREAM           0x00000002          /*   ｽﾄﾘｰﾑｵｰﾌﾟﾝ中 */
#define SER_SR_USE              0x00000004          /*   使用中 */
#define SER_SR_TXENABLE         0x00000008          /*   送信可 */
#define SER_SR_TXBUFEMPTY       0x00000010          /*   送信ﾊﾞｯﾌｧが空 */
#define SER_SR_RXDATA           0x00000020          /*   受信ﾃﾞｰﾀ有り */
#define SER_SR_STATUSCHANGE     0x00000040          /*   ｽﾃｰﾀｽ変更 */

#define COM_DEV_HANDLE          0x0001              /* ﾃﾞﾌｫﾙﾄ ﾊﾝﾄﾞﾙ番号 */


void print_svhead(PSVHEAD pSVHead);
void print_strmhead(PSTRMHEAD pSTRMHead);
void print_iomap(PSER_DEVCTL pIoMap);

