/*!
\file tkb32/saacqTest.cpp
\author Y.Koide
\date 2006.12.20
\brief SA ACQ Test Program
*/

#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include "../u/u.h"
#include "../conf/conf.h"
#include "../tm/tm.h"
#include "trk.h"
#include "if.h"
#include "saacq.h"

typedef struct sParamTkb32{
  int OnOffTime;
  int Pattern;
  int SeqNumber;
  int RSkyTime;
  int Interval;
  double trkInterval;
  double LoInterval;
}tParamTkb32;

static void setParam(tParamTkb32* p);

int MAIN__(void){
  tParamTkb32 p;
  void* ptm1;
  static char cenvdt[3][128] = {
    "ANTFILE=./etc/ant_err.dat",
    "EPHFILE=./etc/ephfile/19241221-20500102.ORG",
    "TIMEFILE=./etc/time.dat"
  };
  int ret;
  
  //!初期化処理
  try{
    //!ログファイル設定
    uInit("./log/saacqTest");
    uM("Initialization.");
    //!パラメーター読込み
    confInit();
    confAddFile("./etc/tkb32.conf");
    confAddFile("./etc/test.device");
    confAddFile("./etc/test.scan");
    confAddFile("./etc/test.source");
    confPrint();  
    memset(&p, 0, sizeof(p));
    setParam(&p);
    //!タイマー初期化
    ptm1=tmInit();
    //!環境変数設定
    for (int i = 0; i < 3; i++) {
      if (putenv(cenvdt[i]) != 0) {
	uE1("Cannot add value to environment (%s)\n", cenvdt[i]);
      }
    }
    //!各種制御モジュール初期化
    int ONE[1]={1};
    int TWO[1]={2};
    trkInit(TWO);
    //chopInit();
    ifInit(TWO);
    saacqInit(ONE);
  }
  catch(...){
    uE("Error in initialization");
  }

  //!メイン処理
  try{
    int patMax;
    int exe;
    int err;
    double tsys;
    double totPow;
    int OffOnR;
    double weath[3]={0,0,0};
    double wind[3]={0,0,0};
    int IFATT=-20;
    double DRADEC[2];
    double DLB[2];
    double DAZEL[2];
    double PAZEL[2];
    double RAZEL[2];
    double Lo1Freq;
    uM("Starting observation.");
    if(p.Pattern==1){
      patMax=2;
    }
    else if(p.Pattern==2){
      patMax=4;
    }
    if((ret=ifRepeat())!=0)
      uM1("main(); ifRepeat(); return %d", ret);
    if((ret=trkRepeat())!=0)
      uM1("main(); trkRepeat(); return %d", ret);
    //!準備完了まで待つ

    for(int seq=0; seq<p.SeqNumber; seq++){
      //!R-Sky Chopper close
      //! R-sky obs
      tmReset(ptm1);
      ret = trkRepeat();
      //!分光計への指示だし
      OffOnR = 2;
      trkGetDRADEC(DRADEC);
      trkGetDLB(DLB);
      trkGetDAZEL(DAZEL);
      trkGetPAZEL(PAZEL);
      trkGetRAZEL(RAZEL);
      ifGetLo1Freq(&Lo1Freq);
      ret = saacqReqStart(OffOnR, 0,
		    DRADEC, DLB, DAZEL,
		    PAZEL, RAZEL,
		    0, Lo1Freq, 0, IFATT,
		    weath, wind, 0,
		    273, 273);
      if(ret){
	uM("main(); saacqReqStart() ret=%d");
      }
      else{
	//saacqGetAns(&exe, &err, &tsys);
	//uM3("main(); saacqReqStart(); return exe=%d err=%d tsys=%lf", exe, err, tsys);      
      }
      //!R-sky Chopper open
      while(tmGetLag(ptm1)<p.RSkyTime){
	trkRepeat();
      }
      //!積分時間を過ぎたら分光計の結果をチェックしに行く
      while(1){
	trkRepeat();
	ret = saacqReqState();
	tmSleepMSec(1000);
	if(ret==0){
	   ret = saacqGetAns(&exe, &err, &tsys, &totPow);
	}
	if(ret)
	  continue;
	uM4("main(); saacqReqStart(); return exe=%d err=%d tsys=%lf totPow=%lf", exe, err, tsys, totPow);
	if(exe==1 || exe==2){
	  break;
	}
	//if(exe==0){
	//uM1("main(); acq state error. endExeFlag=%d", exe);
	//}
	//if(err!=1){
	//uM1("main(); acq state error. acqErr=%d",  err);
	//}
      }
      //!R-sky Chopper open      
      //!OnOff set if
      for(int i=0; i<p.Interval; i++){
	ifRepeat();
	//!OnOff select
	for(int j=0; j<patMax; j++){
	  switch(j){
	  case 0:
	    //Off1
	    trkSetOnOff(1);
	    OffOnR=0;
	    break;
	  case 1:
	    //On
	    trkSetOnOff(0);
	    OffOnR=1;
	    break;
	  case 2:
	    //Off2
	    trkSetOnOff(2);
	    OffOnR=0;
	    break;
	  case 3:
	    //On
	    trkSetOnOff(0);
	    OffOnR=1;
	    break;
	  default:
	    break;
	  }
	  //!分光計に指示出し
	  trkGetDRADEC(DRADEC);
	  trkGetDLB(DLB);
	  trkGetDAZEL(DAZEL);
	  trkGetPAZEL(PAZEL);
	  trkGetRAZEL(RAZEL);
	  ifGetLo1Freq(&Lo1Freq);
	  ret = saacqReqStart(OffOnR, 0,
			     DRADEC, DLB, DAZEL,
			     PAZEL, RAZEL,
			     0, Lo1Freq, 0, IFATT,
			     weath, wind, 0,
			     273, 273);
	  if(ret){
	    uM("main(); saacqReqStart() ret=%d");
	  }
	  else{
	    //saacqGetAns(&exe, &err, &tsys);
	    //uM3("main(); saacqReqStart(); return exe=%d err=%d tsys=%lf", exe, err, tsys);      
	  }
	  //!OnOff obs
	  tmReset(ptm1);
	  while(tmGetLag(ptm1)<p.OnOffTime){
	    trkRepeat();
	  }
	  //!積分時間を過ぎたら分光計の結果をチェックしに行く
	  while(1){
	    trkRepeat();
	    ret = saacqReqState();
	    if(ret==0){
	      ret = saacqGetAns(&exe, &err, &tsys, &totPow);
	    }
	    if(ret)
	      continue;
	    uM4("main(); saacqReqStart(); return exe=%d err=%d tsys=%lf totPow=%lf", exe, err, tsys, totPow);      
	    if(ret){
	      //uM1("main(); saacqGetAns(); return=%d\n", ret);
	      //exe=-1;
	      //err=-1;
	      //break;
	    }
	    if(exe==1 || exe==2){
	      break;
	    }
	    //if(exe==0){
	    //uM1("main(); acq state error. endExeFlag=%d", exe);
	    //}
	    //if(err!=1){
	    //uM1("main(); acq state error. acqErr=%d",  err);
	    //}
	    tmSleepMSec(1000);
	  }
	}//OnOff
      }//Interval
    }//seq
  }//try
  catch(...){
    uE("Error in repeat!!");
  }

  //!終了処理
  try{
    uM("End observation");
    trkEnd();
    ifEnd();
    tmEnd(ptm1);
    uEnd();
  }
  catch(...){
    uE("Error in end");
  }
  return 0;
}

void setParam(tParamTkb32* p){
  if(confSetKey("OnOffTime"))
    p->OnOffTime = atoi(confGetVal());
  if(confSetKey("Pattern"))
    p->Pattern = atoi(confGetVal());
  if(confSetKey("SeqNumber"))
    p->SeqNumber = atoi(confGetVal());
  if(confSetKey("RSkyTime"))
    p->RSkyTime = atoi(confGetVal());
  if(confSetKey("Interval"))
    p->Interval = atoi(confGetVal());
  if(confSetKey("trkInterval"))
    p->trkInterval = atof(confGetVal());
  if(confSetKey("LoInterval"))
    p->LoInterval = atof(confGetVal());
}
