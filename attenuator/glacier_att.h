#include "usb220_gpib.h"


#ifdef __cplusplus
extern "C"{
#endif

#define ATT_MODE_CONNECT 1
#define ATT_MODE_NO_CONNECT 0

int glacier_att_init(int useMode);
int glacier_att_send(char* buf);
int glacier_att_receive(char* buf, int bufLen);
int glacier_att_close();

#ifdef __cplusplus
}
#endif
