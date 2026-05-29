/* Example application of Columbo Simple Serial Library
 * Copyright 2003 Marcin Siennicki <m.siennicki@cloos.pl>
 * see COPYING file for details */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "cssl.h"


/* if it is time to finish */
static int finished=0;

static uint8_t message[30];
static int messagePtr;

char* nishin(uint8_t uint, char *res)
{
  /* static char res[9]; */
  res[0]=(uint & 0x80)?'1':'0';
  res[1]=(uint & 0x40)?'1':'0';
  res[2]=(uint & 0x20)?'1':'0';
  res[3]=(uint & 0x10)?'1':'0';
  res[4]=(uint & 0x08)?'1':'0';
  res[5]=(uint & 0x04)?'1':'0';
  res[6]=(uint & 0x02)?'1':'0';
  res[7]=(uint & 0x01)?'1':'0';
  res[8]='\0';
  return res;
}
int parity(uint8_t uint)
{
  int res = 0;
  /* toggle the 8th bit; */
  if(uint & 0x80)res ^= 0x01;
  if(uint & 0x40)res ^= 0x01;
  if(uint & 0x20)res ^= 0x01;
  if(uint & 0x10)res ^= 0x01;
  if(uint & 0x08)res ^= 0x01;
  if(uint & 0x04)res ^= 0x01;
  if(uint & 0x02)res ^= 0x01;
  if(uint & 0x01)res ^= 0x01;
  return res;
}
uint8_t check_parity(uint8_t uint, int parity)
{
  int res = uint;
  /* toggle the 1st bit; */
  if(uint & 0x40)res ^= 0x80;
  if(uint & 0x20)res ^= 0x80;
  if(uint & 0x10)res ^= 0x80;
  if(uint & 0x08)res ^= 0x80;
  if(uint & 0x04)res ^= 0x80; 
  if(uint & 0x02)res ^= 0x80;
  if(uint & 0x01)res ^= 0x80;
  if(parity){
    if((res & 0x80) == 0x00)
      perror("even expected, but odd.");
    res ^= 0x80;
  }else{
    if(res & 0x80)
      perror("odd expected, but even.");
  }
  return res;
}

/* example callback, it gets its id, buffer, and buffer length */
static void callback(int id,
                     uint8_t *buf,
                     int length)
{
  int i;
  if(messagePtr == 0){
    memset(message, 0, sizeof(message));
  }
  for(i=0;i<length;i++) {
    switch (buf[i]) {

    default:
      message[messagePtr] = check_parity(buf[i], 1);
      messagePtr++;
    }
    if(message[messagePtr-1]=='\n'){
      if(message[messagePtr-2]!='\r'){
        perror("?");
      }
      printf("message: %s\n", message);
      messagePtr = 0;
      finished=1;
    }
  }
  buf[length] = '\0';
  printf("receive %d bytes.\n", length);

  fflush(stdout);
}

void wait_for_massage_complete()
{
  while (!finished)
    pause();
  finished = 0;
}

int main(int argc, char *argv[]){
  cssl_t *serial;
  cssl_start();

  char* devName;
  devName = "/dev/ttyS0";
  int baudrate = 9600;
  int databit = 7;
  int paritybit = 1;
  int stopbit = 1;

  int n_posarg = 0;
  for (int i = 1; i < argc; i++) {
    if((strcmp(argv[i], "--baudrate") == 0 || strcmp(argv[i], "-b") == 0) && (i+1) < argc) {
      baudrate = atoi(argv[++i]);
    }
    else if((strcmp(argv[i], "--databit") == 0 || strcmp(argv[i], "-d") == 0) && (i+1) < argc) {
      databit = atoi(argv[++i]);
    }
    else if((strcmp(argv[i], "--paritybit") == 0 || strcmp(argv[i], "-p") == 0) && (i+1) < argc) {
      paritybit = atoi(argv[++i]);
    }
    else if((strcmp(argv[i], "--stopbit") == 0 || strcmp(argv[i], "-s") == 0) && (i+1) < argc) {
      stopbit = atoi(argv[++i]);
    }
    else if((strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)) {
      printf("%s\n",argv[0]);
      printf("[devicename]                : default=/dev/ttyS0\n");
      printf("--baudrate  / -b [baudrate] : default=9600\n");
      printf("--databit   / -d [databit]  : default=7\n");
      printf("--paritybit / -p [paritybit]: default=1\n");
      printf("--stopbit   / -p [stopbit]  : default=1\n");
      exit(0);
    }
    else {
      if(n_posarg==0) devName = argv[i];
      else printf("Unknown argument: %s\n", argv[i]);
      n_posarg++;
    }
  }

  printf("open devide \"%s\": baudrate=%d, databit=%d, paritybit=%d, stopbit=%d \n",devName,baudrate,databit,paritybit,stopbit);
  printf("--> HEADER WORD = \"\", TRAILER WORD = \"\\r\\n\" (which is fixed in test_cssl.cpp)\n");
  serial=cssl_open(devName,callback,0,
                   baudrate,databit,paritybit,stopbit);

  if (!serial) {
    printf("%s\n",cssl_geterrormsg());
    return -1;
  }

  cssl_putstring(serial, "*IDN?\r\n");
  printf("> *IDN?\n");
  wait_for_massage_complete();

  printf("we exit after 2 sec.\n");
  usleep(2000*1000);
  cssl_close(serial);
  cssl_stop();

  return 0;
}
