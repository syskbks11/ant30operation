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
#include <time.h>
#include "../libtkb/src/libtkb.h"
#include "penguin_weather.h"

const int srvPort = 10102;
static netServerClass_t* net = NULL;

static void _saveLog(double* data);

void _saveLog(double* data){
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
    fprintf(fp, "\"%04d/%02d/%02d-%02d:%02d:%02d\" %.1f %.1f %.1f",
            tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec, data[0], data[1], data[2]);
    fclose(fp);
  }
}

int main(int argc, char* argv[]){
  const int myId = 0;
  const int if1Id = 19;
  const int attId = 28;
  double req[3];          //!< LoFreq[GHz], LoAmp[dBm], StepAtt[dB]
  //double ans[3];          //!< LoFreq[GHz], LoAmp[dBm], StepAtt[dB]
  double* ans;
  double reqLoFreq = 0;
  double reqLoAmp = 0;
  double reqAtt = 0;
  double ansAtt = 0;
  const int tmpSize = 256;
  char tmp[256];
  char* c;
  int ret;

  //memset(ans, 0, sizeof(double)*3);
  
  //sprintf("./%s", argv[0]);
  uInit("srvWeather");
  uM2("This program is Compiled at %s %s", __DATE__, __TIME__);

  ret = penguin_weather_init("/dev/ttyUSB7");

  if(ret){
    uM1("main(); penguin_weather_init(); return %d", ret);
    return -1;
  }

  net = netsvInit(srvPort);
  if(net==NULL)
    return -1;
  uEnd();
  while(1){
    if(netsvWaiting(net) < 0){
      continue;
    }
    uInit("srvWeather");
    while(1){

      ans = penguin_weather_getData();

      uM3("main(); temperature=%.1lf deg C, humidity=%5.1lf %, pressure=%5.1lf hPa", ans[0], ans[1], ans[2]);

      _saveLog(ans);

      ret = netsvWrite(net, (const unsigned char*)ans, sizeof(double)*3);
      if(ret <= 0){
        uM1("main(); netsvWrite(); return %d", ret);
        break;
      }
    }
    netsvDisconnect(net);
    uEnd();
  }
  //gpibWdmEnd();
  penguin_weather_end();
  netsvEnd(net);
}