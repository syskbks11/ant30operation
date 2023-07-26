#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_gauge.h"

#define STRLEN 120
#define LKS_BUFFSIZE 4096
char buffer[LKS_BUFFSIZE];

static void show_Help();

void show_Help(){
  printf("%s\n", "This is help of \"test_pengauge\". ");
  printf("%s\n", "Available commands: ");
  printf("%s\n", "(program)");
  printf("%s\n", "\tH, HELP: ");
  printf("%s\n", "\t\tShow this document. ");
  printf("%s\n", "\tCntl+C: ");
  printf("%s\n", "\t\tExit the program, with no changes of state");
  printf("%s\n", "\tE, EXIT: ");
  printf("%s\n", "\t\tExit the program. ");
  printf("%s\n", "(monitor)");
  printf("%s\n", "\tP: ");
  printf("%s\n", "\t\tget pressure");
}

int oper_manual(penguin_gauge_t* ppengag){

  printf("Manual operation starting... \n");
  penguin_gauge_fitclose(ppengag);
  penguin_gauge_fitopen(ppengag);

  int cont = 1;
  char tmpstr[STRLEN+1];
  do{
    printf("[RPCOMFIT%1d:GAG] ", ppengag->pctrl->deviceid);
    fflush(stdout);
    fgets(tmpstr, STRLEN+1, stdin);

    printf("cmd:%s", tmpstr);

    penguin_gauge_fitclose(ppengag);
    penguin_gauge_fitopen(ppengag);

    switch(tmpstr[0]){
    case 'p':
    case 'P':
      printf("Pressure: %lf \n", penguin_gauge_get_pressure(ppengag));
      break;
    case 'h':
    case 'H':
      show_Help();
      break;
    case 'e':
    case 'E':
      cont = 0;
      break;
    default:
      break;
    }
  }while(cont);

  printf("%s\n", "Test end. ");
  penguin_gauge_fitclose(ppengag);

  return 0;
}


int main(int argc, char *argv[]) {
  printf("This software is \"gauge/test\", a test program for vacuum pressure monitor of Pfeiffer TPG 261.\n");
  printf("Type \"help\" for more information. \n");

  if(argc != 3){
    printf("Invalid command format. \n --> %s [IP-ADDRESS: 192.168.10.3] [DEVICE-ID: 1-7] \n",argv[0]);
    exit(0);
  }

  int devid =  atoi(argv[2]);
  penguin_gauge_t* pm = NULL;
  pm = penguin_gauge_init(argv[1], devid);

  oper_manual(pm);
  penguin_gauge_end(pm);

  return 0;
}
