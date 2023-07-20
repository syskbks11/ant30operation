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
#include <stdint.h> // for uint8_t
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_addr

#include "penguin_motor.h"
#include "rpcomfit.h"
#include "fitprot.h"

#define IP_ADDRESS  "192.168.10.3" // for RPCOM device
#define MTR_BUFFSIZE 4096
#define SEP 0x3b
#define HEADER 0x02
#define TRAILER 0x04

char tmpstr[MTR_BUFFSIZE];
char cmdWord[FIT_WORDSIZE];

// initialization for FitProt
penguin_motor_t* penguin_motor_init(char* ipaddress, const unsigned int deviceid, int positiveSoftLimit, int negativeSoftLimit){

  penguin_motor_t* ret = (penguin_motor_t*)malloc(sizeof(penguin_motor_t));
  ret->pctrl = (rpcomfitctrl*)malloc(sizeof(rpcomfitctrl));
  ret->pctrl->deviceid = deviceid;
  printf("RPCOM device %s (deviceid = %d) opened as a penguin_motor.\n", ipaddress, ret->pctrl->deviceid);

  SocketOpen(ret->pctrl, ipaddress);

  ret->positiveSoftLimit = positiveSoftLimit;
  ret->negativeSoftLimit = negativeSoftLimit;

  penguin_motor_fitopen(ret);

  int status = 0;
  status = penguin_motor_initconf(ret);
  if(status<0){
    printf("ERROR:: PenguinMotor failed to register initial configurations in penguin_motor_initconf(). \n");
  }

  return ret;

}

void penguin_motor_fitopen(penguin_motor_t* ppenmtr){
  FitClose(ppenmtr->pctrl);
  FitOpen(ppenmtr->pctrl, 9600, 0, 2, 0);
}

void penguin_motor_fitclose(penguin_motor_t* ppenmtr){
  FitClose(ppenmtr->pctrl);
}

int penguin_motor_initconf(penguin_motor_t* ppenmtr){

  int ret = 0;
  //ret += penguin_motor_setParameter(ppenmtr, 47, 8);
  //ret += penguin_motor_setParameter(ppenmtr, 69, 0x0055);
  //ret += penguin_motor_setParameter(ppenmtr, 67, 0x0006);
  //ret += penguin_motor_setParameter(ppenmtr, 50, 0x0007);

  penguin_motor_setVelocity(ppenmtr,10);
  ret += penguin_motor_servOn(ppenmtr);

  return ret;
}

int penguin_motor_end(penguin_motor_t* ppenmtr){
  penguin_motor_servOff(ppenmtr);
  penguin_motor_fitclose(ppenmtr);
  free(ppenmtr->pctrl);
  ppenmtr->pctrl = NULL;
  return 0;
}

// ========== MISC. FUNCTIONS ========== //

void print_err(const char* const err){
  if(strlen(err)==0) return;
  printf("Error returned with code = 0x%s: ", err);
  int errn = strtod(err, NULL);
  if(errn==1)      printf("invalid command");
  else if(errn==3) printf("out of motor limit range");
  else if(errn==4) printf("command not acceptable");
  else if(errn==6) printf("reset failed (\"RESET\" or \"$$$\" called with servo-on)");
  else if(errn==7) printf("servo-motor ON failed (\"SVON\" called with alarm)");
  else if(errn==11)printf("invalid data format");
  else if(errn==13)printf("impossible command to run (e.g. command to move the motor without \"SVON\")");
  else             printf("unknown error (%d) ... ",errn);
  printf("\n");
  return;
}

void print_alarm(const int ecrs){
  if(ecrs==0) return;
  printv("       H-------------L");
  printv_withbinary("ALARM = ",ecrs,13);
  if(ecrs&1)        printf("- EEPROM error -> configuration failure\n");
  else if(ecrs&2)   printf("- overload (>2.4A on motor for some period time)\n");
  else if(ecrs&4)   printf("- PG error -> encoder failure\n");
  else if(ecrs&8)   printf("- regeneration error (over-voltage) -> power failure / too much acceleration\n");
  else if(ecrs&16)  printf("- too high temperature (>90degC)\n");
  else if(ecrs&32)  printf("- system error -> noise / motor moving when powered-on\n");
  else if(ecrs&64)  printf("- over-current -> damages on motor operation lines\n");
  else if(ecrs&128) printf("- too large deviations -> motor stacked / too much moment of inertia\n");
  else if(ecrs&256) printf("- too much velocity (> nominal + 10r/min)\n");
  else if(ecrs&512) printf("- IPM error -> motor lines shorted\n");
  else if(ecrs&1024)printf("- emergency stop\n");
  else if(ecrs&2048)printf("- communication error -> wrong line assignments\n");
  else if(ecrs&4096)printf("- over travel -> limit switch enabled\n");
  return;
}

