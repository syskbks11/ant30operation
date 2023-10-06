/*!
¥file configuration.h
¥author NAGAI Makoto
¥date 2008.11.14
¥brief this file removes the differences between the source files for 32-m telescope and those for 30-cm telescope
*/

#ifndef __CONFIIGURATION_H__
#define __CONFIIGURATION_H__

/* the position of libtkb.h differs because of historical reason */
/* 30 cm */
#include "../libtkb/src/libtkb.h"
#include "../libcssl-0.9.4/cssl.h"
#include "../glacier/serial/glacier_serial.h"
#define RS232C_DEVICE_NAME "/dev/ttyUSB%d"

/* 32 m */
//#include "../../import/libtkb.h"

//#define RS232C_DEVICE_NAME "/dev/usb/ttyUSB%d"

#endif
