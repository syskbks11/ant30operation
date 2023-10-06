/*!
  ¥file be.cpp
  ¥author Y.Koide
  ¥date 2006.12.20
  ¥brief バックエンドとやり取りする
*/
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#include "configuration.h"
#include "beProtocol.h"
#include "errno.h"
#include "be.h"
#include "controlBoard.h"

//! バックエンドへのリクエスト構造体
typedef struct sReqBeTmp{
  long size;
  int exeFlag;
}tReqBeTmp;

typedef struct sParamBe{
  //! パラメータ
  int BeNum;                 //!< バックエンドの総台数
  int* BeUse;                //!< どのバックエンドを使用するか 0:未使用 1:使用する 2:擬似使用(ネットワーク接続をしない)
  //  const char** BeIp;         //!< 各バックエンドのIP
  //  int* BePort;               //!< 各バックエンドのPort番号
  double BeInterval;         //!< beにアクセスする最小時間 [sec]

  //! 変数
  tmClass_t* vtm;                 //!< 時刻tm用
  netClass_t** net;                //!< netcl用
  int* netAct;               //!< netWorkの状態 1:Online 0:Offline
  beReqInteg_t* req;               //!< 通信データ。リクエスト構造体
  beAnsStatus_t* ans;               //!< 通信データ。アンサー構造体
}tParamBe;

static int init = 0;         //!< beInitが完了しているか 0:まだ 1:完了
static tParamBe p;

static int _beNetInit();
static int _myRead(int beId, unsigned char* buf, long bufSize);
static int _myWrite(int beId, const unsigned char* buf, long bufSize);
static void setParam();
static int checkParam();

/*! ¥fn int beInit()
  ¥brief 初期化
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
  * thread 1, phase C0
  */
int beInit(){
  int ret;

  //! すでに初期化していたら終了させる。
  //if(init){
  //  beEnd();
  //}

  //! パラメータ取得
  memset(&p, 0, sizeof(p));
  setParam();
  if(checkParam()){
    //uM("beInit(); checkParam(); error");
    return BE_PARAM_ERR;
  }

  //! 初期化
  init = 1;
  
  p.net = new netClass_t*[p.BeNum];
  p.netAct = new int[p.BeNum];
  p.req = new beReqInteg_t[p.BeNum];
  p.ans = new beAnsStatus_t[p.BeNum];
  memset(p.net, 0, sizeof(*p.net) * p.BeNum);
  memset(p.netAct, 0, sizeof(*p.netAct) * p.BeNum);
  memset(p.req, 0, sizeof(*p.req) * p.BeNum);
  memset(p.ans, 0, sizeof(*p.ans) * p.BeNum);
  /*
    uM3("beInit(); sizeof(p.net)=%d, sizeof(*p.net)=%d, p.BeNum=%d", sizeof(p.net), sizeof(*p.net), p.BeNum);
    uM2("beInit(); sizeof(p.req)=%d, sizeof(*p.req)=%d", sizeof(p.req), sizeof(*p.req));
    uM2("beInit(); sizeof(p.ans)=%d, sizeof(*p.ans)=%d", sizeof(p.ans), sizeof(*p.ans));
  */
  p.vtm = tmInit(); //!< 念のためtmは初期化しておく
  _beNetInit();
  /* 081125 moved to _beNetInit()
     for(int i = 0; i < p.BeNum; i++){
     if(p.BeUse[i] != 1){
     continue;
     }
     p.net[i] = netclInit(p.BeIp[i], p.BePort[i]);
     if(p.net[i] == NULL){
     uM2("beInit(); netclInit(BeIp%02d, BePort%02d); error!!", i + 1, i + 1);
     beEnd();
     return BE_NET_ERR;
     }
     }
  */
  ret = beReqInit();
  if(ret){
    uM1("beInit(); beReqInit(); return %d error!!", ret);
    return ret;
  }
  return 0;
}

/*! ¥fn int beEnd()
  ¥brief 終了時に呼び出す
  ¥retval 0 成功
  * thread 1, phase C2
  */
