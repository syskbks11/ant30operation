/*!
  ¥file tkb32/tkb32Parser.cpp
  ¥author NAGAI Makoto
  ¥date 2008.12.08
  ¥brief Tsukuba 32m Central Program, parser for messages from a clint. 
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
//#include "calc.h"
#include "calcBoth.h"
#include "acu.h"
#include "chop.h"
#include "fe.h"
#include "if.h"
//#include "calc2.h"
#include "be.h"
#include "tkb32Protocol.h"
#include "errno.h"
#include "tkb32Func.h"
#include "tkb32Parser.h"
#include "tkb32ParserTkb32Func.h"
#include "controlBoard.h"

typedef struct sParamTkb32Parser{
  tReq* req;           //!< クライアントからの要求
  tAns* ans;           //!< クライアントへの応答データ
}tParamTkb32Parser;


static void _parseTrackingRequest(int trkReq, tReqRemote* rem, tParamTkb32* p);
static void _parseChopperRequest(int chopReq, tParamTkb32* p);
static void _parseIfRequest(tReqRemote* rem);
static int _parseSaacqRequest(int saacqReq, tReqRemote* rem, tParamTkb32* p);

void _obsEnd();

static tParamTkb32Parser pParser;

/*! ¥fn int setReqData(unsigned char* req, int reqSize)
  ¥brief 中央制御クライアント(tkb32cl,tkb32clGUI)から来たリクエストをセットして処理する。
  ¥brief 通信プロトコルはtkb32Protocol.hにて定義する。
  ¥param[in] req リクエストデータ
  ¥param[in] reqSize リクエストデータのサイズ[Byte]
  ¥retval 0 成功
  ¥retval -1 失敗
  * thread 1, phase A1
  */
