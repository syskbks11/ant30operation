#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "penguin_weather_tkb32.h"

int	main(int argc, char *argv[]) {

	if(penguin_weather_tkb32_init("/dev/ttyUSB0")){
		printf("penguin_weather_tkb32_init() error.");
		exit(1);
	}


	long bytes_returned;

	int isContinue = 1;
	int i = 0;
	while(isContinue){
		bytes_returned = penguin_weather_tkb32_receive();
		if (bytes_returned > 0) {
			printf("%s\n", penguin_weather_tkb32_getMassage());
		}
		else if (bytes_returned == 0) {
			printf("*** [ NOTHING RECEIVED ] ***\n");
		}
		else{
		}
		usleep(1000*100);
		if(i >= 10){
			isContinue = 0;
		}
	}
	penguin_weather_tkb32_end();
}
