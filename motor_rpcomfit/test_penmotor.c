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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h> //for uint8_t

#include "penguin_motor.h"

#define STRLEN 120
#define MTR_BUFFSIZE 4096
char inputstr[STRLEN+1];
char buffer[MTR_BUFFSIZE];

int checkMonitorState(penguin_motor_t* ppenmtr){
  int res;

  printf("==============================================\n");
  res = penguin_motor_checkAlarmState(ppenmtr);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }
  printf("==============================================\n");
  res = penguin_motor_checkErrorCorrecState(ppenmtr);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }
  printf("==============================================\n");
  res = penguin_motor_checkIOState(ppenmtr);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }
  printf("==============================================\n");
  res = penguin_motor_checkDynamicBreakState(ppenmtr);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }
  printf("==============================================\n");
  res = penguin_motor_getPulse(ppenmtr);
  printf("[PMON] %d --> %.2f deg \n",res,penguin_motor_calcAngle(res));
  printf("==============================================\n");
  res = penguin_motor_getVelocity(ppenmtr);
  printf("[VMON] %d --> %.2f deg/s ", res,penguin_motor_calcAngle(res));
  res = penguin_motor_getNominalVelocity(ppenmtr);
  printf("(nominal VF = %d --> %.2f deg/s) \n",res,penguin_motor_calcAngle(res));
  printf("==============================================\n");
  res=penguin_motor_getModeState(ppenmtr);
  printf("[MODE] %d (0:ABS,1:INC) \n",res);
  printf("==============================================\n");

  return 0;
}

int checkServoOnOff(penguin_motor_t* ppenmtr){
  //int len;
  int res;

  res = checkMonitorState(ppenmtr);
  printf("==============================================\n");
  if(res!=0) return res;

  res = penguin_motor_servOn(ppenmtr);
  if(res<0) printf("[SVON] failed with timeout \n");
  else printf("[SVON] success (done in %.2f sec).\n",((float)res)/10);
  printf("==============================================\n");

  res = checkMonitorState(ppenmtr);
  printf("==============================================\n");
  if(res!=0) return res;

  res = penguin_motor_servOff(ppenmtr);
  if(res<0) printf("[SVOF] failed with timeout \n");
  else printf("[SVOF] success (done in %.2f sec).\n",((float)res)/10);
  printf("==============================================\n");

  res = checkMonitorState(ppenmtr);
  printf("==============================================\n");
  if(res!=0) return res;

  return 0;
}

int setAbsolute(penguin_motor_t* ppenmtr){
  //int len;                            //  受信データ数（バイト）
  int res;

  res = checkMonitorState(ppenmtr);
  if(res!=0) return res;

  res = penguin_motor_servOn(ppenmtr);
  if(res<0) printf("[SVON] failed with timeout \n");
  else printf("[SVON] success (done in %.2f sec).\n",((float)res)/10);
  printf("==============================================\n");

  res = checkMonitorState(ppenmtr);
  if(res!=0) return res;


  penguin_motor_setAbsoluteMode(ppenmtr);
  printf("==============================================\n");

  res = checkMonitorState(ppenmtr);
  if(res!=0) return res;

  penguin_motor_setVelocity(ppenmtr, 1); // sloweset setting
  res = penguin_motor_getNominalVelocity(ppenmtr);
  printf("[VSET] %d \n",res);
  printf("==============================================\n");

  return 0;
}

int moveMotor(penguin_motor_t* ppenmtr, int pulse){
  int res;

  res = penguin_motor_getPulse(ppenmtr);
  penguin_motor_setPulse(ppenmtr, res + pulse);

  while(penguin_motor_isReady(ppenmtr)==0) {
    res = penguin_motor_getPulse(ppenmtr);
    printf("[PMON] %d --> %.2f deg      \r",res,penguin_motor_calcAngle(res));
    fflush(stdout);
    usleep(100*1000); // wait to finish moving
  }
  res = penguin_motor_getPulse(ppenmtr);
  printf("[PMON] %d --> %.2f deg      \n",res,penguin_motor_calcAngle(res));

  res = penguin_motor_checkAlarmState(ppenmtr);
  if(res!=0) return res;

  return 0;
}

