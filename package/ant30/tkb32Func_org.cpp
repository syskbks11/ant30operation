#ifndef INCLUDE_GUARD_UUID_53dbdf8b_e1c5_45ab_89f1_c22ee9575521
#define INCLUDE_GUARD_UUID_53dbdf8b_e1c5_45ab_89f1_c22ee9575521
/*!
  ¥file tkb32/tkb32Func.cpp
  ¥author Y.Koide
  ¥date 2007.01.01
  ¥brief Tsukuba 32m Central Program Functions
  ¥brief Auto observation : obsInit() -> obsStart() -> obsAuto() -> obsEnd()
  ¥brief Tracking only : obsInit() -> obsStart() -> obsEnd()
  * 
  * Changed by NAGAI Makoto
  * for 32-m telescope & 30-cm telescope
  * 
  * Changed by Enohi Rina
  * 2輝線同時観測＋2偏波化
  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#include "configuration.h"
#include "compileDate.h"
#include "weath.h"
#include "trk.h"
#include "acu.h"
#include "chop.h"
#include "fe.h"
#include "if.h"
#include "calcBoth.h"
#include "../libtkb/src/conf/conf.h" //20240810
/* 090604 out
   #include "calc.h"
   #include "calc2.h"
*/
#include "be.h"
#include "tkb32Protocol.h"
#include "errno.h"
#include "tkb32Func.h"
#include "tkb32ParserTkb32Func.h"
#include "controlBoard.h"

static const double sleepTime = 1000;

static const char logPath[] = "../log";
static const double PI = M_PI;

static int isEndInvoked = 0; //! 0:NotEnd 1:End

static tParamTkb32 p;

static void* thrdUpdate(void* _p);
/*
  static int obsInit();
  static int obsEnd();
  static int obsStart();
  static int obsStop();
  static void* obsAuto(void* _p);
*/
static int R();
static int OnOff(int OnOff);
void OnOffRGet(int* OnOffRoad); //20240826
static int update();
static int trackingWait();
static int rOpenWait();
static int rCloseWait();
//static int integStart(int OnOffR, const time_t startTime);
static int integWait(const double waitTime, const time_t startTime);

static void _execSourceTracking();
static void _execLeeTracking();

static void setParam();
static int checkParam0();
static int checkParam();

static int _loadAntennaFile();
static int _setEnvironmentForTrk45();

int OnOffRoad;  //20240813 グローバル変数
int OnOffR; //20240813
tParamTkb32* getParam(){
  return &p;
}

/*! ¥fn int init();
  ¥brief 初期化
  ¥brief プログラム起動時に呼び出す。
  ¥brief 各制御プログラムへ接続する。
  * thread 1, phase A0
  */
int init(){
  double weath[6] = {0};
  //  char tmp[1024] = {'\0'};
  //int err = 0;
  int ret;

  uM("init(); ant30 Initialize");
  memset(&p, 0, sizeof(p));

  //! パラメータ取得
  setParam();
  if(checkParam0()){
    uM("init(); checkParam(); error");
    return PARAM_ERR;
  }
  if(controlBoardInit()){//090610 in
    uM("init(); controlBoardInit(); error");
    return PARAM_ERR;
  }
  //! trk45Libraryで必要な環境変数を設定
  if(_setEnvironmentForTrk45()){
    return 1;
  }
  /*
    if(putenv(p.EnvAnt) != 0){
    uM1("init(); Cannot add value to environnmet (%s)¥n", tmp);
    return 1;
    }
    if(putenv(p.EnvEph) != 0){
    uM1("init(); Cannot add value to environnmet (%s)¥n", tmp);
    return 1;
    }
    if(putenv(p.EnvTime) != 0){
    uM1("init(); Cannot add value to environnmet (%s)¥n", tmp);
    return 1;
    }
  */
  //! 各モジュール初期化
  p.vtm = tmInit();
  if(!p.vtm){
    uM("init(); tmInit(); error!!");
    return 2;
  }
  ret = weathInit();
  if(ret){
    uM1("init(); weathInit(); ret = %d", ret);
    return 2;
  }
  /* moved to below.
     ret = trkInit();
     if(ret){
     uM1("init(); trkInit(); ret = %d", ret);
     return 2;
     }
  */
  ret = chopInit();
  if(ret){
    uM1("init(); chopInit(); ret = %d", ret);
    return 2;
  }
  ret = feInit();
  if(ret){
    uM1("init(); feInit(); ret = %d", ret);
    return 2;
  }
  ret = ifInit();
  if(ret){
    uM1("init(); ifInit(); ret = %d", ret);
    return 2;
  }
  ret = trkInit();//090831 in
  if(ret){
    uM1("init(); trkInit(); ret = %d", ret);
    return 2;
  }

  //! モニタリングスレッド起動
  int state = pthread_create(&p.thrdUpdateId, NULL, thrdUpdate, NULL);
  if(state != 0){
      const char* hoge = "ERR";
    uM1("init(); pthred_create(thrdUpdate); state=%d error", state);
    return 2;
  }

  uM("init(); Initialize successfully");
  //phase A0 end
  return 0;
}

/*! ¥fn int end()
  ¥brief 終了
  ¥brief プログラム終了時に呼ぶ
  ¥brief 各制御プログラムとの通信を終了する。
  * thread 1, phase A2
  */
int end(){

  isEndInvoked = 1;

  trkEnd();
  if(p.thrdObsRun == 1){
    obsStop();
  }
  if(p.thrdUpdateRun == 1){
    p.thrdUpdateRun = 0;
    pthread_join(p.thrdUpdateId, NULL);
    p.thrdUpdateId = NULL;
  }
  chopEnd();
  feEnd();
  ifEnd();
  weathEnd();
  uEnd();
  if(p.vtm){
    tmEnd(p.vtm);
    p.vtm = NULL;
  }

  return 0;
}

/*! ¥fn void* thrdUpdate(void* _p)
  ¥brief ステータス取得用スレッド
  ¥param[in] _p 未使用
  ¥return NULL
  * thread 4, phase B
  */
void* thrdUpdate(void* _p){

  //phase B: routin work while no observation is running
  p.thrdUpdateRun = 1;
  while(p.thrdUpdateRun){
    update();
    tmSleepMSec(1000);
  }
  return NULL;
}

/*! ¥fn int obsInit()
  ¥brief 観測設定を各モジュールに読み込ませる。
  ¥brief パラメータをconfモジュールに読み込ませてから呼ぶこと。
  ¥brief 観測前に必ず呼び出す。
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
  * thread 1, phase C0
  */
