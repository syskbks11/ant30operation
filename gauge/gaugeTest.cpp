#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_gauge.h"

int	main(int argc, char *argv[]) {

	if(penguin_gauge_init()){
		printf("penguin_gauge_init() error.");
		exit(1);
	}


	char cmd[256];
	long bytes_returned;

	penguin_gauge_send("TID");

/*
		bytes_returned = penguin_gauge_receive();
		if (bytes_returned > 0) {
			printf("%s\n", penguin_gauge_getMessage());
		}
		else if (bytes_returned == 0) {
			printf("*** [ NOTHING RECEIVED ] ***\n");
		}
		else{
		}
*/
	if(penguin_gauge_receiveAcknowledge()){
		printf("penguin_gauge_receiveAcknowledge() failed. \n");
		exit(1);
	}
	if(penguin_gauge_sendEnquery()){
		printf("penguin_gauge_sendEnquery() failed. \n");
		exit(1);
	}

	bytes_returned = penguin_gauge_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_gauge_getMessage());
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

		if (penguin_gauge_send(cmd) < 0){
			printf("penguin_gauge_send() error.");
			break;
		}
		if(penguin_gauge_receiveAcknowledge()){
			printf("penguin_gauge_receiveAcknowledge() failed. \n");
			exit(1);
		}
		if(penguin_gauge_sendEnquery()){
			printf("penguin_gauge_sendEnquery() failed. \n");
			exit(1);
		}
		bytes_returned = penguin_gauge_receive();
		if (bytes_returned > 0) {
			printf("%s\n", penguin_gauge_getMessage());
		}
		else if (bytes_returned == 0) {
			printf("*** [ NOTHING RECEIVED ] ***\n");
		}
		else{
			break;
		}
	}
	penguin_gauge_end();
}
