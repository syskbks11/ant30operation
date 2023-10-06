#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "glacier_serial.h"
#include "../../debug/glacier_debug.h"


//core
typedef struct glacier_serial_s{
	cssl_t *cssl;//4 byte
	uint8_t message[GLACIER_SERIAL_MAX_MESSASE_LETTER_NUM];//30 byte
	uint8_t messageTmp[GLACIER_SERIAL_MAX_MESSASE_LETTER_NUM];//30 byte
	int modeFirstBit;//4 byte
	int flgFinish;//4 byte
	unsigned int receivedLetters;//4 byte
}glacier_serial_t;//76 byte

static int _checkID(int id);


uint8_t glacier_serial_1st_bit(uint8_t uint, int mode)
{
	if(mode == GLACIER_SERIAL_1ST_NONE)
		return uint;
	if(mode == GLACIER_SERIAL_1ST_IGNORE){
		uint = uint >> 1;
		return uint << 1;
	}
		
	int res = uint;
	if(uint & 0x40)res ^= 0x80;//toggle the 1st bit; 
	if(uint & 0x20)res ^= 0x80;
	if(uint & 0x10)res ^= 0x80;
	if(uint & 0x08)res ^= 0x80;
	if(uint & 0x04)res ^= 0x80; 
	if(uint & 0x02)res ^= 0x80;
	if(uint & 0x01)res ^= 0x80;
	if(mode == GLACIER_SERIAL_1ST_EVEN){
		if((res & 0x80) == 0x00)
			perror("even expected, but odd.");
		res ^= 0x80;
	}else if(mode == GLACIER_SERIAL_1ST_ODD){
		if(res & 0x80)
			perror("odd expected, but even.");
	}else{
		perror("glacier_serial_1st_bit(): invalid mode.");
	}
	return res;
}


//static glacier_serial_t (*serial_t_ptr)[GLACIER_SERIAL_MAX_NUM];	//ポインタ配列
static glacier_serial_t* serial_t_ptr[GLACIER_SERIAL_MAX_NUM];	//ポインタ配列


static int currentNumberOfPorts = 1;

int glacier_serial_getIDforNewPort(){
	if(currentNumberOfPorts == GLACIER_SERIAL_MAX_NUM -1){
		printf("glacier_serial_getIDforNewPort(); no more ports\n");
	}
	return currentNumberOfPorts++;//returns currentNumberOfPorts, and then add 1 to it.
}


/**
 *
 */
int glacier_serial_attach(cssl_t *cssl, int mode)
{
	if(serial_t_ptr[cssl->id] != 0){//代入済み
		return 1;
	}
	printf("glacier_serial_attach(): attach id=%d\n", cssl->id);
	serial_t_ptr[cssl->id] = (glacier_serial_t*)malloc(sizeof(glacier_serial_t));
	/* initialize */
	serial_t_ptr[cssl->id]->cssl = cssl;
	memset(serial_t_ptr[cssl->id]->message, 0, sizeof(serial_t_ptr[cssl->id]->message));
	memset(serial_t_ptr[cssl->id]->messageTmp, 0, sizeof(serial_t_ptr[cssl->id]->messageTmp));
	serial_t_ptr[cssl->id]->modeFirstBit = mode;
	serial_t_ptr[cssl->id]->flgFinish = 0;
	serial_t_ptr[cssl->id]->receivedLetters = 0;
/*
	glacier_serial_t serial = *serial_t_ptr[cssl->id];
	serial.cssl = cssl;
	memset(serial.message, 0, sizeof(serial.message));
	memset(serial.messageTmp, 0, sizeof(serial.messageTmp));
	serial.modeFirstBit = mode;
	serial.flgFinish = 0;
	serial.receivedLetters = 0;
*/

	return 0;//normal end
}

/**
 *
 */
int glacier_serial_dettach(int id)
{
	if(serial_t_ptr[id] == NULL){//空っぽ
		return 1;
	}
	printf("glacier_serial_dettach(): dettach id=%d\n", id);
	cssl_close(serial_t_ptr[id]->cssl);
	printf("y.\n");

	free(serial_t_ptr[id]);
	printf("y.\n");
	serial_t_ptr[id] = NULL;

	return 0;//normal end
}

