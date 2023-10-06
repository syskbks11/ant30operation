#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "penguin_weather.h"

int	main(int argc, char *argv[]) {

	if(penguin_weather_init("/dev/ttyUSB4")){
		printf("penguin_weather_init() error.");
		exit(1);
	}


	char cmd[256];
	long bytes_returned;
/*
	cmd[0]=0x01;
	cmd[1]=0x33;
	cmd[2]=0x00;
	cmd[3]=0x04;
	cmd[4]=0x00;
	cmd[5]=0x00;
	cmd[6]=0x00;
	cmd[7]=0x00;
	cmd[8]=0x00;
	cmd[9]=0x38;
	cmd[10]=0x00;
*/
/*
	cmd[0]=0x01;
	cmd[1]=0x35;
	cmd[2]=0x00;
	cmd[3]=0x04;
	cmd[4]=0x00;
	cmd[5]=0x00;
	cmd[6]=0x00;
	cmd[7]=0x00;
	cmd[8]=0x00;
	cmd[9]=0x3a;
	cmd[10]=0x00;
*/
/*
	cmd[0]=0x01;
	cmd[1]=0x35;
	cmd[2]=0x00;
	cmd[3]=0x04;
	cmd[4]=0x00;
	cmd[5]=0x00;
	cmd[6]=0x00;
	cmd[7]=0x00;
	cmd[8]=0x00;
	cmd[9]=0x3a;
	cmd[10]=0x00;
*/
/*
	cmd[0]=0x01;
	cmd[1]=0x35;
	cmd[2]=0x04;
	cmd[3]=0x00;
	cmd[4]=0x00;
	cmd[5]=0x00;
	cmd[6]=0x00;
	cmd[7]=0x00;
	cmd[8]=0x3a;
	cmd[9]=0x00;
*/
/*
	cmd[0]=0x01;
	cmd[1]=0x35;
	cmd[2]=0x00;
	cmd[3]=0x02;
	cmd[4]=0x00;
	cmd[5]=0x00;
	cmd[6]=0x00;
	cmd[7]=0x38;
	cmd[8]=0x00;
*/
/*
	cmd[0]=0x01;
	cmd[1]=0x36;
	cmd[2]=0x00;
	cmd[3]=0x04;
	cmd[4]=0x00;
	cmd[5]=0x00;
	cmd[6]=0x00;
	cmd[7]=0x00;
	cmd[8]=0x00;
	cmd[9]=0x3b;
	cmd[10]=0x00;
*/
/*
	cmd[0]=0x01;
	cmd[1]=0x36;
	cmd[2]=0x04;
	cmd[3]=0x00;
	cmd[4]=0x00;
	cmd[5]=0x00;
	cmd[6]=0x00;
	cmd[7]=0x00;
	cmd[8]=0x3b;
	cmd[9]=0x00;
*/
/* wrong
	snprintf(cmd, 11, "\x01\x36\x00\x04\x00\x00\x00\x00\x00\x3b\x00");
*/
/*
	cmd[0]=0x00;
	cmd[1]=0x01;
	cmd[2]=0x30;
	cmd[3]=0x00;
	cmd[4]=0x04;
	cmd[5]=0x00;
	cmd[6]=0x00;
	cmd[7]=0x00;
	cmd[8]=0x00;
	cmd[9]=0x00;
	cmd[10]=0x35;
	cmd[11]=0x00;
*/
/*
	cmd[0]=0x00;
	cmd[1]=0x01;
	cmd[2]=0x40;
	cmd[3]=0x00;
	cmd[4]=0x04;
	cmd[5]=0x00;
	cmd[6]=0x00;
	cmd[7]=0x00;
	cmd[8]=0x00;
	cmd[9]=0x00;
	cmd[10]=0x45;
	cmd[11]=0x00;
*/
/*
	cmd[0]=0x00;
	cmd[1]=0x01;
	cmd[2]=0x33;
	cmd[3]=0x00;
	cmd[4]=0x04;
	cmd[5]=0x00;
	cmd[6]=0x00;
	cmd[7]=0x00;
	cmd[8]=0x00;
	cmd[9]=0x00;
	cmd[10]=0x38;
	cmd[11]=0x00;
*/
/*
	unsigned int sum=0;
	int i;
	for(i=0; i < 8; i++){
		sum +=(unsigned int)cmd[i];
	}
	int sum1 = (int)(sum/10);
	int sum2 = sum-sum1*10;
	cmd[9]=(char)sum1;
	cmd[10]=(char)sum2;
	cmd[11]='\0';
*/
	//penguin_weather_awake();
	//usleep(1000*1000);
/*
	penguin_weather_send_frame(cmd, 11);
*/
/*
	penguin_weather_send_simple_command(0x33, 0x00);

		bytes_returned = penguin_weather_receive();
		if (bytes_returned > 0) {
			printf("%s\n", penguin_weather_getMassage());
		}
		else if (bytes_returned == 0) {
			printf("*** [ NOTHING RECEIVED ] ***\n");
		}
		else{
		}
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
	double* data = penguin_weather_getData();
	if(data == NULL){
		printf("Oh, sh..");
	}
		
	printf("temperature = %f deg C, humidity = %f %, pressure = %f hPa\n", data[0], data[1], data[2]);
	penguin_weather_end();
}
