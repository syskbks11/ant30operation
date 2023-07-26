#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include "penguin_gauge.h"

#define IP_ADDRESS  "192.168.10.3" // for RPCOM device
#define MTR_BUFFSIZE 4096
#define ACK 0x06
#define ENQ 0x05

char tmpstr[MTR_BUFFSIZE];
char cmdWord[FIT_WORDSIZE];

penguin_gauge_t* penguin_gauge_init(char* ipaddress, const unsigned int deviceid){

  penguin_gauge_t* ret = (penguin_gauge_t*)malloc(sizeof(penguin_gauge_t));
  ret->pctrl = (rpcomfitctrl*)malloc(sizeof(rpcomfitctrl));
  ret->pctrl->deviceid = deviceid;
  printf("RPCOM device %s (deviceid = %d) opened as a penguin_gauge.\n", ipaddress, ret->pctrl->deviceid);

  SocketOpen(ret->pctrl, ipaddress);

  penguin_gauge_fitopen(ret);

  int status = 0;
  status = penguin_gauge_initconf(ret);
  if(status<0){
    printf("ERROR:: PenguinGauge failed to register initial configurations in penguin_gauge_initconf(). \n");
  }

  return ret;
}

void penguin_gauge_fitopen(penguin_gauge_t* ppengag){
  FitClose(ppengag->pctrl);
  FitOpen(ppengag->pctrl, 9600, 1, 1, 0);
}

void penguin_gauge_fitclose(penguin_gauge_t* ppengag){
  FitClose(ppengag->pctrl);
}

int penguin_gauge_initconf(penguin_gauge_t* ppengag){
  int ret = 0;
  // setup
  return ret;
}

int penguin_gauge_end(penguin_gauge_t* ppengag){
  penguin_gauge_fitclose(ppengag);
  free(ppengag->pctrl);
  ppengag->pctrl = NULL;
  return 0;
}

// ========== LOW LEVEL APIs ========== //

int penguin_gauge_sendData(penguin_gauge_t* ppengag, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%s\r\n", cmd);
  char buffer[MTR_BUFFSIZE];
  //return DataSendReceiveVerbose(ppengag->pctrl,cmdWord,buffer,1);
  return DataSendReceive(ppengag->pctrl,cmdWord,buffer);
}

int penguin_gauge_sendrecvData(penguin_gauge_t* ppengag, const char* const cmd, char* buffer){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%s\r\n", cmd);
  int ret = DataSendReceive(ppengag->pctrl,cmdWord,tmpstr);
  memset(buffer, 0x00, strlen(buffer));
  if(tmpstr[0] == ACK){
    memset(cmdWord, 0x00, strlen(cmdWord));
    sprintf(cmdWord,"%c\r\n", ENQ);
    ret = DataSendReceive(ppengag->pctrl,cmdWord,buffer);
  }
  else{
    ret = -1;
  }
  return ret;
}

// ========== MIDDLE LEVEL APIs ========== //

double penguin_gauge_get_pressure(penguin_gauge_t* ppengag){
  char buffer[MTR_BUFFSIZE];
  memset(buffer, 0x00, strlen(buffer));
  int ret = 0;
  ret = penguin_gauge_sendrecvData(ppengag, "PR1", buffer);
  if(ret<0) return -999; // invalid data received

  int status;
  double value;
  sscanf(tmpstr, "%d,%lf\r\n", &status, &value);
  switch(status){
  case 0:
    break;
  case 1:
    printf("ERROR:: PenguinGauge failed to get data: UnderRange");
    ret = -11;
    break;
  case 2:
    printf("ERROR:: PenguinGauge failed to get data: OverRange");
    ret = -12;
    break;
  case 3:
    printf("ERROR:: PenguinGauge failed to get data: SensorError");
    ret = -13;
    break;
  case 4:
    printf("ERROR:: PenguinGauge failed to get data: SensorOff");
    ret = -14;
    break;
  case 5:
    printf("ERROR:: PenguinGauge failed to get data: NoSensor");
    ret = -15;
    break;
  case 6:
    printf("ERROR:: PenguinGauge failed to get data: IdentificationError");
    ret = -16;
    break;
  }
  if(ret<0) return -999; // invalid data received
  return value;
}

