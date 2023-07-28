/**
 * Communication with Powermeter, E4418B, Agilent Co., Ltd.
 * Script for 30-cm telescope at Antarctica
 * 2008.12.11 written by NAGAI Makoto
 * 2023.7.20 updated by Shunsuke HONDA for RPCOMFIT protcol for communication
 */

#include "rpcomfit.h"
#include "fitprot.h"

typedef struct penguin_powermeter_s{
  rpcomfitctrl* pctrl;
} penguin_powermeter_t;

penguin_powermeter_t* penguin_powermeter_init(char* ipaddress, const unsigned int deviceid);
void penguin_powermeter_fitopen(penguin_powermeter_t* ppenpm);
void penguin_powermeter_fitclose(penguin_powermeter_t* ppenpm);
int penguin_powermeter_initconf(penguin_powermeter_t* ppenpm);
int penguin_powermeter_end(penguin_powermeter_t* ppenpm);

/* medium level API */
int penguin_powermeter_get_identification(penguin_powermeter_t* ppenpm, char* buffer);
double penguin_powermeter_get_power(penguin_powermeter_t* ppenpm);

/* low level API */
int penguin_powermeter_sendData(penguin_powermeter_t* ppenpm, const char* const cmd);
int penguin_powermeter_sendrecvData(penguin_powermeter_t* ppenpm, const char* const cmd, char* buffer);