int beEnd(){
  if(init){
    if(p.BeUse){
      delete[] p.BeUse;
      p.BeUse = NULL;
    }
    /* 081125 moved to _beNetInit()
       if(p.BeIp){
       delete[] p.BeIp;
       p.BeIp = NULL;
       }
       if(p.BePort){
       delete[] p.BePort;
       p.BePort = NULL;
       }
    */
    if(p.vtm){
      tmEnd(p.vtm);
      p.vtm = NULL;
    }
    if(p.net){
      for(int i = 0; i < p.BeNum; i++){
	if(p.net[i]){
	  netclEnd(p.net[i]);
	  p.net[i] = NULL;
	}
      }
      delete[] p.net;
      p.net = NULL;
    }
    if(p.netAct){
      delete[] p.netAct;
      p.netAct = NULL;
    }
    if(p.req){
      delete[] p.req;
      p.req = NULL;
    }
    if(p.ans){
      delete[] p.ans;
      p.ans = NULL;
    }
  }
  init = 0;
  return 0;
}

/*! ¥fn int beReqInit()
  ¥brief 分光計を初期化する。パラメーターを送信する。
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int beReqInit(){
  const char* allParam;
  beReqInit_t* initDat;
  long size;
  int ret;
  int e = 0;

  if(init == 0){
    return BE_NOT_INIT_ERR;
  }

  allParam = confGetAllKeyVal();
  size = sizeof(initDat->size) + sizeof(initDat->paramSize) + strlen(allParam);
  initDat = (beReqInit_t*)(new unsigned char[size]);
  initDat->size = size;
  initDat->paramSize = strlen(allParam); //! 終端文字'¥0'は含まない
  memcpy(&initDat->param, allParam, initDat->paramSize);
  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] != 1 || p.netAct[i] == 1){
      continue;
    }
    //! 初期化指令を送信
    ret = netclWrite(p.net[i], (const unsigned char*)initDat, initDat->size);
    //printf("saaacqInit(); size=%d paramSize=%d¥n", size, initDat->paramSize);
    if(ret <= 0){
      uM2("beReqInit(); netclWrite(BeIp%02d); return %d error!!", i + 1, ret);    
      if(ret < 0){
	e = BE_NET_ERR;
      }
      else{
	e = BE_NET_DAT_ERR;
      }
      continue;
    }
    else if(ret != initDat->size){
      uM3("beReqInit(); netclWrite(BeIp%02d); ret(%d) != initDat->size(%d) error!!", i + 1, ret, initDat->size);
      e = BE_NET_DAT_ERR;
      continue;
    }

    //! ステータスが返ってくるので読み込まなくてはならない。
    memset(&p.ans[i], 0, sizeof(*p.ans));
    unsigned char* buf = (unsigned char*)&p.ans[i];
    long size = 0;                                         //!< 現在の取得サイズ
    long getSize = sizeof(long);
    int err = 1;

    for(int j = 0; j < 5; j++){
      ret = netclRead(p.net[i], buf + size, getSize - size);
      if(ret <= 0){
	//uM2("beReqInit(); netclRead(BeIp%02d); ret=%d error!!", i + 1, ret);
	err = 2;
	break;
      }
      if(j == 0){
	getSize = p.ans[i].size;
	//printf("p.ans.size=%d¥n",p.ans[i].size);
      }
      size += ret;
      //printf("debug size=%d¥n", size);
      if(size >= getSize){
	err = 0;
	break;
      }
    }
    //uM1("debug beReqInit(); netclRead(); getSize=%d", getSize);
    if(err == 2){
      uM1("beReqInit(); netclRead(BeIp%02d); Network error!!", i + 1);
      e = BE_NET_ERR;
      continue;
    }
    else if(err == 1){
      uM2("beReqInit(); netclRead(BeIp%02d); timeout size=%ld error!!", i + 1, size);
      e = BE_NET_TIMEOUT_ERR;
      continue;
    }
    if(p.ans[i].endExeFlag != 4){
      uM2("beReqInit(); netclRead(BeIp%02d); endExeFlag = %d", i + 1, p.ans[i].endExeFlag);
      e = BE_REJECT;
      continue;
    }
    p.netAct[i] = 1;
  }
  delete[] initDat;
  return e;
}

/*! ¥fn int beReqStart(int OnOffRoad, int returnFlag, unsigned long startTime,
  double* RADEC, double* LB, double* AZEL,
  double* DRADEC, double* DLB, double* DAZEL,
  double* PAZEL, double* RAZEL,
  double PA, double FQIF1, double VRAD, double IFATT,
  double* weath, double* wind, double TAU,
  double BATM, double IPINT)
  ¥brief 分光計に積分の開始指示を送る。
  ¥brief 角度の単位は全て[rad]
  ¥param[in] OnOffRoad 観測点の種類を設定する。 0:OnPoint 1:OffPoint 2:RSky
  ¥param[in] returnFlag 予備
  ¥param[in] startTime 観測開始時刻
  ¥param[in] RADEC マップセンターのRADEC位置配列。[RA][DEC]の順に格納
  ¥param[in] LB マップセンターのLB位置配列。[L][B]の順に格納
  ¥param[in] AZEL マップセンターのAZEL位置配列。[AZ][EL]の順に格納
  ¥param[in] DRADEC マップセンターからのオフセット値。[RA][DEC]の順に格納
  ¥param[in] DLB マップセンターからのオフセット値。[L][B]の順に格納
  ¥param[in] DAZEL マップセンターからのオフセット値。[AZ][EL]の順に格納
  ¥param[in] PAZEL アンテナ指示角。[AZ][EL]の順に格納
  ¥param[in] RAZEL アンテナ実際角。[AZ][EL]の順に格納
  ¥param[in] PA ポジションアングル(保留)
  ¥param[in] FQIF1 1stIFのローカル周波数[GHz]
  ¥param[in] VRAD 観測系からの視速度[km/s]
  ¥param[in] IFATT IFのステップアッテネーターの値[dB]
  ¥param[in] weath 気象データ。[気温][気圧][水上気圧]の順
  ¥param[in] wind 気象データ(風)。[風向][風速][最大瞬間風速]の順
  ¥param[in] TAU 大気の厚さ(保留)
  ¥param[in] BATM 大気の温度[K]
  ¥param[in] IPINT 強度較正装置の温度[K]
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int beReqStart(int OnOffRoad, int returnFlag, unsigned long startTime,
               double* RADEC, double* LB, double* AZEL,
               double* DRADEC, double* DLB, double* DAZEL,
               double* PAZEL, double* RAZEL,
               double PA, double FQIF1, double VRAD, double IFATT,
               double* weath, double* wind, double TAU,
               double BATM, double IPINT){

  int ret;
  //time_t now;
  int e = 0;

  if(init == 0){
    return BE_NOT_INIT_ERR;
  }

  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] != 1){
      continue;
    }
    memset(&p.req[i], 0, sizeof(*p.req));
    p.req[i].size = sizeof(*p.req);
    p.req[i].exeFlag = 1;                   //!< 1:Start
    p.req[i].OnOffRoad = OnOffRoad;
    //p.req[i].returnFlag = returnFlag;       //!< (予備)
    //time(&now);
    p.req[i].nowTime = (unsigned long)startTime;
    p.req[i].DRA = DRADEC[0];
    p.req[i].DDEC = DRADEC[1];
    p.req[i].DGL = DLB[0];
    p.req[i].DGB = DLB[1];
    p.req[i].DAZ = DAZEL[0];
    p.req[i].DEL = DAZEL[1];
    //if(p.iaoflg == 1){
    p.req[i].RA = RADEC[0];
    p.req[i].DEC = RADEC[1];
    //}
    //else if(p.iaoflg == 2){
    p.req[i].GL = LB[0];
    p.req[i].GB = LB[1];
    //}
    //else if(p.iaoflg == 3){
    p.req[i].AZ = AZEL[0];
    p.req[i].EL = AZEL[1];
    //}
    p.req[i].PAZ = PAZEL[0];
    p.req[i].PEL = PAZEL[1];
    p.req[i].RAZ = RAZEL[0];
    p.req[i].REL = RAZEL[1];
    p.req[i].PA = PA;                        //!< ポジションアングル(保留)
    p.req[i].FQIF1 = FQIF1;
    p.req[i].VRAD = VRAD;
    p.req[i].IFATT = (int)IFATT;
    p.req[i].TEMP = weath[0];                //!< 温度 [K]
    p.req[i].PATM = weath[1];                //!< 大気圧 [hPa]
    p.req[i].PH2O = weath[2];                //!< 水蒸気圧 [hPa]
    p.req[i].VWIND = wind[1];                //!< 風速
    p.req[i].DWIND = wind[0];                //!< 風向
    p.req[i].TAU = TAU;                      //!< (保留)
    p.req[i].BATM = BATM;
    p.req[i].IPINT = IPINT;
    
    ret = _myWrite(i, (const unsigned char*)&p.req[i], p.req[i].size);
    //uM1("beReqStart(); netclWrite(); ret=%d", ret);
    if(ret <= 0 || ret != p.req[i].size){
      uM3("beReqStart(); _myWrite(BeIp%02d); [%d(%s)] error!!", i + 1, ret, errnoStr(ret));
      if(ret <= 0){
	e = BE_NET_ERR;
      }
      else{
	e = BE_NET_DAT_ERR;
      }
      continue;
      //return ret;
    }

    //! ステータスが返ってくるので読み込まなくてはならない。
    memset(&p.ans[i], 0, sizeof(*p.ans));
    unsigned char* buf = (unsigned char*)&p.ans[i];
    long size = 0;                      //!< 現在の取得サイズ
    long getSize = sizeof(long);
    int err = 1;
    for(int j = 0; j < 5; j++){
      ret = _myRead(i, buf + size, getSize - size);
      if(ret <= 0){
	uM3("beReqStart(); _myRead(BeIp%02d); [%d(%s)]", i + 1, ret, errnoStr(ret));
	err = 2;
	break;
	//continue;
	//return ret;
      }
      if(j == 0){
	getSize = p.ans[i].size;
	//printf("p.ans.size=%d¥n",p.ans[i].size);
      }
      size += ret;
      //printf("debug size=%d¥n", size);
      if(size >= getSize){
	err = 0;
	break;
      }
    }
    //uM1("debug beReqState(); netclRead(); getSize=%d", getSize);
    if(err == 2){
      continue;
    }
    else if(err == 1){
      uM2("beReqStart(); BeIp%02d timeout size=%ld", i + 1, size);
      e = BE_NET_TIMEOUT_ERR;
      continue;
      //return BE_NET_TIMEOUT_ERR;
    }
    if(p.ans[i].endExeFlag != 4){
      uM2("beReqStart(); BeIp%02d endExeFlag = %d", i + 1, p.ans[i].endExeFlag);
      e = BE_REJECT;
      continue;
      //return BE_REJECT;
    }
  }
  return e;
}

/*! ¥fn int beReqStop()
  ¥brief 分光計の処理を正常終了させる。
  ¥brief 積分途中の場合でも終了させる。
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int beReqStop(){
  int ret;
  tReqBeTmp r = {0};
  int e = 0;

  if(init == 0){
    return BE_NOT_INIT_ERR;
  }

  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] != 1){
      continue;
    }
    r.size = sizeof(r);
    r.exeFlag = 0; //!< request stop
    ret = _myWrite(i, (const unsigned char*)&r, r.size);
    if(ret <= 0 || ret != r.size){
      uM2("beReqStop(); netcl(BeIp%02d); return %d error!!", i + 1, ret);
      if(ret <= 0){
	e = BE_NET_ERR;
      }
      else{
	e = BE_NET_DAT_ERR;
      }
      //return ret;
      continue;
    }

    //! ステータスが返ってくるので読み込まなくてはならない。
    memset(&p.ans[i], 0, sizeof(*p.ans));
    unsigned char* buf = (unsigned char*)&p.ans[i];
    long size = 0;                         //!< 現在の取得サイズ
    long getSize = sizeof(long);
    int err = 1;
    for(int j = 0; j < 5; j++){
      ret = _myRead(i, buf + size, getSize - size);
      if(ret <= 0){
	uM2("beReqStop(); netclRead(BeIp%02d); return %d error!!", i + 1, ret);
	e = BE_NET_DAT_ERR;
	err = 2;
	break;
	//return ret;
      }
      if(j == 0){
	getSize = p.ans[i].size;
	//printf("p.ans.size=%d¥n",p.ans.size);
      }
      size += ret;
      //printf("debug size=%d¥n", size);
      if(size >= getSize){
	err = 0;
	break;
      }
    }
    //uM1("debug beReqStop(); netclRead(); getSize=%d", getSize);
    if(err == 2){
      continue;
    }
    else if(err == 1){
      uM2("beReqStop(); _myRead(BeIp%02d); timeout size=%ld", i + 1, size);
      e = BE_NET_TIMEOUT_ERR;
      continue;
      //return BE_NET_TIMEOUT_ERR;
    }
    if(p.ans[i].endExeFlag != 4){
      uM2("beReqStop(); _myRead(BeIp%02d); endExeFlag = %d", i + 1, p.ans[i].endExeFlag);
      e = BE_REJECT;
      continue;
      //return BE_REJECT;
    }
  }
  return e;
}

/*! ¥fn int beReqState()
  ¥breif 分光計へステータスリクエストを送る。
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int beReqState(){
  int ret;
  tReqBeTmp r;
  int e = 0;

  if(init == 0){
    return BE_NOT_INIT_ERR;
  }

  if(tmGetLag(p.vtm) < p.BeInterval)
    return 0;
  tmReset(p.vtm);

  r.size = sizeof(r);
  r.exeFlag = 2;

  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] != 1){
      continue;
    }
    ret = _myWrite(i, (const unsigned char*)&r, r.size);
    //uM1("debug beReqState(); netclWrite(); ret=%d", ret);
    if(ret <= 0 || ret != r.size){
      uM2("beReqState(); _myWrite(BeIp%02d); return %d error!!", i + 1, ret);
      if(ret <= 0){
	e = BE_NET_ERR;
      }
      else{
	e = BE_NET_DAT_ERR;
      }
      continue;
      //return ret;
    }

    //! ステータスが返ってくるので読み込まなくてはならない。
    memset(&p.ans[i], 0, sizeof(*p.ans));
    unsigned char* buf = (unsigned char*)&p.ans[i];
    long size = 0;                             //!< 現在の取得サイズ
    long getSize = sizeof(long);
    int err = 1;
    for(int j = 0; j < 5; j++){
      ret = _myRead(i, buf + size, getSize - size);
      if(ret <= 0){
	uM2("beReqState(); netclRead(BeIp%02d); ret=%d error!!", i + 1, ret);
	memset(&p.ans[i], 0, sizeof(*p.ans));
	e = BE_NET_ERR;
	err = 2;
	break;
	//return ret;
      }
      if(j == 0){
	getSize = p.ans[i].size;
	//printf("p.ans.size=%d¥n",p.ans.size);
      }
      size += ret;
      //printf("debug size=%d¥n", size);
      if(size >= getSize){
	err = 0;
	break;
      }
    }
    //uM1("debug beReqState(); netclRead(); getSize=%d", getSize);
    if(err == 2){
      continue;
    }
    else if(err == 1){
      uM2("beReqState(); netclRead(BeIp%02d); timeout size=%ld", i + 1, size);
      memset(&p.ans[i], 0, sizeof(*p.ans));
      e = BE_NET_TIMEOUT_ERR;
      continue;
      //return BE_NET_TIMEOUT_ERR;
    }
  }
  return e;
}

/*! ¥fn int beGetAns(int* endExeFlag, int* acqErr, double* tsys, double* totPow)
  ¥brief 分光計のステータス情報を取得する
  ¥param[in] beId バックエンドの番号
  ¥param[out] endExeFlag 実行状態 -1:FAILED 1:SUCCESS 2:STANDBY 3:EXECUTION 4:ACCEPT 5:REJECT
  ¥param[out] acqErr エラー値 1:NORMAL 2:ADCovfl 3:ACCovfl 4:ADACCovfl
  ¥param[out] tsys RSkyで得られたシステム雑音温度[K]
  ¥param[out] totPow On点観測で得られたトータルパワー[W/Hz]
  ¥retval 0 成功
*/
int beGetAns(int beId, int* endExeFlag, int* acqErr, double* tsys, double* totPow){

  printf("begetans0\n");
  if(init == 0){
  printf("begetans init==0 \n");
    return BE_NOT_INIT_ERR;
  }
  if(p.BeUse[beId] != 1){
    printf("begetans p.BeUse[beId] != 1 (%d,%d)\n",beId,p.BeUse[beId]);
    *endExeFlag = 0;
    *acqErr = 0;
    *tsys = 0;
    *totPow = 0;
    return BE_NOT_USE;
  }
  printf("begetans register\n");
  *endExeFlag = p.ans[beId].endExeFlag;
  *acqErr = p.ans[beId].acqErr;
  *tsys = p.ans[beId].tsys;
  *totPow = p.ans[beId].totalPower;
  printf("begetans --> return 0\n");
  return 0;
}

