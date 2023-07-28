#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <stdint.h>

#include "penguin_powermeter.h"

#define IP_ADDRESS  "192.168.10.3" // for RPCOM device
#define MTR_BUFFSIZE 4096

char tmpstr[MTR_BUFFSIZE];
char cmdWord[FIT_WORDSIZE];

penguin_powermeter_t* penguin_powermeter_init(char* ipaddress, const unsigned int deviceid){

  penguin_powermeter_t* ret = (penguin_powermeter_t*)malloc(sizeof(penguin_powermeter_t));
  ret->pctrl = (rpcomfitctrl*)malloc(sizeof(rpcomfitctrl));
  ret->pctrl->deviceid = deviceid;
  printf("RPCOM device %s (deviceid = %d) opened as a penguin_powermeter.\n", ipaddress, ret->pctrl->deviceid);

  SocketOpen(ret->pctrl, ipaddress);

  penguin_powermeter_fitopen(ret);

  int status = 0;
  status = penguin_powermeter_initconf(ret);
  if(status<0){
    printf("ERROR:: PenguinPowermeter failed to register initial configurations in penguin_powermeter_initconf(). \n");
  }

  return ret;
}

void penguin_powermeter_fitopen(penguin_powermeter_t* ppenpm){
  FitClose(ppenpm->pctrl);
  FitOpen(ppenpm->pctrl, 9600, 1, 1, 0);
}

void penguin_powermeter_fitclose(penguin_powermeter_t* ppenpm){
  FitClose(ppenpm->pctrl);
}

int penguin_powermeter_initconf(penguin_powermeter_t* ppenpm){
  int ret = 0;
  // setup
  return ret;
}

int penguin_powermeter_end(penguin_powermeter_t* ppenpm){
  penguin_powermeter_fitclose(ppenpm);
  free(ppenpm->pctrl);
  ppenpm->pctrl = NULL;
  return 0;
}

// ========== LOW LEVEL APIs ========== //

int penguin_powermeter_sendData(penguin_powermeter_t* ppenpm, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%s\r\n", cmd);
  char buffer[MTR_BUFFSIZE];
  //return DataSendReceiveVerbose(ppenpm->pctrl,cmdWord,buffer,1);
  return DataSendReceive(ppenpm->pctrl,cmdWord,buffer);
}

int penguin_powermeter_sendrecvData(penguin_powermeter_t* ppenpm, const char* const cmd, char* buffer){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%s\r\n", cmd);
  //return DataSendReceiveVerbose(ppenpm->pctrl,cmdWord,buffer,1);
  return DataSendReceive(ppenpm->pctrl,cmdWord,buffer);
}

// ========== MIDDLE LEVEL APIs ========== //

double penguin_powermeter_get_power(penguin_powermeter_t* ppenpm){
  char buffer[MTR_BUFFSIZE];
  memset(buffer, 0x00, strlen(buffer));
  int ret = 0;
  ret = penguin_powermeter_sendrecvData(ppenpm, "READ?", buffer);
  if(ret<0) return -999; // invalid data received

  return strtod(buffer, NULL);
}

