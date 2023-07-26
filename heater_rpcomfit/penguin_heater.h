/**
 * Communication with TTM-004W, software ver.1.0
 * Script for 30-cm telescope at Antarctica
 * 2008.12.11 written by NAGAI Makoto
 * 2023.7.25 updated by Shunsuke HONDA for RPCOMFIT protcol for communication
 */

#include "rpcomfit.h"
#include "fitprot.h"

typedef struct penguin_heater_s{
  rpcomfitctrl* pctrl;
} penguin_heater_t;

penguin_heater_t* penguin_heater_init(char* ipaddress, const unsigned int deviceid);
void penguin_heater_fitopen(penguin_heater_t* ppenhtr);
void penguin_heater_fitclose(penguin_heater_t* ppenhtr);
int penguin_heater_initconf(penguin_heater_t* ppenhtr);
int penguin_heater_end(penguin_heater_t* ppenhtr);

/* medium level API */
double penguin_heater_get_temperature(penguin_heater_t* ppenhtr, const unsigned int channel);

/* low level API */
int penguin_heater_sendData(penguin_heater_t* ppenhtr, const unsigned int ch, const char* const cmd);
int penguin_heater_sendrecvData(penguin_heater_t* ppenhtr, const unsigned int ch, const char* const cmd, char* buffer);
int penguin_heater_sendrecvData_int(penguin_heater_t* ppenhtr, const unsigned int ch, const char* const cmd);
double penguin_heater_sendrecvData_float(penguin_heater_t* ppenhtr, const unsigned int ch, const char* const cmd);


