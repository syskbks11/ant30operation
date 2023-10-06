#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "usb220_gpib.h"

#include "../libtkb/export/libtkb.h"
#include "penguin_powermeter.h"

#define _USE_U2
#define MAX_USB_DEVICES	256


int fd=0;
short MyGpibAdrs = 1;	// GPIB address for USB-GPIB
char GpAdrsBuf[12];	// GPIB address for target-GPIB
char szBuf[256];
char szCmd[20];

int InitFlag;
int g_tmout;
int g_eoi;
int g_delm;
int g_filelen;

/*
  static void CmdRxData()
  {
  int		GpStatus;
  char	RxBuf[1024];
  int		RxLen;

  printf("%s\n", "Now receiving...");

  RxLen = sizeof(RxBuf);
  memset(RxBuf, 0x00, RxLen);
  GpStatus = U2Gpred( fd, GpAdrsBuf, RxBuf, RxLen );
  if(GpStatus != 0)
  {
  printf("gp_red() error [%d]\n", GpStatus);
  }
  else
  {
  printf("%s\n", "Completed.");
  // 受信データ表示
  printf("%s\n", RxBuf);
  }

  RxLen = gp_count();
  printf("%d bytes received.\n", RxLen);
  }
*/
/*
  static int CmdTxCommand(const char* cmd)
  {
  int	GpStatus;
  char TxBuf[1024];
  int TxLen;
  //char *buff;
  int	ret;

  printf("%s\n", "Now sending...");

  memset(TxBuf, 0x00, sizeof(TxBuf));
  sprintf(TxBuf, "%s", cmd);

  printf("CmdTxCommand [%s]\n", TxBuf);
  GpStatus = U2Gpwrt( fd, GpAdrsBuf, TxBuf );
  if(GpStatus != 0)
  {
  printf("gp_wrt() error [%d]\n", GpStatus);
  ret = 1;
  }
  else
  {
  printf("%s\n", "Completed.");
  ret = 0;
  }
  TxLen = gp_count();
  printf("%d bytes send.\n", TxLen);

  return ret;
  }
*/
/*
  int init(){
  int GpStatus = U2Gpinit( fd, MyGpibAdrs );
  if( GpStatus != 0 )
  {
  printf("gp_init() error [%d]\n", GpStatus);
  return 1;
  }

  // enable IFC
  GpStatus = U2Gpcli(fd);
  if ( GpStatus != 0 )
  {
  printf("gp_cli() error [%d]\n", GpStatus);
  return 1;
  }

  // enable REN
  GpStatus = U2Gpren(fd);
  if ( GpStatus != 0 )
  {
  printf("gp_ren() error [%d]\n", GpStatus);
  return 1;
  }

  // Send SDC command
  // get address of GPIBdevice
  if(GpAdrsBuf[0] != 0){
  GpStatus = U2Gpclr(fd, GpAdrsBuf);
  }else{
  GpStatus = U2Gpclr(fd, "");
  }
  if ( GpStatus != 0 )
  {
  printf("gp_clr() error [%d]\n", GpStatus);
  return 1;
  }
  InitFlag = 1;
  return 0;
  }
*/
int main(int argc, char *argv[]) {
  uInit("powermeterTest");
  int ret = penguin_powermeter_init(13);
  if(ret){
    printf("%s\n", "penguin_powermeter_init() failed. ");
    return 1;
  }
  /*
    int id, i;
    unsigned short UnitId[MAX_USB_DEVICES];
    unsigned short MaxUnit = MAX_USB_DEVICES;

    /* copy from gpibcntrl.c: Initialization procedure
    MaxUnit = U2GpEnumUnit(UnitId, MaxUnit);
    for(i=0;i<MaxUnit;i++){
    printf("U2GpEnumUnit%d UnitId = %02x \n", i, UnitId[i]);
    }
    //最初に見つけたUSB220をオープン
    fd=U2GpOpenUnit(UnitId[0]);
    if(fd<=0){
    printf("Device not opened \n");
    return 0;
    }
    InitFlag = 0;
    g_tmout = 10;
    g_eoi = 0x0;
    g_delm = 0x0a;
    gp_GetDeviceID(fd, 0, (char*)&MyGpibAdrs);
  */
  /* main part */
  //Settings
  penguin_CmdTxCommand("*IDN?");
  penguin_CmdRxData();
  penguin_CmdTxCommand("SYST:RINT?");
  penguin_CmdRxData();
  penguin_CmdTxCommand("SYST:COMM:GPIB:ADDR?");
  penguin_CmdRxData();

  //Measurement
  penguin_CmdTxCommand("*RST");
  //	penguin_CmdTxCommand("READ?");//no
  //	penguin_CmdTxCommand("MEAS?");//no
  //	penguin_CmdTxCommand("MEAS:POW:AC?");
  //	penguin_CmdRxData();

  penguin_powermeter_freerun();
  double value;
  int i;
  for(i = 0; i < 40; i++){
    value = penguin_powermeter_freerun_getData();
    uM2("%d: %lf", i, value);
    //		printf("%d: %lf\n", i, value);
  }
  /*
    init();
    sprintf(GpAdrsBuf, "%d", 13);
    CmdTxCommand("*IDN?");
    CmdRxData();
  */
  /*
    gtk_init (&argc, &argv);

    create_window();

    gtk_main ();
  */
  /* copy from gpibcntrl.c: End procedure*/
  ret = penguin_powermeter_end();
  if(ret){
    printf("%s\n", "penguin_powermeter_end() failed. ");
    return 1;
  }
  /*
    gp_detach(fd);
  */
  uEnd();
  return 0;

}