/**
 *
 */
int glacier_serial_wait_for_massage_complete(int id)
{
	printf("glacier_serial_wait_for_massage_complete(%d)\n", id);
	if(_checkID(id))return -1;
/*
	if(serial_t_ptr[id] == NULL){
		perror("not attached yet.");
		return -1;
	}
*/
	//printf("glacier_serial_wait_for_massage_complete(): %d\n", (*serial_t_ptr[id]).flgFinish);
	while (!(*serial_t_ptr[id]).flgFinish){
		printf("pause()!\n");fflush(stdout);
		pause();
	}
	(*serial_t_ptr[id]).flgFinish = 0;
	printf("glacier_serial_wait_for_massage_complete(%d) returns\n", id);fflush(stdout);
	return 0;
}
/**
 *
 */
int glacier_serial_is_massage_complete(int id)
{
	printf("glacier_serial_is_massage_complete(%d)\n", id);
	if(_checkID(id))return -1;
	return (*serial_t_ptr[id]).flgFinish;
}

/**
 *
 */
void glacier_serial_clear_massage_complete(int id)
{
	(*serial_t_ptr[id]).flgFinish = 0;
}

/**
 * timeout [sec] but rounded in 0.1 sec
 */
int glacier_serial_wait_for_massage_complete_timeout(int id, double timeout)
{
	printf("glacier_serial_wait_for_massage_complete_timeout(%d, %f)\n", id, timeout);
	
	if(_checkID(id))return -1;
	int timeoutInt = (int)(timeout*10); 
	cssl_settimeout(serial_t_ptr[id]->cssl, timeoutInt);
	//printf("glacier_serial_wait_for_massage_complete(): %d\n", (*serial_t_ptr[id]).flgFinish);
	while ((*serial_t_ptr[id]).flgFinish == 0 && timeoutInt > 0){
		printf("usleep(1000)! with timeout %d\n", timeoutInt);fflush(stdout);
		usleep(1000*100);
		timeoutInt--;
/*
		printf("pause()!\n");fflush(stdout);
		pause();
*/
	}
	(*serial_t_ptr[id]).flgFinish = 0;
	//printf("glacier_serial_wait_for_massage_complete_timeout(%d) returns\n", id);fflush(stdout);
	return 0;
}