// ========== HIGH LEVEL APIs ========== //

int penguin_motor_checkDynamicBreakState(penguin_motor_t* ppenmtr){
  printf("penguin_motor_checkDynamicBreakState()\n");
  int status = penguin_motor_getDynamicBreak(ppenmtr);
  if(status==0) printv("ON WHEN POWERED-ON and ALWAYS OFF ONCE SURVO-ON. [0]");
  else if(status==1) printv("ON WHEN POWERED-ON and ALARMED. [1]");
  else if(status==2) printv("ON WHEN POWERED-ON and SURVO-OFF. [2]");
  return 0;
}

int penguin_motor_checkIOState(penguin_motor_t* ppenmtr){
  printf("penguin_motor_checkIOState()\n");
  int ecrs = penguin_motor_getIOState(ppenmtr);
  printv("                               H---------------L");
  printv_withbinary("IOST_OUT = ",(ecrs>> 0),8);
  printv_withbinary("IOST_IN  = ",(ecrs>>16),15);
  if((ecrs>> 0) & 1)   printv("- OUT: ServOn");
  if((ecrs>> 0) & 2)   printv("- OUT: Inposition");
  if((ecrs>> 0) & 4)   printv("- OUT: Alarm");
  if((ecrs>> 0) & 8)   printv("- OUT: InProgress");
  if((ecrs>> 0) & 16)  printv("- OUT: Ready");
  if((ecrs>> 0) & 32)  printv("- OUT: OriginDetected");
  if((ecrs>> 0) & 64)  printv("- OUT: ZsigEnabled");
  if((ecrs>> 0) & 128) printv("- OUT: ResolutionSelected");
  if((ecrs>>16) & 2)   printv("- IN : PositiveLimitEnabled");
  if((ecrs>>16) & 4)   printv("- IN : NegativeLimitEnabled");
  return 0;
}

int penguin_motor_checkErrorCorrecState(penguin_motor_t* ppenmtr){
  printf("penguin_motor_checkErrorCorrecState()\n");
  int ecrs = penguin_motor_getErrorCorrecState(ppenmtr);
  int res = 0;
  if(ecrs == 0){
    printv("penguin_motor_checkErrorCorrecState() POSITION NOT CORRECTED");
    res = 1;
  }
  else if(ecrs == 1){
    res = 0;
  }
  else{
    printv("penguin_motor_checkErrorCorrectionState() INVALID STATUS");
    res = -1;
  }
  return res;
}

int penguin_motor_checkAlarmState(penguin_motor_t* ppenmtr){
  printf("penguin_motor_checkAlarmState()\n");
  int ecrs = penguin_motor_getAlarmState(ppenmtr);
  if(ecrs != 0){
    printv("!!!ALARM ISSUED!!! penguin_motor_checkAlarmState()");
    printv("Check the motor dirver, consulting with its manual!");
    print_alarm(ecrs);
  }
  return ecrs;
}

