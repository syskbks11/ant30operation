/*!
¥file srvSignalG.cpp
* written by NAGAI Makoto
* 2008.12.11
* for 30-cm telescope
* 
* original version is written for 32-m telescope by
¥author Y.Koide
¥date 2007.01.08
¥brief 1stIF controle program
*/

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../libtkb/src/libtkb.h"
#include "penguin_weather2.h"

static void _saveLog(double* data);

void _saveLog(double* data){
	//save log file
	FILE* fp;
	time_t t;
	struct tm* tmt;
	char tmp[256];

	time(&t);
	tmt = localtime(&t);
	sprintf(tmp, "../log/penguin-weather2-%04d%02d%02d.log", tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday);
	fp = fopen(tmp, "a");
	if(fp){
		fprintf(fp, "\"%04d/%02d/%02d-%02d:%02d:%02d\" %.1f %.1f\n",
			tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, data[0], data[1]);
		fclose(fp);
	}
}

int main(int argc, char* argv[]){

	double* ans;
	int ret;

	//memset(ans, 0, sizeof(double)*3);
  
	//sprintf("./%s", argv[0]);
	uInit("srvWeather2");
	uM2("This program is Compiled at %s %s", __DATE__, __TIME__);

	ret = penguin_weather_init(argv[1]);
//	ret = penguin_weather_init("/dev/ttyUSB10");

	if(ret){
		uM1("main(); penguin_weather_init(); return %d", ret);
		return -1;
	}
	while(1){
		
		ans = penguin_weather_getData();

		uM2("%.1lf deg C, %.1lf deg C", ans[0], ans[1]);

		_saveLog(ans);
		usleep(1000*1000*5);
	}
	penguin_weather_end();
}
