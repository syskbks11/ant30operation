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

#define BUFF_SIZE 4096                 // 適当
//#define DEV_NAME "/dev/tty.usbserial-FT2GX65L0"
#define DEV_NAME 1
#define STRLEN 12

char tmpstr_test[STRLEN+1];

int checkMonitorState(const penguin_motor_t* fd){
  int len; //  受信データ数（バイト）
  char buffer[BUFF_SIZE];    // データ受信バッファ
  char* rest;
  int resint;
  int res;

  res = penguin_motor_checkAlermState(fd);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }

  res = penguin_motor_checkErrorCorrectionState(fd);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }

  res = penguin_motor_checkDOState(fd);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }

  res = penguin_motor_checkDynamicBreakState(fd);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }

  res = penguin_motor_getPulseActual(fd);
  printf("[PACT]%d\n",res);

  res = penguin_motor_getVelocityActual(fd);
  printf("[VACT]%d\n",res);

  res=penguin_motor_getModeState(fd);
  printf("[M]%d (0:ABS,1:INC) \n",res);

  return 0;

}

int testPhase2(const penguin_motor_t* fd){
  int len;                            //  受信データ数（バイト）
  char buffer[BUFF_SIZE];    // データ受信バッファ
  //char* rest;
  int resint;
  int res;

  //usleep(100*1000);

  printf("==============================================\n");
  res = penguin_motor_checkAlermState(fd);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }
  printf("==============================================\n");
  res = penguin_motor_checkDOState(fd);
  if(res == 0)
    printf("[DOST] success.\n");
  printf("==============================================\n");

  //penguin_motor_setDynamicBreakOFF(fd); // might not be needed.
  printf("==============================================\n");
  penguin_motor_setSERVON(fd);
  printf("==============================================\n");

  usleep(4000*1000);

  printf("==============================================\n");
  res = penguin_motor_checkDOState(fd);
  if(res == 0)
    printf("[DOST] success.\n");
  printf("==============================================\n");

  printf("==============================================\n");
  res = penguin_motor_checkErrorCorrectionState(fd);
  printf("==============================================\n");

  printf("==============================================\n");
  res = penguin_motor_getPulseActual(fd);
  printf("[PACT]%d\n",res);
  printf("==============================================\n");

  printf("==============================================\n");
  res = penguin_motor_getVelocityActual(fd);
  printf("[VACT]%d\n",res);
  printf("==============================================\n");

  printf("==============================================\n");
  res = penguin_motor_checkAlermState(fd);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }
  printf("==============================================\n");

  printf("==============================================\n");
  penguin_motor_setSERVOFF(fd);
  printf("==============================================\n");

  usleep(200*1000);

  printf("==============================================\n");
  res = penguin_motor_checkAlermState(fd);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }
  printf("==============================================\n");


  printf("==============================================\n");
  res = penguin_motor_checkDOState(fd);
  if(res == 0)
    printf("[DOST] success.\n");
  printf("==============================================\n");

  return 0;

}