/**
 * thread 1, phase C0
 */
int _beNetInit(){
  if(p.BeNum == 0)
    return 0;

  // zero fill
  int n = p.BeNum;
  const char** beIP = new const char*[n];
  int* bePort = new int[n];
  memset(beIP, 0, sizeof(*beIP) * n);
  memset(bePort, 0, sizeof(*bePort) * n);

  if(controlBoardGetBackEnd() != CONTROL_BOARD_USE_FULL){//090611 in
    for(int i = 0; i < n; i++){
      p.BeUse[i] = 0;
    }
    return 0;
  }
  //load from configuration
  char tmp[256];
  for(int i = 0; i < n; i++){
    sprintf(tmp, "BeUse%02d", i + 1);
    if(confSetKey(tmp)){
      p.BeUse[i] = atoi(confGetVal());
    }
    sprintf(tmp, "BeIp%02d", i + 1);
    if(confSetKey(tmp)){
      beIP[i] = confGetVal();
    }
    sprintf(tmp, "BePort%02d", i + 1);
    if(confSetKey(tmp)){
      bePort[i] = atoi(confGetVal());
    }
  }

  //check parameters
  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] > 0){//081125 in
      //		if(p.BeUse[i] == 1){//081125 out
      sprintf(tmp, "BeIp%02d", i + 1);
      if(!confSetKey(tmp)){
        uM1("BeIp%02d(NULL) error!!", i + 1);
        return -1;
      }
      if(bePort[i] <= 0){
        uM2("BePort%02d(%d) error!!", i + 1, bePort[i]);
        return -1;
      }
    }
  }

  //initialize TCP/IP
  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] != 1){
      continue;
    }
    p.net[i] = netclInit(beIP[i], bePort[i]);
    if(p.net[i] == NULL){
      uM2("beInit(); netclInit(BeIp%02d, BePort%02d); error!!", i + 1, i + 1);
      beEnd();
      return BE_NET_ERR;
    }
  }

  if(beIP){
    delete[] beIP;
    beIP = NULL;
  }
  if(bePort){
    delete[] bePort;
    bePort = NULL;
  }

  return 0;//normal end
}

