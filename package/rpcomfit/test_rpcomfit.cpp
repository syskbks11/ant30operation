#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h> // sockaddr_in
#include <fcntl.h>
#include <ctype.h>

#include "rpcomfit.h"

int main(int argc,char *argv[]){
  printf("RPCOMFIT test program\n");

  if(argc != 7){
    printf("Invalid format:\n --> %s [IP-ADDRESS:192.168.11.3] [DEVICE-ID:1-7] [BAUDRATE] [DATA_BIT] [PARITY_BIT] [STOP_BIT]\n",argv[0]);
    exit(0);
  }

  int devid =  atoi(argv[2]);
  int baudrate = atoi(argv[3]);
  int databit = atoi(argv[4]);
  int paritybit = atoi(argv[5]);
  int stopbit = atoi(argv[6]);
  rpcomfitctrl* pctrl = (rpcomfitctrl*)malloc(sizeof(rpcomfitctrl));
  pctrl->deviceid = devid;
  SocketOpen(pctrl, argv[1]);

  int cont = 1;
  char tmpstr[64];
  char cmdWord[128];
  char buffer[1024];
  do{
    memset(tmpstr, 0x00, strlen(tmpstr));
    printf("[RPCOMFIT%1d] ",devid);
    fflush(stdout);
    fgets(tmpstr,64,stdin);
    // if(strlen(tmpstr)==0) continue;
    memset(cmdWord, 0x00, strlen(cmdWord));
    sprintf(cmdWord,"%s\r\n",tmpstr);
    FitOpen(pctrl, baudrate, databit, paritybit, stopbit);
    memset(buffer, 0x00, strlen(buffer));
    DataSendReceiveVerbose(pctrl,tmpstr,buffer,1);
  }while(cont);

  return 0;
}
