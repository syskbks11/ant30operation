
/* Example application of Columbo Simple Serial Library
 * Copyright 2003 Marcin Siennicki <m.siennicki@cloos.pl>
 * see COPYING file for details */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "cssl.h"


/* if it is time to finish */
static int finished=0;

static uint8_t message[30];
static int messagePtr;

char* nishin(uint8_t uint, char *res)
{
	//static char res[9];
	res[0]=(uint & 0x80)?'1':'0';
	res[1]=(uint & 0x40)?'1':'0';
	res[2]=(uint & 0x20)?'1':'0';
	res[3]=(uint & 0x10)?'1':'0';
	res[4]=(uint & 0x08)?'1':'0';
	res[5]=(uint & 0x04)?'1':'0';
	res[6]=(uint & 0x02)?'1':'0';
	res[7]=(uint & 0x01)?'1':'0';
	res[8]='\0';
	return res;
}
int parity(uint8_t uint)
{
	int res = 0;
	if(uint & 0x80)res ^= 0x01;//toggle the 8th bit; 
	if(uint & 0x40)res ^= 0x01;//toggle the 8th bit; 
	if(uint & 0x20)res ^= 0x01;//toggle the 8th bit; 
	if(uint & 0x10)res ^= 0x01;//toggle the 8th bit; 
	if(uint & 0x08)res ^= 0x01;//toggle the 8th bit; 
	if(uint & 0x04)res ^= 0x01;//toggle the 8th bit; 
	if(uint & 0x02)res ^= 0x01;//toggle the 8th bit; 
	if(uint & 0x01)res ^= 0x01;//toggle the 8th bit; 
	return res;
}
uint8_t check_parity(uint8_t uint, int parity)
{
	int res = uint;
	if(uint & 0x40)res ^= 0x80;//toggle the 1st bit; 
	if(uint & 0x20)res ^= 0x80;
	if(uint & 0x10)res ^= 0x80;
	if(uint & 0x08)res ^= 0x80;
	if(uint & 0x04)res ^= 0x80; 
	if(uint & 0x02)res ^= 0x80;
	if(uint & 0x01)res ^= 0x80;
	if(parity){
		if((res & 0x80) == 0x00)
			perror("even expected, but odd.");
		res ^= 0x80;
	}else{
		if(res & 0x80)
			perror("odd expected, but even.");
	}
	return res;
}

/* example callback, it gets its id, buffer, and buffer length */
static void callback(int id,
		     uint8_t *buf,
		     int length)
{
    int i;
	//char res[9];
	if(messagePtr == 0){
		memset(message, 0, sizeof(message));
	}
	for(i=0;i<length;i++) {
		//printf("%d %c %x %s\n", buf[i], buf[i], buf[i], nishin(buf[i], res));
//		printf("%d %c %x %s %d %c\n", buf[i], buf[i], buf[i], nishin(buf[i], res), parity(buf[i]), check_parity(buf[i], 1));

		switch (buf[i]) {

		//case 0x04:  /* Ctrl-D */
			//finished=1;
			//return;

		//case 0x0d:
		//break;
		//case 0x0a:
		default:
			message[messagePtr] = check_parity(buf[i], 1);
//			buf[i] = check_parity(buf[i], 1);
//			message[messagePtr] = buf[i];
			messagePtr++;
		}
		if(message[messagePtr-1]=='\n'){
			if(message[messagePtr-2]!='\r'){
				perror("?");
			}
			printf("message: %s\n", message);
			messagePtr = 0;
			finished=1;
		}


		//putchar(buf[i]);
    	}
	buf[length] = '\0';
	printf("receive %d bytes.\n", length);
	//printf("receive %d bytes: %s\n", length, buf);

	fflush(stdout);
}

void wait_for_massage_complete()
{
	while (!finished)
		pause();
	finished = 0;
}


int main(int argc, char *argv[])
{
	//uint8_t buffer[30];
    cssl_t *serial;

    cssl_start();

    
	char *devName;
	if(argc == 2){
		devName = argv[1];
	}else{
		devName = "/dev/ttyS0";
	}
    serial=cssl_open(devName,callback,0,
		     9600,7,1,1);
//    serial=cssl_open(devName,callback,0,
//		     19200,8,0,1);

    if (!serial) {
	printf("%s\n",cssl_geterrormsg());
	return -1;
    }

	cssl_putstring(serial,"*IDN?\r\n");
	printf("> *IDN?\n");
	wait_for_massage_complete();
	//usleep(2000*1000);
	cssl_putstring(serial,"BAUD?\r\n");
	printf("> BAUD?\n");
	wait_for_massage_complete();
	//usleep(2000*1000);
	cssl_putstring(serial,"KRDG? 1\r\n");
	printf("> KRDG? 1\n");
	wait_for_massage_complete();
	//usleep(2000*1000);
	//cssl_getdata(serial, buffer, sizeof(buffer));
	//printf("obtained: %s\n", buffer);
    //while (!finished)
	//pause();

    printf("we exit after 2 sec.\n");
	usleep(2000*1000);
    cssl_close(serial);
    cssl_stop();

    return 0;
}