/*! ¥fn int _myRead(void* net, unsigned char* buf, long bufSize)
  ¥brief ネットワーク経由でのデータの受信を行う
  ¥param[in] beId 接続先の番号
  ¥param[in] buf データの格納領域の先頭ポインター
  ¥param[in] bufSize bufの最大サイズ[Byte]
  ¥retval 1以上 受信バイト数
  ¥retval 0　エラー
  ¥retval -1以下 エラーコード
*/
int _myRead(int beId,  unsigned char* buf, long bufSize){
  int ret;

  if(p.netAct[beId] == 0){
    ret = beReqInit();
    if(ret){
      //!error
      return ret;
    }
    //else{
    //  p.netAct[beId] = 1;
    //}
  }
  ret = netclRead(p.net[beId], buf, bufSize);
  if(ret <= 0){
    p.netAct[beId] = 0;
    return BE_NET_ERR;
  }
  //else if(ret == 0){
  //  return 0;
  //}
  return ret;
}

/*! ¥fn int _myWrite(void* net, const unsigned char* buf, long bufSize)
  ¥brief ネットワーク経由でのデータの送信を行う
  ¥brief 通信が切断していた場合は分光計の初期化を行って、通信を継続する。
  ¥param[in] beId 接続先の番号
  ¥param[in] buf 送信するデータの格納領域の先頭ポインター
  ¥param[in] bufSize 送信データのサイズ
  ¥retval 1以上 送信バイト数
  ¥retval 0　エラー
  ¥retval -1以下 エラーコード
*/
int _myWrite(int beId, const unsigned char* buf, long bufSize){
  int ret;

  if(p.netAct[beId] == 0){
    ret = beReqInit();
    if(ret){
      //! error
      return ret;
    }
    //else{
    //  p.netAct[beId] = 1;
    //}
  }
  ret = netclWrite(p.net[beId], buf, bufSize);
  if(ret <= 0){
    p.netAct[beId] = 0;
    return BE_NET_ERR;
  }
  //else if(ret == 0){
  //  return 0;
  //}
  return ret;
}