int moveBackAndForth(penguin_motor_t* ppenmtr, int pulse, unsigned int num){
  setAbsolute(ppenmtr);
  while(num!=0){
    moveMotor(ppenmtr, pulse);
    moveMotor(ppenmtr, pulse*-1);
    num = num-1;
  }

  return 0;
}

int end(penguin_motor_t* ppenmtr){
  //int len;
  int res;

  penguin_motor_stop(ppenmtr);
  penguin_motor_stopOriginSearch(ppenmtr);

  res = checkMonitorState(ppenmtr);
  if(res!=0) return res;

  penguin_motor_servOff(ppenmtr);
  if(res<0) printf("[SVOF] failed with timeout \n");
  else printf("[SVOF] success (done in %.2f sec).\n",((float)res)/10);
  printf("==============================================\n");

  res = checkMonitorState(ppenmtr);
  if(res!=0) return res;

  return 0;

}

void upperstring(char *out, const char *in){
  int i;
  i = 0;
  while(in[i] != '\0'){
    out[i] = toupper(in[i]);
    i++;
  }
}

void showHelp(){
  printf("%s\n", "This is help of \"test_penmotor\". ");
  printf("%s\n", "Available commands: ");
  printf("%s\n", "(program)");
  printf("%s\n", "\tH, HELP: ");
  printf("%s\n", "\t\tShow this document");
  printf("%s\n", "\tCntl+C: ");
  printf("%s\n", "\t\tExit the program, with no changes of motor state");
  printf("%s\n", "\tE, EXIT: ");
  printf("%s\n", "\t\tExit the program, with motor SERV-OFF");
  printf("%s\n", "(control)");
  printf("%s\n", "\tON: ");
  printf("%s\n", "\t\tservo motor on");
  printf("%s\n", "\tOFF: ");
  printf("%s\n", "\t\tservo motor off");
  printf("%s\n", "\tSTOP: ");
  printf("%s\n", "\t\tstop while motor moving");
  printf("%s\n", "\tP [PULSE]: ");
  printf("%s\n", "\t\tmotor moving to absolute pulse position [PULSE] from the origin");
  printf("%s\n", "\tPREL [PULSE]: ");
  printf("%s\n", "\t\tmotor moving [PULSE] from current position");
  printf("%s\n", "\tA [ANGLE]: ");
  printf("%s\n", "\t\tmotor moving to absolute angle position [ANGLE] from the origin [deg]");
  printf("%s\n", "\tAREL [ANGLE]: ");
  printf("%s\n", "\t\tmotor moving [ANGLE] from current position [deg]");
  printf("%s\n", "\tS [ARCSEC]: ");
  printf("%s\n", "\t\tmotor moving to absolute angle position [ARCSEC] from the origin [arcsec]");
  printf("%s\n", "\tSREL [ARCSEC]: ");
  printf("%s\n", "\t\tmotor moving [ARCSEC] from current position [arcsec]");
  printf("%s\n", "\tFWBW [PULSE] [NUM]: ");
  printf("%s\n", "\t\tmotor moving [PULSE] forward and backward repeatedly [NUM] times.");
  printf("%s\n", "\tZ: ");
  printf("%s\n", "\t\tsearch origin from the initial position");
  printf("%s\n", "\t\tazimuth should be noth direction / elevation should be ~65deg");
  printf("%s\n", "\t\tBefore use this command, PLEASE CHECK the HARD LIMITS WORK well.");
  printf("%s\n", "\tZF: ");
  printf("%s\n", "\t\tsearch origin toward positive = forward direction.");
  printf("%s\n", "\t\tIf the origin were not found, the motor goes to positive limit.");
  printf("%s\n", "\t\tBefore use this command, PLEASE CHECK the POSITIVE HARD LIMIT WORKS well.");
  printf("%s\n", "\tZR: ");
  printf("%s\n", "\t\tsearch origin toward negative = reversed direction.");
  printf("%s\n", "\t\tIf the origin were not found, the motor goes to negative limit.");
  printf("%s\n", "\t\tBefore use this command, PLEASE CHECK the NEGATIVE HARD LIMIT WORKS well.");
  printf("%s\n", "\tDBON: ");
  printf("%s\n", "\t\tdynamic break on");
  printf("%s\n", "\tDBOFF: ");
  printf("%s\n", "\t\tdynamic break off");
  printf("%s\n", "(configuration)");
  printf("%s\n", "\tV [VELOCITY]: ");
  printf("%s\n", "\t\tset velocity pulse for moving (*0.1 r/min = *0.6 deg/sec)");
  printf("%s\n", "\tPARAM [NUM]: ");
  printf("%s\n", "\t\tget parameter #[NUM]");
  printf("%s\n", "\tPARAM [NUM] [VALUE]: ");
  printf("%s\n", "\t\tset parameter #[NUM] as [VALUE]");
  printf("%s\n", "(information)");
  printf("%s\n", "\tSTATUS: ");
  printf("%s\n", "\t\tget error status. ");
  printf("%s\n", "\tDBST: ");
  printf("%s\n", "\t\tshow dynamic break status");
  printf("%s\n", "\tRESET ");
  printf("%s\n", "\t\tclear alarms");
}

