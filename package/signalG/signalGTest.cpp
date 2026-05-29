#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_signalG.h"

int	main(int argc, char *argv[]) {

	if(penguin_signalG_init()){
		printf("penguin_signalG_init() error.");
		exit(1);
	}


	char cmd[256];
	long bytes_returned;

	while(1){
		memset(cmd, 0, 256);		// initialize command string
//		memset(buf, 0, BUF_LEN);	// initialize buffer
		printf("Input command or query ('q' to exit): ");
		fgets(cmd,256,stdin);
		cmd[strlen(cmd)-1] = 0;		// just gets rid of the \n
		if (strncasecmp(cmd, "q",1) == 0) break;

		if (penguin_signalG_send_command(cmd) < 0){
			printf("penguin_signalG_send_command() error.");
			break;
		}
		if (strstr(cmd, "?") != 0) {
			bytes_returned = penguin_signalG_receive();
			if (bytes_returned > 0) {
				printf("%s\n", penguin_signalG_getMassage());
			}
			else if (bytes_returned == 0) {
				printf("*** [ NOTHING RECEIVED ] ***\n");
			}
			else{
				break;
			}
		}
	}
}
