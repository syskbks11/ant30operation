#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_chopper.h"

#define DEV_NAME "/dev/tty.usbserial-FT2GX65L0"

void printHelp(){
  printf("%s\n", "This is a test program to control pulse motor with driver QT-CD1. ");
  printf("%s\n", "(program)");
  printf("\t%s\n", "e:");
  printf("\t\t%s\n", "End the program.");
  printf("\t%s\n", "h:");
  printf("\t\t%s\n", "Show this message.");
  printf("%s\n", "(configure)");
  printf("\t%s\n", "i:");
  printf("\t\t%s\n", "Configure settings of the driver.");
  printf("\t%s\n", "O:");
  printf("\t\t%s\n", "Move the origin and set the position.");
  printf("%s\n", "(status)");
  printf("\t%s\n", "?:");
  printf("\t\t%s\n", "Ask sensor status.");
  printf("%s\n", "(moter)");
  printf("\t%s\n", "s:");
  printf("\t\t%s\n", "Stop.");
  printf("\t%s\n", "x [num]:");
  printf("\t\t%s\n", "Move to the specified position.");
  printf("%s\n", "(chopper)");
  printf("\t%s\n", "o:");
  printf("\t\t%s\n", "Open the chopper.");
  printf("\t%s\n", "c:");
  printf("\t\t%s\n", "Close the chopper.");

}

int test_by_manual(){
  int len;                            //  受信データ数（バイト）
  char tmpstr[256];    // データ受信バッファ
  int res, pact, vact;

  printf("%s\n", "Test start. ");
  int n = 0;

  int cont = 1;

  penguin_chopper_configure_settings();

  if(penguin_chopper_configure_origin()){
    exit(1);
  }

  do{
    n++;
    printf("[chopper: %d] > ", n);
    fflush(stdout);
    //printf("Esc");
    //init_keyboard();

    fgets(tmpstr, 256, stdin);
    //key = fgetc(stdin)
    //read(0,&ch,1);
    //close_keyboard();


    pact = penguin_chopper_getPulse();
    printf("[Q:A1]%d\n",pact);

    //	state_switches = Read(sn);
    //printf("PACT, VACT = %d %d\n",pact, vact);
    //printf("PACT, VACT, SW = %d %d %d\n",pact, vact, state_switches);
    //	limitState(fd, sn);
    printf("cmd:%s", tmpstr);

    long pos;
    char *token;
    switch(tmpstr[0]){
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
      token=strtok(tmpstr," ");
      if(token == NULL)break;
      printf("token%s", token);
      token=strtok(NULL," ");
      if(token == NULL)break;
      pos = strtod(token, NULL);
      penguin_chopper_setPulse(pos);
      break;
    case 's':
      penguin_chopper_stop();
      break;
    case 'r':
      penguin_chopper_reset();
      break;
    case 'e':
      cont = 0;
      break;
    case 'h':
      printHelp();
      break;
    case '?':
      penguin_chopper_askSensorStatus();
      printf("status:%s", penguin_chopper_getMassage());
      break;

    default:break;
    }

  }while(cont);



  printf("%s\n", "Test end. ");

  return 0;

}


int main(int argc, char *argv[]) {

  char *devName;
  if(argc == 2){
    devName = argv[1];
  }else{
    devName = DEV_NAME;
  }

  if(penguin_chopper_init(devName)){
    printf("penguin_chopper_init() error.");
    exit(1);
  }


  test_by_manual();

  penguin_chopper_end();
}