int prepareAbsolute(const penguin_motor_t* fd){
  int len;                            //  受信データ数（バイト）
  char buffer[BUFF_SIZE];    // データ受信バッファ
  //char* rest;
  int resint;
  int res;

  if(penguin_motor_configureInput(fd)!=3){
    printf("%s\n", "prepareAbsolute(); penguin_motor_configureInput() failed, i.e. it's danger to move the motor.\n");
    return -1;
  }

  //res = penguin_motor_checkAlermState(fd);
  // if(res != 0){
  //   printf("%s\n", "Test failed with error. ");
  //   return res;
  // }

  res = penguin_motor_checkDOState(fd);
  if(res == 0)
    printf("[DOST] success.\n");

  penguin_motor_setSERVON(fd);
  buffer[len] = 0;
  printf("[SVON]%s",buffer);

  usleep(4000*1000);

  res = penguin_motor_checkDOState(fd);
  if(res == 0)
    printf("[DOST] success.\n");
  /*
    resint = penguin_motor_getDOState(fd);
    printf("[DOST]%d\n",res);
  */
  //    len=send_receive(fd, "DOST\r", 5, buffer);
  //	buffer[len] = 0;
  //	printf("[DOST]%s",buffer);

  res = penguin_motor_checkErrorCorrectionState(fd);
  /*
    len=penguin_motor_send_receive(fd, "ECRS\r", 5, buffer);
    buffer[len] = 0;
    printf("[ECRS]%s",buffer);
  */

  res = penguin_motor_checkAlermState(fd);
  /*
    res = penguin_motor_getAlermState(fd);
  */
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }

  len=penguin_motor_send_receive(fd, "M\r", 2, buffer);
  buffer[len] = 0;
  printf("[M]%s",buffer);

  penguin_motor_setAbsoluteMode(fd);

  len=penguin_motor_send_receive(fd, "M\r", 2, buffer);
  buffer[len] = 0;
  printf("[M]%s",buffer);

  res = penguin_motor_checkDOState(fd);
  if(res == 0)
    printf("[DOST] success.\n");
  /*
    resint = penguin_motor_getDOState(fd);
    printf("[DOST]%d\n",res);
  */
  //    len=send_receive(fd, "DOST\r", 5, buffer);
  //	buffer[len] = 0;
  //	printf("[DOST]%s",buffer);


  usleep(3000*1000);

  len=penguin_motor_send_receive(fd,"#22\r", 4, buffer);
  buffer[len] = 0;
  printf("[#22=]%s",buffer);

  penguin_motor_setVelocity(fd, 1);

  len=penguin_motor_send_receive(fd,"#22\r", 4, buffer);
  buffer[len] = 0;
  printf("[#22=]%s",buffer);

  resint = penguin_motor_getVelocity(fd);
  printf("[V=]%d\r",resint);

  res = penguin_motor_getPulseActual(fd);
  printf("[PACT]%d\n",res);

  res = penguin_motor_getVelocityActual(fd);
  printf("[VACT]%d\n",res);

  return 0;

}

int movePositive(const penguin_motor_t* fd){
  char buffer[BUFF_SIZE];    // データ受信バッファ

  penguin_motor_setPulse(fd, 6500000);

  while(penguin_motor_getPulseActual(fd) < 0){
    usleep(3000*1000);
  }
  int res;
  while(1){
    res = penguin_motor_getAlermState(fd);
    if(res){
      printf("%s: %d\n", "stop positive", res);
      return res;
    }
    printf("%s", ".");
    fflush(stdout);
    usleep(1000*1000);
  }
}
int moveNegative(const penguin_motor_t* fd){
  char buffer[BUFF_SIZE];    // データ受信バッファ

  penguin_motor_setPulse(fd, -6300000);
  while(penguin_motor_getPulseActual(fd) > 0){
    usleep(3000*1000);
  }
  int res;
  while(1){
    res = penguin_motor_getAlermState(fd);
    if(res){
      printf("%s: %d\n", "stop negative", res);
      //printf("P=%d\n", penguin_motor_getPulseActual(fd)),
      return res;
    }
    printf("%s", ".");
    fflush(stdout);
    usleep(1000*1000);
  }
}
int moveBackAndForth(const penguin_motor_t* fd){
  prepareAbsolute(fd);
  while(1){
    movePositive(fd);
    printf("P=%d\n", penguin_motor_getPulseActual(fd));
    moveNegative(fd);
    printf("P=%d\n", penguin_motor_getPulseActual(fd));
  }
}

int endProcedure(const penguin_motor_t* fd){
  int len;                            //  受信データ数（バイト）
  char buffer[BUFF_SIZE];    // データ受信バッファ
  //char* rest;
  int resint;
  int res;

  penguin_motor_buffered_write(fd, "S\r", 2);
  //len=penguin_motor_getPreviousCommand(fd, buffer);
  //buffer[len] = 0;
  //printf("[S]%s",buffer);


  penguin_motor_buffered_write(fd, "PUL\r", 4);

  res = penguin_motor_checkDOState(fd);
  if(res == 0)
    printf("[DOST] success.\n");
  /*
    resint = penguin_motor_getDOState(fd);
    printf("[DOST]%d\n",resint);
  */
  //    len=send_receive(fd, "DOST\r", 5, buffer);
  //	buffer[len] = 0;
  //	printf("[DOST]%s",buffer);

  penguin_motor_setSERVOFF(fd);
  //buffered_write(fd, "SVOF\r", 5);
    
  usleep(200*1000);

  res = penguin_motor_getAlermState(fd);
  if(res != 0){
    printf("%s\n", "Test failed with error. ");
    return res;
  }


  resint = penguin_motor_getDOState(fd);
  printf("[DOST]%d\n",res);
  //    len=send_receive(fd, "DOST\r", 5, buffer);
  //	buffer[len] = 0;
  //	printf("[DOST]%s",buffer);

  return 0;

}

