/*
 * ¥file autoSignalG.cpp
 * written by NAGAI Makoto
 * 2009.02.20
 * for 30-cm telescope
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "../libtkb/src/libtkb.h"
#include "penguin_chopper.h"

#define DEV_NAME "/dev/tty.usbserial-FT2GX65L0"

void showHelp(){
  printf("%s\n", "This is help of \"autoChopper\". ");
  printf("%s\n", "Available commands: ");
  printf("%s\n", "(program)");
  printf("%s\n", "\thelp: ");
  printf("%s\n", "\t\tShow this document. ");
  //	printf("%s\n", "\tCntl+C: ");
  //	printf("%s\n", "\t\tExit the program, with no change of motor's state. ");
  printf("%s\n", "\te, exit: ");
  printf("%s\n", "\t\tExit the program. ");
  printf("%s\n", "(emergency stop)");
  printf("%s\n", "\ts, S: ");
  printf("%s\n", "\t\tstop, for emergency when the motor moves. ");
  printf("%s\n", "(information)");
  printf("%s\n", "\t?: ");
  printf("%s\n", "\t\tget sensor status. ");
  printf("%s\n", "\tp: ");
  printf("%s\n", "\t\tget current position in pulse unit. ");
  //	printf("%s\n", "\t#: ");
  //	printf("%s\n", "\t\tshow important paramters of the driver. ");
  printf("%s\n", "(move the motor in ABSolute mode)");
  printf("%s\n", "\to: ");
  printf("%s\n", "\t\tmove to a position of \"open\"");
  printf("%s\n", "\tc: ");
  printf("%s\n", "\t\tmove to a position of \"close\"");
  printf("%s\n", "(move the motor in relative mode)");
  printf("%s\n", "\tx: ");
  printf("%s\n", "\t\tmove to a position. Specify a position.");
  printf("%s\n", "\t\tTo (current) origin: x 0");
  printf("%s\n", "\t\tnear positive limit: x 24000");
  printf("%s\n", "(go to origin)");
  printf("%s\n", "\tO: ");
  printf("%s\n", "\t\tsearch origin toward negative direction.");
  printf("%s\n", "\t\tThe origin is the negative limit.");
  printf("%s\n", "\t\tBefore use this command, PLEASE CHECK the NEGATIVE HARD LIMIT WORKS well.");
  printf("%s\n", "\ti: ");
  printf("%s\n", "\t\tsearch origin toward negative direction.");
  printf("%s\n", "\t\tIn addition to \"O\" command, this command set parameters of the driver.");
  printf("%s\n", "\t\tBefore use this command, PLEASE CHECK the NEGATIVE HARD LIMIT WORKS well.");

}

int manageCommand(char* cmd){
  long pos;
  char *token;
  int pact;

  uM1("cmd: %s", cmd);
  switch(cmd[0]){
  case 'o':
    penguin_chopper_open();
    break;
  case 'c':
    penguin_chopper_close();
    break;
  case 'i':
    penguin_chopper_configure_settings();
  case '0':
    penguin_chopper_configure_origin();
    break;
  case 'x':
    token=strtok(cmd," ");
    if(token == NULL)break;
    printf("token%s", token);
    token=strtok(NULL," ");
    if(token == NULL)break;
    pos = strtod(token, NULL);
    penguin_chopper_setPulse(pos);
    break;
  case 'S':
  case 's':
    penguin_chopper_stop();
    break;
  case 'r':
    penguin_chopper_reset();
    break;
  case 'e':
    return 0;//stop the program.
  case '#':
    break;
  case '?':
    penguin_chopper_askSensorStatus();
    uM1("status:%s", penguin_chopper_getMassage());
    break;
  case 'p':
    pact = penguin_chopper_getPulse();
    uM1("pulse:%d",pact);
    break;
  case 'h':
    showHelp();
    break;
  default:break;
  }
  return 1;//continue the  program.
}

void checkAndClose(){
  int pact = penguin_chopper_getPulse();
  uM1("pulse:%d",pact);
  int sensor = penguin_chopper_getSensorStatus();
  uM1("status:%d", sensor);
  if(sensor == 1010){
    uM("WARNING: Should be open, but close!");
  }else if(sensor == 10){
    uM("WARNING: Should be open, but not!");
  }else if(sensor != 110){
    uM1("WARNING: unexpected sensor status (%d)!", sensor);
  }
  //	penguin_chopper_askSensorStatus();
  //	uM1("status:%s", penguin_chopper_getMassage());
  printf("I'll close the chopper.\n");
  penguin_chopper_close();
}

void checkAndOpen(){
  int pact = penguin_chopper_getPulse();
  uM1("pulse:%d",pact);
  int sensor = penguin_chopper_getSensorStatus();
  uM1("status:%d", sensor);
  if(sensor == 110){
    uM("WARNING: Should be close, but open!");
  }else if(sensor == 10){
    uM("WARNING: Should be close, but not!");
  }else if(sensor != 1010){
    uM1("WARNING: unexpected sensor status (%d)!", sensor);
  }
  //	penguin_chopper_askSensorStatus();
  //	uM1("status:%s", penguin_chopper_getMassage());
  printf("I'll open the chopper.\n");
  penguin_chopper_open();
}

int main(int argc, char* argv[]){
  char *devName;
  if(argc == 2){
    devName = argv[1];
  }else{
    devName = DEV_NAME;
  }

  int ret;
  printf("%s\n", "This software is \"autoChopper\", a test program for linear motor and its driver.");
  printf("%s\n", "If you need more information, please type \"help\". ");

  uInit("../log/autoChopper");//090813 in
  /* 090813 out
     uInit("../log/autoChopper");
  */
  uM2("This program is Compiled at %s %s", __DATE__, __TIME__);

  ret = penguin_chopper_init(devName);

  if(ret){
    uM1("main(); penguin_chopper_init(); failed (%d)", ret);
    return -1;
  }
  if(penguin_chopper_configure_settings()){
    uM("main(); penguin_chopper_configure_settings(); failed.");
    return -1;
  }
  if(penguin_chopper_configure_origin()){
    exit(1);
  }

  uM("main(); penguin_chopper was initialized successfully.");

  /* check stdin, while the program move the chopper. */
  fd_set rfds;
  struct timeval tv;
  char tmpstr[256];    // データ受信バッファ
  int cont = 1;
  int n = 1;

  int isOpen = 1;
  do{

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* Wait up to ten seconds. */
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    /* print prompt */
    printf("[autoChopper: %d] > ", n);
    fflush(stdout);

    ret = select(1, &rfds, NULL, NULL, &tv);

    if(ret == -1){
      perror("select()");
    }else if(ret){
      fgets(tmpstr, 256, stdin);
      cont = manageCommand(tmpstr);
      n++;
    }else{
      if(isOpen){
        checkAndClose();
        isOpen = 0;
      }else{
        checkAndOpen();
        isOpen = 1;
      }
    }
  }while(cont);
  uEnd();
  penguin_chopper_end();
}
