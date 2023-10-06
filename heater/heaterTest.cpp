#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "penguin_heater.h"

int	main(int argc, char *argv[]) {

	if(penguin_heater_init()){
		printf("penguin_heater_init() error.");
		exit(1);
	}


	char cmd[256];
	long bytes_returned;

/*
	cmd[0]=0x02;
	cmd[1]='0';
	cmd[2]='1';
	cmd[3]='R';
	cmd[4]='P';
	cmd[5]='V';
	cmd[6]='1';
	cmd[7]=0x03;
	cmd[8]=0x61;
*/
/*
	cmd[0]=0x02;
	cmd[1]='0';
	cmd[2]='1';
	cmd[3]='R';
	cmd[4]='P';
	cmd[5]='V';
	cmd[6]='1';
	cmd[7]=0x03;
	//penguin_weather_awake();
	penguin_heater_send_frame(cmd, 8);
*/
/*
	penguin_heater_send_get_command(1, "PV1");

	bytes_returned = penguin_heater_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_heater_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}

	penguin_heater_send_get_command(2, "PV1");

	bytes_returned = penguin_heater_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_heater_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}

	penguin_heater_send_get_command(3, "PV1");

	bytes_returned = penguin_heater_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_heater_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}

	penguin_heater_send_get_command(3, "SV1");

	bytes_returned = penguin_heater_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_heater_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}

	penguin_heater_send_set_command(3, "SV1", 250);

	bytes_returned = penguin_heater_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_heater_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}

	penguin_heater_send_get_command(3, "SV1");

	bytes_returned = penguin_heater_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_heater_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}

	penguin_heater_send_set_command(3, "SV1", 200);

	bytes_returned = penguin_heater_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_heater_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}

	penguin_heater_send_get_command(3, "SV1");

	bytes_returned = penguin_heater_receive();
	if (bytes_returned > 0) {
		printf("%s\n", penguin_heater_getMassage());
	}
	else if (bytes_returned == 0) {
		printf("*** [ NOTHING RECEIVED ] ***\n");
	}
	else{
	}
*/
	double temp1 = penguin_heater_get_temperature(1);
	printf("ch1: %.1f degree Celsius\n", temp1);
	double temp2 = penguin_heater_get_temperature(2);
	printf("ch2: %.1f degree Celsius\n", temp2);
	double temp3 = penguin_heater_get_temperature(3);
	printf("ch3: %.1f degree Celsius\n", temp3);

	//	penguin_heater_end();
}