void showHelp(){
  printf("%s\n", "This is help of \"test\". ");
  printf("%s\n", "Available commands: ");
  printf("%s\n", "(program)");
  printf("%s\n", "\thelp: ");
  printf("%s\n", "\t\tShow this document. ");
  printf("%s\n", "\tCntl+C: ");
  printf("%s\n", "\t\tExit the program, with no change of motor's state. ");
  printf("%s\n", "\te, exit: ");
  printf("%s\n", "\t\tExit the program, motor will become SERV-OFF. ");
  printf("%s\n", "(emergency stop)");
  printf("%s\n", "\tq, Q: ");
  printf("%s\n", "\t\tSERV-OFF, for emergency with SERV-ON. ");
  printf("%s\n", "\ts, S: ");
  printf("%s\n", "\t\tstop, for emergency when the motor moves. ");
  printf("%s\n", "(information)");
  printf("%s\n", "\t?: ");
  printf("%s\n", "\t\tget error status. ");
  printf("%s\n", "\t#: ");
  printf("%s\n", "\t\tshow important paramters of the driver. ");
  printf("%s\n", "(tests)");
  printf("%s\n", "\t1: ");
  printf("%s\n", "\t\ttest phase 1: check motor state without any change. ");
  printf("%s\n", "\t2: ");
  printf("%s\n", "\t\ttest phase 2: check motor state, SERV-ON, and SERV-OFF. ");
  printf("%s\n", "(operation)");
  printf("%s\n", "\tv, velocity: ");
  printf("%s\n", "\t\tset velocity appropriately. ");
  printf("%s\n", "\tc, clear: ");
  printf("%s\n", "\t\tclear alerms. ");
  printf("%s\n", "\ta, absolute: ");
  printf("%s\n", "\t\tset up ABSolute mode. ");
  printf("%s\n", "(move the motor in ABSolute mode)");
  printf("%s\n", "\tx: ");
  printf("%s\n", "\t\tmove to a position. Specify a position.");
  printf("%s\n", "\t\tTo (current) origin: x 0");
  printf("%s\n", "\t\tnear positive limit: x 5000000");
  printf("%s\n", "\t\tnear negative limit: x -6000000");
  printf("%s\n", "\tX+: ");
  printf("%s\n", "\t\tmove to a position, +10 degree from the (current) origin.");
  printf("%s\n", "\tX-: ");
  printf("%s\n", "\t\tmove to a position, -10 degree from the (current) origin.");
  printf("%s\n", "(go to origin)");
  printf("%s\n", "\th+: ");
  printf("%s\n", "\t\tsearch origin toward positive direction.");
  printf("%s\n", "\t\tIf the origin were not found, the motor goes to positive limit.");
  printf("%s\n", "\t\tBefore use this command, PLEASE CHECK the POSITIVE HARD LIMIT WORKS well.");
  printf("%s\n", "\th-: ");
  printf("%s\n", "\t\tsearch origin toward negative direction.");
  printf("%s\n", "\t\tIf the origin were not found, the motor goes to negative limit.");
  printf("%s\n", "\t\tBefore use this command, PLEASE CHECK the NEGATIVE HARD LIMIT WORKS well.");
}

