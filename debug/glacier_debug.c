#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "glacier_debug.h"

/**
 * Converts uint8_t to binary. 
 */
char* glacier_debug_nishin(uint8_t uint, char *res)
{
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

/**
 * Calculates the parity of an uint8_t.
 * @return 0 even, 1 odd. 
 */
int glacier_debug_parity(uint8_t uint)
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

void glacier_debug_size()
{
	printf("char\t:%d\n", sizeof(char));
	printf("short\t:%d\n", sizeof(short));
	printf("int\t:%d\n", sizeof(int));
	printf("long\t:%d\n", sizeof(long));
	printf("float\t:%d\n", sizeof(float));
	printf("double\t:%d\n", sizeof(double));
}
