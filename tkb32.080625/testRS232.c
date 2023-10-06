#include <stdio.h>
#include "../rs232c/rs232c.h"

int main(){
  unsigned char buf[1024];
  int ret;
  int com=1;

  /*ret=rs232cInit(com,4800,8,0,1);*/
  ret=rs232cInit(com,1200,8,2,2);
  if(ret<0){
    printf("%d\n",ret);
    return 0;
  }
  while(1){
    ret=rs232cRead(com, buf, 1023);
    if(ret>0){
      buf[ret]=0;
      printf("%s",buf);
    }
    else{
      printf("no input[%d]\n",ret);
    }
  }
  rs232cEnd(com);
  return 0;
}