int oper_manual(const penguin_motor_t* fd){
  int len;
  char buffer[BUFF_SIZE];
  int res, pact, vact;
  unsigned long state_switches;

  printf("Manual operation starting... \n");
  int n = 0;
  int cont = 1;
  do{
    n++;
    printf("[test: %d] > ", n);
    fflush(stdout);
    fgets(tmpstr_test,STRLEN,stdin);

    printf("fd=%p\n", fd);
    printf("id=%d\n", penguin_motor_getID(fd));
    vact = penguin_motor_getVelocityActual(fd);
    printf("[VACT]%d\n",vact);
    pact = penguin_motor_getPulseActual(fd);
    printf("[PACT]%d\n",pact);

    printf("input command: %s", tmpstr_test);

    long pos;
    char *token;
    switch(tmpstr_test[0]){
    case 'q':
    case 'Q':
      penguin_motor_setSERVOFF(fd);
      break;
    case 'o':
    case 'O':
      penguin_motor_setSERVON(fd);
      break;
    case 's':
    case 'S':
      penguin_motor_stop(fd);
      break;
    case 'g':
    case 'G':
      penguin_motor_start(fd);
      break;
    case 'd':
      switch(tmpstr_test[1]){
      case 'n':
        penguin_motor_setDynamicBreakON(fd);
        printf("[DBONS]%d\n",penguin_motor_checkDynamicBreakState(fd));
        break;
      case 'f':
        penguin_motor_setDynamicBreakOFF(fd);
        printf("[DBONS]%d\n",penguin_motor_checkDynamicBreakState(fd));
        break;
      case 's':
        printf("[DBONS]%d\n",penguin_motor_checkDynamicBreakState(fd));
        break;
      default:
        break;
      }
    case '1':
      checkMonitorState(fd);
      break;
    case '2':
      testPhase2(fd);
      break;
    case '+':
      movePositive(fd);
      break;
    case '-':
      moveNegative(fd);
      break;
    case 'x':
      token=strtok(tmpstr_test," ");
      if(token == NULL)break;
      printf("token%s", token);
      token=strtok(NULL," ");
      if(token == NULL)break;
      pos = atoi(token);
      penguin_motor_setPulse(fd, pos);
      sprintf(buffer, "P=%ld", pos);
      break;
    case 'X':
      switch(tmpstr_test[1]){
      case '-':
        pos = -10*PULSE_360DEG/360;
        break;
      case '+':
      default:
        pos = 10*PULSE_360DEG/360;
        break;
      }
      penguin_motor_setPulse(fd, pos);
      sprintf(buffer, "P=%ld", pos);
      break;
    case 'h':
      penguin_motor_setVelocity(fd, 10);
      penguin_motor_setJogVelocity(fd, 1);
      switch(tmpstr_test[1]){
      case '-':
        penguin_motor_goOriginNegative(fd);
        break;
      case '+':
        penguin_motor_goOriginPositive(fd);
        break;
      case 'e':
        showHelp();
      default:
        break;
      }
      break;
    case 'v':
      penguin_motor_setVelocity(fd, 1);
      break;
    case 'a':
      prepareAbsolute(fd);
      break;
    case 'b':
      moveBackAndForth(fd);
      break;
    case 'c':
      penguin_motor_clearAlerm(fd);
      break;
    case 'e':
      cont = 0;
      endProcedure(fd);
      break;
    case '#':
      res = penguin_motor_getPositionLoopConstant(fd);
      printf("[POSLOOPCONST]%d\n",res);
      res = penguin_motor_getVelocityLoopConstant(fd);
      printf("[VELLOOPCONST]%d\n",res);
      res = penguin_motor_getGain(fd);
      printf("[GAIN]%d\n",res);
      break;
    case '?':
      res = penguin_motor_checkDynamicBreakState(fd);
      if(res == 0)
        printf("[DBST] success.\n");
      res = penguin_motor_checkDOState(fd);
      if(res == 0)
        printf("[DOST] success.\n");
      res = penguin_motor_checkErrorCorrectionState(fd);
      if(res == 0)
        printf("[ECRS] success.\n");
      res = penguin_motor_checkAlermState(fd);
      if(res == 0)
        printf("[ALST] success.\n");
      break;

    default:
      break;
    }
  }while(cont);

  printf("%s\n", "Test end. ");

  return 0;
}

int main(int argc,char *argv[]){
  printf("This software is \"motor/test\", a test program for motor of Harmonic Drive, HA770, and its driver.\n");
  printf("Type \"help\" for more information. \n");

  unsigned int devName;
  if(argc == 2){
    devName = atoi(argv[1]);
  }else{
    devName = DEV_NAME;
  }

  penguin_motor_t* pm = penguin_motor_init(devName, 6000000, -6000000);
  int len = 0;
  char buffer[BUFF_SIZE];

  oper_manual(pm);
  penguin_motor_end(pm);

  exit(0);
}

