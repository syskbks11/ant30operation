/*!
  ¥file weath.cpp
  ¥author S.Honda
  ¥date 2024.10.24
  ¥brief Weather Function
  *
  */
#include <stdlib.h>
#include <memory.h>

#include "configuration.h"
#include "errno.h"
#include "weath.h"
#include "controlBoard.h"
#include "../weather/data_penweather.h"

static int init = 0;
static netClass_t* netWeath = NULL;
static tParamWeath p;

static int _weathNet();
static int _weathNetInit();
static void setParam();
static int checkParam();

/*! ¥fn int weathInit()
  ¥brief weathを使用する際、必ず最初に呼び出す。
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
  * thread 1, phase A0
  */
int weathInit(){

  if(init){
    weathEnd();
  }
  init = 0;
  memset(&p, 0, sizeof(p));
  setParam();
  if(checkParam()){
    uM("weathInit(); checkParam(); error");
    return WEATH_PARAM_ERR;
  }
  if(controlBoardGetWeather() == CONTROL_BOARD_USE_NO){//090611 in
    return WEATH_NOT_USE;
  }
  if(controlBoardGetWeather() == CONTROL_BOARD_USE_FULL){//090611 in
    _weathNetInit();
    if(netWeath==NULL){
      uM("weathInit(); netclInit(); error");
      return WEATH_NET_ERR;
    }
  }
  init = 1;
  return 0;
}

/*! ¥fn int weathEnd()
  ¥brief 終了時に呼び出す。
  ¥retval 0 成功
  * thread 1, phase A2
  */
int weathEnd(){
  if(netWeath){
    netclEnd(netWeath);
    netWeath = NULL;
  }
  return 0;
}

/*! ¥fn int weathRepeat()
  ¥brief weathの繰り返し処理部
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int weathRepeat(){
  int ret;

  if(controlBoardGetWeather() == CONTROL_BOARD_USE_NO){//090611 in
    return WEATH_NOT_USE;
  }
  if(controlBoardGetWeather() == CONTROL_BOARD_USE_FULL){//090611 in
    ret = _weathNet();
    if(ret)
      return ret;
  }

  return 0;
}

/*! ¥fn int weathGetState()
  ¥return 制御プログラムの状態
*/
int weathGetState(){
  return p.datWeath.status;
}
/*! ¥fn int weathGetWeath(double* weath)
  ¥brief 気象データ気温・気圧・水上気圧を取得する
  ¥param[out] weath データを格納するための配列。[気温][気圧][水上気圧]の順
  ¥retval 0 成功
*/
int weathGetWeath(double* weath){
  weath[0] = p.datWeath.temp;     //!< 気温 [K]
  weath[1] = p.datWeath.press;    //!< 気圧 [hPa]
  weath[2] = p.datWeath.water;    //!< 水蒸気圧 [hPa]
  return 0;
}
/*! int weathGetWind(double* wind)
  ¥brief 気象データの風向・風速・最大瞬間風速を取得する
  ¥param[out] wind データを格納するための配列。[風向][風速][最大瞬間風速]の順
  ¥retval 0 成功
*/
int weathGetWind(double* wind){
  wind[0] = p.datWeath.direct;     //!< 風向 16方位
  wind[1] = p.datWeath.windVel;    //!< 風速 [m/s]
  wind[2] = p.datWeath.windVelMax; //!< 最大瞬間風速 [m/s]
  return 0;
}

/*! ¥fn int weathSetReq(int reqSet)
  ¥param[in] reqSet 制御プログラムへ渡す制御コード
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int weathSetReq(int reqSet){
  if(reqSet < 0 || reqSet >= 6){
    uM1("weathSetReq(); invalid reqSet=%d",reqSet);
    return WEATH_SET_ERR;
  }
  p.reqWeath.reqSet = reqSet;
  return 0;
}

/*! ¥fn int _weathNet()
  ¥brief 制御プログラムとのネットワーク通信処理
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int _weathNet(){

  tDatWeath aWeath;
  int ret;

  netuCnvInt((unsigned char*)&p.reqWeath.reqSet);
  ret = netclWrite(netWeath, (const unsigned char*)&p.reqWeath, sizeof(p.reqWeath));
  if(ret < 0){
    uM1("_weathNet(); Weather network error = %d", ret);
    return WEATH_NET_ERR;
  }
  else if(ret != sizeof(p.reqWeath)){
    uM1("_weathNet(); Weather send size error = %d", ret);
    return WEATH_NET_DAT_ERR;
  }
  ret = netclRead(netWeath, (unsigned char*)&aWeath, sizeof(aWeath));
  if(ret < 0){
    uM1("_weathNet(); Weather network error = %d", ret);
    return WEATH_NET_ERR;
  }
  if(ret != sizeof(aWeath)){
    uM1("_weathNet(); Weather receive size error = %d", ret);
    return WEATH_NET_DAT_ERR;
  }
  p.datWeath.temp  = aWeath.temp;
  p.datWeath.humid = aWeath.humid;
  p.datWeath.press = aWeath.press;
  p.datWeath.water = aWeath.water;
  p.datWeath.direct =aWeath.direct;
  p.datWeath.windVel = aWeath.windVel;
  p.datWeath.windVelMax = aWeath.windVelMax;
  p.datWeath.status = aWeath.status;
  p.datWeath.time = aWeath.time;

  return 0;
}

/**
 * thread 1, phase A0
 */
int _weathNetInit(){

  char weathIp[256];  //!< 制御プログラムのIP
  int weathPort = 0;  //!< 制御プログラムのPort

  // load and check parameters from configuration
  if(confSetKey("WeathIp"))
    memcpy(&weathIp, confGetVal(), strlen(confGetVal())+1);
  else{
    uM("_weathNetInit(); ERROR: There is no WeathIp in conf.");
    return 1;
  }
  if(confSetKey("WeathPort"))
    weathPort = atoi(confGetVal());
  if(weathPort <= 0){
    uM1("_weathNetInit(); ERROR: Invalid WeathPort (%d).", weathPort);
    return 1;
  }

  // init net
  netWeath = netclInit(weathIp, weathPort);

  return 0;
}

/*! ¥fn void setParam()
  ¥brief パラメーター情報を取得する
  * thread 1, phase A0
  */
void setParam(){
}

/*! ¥fn int checkParam()
  ¥brief パラメーターのチェック
  ¥retval 0 成功
  ¥retval -1 失敗
  * thread 1, phase A0
  */
int checkParam(){
  return 0;
}