/*! ¥fn void setParam()
  ¥brief パラメーター情報を取得する
  * thread 1, phase C0
  */
void setParam(){
  //  char tmp[256];

  if(confSetKey("BeNum"))
    p.BeNum = atoi(confGetVal());
  if(p.BeNum > 0){//081125 in
    p.BeUse = new int[p.BeNum];
    memset(p.BeUse, 0, sizeof(*p.BeUse) * p.BeNum);
  }
  /* 081125 moved to _beNetInit()
     if(p.BeNum > 0){
     int n = p.BeNum;
     p.BeUse = new int[n];
     p.BeIp = new const char*[n];
     p.BePort = new int[n];
     memset(p.BeUse, 0, sizeof(*p.BeUse) * n);
     memset(p.BeIp, 0, sizeof(*p.BeIp) * n);
     memset(p.BePort, 0, sizeof(*p.BePort) * n);
     for(int i = 0; i < n; i++){
     sprintf(tmp, "BeUse%02d", i + 1);
     if(confSetKey(tmp)){
     p.BeUse[i] = atoi(confGetVal());
     }
     sprintf(tmp, "BeIp%02d", i + 1);
     if(confSetKey(tmp)){
     p.BeIp[i] = confGetVal();
     }
     sprintf(tmp, "BePort%02d", i + 1);
     if(confSetKey(tmp)){
     p.BePort[i] = atoi(confGetVal());
     }
     }
     }
  */
  //if(confSetKey("useBe"))
  //  p.use = atoi(confGetVal());
  //if(confSetKey("Hardware"))
  //  p.use = atoi(confGetVal());
  //if(confSetKey("beip"))
  //  strcpy(p.beip, confGetVal());
  //if(confSetKey("beport"))
  //  p.beport=atoi(confGetVal());
  if(confSetKey("BeInterval"))
    p.BeInterval=atof(confGetVal());
  //if(confSetKey("Coordinate"))
  //  p.iaoflg=atoi(confGetVal());
  //if(confSetKey("X_Rad"))
  //  p.dsourc[0] = atof(confGetVal());
  //if(confSetKey("Y_Rad"))
  //p.dsourc[1] = atof(confGetVal());
}