int setReqData(unsigned char* req, int reqSize){
  static const char tkb32ConfName[] = "../etc/ant30_phaseC0.conf";
  /*
    static const char tkb32ConfName[] = "../etc/tkb32.conf";
  */
  int ret;
  tParamTkb32* p = getParam();
  
  if(((tReq*)req)->size != reqSize){
    uM2("setReqData(); size=%d not reqSize=%d", ((tReq*)req)->size, reqSize);
    return -1;
  }
  pParser.req = (tReq*)req;
  
  //! プロトコルバージョンチェック
  if(pParser.req->reqVer != TKB32_REQ_VER){
    uM2("setReqData(); reqVer(%d) != TKB32_REQ_VER(%d)", pParser.req->reqVer, TKB32_REQ_VER);
    (*p).reqRet = 1;
    return 0;
  }
  
  //uM1("setReqData(); reqFlg=%d", pParser.req->reqFlg);
  switch(pParser.req->reqFlg){
  case 0:
    //uM("setReqData(); Request Status");//090610 in, but it's noisy.
    break;
    /*
      if((*p).req->reqFlg == 0){
      //! Status
      */
  case 1:
    /*
      }
      else if((*p).req->reqFlg == 1){
    */
    //! Initialize
    uM("setReqData(); Request Initialize");
    if((*p).thrdObsRun){
      uM("Reject Alreqdy Startobs");
      (*p).reqRet = 1;
    }
    else{
      //phase C0: initialization of an observation
      if((*p).isInitialized){
        uM("End current obs");
        _obsEnd();
        tmSleepMSec(1000); //!< 分光計のネットワークソケットの再起動が終わるまで1sec待つ。
      }
      tReqInit* ini = (tReqInit*)(&(pParser.req->data));
      (&ini->param)[(ini->paramSize)-1] = '\0';
      //uM2("setReqData(); reqSize=%d paramSize=%d", reqSize, ini->paramSize);
      if((&ini->param)[ini->paramSize - 1] != '\0'){
	uM("setReqData(); invalid request data");
	(*p).reqRet = 1;
	return -1;
      }
      confInit();
      confAddFile(tkb32ConfName);
      confAddStr((const char*)&ini->param);
      ret = obsInit();
      if(ret){
	uM("Error in obsInit();");
	(*p).reqRet = 1;
	_obsEnd();
	return -1;
      }
      (*p).reqRet = 0;
    }
    break;
  case 2:
    /*
      }
      else if((*p).req->reqFlg == 2){
    */
    //! RemoteControle
    (*p).reqRet = 0;
    uM("setReqData(); Request remote control");
    tReqRemote* rem = (tReqRemote*)&pParser.req->data;
    //! debug
    //uM1("trkReq = %u", rem->trkReq); 
    //uM1("chopReq = %u", rem->chopReq);
    //uM1("saacqReq = %u", rem->saacqReq);
    //uM1("sasoftReq = %u", rem->sasoftReq);
    _parseTrackingRequest(rem->trkReq, rem, p);//081208 in
    /* 081208 out
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
       if((*p).useTrk == 1){
       (*p).reqRet = acuSetAcuUnStow();
       }else{
       (*p).reqRet = 0;
       }
       
       }
       if(rem->trkReq & 16){
       //! Stow lock
       uM("Request Stow lock");
       if((*p).thrdObsRun){
       uM("Reject  Already startobs");
       (*p).reqRet = 1;
       }
       else{
       if((*p).useTrk == 1){
       ret = acuSetAcuMood(3);
       if(ret){
       (*p).reqRet = 1;
       }
       }
       }
       }
       if(rem->trkReq & 32){
       //! Progrum tracking mood
       uM("Request program tracking mood");
       if((*p).useTrk == 1){
       ret = acuSetAcuMood(2);
       if(ret){
       (*p).reqRet = 1;
       }
       }
       }
       if(rem->trkReq & 64){
       //! ACU Standby
       uM("Request ACU Standby");
       if((*p).useTrk == 1){
       ret = acuSetAcuMood(0);
       if(ret){
       (*p).reqRet = 1;
       }
       }
       }
       if(rem->trkReq & 128){
       //! Drive unlock
       uM("Request Drive unlock");
       if((*p).useTrk == 1){
       ret = acuSetAcuBan(0);
       if(ret){
       (*p).reqRet = 1;
       }
       }
       }
       if(rem->trkReq & 256){
       //! Drive lock
       uM("Request Drive lock");
       //if((*p).thrdObsRun){
       //uM("Reject  Already startobs");
       //(*p).reqRet = 1;
       //}
       //else{
       if((*p).useTrk == 1){
       ret = acuSetAcuBan(1);
       if(ret){
       (*p).reqRet = 1;
       }
       }
       //}
       }
       //if(rem->trkReq & 512){
       //!Tracking Start
       //uM("Request Tracking Start");
       //}
       */
    _parseChopperRequest(rem->chopReq, p);
    /* 081208 out
       if(rem->chopReq == 1){
       //! close
       uM("Requset Chopper close");
       chopSetReq(2);       //!< 2:close
       ret = chopRepeat();
       (*p).stat.chopState = ret;
       if(ret){
       uM2("setReqData(); chopRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
       (*p).reqRet = ret;
       }
       }
       else if(rem->chopReq == 2){
       //! open
       uM("Requset Chopper open");
       chopSetReq(3);       //!< 3:open
       ret = chopRepeat();
       (*p).stat.chopState = ret;
       if(ret){
       uM2("setReqData(); chopRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
       (*p).reqRet = ret;
       }
       }
       else if(rem->chopReq == 3){
       //! origine
       uM("Request Chopper origine");
       chopSetReq(4);       //!< 4:origine
       ret = chopRepeat();
       (*p).stat.chopState = ret;
       if(ret){
       uM2("setReqData(); chopRepeat(); [%d(%s)]", ret, errnoStr(ret));
       (*p).reqRet = ret;	
       }
       }
       else if(rem->chopReq == 4){
       //! stop
       uM("Request Chopper stop");
       chopSetReq(5);       //!< 5:stop
       ret = chopRepeat();
       (*p).stat.chopState = ret;
       if(ret){
       uM2("setReqData(); chopRepeat(); [%d(%s)]", ret, errnoStr(ret));
       (*p).reqRet = ret;
       }
       }
    */
    _parseIfRequest(rem);
    /* 081208 out
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
    */
    ret = _parseSaacqRequest(rem->saacqReq, rem, p);
    if(ret){
      return ret;
    }
    /* 081208 out
       if(rem->saacqReq & 1){
       //! Backend Set Integ Time
       if((*p).isInitialized == 0){
       uM("setReqData(); Not initialized");
       (*p).reqRet = 1;
       return -1;
       }
       //uM("Request Set SaAcq Integ");
       }
       if(rem->saacqReq & 2){
       //! Start integration
       uM1("Request start integration OnOffR=%d", rem->saacqOnOffR);
       if((*p).isInitialized == 0){
       uM("setReqData(); Not initialized");
       (*p).reqRet = 1;
       return -1;
       }
       if((*p).thrdObsRun){
       uM("Reject  Already startobs");
       (*p).reqRet = 1;
       }
       else{
       ret = beReqState();
       if(ret){
       uM2("setReqData(); sacqReqState(); [%d(%s)] error!!", ret, errnoStr(ret));
       (*p).reqRet = 1;
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
       //  (*p).reqRet = 1;
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
       (*p).reqRet = ret;	  
       }
       //}
       }//!< if(beReqState())
       }//!< if((*p).thrdObsRun)
       }
       if(rem->saacqReq & 4){
       //! Stop backend
       if((*p).isInitialized == 0){
       uM("setReqData(); Not initialized");
       (*p).reqRet = 1;
       return -1;
       }
       uM("Request Stop Backend");
       (*p).reqRet = beReqStop();
       }
    */
    if(rem->sasoftReq & 1){
      //! Dummy
      if((*p).isInitialized == 0){
        uM("setReqData(); Not initialized");
        (*p).reqRet = 1;
        return -1;
      }
    }
    break;
  case 3:
    /*
      }
      else if((*p).req->reqFlg == 3){
    */
    //! Strat Obs
    (*p).reqRet = 1;
    uM("setReqData(); Start observation");
    if((*p).isInitialized == 0){
      uM("Reject Not initialized");
    }
    else if((*p).thrdObsRun){
      uM("Reject Alreqdy Startobs");
    }
    else{
      //phase C1: an observation
      ret = obsStart();
      if(ret){
	uM1("setReqData(); obsStart(); ret = %d", ret);
	obsStop();
	return ret;
      }
      int state = pthread_create(&(*p).thrdObsId, NULL, obsAuto, NULL);
      if(state != 0){
	uM1("setReqData(); pthred_create(obsAuto); state=%d error", state);
	return -2;
      }
      (*p).reqRet = 0;
    }
    break;
  case 4:
    /*
      }
      else if((*p).req->reqFlg == 4){
    */
    //! Stop observation
    (*p).reqRet = 1;
    uM("setReqData(); Request stop observation");
    if((*p).isInitialized == 0){
      uM("Reject  Not initialized");
    }
    else if((*p).thrdObsRun == 0){
      uM("Reject  Not Startobs");
    }
    else{
      obsStop();
      (*p).reqRet = 0;
    }
    break;
  case 5:
    /*
      }
      else if((*p).req->reqFlg == 5){
    */
    //! End Obs
    (*p).reqRet = 1;
    uM("setReqData(); Request end observation");
    if((*p).isInitialized == 0){
      uM("Reject  Not Initialized");
    }
    else{
      //phase C2: terminate an observation
      if((*p).thrdObsRun){
        (*p).reqRet = obsStop();
      }
      _obsEnd();
      (*p).reqRet = 0;
    }
    break;
  case 6:
    /*
      }
      else if((*p).req->reqFlg == 6){
    */
    //! Paramter List
    uM("setReqData(); Request Parameter list");//090610 in
    break;
  default:
    uM1("setReqData(); Invalid request [%d]!! Ignore.", pParser.req->reqFlg);//090610 in
    break;
  }
  
  //uM1("debug (*p).req->reqFlg=%d", (*p).req->reqFlg);
  return 0;
}