int penguin_motor_checkParameter(penguin_motor_t* ppenmtr){
  printf("PARAM #%02d = %d \n", 0,penguin_motor_getParameter(ppenmtr, 0));
  printf("PARAM #%02d = %d \n", 1,penguin_motor_getParameter(ppenmtr, 1));
  printf("PARAM #%02d = %d \n", 2,penguin_motor_getParameter(ppenmtr, 2));
  printf("PARAM #%02d = %d \n", 3,penguin_motor_getParameter(ppenmtr, 3));
  printf("PARAM #%02d = %d \n", 4,penguin_motor_getParameter(ppenmtr, 4));
  printf("PARAM #%02d = %d \n", 6,penguin_motor_getParameter(ppenmtr, 6));
  printf("PARAM #%02d = %d \n", 7,penguin_motor_getParameter(ppenmtr, 7));
  printf("PARAM #%02d = %d \n", 9,penguin_motor_getParameter(ppenmtr, 9));
  printf("PARAM #%02d = %d \n",10,penguin_motor_getParameter(ppenmtr,10));
  printf("PARAM #%02d = %d \n",11,penguin_motor_getParameter(ppenmtr,11));
  printf("PARAM #%02d = %d \n",12,penguin_motor_getParameter(ppenmtr,12));
  printf("PARAM #%02d = %d \n",13,penguin_motor_getParameter(ppenmtr,13));
  printf("PARAM #%02d = %d \n",15,penguin_motor_getParameter(ppenmtr,15));
  printf("PARAM #%02d = %d \n",16,penguin_motor_getParameter(ppenmtr,16));
  printf("PARAM #%02d = %d \n",17,penguin_motor_getParameter(ppenmtr,17));
  printf("PARAM #%02d = %d \n",18,penguin_motor_getParameter(ppenmtr,18));
  printf("PARAM #%02d = %d \n",19,penguin_motor_getParameter(ppenmtr,19));
  printf("PARAM #%02d = %d \n",20,penguin_motor_getParameter(ppenmtr,20));
  printf("PARAM #%02d = %d \n",21,penguin_motor_getParameter(ppenmtr,21));
  printf("PARAM #%02d = %d \n",22,penguin_motor_getParameter(ppenmtr,22));
  printf("PARAM #%02d = %d \n",23,penguin_motor_getParameter(ppenmtr,23));
  printf("PARAM #%02d = %d \n",24,penguin_motor_getParameter(ppenmtr,24));
  printf("PARAM #%02d = %d \n",30,penguin_motor_getParameter(ppenmtr,30));
  printf("PARAM #%02d = %d \n",31,penguin_motor_getParameter(ppenmtr,31));
  printf("PARAM #%02d = %d \n",32,penguin_motor_getParameter(ppenmtr,32));
  printf("PARAM #%02d = %d \n",33,penguin_motor_getParameter(ppenmtr,33));
  printf("PARAM #%02d = %d \n",35,penguin_motor_getParameter(ppenmtr,35));
  printf("PARAM #%02d = %d \n",36,penguin_motor_getParameter(ppenmtr,36));
  printf("PARAM #%02d = %d \n",37,penguin_motor_getParameter(ppenmtr,37));
  printf("PARAM #%02d = %d \n",38,penguin_motor_getParameter(ppenmtr,38));
  printf("PARAM #%02d = %d \n",40,penguin_motor_getParameter(ppenmtr,40));
  printf("PARAM #%02d = %d \n",41,penguin_motor_getParameter(ppenmtr,41));
  printf("PARAM #%02d = %d \n",42,penguin_motor_getParameter(ppenmtr,42));
  printf("PARAM #%02d = %d \n",43,penguin_motor_getParameter(ppenmtr,43));
  printf("PARAM #%02d = %d \n",45,penguin_motor_getParameter(ppenmtr,45));
  printf("PARAM #%02d = %d \n",46,penguin_motor_getParameter(ppenmtr,46));
  printf("PARAM #%02d = %d \n",47,penguin_motor_getParameter(ppenmtr,47));
  printf("PARAM #%02d = %d \n",50,penguin_motor_getParameter(ppenmtr,50));
  printf("PARAM #%02d = %d \n",51,penguin_motor_getParameter(ppenmtr,51));
  printf("PARAM #%02d = %d \n",52,penguin_motor_getParameter(ppenmtr,52));
  printf("PARAM #%02d = %d \n",54,penguin_motor_getParameter(ppenmtr,54));
  printf("PARAM #%02d = %d \n",55,penguin_motor_getParameter(ppenmtr,55));
  printf("PARAM #%02d = %d \n",56,penguin_motor_getParameter(ppenmtr,56));
  printf("PARAM #%02d = %d \n",57,penguin_motor_getParameter(ppenmtr,57));
  printf("PARAM #%02d = %d \n",60,penguin_motor_getParameter(ppenmtr,60));
  printf("PARAM #%02d = %d \n",61,penguin_motor_getParameter(ppenmtr,61));
  printf("PARAM #%02d = %d \n",62,penguin_motor_getParameter(ppenmtr,62));
  printf("PARAM #%02d = %d \n",63,penguin_motor_getParameter(ppenmtr,63));
  printf("PARAM #%02d = %d \n",64,penguin_motor_getParameter(ppenmtr,64));
  printf("PARAM #%02d = %d \n",65,penguin_motor_getParameter(ppenmtr,65));
  printf("PARAM #%02d = %d \n",66,penguin_motor_getParameter(ppenmtr,66));
  printf("PARAM #%02d = %d \n",67,penguin_motor_getParameter(ppenmtr,67));
  printf("PARAM #%02d = %d \n",68,penguin_motor_getParameter(ppenmtr,68));
  printf("PARAM #%02d = %d \n",69,penguin_motor_getParameter(ppenmtr,69));
  printf("PARAM #%02d = %d \n",70,penguin_motor_getParameter(ppenmtr,70));
  printf("PARAM #%02d = %d \n",75,penguin_motor_getParameter(ppenmtr,75));
  return 0;
}

