/**
 * Communication with Lakeshore218, software ver.1.0
 * Script for 30-cm telescope at Antarctica
 * 2008.12.11 written by NAGAI Makoto
 * 2023.7.20 updated by Shunsuke HONDA for RPCOMFIT protcol for communication
 */

#include "rpcomfit.h"
#include "fitprot.h"

typedef struct penguin_lakeshore_s{
  rpcomfitctrl* pctrl;
} penguin_lakeshore_t;

penguin_lakeshore_t* penguin_lakeshore_init(char* ipaddress, const unsigned int deviceid);
void penguin_lakeshore_fitopen(penguin_lakeshore_t* ppenlks);
void penguin_lakeshore_fitclose(penguin_lakeshore_t* ppenlks);
int penguin_lakeshore_initconf(penguin_lakeshore_t* ppenlks);
int penguin_lakeshore_end(penguin_lakeshore_t* ppenlks);

/* medium level API */
int penguin_lakeshore_get_identification(penguin_lakeshore_t* ppenlks, char* buffer);
double penguin_lakeshore_get_temperature(penguin_lakeshore_t* ppenlks, const unsigned int channel);

/* low level API */
int penguin_lakeshore_sendData(penguin_lakeshore_t* ppenlks, const char* const cmd);
int penguin_lakeshore_sendrecvData(penguin_lakeshore_t* ppenlks, const char* const cmd, char* buffer);
int penguin_lakeshore_sendrecvData_int(penguin_lakeshore_t* ppenlks, const char* const cmd);


