#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "penguin_weather2.h"

int	main(int argc, char *argv[]) {

	if(penguin_weather_init("/dev/ttyUSB10")){
		printf("penguin_weather_init() error.");
		exit(1);
	}


//	char cmd[256];
	long bytes_returned;
/*
	cmd[0]=0x11;
	cmd[1]=0x00;
*/
/*
	penguin_weather_send_frame(cmd, 1);
*/
	printf("Test #1\n");
	penguin_weather_send(0x11);
	bytes_returned = penguin_weather_receive(2);
/*
	bytes_returned = penguin_weather_receive();
	bytes_returned = penguin_weather_receive();
*/
/*
	printf("\nTest #2\n");
	penguin_weather_send(0x05);
	bytes_returned = penguin_weather_receive();
*/
	printf("\nTest #3\n");
	penguin_weather_send(0x0B);
	bytes_returned = penguin_weather_receive(10);

	printf("\nTest #4\n");
	double* data = penguin_weather_getData();
	printf("T1 = %lf deg, T2 = %lf deg. \n", data[0], data[1]);
/*
	bytes_returned = penguin_weather_receive();
	bytes_returned = penguin_weather_receive();
	bytes_returned = penguin_weather_receive();
	bytes_returned = penguin_weather_receive();
	bytes_returned = penguin_weather_receive();
	bytes_returned = penguin_weather_receive();
	bytes_returned = penguin_weather_receive();
	bytes_returned = penguin_weather_receive();
	bytes_returned = penguin_weather_receive();
	bytes_returned = penguin_weather_receive();
*/
/*
	penguin_weather_send_simple_command(0x33, 0x00);

	penguin_weather_send_simple_command(0x36, 0x00);
		bytes_returned = penguin_weather_receive();
		if (bytes_returned > 0) {
			printf("%s\n", penguin_weather_getMassage());
		}
		else if (bytes_returned == 0) {
			printf("*** [ NOTHING RECEIVED ] ***\n");
		}
		else{
		}
	penguin_weather_send_simple_command(0x39, 0x00);
		bytes_returned = penguin_weather_receive();
		if (bytes_returned > 0) {
			printf("%s\n", penguin_weather_getMassage());
		}
		else if (bytes_returned == 0) {
			printf("*** [ NOTHING RECEIVED ] ***\n");
		}
		else{
		}
*/
/*
	double* data = penguin_weather_getData();
	if(data == NULL){
		printf("Oh, sh..");
	}
		
	printf("temperature = %f deg C, humidity = %f %, pressure = %f hPa\n", data[0], data[1], data[2]);
*/
	penguin_weather_end();
	//fflush(stdout);
}
