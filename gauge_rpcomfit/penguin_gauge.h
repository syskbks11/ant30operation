/**
 * Communication with vacuum gauge Pfeiffer TPG 261, software ver.1.0
 * Script for 30-cm telescope at Antarctica
 * 2008.12.11 written by NAGAI Makoto
 * 2023.7.20 updated by Shunsuke HONDA for RPCOMFIT protcol for communication
 */

#include "rpcomfit.h"
#include "fitprot.h"

typedef struct penguin_gauge_s{
  rpcomfitctrl* pctrl;
} penguin_gauge_t;

penguin_gauge_t* penguin_gauge_init(char* ipaddress, const unsigned int deviceid);
void penguin_gauge_fitopen(penguin_gauge_t* ppengag);
void penguin_gauge_fitclose(penguin_gauge_t* ppengag);
int penguin_gauge_initconf(penguin_gauge_t* ppengag);
int penguin_gauge_end(penguin_gauge_t* ppengag);

/* medium level API */
double penguin_gauge_get_pressure(penguin_gauge_t* ppengag);

/* low level API */
int penguin_gauge_sendData(penguin_gauge_t* ppengag, const char* const cmd);
int penguin_gauge_sendrecvData(penguin_gauge_t* ppengag, const char* const cmd, char* buffer);

