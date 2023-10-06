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


typedef struct _RPCOMFIT_CONFIG {
  int     socket;
  struct sockaddr_in socketin;
  int     deviceid;               /*   ﾃﾞﾊﾞｲｽﾊﾞﾝｺﾞｳ      */
  WORD    seq_num;                /*   通信番号          */
  DWORD   baudrate;               /*   ﾎﾞｰﾚｰﾄ      (R/W) */
  WORD    data_bit;               /*   ﾃﾞｰﾀﾋﾞｯﾄ    (R/W) */
  WORD    parity_bit;             /*   ﾊﾟﾘﾃｨﾋﾞｯﾄ   (R/W) */
  WORD    stop_bit;               /*   ｽﾄｯﾌﾟﾋﾞｯﾄ   (R/W) */
}rpcomfitconfig, *prpcomfitconfig;


