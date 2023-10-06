/**
   FactoryIT ���ĺ�ͯ��
*/

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned int  DWORD;
typedef unsigned long DWORD64;

/*--------------------------------------------------------------*/
/*  �ʐM���ފ֘A                                                */
/*--------------------------------------------------------------*/
/* ����ͯ�� */
#ifdef  WIN32   /* Windows */
#pragma pack(1)
#endif
typedef struct  _SVHEAD {
  WORD    wSVHId;             /* ���ʎq */
  BYTE    bSVHVer;            /* ͯ�ލ\�����ް�ޮ� */
  BYTE    bSVHCommand;        /* ����� */
  WORD    wSVHSeqNo;          /* ���ݽ�ԍ� */
  WORD    wSVHReplyId;        /* �������ʎq */
  DWORD   dwSVHVAddr;         /* ���z���ڽ */
  WORD    wSVHAccessSize;     /* �������� */
  WORD    wSVHStatus;         /* �ð�� */
  BYTE    bSVHAccInd[8];      /* �������ʎq */
  BYTE    bSVHRemMacAddr[6];  /* �����MAC���ڽ */
  BYTE    bSVHResv[6];        /* �\�� */
  /* �ް� */
}SVHEAD,*PSVHEAD;
#ifdef  WIN32   /* Windows */
#pragma pack()
#endif

#define SV_ID                       ( 0x5653 )      /* ���ʎq('SV') */
#define SV_VER                      ( 1 )           /* ͯ�ލ\�����ް�ޮ� */
#define SV_DATA_SIZE                ( 1436 )        /* �ް����� */

/* �ׯ�ߺ���ގ����ް����̍\�� */
#ifdef  WIN32   /* Windows */
#pragma pack(1)
#endif
typedef struct _SVHTRAPDATA {
  WORD    wTrapCommand;       /* ����ޔԍ� */
  WORD    wTrapDataLen;       /* �ް��� */
  DWORD   dwTrapFragment;     /* �׸�����ް� */
  /* �ް� */
}SVHTRAPDATA, *PSVHTRAPDATA;
#ifdef  WIN32   /* Windows */
#pragma pack()
#endif

/* ����ޔԍ� */
#define SV_READ                     ( 1 )           /* ���z���ڽ�̓Ǎ��� */
#define SV_WRITE                    ( 2 )           /* ���z���ڽ�ւ̏����� */
#define SV_MESSAGE                  ( 3 )           /* ү���ޑ��M */
#define SV_TRAP                     ( 4 )           /* �ׯ�� */
#define SV_RESET                    ( 5 )           /* ؾ�� */
#define SV_DATA_STREAM              ( 6 )           /* �ް���ذ� */
#define SV_ACK_FLAG                 ( 0x80 )        /* �����ڰ�(ACK)�׸� */
#define SV_NO_ACK_FLAG              ( 0x40 )        /* �����s�v�ڰ�(NoACK)�׸� */
#define SV_COMMAND_MASK             ( 0x3f )        /* ������ޯ� */

/* �ð�� */
#define SV_OK                       ( 0x0000 )      /* ����I�� */
#define SV_AS_NO                    ( 0x0001 )      /* �������ᔽ�F Read Only �̗̈�ɏ������݂��s�Ȃ����ꍇ */
#define SV_AREA_ERR                 ( 0x0002 )      /* �̈�װ�F �@��Œ�`����Ă��Ȃ��̈�ɱ��������ꍇ */
#define SV_AS_SIZE_ERR              ( 0x0003 )      /* �������޴װ�F 1452�޲Ĉȏ�̱�����v�������ꍇ */
#define SV_PARM_ERR                 ( 0x0004 )      /* ���Ұ��װ�F ���Ұ��̓��e���s���ł���B��߰ĊO�̺���ޓ�����M������ */
#define SV_LENGTH_ERR               ( 0x0005 )      /* �ݸ޽�װ�F ���M�����s���ł���BUDP/IP����Z�o������ް����Ɛ������Ȃ��� */
#define SV_RES_FL                   ( 0x0006 )      /* ؿ���s���F ACK�ް��҂�������������Aؿ���s���ƂȂ����ꍇ�B��ѱ�Ăɂ��J������ */
#define SV_WRITE_ERR                ( 0x0007 )      /* �������ݴװ : ��ذт��ް����M�ȂǂŐ���ɏ������݂��ł��Ȃ��ꍇ */
#define SV_TIMEOUT_ERR              ( 0x0008 )      /* ��ѱ�Ĵװ : ��ذт��ް����M�Ȃǂ���ѱ�Ă����������ꍇ */