int penguin_motor_sendData(penguin_motor_t* ppenmtr, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%c%s%c", HEADER,cmd,TRAILER);
  //DataSend(ppenmtr->pctrl,cmdWord);
  char buffer[MTR_BUFFSIZE];
  //DataSendReceiveVerbose(ppenmtr->pctrl,cmdWord,buffer,1);
  return DataSendReceive(ppenmtr->pctrl,cmdWord,buffer);
}

int penguin_motor_sendrecvData(penguin_motor_t* ppenmtr, const char* const cmd, char* buffer){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%c%s%c", HEADER,cmd,TRAILER);
  int ret = DataSendReceive(ppenmtr->pctrl,cmdWord,buffer);
  //int ret = DataSendReceiveVerbose(ppenmtr->pctrl,cmdWord,buffer,1);
  if(strncmp(buffer,"ERR",3)==0){
    print_err(buffer+4);
  }
  return ret;
  //return strlen(buffer);
}

int penguin_motor_sendrecvData_int(penguin_motor_t* ppenmtr, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%c%s%c", HEADER,cmd,TRAILER);
  char buffer[MTR_BUFFSIZE];
  int status = DataSendReceive(ppenmtr->pctrl,cmdWord,buffer);
  //int status = DataSendReceiveVerbose(ppenmtr->pctrl,cmdWord,buffer,1);
  if(strncmp(buffer,"ERR",3)==0){
    print_err(buffer+4);
  }
  if(status!=0) return -999;
  return strtod(buffer+strlen(cmdWord), NULL);
}

int penguin_motor_sendrecvData_hexint(penguin_motor_t* ppenmtr, const char* const cmd){
  memset(cmdWord, 0x00, strlen(cmdWord));
  sprintf(cmdWord,"%c%s%c", HEADER,cmd,TRAILER);
  char buffer[MTR_BUFFSIZE];
  int status = DataSendReceive(ppenmtr->pctrl,cmdWord,buffer);
  if(strncmp(buffer,"ERR",3)==0){
    print_err(buffer+4);
  }
  //int status = DataSendReceiveVerbose(ppenmtr->pctrl,cmdWord,buffer,1);
  if(status!=0) return -999;
  char hexbuffer[MTR_BUFFSIZE];
  sprintf(hexbuffer,"0x%s",buffer+strlen(cmdWord));
  return strtod(hexbuffer, NULL);
}

void penguin_motor_setPulse(penguin_motor_t* ppenmtr, int value){
  if(value > ppenmtr->positiveSoftLimit)
    penguin_motor_setCommand(ppenmtr,"P",ppenmtr->positiveSoftLimit);
  else if(value < ppenmtr->negativeSoftLimit)
    penguin_motor_setCommand(ppenmtr,"P",ppenmtr->negativeSoftLimit);
  else
    penguin_motor_setCommand(ppenmtr,"P",value);
}

int penguin_motor_servOn(penguin_motor_t* ppenmtr){
  int status = 0;
  status = penguin_motor_sendData(ppenmtr,"SVON");
  unsigned int len = 0;
  while(status==0 && len<100){ // wait to survo-on for <10sec
    if((penguin_motor_isServOn(ppenmtr)==1) && (penguin_motor_isReady(ppenmtr)==1)) break;
    usleep(100*1000);
    len++;
  }
  if(len>=100) return -1;
  return 0;
}

int penguin_motor_servOff(penguin_motor_t* ppenmtr){
  int status = 0;
  status = penguin_motor_sendData(ppenmtr,"SVOFF");
  unsigned int len = 0;
  while(status==0 && len<100){ // wait to survo-on for <10sec
    if((penguin_motor_isServOn(ppenmtr)==0) && (penguin_motor_isReady(ppenmtr)==1)) break;
    usleep(100*1000);
    len++;
  }
  if(len>=100) return -1;
  return len;
}

