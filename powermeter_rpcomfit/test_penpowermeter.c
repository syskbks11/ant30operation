#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_powermeter.h"

#define STRLEN 120
#define PM_BUFFSIZE 4096
char buffer[PM_BUFFSIZE];

static void show_Help();

void show_Help(){
  printf("%s\n", "This is help of \"test_penpowermeter\". ");
  printf("%s\n", "Available commands: ");
  printf("%s\n", "(program)");
  printf("%s\n", "\tH, HELP: ");
  printf("%s\n", "\t\tShow this document. ");
  printf("%s\n", "\tCntl+C: ");
  printf("%s\n", "\t\tExit the program, with no changes of state");
  printf("%s\n", "\tE, EXIT: ");
  printf("%s\n", "\t\tExit the program. ");
  printf("%s\n", "(monitor)");
  printf("%s\n", "\t[empty]: ");
  printf("%s\n", "\t\tget powermeter vallue");
}

int oper_manual(penguin_powermeter_t* ppenpm){

  printf("Manual operation starting... \n");
  penguin_powermeter_fitclose(ppenpm);
  penguin_powermeter_fitopen(ppenpm);

  int cont = 1;
  char tmpstr[STRLEN+1];
  do{
    printf("[RPCOMFIT%1d:PM] ", ppenpm->pctrl->deviceid);
    fflush(stdout);
    fgets(tmpstr, STRLEN+1, stdin);

    printf("cmd:%s", tmpstr);

    penguin_powermeter_fitclose(ppenpm);
    penguin_powermeter_fitopen(ppenpm);

    switch(tmpstr[0]){
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
    printf("powermeter = %f dBm\n",penguin_powermeter_get_power(ppenpm));
  }while(cont);

  printf("%s\n", "Test end. ");
  penguin_powermeter_fitclose(ppenpm);

  return 0;
}


int main(int argc, char *argv[]) {
  printf("This software is \"powermeter/test\", a test program for powermeter Agilent E4418B.\n");
  printf("Type \"help\" for more information. \n");

  if(argc != 3){
    printf("Invalid command format. \n --> %s [IP-ADDRESS: 192.168.10.3] [DEVICE-ID: 1-7] \n",argv[0]);
    exit(0);
  }

  int devid =  atoi(argv[2]);
  penguin_powermeter_t* pm = NULL;
  pm = penguin_powermeter_init(argv[1], devid);

  oper_manual(pm);
  penguin_powermeter_end(pm);

  return 0;
}