/*! ¥fn int getAnsData(unsigned char** ans, int *ansSize)
  ¥brief 応答データを取得する
  ¥brief 通信プロトコルはtkb32Protocol.hにて定義する。
  ¥param[out] ans 応答データ
  ¥param[out] ansSize 応答データのサイズ[Byte]
  ¥retval 0 成功
  * thread 1, phase A1
  */
int getAnsData(unsigned char** ans, int *ansSize){

  tParamTkb32* p = getParam();
  
  const size_t headSize = sizeof(pParser.ans->size) + sizeof(pParser.ans->ansVer) + sizeof(pParser.ans->ansFlg);

  
  //uM("getAnsData();");
  if(pParser.ans){
    delete[] pParser.ans;
    pParser.ans = NULL;
  }
  long size;
  time_t t;
  
  if(pParser.req->reqFlg==0){
    //! Status
    time(&t);
    (*p).stat.UT = t;
    (*p).stat.LST = calcGetLST();
    //trkGetLST(&(*p).stat.LST);//081113 out
    weathGetWeath(&(*p).stat.weathData[0]);
    weathGetWind(&(*p).stat.weathData[3]);
    (*p).stat.trkScanCnt = (*p).cntOn;
    trkGetXY(&(*p).stat.trkXYCoord, (*p).stat.trkXY);
    calcGetScanOff(&(*p).stat.trkSofCoord, (*p).stat.trkSof);
    calcGetPointOff(&(*p).stat.trkPofCoord, (*p).stat.trkPof);
    trkGetPZONE(&(*p).stat.trkPZONE);
    (*p).stat.trkRZONE = acuGetRZONE();
    //trkGetRZONE(&(*p).stat.trkRZONE);//081113
    calcGetPAZEL((*p).stat.trkP3AZEL);
    //printf("PAZEL %lf %lf¥n", (*p).stat.trkPAZEL[0], (*p).stat.trkPAZEL[1]);
    acuGetRAZEL((*p).stat.trkRAZEL);
    trkGetEAZEL((*p).stat.trkEAZEL);
    (*p).stat.trkTracking = trkGetTrackStatus();
    //trkGetTrackStatus(&(*p).stat.trkTracking);//081113 out
    acuGetACUStatus((*p).stat.trkACUStatus);
    //chopSetReq(1);
    //chopRepeat();
    (*p).stat.chopStat = chopGetState();
    (*p).stat.chopMoter = chopGetMoter();
    (*p).stat.chopTemp1 = chopGetTemp1();
    (*p).stat.chopTemp2 = chopGetTemp2();
    (*p).stat.feK = feGetTemperatureK();
    (*p).stat.fePa = feGetPressurePa();

    (*p).stat.ifVrad = calc2GetVRAD();
    (*p).stat.if1PFreq = ifGetLo1PFreq();
    (*p).stat.if1PAmp = ifGetLo1PAmp();
    (*p).stat.if1PAtt = ifGetLo1PAtt();
    (*p).stat.if1RFreq = ifGetLo1RFreq();
    (*p).stat.if1RAmp = ifGetLo1RAmp();
    (*p).stat.if1RAtt = ifGetLo1RAtt();
    (*p).stat.if2PFreq = ifGetLo2PFreq();
    (*p).stat.if2PAmp = ifGetLo2PAmp();
    (*p).stat.if2RFreq = ifGetLo2RFreq();
    (*p).stat.if2RAmp = ifGetLo2RAmp();
    /* 081121 out
       ifGetVRAD(&(*p).stat.ifVrad);
       ifGetLo1PFreq(&(*p).stat.if1PFreq);
       ifGetLo1PAmp(&(*p).stat.if1PAmp);
       ifGetLo1PAtt(&(*p).stat.if1PAtt);
       ifGetLo1RFreq(&(*p).stat.if1RFreq);
       ifGetLo1RAmp(&(*p).stat.if1RAmp);
       ifGetLo1RAtt(&(*p).stat.if1RAtt);
       ifGetLo2PFreq(&(*p).stat.if2PFreq);
       ifGetLo2PAmp(&(*p).stat.if2PAmp);
       ifGetLo2RFreq(&(*p).stat.if2RFreq);
       ifGetLo2RAmp(&(*p).stat.if2RAmp);
    */
    //if((*p).thrdObsRun  == 0 && (*p).useSaacq){
    //if((*p).thrdObsRun == 0){
    //  (*p).stat.saacqState = beReqState();
    //  if((*p).stat.saacqState){
    //beGetAns(0, &(*p).stat.saacqExe, &(*p).stat.saacqErr, &(*p).stat.saacqTsys, &(*p).stat.saacqTotPow);
    //  }
    //}
    //uM("getAnsData(); get data");
    //uM1("getAnsData(); pParser.req->reqFlg=%d", pParser.req->reqFlg)
    size = headSize + sizeof((*p).stat);
    pParser.ans = (tAns*)new unsigned char[size];
    pParser.ans->size = size;
    pParser.ans->ansFlg = 0;
    memcpy(&(pParser.ans->data), &(*p).stat, sizeof((*p).stat));
  }
  else if(pParser.req->reqFlg > 0 && pParser.req->reqFlg < 6){//081208 in
    size = headSize + sizeof(pParser.ans->data);
    pParser.ans = (tAns*)new unsigned char[size];
    pParser.ans->size = size;
    pParser.ans->ansFlg = pParser.req->reqFlg;
    pParser.ans->data = (*p).reqRet;
  }
  /* 081208 out
     else if((*p).req->reqFlg==1){
     //! Initialize
     size = headSize + sizeof(pParser.ans->data);
     pParser.ans = (tAns*)new unsigned char[size];
     pParser.ans->size = size;
     pParser.ans->ansFlg = (*p).req->reqFlg;
     pParser.ans->data = (*p).reqRet;
     }
     else if((*p).req->reqFlg==2){
     //! RemoteControle
     size = headSize + sizeof(pParser.ans->data);
     pParser.ans = (tAns*)new unsigned char[size];
     pParser.ans->size = size;
     pParser.ans->ansFlg = (*p).req->reqFlg;
     pParser.ans->data = (*p).reqRet;
     }
     else if((*p).req->reqFlg==3){
     //! Strat Obs
     size = headSize + sizeof(pParser.ans->data);
     pParser.ans = (tAns*)new unsigned char[size];
     pParser.ans->size = size;
     pParser.ans->ansFlg = (*p).req->reqFlg;
     pParser.ans->data = (*p).reqRet;
     }
     else if((*p).req->reqFlg==4){
     //! Stop Obs
     size = headSize + sizeof(pParser.ans->data);
     pParser.ans = (tAns*)new unsigned char[size];
     pParser.ans->size = size;
     pParser.ans->ansFlg = (*p).req->reqFlg;
     pParser.ans->data = (*p).reqRet;
     }
     else if((*p).req->reqFlg==5){
     //! End
     size = headSize + sizeof(pParser.ans->data);
     pParser.ans = (tAns*)new unsigned char[size];
     pParser.ans->size = size;
     pParser.ans->ansFlg = (*p).req->reqFlg;
     pParser.ans->data = (*p).reqRet;
     }
  */
  pParser.ans->ansVer = TKB32_ANS_VER;
  *ans = (unsigned char*)pParser.ans;
  *ansSize = size;
  return 0;
}

