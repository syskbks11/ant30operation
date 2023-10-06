#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <stdint.h>

#include "usb220_gpib.h"

#include "penguin_powermeter.h"

#define BUF_LEN 100000
#define MAX_USB_DEVICES	256

/*
 * singleton, since we have only one chopper motor.
 */
typedef struct penguin_powermeter_s{
  int id;
  int InitFlag;
  int tmout;
  int eoi;
  int delm;
  unsigned short MyGpibAdrs;
  //	short MyGpibAdrs;
  char GpAdrsBuf[12];
  char message[1024];
}penguin_powermeter_t;

static penguin_powermeter_t p;
// static short MyGpibAdrs = 1;
// static int fd;

static int _init(int fd);
static int _receive(int id);
//static int _send_receive(int id, char* buf, const unsigned int bufSize);

/*
  static int _send_receive(const char* cmd, int length);
  static void _buffered_write(int id, const char* buf, const unsigned int size);
  static int _send_frame(char const* cmd, int length);
  static int _receive_more(int id, int length);
  static int _isMessageCompleted();
*/

int penguin_powermeter_init(int cliantGPIBAddress){
  int i;
  unsigned short UnitId[MAX_USB_DEVICES];
  unsigned short MaxUnit = MAX_USB_DEVICES;

  /* copy from gpibcntrl.c: Initialization procedure*/
  MaxUnit = U2GpEnumUnit(UnitId, MaxUnit);
  for(i=0;i<MaxUnit;i++){
    printf("U2GpEnumUnit%d UnitId = %02x \n", i, UnitId[i]);
  }
  //最初に見つけたUSB220をオープン
  int fd = U2GpOpenUnit(UnitId[0]);
  if(fd<=0){
    printf("Device not opened \n");
    return 1;
  }
  p.InitFlag = 0;
  p.tmout = 10;
  p.eoi = 0x0;
  p.delm = 0x0a;
  memset(p.GpAdrsBuf, 0, sizeof(p.GpAdrsBuf));
  /*
    gp_GetDeviceID(fd, 0, (char*)&MyGpibAdrs);
  */

  p.MyGpibAdrs = 1;
  gp_GetDeviceID(fd, 0, (char*)&p.MyGpibAdrs);
  /* main part */	
  int ret = _init(fd);//090828 in
  if(ret){
    printf("Error in GPIB communication.\n");
    return 2;
  }
  sprintf(p.GpAdrsBuf, "13");
  //	sprintf(p.GpAdrsBuf, "%d\0", cliantGPIBAddress);

  p.id = fd;
  memset(p.message, 0, 256);
  printf("penguin_powermeter initialized successfully.\n");
  return 0;
}

int penguin_powermeter_end(){
  U2GpCloseUnit(p.id);
  //	gp_detach(p.id);
  return 0;
}

int penguin_powermeter_freerun(){
  int ret;
  ret = penguin_CmdTxCommand("INIT:CONT ON");
  if(ret){
    printf("%s\n", "penguin_powermeter_freerun(): \"INIT:CONT ON\" failed.");
    return ret;
  }
  ret = penguin_CmdTxCommand("TRIG:SOUR IMM");
  if(ret){
    printf("%s\n", "penguin_powermeter_freerun(): \"TRIG:SOUR IMM\" failed.");
    return ret;
  }
  return 0;//normal end
}

double penguin_powermeter_freerun_getData(){
  int ret;
  ret = penguin_CmdTxCommand("FETC?");
  if(ret){
    printf("%s\n", "penguin_powermeter_freerun_getData(): \"FETC?\" failed.");
    return ret;
  }
  ret = penguin_CmdRxData();
  if(ret == 0){
    printf("%s\n", "penguin_powermeter_freerun_getData(): no data received.");
    return -1;
  }
  double res;
  ret = sscanf(p.message, "%lf", &res);
  if(ret <= 0){
    printf("penguin_powermeter_freerun_getData(): received data (%s) is not a number.\n", p.message);
    return -1;
  }
  return res;
}

int penguin_CmdTxCommand(const char* cmd)
{
  int	GpStatus;
  char TxBuf[1024];
  int TxLen;
  //char *buff;
  int	ret;

  //printf("%s\n", "Now sending...");

  memset(TxBuf, 0x00, sizeof(TxBuf));
  sprintf(TxBuf, "%s", cmd);

  //printf("CmdTxCommand [%s]\n", TxBuf);
  GpStatus = U2Gpwrt( p.id, p.GpAdrsBuf, TxBuf );
  if(GpStatus != 0)
    {
      printf("gp_wrt() error [%d]\n", GpStatus);
      ret = 1;
    }
  else
    {
      //printf("%s\n", "Completed.");
      ret = 0;
    }
  TxLen = gp_count();
  //printf("%d bytes send.\n", TxLen);

  return ret;
}

int penguin_CmdRxData()
{
  int		GpStatus;
  int		RxLen;

  //printf("%s\n", "Now receiving...");

  RxLen = sizeof(p.message);
  //printf("RxLen = %d\n", RxLen);
  //memset(p.message, 0, RxLen);
  GpStatus = U2Gpred( p.id, p.GpAdrsBuf, p.message, RxLen );
  if(GpStatus != 0)
    {
      printf("gp_red() error [%d]\n", GpStatus);
    }
  else
    {
      //printf("%s\n", "Completed.");
    }
  RxLen = gp_count();
  //printf("%d bytes received.\n", RxLen);
  p.message[RxLen] = '\0';//It seems necessary because of U2Gpred(). 
  // 受信データ表示
  //printf("%s\n", p.message);

  return RxLen;
}

char* penguin_chopper_getMessage(){
  return p.message;
}


int _init(int fd){
  int GpStatus = U2Gpinit( fd, p.MyGpibAdrs );
  /*
    int GpStatus = U2Gpinit( p.id, p.MyGpibAdrs );
  */
  if( GpStatus != 0 )
    {
      printf("gp_init() error [%d]\n", GpStatus);
      return 1;
    }

  // enable IFC
  GpStatus = U2Gpcli(fd);
  /*
    GpStatus = U2Gpcli(p.id);
  */
  if ( GpStatus != 0 )
    {
      printf("gp_cli() error [%d]\n", GpStatus);
      return 1;
    }

  // enable REN
  GpStatus = U2Gpren(fd);
  /*
    GpStatus = U2Gpren(p.id);
  */
  if ( GpStatus != 0 )
    {
      printf("gp_ren() error [%d]\n", GpStatus);
      return 1;
    }

  // Send SDC command
  // get address of GPIBdevice
  if(p.GpAdrsBuf[0] != 0){
    GpStatus = U2Gpclr(fd, p.GpAdrsBuf);
    /*
      GpStatus = U2Gpclr(p.id, p.GpAdrsBuf);
    */
  }else{
    GpStatus = U2Gpclr(fd, "");
    /*
      GpStatus = U2Gpclr(p.id, "");
    */
  }
  if ( GpStatus != 0 )
    {
      printf("gp_clr() error [%d]\n", GpStatus);
      return 1;
    }

  // check register values
  unsigned short reg;
  unsigned short i;
  for(i = 0; i < 8; i++){
    reg = U2InPort(fd, i);
    printf("reg %d: %d\n", i, reg);
  }

  p.InitFlag = 1;
  return 0;
}

