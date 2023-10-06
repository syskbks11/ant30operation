#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_lakeshore.h"

static void show_Help();
static int test_by_manual();

void show_Help(){
  printf("%s\n", "This is help of \"lakeshore\". ");
  printf("%s\n", "Available commands: ");
  printf("%s\n", "\te, exit: ");
  printf("%s\n", "\t\tExit the program. ");
  printf("%s\n", "\thelp: ");
  printf("%s\n", "\t\tShow this document. ");
  printf("%s\n", "\t1: ");
  printf("%s\n", "\t\tGet temperature of Channel 1 now. ");
  printf("%s\n", "\t2: ");
  printf("%s\n", "\t\tGet temperature of Channel 1 now. ");
  printf("%s\n", "\t3: ");
  printf("%s\n", "\t\tGet temperature of Channel 1 now. ");
  printf("%s\n", "\ti, identification: ");
  printf("%s\n", "\t\tget name the instrument. ");
}

int test_by_manual(){
  int len;                            //  受信データ数（バイト）
  char tmpstr[256];    // データ受信バッファ
  int res, pact, vact;

  printf("%s\n", "Test start. ");
  int n = 0;

  int cont = 1;

  do{
    n++;
    printf("[lakeshore: %d] > ", n);
    fflush(stdout);
    //printf("Esc");	
    //init_keyboard();

    fgets(tmpstr, 256, stdin);
    //key = fgetc(stdin)
    //read(0,&ch,1);
    //close_keyboard();

    printf("cmd:%s\n", tmpstr);

    long pos;
    char *token;
    double temperature;
    switch(tmpstr[0]){
    case '1':
      temperature = penguin_lakeshore_get_temperature(1);
      printf("CH1: %lf K\n", temperature);
      break;
    case '2':
      temperature = penguin_lakeshore_get_temperature(2);
      printf("CH2: %lf K\n", temperature);
      break;
    case '3':
      temperature = penguin_lakeshore_get_temperature(3);
      printf("CH3: %lf K\n", temperature);
      break;
    case '4':
      temperature = penguin_lakeshore_get_temperature(4);
      printf("CH4: %lf K\n", temperature);
      break;
    case '5':
      temperature = penguin_lakeshore_get_temperature(5);
      printf("CH5: %lf K\n", temperature);
      break;
    case '6':
      temperature = penguin_lakeshore_get_temperature(6);
      printf("CH6: %lf K\n", temperature);
      break;
    case 'i':
      penguin_lakeshore_get_identification();
      printf("id: %s\n", penguin_lakeshore_getMessage());
      break;
    case 'e':
      cont = 0;
      break;
    case 'h':
      if(strncmp("help", tmpstr, 4)==0){
        show_Help();
      }
      break;

    default:break;	
    }
		
  }while(cont);



  printf("%s\n", "Test end. ");

  return 0;
	
}


int	main(int argc, char *argv[]) {


  printf("%s\n", "This software is \"lakeshore\", a test program for temperature monitor of Lakeshore. ");	
  printf("%s\n", "If you need more information, please type \"help\". ");	

  if(penguin_lakeshore_init()){
    printf("penguin_lakeshore_init() error.");
    exit(1);
  }


  test_by_manual();

  penguin_lakeshore_end();
}
