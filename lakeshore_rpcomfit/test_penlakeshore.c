#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_lakeshore.h"

#define STRLEN 120
#define LKS_BUFFSIZE 4096
char buffer[LKS_BUFFSIZE];

static void show_Help();

void show_Help(){
  printf("%s\n", "This is help of \"test_penlakeshore\". ");
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
  printf("%s\n", "\tI: ");
  printf("%s\n", "\t\tget name the instrument name");
  printf("%s\n", "\tA,ALL: ");
  printf("%s\n", "\t\tget temperatures of all channels");
}

int oper_manual(penguin_lakeshore_t* ppenlks){

  printf("Manual operation starting... \n");
  penguin_lakeshore_fitclose(ppenlks);
  penguin_lakeshore_fitopen(ppenlks);

  int cont = 1;
  char tmpstr[STRLEN+1];
  do{
    printf("[RPCOMFIT%1d:LKS] ", ppenlks->pctrl->deviceid);
    fflush(stdout);
    fgets(tmpstr, STRLEN+1, stdin);

    printf("cmd:%s", tmpstr);

    penguin_lakeshore_fitclose(ppenlks);
    penguin_lakeshore_fitopen(ppenlks);

    switch(tmpstr[0]){
    case '1':
      printf("CH1: %lf K\n", penguin_lakeshore_get_temperature(ppenlks,1));
      break;
    case '2':
      printf("CH2: %lf K\n", penguin_lakeshore_get_temperature(ppenlks,2));
      break;
    case '3':
      printf("CH3: %lf K\n", penguin_lakeshore_get_temperature(ppenlks,3));
      break;
    case '4':
      printf("CH4: %lf K\n", penguin_lakeshore_get_temperature(ppenlks,4));
      break;
    case '5':
      printf("CH5: %lf K\n", penguin_lakeshore_get_temperature(ppenlks,5));
      break;
    case '6':
      printf("CH6: %lf K\n", penguin_lakeshore_get_temperature(ppenlks,6));
      break;
    case '7':
      printf("CH7: %lf K\n", penguin_lakeshore_get_temperature(ppenlks,7));
      break;
    case '8':
      printf("CH8: %lf K\n", penguin_lakeshore_get_temperature(ppenlks,8));
      break;
    case 'i':
    case 'I':
      penguin_lakeshore_get_identification(ppenlks,buffer);
      printf("id: %s\n", buffer);
      break;
    case 'a':
    case 'A':
      printf("CH1-8: %lf K, %lf K, %lf K, %lf K, %lf K, %lf K, %lf K, %lf K \n",
             penguin_lakeshore_get_temperature(ppenlks,1),
             penguin_lakeshore_get_temperature(ppenlks,2),
             penguin_lakeshore_get_temperature(ppenlks,3),
             penguin_lakeshore_get_temperature(ppenlks,4),
             penguin_lakeshore_get_temperature(ppenlks,5),
             penguin_lakeshore_get_temperature(ppenlks,6),
             penguin_lakeshore_get_temperature(ppenlks,7),
             penguin_lakeshore_get_temperature(ppenlks,8));
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
  penguin_lakeshore_fitclose(ppenlks);

  return 0;
}


int main(int argc, char *argv[]) {
  printf("This software is \"lakeshore/test\", a test program for temperature monitor of Lakeshore Model218.\n");
  printf("Type \"help\" for more information. \n");

  if(argc != 3){
    printf("Invalid command format. \n --> %s [IP-ADDRESS: 192.168.10.3] [DEVICE-ID: 1-7] \n",argv[0]);
    exit(0);
  }

  int devid =  atoi(argv[2]);
  penguin_lakeshore_t* pm = NULL;
  pm = penguin_lakeshore_init(argv[1], devid);

  oper_manual(pm);
  penguin_lakeshore_end(pm);

  return 0;
}
