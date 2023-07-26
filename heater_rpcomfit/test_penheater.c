#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_heater.h"

#define STRLEN 120
#define HTR_BUFFSIZE 4096
char buffer[HTR_BUFFSIZE];

static void show_Help();

void show_Help(){
  printf("%s\n", "This is help of \"test_penheater\". ");
  printf("%s\n", "Available commands: ");
  printf("%s\n", "(program)");
  printf("%s\n", "\tH, HELP: ");
  printf("%s\n", "\t\tShow this document. ");
  printf("%s\n", "\tCntl+C: ");
  printf("%s\n", "\t\tExit the program, with no changes of state");
  printf("%s\n", "\tE, EXIT: ");
  printf("%s\n", "\t\tExit the program. ");
  printf("%s\n", "(monitor)");
  printf("%s\n", "\t{1,2,3,4,5,6,7,8}: ");
  printf("%s\n", "\t\tget temperature of channel {1,2,3,4,5,6,7,8}");
  printf("%s\n", "\tA,ALL: ");
  printf("%s\n", "\t\tget temperatures of all channels");
}

int oper_manual(penguin_heater_t* ppenhtr){

  printf("Manual operation starting... \n");
  penguin_heater_fitclose(ppenhtr);
  penguin_heater_fitopen(ppenhtr);

  int cont = 1;
  char tmpstr[STRLEN+1];
  do{
    printf("[RPCOMFIT%1d:HTR] ", ppenhtr->pctrl->deviceid);
    fflush(stdout);
    fgets(tmpstr, STRLEN+1, stdin);

    printf("cmd:%s", tmpstr);

    penguin_heater_fitclose(ppenhtr);
    penguin_heater_fitopen(ppenhtr);

    switch(tmpstr[0]){
    case '1':
      printf("CH1: %lf K\n", penguin_heater_get_temperature(ppenhtr,1));
      break;
    case '2':
      printf("CH2: %lf K\n", penguin_heater_get_temperature(ppenhtr,2));
      break;
    case '3':
      printf("CH3: %lf K\n", penguin_heater_get_temperature(ppenhtr,3));
      break;
    case 'a':
    case 'A':
      printf("CH1-3: %lf K, %lf K, %lf K \n",
             penguin_heater_get_temperature(ppenhtr,1),
             penguin_heater_get_temperature(ppenhtr,2),
             penguin_heater_get_temperature(ppenhtr,3));
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
  penguin_heater_fitclose(ppenhtr);

  return 0;
}


int main(int argc, char *argv[]) {
  printf("This software is \"heater/test\", a test program for temperature monitor of Heater TTM-004W.\n");
  printf("Type \"help\" for more information. \n");

  if(argc != 3){
    printf("Invalid command format. \n --> %s [IP-ADDRESS: 192.168.10.3] [DEVICE-ID: 1-7] \n",argv[0]);
    exit(0);
  }

  int devid =  atoi(argv[2]);
  penguin_heater_t* pm = NULL;
  pm = penguin_heater_init(argv[1], devid);

  oper_manual(pm);
  penguin_heater_end(pm);

  return 0;
}