static unsigned int nToRead;
void glacier_serial_set_bytes_to_read(unsigned int num)
{
	nToRead = num;
}
void glacier_serial_callback_FIXED(int id, uint8_t *buf, int length)
{
	int i;

	//char res[9];
	
	//printf("glacier_serial_callback_CRLF(): id=%d\n", id);fflush(stdout);
	if(serial_t_ptr[id] == 0){
		perror("not attached");
	}
	//glacier_serial_t serial = *serial_t_ptr[id];
	//printf("glacier_serial_callback_CRLF(): length=%d\n", length);fflush(stdout);
	if(length > GLACIER_SERIAL_MAX_MESSASE_LETTER_NUM) {
		fprintf(stderr, "glacier_serial_callback_CR[id=%d]: terrible!\n", id);
	}
	//printf("glacier_serial_callback_CRLF(): serial.receivedLetters=%d\n", serial_t_ptr[id]->receivedLetters);fflush(stdout);
	if(serial_t_ptr[id]->receivedLetters == 0){
		memset(serial_t_ptr[id]->messageTmp, 0, sizeof(serial_t_ptr[id]->messageTmp));
	}
	//printf(".");fflush(stdout);
	for(i=0;i<length;i++) {
		//printf("%d %c %x %s\n", buf[i], buf[i], buf[i], glacier_debug_nishin(buf[i], res));
//		printf("%d %c %x %s %d %c\n", buf[i], buf[i], buf[i], nishin(buf[i], res), parity(buf[i]), check_parity(buf[i], 1));

		serial_t_ptr[id]->messageTmp[serial_t_ptr[id]->receivedLetters] = glacier_serial_1st_bit(buf[i], serial_t_ptr[id]->modeFirstBit);
		//printf("glacier_serial_callback_CRLF(): serial.received=%d\n", serial.messageTmp[serial.receivedLetters]);fflush(stdout);
//		buf[i] = check_parity(buf[i], 1);
//		message[messagePtr] = buf[i];
		serial_t_ptr[id]->receivedLetters++;
		//printf("glacier_serial_callback_CRLF(): serial.receivedLetters=%d\n", serial_t_ptr[id]->receivedLetters);fflush(stdout);

		//printf("(%x)", buf[i]);
		//putchar(buf[i]);
    	}
	if(serial_t_ptr[id]->receivedLetters >= nToRead){
		//printf("how? : %d %d\n", serial_t_ptr[id]->receivedLetters, nToRead);
		memcpy(serial_t_ptr[id]->message, serial_t_ptr[id]->messageTmp, serial_t_ptr[id]->receivedLetters);
		//strncpy(serial_t_ptr[id]->message, serial_t_ptr[id]->messageTmp, serial_t_ptr[id]->receivedLetters);
		serial_t_ptr[id]->message[serial_t_ptr[id]->receivedLetters] = '\0';
		//printf("message[%d]: %s\n", id, serial_t_ptr[id]->message);
		serial_t_ptr[id]->receivedLetters = 0;
		serial_t_ptr[id]->flgFinish=1;
	}
	printf("receive %d bytes[%d].\n", length, id);
	//printf("receive %d bytes: %s\n", length, buf);

	fflush(stdout);
}
/**
 *
 */
void glacier_serial_callback_DEFAULT(int id, uint8_t *buf, int length)
{
	int i;

	//char res[9];
	
	//printf("glacier_serial_callback_CRLF(): id=%d\n", id);fflush(stdout);
	if(serial_t_ptr[id] == 0){
		perror("not attached");
	}
	//glacier_serial_t serial = *serial_t_ptr[id];
	//printf("glacier_serial_callback_CRLF(): length=%d\n", length);fflush(stdout);
	if(length > GLACIER_SERIAL_MAX_MESSASE_LETTER_NUM) {
		fprintf(stderr, "glacier_serial_callback_CR[id=%d]: terrible!\n", id);
	}
	//printf("glacier_serial_callback_CRLF(): serial.receivedLetters=%d\n", serial_t_ptr[id]->receivedLetters);fflush(stdout);
	if(serial_t_ptr[id]->receivedLetters == 0){
		memset(serial_t_ptr[id]->messageTmp, 0, sizeof(serial_t_ptr[id]->messageTmp));
	}
	//printf(".");fflush(stdout);
	for(i=0;i<length;i++) {
		//printf("%d %c %x %s\n", buf[i], buf[i], buf[i], glacier_debug_nishin(buf[i], res));
//		printf("%d %c %x %s %d %c\n", buf[i], buf[i], buf[i], nishin(buf[i], res), parity(buf[i]), check_parity(buf[i], 1));

		serial_t_ptr[id]->messageTmp[serial_t_ptr[id]->receivedLetters] = glacier_serial_1st_bit(buf[i], serial_t_ptr[id]->modeFirstBit);
		//printf("glacier_serial_callback_CRLF(): serial.received=%d\n", serial.messageTmp[serial.receivedLetters]);fflush(stdout);
//		buf[i] = check_parity(buf[i], 1);
//		message[messagePtr] = buf[i];
		serial_t_ptr[id]->receivedLetters++;
		//printf("glacier_serial_callback_CRLF(): serial.receivedLetters=%d\n", serial_t_ptr[id]->receivedLetters);fflush(stdout);

		//printf("(%x)", buf[i]);
		//putchar(buf[i]);
    	}
	memcpy(serial_t_ptr[id]->message, serial_t_ptr[id]->messageTmp, serial_t_ptr[id]->receivedLetters);
	//strncpy(serial_t_ptr[id]->message, serial_t_ptr[id]->messageTmp, serial_t_ptr[id]->receivedLetters);
	serial_t_ptr[id]->message[serial_t_ptr[id]->receivedLetters] = '\0';
	printf("message[%d]: %s\n", id, serial_t_ptr[id]->message);
	serial_t_ptr[id]->receivedLetters = 0;
	printf("receive %d bytes[%d].\n", length, id);
	//printf("receive %d bytes: %s\n", length, buf);
	serial_t_ptr[id]->flgFinish=1;

	fflush(stdout);
}
/**
 *
 */
