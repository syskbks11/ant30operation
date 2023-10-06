#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>//signal()

#include "penguin_powermeter.h"
#include "../libtkb/export/libtkb.h"

static int monCont;
static void showHelp();
static int test_by_manual();
static void* totalPowerMonitor(void* p);

void showHelp(){
  printf("%s\n", "This is a test program to communicate with Powermeter, Agilent. ");	
  printf("%s\n", "(commands)");
  printf("\t%s\n", "e:");	
  printf("\t\t%s\n", "End the program.");	
  printf("\t%s\n", "h:");	
  printf("\t\t%s\n", "Show this message.");	
  printf("\t%s\n", "#:");	
  printf("\t\t%s\n", "Get important settings.");	
  printf("\t%s\n", "m:");	
  printf("\t\t%s\n", "Start/Stop monitor.");	
  printf("\t\t%s\n", "While a monitor runs, the data are written in a file.");	
}

int test_by_manual(){
  //	int len;                            //  受信データ数（バイト）
  char tmpstr[256];    // データ受信バッファ
  //int res, pact, vact;

  printf("%s\n", "Test start. ");
  int n = 0;
	
  int cont = 1;    
  thrdClass_t* thrd = NULL;
  do{
    n++;
    printf("[powermeter: %d] > ", n);
    fflush(stdout);
	
    fgets(tmpstr, 256, stdin);

    printf("cmd:%s", tmpstr);
	
    //	int value, num;
    //	float preset = 20;
    //	char token[32];
    int len;
    switch(tmpstr[0]){
    case 'e':
      cont = 0;
      break;
    case '#':
      penguin_CmdTxCommand("*IDN?");
      penguin_CmdRxData();
      printf("Received [%s]\n", penguin_chopper_getMessage());
      penguin_CmdTxCommand("SYST:RINT?");
      penguin_CmdRxData();
      printf("Received [%s]\n", penguin_chopper_getMessage());
      penguin_CmdTxCommand("SYST:COMM:GPIB:ADDR?");
      penguin_CmdRxData();
      printf("Received [%s]\n", penguin_chopper_getMessage());
      break;
    case 'h':
      showHelp();
      break;
    case 'm':
      if(thrd){
        monCont = 0;
        thrdStop(thrd);
        printf("Stop the monitor. \n");
        thrdEnd(thrd);
        thrd = NULL;
      }else{
        printf("Start a monitor. \n");
        thrd = thrdInit(totalPowerMonitor, (void*)NULL);
        thrdStart(thrd);
        usleep(1000*1000*1);
      }
      break;

    default:
      len = strlen(tmpstr);
      if(len <= 1){
        break;
      }
      tmpstr[len-1] = '\0';
      penguin_CmdTxCommand(tmpstr);
      if(strstr(tmpstr, "?")){
        penguin_CmdRxData();
        printf("Received [%s]\n", penguin_chopper_getMessage());
      }
      break;
    }
		
  }while(cont);



  printf("%s\n", "Test end. ");

  return 0;
	
}

