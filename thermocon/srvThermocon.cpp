/*!
¥file srvThermocon.cpp
* written by NAGAI Makoto
* 2009.8.7
* for 30-cm telescope
* 
*/

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <unistd.h>//usleep()
#include <signal.h>//SIGINT

#include "../libtkb/src/libtkb.h"
#include "penguin_thermocon.h"

void _sigFunc(int sig);


static int cont = 1;

int _simpleLogger(){
	int ret;

	uInit("../log/srvThermocon/srvThermocon");//090813 in
/* 090813 out
	uInit("../log/srvThermocon");
*/
	uM2("This program is Compiled at %s %s", __DATE__, __TIME__);

	ret = penguin_thermocon_init("/dev/ttyUSB9");

	if(ret){
		uM1("main(); penguin_thermocon_init(); return %d", ret);
		return -1;
	}
	double temp1;
	int alerm;
	temp1 = penguin_thermocon_getPresetTemperature() / 100.0;
	uM1("Preset temperature = %lf [degC]", temp1);

	while(cont){
		temp1 = penguin_thermocon_getInsideTemperature() / 100.0;
		alerm = penguin_thermocon_getAlerm();
		uM2("%d\t%.2f", alerm, temp1);
		usleep(1000*1000*1);
	}
	penguin_thermocon_end();
	uEnd();
	return 0;
}

int main(int argc, char* argv[]){
	if(signal(SIGINT, _sigFunc) == SIG_ERR){
		uM("signal(); SIGINT error!!");
	}

	return _simpleLogger();
}
void _sigFunc(int sig){
  switch(sig){
  case SIGINT:
	cont = 0;
	return;
  default:
	return;
  }
}

