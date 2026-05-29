#ifndef INCLUDE_GUARD_UUID_4634e9ec_525f_48a6_8740_64f3d3cf2dee
#define INCLUDE_GUARD_UUID_4634e9ec_525f_48a6_8740_64f3d3cf2dee
/*!
  \file cln_penweather.cpp
  \author S.Honda
  \date 2024.09.28
  \brief client program of weather monitoring for ant30 sub-mm telecscope
*/

#include <time.h>
#include <string.h>
#include "data_penweather.h"
#include "../libtkb/src/libtkb.h"

#define SRVIP "127.0.0.1"
#define SRVPORT 10102

#define ABS_ZERO 273.15 // [degC] --> [K]

static int is_init = 0;
static netClass_t* net = NULL;

int init();
int end();
int repeat();
int _netInit();
int _netMain();

/*! ¥fn int init()
  ¥brief 最初に呼び出す。
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int init(){
  if(is_init){
    end();
  }
  is_init = 0;
  // memset(&p, 0, sizeof(p));
  net = NULL;
  _netInit();
  is_init = 1;
  return 0;
}

/*! ¥fn int end()
  ¥brief 終了時に呼び出す。
  ¥retval 0 成功
*/
int end() {
  if(net){
    netclEnd(net);
    net = NULL;
  }
  return 0;
}


/*! ¥fn int repeat()
  ¥brief 繰り返し処理部
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int repeat() {
  int ret = _netMain();
  if(ret) return ret;
  return 0;
}

int _netInit() {
  char ip[256];  //!< 制御プログラムのIP
  int port;      //!< 制御プログラムのPort

  sprintf(ip, "%s", SRVIP);
  port = SRVPORT;
  net = netclInit(ip, port);

  return 0;//normal end
}

/*! ¥fn int _netMain()
  ¥brief 制御プログラムとのネットワーク通信処理
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int _netMain(){
  int req = 0;
  int ret;
  tDatWeath datWeath;

  ret = netclWrite(net, (const unsigned char*)&req, sizeof(req));
  if (ret < 0) {
    uM1("_net(); per network error = %d", ret);
    return -1;
  }
  else if (ret != sizeof(req)) {
    uM1("_net(); per send size error = %d", ret);
    return -2;
  }
  ret = netclRead(net, (unsigned char*)&datWeath, sizeof(tDatWeath));
  if (ret < 0) {
    uM1("_net(); per network error = %d", ret);
    return 1;
  }
  if (ret != sizeof(datWeath)) {
    uM1("_net(); per receive size error = %d", ret);
    return 2;
  }

  printf("%s: temperature = %5.1lf degC, humidity = %5.1lf %, pressure = %5.1lf hPa, water = %5.1lf hPa\n",
         tmGetTimeStr(tmFromUnixTime(datWeath.time),0),
         datWeath.temp-ABS_ZERO, datWeath.humid, datWeath.press, datWeath.water);
  return 0;
}

int main(int argc, char* argv[]) {
  printf("This software is \"weather/test\", a test program for getting environment sensor values from network.\n");
  printf("Type \"help\" for more information. \n");
  init();
  while (1) {
    char cmd[256];
    printf("weather/test> ");
    scanf("%s", cmd);
    if (strcmp(cmd, "help") == 0) {
      printf("help : show this help\n");
      printf("v    : show sensor values\n");
      printf("exit : exit this program\n");
    }
    else if (strcmp(cmd, "exit") == 0) {
      break;
    }
    else if (strcmp(cmd, "v") == 0) {
      repeat();
    }
    else {
      printf("Unknown command: %s\n", cmd);
    }
  }

  return 0;

}

#endif