void glacier_serial_callback_CR(int id, uint8_t *buf,int length)
{
	int i;

	printf("glacier_serial_callback_CR(): id=%d\n", id);fflush(stdout);
	if(serial_t_ptr[id] == 0){
		perror("not attached");
	}
	if(length > GLACIER_SERIAL_MAX_MESSASE_LETTER_NUM) {
		fprintf(stderr, "glacier_serial_callback_CR[id=%d]: terrible!\n", id);
	}
	if(serial_t_ptr[id]->receivedLetters == 0){
		memset(serial_t_ptr[id]->messageTmp, 0, sizeof(serial_t_ptr[id]->messageTmp));
	}
	for(i=0;i<length;i++) {
		//printf("%d %c %x %s\n", buf[i], buf[i], buf[i], glacier_debug_nishin(buf[i], res));

		serial_t_ptr[id]->messageTmp[serial_t_ptr[id]->receivedLetters] = glacier_serial_1st_bit(buf[i], serial_t_ptr[id]->modeFirstBit);
		//printf("glacier_serial_callback_CR(): serial.received=%d\n", serial.messageTmp[serial.receivedLetters]);fflush(stdout);
//		buf[i] = check_parity(buf[i], 1);
//		message[messagePtr] = buf[i];
		serial_t_ptr[id]->receivedLetters++;
		//printf("glacier_serial_callback_CR(): serial.receivedLetters=%d\n", serial_t_ptr[id]->receivedLetters);fflush(stdout);
		if(serial_t_ptr[id]->messageTmp[serial_t_ptr[id]->receivedLetters-1]=='\r'){
			//printf("glacier_serial_callback_CR(): serial.receivedLetters=%d\n", serial_t_ptr[id]->receivedLetters);fflush(stdout);
			memcpy(serial_t_ptr[id]->message, serial_t_ptr[id]->messageTmp, serial_t_ptr[id]->receivedLetters-1);
			serial_t_ptr[id]->message[serial_t_ptr[id]->receivedLetters-1] = '\0';//remove CR
			printf("message[%d]: %s\n", id, serial_t_ptr[id]->message);
			serial_t_ptr[id]->receivedLetters = 0;
			serial_t_ptr[id]->flgFinish=1;
		}

		printf("(%x)", buf[i]);
		putchar(buf[i]);
    	}
	//buf[length] = '\0';
	//printf("CR receive %d bytes[%d].\n", length, id);
	//printf("receive %d bytes: %s\n", length, buf);
	//printf("flagFinish = %d [%d].\n", serial_t_ptr[id]->flgFinish, id);

	fflush(stdout);

}
/**
 *
 */