int oper_manual(penguin_motor_t* ppenmtr){
  //int len;
  int res;//, pact, vact;
  //unsigned long state_switches;

  printf("Manual operation starting... \n");
  penguin_motor_fitopen(ppenmtr);
  checkServoOnOff(ppenmtr);
  //int n = 0;
  int cont = 1;
  char tmpstr[STRLEN+1];
  char tmpstr2[STRLEN+1];
  char* token;
  int num;
  do{
    memset(tmpstr,   0x00, STRLEN+1);
    memset(tmpstr2,   0x00, STRLEN+1);
    memset(inputstr, 0x00, STRLEN+1);
    penguin_motor_fitopen(ppenmtr);
    res = penguin_motor_getPulse(ppenmtr);
    printf("[PMON] %d --> %.2f deg      \n",res,penguin_motor_calcAngle(res));
    //n++;
    printf("[RPCOMFIT%1d:MOT] ", ppenmtr->pctrl->deviceid);
    fflush(stdout);
    fgets(tmpstr,STRLEN+1,stdin);
    //continue;
    if(strlen(tmpstr)==0) continue;
    strncpy(tmpstr2,tmpstr,strlen(tmpstr)-1);
    upperstring(inputstr,tmpstr2);
    if(strlen(inputstr)==0) continue;

    printf("[COMM] \"%s\" (size = %lu) ", inputstr, strlen(inputstr));
    fflush(stdout);

    penguin_motor_fitopen(ppenmtr);
    int pos;
    if(strncmp(inputstr,"SVON",4)==0){
      printf(": RECOGNIZED \n");
      penguin_motor_servOn(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"SVOFF",5)==0){
      printf(": RECOGNIZED \n");
      penguin_motor_servOff(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"PREL",4)==0){
      printf(": RECOGNIZED \n");
      if(strlen(inputstr)==4){
        printf("ERROR:: EMPTY PULSE VALUE. \"PREL [PULSE]\" \n");
        continue;
      }
      else pos = atoi(inputstr+4);
      if(penguin_motor_isOriginDetected(ppenmtr)==0){
        printf("ERROR:: NOT DETECTED ORIGIN POSITION: DO \"Z\" IN ADVANCE \n");
        continue;
      }
      if(penguin_motor_isServOn(ppenmtr)==0) penguin_motor_servOn(ppenmtr);
      pos += penguin_motor_getPulse(ppenmtr);
      penguin_motor_setPulse(ppenmtr, pos);
      continue;
    }
    if(strncmp(inputstr,"P ",2)==0){
      printf(": RECOGNIZED \n");
      if(penguin_motor_isOriginDetected(ppenmtr)==0){
        printf("ERROR:: NOT DETECTED ORIGIN POSITION: DO \"Z\" IN ADVANCE \n");
        continue;
      }
      if(penguin_motor_isServOn(ppenmtr)==0) penguin_motor_servOn(ppenmtr);
      if(strlen(inputstr)==1){
        printf("ERROR:: EMPTY PULSE VALUE. \"P [PULSE]\" \n");
        continue;
      }
      else pos = atoi(inputstr+1);
      penguin_motor_setPulse(ppenmtr, pos);
      continue;
    }
    if(strncmp(inputstr,"AREL",4)==0){
      printf(": RECOGNIZED \n");
      if(strlen(inputstr)==4){
        printf("ERROR:: EMPTY ANGLE VALUE: \"AREL [ANGLE]\" \n");
        continue;
      }
      else pos = penguin_motor_calcPulse(atof(inputstr+4));
      if(penguin_motor_isOriginDetected(ppenmtr)==0){
        printf("ERROR:: NOT DETECTED ORIGIN POSITION: DO \"Z\" IN ADVANCE \n");
        continue;
      }
      if(penguin_motor_isServOn(ppenmtr)==0) penguin_motor_servOn(ppenmtr);
      pos += penguin_motor_getPulse(ppenmtr);
      penguin_motor_setPulse(ppenmtr, pos);
      continue;
    }
    if(strncmp(inputstr,"A ",2)==0){
      printf(": RECOGNIZED \n");
      if(strlen(inputstr)==1){
        printf("ERROR:: EMPTY ANGLE VALUE: \"A [ANGLE]\" \n");
        continue;
      }
      else pos = penguin_motor_calcPulse(atof(inputstr+1));
      if(penguin_motor_isOriginDetected(ppenmtr)==0){
        printf("ERROR:: NOT DETECTED ORIGIN POSITION: DO \"Z\" IN ADVANCE \n");
        continue;
      }
      if(penguin_motor_isServOn(ppenmtr)==0) penguin_motor_servOn(ppenmtr);
      penguin_motor_setPulse(ppenmtr, pos);
      continue;
    }
    if(strncmp(inputstr,"SREL",4)==0){
      printf(": RECOGNIZED \n");
      if(strlen(inputstr)==4){
        printf("ERROR:: EMPTY ARCSEC VALUE: \"SREL [ARCSEC]\" \n");
        continue;
      }
      else pos = penguin_motor_calcPulse(atof(inputstr+4)/3600.);
      if(penguin_motor_isOriginDetected(ppenmtr)==0){
        printf("ERROR:: NOT DETECTED ORIGIN POSITION: DO \"Z\" IN ADVANCE \n");
        continue;
      }
      if(penguin_motor_isServOn(ppenmtr)==0) penguin_motor_servOn(ppenmtr);
      pos += penguin_motor_getPulse(ppenmtr);
      penguin_motor_setPulse(ppenmtr, pos);
      continue;
    }
    if(strncmp(inputstr,"S ",2)==0){
      printf(": RECOGNIZED \n");
      if(strlen(inputstr)==1){
        printf("ERROR:: EMPTY ARCSEC VALUE: \"S [ARCSEC]\" \n");
        continue;
      }
      else pos = penguin_motor_calcPulse(atof(inputstr+1)/3600.);
      if(penguin_motor_isOriginDetected(ppenmtr)==0){
        printf("ERROR:: NOT DETECTED ORIGIN POSITION: DO \"Z\" IN ADVANCE \n");
        continue;
      }
      if(penguin_motor_isServOn(ppenmtr)==0) penguin_motor_servOn(ppenmtr);
      penguin_motor_setPulse(ppenmtr, pos);
      continue;
    }
    if(strncmp(inputstr,"STATUS",5)==0){
      printf(": RECOGNIZED \n");
      checkMonitorState(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"STOP",4)==0){
      printf(": RECOGNIZED \n");
      penguin_motor_stop(ppenmtr);
      penguin_motor_stopOriginSearch(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"DBON",4)==0){
      printf(": RECOGNIZED \n");
      penguin_motor_setDynamicBreakOn(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"DBOFF",5)==0){
      printf(": RECOGNIZED \n");
      penguin_motor_setDynamicBreakOff(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"DBST",4)==0){
      printf(": RECOGNIZED \n");
      penguin_motor_checkDynamicBreakState(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"FWBW",4)==0){
      printf(": RECOGNIZED \n");
      token=strtok(inputstr," ");
      token=strtok(NULL," ");
      if(token == NULL){
        pos = penguin_motor_getPulse(ppenmtr);
        num = 10;
      }
      else{
        pos = atoi(token);
        token=strtok(NULL," ");
        if(token == NULL) num = 10;
        else num = atoi(token);
      }
      if(penguin_motor_isOriginDetected(ppenmtr)==0){
        printf("ERROR:: NOT DETECTED ORIGIN POSITION: DO \"Z\" IN ADVANCE \n");
        continue;
      }
      if(penguin_motor_isServOn(ppenmtr)==0) penguin_motor_servOn(ppenmtr);
      moveBackAndForth(ppenmtr,pos,num);
      continue;
    }
    if(strncmp(inputstr,"Z",1)==0 && strlen(inputstr)==1){
      printf(": RECOGNIZED \n");
      if(penguin_motor_isServOn(ppenmtr)==0) penguin_motor_servOn(ppenmtr);
      penguin_motor_goOrigin(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"ZF",5)==0){
      printf(": RECOGNIZED \n");
      if(penguin_motor_isServOn(ppenmtr)==0) penguin_motor_servOn(ppenmtr);
      penguin_motor_goOriginPositive(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"ZR",5)==0){
      printf(": RECOGNIZED \n");
      if(penguin_motor_isServOn(ppenmtr)==0) penguin_motor_servOn(ppenmtr);
      penguin_motor_goOriginNegative(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"RESET",4)==0){
      printf(": RECOGNIZED \n");
      penguin_motor_clearAlarm(ppenmtr);
      continue;
    }
    if(strncmp(inputstr,"V",1)==0){
      printf(": RECOGNIZED \n");
      if(strlen(inputstr)==1){
        pos = penguin_motor_getVelocity(ppenmtr);
        printf("[VMON] %d --> %.2f deg/s ", pos,pos*0.6);
        pos = penguin_motor_getNominalVelocity(ppenmtr);
        printf("(nominal VF = %d --> %.2f deg/s) \n",pos,pos*0.6);
        continue;
      }
      else{
        pos = atoi(inputstr+1);
        penguin_motor_setVelocity(ppenmtr,pos);
      }
    }
    if(strncmp(inputstr,"PARAM",5)==0){
      printf(": RECOGNIZED \n");
      token=strtok(inputstr," ");
      token=strtok(NULL," ");
      if(token == NULL){
        penguin_motor_checkParameter(ppenmtr);
      }
      else{
        pos = atoi(token);
        token=strtok(NULL," ");
        if(token != NULL){
          num = atoi(token);
          penguin_motor_setParameter(ppenmtr,pos,num);
        }
        printf("[PRMT] %d = %d",pos,penguin_motor_getParameter(ppenmtr,pos));
      }
      continue;
    }
    if(strncmp(inputstr,"HELP",4)==0){
      printf(": RECOGNIZED \n");
      showHelp();
      continue;
    }
    if(strncmp(inputstr,"E",1)==0 && strlen(inputstr)==1){
      printf(": RECOGNIZED \n");
      end(ppenmtr);
      break;
    }
    printf(": COMMAND MODE \n");
    memset(buffer, 0x00, MTR_BUFFSIZE);
    printf("SEND: %s\n",inputstr);
    penguin_motor_sendrecvData(ppenmtr,inputstr,buffer);
    printf("RECV: %s\n",buffer);
  }while(cont);

  return 0;
}

int main(int argc,char *argv[]){
  printf("This software is \"motor/test\", a test program for motor of Harmonic Drive, HA770, and its driver.\n");
  printf("Type \"help\" for more information. \n");

  if(argc != 3){
    printf("Invalid command format. \n --> %s [IP-ADDRESS: 192.168.10.3] [DEVICE-ID: 1-7] \n",argv[0]);
    exit(0);
  }

  int devid =  atoi(argv[2]);
  penguin_motor_t* pm = NULL;
  if(devid==1)      pm = penguin_motor_init(argv[1], devid, 6000000, -11500000);
  else if(devid==2) pm = penguin_motor_init(argv[1], devid, 500000,-2300000);
  else              pm = penguin_motor_init(argv[1], devid, 60000000, -60000000);

  oper_manual(pm);
  penguin_motor_end(pm);

  return 0;
}

