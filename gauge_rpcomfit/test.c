#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../libcssl-0.9.4/cssl.h"
#include "../serial/glacier_serial.h"
#include "../debug/glacier_debug.h"

int
main(int argc,char *argv[])
{
  //uint8_t buffer[30];
  cssl_t *serial;

  int id=0;

  cssl_start();
  glacier_debug_size();
  printf("cssl_t:%d\n", sizeof(cssl_t));
  printf("glacier_serial_t:%d\n", sizeof(glacier_serial_t));

    
  char *devName;
  if(argc == 2){
    devName = argv[1];
  }else{
    devName = "/dev/ttyS0";
  }
  serial=cssl_open(devName,glacier_serial_callback_CRLF,id,
                   9600,8,0,1);
  //    serial=cssl_open(devName,callback,0,
  //		     19200,8,0,1);

  if (!serial) {
    printf("%s\n",cssl_geterrormsg());
    return -1;
  }
  int res = glacier_serial_attach(serial, GLACIER_SERIAL_1ST_NONE);
  //int res = glacier_serial_attach(serial, GLACIER_SERIAL_1ST_ODD);
  if(res){
    printf("already attached. ");
  }
  do{
    cssl_putstring(serial,"TID\r\n");
    printf("> TID\n");
    glacier_serial_wait_for_massage_complete(id);
  }while(glacier_serial_get_massage(id)[0]!=6);

  cssl_putchar(serial,5);
  printf("> <ENQ>\n");
  glacier_serial_wait_for_massage_complete(id);

  do{
    cssl_putstring(serial,"PR1\r\n");
    printf("> PR1\n");
    glacier_serial_wait_for_massage_complete(id);
  }while(glacier_serial_get_massage(id)[0]!=6);
  cssl_putchar(serial,5);
  printf("> <ENQ>\n");
  glacier_serial_wait_for_massage_complete(id);

  do{
    cssl_putstring(serial,"COM,1\r\n");
    printf("> COM,1\n");
    glacier_serial_wait_for_massage_complete(id);
  }while(glacier_serial_get_massage(id)[0]!=6);

  printf("we measure for 10 sec.\n");
  int i;
  for (i=0; i < 10; i++){
    glacier_serial_wait_for_massage_complete(id);
  }
  //usleep(10000*1000);
  cssl_putchar(serial,5);
  printf("> <ENQ>\n");
  printf("we wait for 3 sec.\n");
  usleep(3000*1000);

  do{
    cssl_putstring(serial,"PR1\r\n");
    printf("> PR1\n");
    glacier_serial_wait_for_massage_complete(id);
  }while(glacier_serial_get_massage(id)[0]!=6);
  cssl_putchar(serial,5);
  printf("> <ENQ>\n");
  glacier_serial_wait_for_massage_complete(id);

  printf("we exit after 2 sec.\n");
  usleep(2000*1000);
  cssl_close(serial);
  cssl_stop();

  return 0;
}