void penguin_motor_setCommand(penguin_motor_t* ppenmtr, const char* const cmd, const int value){
  memset(tmpstr, 0x00, MTR_BUFFSIZE);
  sprintf(tmpstr, "%s=%d", cmd, value);
  penguin_motor_sendData(ppenmtr, tmpstr);
}

int penguin_motor_setParameter(penguin_motor_t* ppenmtr, const int param, const int value){
  memset(tmpstr, 0x00, MTR_BUFFSIZE);
  sprintf(tmpstr, "PRW%d=%d", param, value);
  penguin_motor_sendData(ppenmtr, tmpstr);
  int ret = penguin_motor_getParameter(ppenmtr, param);
  if(ret != value){
    printf("ERROR:: penguin_motor_setParameter (param# %d set to %d) not successed. Current value is %d. \n", param, value, ret);
    return -1;
  }
  return 0;
}

int penguin_motor_getParameter(penguin_motor_t* ppenmtr, int param){
  memset(tmpstr, 0x00, MTR_BUFFSIZE);
  sprintf(tmpstr, "PRR%d", param);
  return penguin_motor_sendrecvData_int(ppenmtr, tmpstr);
}

int penguin_motor_getAlarmHistory(penguin_motor_t* ppenmtr, const unsigned int number){
  if(number>8){
    printf("penguin_motor_getAlarmHistory(): ERROR:: Invalid history number %d <-- should be 0-8.",number);
    return -1;
  }
  memset(tmpstr, 0x00, MTR_BUFFSIZE);
  penguin_motor_sendrecvData(ppenmtr, "ALM", tmpstr);

  char reschar[4];
  strncpy(reschar, tmpstr+number*5, 4);
  return strtod(reschar, NULL);
}

int penguin_motor_getAlarmState(penguin_motor_t* ppenmtr){
  return penguin_motor_getAlarmHistory(ppenmtr, 0);
}

int penguin_motor_getModeState(penguin_motor_t* ppenmtr){
  memset(tmpstr, 0x00, MTR_BUFFSIZE);
  const int len = penguin_motor_sendrecvData(ppenmtr, "M", tmpstr);
  if(len == 0){
    perror("No data received.\n");
    return -1;
  }
  if(strcmp(tmpstr, "ABS") !=0){
    return 0;
  }
  else if(strcmp(tmpstr, "INC") !=0){
    return 1;
  }
  else{
    printf("penguin_motor_getModeState(): ERROR:: INVALID MODE RETURNED (%s). PLEASE IMPLEMENET THIS IN THE CODE.",tmpstr);
    return -1;
  }
}

int penguin_motor_getIOState_outbit(penguin_motor_t* ppenmtr, const unsigned int bitnumber){
  int ecrs = penguin_motor_getIOState(ppenmtr);
  unsigned int bit = 1 << bitnumber;
  return ((ecrs>> 0) & (bit))>>bitnumber;
}

int penguin_motor_getIOState_inbit(penguin_motor_t* ppenmtr, const unsigned int bitnumber){
  int ecrs = penguin_motor_getIOState(ppenmtr);
  unsigned int bit = 1 << bitnumber;
  return ((ecrs>> 16) & (bit))>>bitnumber;
}

int penguin_motor_getPreviousCommand(penguin_motor_t* ppenmtr, char* buffer){
  perror("penguin_motor_getPreviousCommand() This commnand is not supported anymore.");
  int len = 0;
  buffer[len] = 0;
  return len;
}

int penguin_motor_checkPreviousCommand(penguin_motor_t* ppenmtr, const char* cmd){
  char buffer[256];
  int len = penguin_motor_getPreviousCommand(ppenmtr, buffer);
  if(len==0){
    printf("penguin_motor_checkPreviousCommand(); Specified command was [%s], but received [%s].\n", cmd, buffer);
    return 1;
  }
  if(buffer[len-1] != '\n')
    printf("penguin_motor_checkPreviousCommand(), ??\n");
  buffer[len-1]='\0';
  if(strcmp(cmd, buffer) != 0){
    printf("penguin_motor_checkPreviousCommand(); Specified command was [%s], but received [%s].\n", cmd, buffer);
    return 1;
  }
  return 0;
}