/*! ¥fn int checkParam()
  ¥brief パラメーターのチェック
  ¥retval 0 成功
  ¥retval -1 失敗
  * thread 1, phase C0
  */
int checkParam(){
  //  char tmp[256];

  if(p.BeNum < 0){
    uM1("BeNum(%d) error!!", p.BeNum);
    return -1;
  }
  for(int i = 0; i < p.BeNum; i++){
    if(p.BeUse[i] < 0 || p.BeUse[i] > 2){
      uM2("BeUse%02d(%d) error!!", i + 1, p.BeUse[i]);
      return -1;
    }
  }
  if(p.BeInterval < 0){
    uM1("BeInterval(%lf) error!!", p.BeInterval);
    return -1;
  }
  /* 081125 out
     for(int i = 0; i < p.BeNum; i++){
     if(p.BeUse[i] < 0 || p.BeUse[i] > 2){
     uM2("BeUse%02d(%d) error!!", i + 1, p.BeUse[i]);
     return -1;
     }
     if(p.BeUse[i] == 1){
     sprintf(tmp, "BeIp%02d", i + 1);
     if(!confSetKey(tmp)){
     uM1("BeIp%02d(NULL) error!!", i + 1);
     return -1;
     }
     if(p.BePort[i] <= 0){
     uM2("BePort%02d(%d) error!!", i + 1, p.BePort[i]);
     return -1;
     }
     }
     if(p.BeInterval < 0){
     uM1("BeInterval(%lf) error!!", p.BeInterval);
     return -1;
     }
     }
  */
  return 0;
}