void glacier_serial_callback_CRLF(int id, uint8_t *buf,int length)
{
	int i;

	//char res[9];
	
	//printf("glacier_serial_callback_CRLF(): id=%d\n", id);fflush(stdout);
	if(serial_t_ptr[id] == NULL){
		perror("not attached");
	}
	//glacier_serial_t serial = *serial_t_ptr[id];
	//printf("glacier_serial_callback_CRLF(): length=%d\n", length);fflush(stdout);
	if(length > GLACIER_SERIAL_MAX_MESSASE_LETTER_NUM) {
		fprintf(stderr, "glacier_serial_callback_CR[id=%d]: terrible!\n", id);
	}
	//printf("glacier_serial_callback_CRLF(): serial.receivedLetters=%d\n", serial_t_ptr[id]->receivedLetters);fflush(stdout)			strncpy(serial_t_ptr[id]->message, serial_t_ptr[id]->messageTmp, serial_t_ptr[id]->receivedLetters-2);
			serial_t_ptr[id]->message[serial_t_ptr[id]->receivedLetters-2] = '\0';
			printf("message[%d]: %s\n", id, serial_t_ptr[id]->message);
			serial_t_ptr[id]->receivedLetters = 0;
;
	if(serial_t_ptr[id]->receivedLetters == 0){
		memset(serial_t_ptr[id]->messageTmp, 0, sizeof(serial_t_ptr[id]->messageTmp));
	}
	//printf(".");fflush(stdout);
	for(i=0;i<length;i++) {
		//printf("%d %c %x %s\n", buf[i], buf[i], buf[i], glacier_debug_nishin(buf[i], res));
//		printf("%d %c %x %s %d %c\n", buf[i], buf[i], buf[i], nishin(buf[i], res), parity(buf[i]), check_parity(buf[i], 1));

		serial_t_ptr[id]->messageTmp[serial_t_ptr[id]->receivedLetters] = glacier_serial_1st_bit(buf[i], serial_t_ptr[id]->modeFirstBit);
		//printf("glacier_serial_callback_CRLF(): serial.received=%d\n", serial.messageTmp[serial.receivedLetters]);fflush(stdout);
//		buf[i] = check_parity(buf[i], 1);
//		message[messagePtr] = buf[i];
		serial_t_ptr[id]->receivedLetters++;
		//printf("glacier_serial_callback_CRLF(): serial.receivedLetters=%d\n", serial_t_ptr[id]->receivedLetters);fflush(stdout);
		if(serial_t_ptr[id]->messageTmp[serial_t_ptr[id]->receivedLetters-1]=='\n'){
			//printf("glacier_serial_callback_CRLF(): serial.receivedLetters=%d\n", serial_t_ptr[id]->receivedLetters);fflush(stdout);
			//perror("<LF>");
			if(serial_t_ptr[id]->messageTmp[serial_t_ptr[id]->receivedLetters-2]!='\r'){
				perror("?");
			}
			memcpy(serial_t_ptr[id]->message, serial_t_ptr[id]->messageTmp, serial_t_ptr[id]->receivedLetters-2);
//			strncpy(serial_t_ptr[id]->message, serial_t_ptr[id]->messageTmp, serial_t_ptr[id]->receivedLetters-2);
			serial_t_ptr[id]->message[serial_t_ptr[id]->receivedLetters-2] = '\0';
			printf("message[%d]: %s\n", id, serial_t_ptr[id]->message);
			serial_t_ptr[id]->receivedLetters = 0;
			serial_t_ptr[id]->flgFinish=1;
		}

		printf("(%x)", buf[i]);
		putchar(buf[i]);
    	}
	//buf[length] = '\0';
	printf("CRLF receive %d bytes[%d].\n", length, id);
	//printf("receive %d bytes: %s\n", length, buf);

	fflush(stdout);
}
/**
 *
 */
void glacier_serial_callback_LF(int id, uint8_t *buf, int length)
{
}
/**
 *
 */
uint8_t* glacier_serial_get_massage(int id)
{
	return serial_t_ptr[id]->message;
}


int glacier_serial_putchar(int id, char c){
	if(_checkID(id))return -1;
	return cssl_putchar(serial_t_ptr[id]->cssl, c);
}
int glacier_serial_putstring(int id, char *str){
	if(_checkID(id))return -1;
	return cssl_putstring(serial_t_ptr[id]->cssl, str);
}
int glacier_serial_putdata(int id, uint8_t *data, int datalen){
	//printf("glacier_serial_putdata(%d,,%d)\n",id, datalen);fflush(stdout);
	if(_checkID(id))return -1;
	return cssl_putdata(serial_t_ptr[id]->cssl, data, datalen);
}

int _checkID(int id){
	if(id >= GLACIER_SERIAL_MAX_NUM){
		perror("no such ID");
		return -1;
	}
	if(serial_t_ptr[id] == NULL){
		perror("not attached");
		return -1;
	}
	return 0;
}