int obsInit(){
  char log[128];
  char tmp[1024];
  int ret;
  
  //! 観測パラメータ初期化処理
  /* 090610 out, since we use scanTable.scanTableGetScanFlag() instead. 
     p.ScanFlag = 0;
  */
  p.OnOffTime = 0;
  p.SetNumber = 0;
  p.RSkyTime = 0;
  /* 090610 out, since we use scanTable.scanTableGetLineTime() instead. 
     p.LineTime = 0;
  */
  p.TimeScan = 0;//080530 //090608 in for 30 cm
  p.TimeApp = 0;//080530 //090608 in for 30 cm
  p.cntOn = 0;
  p.isStow = 0;
  memset(&p.stat, 0, sizeof(p.stat));

  //! パラメータを設定
  setParam();
  if(checkParam()){
    uM("obsInit(); checkParam(); error");
    confPrint();
    return PARAM_ERR;
  }

  //! ログファイル名を生成。*.obsの*部分を使用する。
  if(confSetKey("ObsTable")){
    strcpy(log, confGetVal());
    char* dot = strrchr(log, '.');
    if(dot != NULL){
      *dot = '\0';
    }
  }
  else{
    strcpy(log, "tkb32");
  }

  //! 初期化開始
  sprintf(tmp, "%s/ant30/ant30-%s", logPath, log);//090929 in
  /* 090929 out
     sprintf(tmp, "%s/tkb32-%s", logPath, log);
  */
  uInit(tmp);
  //! LogNameをパラメーターに追加
  sprintf(tmp, "LogName¥t%s", uGetLogName());
  confAddStr(tmp);

  uM1("This program is Compiled at %s", COMPILE_DATE);
  uM("### Initialization");

  //! パラメーターAntX,Y,Zを追加

  if(_loadAntennaFile()){
    return 1;
  }

  /*  081125 moved to _loadAntennaFile()
      FILE* fp;
      if((fp = fopen(p.FileAnt, "r")) == NULL){
      uM1("obsInit(); Ant file open error %s", p.EnvAnt);
      return 1;
      }
      else{
      while(fgets(tmp, 255, fp) != NULL){
      //printf(" %s¥n", tmp);
      if(tmp[0] == ' '){
      double AntX;
      double AntY;
      double AntZ;
      sscanf(tmp, " %lf %lf %lf", &AntX, &AntY, &AntZ);
      sprintf(tmp, "AntX %lf", AntX);
      confAddStr(tmp);
      sprintf(tmp, "AntY %lf", AntY);
      confAddStr(tmp);
      sprintf(tmp, "AntZ %lf", AntZ);
      confAddStr(tmp);
      //uM3("AntPos %lf %lf %lf", AntX, AntY, AntZ);
      break;
      }
      }
      fclose(fp);
      }
      fp = NULL;
  */
  uM("## Parameter list");
  confPrint();

  //! Make schedule file
  char sch1[256];
  sprintf(sch1, "../log/sch1.tmp");
  FILE* fp = fopen(sch1, "w");
  if(!fp){
    uM1("obsInit(); file[%s] open error!!", sch1);
    return 1;
  }
  fprintf(fp, "%s", confGetAllKeyVal());
  fclose(fp);

  char sch2[256];
  sprintf(sch2, "../log/obs.sch");
  sprintf(tmp, "./makeSch %s %s", sch1, sch2);
  ret = system(tmp);
  if(ret){
    uM1("obsInit(); system(makeSch); [%d] error!!", ret);
    return 1;
  }

  //! Read schedule file
  confInit();
  confAddFile(sch2);
  uM("obsInit(); SchFile Data");
  confPrint();
  setParam();
  if(checkParam()){
    uM("obsInit(); checkParam(); error");
    return PARAM_ERR;
  }
    
  //! 各モジュール初期化
  //update();

  //! trk
  ret = trkInitParam();
  if(ret){
    const char* hoge = "ERR";
    uM2("obsInit(); trkInitParam(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
    return 2;
  }

  const int scanFlag = scanTableGetScanFlag();//090610 in
  if(scanFlag >= SCANTABLE_FLAG_ONOFF && scanFlag <= SCANTABLE_FLAG_CROSS){//090611 in
    /* 090611 out
       if(scanFlag >= 1 && scanFlag <= 6){
    */
    //! be
    ret = beInit();
    if(ret){
      const char* hoge = "ERR";
      uM2("obsInit(); beInit(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
      return 2;
    }
  
    //! IF
    ret = ifStart();
    if(ret){
      const char* hoge = "ERR";
      uM2("obsInit(); ifStart(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
      //err += -1;
    }
    else{
      //! IFにLo制御信号を送信
      uM("obsInit(); ifUpdate();");
      const int OnOffR = 2; //!< 初期化時は減衰器の設定をR用に設定しておく
      ifSetAttOnOffR(OnOffR);
      ret = ifUpdate();
      p.stat.if1State = ret;
      p.stat.if2State = ret;
      if(ret){
        const char* hoge = "ERR";
	uM2("obsInit(); ifUpdate(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
	return 2;
      }
    }
  }
  p.isInitialized = 1;
  uM("obsInit(); Success");

  return 0;
}

/*! ¥fn int obsEnd()
  ¥brief 各モジュールの終了処理を行う
  ¥brief 最後に必ず呼び出す
  ¥return 0 成功
  * thread 1, phase C2
  */
int obsEnd(){
  uM("obsEnd(); End observation");

  trkStop();
  beReqStop();
  beEnd();

  uEnd();
  /* 081208 moved to tkb32Parser.cpp
     if(p.ans){
     delete[] p.ans;
     p.ans = NULL;
     }
  */
  //if(p.BeUse){
  //  delete[] p.BeUse;
  //  p.BeUse = NULL;
  //}
  //tReq* tmpReq = p.req;
  //memset(&p, 0, sizeof(p));
  //p.req = tmpReq;
  p.isInitialized = 0;
  return 0;
}

/*! ¥fn int obsStart()
  ¥brief 追尾スタート
  ¥return 0:Success othre:Error
  * thread 1, phase C1
  */
int obsStart(){
  int ret;

  //! 追尾モジュール起動
  ret = trkStart();
  if(ret){
    const char* hoge = "ERR";
    uM2("obsStart(); trkStart(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
    return ret;
  }
  uM("obsStart(); Success");
  
  return 0;
}

/*! ¥fn int obsStop()
  ¥brief 追尾停止
  ¥return 0:success other:fault
  * thread 1, phase C1
  */
int obsStop(){
  if(p.thrdObsRun){
    //beReqStop();
    p.thrdObsRun = 0;
    pthread_join(p.thrdObsId, NULL);
  }
  return 0;
}

/*! ¥fn void* obsAuto(void* _p)
  ¥brief 自動観測を実行。スレッドで呼び出す。
  ¥return NULL　成功
  * thread 6, phase C1
  */
void* obsAuto(void* _p){
  int ret;

  //! ステータス取得スレッドを終了
  if(p.thrdUpdateRun == 1){
    p.thrdUpdateRun = 0;
    pthread_join(p.thrdUpdateId, NULL);
  }

  uM("### Starting observation.");
  p.thrdObsRun = 1;

  //! RSkyをまったく行わない場合(ON点だけを追尾するような場合)はChopperをOpenしておく
  if(p.RSkyTime <= 0 && p.thrdObsRun == 1){
    uM("obsAuto(); Chopper open");
    ret = rOpenWait();
    if(ret){
      const char* hoge = "ERR";
      uM2("obsAuto(); rOpenWait(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
    }
    else{
      uM("obsAuto(); Chopper open success");
    }
  }

  //! スキャン開始
  const int scanFlag = scanTableGetScanFlag();//090610 in
  if(scanFlag >= SCANTABLE_FLAG_ONOFF && scanFlag <= SCANTABLE_FLAG_CROSS){//090611
    //! OnPoint,5Point,Grid,9Point,Random,Raster, and Cross
    /* 090611 out
       if(scanFlag >= 1 && scanFlag <= 6){
       //! OnPoint,5Point,Grid,9Point,Random,Raster
       */
    //! ここでの処理はRを1Setとしているので1SetにRが複数回入るランダム観測時などに適さない
    //! 全てSetPatternでスケジュール観測すべし。ただし、ラスターなどは別扱い。
    int rFlg = 0; //!< rがでたら1:をセットする
    int id = 0;   //!< trkSEtSofNext()の戻り値を格納する。
    for(int i = 0; i < p.SetNumber && p.thrdObsRun; i++){
     // uM1("20231108; tkb32Func p.thrdObsRun= %d", p.thrdObsRun);
      while(p.thrdObsRun){
        //! 次のスキャンを取得する。0:R +:OnPoint -:OffPoint
        ret = trkSetSofNext(&id);
        if(ret){
          //! 1Set完了
          break;
        }

        if(id > 0){
          //! OnPoint
          uM1("obsAuto(); Antenna Moving to On-Point ID=%+d", id);
          tmSleepMSec(1000); //!< trkSetSofNext();直後にtrkGetTrackStatus();を行うと、直前のTrackingを出力してしまうため1.0[sec]待つ
        }
        else if (id < 0){
          //! OffPoint
          uM1("obsAuto(); Antenna Moving to Off-Point ID=%+d", id);
          tmSleepMSec(1000); //!< trkSetSofNext();直後にtrkGetTrackStatus();を行うと、直前のTrackingを出力してしまうため1.0[sec]待つ
        }

        //! 前回にRフラグが立っていれば処理する
        if(rFlg){
          //! R
          rFlg = 0;
          if(p.RSkyTime > 0){
            uM("## R-Sky ID=0");
            while(R() != 0 && p.thrdObsRun == 1){
              //! Rが成功するまで再試行する
              tmSleepMSec(sleepTime);
            }	
          }
        }

        //! Rの場合は次の観測点に向けてから処理する
        if(id == 0){
          //! R
          rFlg = 1;
          continue; //!< 1Set終了
        }
        else{
          //! On or Off
          if(p.OnOffTime > 0 || (scanFlag == SCANTABLE_FLAG_RASTER || scanFlag == SCANTABLE_FLAG_CROSS)){//090611 in
            /* 090611 out
               if(p.OnOffTime > 0 || scanFlag == 6){
            */
            //! OnOffTime >0 もしくはRasterだったら実行
            uM1("## OnOff ID=%+d", id);
            while(OnOff(id) != 0 && p.thrdObsRun == 1){
              //! OnOffが成功するまで再試行する。
              tmSleepMSec(sleepTime);
            }
          }
        }		
      }//while
    }//for
    if(controlBoardGetChopper() == CONTROL_BOARD_USE_FULL && p.thrdObsRun){
      /*
        if(p.useChop != 0 && p.thrdObsRun == 1){
      */
      uM("## R-Sky");
      R();
    }
  }//if
  else if(scanFlag == SCANTABLE_FLAG_SOURCE){
    /* 090611 out
       else if(scanFlag == 1001){
    */
    //! Source Tracking
    _execSourceTracking();
  }
  else if(scanFlag == SCANTABLE_FLAG_LEE){
    /* 090611 out
       else if(scanFlag == 1002){
    */
    //! Lee Tracking
    _execLeeTracking();//090610 in
  }

  //! 観測終了処理
  trkStop();
  p.thrdObsRun = 0;
  //	uM1("obsAuto(); reqRet=%d", p.reqRet);
  p.reqRet = 0;  	
  uM("### End observation. ");
  system("./wave ../etc/oto.wav");

  //! ステータス取得スレッド起動
  int state = pthread_create(&p.thrdUpdateId, NULL, thrdUpdate, NULL);
  if(state != 0){
    uM1("obsAuto(); pthred_create(thrdUpdate); state=%d error!!", state);
    return NULL;
  }

  return NULL;
}

/*
 * invoked by obsAuto()
 * thread 6, phase C1
 */
void _execSourceTracking(){
  uM("## Start Source Tracking Mode");
  tmReset(p.vtm);
  while(p.thrdObsRun == 1){
    //! Endress Loop
    for(int i = 0; i < 60 && p.thrdObsRun; i++){
      update();
      //if(p.stat.trkState){
      //  p.thrdObsRun = 0;
      //  break;
      //}
      tmSleepMSec(sleepTime);
    }
    int min = tmGetLag(p.vtm) / 60;
    uM1("# Source Tracking Running (%d minutes)", min);
  }
}

/*
 * invoked by obsAuto()
 * thread 6, phase C1
 */
void _execLeeTracking(){

  int ret;

  uM("## Start Lee Tracking");
  tmReset(p.vtm);
    
  while(p.thrdObsRun == 1){
    //! Endress Loop
    double wind[3] = {0};  //!< 風向き(0:無風(カーム) 北を360deg(0degと同等) 東90dge 右回り)、風速(m/s)、最大瞬間風速(m/s)の順
    double wd = 0;         //!< 風下の方角
    double xy[2] = {0};    //!< AZELの値 rad
    const int coordAzel = 3;

    xy[1] = 10.0 * PI / 180.0; //!< ELの値を固定

    for(int i = 0; i < 6 && p.thrdObsRun; i++){
      update();
      weathGetWind(wind);
      wd = wind[0];
      if(wd != 0){
        wd -= 180.0; //! 風向きと逆方向を設定
        if(wd < 0){
          wd += 360.0;
        }
        xy[0] = wd * PI/ 180.0;
        ret = trkSetXY(coordAzel, xy);
        if(ret){
          uM("");
        }
      }
      tmSleepMSec(10.0 * 1000.0);
    }
    int min = tmGetLag(p.vtm) / 60;
    uM1("# Lee Tracking Running (%d minutes)", min);
  }

}

/* ¥fn int R()
  ¥brief R-sky観測を行う
  ¥retval 0 成功
*/
int R(){
//  int OnOffR; グローバル変数にする
  int OnOffRoad; //ローカル変数　20240827
  int ret;
  time_t t;
  double acuStatDazel[2]; //20240925
  acuGetRAZEL(acuStatDazel);//20240925

  OnOffR = 2;
  OnOffRGet(&OnOffRoad); //20240827
// uM1("20240827 R(); OnOffRoad = %d",OnOffRoad);
  //! 可変減衰器の減衰値をR用に設定
  ifSetAttOnOffR(OnOffR);
  ifRepeat();
  //! trkの準備完了まで待つ。アンテナの移動中は受信機全体が大きく振動しているため
  ret = trackingWait();
  if(ret){
    const char* hoge = "ERR";
    uM2("R(); trackingWait(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
    return ret;
  }
  //! R-Sky Chopper close
  if(p.thrdObsRun){
    ret = rCloseWait();
    if(ret){
      const char* hoge = "ERR";
      uM2("R(); rCloseWait(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
      return ret;
    }
  }

  //! R-Sky Obs
  if(p.thrdObsRun){
    uM("# R-Sky     Integ start");
    time(&t);
    t += 1;
    ret = integStart(OnOffRoad, t); //20240813
    uM1("20241223 R() integStart() OnOffRoad = %d", OnOffRoad);
    if(ret){
      //uM4("R(); Backend answer exe=%d err=%d tsys=%lf totPow=%lf",
      //  p.stat.saacqExe, p.stat.saacqErr, p.stat.saacqTsys, p.stat.saacqTotPow);
      const char* hoge = "ERR";
      uM2("R(); integStart(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret)); 
      return ret;
    }
 }
    else{
       }   
  //! 積分完了待ち
  if(p.thrdObsRun){
    uM3("202411223 R() Before integWait(): p.thrdObsRun = %d, p.RSkyTime = %ld, t = %ld", p.thrdObsRun, p.RSkyTime, t);
    ret = integWait(p.RSkyTime, t);
    uM1("20241223 R() after integWait() returned : %d\n", ret);
    //if(p.useSaacq == 1){
    //uM4("R(); Saacq answer exe=%d saerr=%d tsys=%lf totPow=%lf",
    //  p.stat.saacqExe, p.stat.saacqErr, p.stat.saacqTsys, p.stat.saacqTotPow);      
    if(ret){
      const char* hoge = "ERR";
      uM2("R(); integWait(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
      return ret;
    }
    //if(p.stat.saacqErr != 1){
    //uM2("R(); saerr=%d(%s)",  p.stat.saacqErr, errnoSaacqErr(p.stat.saacqErr));
    //}
    //}
  }
  uM2("# R-Sky     Integ end  (AZ,EL) = (%lf, %lf)",acuStatDazel[0], acuStatDazel[1]);

  //! R-Sky Chopper Open
  if(p.thrdObsRun){
    ret = rOpenWait();
    if(ret){
      const char* hoge = "ERR";
      uM2("R(); rOpenWait(); [%d(%s)]", ret, hoge);//errnoStrAnt30(ret));
      //return ret;
    }
  }

  return 0;
}

/*! ¥fn int OnOff(int OnOff)
  ¥brief On or Off観測を行う
  ¥param[in] OnOff 1:On-Point -1:Off-Point observation
  ¥retval 0 成功
*/
int OnOff(int OnOff){
  //int exe;
  //int saerr;
  //double tsys;
  //double totPow;
 // int OnOffR;	ローカル変数ではなくグローバル変数にする 20240827
  int ret;
  double waitTime = 0;
  time_t t;
  long tIntegS = 0;//081206 in //090806 from Ogino by Nagai
  const int scanFlag = scanTableGetScanFlag(); //20240810
  double acuStatDazel[2]; //20240925
  //! R Open check
  if(p.thrdObsRun){
    ret = rOpenWait();
    if(ret){
      const char* hoge = "ERR";
      uM2("OnOff(); rOpenWait(); [%d(%s)]", ret, hoge);//errnoStrAnt30(ret));
      return ret;
    }
  }

  //! 可変減衰器の減衰値をOnOff用に設定
  ifSetAttOnOffR(1);

//20240829
  if(OnOff > 0 && scanFlag != 6){
    //! OnPoint(PSW)
    ifRepeat();
    OnOffR = 0;
    OnOffRGet(&OnOffRoad); //20240827
// uM1("20240827 OnOff(); &OnOffRoad = %d",&OnOffRoad);
uM1("OnOff(); OnPoint(PSW) OnOffR = %d",OnOffR);
  }
  //! OnPoint(OTF)
  else if(OnOff > 0 && scanFlag == 6){
    ifRepeat();
    OnOffR =3;
    OnOffRGet(&OnOffRoad); //20240827
//uM1("20240827 OnOff(); &OnOffRoad = %d",&OnOffRoad);
uM1("OnOff(); OnPoint(OTF) OnOffR = %d",OnOffR);
  }
  //! OffPoint 
  else{
    OnOffR = 1;
    OnOffRGet(&OnOffRoad); //20240827
    //! ローカル周波数のdoppler補正
    ret = ifUpdate();
    p.stat.if1State = ret;
    p.stat.if2State = ret;
    if(ret && (controlBoardGet1stIF() || controlBoardGet2ndIF())){//090610 in
      /* 090610 out
         if(ret && (p.useIf1 || p.useIf2)){
      */
      const char* hoge = "ERR";
      uM2("OnOff(); ifUpdate(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
      return IF_STATE_ERR;
    }
    else{
      //! 準備完了まで待つ(保留)
    }
  }

  //! trkの準備完了まで待つ
  ret = trackingWait();
  if(ret){
    const char* hoge = "ERR";
    uM2("OnOff(); trackingWait(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
    return ret;
  }

//  const int scanFlag = scanTableGetScanFlag();//090610 in  //20240810 コメントアウト

  //! OnOff Obs
  if(p.thrdObsRun){
    //! 観測点を表示
    if(OnOff > 0){
      uM("# On-Point  Integ start");
    }
    else{
      uM("# Off-Point Integ start");

      //************************** 20110911 new! ************************
      if(scanTableGetOffMode() == 2) {
        double xy[2] = {0};
        int offCoord = scanTableGetOffCoord();
        switch(offCoord) {
        case 1:
          calcGetRADEC(xy);
          uM2("OnOff(); absolute Off-point (RA,DEC)=(%f,%f)[deg]\n", xy[0]*180/PI, xy[1]*180/PI);
          break;
        case 2:
          calcGetLB(xy);
          uM2("OnOff(); absolute Off-point (L,B)=(%f,%f)[deg]\n", xy[0]*180/PI, xy[1]*180/PI);
          break;
        case 3:
          calcGetAZEL(xy);
          uM2("OnOff(); absolute Off-point (AZ,EL)=(%f,%f)[deg]\n", xy[0]*180/PI, xy[1]*180/PI);
          break;
        }
      }
      //*******************************************************************

    }

    //! 次の1正秒後から観測に入る
    time(&t);
    t += 1;

    if(OnOff > 0){//080530 //090608 in for 30-cm
      if(scanFlag == SCANTABLE_FLAG_RASTER || scanFlag == SCANTABLE_FLAG_CROSS){//090611 in
        /* 090611 out
           if(scanFlag == 6){
        */
//20240201 raster error
        ret = trkSetRastStart(t+1);
        if(ret){
          const char* hoge = "ERR";
          uM2("OnOff(); trkSetRastStart(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
          return ret;
        }
      }
      if(scanFlag == SCANTABLE_FLAG_RASTER || scanFlag == SCANTABLE_FLAG_CROSS){//090611 in
        /* 090611 out
           if(scanFlag == 6){//080530 in IntegStart()までTimeApp分だけ待つ //090608 in for 30-cm
        */
        uM1("OnOff(); AppWaiting %lf seconds. ", p.TimeApp);
        //			sleep(p.TimeApp + 1.0); // 080602 change 下で積分開始を1.0s 遅らせる影響を考慮している
        sleep(p.TimeApp); // 081205 in IntegStart()を早めるために↑から変更
        uM("OnOff(); AppWaiting end. ");
      }
    }
//uM("20231031; timeToWaitIntegStart start");
    //! 積分指令
    double timeToWaitIntegStart = 1.0;//080530 in 積分開始を1.0s 遅らせるための値//090608 in for 30-cm
    //    if(p.ScanFlag == 6){//080530 in
    //  timeToWaitIntegStart += p.TimeApp;
    //}
    //double tIntegS = t + timeToWaitIntegStart;
    tIntegS = t + timeToWaitIntegStart;//081206 change

    if((scanFlag == SCANTABLE_FLAG_RASTER || scanFlag == SCANTABLE_FLAG_CROSS) && OnOff > 0){//090611 in
      /* 090611 out
         if(scanFlag == 6 && OnOff > 0){ // 080602 in RasterでOnScanの場合
      */
      tIntegS += p.TimeApp; // 080604 in App分を考慮した積分開始時刻をintegStart()に渡す
//uM("20231031; scanflg if");    
}
//uM("20231031; scanflg if 2");
    ret = integStart(OnOffRoad, tIntegS);//! RasterのOnScan以外は、そのままの積分開始時刻を渡す //090608 in for 30-cm  //20240813 OnOffRoad
  // uM1("20231106 tkb32Func ret = %d", ret);
/* 090608 out
       ret = integStart(OnOffR, t+1);
    */
//uM("20231031; integstart check");
    if(ret){
      const char* hoge = "ERR";
      uM2("OnOff(); integStart(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret)); 
      return ret;
    }

    if(OnOff > 0){
//uM("20231031; OnOff > 0");
      //! On
      if(scanFlag != SCANTABLE_FLAG_RASTER && scanFlag != SCANTABLE_FLAG_CROSS){//090611 in
        /* 090611 out
           if(scanFlag != 6){
        */
	//! Not Raster
	waitTime = p.OnOffTime;
      }
      else{
//uM("20231031; OnOff raster");
	//! Raster
	waitTime = scanTableGetLineTime();
        /* 090610 out 
           waitTime = p.LineTime;
        */
        /* 080530 out? 090611 out for 30 cm
           ret = trkSetRastStart(t+1);
           if(ret){
           const char* hoge = "ERR";
           uM2("OnOff(); trkSetRastStart(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
           return ret;
           }
        */
      }
    }
    else{
//uM("20231031; OnOff off");
      //! Off
      waitTime = p.OnOffTime;
    }
  }
//uM("20231031; integwait start 2");
  //! 積分完了待ち
  if(p.thrdObsRun){
    uM3("20241223 OnOff() Before integWait: p.thrdObsRun = %d, p.RSkyTime = %ld, t = %ld", p.thrdObsRun, p.RSkyTime, t);
    ret = integWait(waitTime, tIntegS);//090608 in for 30-cm
    /* 090608 out
       ret = integWait(waitTime, t);
    */
    //if(p.useSaacq == 1){
    //uM4("OnOff(); Saacq answer exe=%d saerr=%d tsys=%lf totPow=%lf",
    //  p.stat.saacqExe, p.stat.saacqErr, p.stat.saacqTsys, p.stat.saacqTotPow);
    if(ret){
      const char* hoge = "ERR";
      uM2("OnOff(); integWait(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
      return ret;
    }
    //if(p.stat.saacqErr != 1){
    //uM2("OnOff(); saerr [%d(%s)]",  p.stat.saacqErr, errnoSaacqErr(p.stat.saacqErr));
    //}
    //}
  }  

  //! 成功した場合は現在の観測点とOn点の場合はそのカウント数を表示
  if(p.thrdObsRun){
  // OnOffRを取得する 20240811
 // ret = OnOffRGet(&OnOffR);
acuGetRAZEL(acuStatDazel);//20240925
    if(OnOff > 0){
      p.cntOn++;
      uM3("# On-Point  Integ end (On-Count:%d) (AZ,EL) = (%lf, %lf)", p.cntOn, acuStatDazel[0], acuStatDazel[1]);
    }
    else{
      uM2("# Off-Point Integ end (AZ,EL) = (%lf, %lf)", acuStatDazel[0], acuStatDazel[1]);
    }
  }
  //uM("debug end onoff check time");
  return 0;
}

//20240811
/*! ¥fn int OnOffRGet(int* OnOffRoad)
  ¥brief OnOffRを取得する
  ¥param[out] OnOffRoadの値を受け取るポインタ
  ¥retval 0 成功
*/
//20240826
void OnOffRGet(int* OnOffRoad){
 memcpy(OnOffRoad, &OnOffR, sizeof(int));
// uM1("20240827 OnOffRGet(); OnOffRoad = %d", *OnOffRoad);
 return;
}

//int OnOffRGet(){
//OnOffRoad = OnOffR;
//return OnOffRoad;
//}


/*! ¥fn int update()
  ¥brief 各モジュールの状態をチェックする
  ¥brief モジュールによってはデバイスの情報を更新する
  ¥brief 以前のステータスと異なる場合のみ表示する。
  ¥retval 0 成功 other:fault
  * thread 4, phase B
  * 
  */
int update(){
  int ret;
  double weath[3];
  int err = 0;

  if(isEndInvoked == 1){
    return 0;
  }

  //! weath
  ret = weathRepeat();
  if(ret != 0 && ret != p.stat.weathState){
    const char* hoge = "ERR";
    uM2("update(); weathRepeat(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
    err = 1;
  }
  else if(ret != p.stat.weathState){
    const char* hoge = "ERR";
    uM2("update(); weathRepeat(); [%d(%s)] recovery", ret, hoge);//errnoStrAnt30(ret));
  }
  p.stat.weathState = ret;
  if(ret == 0){
    weathGetWeath(weath);
    calcSetWeath(weath);
    // uM3("weathGetWeath(); %lf, %lf, %lf", weath[0], weath[1], weath[2]);//errnoStrAnt30(ret));
  }

  //! trk
  ret = trkRepeat();
  if(ret != 0 && ret != p.stat.trkState){
    const char* hoge = "ERR";
    uM2("update(); trkRepeat(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
    err = 1;
  }
  else if(ret != p.stat.trkState){
    const char* hoge = "ERR";
    uM2("update(); trkRepeat(); [%d(%s)] recovery", ret, hoge);//errnoStrAnt30(ret));
  }
  p.stat.trkState = ret;
  if(ret == 0){
    const unsigned char STOW = 0x32;
    acuGetACUStatus(p.stat.trkACUStatus);
    if((p.stat.trkACUStatus[2] & STOW) == STOW){
      p.isStow = 1;
    }
    else{
      p.isStow = 0;
    }
  }

  //! chop
  chopSetReq(1); //!< 1:status
  ret = chopRepeat();
  if(ret != 0 && ret != p.stat.chopState){
    const char* hoge = "ERR";
    uM2("update(); chopRepeat(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
    err = 1;
  }
  else if(ret != p.stat.chopState){
    const char* hoge = "ERR";
    uM2("update(); chopRepeat(); [%d(%s)] recovery", ret, hoge);//errnoStrAnt30(ret));
  }  
  p.stat.chopState = ret;

  //! fe
  ret = feRepeat();
  if(ret != 0 && ret != p.stat.feState){
    const char* hoge = "ERR";
    uM2("update(); feRepeat(); [%d(%s)] error!!", ret, hoge);//errnoStrAnt30(ret));
    err = 1;
  }
  else if(ret != p.stat.feState){
    const char* hoge = "ERR";
    uM2("update(); feRepeat(); [%d(%s)] recovery", ret, hoge);//errnoStrAnt30(ret));
  }
  p.stat.feState = ret;

  return err;
}

/*! ¥fn int trackingWait()
  ¥brief Tracking状態になるまで待つ
  ¥return 0:Success othre:Error
*/
int trackingWait(){
  //! trkの準備完了まで待つ。アンテナの移動中は受信機全体が大きく振動しているため
  if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090610 in
    /* 090610 out
       if(p.useTrk == 1){
    */
    int stat=1;
    //!1分間(アンテナ1/4Az周分)まで待つ 
    for(int i=0; i<30 && p.thrdObsRun; i++){
      update();
      if(p.stat.trkState == 0){
	stat = trkGetTrackStatus();
	//trkGetTrackStatus(&stat);
	if(stat==0){//230523
         // uM("20231031; if stat = 0 ");
          break;
        }
      }
      tmSleepMSec(sleepTime);
    }
//uM2("20231101; tkb32Func trackingWait() p.stat.trkState = %d, stat = %d", p.stat.trkState, stat);
    //! Error
    if(p.stat.trkState){
     // uM("20231102; tkb32Func.cpp return p.stat.trkState");
      return p.stat.trkState;
    }
    else if(stat){
     // uM("20231102; tkb32Func.cpp return TRK_TRACKING_TIMEOUT_ERR");
      return TRK_TRACKING_TIMEOUT_ERR;
    }
  }
  else{
   // uM("20231102; trackingwait end");
    update();
  }
  return 0;
}
/*! ¥fn int rOpenWait()
  ¥brief RにOpen指令を出し、Openするまで待機する。
  ¥retunr 0:Success othre:Error
*/
int rOpenWait(){
  int err;
  int ret;

  //! 現在のRの状態を取得しOpenなら終了する。
  if(controlBoardGetChopper() == CONTROL_BOARD_USE_FULL && p.thrdObsRun){
    /*
      if(p.useChop == 1 && p.thrdObsRun){
    */
    update();
    chopSetReq(1);  //!< 1:status
    ret = chopRepeat();
    p.stat.chopState = ret;
    if(ret == 0 && chopGetState() == 2 && chopGetMoter() == 4){ //!< 2:open, 4:stop状態
      return 0;
    }
  }
  else{
    return 0;
  }

  //! Open指令を出す
  uM("rOpenWait(); Chopper open");
  if(controlBoardGetChopper() == CONTROL_BOARD_USE_FULL && p.thrdObsRun){
    /*
      if(p.useChop == 1 && p.thrdObsRun){
    */
    chopSetReq(3);     //!< 3:open
    ret = chopRepeat();
    p.stat.chopState = ret;
    if(ret){
      return CHOP_STATE_ERR;
    }
  }

  //! 準備完了まで待つ(タイムリミット10秒)
  if(controlBoardGetChopper() == CONTROL_BOARD_USE_FULL && p.thrdObsRun){
    /*
      if(p.useChop == 1 && p.thrdObsRun){
    */
    err = 1;
    for(int i = 0; i < 10 && p.thrdObsRun; i++){
      update();
      chopSetReq(1);  //!< 1:status
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	return CHOP_STATE_ERR;
      }
      else{
	if(chopGetState() == 2 && chopGetMoter() == 4){ //!< 2:open, 4:stop状態
	  err=0;
	  break;
	}
      }
      tmSleepMSec(sleepTime);
    }
    if(err){
      return CHOP_MOVING_TIMEOUT_ERR;
    }
  }
  uM("rOpenWait(); Chopper open success");
  return 0;
}

/*! ¥fn int rCloseWait()
  ¥brief RにClose指令を出し、Closeするまで待機する。
  ¥retunr 0:Success othre:Error
*/
int rCloseWait(){
  int err;
  int ret;

  //! 現在のRの状態を取得しCloseなら終了する。
  if(controlBoardGetChopper() == CONTROL_BOARD_USE_FULL && p.thrdObsRun){
    /*
      if(p.useChop == 1 && p.thrdObsRun){
    */
    update();
    chopSetReq(1);  //!< 1:status
    ret = chopRepeat();
    p.stat.chopState = ret;
    if(ret == 0 && chopGetState() == 1 && chopGetMoter() == 4){ //!< 1:close, 4:stop状態
      return 0;
    }
  }

  //! Close指令を出す
  uM("rCloseWait(); Chopper close");
  if(controlBoardGetChopper() == CONTROL_BOARD_USE_FULL && p.thrdObsRun){
    /*
      if(p.useChop == 1 && p.thrdObsRun){
    */
    chopSetReq(2);        //!< 2:close
    ret = chopRepeat();
    p.stat.chopState = ret;
    if(ret){
      return CHOP_STATE_ERR;
    }
  }

  //! 準備完了まで待つ(タイムリミット10秒)
  if(controlBoardGetChopper() == CONTROL_BOARD_USE_FULL && p.thrdObsRun){
    /*
      if(p.useChop == 1 && p.thrdObsRun){
    */
    err=1;
    for(int i=0; i<10 && p.thrdObsRun; i++){
      chopSetReq(1);    //!< 1:status
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	return CHOP_STATE_ERR;
      }
      else{
	if(chopGetState() == 1 && chopGetMoter() == 4){ //!< 1:close, 4:stop状態
	  err = 0;
	  break;
	}
      }
      tmSleepMSec(sleepTime);
    }
    if(err){
      return CHOP_MOVING_TIMEOUT_ERR;
    }
  }
  uM("rCloseWait(); Chopper close success");
  return 0;
}

/*! ¥fn int integStart(int OnOffRoad) //20240813
  ¥brief 分光計積分開始
  ¥param[in] OnOffRoad 0:On-Point(PSW) 1:Off-Point 2:R-Sky 0:On-Point(OTF)
  ¥param[in] startTime 積分を開始する時刻time(&t);で取得する時刻表記
  ¥retval 0 成功
  ¥retval 0以外 エラーコード
*/
int integStart(int OnOffRoad, const time_t startTime){
  double RADEC[2];
  double LB[2];
  double AZEL[2];
  double DRADEC[2];
  double DLB[2];
  double DAZEL[2];
  double PAZEL[2];
  double RAZEL[2];
  double PA = 0;
 // int  OnOffRoad; //20240827
  double FQIF1;
  double VRAD;
  double IFATT;
  double IFATT2; //20241126
  double IFATT3; //20241126
  double IFATT4; //20241126
  double weath[3];
  double wind[3];
  double TAU;        //!< 大気の厚さ
  double BATM;       //!< 大気の温度 [K] 20240918
  double IPINT;
  int exe;
  int saerr;
  double tsys;
  double totPow;
  int ret;
  time_t t;
  struct tm* stm;
  char timeStr[16]; //20240905
  //! 使用していない場合は終了
  //if(p.useSaacq != 1){
  //  return SAACQ_NOT_USE;
  //}

  //! 現在のステータスを確認する
  uM("20241223 integStart() -> beReqState()");
  ret = beReqState();
  uM1("2024123 beReqState() returned: %d\n", ret);
  p.stat.saacqState = ret;
  if(ret){
    return ret;
  }
  printf("BeNUM = %d \n",p.BeNum);
  for(int i = 0; i < p.BeNum; i++){
   // uM("20231106 tkb32Func.cpp before beGetAns");
   // uM4("20231106; tkb32Func tsys = %d, totPow = %d, exe = %d, saerr = %d", p.stat.saacqTsys, p.stat.saacqTotPow, p.stat.saacqExe, p.stat.saacqErr);
    beGetAns(i, &exe, &saerr, &tsys, &totPow);
    //uM("20231106 tkb32Func after beGetAns");
//uM4("20231101; tkb32Func tsys = %d, totPow = %d, exe = %d, saerr = %d",p.stat.saacqTsys, p.stat.saacqTotPow, p.stat.saacqExe, p.stat.saacqErr);
    p.stat.saacqTsys = tsys;
    p.stat.saacqTotPow = totPow;
    p.stat.saacqExe = exe;
    p.stat.saacqErr = saerr;
//uM4("20231101; tkb32Func p.stat.saacqTsys = %d, p.stat.saacqTotPow = %d, p.stat.saacqExe = %d, p.stat.saacqErr = %d",  tsys, totPow, exe, saerr); 

    if(exe != 0 && exe != 1 && exe != 2){
      //!< -1:Failed 1:Success 2:Standby 3:Execution 4:Accept 5:Reject
      uM2("integStart(); Be%02d exeFlag(%d) error!! ", i + 1, exe);
      p.stat.saacqState = SAACQ_STATE_ERR;
      return p.stat.saacqState;
    }
   else{
    // uM("20231031; integstart fail");
   }
  }

  //! 積分開始指令
  t = startTime;
  stm = localtime(&t);

  sprintf(timeStr, "%02d:%02d:%02d", stm->tm_hour, stm->tm_min, stm->tm_sec); 
  uM2("integStart(); startTime=%s, OnOffRoad = %d", timeStr, OnOffRoad);

  calcGetRADEC(RADEC);
  calcGetLB(LB);
  calcGetAZEL(AZEL);
  calcGetDRADEC(DRADEC);
  calcGetDLB(DLB);
  calcGetDAZEL(DAZEL);
  calcGetPAZEL(PAZEL);
  acuGetRAZEL(RAZEL);
  FQIF1 = ifGetLo1PFreq();
  VRAD = calc2GetVRAD(); 
  IFATT = ifGetLo1PAtt();
  IFATT2 = ifGetLo1PAtt2(); //20241126
  IFATT3 = ifGetLo1PAtt3(); //20241126
  IFATT4 = ifGetLo1PAtt4(); //20241126
  OnOffRGet(&OnOffRoad); //20240826
//uM1("20240827 IntegStart(); OnoffRoad = %d", OnOffRoad);
//20241126 beReqStart()に増やした分追加
double FQTRK;
if(confSetKey("Ftrack_1"))
    FQTRK = atof(confGetVal());
double FQTRK2;
if(confSetKey("Ftrack_2")) 
    FQTRK2 = atof(confGetVal()); 
double FQTRK3;
if(confSetKey("Ftrack_3"))
    FQTRK3 = atof(confGetVal());
double FQTRK4;
if(confSetKey("Ftrack_4"))
    FQTRK4 = atof(confGetVal());
double LO2_1;
if(confSetKey("lo2_1"))
    LO2_1 = atof(confGetVal());
double LO2_2;
if(confSetKey("lo2_2"))
    LO2_2 = atof(confGetVal()); 
double LO2_3;
if(confSetKey("lo2_3"))
    LO2_3 = atof(confGetVal());
double LO2_4;
if(confSetKey("lo2_4"))
    LO2_4 = atof(confGetVal());
//double TAU;
if(confSetKey("TAU"))
    TAU = atof(confGetVal());

//20240918
 IPINT = 270.0;
// BATM = 250.0; 気象装置から読みとるようにした. 20241126
  /*
    ifGetLo1PFreq(&FQIF1);
    ifGetVRAD(&VRAD);
    ifGetLo1PAtt(&IFATT);
  */
  weathGetWeath(weath);
  weathGetWind(wind);
  //20240917 out この仮定は成立しない
 // IPINT = weath[0];//090930 in; We assume that the load temperature is equal to the air temperature for 30-cm telescope
  /* 090930 out
     IPINT = chopGetTemp2();
  */
// 20240810 beProtocol.hに足した分だけ追加
// 20240813 OnOffRをOnOffRoadに変更
  ret = beReqStart(OnOffRoad, 0, startTime,
		   RADEC, LB, AZEL,
		   DRADEC, DLB, DAZEL,
		   PAZEL, RAZEL,
		   PA, FQTRK, FQTRK2, FQTRK3, FQTRK4, FQIF1, VRAD, IFATT, IFATT2, IFATT3, IFATT4,
                   LO2_1, LO2_2, LO2_3, LO2_4,
		   weath, wind, TAU,
		   BATM, IPINT);

  p.stat.saacqState = ret;
  return ret;
}

/*! ¥fn int integWait()
  ¥brief 分光計のデータを取得するまで待機する。
  ¥return 0:Success other:Error
*/
int integWait(const double waitTime, time_t startTime){
  //int exe;
  //int saerr;
  //double tsys;
  //double totPow;
  int ret;

  uM("20241223 integWait()");
  //! 積分開始まで待つ
  while(tmGetDiff(startTime, 0) > 0.001){
    tmSleepMSec(1);
  }

  //! 積分時間待つ
  tmReset(p.vtm);
  while(tmGetLag(p.vtm) < waitTime && p.thrdObsRun){
    update();
    tmSleepMSec(sleepTime);
  }

  //! 積分時間を過ぎたら分光計の結果をチェックしに行く
  if(p.thrdObsRun){
    uM("20241223 integWait() -> beReqState()");
    ret = beReqState();
    uM1("20241223 beReqState() retrned : %d\n", ret);
    p.stat.saacqState = ret;
    if(ret){
      return ret;
    }
    //beGetAns(&exe, &saerr, &tsys, &totPow);
    //p.stat.saacqTsys = tsys;
    //p.stat.saacqTotPow = totPow;
    //p.stat.saacqExe = exe;
    //p.stat.saacqErr = saerr;
    //if(exe != 3){
    //!< -1:Failed 1:Success 2:Standby 3:Execution 4:Accept 5:Reject
    //if(exe != 1){
    //p.stat.saacqState = BE_STATE_ERR;
    //return p.stat.saacqState;
    //}
    // break;
    //}
    update();
    //tmSleepMSec(sleepTime);
  }
  return 0;
}

/*! ¥fn void setParam()
  ¥brief パラメーター情報を取得する
*/
void setParam(){
  //char tmp[256] = {'\0'};
  /* 090610 moved to controlBoard.cpp
     if(confSetKey("WeathUse"))
     p.useWeath = atoi(confGetVal());
     if(confSetKey("TrkUse"))
     p.useTrk = atoi(confGetVal());
     if(confSetKey("ChopUse"))
     p.useChop = atoi(confGetVal());
     if(confSetKey("FeUse"))
     p.useFe = atoi(confGetVal());
     if(confSetKey("IfUse01"))
     p.useIf1 = atoi(confGetVal());
     if(confSetKey("IfUse02"))
     p.useIf2 = atoi(confGetVal());
  */
  if(confSetKey("BeNum"))
    p.BeNum = atoi(confGetVal());
  
  /* 090610 moved to _loadAntennaFile()
     if(confSetKey("EnvAnt"))
     strcpy(p.FileAnt, confGetVal());
  */
  /*
    if(confSetKey("EnvAnt")){
    strcpy(p.FileAnt, confGetVal());
    sprintf(p.EnvAnt, "ANTFILE=%s", confGetVal());
    }
    if(confSetKey("EnvEph"))
    sprintf(p.EnvEph, "EPHFILE=%s", confGetVal());
    if(confSetKey("EnvTime"))
    sprintf(p.EnvTime, "TIMEFILE=%s", confGetVal());
  */

  /* 090610 out, since we use scanTable.scanTableGetScanFlag() instead. 
     if(confSetKey("ScanFlag"))
     p.ScanFlag = atoi(confGetVal());
  */
  if(confSetKey("OnOffTime"))
    p.OnOffTime = atof(confGetVal());
  if(confSetKey("SeqNumber"))          //!< 互換性のため当面設定しておく
    p.SetNumber = atoi(confGetVal());
  if(confSetKey("SetNumber"))
    p.SetNumber = atoi(confGetVal());
  if(confSetKey("RSkyTime"))
    p.RSkyTime = atoi(confGetVal());
  /* 090610 out, since we use scanTable.scanTableGetLineTime() instead. 
     if(confSetKey("LineTime"))
     p.LineTime = atof(confGetVal());
  */
  if(confSetKey("TimeScan"))//080530 in //090608 in for 30 cm
    p.TimeScan = atof(confGetVal());
  if(confSetKey("TimeApp"))//080530 in //090608 in for 30 cm
    p.TimeApp = atof(confGetVal());
}

/*! ¥fn int checkParam0()
  ¥brief init()でのパラメーターのチェック
  ¥retval 0 成功
  ¥retval -1 失敗
  * thread 1, phase A0
  */
int checkParam0(){

  /* 090610 moved to controlBoard.cpp
     if(p.useWeath < 0 || p.useWeath > 2){
     uM1("checkParam(); useWeath[%d] error!!", p.useWeath);
     return -1;
     }
     if(p.useTrk < 0 || p.useTrk > 2){
     uM1("checkParam(); useTrk[%d] error!!", p.useTrk);
     return -1;
     }
     if(p.useChop < 0 || p.useChop > 2){
     uM1("checkParam(); useChop[%d] error!!", p.useChop);
     return -1;
     }
     if(p.useFe < 0 || p.useFe > 2){
     uM1("checkParam(); useFe[%d] error!!", p.useFe);
     return -1;
     }
     if(p.useIf1 < 0 || p.useIf1 > 2){
     uM1("checkParam(); useIf1[%d] error!!", p.useIf1);
     return -1;
     }
     if(p.useIf2 < 0 || p.useIf2 > 2){
     uM1("checkParam(); useIf2[%d] error!!", p.useIf2);
     return -1;
     }
  */
  if(!confSetKey("EnvAnt")){
    uM("checkParam0(); EnvAnt error!!");
    confPrint();
    printf("==================\n");
    return -1;
  }
  /* 081125 moved
     if(!confSetKey("EnvEph")){
     uM("checkParam(); EnvEph error!!");
     return -1;
     }
     if(!confSetKey("EnvTime")){
     uM("checkParam(); EnvTime error!!");
     return -1;
     }
  */
  return 0;
}

/*! ¥fn int checkParam()
  ¥brief obsInit()でのパラメーターのチェック
  ¥retval 0 成功
  ¥retval -1 失敗
  * thread 1, phase C0
  */
int checkParam(){
  /* 090610 removed. I think it's not good to read these parameters again in phase C0.
   * If we want to over-write the value read from tkb32.conf/ant30.conf, then we should restart the program. 

   if(p.useWeath < 0 || p.useWeath > 2){
   uM1("checkParam(); useWeath[%d] error!!", p.useWeath);
   return -1;
   }
   if(p.useTrk < 0 || p.useTrk > 2){
   uM1("checkParam(); useTrk[%d] error!!", p.useTrk);
   return -1;
   }
   if(p.useChop < 0 || p.useChop > 2){
   uM1("checkParam(); useChop[%d] error!!", p.useChop);
   return -1;
   }
   if(p.useFe < 0 || p.useFe > 2){
   uM1("checkParam(); useFe[%d] error!!", p.useFe);
   return -1;
   }
   if(p.useIf1 < 0 || p.useIf1 > 2){
   uM1("checkParam(); useIf1[%d] error!!", p.useIf1);
   return -1;
   }
   if(p.useIf2 < 0 || p.useIf2 > 2){
   uM1("checkParam(); useIf2[%d] error!!", p.useIf2);
   return -1;
   }
  */
  if(p.BeNum < 0){
    uM1("checkParam(); BeNum[%d] < 0 error!!", p.BeNum);
    return -1;
  }

  /* // 230523
  if(!confSetKey("EnvAnt")){
    uM("checkParam(); EnvAnt error!!");
    confPrint();
    printf("==================\n");
    return -1;
  }
  */
  /* 090113 out
     if(!confSetKey("EnvEph")){
     uM("checkParam(); EnvEph error!!");
     return -1;
     }
     if(!confSetKey("EnvTime")){
     uM("checkParam(); EnvTime error!!");
     return -1;
     }
  */
  /* 090610 out, since we use scanTable.scanTableGetScanFlag() instead. 
     if(!((p.ScanFlag >= 1 && p.ScanFlag <= 6)
     || (p.ScanFlag >= 1001 && p.ScanFlag <= 1002))){
     uM1("ScanFlag(%d) error!!", p.ScanFlag);
     return -1;
     }
  */
  if(p.OnOffTime < 0){
    uM1("checkParam(); OnOffTime[%d] error!!", p.OnOffTime);
    return -1;
  }
  if(p.SetNumber < 0){
    uM1("checkParam(); SetNumber[%d] error!!", p.SetNumber);
    return -1;
  }
  if(p.RSkyTime < 0){
    uM1("checkParam(); RSkyTime[%d] error!!", p.RSkyTime);
    return -1;
  }
  /* 090610 out, since we use scanTable.scanTableGetLineTime() instead. 
     if(p.LineTime < 0){
     uM1("checkParam(); LineTime[%lf] error!!", p.LineTime);
     return -1;
     }
  */
  if(p.TimeScan < 0){//080530 in //090608 in for 30 cm
    uM1("checkParam(); TimeScan[%lf] error!!", p.TimeScan);
    return -1;
  }
  if(p.TimeApp < 0){//080530 in //090608 in for 30 cm
    uM1("checkParam(); TimeApp[%lf] error!!", p.TimeApp);
    return -1;
  }
  return 0;
}
/*
 * invoked by obsInit()
 * thread 1, phase C0
 */
int _loadAntennaFile(){

  char tmp[1024];
  //char FileAnt[128];//090610 in //230523
  char FileAnt[128];
  FILE* fp;

  memset(FileAnt, 0, sizeof(char)*128);
  //if(confSetKey("EnvAnt")) // 230523
  //strcpy(FileAnt, confGetVal());//090610 in // 230523
  strcpy(FileAnt, "../etc/ant_err.dat");
//  strcpy(FileAnt, "../etc/ant_err_DomeFuji.dat");
  /* 090610 out
     strcpy(p.FileAnt, confGetVal());
  */
  if((fp = fopen(FileAnt, "r")) == NULL){
    uM1("obsInit(); 230523 Ant file open error \"%s\"", FileAnt);//090610 in
    /* 090610 out
       if((fp = fopen(p.FileAnt, "r")) == NULL){
       uM1("obsInit(); Ant file open error %s", p.FileAnt);//081125 in
    */
    //		uM1("obsInit(); Ant file open error %s", p.EnvAnt);
    return 1;
  }
  else{
    while(fgets(tmp, 255, fp) != NULL){
      //printf(" %s¥n", tmp);
      if(tmp[0] == ' '){
        double AntX;
        double AntY;
        double AntZ;
        sscanf(tmp, " %lf %lf %lf", &AntX, &AntY, &AntZ);
        sprintf(tmp, "AntX %lf", AntX);
        confAddStr(tmp);
        sprintf(tmp, "AntY %lf", AntY);
        confAddStr(tmp);
        sprintf(tmp, "AntZ %lf", AntZ);
        confAddStr(tmp);
        //uM3("AntPos %lf %lf %lf", AntX, AntY, AntZ);
        break;
      }
    }
    fclose(fp);
  }
  fp = NULL;

  return 0;//normal end

}

/*
 * invoked by init()
 * thread 1, phase A0
 */
int _setEnvironmentForTrk45(){


  //check the parameters
  /* 090605 out, since they are in penguin_tracking
     if(!confSetKey("EnvEph")){
     uM("checkParam(); EnvEph error!!");
     return -1;
     }
     if(!confSetKey("EnvTime")){
     uM("checkParam(); EnvTime error!!");
     return -1;
     }
  */
  //load from configuration
  /* 090605 out
     sprintf(p.EnvAnt, "ANTFILE=%s", p.FileAnt);
  */
  /*
    if(confSetKey("EnvAnt")){
    strcpy(p.FileAnt, confGetVal());
    sprintf(p.EnvAnt, "ANTFILE=%s", confGetVal());
    }
  */
  /* 090605 out, since they are in penguin_tracking
     if(confSetKey("EnvEph"))
     sprintf(p.EnvEph, "EPHFILE=%s", confGetVal());
     if(confSetKey("EnvTime"))
     sprintf(p.EnvTime, "TIMEFILE=%s", confGetVal());



     if(putenv(p.EnvAnt) != 0){
     uM1("init(); Cannot add value to environnmet (%s)", p.EnvAnt);
     return 1;
     }
     if(putenv(p.EnvEph) != 0){
     uM1("init(); Cannot add value to environnmet (%s)", p.EnvEph);
     return 1;
     }
     if(putenv(p.EnvTime) != 0){
     uM1("init(); Cannot add value to environnmet (%s)", p.EnvTime);
     return 1;
     }
  */
  return 0;//normal end
}

#endif