void* totalPowerMonitor(void* p){
  /*
    int ret = penguin_powermeter_freerun();
    if(ret){
    return ret;
    }
  */
  int ret = penguin_CmdTxCommand("UNIT:POW DBM");
  if(ret){//error
      return NULL;
  }
  ret = penguin_CmdTxCommand("INIT:CONT OFF");
  if(ret){//error
      return NULL;
  }
  ret = penguin_CmdTxCommand("AVER:COUNT:AUTO OFF");
  if(ret){//error
      return NULL;
  }
  char fname[1024];
  FILE* fp;
  time_t t;
  struct tm* stm;	time(&t);
  stm = localtime(&t);
  sprintf(fname, "../log/powermeter-%04d%02d%02d%02d%02d%02d.dat", 
          stm->tm_year + 1900, stm->tm_mon + 1, stm->tm_mday, stm->tm_hour, stm->tm_min, stm->tm_sec);
  fp = fopen(fname, "wb");
  if(!fp){
    fprintf(stderr, "fopen(); file(%s) open error!!", fname);
    return NULL;
  }
  printf("File(%s) opened successfully. \n", fname);

  FILE* gnuplot = popen("/home/nagai/workspace/gnuplot/stdin_gnuplot", "w");
  if(!gnuplot){
    fprintf(stderr, "stdin_gnuplot open error!!");
    return 1;
  }
  printf("stdin_gnuplot opened successfully. \n");

  //double value;
	
  monCont = 1;
  char tmp[1024];
  memset(tmp, 0, sizeof(tmp));
  //	int i;
  //	for(i = 0; i < 100 && cont; i++){
  while(monCont){
    //printf("Acquisition start.\n");
    sprintf(tmp, "AVER:COUNT %d", 1);//1 sec
                                         //		sprintf(tmp, "AVER:COUNT %d", (int)(1 * 20.835 - 3.4832));//1 sec
                                                                                                                      ret = penguin_CmdTxCommand(tmp);
    if(ret){//error
      fprintf(stderr, "AVER:COUNT couldn't send");
      monCont=0;
      break;
    }
    ret = penguin_CmdTxCommand("READ?");
    if(ret){//error
        fprintf(stderr, "READ? couldn't send");
      monCont=0;
      break;
    }
    ret = penguin_CmdRxData();
    if(ret > 0 ){//success
      sprintf(tmp, "%s", penguin_chopper_getMessage());
      //break;
    }else{
      fprintf(stderr, " couldn't obtained a reply for READ?.");
      monCont=0;
      break;
    }
    //printf("%s\n", tmp);
    fprintf(fp, "%s\n", tmp);
    fprintf(gnuplot, "%s\n", tmp);
    /*
      value = penguin_powermeter_freerun_getData();
      printf("%lf\n", value);
      fprintf(fp, "%lf\n", value);
      fprintf(gnuplot, "%lf\n", value);
    */
    //printf("Acquisition end.\n");
    //fflush(stdout);
    fflush(gnuplot);
  }
  printf("End Procedure Phase 1.\n");
  pclose(gnuplot);
  fflush(fp);
  fclose(fp);
  return NULL;
}
/*
  void stopLoop(){
  printf("stopLoop().\n");
  usleep(1000*1000*1);
  cont = 0;
  }

  void callBackAtexit(void){
  printf("callBackAtexit().\n");
  usleep(1000*1000*1);
  //	stopLoop();
  }
*/
/*
  void _sigFunc(int sig){
  switch(sig){
  case SIGTERM:
  case SIGINT:
  callBackAtexit();
  return;
  default:
  return;
  }
  }
*/
/*
  void sigint_handler(int sig){
  stopLoop();
  }
*/
int	main(int argc, char *argv[]) {

  if(penguin_powermeter_init(13)){
    printf("penguin_powermeter_init() error.\n");
    exit(1);
  }


  //	if(argc > 1){
  test_by_manual();
  /*
    }else{
    struct sigaction sa_sigint;
    memset(&sa_sigint, 0, sizeof(sa_sigint));
    sa_sigint.sa_handler = sigint_handler;
    sa_sigint.sa_flags = SA_RESTART;

    if(sigaction(SIGINT, &sa_sigint, NULL)){
    fprintf(stderr, "sigaction(); SIGINT error!!");
    }
    if(sigaction(SIGTERM, &sa_sigint, NULL)){
    fprintf(stderr, "sigaction(); SIGTERM error!!");
    }
    if(atexit(callBackAtexit)){
    fprintf(stderr, "atexit(); error!!");
    return -1;
    }
    int ret = totalPowerMonitor();
    if(ret){
    fprintf(stderr, "totalPowerMonitor() ends with an error.");
    }
  */
  //	}
  printf("End procedure Phase 2.\n");
  //penguin_CmdTxCommand("*RST");

  penguin_powermeter_end();
  //fflush(stdout);
  return 0;
}


