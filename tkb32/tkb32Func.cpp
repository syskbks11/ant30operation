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
/*
typedef struct sParamTkb32{
  //! 各制御の起動フラグ 0:起動無し, 1:起動, 2:起動(通信部以外)
  int useWeath;
  int useTrk;
  int useChop;
  int useFe;
  int useIf1;
  int useIf2;
  //int* BeUse;

  char FileAnt[128];
	//The following three values should be static, since they are passed to putenv(), though they appear only in _setEnvironmentForTrk45().
  char EnvAnt[128];
  char EnvEph[128];
  char EnvTime[128];

  double OnOffTime;
  int ScanFlag;        //!< 1:OnOff 2:5Point 3:Grid 4:Random 5:9Point 6:Rastar 1001:SourceTracking 1002:Lee Tracking
  int SetNumber;
  int RSkyTime;
  double LineTime;     //! ラスターのアプローチ時間を考慮した1Lineの時間 [sec]
  int BeNum;           //!< バックエンドの最大台数

  //! 内部で使用する変数
  int isInitialized;            //!< 初期化の状態 0:初期化前 1:初期化済み
  tmClass_t* vtm;           //!< tmモジュール用
  pthread_t thrdUpdateId;//!< ステータススレッドID
  int thrdUpdateRun;   //!< ステータス取得スレッド 0:停止中 1:実行中
  pthread_t thrdObsId; //!< 観測スレッドID
  int thrdObsRun;      //!< 観測スレッド 0:停止中 1:実行中
  int cntOn;           //!< On点観測のカウントを行う
  int isStow;          //!< stowが入っているか 0:unStow 1:Stow

  //! 通信用のデータ格納
  int reqRet;          //!< クライアントリクエストの結果 0:成功 other:失敗
  tReq* req;           //!< クライアントからの要求
  tAns* ans;           //!< クライアントへの応答データ
  tAnsState stat;      //!< 全ステータス情報
}tParamTkb32;
*/
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
  ret = trkInit();
  if(ret){
    uM1("init(); trkInit(); ret = %d", ret);
    return 2;
  }
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

  //! 気象モジュール起動
  ret = weathStart();
  p.stat.weathState = ret;
  if(ret){
    uM2("init(); weathStart(); return %d(%s)", ret, errnoStr(ret));
    return 3;
  }
  else{
    uM("init(); weathStart();");
	if(controlBoardGetWeather() == CONTROL_BOARD_USE_FULL){//090610 in
/* 090610 out
    if(p.useWeath == 1){
*/
      int e = 1;
      //! データ取得まで最大21秒待つ
      for(int i=0; i < 21; i++){
	weathGetWeath(weath);
	if(weath[0] != 0 && weath[1] != 0 && weath[2] != 0){
	  char tmp[256] = {'\0'};
	  e = 0;
	  weathGetWind(&weath[3]);
	  sprintf(tmp, "Tamb=%lf Pamb=%lf Pwater=%lf WindD=.0%lf WindAvg=%lf WindMax=%lf",
		  weath[0], weath[1], weath[2], weath[3], weath[4], weath[5]);
	  uM1("%s", tmp);
	  break;
	}
	tmSleepMSec(sleepTime);
      }
      if(e){
	uM("init(); weathGetWeath(); time out");
	p.stat.weathState = 1;
	return 3;
      }
    }
  }

  //! モニタリングスレッド起動
  int state = pthread_create(&p.thrdUpdateId, NULL, thrdUpdate, NULL);
  if(state != 0){
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
  sprintf(tmp, "%s/tkb32-%s", logPath, log);
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
    uM2("obsInit(); trkInitParam(); [%d(%s)] error!!", ret, errnoStr(ret));
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
      uM2("obsInit(); beInit(); [%d(%s)] error!!", ret, errnoStr(ret));
      return 2;
    }
  
    //! IF
    ret = ifStart();
    if(ret){
      uM2("obsInit(); ifStart(); [%d(%s)] error!!", ret, errnoStr(ret));
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
	uM2("obsInit(); ifUpdate(); [%d(%s)] error!!", ret, errnoStr(ret));
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
  //double weath[3];
  int ret;

  //! 追尾モジュール起動
  ret = trkStart();
  if(ret){
    uM2("obsStart(); trkStart(); [%d(%s)] error!!", ret, errnoStr(ret));
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
      uM2("obsAuto(); rOpenWait(); [%d(%s)] error!!", ret, errnoStr(ret));
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

/*! ¥fn int R()
¥brief R-sky観測を行う
¥retval 0 成功
*/
int R(){
  int OnOffR;
  int ret;
  time_t t;

  OnOffR = 2;

  //! 可変減衰器の減衰値をR用に設定
  ifSetAttOnOffR(OnOffR);
  ifRepeat();

  //! trkの準備完了まで待つ。アンテナの移動中は受信機全体が大きく振動しているため
  ret = trackingWait();
  if(ret){
    uM2("R(); trackingWait(); [%d(%s)] error!!", ret, errnoStr(ret));
    return ret;
  }

  //! R-Sky Chopper close
  if(p.thrdObsRun){
    ret = rCloseWait();
    if(ret){
      uM2("R(); rCloseWait(); [%d(%s)] error!!", ret, errnoStr(ret));
      return ret;
    }
  }

  //! R-Sky Obs
  if(p.thrdObsRun){
    uM("# R-Sky     Integ start");
    time(&t);
    t += 1;
    ret = integStart(OnOffR, t);
    if(ret){
      //uM4("R(); Backend answer exe=%d err=%d tsys=%lf totPow=%lf",
      //  p.stat.saacqExe, p.stat.saacqErr, p.stat.saacqTsys, p.stat.saacqTotPow);
      uM2("R(); integStart(); [%d(%s)] error!!", ret, errnoStr(ret)); 
      return ret;
    }
  }

  //! 積分完了待ち
  if(p.thrdObsRun){
    ret = integWait(p.RSkyTime, t);
    //if(p.useSaacq == 1){
      //uM4("R(); Saacq answer exe=%d saerr=%d tsys=%lf totPow=%lf",
      //  p.stat.saacqExe, p.stat.saacqErr, p.stat.saacqTsys, p.stat.saacqTotPow);      
      if(ret){
	uM2("R(); integWait(); [%d(%s)] error!!", ret, errnoStr(ret));
	return ret;
      }
      //if(p.stat.saacqErr != 1){
      //uM2("R(); saerr=%d(%s)",  p.stat.saacqErr, errnoSaacqErr(p.stat.saacqErr));
      //}
    //}
  }
  uM("# R-Sky     Integ end");

  //! R-Sky Chopper Open
  if(p.thrdObsRun){
    ret = rOpenWait();
    if(ret){
      uM2("R(); rOpenWait(); [%d(%s)]", ret, errnoStr(ret));
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
  int OnOffR;
  int ret;
  double waitTime = 0;
  time_t t;
	long tIntegS = 0;//081206 in //090806 from Ogino by Nagai

  //! R Open check
  if(p.thrdObsRun){
    ret = rOpenWait();
    if(ret){
      uM2("OnOff(); rOpenWait(); [%d(%s)]", ret, errnoStr(ret));
      return ret;
    }
  }

  //! 可変減衰器の減衰値をOnOff用に設定
  ifSetAttOnOffR(1);

  if(OnOff > 0){
    //! OnPoint
    ifRepeat();
    OnOffR = 0;
  }
  else{
    //! OffPoint
    OnOffR = 1;
    //! ローカル周波数のdoppler補正
    ret = ifUpdate();
    p.stat.if1State = ret;
    p.stat.if2State = ret;
	if(ret && (controlBoardGet1stIF() || controlBoardGet2ndIF())){//090610 in
/* 090610 out
    if(ret && (p.useIf1 || p.useIf2)){
*/
      uM2("OnOff(); ifUpdate(); [%d(%s)] error!!", ret, errnoStr(ret));
      return IF_STATE_ERR;
    }
    else{
      //! 準備完了まで待つ(保留)
    }
  }

  //! trkの準備完了まで待つ
  ret = trackingWait();
  if(ret){
    uM2("OnOff(); trackingWait(); [%d(%s)] error!!", ret, errnoStr(ret));
    return ret;
  }

	const int scanFlag = scanTableGetScanFlag();//090610 in

  //! OnOff Obs
  if(p.thrdObsRun){
    //! 観測点を表示
    if(OnOff > 0){
      uM("# On-Point  Integ start");
    }
    else{
      uM("# Off-Point Integ start");
    }

    //! 次の1正秒後から観測に入る
    time(&t);
    t += 1;

	if(OnOff > 0){//080530 //090608 in for 30-cm
		if(scanFlag == SCANTABLE_FLAG_RASTER || scanFlag == SCANTABLE_FLAG_CROSS){//090611 in
/* 090611 out
		if(scanFlag == 6){
*/
			ret = trkSetRastStart(t+1);
			if(ret){
				uM2("OnOff(); trkSetRastStart(); [%d(%s)] error!!", ret, errnoStr(ret));
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
	}

    ret = integStart(OnOffR, tIntegS);//! RasterのOnScan以外は、そのままの積分開始時刻を渡す //090608 in for 30-cm
/* 090608 out
    ret = integStart(OnOffR, t+1);
*/
    if(ret){
      uM2("OnOff(); integStart(); [%d(%s)] error!!", ret, errnoStr(ret)); 
      return ret;
    }

    if(OnOff > 0){
      //! On
		if(scanFlag != SCANTABLE_FLAG_RASTER && scanFlag != SCANTABLE_FLAG_CROSS){//090611 in
/* 090611 out
      if(scanFlag != 6){
*/
	//! Not Raster
	waitTime = p.OnOffTime;
      }
      else{
	//! Raster
	waitTime = scanTableGetLineTime();
/* 090610 out 
	waitTime = p.LineTime;
*/
/* 080530 out? 090611 out for 30 cm
	ret = trkSetRastStart(t+1);
	if(ret){
	  uM2("OnOff(); trkSetRastStart(); [%d(%s)] error!!", ret, errnoStr(ret));
	  return ret;
	}
*/
      }
    }
    else{
      //! Off
      waitTime = p.OnOffTime;
    }
  }

  //! 積分完了待ち
  if(p.thrdObsRun){
    ret = integWait(waitTime, tIntegS);//090608 in for 30-cm
/* 090608 out
    ret = integWait(waitTime, t);
*/
    //if(p.useSaacq == 1){
      //uM4("OnOff(); Saacq answer exe=%d saerr=%d tsys=%lf totPow=%lf",
      //  p.stat.saacqExe, p.stat.saacqErr, p.stat.saacqTsys, p.stat.saacqTotPow);
      if(ret){
	uM2("OnOff(); integWait(); [%d(%s)] error!!", ret, errnoStr(ret));
	return ret;
      }
      //if(p.stat.saacqErr != 1){
      //uM2("OnOff(); saerr [%d(%s)]",  p.stat.saacqErr, errnoSaacqErr(p.stat.saacqErr));
      //}
      //}
  }  

  //! 成功した場合は現在の観測点とOn点の場合はそのカウント数を表示
  if(p.thrdObsRun){
    if(OnOff > 0){
      p.cntOn++;
      uM1("# On-Point  Integ end (On-Count:%d)", p.cntOn);
    }
    else{
      uM("# Off-Point Integ end");
    }
  }
  //uM("debug end onoff check time");
  return 0;
}

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
    uM2("update(); weathRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.weathState){
    uM2("update(); weathRepeat(); [%d(%s)] recovery", ret, errnoStr(ret));
  }
  p.stat.weathState = ret;
  if(ret == 0){
    weathGetWeath(weath);
    calcSetWeath(weath);
  }
  
  //! trk
	//uM1("DEBUG: update(); p.stat.trkState = %d", p.stat.trkState);//090608 in
  ret = trkRepeat();
  if(ret != 0 && ret != p.stat.trkState){
    uM2("update(); trkRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.trkState){
    uM2("update(); trkRepeat(); [%d(%s)] recovery", ret, errnoStr(ret));
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
    uM2("update(); chopRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.chopState){
    uM2("update(); chopRepeat(); [%d(%s)] recovery", ret, errnoStr(ret));
  }  
  p.stat.chopState = ret;

  //! fe
  ret = feRepeat();
  if(ret != 0 && ret != p.stat.feState){
    uM2("update(); feRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.feState){
    uM2("update(); feRepeat(); [%d(%s)] recovery", ret, errnoStr(ret));
  }
  p.stat.feState = ret;

  //! if
  /*ret = ifRepeat();
  if(ret != 0 && ret != p.stat.if1State){
    uM2("update(); ifRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.if1State){
    uM2("update(); ifRepeat(); [%d(%s)] recovery", ret, errnoStr(ret));  
  }
  p.stat.if1State = ret;
  p.stat.if2State = ret;*/

  //! saacq
  /*ret = saacqReqState();
  if(ret != 0 && ret != p.stat.saacqState){
    uM2("update(); saacqReqState(); [%d(%s)] error!!", ret, errnoStr(ret));
    err = 1;
  }
  else if(ret != p.stat.saacqState){
    uM2("update(); saacqReqState(); [%d(%s)] recovery", ret, errnoStr(ret));
  }
  p.stat.saacqState = ret;*/

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
	if(stat==0){
	  break;
	}
      }
      tmSleepMSec(sleepTime);
    }
    //! Error
    if(p.stat.trkState){
      return p.stat.trkState;
    }
    else if(stat){
      return TRK_TRACKING_TIMEOUT_ERR;
    }
  }
  else{
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

/*! ¥fn int integStart(int OnOffR)
¥brief 分光計積分開始
¥param[in] OnOffR 0:On-Point 1:Off-Point 2:R-Sky
¥param[in] startTime 積分を開始する時刻time(&t);で取得する時刻表記
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int integStart(int OnOffR, const time_t startTime){
  double RADEC[2];
  double LB[2];
  double AZEL[2];
  double DRADEC[2];
  double DLB[2];
  double DAZEL[2];
  double PAZEL[2];
  double RAZEL[2];
  double PA = 0;
  double FQIF1;
  double VRAD;
  double IFATT;
  double weath[3];
  double wind[3];
  double TAU = 0;        //!< 大気の厚さ
  double BATM = 0;       //!< 大気の温度 [K]
  double IPINT;
  int exe;
  int saerr;
  double tsys;
  double totPow;
  int ret;
  time_t t;
  struct tm* stm;
  char timeStr[16];

  //! 使用していない場合は終了
  //if(p.useSaacq != 1){
  //  return SAACQ_NOT_USE;
  //}

  //! 現在のステータスを確認する
  ret = beReqState();
  p.stat.saacqState = ret;
  if(ret){
    return ret;
  }
  for(int i = 0; i < p.BeNum; i++){
    beGetAns(i, &exe, &saerr, &tsys, &totPow);
    p.stat.saacqTsys = tsys;
    p.stat.saacqTotPow = totPow;
    p.stat.saacqExe = exe;
    p.stat.saacqErr = saerr;
    if(exe != 0 && exe != 1 && exe != 2){
      //!< -1:Failed 1:Success 2:Standby 3:Execution 4:Accept 5:Reject
      uM2("integStart(); Be%02d exeFlag(%d) error!! ", i + 1, exe);
      p.stat.saacqState = SAACQ_STATE_ERR;
      return p.stat.saacqState;
    }
  }

  //! 積分開始指令
  t = startTime;
  stm = localtime(&t);
  sprintf(timeStr, "%02d:%02d:%02d", stm->tm_hour, stm->tm_min, stm->tm_sec);
  uM1("integStart(); startTime=%s", timeStr);

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
/*
  ifGetLo1PFreq(&FQIF1);
  ifGetVRAD(&VRAD);
  ifGetLo1PAtt(&IFATT);
*/
  weathGetWeath(weath);
  weathGetWind(wind);
  IPINT = chopGetTemp2();
  ret = beReqStart(OnOffR, 0, startTime,
		   RADEC, LB, AZEL,
		   DRADEC, DLB, DAZEL,
		   PAZEL, RAZEL,
		   PA, FQIF1, VRAD, IFATT,
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
    ret = beReqState();
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

/*! ¥fn int setReqData(unsigned char* req, int reqSize)
¥brief 中央制御クライアント(tkb32cl,tkb32clGUI)から来たリクエストをセットして処理する。
¥brief 通信プロトコルはtkb32Protocol.hにて定義する。
¥param[in] req リクエストデータ
¥param[in] reqSize リクエストデータのサイズ[Byte]
¥retval 0 成功
¥retval -1 失敗
*/
/*
int setReqData(unsigned char* req, int reqSize){
  int ret;

  if(((tReq*)req)->size != reqSize){
    uM2("setReqData(); size=%d not reqSize=%d", ((tReq*)req)->size, reqSize);
    return -1;
  }
  p.req = (tReq*)req;

  //! プロトコルバージョンチェック
  if(p.req->reqVer != TKB32_REQ_VER){
    uM2("setReqData(); reqVer(%d) != TKB32_REQ_VER(%d)", p.req->reqVer, TKB32_REQ_VER);
    p.reqRet = 1;
    return 0;
  }

  //uM1("setReqData(); reqFlg=%d", p.req->reqFlg);
  if(p.req->reqFlg == 0){
    //! Status
  }
  else if(p.req->reqFlg == 1){
    //! Initialize
    uM("setReqData(); Request Initialize");
    if(p.thrdObsRun){
      uM("Reject Alreqdy Startobs");
      p.reqRet = 1;
    }
    else{
      if(p.isInitialized){
	uM("End current obs");
	obsEnd();
	tmSleepMSec(1000); //!< 分光計のネットワークソケットの再起動が終わるまで1sec待つ。
      }
      tReqInit* ini = (tReqInit*)(&(p.req->data));
      (&ini->param)[(ini->paramSize)-1] = '\0';
      //uM2("setReqData(); reqSize=%d paramSize=%d", reqSize, ini->paramSize);
      if((&ini->param)[ini->paramSize - 1] != '\0'){
	uM("setReqData(); invalid request data");
	p.reqRet = 1;
	return -1;
      }
      confInit();
      confAddFile(tkb32ConfName);
      confAddStr((const char*)&ini->param);
      ret = obsInit();
      if(ret){
	uM("Error in obsInit();");
	p.reqRet = 1;
	obsEnd();
	return -1;
      }
      p.reqRet = 0;
    }
  }
  else if(p.req->reqFlg == 2){
    //! RemoteControle
    p.reqRet = 0;
    uM("setReqData(); Request remote controle");
    tReqRemote* rem = (tReqRemote*)&p.req->data;
    //! debug
    //uM1("trkReq = %u", rem->trkReq); 
    //uM1("chopReq = %u", rem->chopReq);
    //uM1("saacqReq = %u", rem->saacqReq);
    //uM1("sasoftReq = %u", rem->sasoftReq);
    if(rem->trkReq & 1){
      //! AzEl
    }
    if(rem->trkReq & 2){
      //! Pointing Offset
      //trkSetPof(rem->trkPof, rem->trkPofCoord);
      //! 取りあえずAZEL指定で。
      trkSetPof(rem->trkPof, 3);
    }
    if(rem->trkReq & 4){
      //! Trcking Stop
      uM("Request Trcking Stop");
      trkStop();
    }
    if(rem->trkReq & 8){
      //! Stow unlock
      uM("Request Stow unlock");
	if(p.useTrk == 1){
		p.reqRet = acuSetAcuUnStow();
	}else{
		p.reqRet = 0;
	}

    }
    if(rem->trkReq & 16){
      //! Stow lock
      uM("Request Stow lock");
      if(p.thrdObsRun){
	uM("Reject  Already startobs");
	p.reqRet = 1;
      }
      else{
	if(p.useTrk == 1){
		ret = acuSetAcuMood(3);
		if(ret){
		  p.reqRet = 1;
		}
	}
      }
    }
    if(rem->trkReq & 32){
      //! Progrum tracking mood
      uM("Request program tracking mood");
	if(p.useTrk == 1){
		ret = acuSetAcuMood(2);
		if(ret){
			p.reqRet = 1;
		}
	}
    }
    if(rem->trkReq & 64){
      //! ACU Standby
      uM("Request ACU Standby");
	if(p.useTrk == 1){
		ret = acuSetAcuMood(0);
		if(ret){
			p.reqRet = 1;
		}
	}
    }
    if(rem->trkReq & 128){
      //! Drive unlock
      uM("Request Drive unlock");
	if(p.useTrk == 1){
		ret = acuSetAcuBan(0);
		if(ret){
			p.reqRet = 1;
		}
	}
    }
    if(rem->trkReq & 256){
      //! Drive lock
      uM("Request Drive lock");
      //if(p.thrdObsRun){
      //uM("Reject  Already startobs");
      //p.reqRet = 1;
      //}
      //else{
	if(p.useTrk == 1){
		ret = acuSetAcuBan(1);
		if(ret){
			p.reqRet = 1;
		}
	}
      //}
    }
    //if(rem->trkReq & 512){
      //!Tracking Start
      //uM("Request Tracking Start");
    //}
    if(rem->chopReq == 1){
      //! close
      uM("Requset Chopper close");
      chopSetReq(2);       //!< 2:close
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	uM2("setReqData(); chopRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
	p.reqRet = ret;
      }
    }
    else if(rem->chopReq == 2){
      //! open
      uM("Requset Chopper open");
      chopSetReq(3);       //!< 3:open
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	uM2("setReqData(); chopRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
	p.reqRet = ret;
      }
    }
    else if(rem->chopReq == 3){
      //! origine
      uM("Request Chopper origine");
      chopSetReq(4);       //!< 4:origine
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	uM2("setReqData(); chopRepeat(); [%d(%s)]", ret, errnoStr(ret));
	p.reqRet = ret;	
      }
    }
    else if(rem->chopReq == 4){
      //! stop
      uM("Request Chopper stop");
      chopSetReq(5);       //!< 5:stop
      ret = chopRepeat();
      p.stat.chopState = ret;
      if(ret){
	uM2("setReqData(); chopRepeat(); [%d(%s)]", ret, errnoStr(ret));
	p.reqRet = ret;
      }
    }
    if(rem->if1Req & 1){
      //! SetFreq
      ifSetLo1Freq(rem->if1Freq);
    }
    if(rem->if1Req & 2){
      //! SetAmplitude
      ifSetLo1Amp(rem->if1Amp);
    }
    if(rem->if1Req & 4){
      //! SetAtt
      ifSetLo1Att(rem->if1Att);
    }
    if(rem->if2Req & 1){
      //! SetFreq
      ifSetLo2Freq(rem->if2Freq);
    }
    if(rem->if2Req & 2){
      //! SetAmp
      ifSetLo2Amp(rem->if2Amp);
    }
    if(rem->if1Req || rem->if2Req){
      //! Update if
      ifRepeat();
    }
    if(rem->saacqReq & 1){
      //! Backend Set Integ Time
      if(p.isInitialized == 0){
	uM("setReqData(); Not initialized");
	p.reqRet = 1;
	return -1;
      }
      //uM("Request Set SaAcq Integ");
    }
    if(rem->saacqReq & 2){
      //! Start integration
      uM1("Request start integration OnOffR=%d", rem->saacqOnOffR);
      if(p.isInitialized == 0){
	uM("setReqData(); Not initialized");
	p.reqRet = 1;
	return -1;
      }
      if(p.thrdObsRun){
	uM("Reject  Already startobs");
	p.reqRet = 1;
      }
      else{
	ret = beReqState();
	if(ret){
	  uM2("setReqData(); sacqReqState(); [%d(%s)] error!!", ret, errnoStr(ret));
	  p.reqRet = 1;
	}
	else{
	  //int exe;
	  //int saerr;
	  //double tsys;
	  //double totPow;
	  time_t t;
	  //beGetAns(&exe, &saerr, &tsys, &totPow);
	  //if(exe == 3){
	  //  uM("Request Integ start backend is in execution");
	  //  p.reqRet = 1;
	  //}
	  //else{
	    int OnOffR;
	    //! 分光計への指示だし
	    OnOffR = rem->saacqOnOffR;
	    time(&t);
	    t += 1;
	    ret = integStart(OnOffR, t);
	    if(ret){
	      uM2("setReqData(); integStart(); [%d(%s)] error!!", ret, errnoStr(ret));
	      p.reqRet = ret;	  
	    }
	  //}
	}//!< if(beReqState())
      }//!< if(p.thrdObsRun)
    }
    if(rem->saacqReq & 4){
      //! Stop backend
      if(p.isInitialized == 0){
	uM("setReqData(); Not initialized");
	p.reqRet = 1;
	return -1;
      }
      uM("Request Stop Backend");
      p.reqRet = beReqStop();
    }
    if(rem->sasoftReq & 1){
      //! Dummy
      if(p.isInitialized == 0){
	uM("setReqData(); Not initialized");
	p.reqRet = 1;
	return -1;
      }
    }
  }
  else if(p.req->reqFlg == 3){
    //! Strat Obs
    p.reqRet = 1;
    uM("setReqData(); Start observation");
    if(p.isInitialized == 0){
      uM("Reject Not initialized");
    }
    else if(p.thrdObsRun){
      uM("Reject Alreqdy Startobs");
    }
    else{
      ret = obsStart();
      if(ret){
	uM1("setReqData(); obsStart(); ret = %d", ret);
	obsStop();
	return ret;
      }
      int state = pthread_create(&p.thrdObsId, NULL, obsAuto, NULL);
      if(state != 0){
	uM1("setReqData(); pthred_create(obsAuto); state=%d error", state);
	return -2;
      }
      p.reqRet = 0;
    }
  }
  else if(p.req->reqFlg == 4){
    //! Stop observation
    p.reqRet = 1;
    uM("setReqData(); Request stop observation");
    if(p.isInitialized == 0){
      uM("Reject  Not initialized");
    }
    else if(p.thrdObsRun == 0){
      uM("Reject  Not Startobs");
    }
    else{
      obsStop();
      p.reqRet = 0;
    }
  }
  else if(p.req->reqFlg == 5){
    //! End Obs
    p.reqRet = 1;
    uM("setReqData(); Request end observation");
    if(p.isInitialized == 0){
      uM("Reject  Not Initialized");
    }
    else{
      if(p.thrdObsRun){
	p.reqRet = obsStop();
      }
      obsEnd();
      p.reqRet = 0;
    }
  }
  else if(p.req->reqFlg == 6){
    //! Paramter List
  }

  //uM1("debug p.req->reqFlg=%d", p.req->reqFlg);
  return 0;
}
*/

/*! ¥fn int getAnsData(unsigned char** ans, int *ansSize)
¥brief 応答データを取得する
¥brief 通信プロトコルはtkb32Protocol.hにて定義する。
¥param[out] ans 応答データ
¥param[out] ansSize 応答データのサイズ[Byte]
¥retval 0 成功
*/
/*
int getAnsData(unsigned char** ans, int *ansSize){
  const size_t headSize = sizeof(p.ans->size) + sizeof(p.ans->ansVer) + sizeof(p.ans->ansFlg);

  //uM("getAnsData();");
  if(p.ans){
    delete[] p.ans;
    p.ans = NULL;
  }
  long size;
  time_t t;

  if(p.req->reqFlg==0){
    //! Status
    time(&t);
    p.stat.UT = t;
	p.stat.LST = calcGetLST();
    //trkGetLST(&p.stat.LST);//081113 out
    weathGetWeath(&p.stat.weathData[0]);
    weathGetWind(&p.stat.weathData[3]);
    p.stat.trkScanCnt = p.cntOn;
    trkGetXY(&p.stat.trkXYCoord, p.stat.trkXY);
    calcGetScanOff(&p.stat.trkSofCoord, p.stat.trkSof);
    calcGetPointOff(&p.stat.trkPofCoord, p.stat.trkPof);
    trkGetPZONE(&p.stat.trkPZONE);
	p.stat.trkRZONE = acuGetRZONE();
    //trkGetRZONE(&p.stat.trkRZONE);//081113
    calcGetPAZEL(p.stat.trkP3AZEL);
    //printf("PAZEL %lf %lf¥n", p.stat.trkPAZEL[0], p.stat.trkPAZEL[1]);
    acuGetRAZEL(p.stat.trkRAZEL);
    trkGetEAZEL(p.stat.trkEAZEL);
	p.stat.trkTracking = trkGetTrackStatus();
    //trkGetTrackStatus(&p.stat.trkTracking);//081113 out
    acuGetACUStatus(p.stat.trkACUStatus);
    //chopSetReq(1);
    //chopRepeat();
    p.stat.chopStat = chopGetState();
    p.stat.chopMoter = chopGetMoter();
    p.stat.chopTemp1 = chopGetTemp1();
    p.stat.chopTemp2 = chopGetTemp2();
    p.stat.feK = feGetTemperatureK();
    p.stat.fePa = feGetPressurePa();

	p.stat.ifVrad = calc2GetVRAD();
	p.stat.if1PFreq = ifGetLo1PFreq();
	p.stat.if1PAmp = ifGetLo1PAmp();
	p.stat.if1PAtt = ifGetLo1PAtt();
	p.stat.if1RFreq = ifGetLo1RFreq();
	p.stat.if1RAmp = ifGetLo1RAmp();
	p.stat.if1RAtt = ifGetLo1RAtt();
	p.stat.if2PFreq = ifGetLo2PFreq();
	p.stat.if2PAmp = ifGetLo2PAmp();
	p.stat.if2RFreq = ifGetLo2RFreq();
	p.stat.if2RAmp = ifGetLo2RAmp();
    //if(p.thrdObsRun  == 0 && p.useSaacq){
    //if(p.thrdObsRun == 0){
    //  p.stat.saacqState = beReqState();
    //  if(p.stat.saacqState){
    //beGetAns(0, &p.stat.saacqExe, &p.stat.saacqErr, &p.stat.saacqTsys, &p.stat.saacqTotPow);
    //  }
    //}
    //uM("getAnsData(); get data");
    //uM1("getAnsData(); p.req->reqFlg=%d", p.req->reqFlg)
    size = headSize + sizeof(p.stat);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = 0;
    memcpy(&(p.ans->data), &p.stat, sizeof(p.stat));
  }
  else if(p.req->reqFlg==1){
    //! Initialize
    size = headSize + sizeof(p.ans->data);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = p.req->reqFlg;
    p.ans->data = p.reqRet;
  }
  else if(p.req->reqFlg==2){
    //! RemoteControle
    size = headSize + sizeof(p.ans->data);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = p.req->reqFlg;
    p.ans->data = p.reqRet;
  }
  else if(p.req->reqFlg==3){
    //! Strat Obs
    size = headSize + sizeof(p.ans->data);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = p.req->reqFlg;
    p.ans->data = p.reqRet;
  }
  else if(p.req->reqFlg==4){
    //! Stop Obs
    size = headSize + sizeof(p.ans->data);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = p.req->reqFlg;
    p.ans->data = p.reqRet;
  }
  else if(p.req->reqFlg==5){
    //! End
    size = headSize + sizeof(p.ans->data);
    p.ans = (tAns*)new unsigned char[size];
    p.ans->size = size;
    p.ans->ansFlg = p.req->reqFlg;
    p.ans->data = p.reqRet;
  }
  p.ans->ansVer = TKB32_ANS_VER;
  *ans = (unsigned char*)p.ans;
  *ansSize = size;
  return 0;
}
*/

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
    uM("checkParam(); EnvAnt error!!");
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


  if(!confSetKey("EnvAnt")){
    uM("checkParam(); EnvAnt error!!");
    return -1;
  }
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
	char FileAnt[128];//090610 in
	FILE* fp;

	memset(FileAnt, 0, sizeof(char)*128);
	if(confSetKey("EnvAnt"))
		strcpy(FileAnt, confGetVal());//090610 in
/* 090610 out
		strcpy(p.FileAnt, confGetVal());
*/
	if((fp = fopen(FileAnt, "r")) == NULL){
		uM1("obsInit(); Ant file open error %s", FileAnt);//090610 in
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

