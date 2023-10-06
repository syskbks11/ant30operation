/*!
¥file weatherMonitor.cpp
¥author NAGAI Makoto
¥date 2008.12.16
¥brief 気象データを取得する
* For Tsukuba 32-m telescope. 
*/

#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>

#include "configuration.h"
#include "errno.h"

#include "weatherMonitor.h"

const double ABS_ZERO = 273.15;

static double _weathCalWater(const double T, const double moisture);

int weatherMonitorInit(const char* devName, int id){
	if(penguin_weather_tkb32_init(devName)){
		printf("penguin_weather_init() error.");
		return 1;
	}
	return 0;//normal end
}

int weatherMonitorGetData(tWeathDat *dst){

	double* data = penguin_weather_tkb32_getData();
	if(data == NULL){
		printf("weatherMonitorGetData(); penguin_weather_getData() failed.");
		return 1;
	}


	dst->press = data[2];                        //!< 気圧 [hPa]

	dst->direct = penguin_weather_tkb32_getWindDirection();//!< 360:N 90:E 180:S 270:W とする方位角(右回り系) 0:カーム(静穏時)
	dst->windVel = data[3];	
	dst->windVelMax = data[4];

	dst->temp = data[0] + ABS_ZERO;                         //!< 温度 K
	dst->water = _weathCalWater(dst->temp, data[1]);   //!< 水蒸気圧 hPa


	//save log file
	FILE* fp;
	time_t t;
	struct tm* tmt;
	char tmp[256];

	time(&t);
	tmt = localtime(&t);
	sprintf(tmp, "../log/penguin-weather-%04d%02d%02d.log", tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday);
	fp = fopen(tmp, "a");
	if(fp){
		fprintf(fp, "\"%04d/%02d/%02d-%02d:%02d:%02d\", %s\n",
			tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, penguin_weather_tkb32_getMessage());
//		fprintf(fp, "\"%04d/%02d/%02d-%02d:%02d:%02d\" %.1f %.1f %.1f\n",
//			tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, data[0], data[1], data[2]);
		fclose(fp);
	}

	return 0;//normal end
}

int weatherMonitorEnd(){
	penguin_weather_tkb32_end();
	return 0;//normal end
}



/*! ¥fn double _weathCalWater(double T, double moisture)
¥brief 湿度と温度から水蒸気圧を算出する。
¥brief 計算にはワグナーの式を用いている。
¥brief ここでの換算は273〜313Kまでしかサポートしていないので低温化で使用する場合は注意が必要。
¥param[in] T 気温 [K]
¥param[in] moisture 湿度 [%]
¥return 水蒸気圧 [hPa]
*/
double _weathCalWater(const double T, const double moisture){
  const double A = -7.76451;
  const double B = 1.45838;
  const double C = -2.7758;
  const double D = -1.23303;
  const double Tc = 647.3; //!< K
  const double Pc = 22120; //!< kPa(使用範囲275〜647.3 K、273〜313 Kにおいて0.1hPa以下の誤差)
  double tau = 1.0-T/Tc;
  double Pw;
  double tmp;

  tmp = (A * tau + B * pow(tau, 1.5) + C * pow(tau, 3) + D * pow(tau, 6)) / (T / Tc);
  Pw = pow(M_E, tmp) * Pc; //!< [kPa]
  Pw *= 10;                //!< [hPa]
  Pw *= moisture / 100.0;  //!< Pwater [hPa]
  return Pw;
}