/*--------------------------------------------------------------*/
/*  ��ذѺ���ފ֘A                                              */
/*--------------------------------------------------------------*/
/* ��ذ�ͯ�� */
#ifdef  WIN32   /* Windows */
#pragma pack(1)
#endif
typedef struct _STRMHEAD {
  BYTE    bSTRMVer;           /* ͯ�ލ\�����ް�ޮ� */
  BYTE    bSTRMCommand;       /* ����� */
  WORD    bSTRMHandle;        /* ����ٔԍ� */
  DWORD   dwSTRMOffset;       /* �̾�� */
  WORD    wSTRMSize;          /* �ް��� */
  WORD    wSVHResv;           /* �\�� */
  /* �ް� */
}STRMHEAD, *PSTRMHEAD;
#ifdef  WIN32   /* Windows */
#pragma pack()
#endif

#define STRM_VER                    ( 1 )           /* ͯ�ލ\�����ް�ޮ� */
#define STRM_DATA_SIZE              ( 1424 )        /* �ް����� */


/* ��ذѺ���ޔԍ� */
#define STRM_RESV                   ( 0 )           /* �\�� */
#define STRM_OPEN                   ( 1 )           /* ����� */
#define STRM_CLOSE                  ( 2 )           /* �۰�� */
#define STRM_SEND                   ( 3 )           /* �ް����M */
#define STRM_BST_SEND               ( 4 )           /* �ް����M(�ް��) */
#define STRM_TRAP                   ( 5 )           /* �ׯ�� */
#define STRM_COMMAND_MASK           ( 0x0f )        /* ������ޯ� */

/* ��ذѺ���ނ̎��۔ԍ� */
#define STRM_EVENT_CANSEL           ( 1 )           /* ���M�̷�ݾ� */
#define STRM_EVENT_PAUSE            ( 2 )           /* ���M�̈ꎞ��~ */
#define STRM_EVENT_RESUME           ( 3 )           /* ���M�̍ĊJ */
#define STRM_EVENT_STA_CHANGE       ( 4 )           /* �ð���ω� */


/*--------------------------------------------------------------*/
/*  ���zMAP�֘A                                                 */
/*--------------------------------------------------------------*/
#define SV_VA_IO_SPACE1     ( 0x00300000L )         /* I/O���1(64K�޲�) */
#define SV_VA_IO_CHSPACE    ( 0x00010000 )         /* I/O���(64K�޲�) */
#define SV_VA_IO_CTRL       ( SV_VA_IO_SPACE1 )     /* I/O����(SER_DEVCTL�\���̂̊i�[���ڽ) */

#define SV_VA_SETCTRL_OFFS  ( 0x00000020 )          /* ���޲�����̈ʒu */
#define SV_VA_SETCTRL_SIZE  ( 4 )                   /* ���޲�����̻��� */

#define SV_VA_SETSTATE_OFFS ( 0x00000024 )          /* �ްڰĂ̈ʒu */
#define SV_VA_SETSTATE_SIZE ( 20 )                  /* �ްڰā`�M������ �܂ł̻��� */

#define SV_VA_GETMSTAT_OFFS ( 0x000000a8 )          /* ���ѽð���̈ʒu */
#define SV_VA_GETMSTAT_SIZE ( 4  )                  /* ���ѽð���̻��� */

#define SV_VA_SET_XON_OFFS  ( 0x00000030 )          /* XON�����̈ʒu */
#define SV_VA_SET_XON_SIZE  ( 2  )                  /* XOFF�����̻��� */

#define SV_VA_SET_XOFF_OFFS ( 0x00000032 )          /* ��ڰ��ޯĂ̈ʒu */
#define SV_VA_SET_XOFF_SIZE ( 2  )                  /* ��ڰ��ޯĂ̻��� */

#define SV_VA_SET_BRK_OFFS  ( 0x00000034 )          /* ��ڰ��ޯĂ̈ʒu */
#define SV_VA_SET_BRK_SIZE  ( 2  )                  /* ��ڰ��ޯĂ̻��� */


#define SV_VA_SET_SIG_OFFS  ( 0x00000036 )          /* �M������̈ʒu */
#define SV_VA_SET_SIG_SIZE  ( 2  )                  /* �M������̻��� */

#define SV_VA_GETCNT_OFFS   ( 0x000000c0 )          /* �������̈ʒu */
#define SV_VA_GETCNT_SIZE   ( 4 + 4 + 4  )          /* �������̻��� */


