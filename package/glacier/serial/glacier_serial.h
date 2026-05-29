#include "../../libcssl-0.9.4/cssl.h"


#ifdef __cplusplus
extern "C"{
#endif

#define GLACIER_SERIAL_MAX_NUM 8
#define GLACIER_SERIAL_MAX_MESSASE_LETTER_NUM 255 
#define GLACIER_SERIAL_MESSASE_COMPLETE 1
#define GLACIER_SERIAL_MESSASE_IMCOMPLETE 0

#define GLACIER_SERIAL_1ST_NONE 0
#define GLACIER_SERIAL_1ST_ODD 1
#define GLACIER_SERIAL_1ST_EVEN 2
#define GLACIER_SERIAL_1ST_IGNORE 3

uint8_t glacier_serial_1st_bit(uint8_t uint, int mode);

/* utility */
int glacier_serial_getIDforNewPort();

/* basic functions */
int glacier_serial_attach(cssl_t *cssl, int mode);
int glacier_serial_dettach(int id);
void glacier_serial_callback_DEFAULT(int id,uint8_t *buf,int length);
void glacier_serial_callback_CR(int id,uint8_t *buf,int length);
void glacier_serial_callback_CRLF(int id,uint8_t *buf,int length);
void glacier_serial_callback_LF(int id,uint8_t *buf,int length);
uint8_t* glacier_serial_get_massage(int id);

/* wait for message */
int glacier_serial_is_massage_complete(int id);
void glacier_serial_clear_massage_complete(int id);
int glacier_serial_wait_for_massage_complete(int id);
int glacier_serial_wait_for_massage_complete_timeout(int id, double timeout);

/* to receive messages with fixed size */
void glacier_serial_set_bytes_to_read(unsigned int num);
void glacier_serial_callback_FIXED(int id, uint8_t *buf, int length);

/* to send messages. delegation to cssl.h */
/* sends a char via serial port */
int glacier_serial_putchar(int id, char c);
/* sends a null terminated string */
int glacier_serial_putstring(int id, char *str);
/* sends a data of known size */
int glacier_serial_putdata(int id, uint8_t *data, int datalen);


#ifdef __cplusplus
}
#endif
