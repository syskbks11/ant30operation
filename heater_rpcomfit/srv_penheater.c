/*!
  ¥file srv_penheater.cpp
  * written by NAGAI Makoto
  * 2008.12.11
  * for 30-cm telescope
  ¥author S.Honda
  ¥date 2023.07.25
  ¥brief heater logger
*/

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "penguin_heater.h"
#include "../libtkb/src/u/u.h"

int _simpleLogger(){
  //penguin_heater_t

  int ret = 0;
  ret = uInit("../log/heater/srvHeater");//090813 in
  if(ret<0){
    uE("ERROR with opening log file...");
    return -1;
  }
  uM2("This program is Compiled at %s %s", __DATE__, __TIME__);

  penguin_heater_t* pm = NULL;
  pm = penguin_heater_init("192.168.10.3",3);

  double temp1, temp2, temp3;
  while(1){
    penguin_heater_fitclose(pm);
    penguin_heater_fitopen(pm);

    temp1 = penguin_heater_get_temperature(pm,1);
    if(temp1 < 0){
      uM("CH1 failed.");
      temp1 = penguin_heater_get_temperature(pm,1);
    }
    temp2 = penguin_heater_get_temperature(pm,2);
    if(temp2 < 0){
      uM("CH2 failed.");
      temp2 = penguin_heater_get_temperature(pm,2);
    }
    temp3 = penguin_heater_get_temperature(pm,3);
    if(temp3 < 0){
      uM("CH3 failed.");
      temp3 = penguin_heater_get_temperature(pm,3);
    }
    uM3("%.1f\t%.1f\t%.1f", temp1, temp2, temp3);
    usleep(1000*1000*5);
  }

  penguin_heater_fitclose(pm);
  penguin_heater_end(pm);
  uEnd();

  return 0;
}

int main(int argc, char* argv[]){
  return _simpleLogger(strtod());
}
