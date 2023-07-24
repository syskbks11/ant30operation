#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include "penguin_lakeshore.h"

#define IP_ADDRESS  "192.168.10.3" // for RPCOM device
#define MTR_BUFFSIZE 4096

char tmpstr[MTR_BUFFSIZE];
char cmdWord[FIT_WORDSIZE];

penguin_lakeshore_t* penguin_lakeshore_init(char* ipaddress, const unsigned int deviceid){

  penguin_lakeshore_t* ret = (penguin_lakeshore_t*)malloc(sizeof(penguin_lakeshore_t));
  ret->pctrl = (rpcomfitctrl*)malloc(sizeof(rpcomfitctrl));
  ret->pctrl->deviceid = deviceid;
  printf("RPCOM device %s (deviceid = %d) opened as a penguin_lakeshore.\n", ipaddress, ret->pctrl->deviceid);

  SocketOpen(ret->pctrl, ipaddress);

  penguin_lakeshore_fitopen(ret);

  int status = 0;
  status = penguin_lakeshore_initconf(ret);
  if(status<0){
    printf("ERROR:: PenguinLakeshore failed to register initial configurations in penguin_lakeshore_initconf(). \n");
  }

  return ret;
}

void penguin_lakeshore_fitopen(penguin_lakeshore_t* ppenlks){
  FitClose(ppenlks->pctrl);
  FitOpen(ppenlks->pctrl, 9600, 1, 1, 0);
}

void penguin_lakeshore_fitclose(penguin_lakeshore_t* ppenlks){
  FitClose(ppenlks->pctrl);
}

int penguin_lakeshore_initconf(penguin_lakeshore_t* ppenlks){

  int ret = 0;
  // setup
  return ret;
}

int penguin_lakeshore_end(penguin_lakeshore_t* ppenlks){
  penguin_lakeshore_fitclose(ppenlks);
  free(ppenlks->pctrl);
  ppenlks->pctrl = NULL;
  return 0;
}

// ========== LOW LEVEL APIs ========== //

int penguin_lakeshore_sendData(penguin_lakeshore_t* ppenlks, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%s\r\n", cmd);
  char buffer[MTR_BUFFSIZE];
  //return DataSendReceiveVerbose(ppenlks->pctrl,cmdWord,buffer,1);
  return DataSendReceive(ppenlks->pctrl,cmdWord,buffer);
}

int penguin_lakeshore_sendrecvData(penguin_lakeshore_t* ppenlks, const char* const cmd, char* buffer){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%s\r\n", cmd);
  int ret = DataSendReceive(ppenlks->pctrl,cmdWord,buffer);
  //int ret = DataSendReceiveVerbose(ppenlks->pctrl,cmdWord,buffer,1);
  // if(strncmp(buffer,"ERR",3)==0){
  //   print_err(buffer+4);
  // }
  return ret;
}

int penguin_lakeshore_sendrecvData_int(penguin_lakeshore_t* ppenlks, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%s\r\n", cmd);
  char buffer[MTR_BUFFSIZE];
  int status = DataSendReceive(ppenlks->pctrl,cmdWord,buffer);
  //int status = DataSendReceiveVerbose(ppenlks->pctrl,cmdWord,buffer,1);
  // if(strncmp(buffer,"ERR",3)==0){
  //   print_err(buffer+4);
  // }
  if(status!=0) return -999;
  return strtod(buffer, NULL);
}

double penguin_lakeshore_sendrecvData_float(penguin_lakeshore_t* ppenlks, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%s\r\n", cmd);
  char buffer[MTR_BUFFSIZE];
  int status = DataSendReceive(ppenlks->pctrl,cmdWord,buffer);
  if(status!=0) return -999;
  return strtod(buffer, NULL);
}

// ========== MIDDLE LEVEL APIs ========== //

int penguin_lakeshore_get_identification(penguin_lakeshore_t* ppenlks, char* buffer){
  return penguin_lakeshore_sendrecvData(ppenlks, "*IDN?", buffer);
}

double penguin_lakeshore_get_temperature(penguin_lakeshore_t* ppenlks, const unsigned int channel){
  memset(tmpstr, 0x00, MTR_BUFFSIZE);
  sprintf(tmpstr, "KRDG? %d", channel);
  return penguin_lakeshore_sendrecvData_float(ppenlks, tmpstr);
}

