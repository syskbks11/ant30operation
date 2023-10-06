#include "../libvxi11/vxi11_user.h"

#include "penguin_signalG.h"

#define BUF_LEN 100000


/*
 * singleton, since we have only one signal generator. 
 */
typedef struct penguin_signalG_s{
	char	device_ip[64];
	CLINK		*clink;
	char	message[256];
}penguin_signal_t;

static penguin_signal_t p;

int penguin_signalG_init(){

	p.clink = new CLINK;

	memset(p.device_ip, 0, 64);
	memset(p.message, 0, 256);

	sprintf(p.device_ip, "192.168.11.%d", 4);

	int ret=vxi11_open_device(p.device_ip, p.clink);

	if (ret != 0) {
		printf("Error: could not open device %s, quitting\n", p.device_ip);
		exit(2);
		}

}

int penguin_signalG_end(){
	int ret=vxi11_close_device(p.device_ip, p.clink);
	return 0;
}

void penguin_signalG_setFrequency(double value){
	char tmp[256];
	printf("penguin_signalG_setFrequency(%f) implemented?\n", value);
	sprintf(tmp, "FREQ %.10e GHz", value);
	penguin_signalG_send_command(tmp);

//	printf("penguin_signalG_setFrequency(%f) not implemented yet.\n", value);
}

void penguin_signalG_setPower(double value){
	char tmp[256];
	printf("penguin_signalG_setPower(%f) implemented?\n", value);
	sprintf(tmp, "POW:AMPL %.2e dBm", value);
	penguin_signalG_send_command(tmp);

//	printf("penguin_signalG_setPower(%f) not implemented yet.\n", value);
}

double penguin_signalG_getFrequency(){
	long int ret = penguin_signalG_send_and_receive("FREQ:CW?");
	if(ret == 0)
		return 0.0;
	return strtod(p.message, NULL);
}
double penguin_signalG_getPower(){
	int ret = penguin_signalG_send_and_receive("POW:AMPL?");
	if(ret == 0)
		return 0.0;
	return strtod(p.message, NULL);
}

void penguin_signalG_setOutput(int isOn){
	char tmp[256];
	printf("penguin_signalG_setOutput(%d) implemented?\n", isOn);
	if(isOn){
		sprintf(tmp, "OUTP:STAT ON");
	}else{
		sprintf(tmp, "OUTP:STAT OFF");
	}
	penguin_signalG_send_command(tmp);

//	printf("penguin_signalG_setPower(%f) not implemented yet.\n", value);
}

/* medium level API */
int penguin_signalG_send_and_receive_int(const char* cmd){
}


/* low level API */
long penguin_signalG_send_and_receive(const char* cmd){
	if (strstr(cmd, "?") == 0) {
		printf("penguin_singalG_send_and_receive(), command(%s) is not query!\n");
		return -1;
	}

	penguin_signalG_send_command(cmd);

	long bytes_returned = vxi11_receive(p.clink, p.message, 256);
	if (bytes_returned == -15) {
		//printf("*** [ NOTHING RECEIVED ] ***\n");
		return 0;
	}

	if (bytes_returned > 0) {
		//printf("%s\n", p.message);
	}
	return bytes_returned;
}

long penguin_signalG_receive(){

	long bytes_returned = vxi11_receive(p.clink, p.message, 256);
	if (bytes_returned == -15) {
		//printf("*** [ NOTHING RECEIVED ] ***\n");
		return 0;
	}

	if (bytes_returned > 0) {
		//printf("%s\n", p.message);
	}
	return bytes_returned;
}

int penguin_signalG_send_command(const char* cmd){
	if (vxi11_send(p.clink, cmd) < 0){
		printf("penguin_singalG_send_command(%s) error.\n", cmd);
		return -1;
	}
	return 0;
}

char* penguin_signalG_getMassage(){
	return p.message;
}


