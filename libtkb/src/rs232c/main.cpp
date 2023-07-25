/*!
\file rs232c/main.cpp
\author Y.Koide
\date 2006.08.15
\brief rs232cテストプログラム
*/
#include <stdio.h>
#include <string.h>
#include "rs232c.h"

main(){
  unsigned char buf[1024];
  int ret;

  ret=rs232cInit(1,4800,8,0,1);
  if(ret<0){
    printf("Com open error[%d]!!\n",ret);
    return -1;
  }
  while(1){
    ret=rs232cRead(1,buf,1023);
    if(ret>0){
      buf[ret]='\0';
      printf("%s",buf);
    }
  }
  rs232cEnd(1);
  /*
  char tmp[256];
  char* buf = new char[1024];

  //DBサンプル
  rs232cInit(2,4800,7,1,1);
  getchar();
  strcpy(buf,"  1, 1, DF");
  buf[0]=0x02;
  buf[7]=0x03;
  buf[10]=0x0d;
  buf[11]=0x0a;
  int i=rs232cWrite(2,(unsigned char*)buf,12);

  getchar();
  i=rs232cRead(2,(unsigned char*)buf,1023);
  buf[i]='\0';
  printf("%s\n",buf);
  rs232cEnd(2);
  delete[] buf;
  */

  //ANELVAサンプル
  /*rs232cInit(2,9600,8,0,1);
  strcpy(buf,"$O\r");
  i=rs232cWrite(2,(unsigned char*)buf,5);
  getchar();
  i=rs232cRead(2,(unsigned char*)buf,1023);
  buf[i]='\0';
  printf("%s\n",buf);
  */
  /*while(1){
  printf(">");
  scanf("%256s",tmp);
  int cnt=strlen(tmp);
  tmp[cnt]=0x0D;
  tmp[cnt+1]='\0';
  printf("%s [%dByte]OK?",tmp,strlen(tmp));
  getchar();
  getchar();
  rs232cWrite((unsigned char*)tmp,strlen(tmp)-1);
  int i=rs232cRead(buf, 1023);
  buf[i]='\0';
  printf("<%s\n",buf);
  }*/
  //rs232cEnd(2);
  getchar();
}