/**
 * thread 1, phase A1
 */
void _parseTrackingRequest(int trkReq, tReqRemote* rem, tParamTkb32* p){
  int ret;

  if(trkReq & 1){
    //! AzEl
  }
  if(trkReq & 2){
    //! Pointing Offset
    //trkSetPof(rem->trkPof, rem->trkPofCoord);
    //! 取りあえずAZEL指定で。
    trkSetPof(rem->trkPof, 3);
  }
  if(trkReq & 4){
    //! Trcking Stop
    uM("Request Trcking Stop");
    trkStop();
  }
  if(trkReq & 8){
    //! Stow unlock
    uM("Request Stow unlock");
    if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090610 in
      /* 090610 out
         if((*p).useTrk == 1){
      */
      (*p).reqRet = acuSetAcuUnStow();
    }else{
      (*p).reqRet = 0;
    }
    
  }
  if(trkReq & 16){
    //! Stow lock
    uM("Request Stow lock");
    if((*p).thrdObsRun){
      uM("Reject  Already startobs");
      (*p).reqRet = 1;
    }
    else{
      if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090610 in
        /* 090610 out
           if((*p).useTrk == 1){
        */
        ret = acuSetAcuMood(3);
        if(ret){
          (*p).reqRet = 1;
        }
      }
    }
  }
  if(trkReq & 32){
    //! Progrum tracking mood
    uM("Request program tracking mood");
    if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090610 in
      /* 090610 out
         if((*p).useTrk == 1){
      */
      ret = acuSetAcuMood(2);
      if(ret){
        (*p).reqRet = 1;
      }
    }
  }
  if(trkReq & 64){
    //! ACU Standby
    uM("Request ACU Standby");
    if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090610 in
      /* 090610 out
         if((*p).useTrk == 1){
      */
      ret = acuSetAcuMood(0);
      if(ret){
        (*p).reqRet = 1;
      }
    }
  }
  if(trkReq & 128){
    //! Drive unlock
    uM("Request Drive unlock");
    if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090610 in
      /* 090610 out
         if((*p).useTrk == 1){
      */
      ret = acuSetAcuBan(0);
      if(ret){
        (*p).reqRet = 1;
      }
    }
  }
  if(trkReq & 256){
    //! Drive lock
    uM("Request Drive lock");
    //if((*p).thrdObsRun){
    //uM("Reject  Already startobs");
    //(*p).reqRet = 1;
    //}
    //else{
    if(controlBoardGetAntenna() == CONTROL_BOARD_USE_FULL){//090610 in
      /* 090610 out
         if((*p).useTrk == 1){
      */
      ret = acuSetAcuBan(1);
      if(ret){
        (*p).reqRet = 1;
      }
    }
    //}
  }
  //if(trkReq & 512){
  //!Tracking Start
  //uM("Request Tracking Start");
  //}
}

