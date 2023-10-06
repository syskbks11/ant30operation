#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_chopper.h"

#define DEV_NAME "/dev/tty.usbserial-FT2GX65L0"

int main(int argc, char *argv[]) {

  char *devName;
  if(argc == 2){
    devName = argv[1];
  }else{
    devName = DEV_NAME;
  }

  if(penguin_chopper_init(devName)){
    printf("penguin_chopper_init() error.");
    exit(1);
  }


  char cmd[256];
  long bytes_returned;

  penguin_chopper_send_command("?:");
  bytes_returned = penguin_chopper_receive();
  if (bytes_returned > 0) {
    printf("%s\n", penguin_chopper_getMassage());
  }
  else if (bytes_returned == 0) {
    printf("*** [ NOTHING RECEIVED ] ***\n");
  }
  else{
  }
  while(1){
    memset(cmd, 0, 256);		// initialize command string
    //		memset(buf, 0, BUF_LEN);	// initialize buffer
    printf("Input command or query ('q' to exit): ");
    fgets(cmd,256,stdin);
    cmd[strlen(cmd)-1] = 0;		// just gets rid of the \n
    if (cmd[0] == 'q') break;// do not quit when 'Q'
    //		if (strncasecmp(cmd, "q",1) == 0) break;

    if (penguin_chopper_send_command(cmd) < 0){
      printf("penguin_signalG_send_command() error.");
      break;
    }
    bytes_returned = penguin_chopper_receive();
    if (bytes_returned > 0) {
      printf("%s\n", penguin_chopper_getMassage());
    }
    else if (bytes_returned == 0) {
      printf("*** [ NOTHING RECEIVED ] ***\n");
    }
    else{
      break;
    }
  }
  penguin_chopper_end();
}