#ifdef  WIN32   /* Windows */
#pragma pack(1)
#endif
typedef struct  _SER_DEVCTL {           /* ���޲������� */
  WORD    version;                /*   �ް�ޮݔԍ� (R) */
  WORD    resv0[ 7 ];             /*   �\��        (R/W) */
  BYTE    name[ 16 ];             /*   ���޲���    (R) */
  DWORD   ctrl_reg;               /*   ����ڼ޽�   (R/W) */
  DWORD   baudrate;               /*   �ްڰ�      (R/W) */
  WORD    data_bit;               /*   �ް��ޯ�    (R/W) */
  WORD    parity_bit;             /*   ���è�ޯ�   (R/W) */
  WORD    stop_bit;               /*   �į���ޯ�   (R/W) */
  WORD    flow_ctrl;              /*   �۰����     (R/W) */
  WORD    xon_char;               /*   XON����     (R/W) */
  WORD    xoff_char;              /*   XOFF����    (R/W) */
  WORD    break_bit;              /*   ��ڰ��ޯ�   (R/W) */
  WORD    signal_ctrl;            /*   �M������    (R/W) */
  DWORD   mode;                   /*   ����Ӱ��    (R/W) */
  DWORD   tx_wait;                /*   ���M�҂�����(R/W) */
  DWORD   sampl_time;             /*   �ð���ω��̻����ݸގ��� (R/W) */
  DWORD   age_time;               /*   ��������    (R/W) */

  /* �������ޗp�đ���� */
  DWORD   ctrl_cmd_retx_intval;   /*   �đ��Ԋu    (R/W) */
  WORD    ctrl_cmd_retx_cnt;      /*   �đ���    (R/W) */
  WORD    resv1[ 1 ];             /*   �\�� */

  /* �ް��p�đ���� */
  DWORD   data_cmd_retx_intval;   /*   �đ��Ԋu    (R/W) */
  WORD    data_cmd_retx_cnt;      /*   �đ���    (R/W) */
  WORD    resv2[ 1 ];             /*   �\�� */

  /* �Ǘ��җp���ڽ��� */
  DWORD   ip_addr;                /*   IP���ڽ     (R/W) */
  WORD    port_no;                /*   �߰Ĕԍ�    (R/W) */
  WORD    resv3[ 1 ];             /*   �\�� */

  /* ��ذѺ���ޗp���ڽ��� */
  DWORD   stm_ip_addr;            /*   IP���ڽ     (R/W) */
  WORD    stm_port_no;            /*   �߰Ĕԍ�    (R/W) */
  WORD    resv4[ 1 ];             /*   �\�� */

  WORD    host_tx_resum;          /*   νĂ�����ް����M���ĊJ���銄�� (R/W) */
  WORD    host_tx_pause;          /*   νĂ�����ް����M�𒆒f���銄�� (R/W) */
  WORD    rs232c_rx_resum;        /*   RS-232C����̎�M�����ް��̑��M���ĊJ���銄�� (R/W) */
  WORD    re232c_rx_pause;        /*   RS-232C����̎�M�����ް��̑��M�𒆒f���銄�� (R/W) */
  WORD    resv5[ 24 ];            /*   �\�� */

  DWORD   status_reg;             /*   �ð��ڼ޽�  (R) */
  WORD    handle;                 /*   ����ٔԍ�   (R) */
  WORD    lsr;                    /*   LSRڼ޽�    (R) */
  WORD    msr;                    /*   ���ѽð��   (R) */
  WORD    resv6[ 11 ];            /*   �\�� */

  /*   �װ�����֘A (R/W) */
  DWORD   parity_errors;          /*     ���è�װ */
  DWORD   frame_errors;           /*     �ڰѴװ */
  DWORD   overrun_errors;         /*     ���ް�ݴװ */
  WORD    resv7[ 26 ];            /*   �\�� */
}SER_DEVCTL, *PSER_DEVCTL;
#ifdef  WIN32   /* Windows */
#pragma pack()
#endif

#define SER_FITDEV_VERSION      1                   /* �ް�ޮݔԍ� */
#define SER_FITDRV_NAME         "COM1"              /* ���޲��� */
                                                    /* ����ڼ޽� (R/W) */
#define SER_CR_INIT             0x00000000          /*   ������� */
#define SER_CR_START            0x00000001          /*   �J�n */
#define SER_CR_HOST_RXBUFFULL   0x00000002          /*   ν�PC���̎�M�ޯ̧����t */
                                                    /* �ð��ڼ޽� (R) */
#define SER_SR_NOTUSE           0x00000000          /*   ���g�p */
#define SER_SR_ACTIVE           0x00000001          /*   ���쒆 */
#define SER_SR_STREAM           0x00000002          /*   ��ذѵ���ݒ� */
#define SER_SR_USE              0x00000004          /*   �g�p�� */
#define SER_SR_TXENABLE         0x00000008          /*   ���M�� */
#define SER_SR_TXBUFEMPTY       0x00000010          /*   ���M�ޯ̧���� */
#define SER_SR_RXDATA           0x00000020          /*   ��M�ް��L�� */
#define SER_SR_STATUSCHANGE     0x00000040          /*   �ð���ύX */

#define COM_DEV_HANDLE          0x0001              /* ��̫�� ����ٔԍ� */


void print_svhead(PSVHEAD pSVHead);
void print_strmhead(PSTRMHEAD pSTRMHead);
void print_iomap(PSER_DEVCTL pIoMap);

