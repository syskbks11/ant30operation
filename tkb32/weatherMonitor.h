/*!
¥file weatherMonitor.h
¥author NAGAI Makoto
¥date 2008.12.16
¥brief 気象データを取得する
* For Tsukuba 32-m telescope & Antarctic 30-cm telescope
*/
#ifndef __WEATHERMONITOR_H__
#define __WEATHERMONITOR_H__

#ifdef __cplusplus
extern "C"{
#endif


typedef struct sWeathDat{
  int    direct;        //!< 風向 16
  double windVel;       //!< 風速 m/s
  double windVelMax;    //!< 最大瞬間風速 m/s
  double temp;          //!< 気温 K
  double water;         //!< 水蒸気圧 hPa
  double press;         //!< 気圧 hPa
}tWeathDat;



int weatherMonitorInit(const char* devName, int id);
int weatherMonitorGetData(tWeathDat *dst);
int weatherMonitorEnd();

#ifdef __cplusplus
}
#endif
#endif
