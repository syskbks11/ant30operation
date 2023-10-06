#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "penguin_thermocon.h"

int	main(int argc, char *argv[]) {

	if(penguin_thermocon_init("/dev/ttyUSB9")){
		printf("penguin_thermocon_init() error.");
		exit(1);
	}


	char cmd[256];
	long bytes_returned;
/*
	cmd[0]=0x05;
	cmd[1]=0x31;
	cmd[2]=0x33;
	cmd[3]=0x31;
	cmd[4]=0x0D;
	cmd[5]=0x00;
	penguin_thermocon_send_frame(cmd, 5);
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

	penguin_thermocon_send_get_command(0x31);
	bytes_returned = penguin_thermocon_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_thermocon_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}
	penguin_thermocon_send_get_command(0x32);
	bytes_returned = penguin_thermocon_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_thermocon_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}
	penguin_thermocon_send_get_command(0x33);
	bytes_returned = penguin_thermocon_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_thermocon_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}
	penguin_thermocon_send_get_command(0x34);
	bytes_returned = penguin_thermocon_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_thermocon_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}
	penguin_thermocon_send_set_command(0x31, 250);
	bytes_returned = penguin_thermocon_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_thermocon_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}

	int preset = penguin_thermocon_getPresetTemperature();
	printf("preset: %d\n", preset);
	preset = penguin_thermocon_getInsideTemperature();
	printf("inside: %d\n", preset);
	preset = penguin_thermocon_getOutsideTemperature();
	printf("outside: %d\n", preset);
	preset = penguin_thermocon_getAlerm();
	printf("alerm: %d\n", preset);
/*
	double* data = penguin_weather_getData();
	if(data == NULL){
		printf("Oh, sh..");
	}
		
	printf("temperature = %f deg C, humidity = %f %, pressure = %f hPa\n", data[0], data[1], data[2]);
*/
	penguin_thermocon_end();
}
