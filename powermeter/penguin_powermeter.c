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

  p.InitFlag = 0;
  p.tmout = 10;
  p.eoi = 0x0;
  p.delm = 0x0a;
  p.MyGpibAdrs = 1;
  sprintf(p.GpAdrsBuf, "13");

  p.id = 0;
  memset(p.message, 0, 256);

  return 0;
}

int penguin_powermeter_end(){
  //U2GpCloseUnit(p.id);
  //	gp_detach(p.id);
  return 0;
}

int penguin_powermeter_freerun(){
  return 0;//normal end
}

double penguin_powermeter_freerun_getData(){
  return -99.0;
}

int penguin_CmdTxCommand(const char* cmd)
{
  return 0;
}

int penguin_CmdRxData()
{
  return 1;
}

char* penguin_chopper_getMessage(){
  return p.message;
}

int _init(int fd){
  return 0;
}

