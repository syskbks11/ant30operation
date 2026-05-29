#ifndef INCLUDE_GUARD_UUID_9a16e4fe_ca26_460a_b8f8_19e8ee518d04
#define INCLUDE_GUARD_UUID_9a16e4fe_ca26_460a_b8f8_19e8ee518d04
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>

#include "penguin_weather.h"

int main(int argc, char *argv[]) {

  if(penguin_weather_init("/dev/serial/by-id/usb-FTDI_USB_HS_SERIAL_CONVERTER_FTRTK9ES-if00-port0")){
    printf("penguin_weather_init() error.");
    exit(1);
  }

  char cmd[256];
  long bytes_returned;

  double data[3];
  penguin_weather_getData(data);
  if(data == NULL){
    printf("Oh, sh..");
  }

  printf("temperature = %f deg C, humidity = %f %, pressure = %f hPa\n", data[0], data[1], data[2]);
  penguin_weather_end();
}
#endif

