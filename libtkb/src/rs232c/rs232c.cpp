/*!
\file rs232c.cpp
\author Y.Koide
\date 2006.08.15
\brief RS-232Cを制御するライブラリ
*/
#ifdef WIN32
# include <windows.h>
# include <locale.h>
#endif

#include <stdio.h>
//#include "../u/u.h"
#include "rs232c.h"

static const char version[] = "0.0.0";
#define COM_MAX 20

/*! \fn const char* rs232cVersion()
\reutrn バージョン文字列
*/
const char* rs232cVersion(){
  return version;
}

#ifdef WIN32
static HANDLE hComm[COM_MAX]; //!RS-232Cの通信ハンドル

/*! \fn int rs232cInit(int comNo, int bps, int byteSize, int parityBit, int stopBit)
\brief rs232cライブラリを使用する際に最初に呼び出す
\param[in] comNo comポート番号。整数1～
\param[in] bps 通信速度
\param[in] byteSize 7:7bit 8:8bit
\param[in] parityBit 0:none 1:oddParity 2:evenParity
\param[in] stopBit 0,1:1bit 2:2bit 3:1.5bit
\return 0:Success other:Error
*/
int rs232cInit(int comNo, int bps, int byteSize, int parityBit, int stopBit){
  int ret;
  wchar_t tmp[10];
  char tmpc[10];

  if(comNo<=0 || comNo > COM_MAX || comNo < -COM_MAX){
    printf("rs232cInit(); comNo[%d]\n",comNo);
    return -1;
  }

  sprintf(tmpc, "COM%d",comNo);
  setlocale(LC_ALL, "ja");
  ret = mbstowcs(tmp, tmpc, strlen(tmpc) + 1);
  if(ret == -1){
    printf("rs232cINit(); mbstowcs(); error!!\n");
    return -1;
  }

  hComm[comNo] = CreateFile(tmp, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(hComm[comNo] == INVALID_HANDLE_VALUE){
    printf("rs232cInit(); Com[%d] Open error!!\n", comNo);
    return -1;
  }
  DCB dcb;
  GetCommState(hComm[comNo], &dcb);

  switch(bps){
    case 0: /* drop DTR */
    case 50:
    case 75:
    case 110:
    case 134: /* 134.5 bps*/
    case 150:
    case 200:
    case 300:
    case 600:
    case 1200:
    case 1800:
    case 2400:
    case 4800:
    case 9600:
    case 19200:
    case 38400:
    case 57600:
    case 76800:
    case 115200:
    dcb.BaudRate = (DWORD) bps;
    break;
    default:
      printf("rs232cInit(); Com[%d] bps[%d] error!!\n", comNo, bps);
      return -1;
  }
  switch(byteSize){
        case 7:
        case 8:
          dcb.ByteSize = (BYTE) byteSize;
          break;
        default:
          printf("rs232cInit(); Com[%d] byteSize[%d]error!!\n", comNo, byteSize);
          return -1;
  }
  switch(parityBit){
        case 0:
          dcb.fParity = FALSE;
          dcb.Parity = NOPARITY;
          break;
        case 1:
          dcb.fParity = TRUE;
          dcb.Parity = ODDPARITY;
          break;
        case 2:
          dcb.fParity = TRUE;
          dcb.Parity = EVENPARITY;
          break;
        default:
          printf("rs232cInit(); Com[%d] parityBit[%d]error!!\n", comNo, parityBit);
          return -1;
  }
  switch(stopBit){
        case 0:
        case 1: /* 1bit */
          dcb.StopBits = ONESTOPBIT;
          break;
        case 2: /* 2bit */
          dcb.StopBits = TWOSTOPBITS;
          break;
        case 3: /* 1.5 bit */ 
          dcb.StopBits = ONE5STOPBITS;
          break;
        default:
          printf("rs232cInit(); Com[%d] stopBit[%d]error!!\n", comNo, stopBit);
          return -1;
  }
  SetCommState(hComm[comNo],&dcb);
  return 0;
}

/*! \fn int rs232cEnd(int comNo)
\brief rs232cライブラリを終了させる際に呼ぶ
\param[in] comNo comポート番号。整数1～
\return 0:Success other:Error
*/
int rs232cEnd(int comNo){

  if(hComm[comNo] == INVALID_HANDLE_VALUE){
    printf("rs232cEnd(); Com[%d] invalid error!!\n", comNo);
    return -1;
  }
  CloseHandle(hComm[comNo]);
  return 0;
}

/*! \fn int rs232cWrite(int comNo, unsigned char* buf, unsigned int bufSize)
\brief RS-232Cからデータを出力する
\param[in] comNo comポート番号。整数1～
\param[in] buf 出力データへのポインタ
\param[in] bufSize 出力データのバイトサイズ
\return over0:writeSize under0:Error
*/
int rs232cWrite(int comNo, unsigned char* buf, const unsigned int bufSize){	
  DWORD writeSize;

  if(hComm[comNo] == INVALID_HANDLE_VALUE){
    printf("rs232cWrite(); Com[%d] invalid error!!\n", comNo);
    return -1;
  }
  WriteFile(hComm[comNo], buf, bufSize, &writeSize, NULL);
  if(writeSize != bufSize){
    printf("rs232cWrite(); Com[%d] writeSize[%d] != bufSize[%d] error!!\n", comNo, writeSize, bufSize);
    return -1;
  }
  return writeSize;
}

/*! \fn int rs232cRead(int comNo, unsigned char* buf, unsigned int bufSize)
\brief RS-232Cからデータを読込む
\param[in] comNo comポート番号。整数1～
\param[in] buf 読込みデータ格納領域
\param[in] bufSize 読込みデータ格納領域のバイトサイズ
\return[in] over0:ReadSize, under0:Error
*/
int rs232cRead(int comNo, unsigned char* buf, const unsigned int bufSize){
  DWORD err;///エラー情報
  COMSTAT comStat;///com Status
  DWORD readCnt;///受信データのバイト数
  DWORD readSize;///読み込んだバイト数

  if(hComm[comNo] == INVALID_HANDLE_VALUE){
    printf("rs232cRead(); Com[%d] invalid error!!\n",comNo);
    return -1;
  }

  ClearCommError(hComm[comNo], &err, &comStat);
  readCnt = comStat.cbInQue;
  if(readCnt > bufSize){
    printf("rs232cRead(); Com[%d] readCnt[%d] > bufSize[%d] error!!\n", comNo, readCnt, bufSize);
    return -1;
  }

  ReadFile(hComm[comNo], buf, readCnt, &readSize, NULL);
  if(readCnt != readSize){
    printf("rs232cRead(); Com[%d] readCnt != readSize error!!\n", comNo);
    return -1;
  }
  return readSize;
}



#else
//OS without Win32
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
//#include <errno.h>
#include <sys/signal.h>
#include <sys/types.h>

#define _POSIX_SOURCE 1 /* POSIX 標準のソース */

void signal_handler_IO(int status);
static int fd[COM_MAX];
volatile int STOP=0;
int wait_flag=1;

int rs232cInit(int comNo, int bps, int byteSize, int parityBit, int stopBit){
  struct termios options;

  if(comNo==0 || comNo > COM_MAX || comNo < - COM_MAX){
    //uM1("rs232cInit(); comNo %d\n",comNo);
    return -1;
  }

  char tmp[20];
  if(comNo >= 1){
    sprintf(tmp, "/dev/usb/ttyUSB%d",comNo-1);
  }
  else if(comNo <=-1){
     sprintf(tmp, "/dev/ttyS%d", -(comNo)-1);
  }
  /* open the port */
  fd[comNo] = open(tmp, O_RDWR | O_NOCTTY | O_NDELAY);
  if(fd[comNo] == -1){
    //printf("rs232cInit(); [/dev/usb/ttyUSB%d] Open error!!\n", comNo);
    return -2;
  }
  fcntl(fd[comNo], F_SETFL, 0);

  /* get the current options */
  tcgetattr(fd[comNo], &options);

  /* set raw input, 1 second timeout */
  options.c_cflag     |= (CLOCAL | CREAD);
  options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag     &= ~OPOST;
  options.c_cc[VMIN]  = 0;
  options.c_cc[VTIME] = 10; /* 10 [/10sec] */

  speed_t s;
  switch(bps){
    case 0: /* drop DTR */
      s=B0;
      break;
    case 50:
      s=B50;
      break;
    case 75:
      s=B75;
      break;
    case 110:
      s=B110;
      break;
    case 134: /* 134.5 bps*/
      s=B134;
      break;
    case 150:
      s=B150;
      break;
    case 200:
      s=B200;
      break;
    case 300:
      s=B300;
      break;
    case 600:
      s=B600;
      break;
    case 1200:
      s=B1200;
      break;
    case 1800:
      s=B1800;
      break;
    case 2400:
      s=B2400;
      break;
    case 4800:
      s=B4800;
      break;
    case 9600:
      s=B9600;
      break;
    case 19200:
      s=B19200;
      break;
    case 38400:
      s=B38400;
      break;
    case 57600:
      s=B57600;
      break;
    /*case 76800:
      s=B76800;
      break;*/
    case 115200:
      s=B115200;
      break;
    default:
      //uM2("rs232cInit(); Com%d bps %d error!!\n", comNo, bps);
      return -3;
  }
  cfsetispeed(&options, s);
  cfsetospeed(&options, s);
  switch(byteSize){
        case 7:
          options.c_cflag &= ~CSIZE; /* Mask the character size bits */
          options.c_cflag |= CS7;    /* Select 7 data bits */
          break;
        case 8:
          options.c_cflag &= ~CSIZE;
          options.c_cflag |= CS8;
          break;
        default:
          //uM2("rs232cInit(); Com%d byteSize %d error!!\n", comNo, byteSize);
          return -4;
  }
  switch(parityBit){
        case 0:
          options.c_cflag &= ~PARENB; /* None parity bit */
          break;
        case 1:
          options.c_cflag |= PARENB;
          options.c_cflag |= PARODD;  /* Odd parity */
          break;
        case 2:
          options.c_cflag |= PARENB;
          options.c_cflag &= ~PARODD; /* Even parity */
          break;
        default:
          //uM2("rs232cInit(); Com[%d] parityBit[%d]error!!\n", comNo, parityBit);
          return -5;
  }
  switch(stopBit){
        case 0:
        case 1: /* 1 stop bits */
          options.c_cflag &= ~CSTOPB;
          break;
        case 2: /* 2 stop bits */
          options.c_cflag |= CSTOPB;
          break;
        case 3: /* 1.5 bit (nosupport) */ 
        default:
          //uM2("rs232cInit(); Com%d stopBit %d error!!\n", comNo, stopBit);
          return -6;
  }
  /* set the options */
  tcsetattr(fd[comNo], TCSANOW, &options);
  return 0;
}

void signal_handler_IO(int status){
  //uM("signal get\n");
  wait_flag=0;
}

int rs232cEnd(int comNo){
  //!DTR信号も自動的にLow(HangUp)になる
  close(fd[comNo]);
  return 0;
}

int rs232cWrite(int comNo, const unsigned char* buf, const unsigned int bufSize){
  int ret;
  ret = write(fd[comNo], buf, bufSize);
  //if(ret != bufSize){
  //  uM3("rs232cWrite(); Com%d writeSize %d != bufSize %d error!!\n", comNo, ret, bufSize);
  //}
  return ret;
}

int rs232cRead(int comNo, unsigned char* buf, const unsigned int bufSize){
  int ret;
  ret = read(fd[comNo], buf, bufSize);
  return ret;
}
#endif
