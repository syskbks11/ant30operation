#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include "penguin_heater.h"

#define IP_ADDRESS  "192.168.10.3" // for RPCOM device
#define MTR_BUFFSIZE 4096
#define HEADER 0x02
#define TRAILER 0x03

char tmpstr[MTR_BUFFSIZE];
char cmdWord[FIT_WORDSIZE];

penguin_heater_t* penguin_heater_init(char* ipaddress, const unsigned int deviceid){

  penguin_heater_t* ret = (penguin_heater_t*)malloc(sizeof(penguin_heater_t));
  ret->pctrl = (rpcomfitctrl*)malloc(sizeof(rpcomfitctrl));
  ret->pctrl->deviceid = deviceid;
  printf("RPCOM device %s (deviceid = %d) opened as a penguin_heater.\n", ipaddress, ret->pctrl->deviceid);

  SocketOpen(ret->pctrl, ipaddress);

  penguin_heater_fitopen(ret);

  int status = 0;
  status = penguin_heater_initconf(ret);
  if(status<0){
    printf("ERROR:: PenguinHeater failed to register initial configurations in penguin_heater_initconf(). \n");
  }

  return ret;
}

void penguin_heater_fitopen(penguin_heater_t* ppenhtr){
  FitClose(ppenhtr->pctrl);
  FitOpen(ppenhtr->pctrl, 9600, 0, 0, 1);
}

void penguin_heater_fitclose(penguin_heater_t* ppenhtr){
  FitClose(ppenhtr->pctrl);
}

int penguin_heater_initconf(penguin_heater_t* ppenhtr){
  int ret = 0;
  // setup
  return ret;
}

int penguin_heater_end(penguin_heater_t* ppenhtr){
  penguin_heater_fitclose(ppenhtr);
  free(ppenhtr->pctrl);
  ppenhtr->pctrl = NULL;
  return 0;
}

// ========== LOW LEVEL APIs ========== //

int penguin_heater_sendData(penguin_heater_t* ppenhtr, const unsigned int ch, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%c%.2dW%s%c",HEADER,ch,cmd,TRAILER);
  printf(cmdWord);
  printf('\n');
  return DataSend(ppenhtr->pctrl,cmdWord);
}

int penguin_heater_sendrecvData(penguin_heater_t* ppenhtr, const unsigned int ch, const char* const cmd, char* buffer){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%c%.2dR%s%c", HEADER,ch,cmd,TRAILER);
  int ret = DataSendReceive(ppenhtr->pctrl,cmdWord,buffer);
  printf(cmdWord);
  printf('\n');
  printf(buffer);
  printf('\n');
  return ret;
}

int penguin_heater_sendrecvData_int(penguin_heater_t* ppenhtr, const unsigned int ch, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%c%.2dR%s%c", HEADER,ch,cmd,TRAILER);
  char buffer[MTR_BUFFSIZE];
  int status = DataSendReceive(ppenhtr->pctrl,cmdWord,buffer);
  if(status!=0) return -999;
  return strtod(buffer+7, NULL);
}

double penguin_heater_sendrecvData_float(penguin_heater_t* ppenhtr, const unsigned int ch, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%c%.2dR%s%c", HEADER,ch,cmd,TRAILER);
  char buffer[MTR_BUFFSIZE];
  int status = DataSendReceive(ppenhtr->pctrl,cmdWord,buffer);
  if(status!=0) return -999;
  return strtod(buffer+7, NULL);
}

// ========== MIDDLE LEVEL APIs ========== //

double penguin_heater_get_temperature(penguin_heater_t* ppenhtr, const unsigned int channel){
  return penguin_heater_sendrecvData_float(ppenhtr, channel, "PV1")/10.0;
}