/**
 * thread 1, phase A1
 */
void _parseChopperRequest(int chopReq, tParamTkb32* p){
  int ret;
  if(chopReq == 1){
    //! close
    uM("Requset Chopper close");
    chopSetReq(2);       //!< 2:close
    ret = chopRepeat();
    (*p).stat.chopState = ret;
    if(ret){
      uM2("setReqData(); chopRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
      (*p).reqRet = ret;
    }
  }
  else if(chopReq == 2){
    //! open
    uM("Requset Chopper open");
    chopSetReq(3);       //!< 3:open
    ret = chopRepeat();
    (*p).stat.chopState = ret;
    if(ret){
      uM2("setReqData(); chopRepeat(); [%d(%s)] error!!", ret, errnoStr(ret));
      (*p).reqRet = ret;
    }
  }
  else if(chopReq == 3){
    //! origine
    uM("Request Chopper origine");
    chopSetReq(4);       //!< 4:origine
    ret = chopRepeat();
    (*p).stat.chopState = ret;
    if(ret){
      uM2("setReqData(); chopRepeat(); [%d(%s)]", ret, errnoStr(ret));
      (*p).reqRet = ret;	
    }
  }
  else if(chopReq == 4){
    //! stop
    uM("Request Chopper stop");
    chopSetReq(5);       //!< 5:stop
    ret = chopRepeat();
    (*p).stat.chopState = ret;
    if(ret){
      uM2("setReqData(); chopRepeat(); [%d(%s)]", ret, errnoStr(ret));
      (*p).reqRet = ret;
    }
  }
}

/**
 * thread 1, phase A1
 */
void _parseIfRequest(tReqRemote* rem){
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
}

/**
 * thread 1, phase A1
 */
int _parseSaacqRequest(int saacqReq, tReqRemote* rem, tParamTkb32* p){
  int ret;
  if(saacqReq & 1){
    //! Backend Set Integ Time
    if((*p).isInitialized == 0){
      uM("setReqData(); Not initialized");
      (*p).reqRet = 1;
      return -1;
    }
    //uM("Request Set SaAcq Integ");
  }
  if(saacqReq & 2){
    //! Start integration
    uM1("Request start integration OnOffR=%d", rem->saacqOnOffR);
    if((*p).isInitialized == 0){
      uM("setReqData(); Not initialized");
      (*p).reqRet = 1;
      return -1;
    }
    if((*p).thrdObsRun){
      uM("Reject  Already startobs");
      (*p).reqRet = 1;
    }
    else{
      ret = beReqState();
      if(ret){
        uM2("setReqData(); sacqReqState(); [%d(%s)] error!!", ret, errnoStr(ret));
        (*p).reqRet = 1;
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
        //  (*p).reqRet = 1;
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
          (*p).reqRet = ret;	  
        }
        //}
      }//!< if(beReqState())
    }//!< if((*p).thrdObsRun)
  }
  if(saacqReq & 4){
    //! Stop backend
    if((*p).isInitialized == 0){
      uM("setReqData(); Not initialized");
      (*p).reqRet = 1;
      return -1;
    }
    uM("Request Stop Backend");
    (*p).reqRet = beReqStop();
  }
  return 0;//normal end
}

/**
 * thread 1, phase C2
 */
void _obsEnd(){
  if(pParser.ans){
    delete[] pParser.ans;
    pParser.ans = NULL;
  }
  obsEnd();
}

