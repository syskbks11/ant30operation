/*!
\file tkb32clFunc.cpp
\date 2007.01.06
\author Y.Koide
\brief tkb32 Central Program Client Function
*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "../../libtkb/export/libtkb.h"
#include "tkb32clFunc.h"

typedef struct sParamTkb32cl{
  char ip[256];
  int port;

  void* net;
  tReq* req;
  tReq* reqRem;
  tReqRemote* rem;
  tAns* ans;
  tAnsState stat;
}tParamTkb32cl;

static tParamTkb32cl p;

static int tkb32clFuncReq(tReq* req);
static int tkb32clFuncGetAns(tAns** ans);
static int tkb32clFuncReqObs(tReq* req, int reqFlg, tAns** ans, char* data);
static int tkb32clFuncReqRemote(tReqRemote* rem, tAns** ans, char* data);
static void setParam();

int tkb32clFuncInit(){
  long size;

  uM2("Compile at %s %s", __DATE__, __TIME__);

  memset(&p, 0, sizeof(p));
  setParam();
  p.net = netclInit(p.ip, p.port);  
  if(!p.net){
    return -1;
  }
  size = sizeof(p.req->size) + sizeof(p.req->reqVer) + sizeof(p.req->reqFlg) + sizeof(p.req->data);
  p.req = (tReq*)new unsigned char[size];
  size = sizeof(p.req->size) + sizeof(p.req->reqVer) + sizeof(p.req->reqFlg) + sizeof(tReqRemote);
  p.reqRem = (tReq*)new unsigned char[size];
  p.rem = (tReqRemote*)new unsigned char[sizeof(tReqRemote)];
  return 0;
}

int tkb32clFuncEnd(){
  if(p.net){
    netclEnd(p.net);
    p.net = NULL;
  }
  if(p.req){
    delete[] p.req;
    p.req = NULL;
  }
  if(p.reqRem){
    delete[] p.reqRem;
    p.reqRem = NULL;
  }
  if(p.rem){
    delete[] p.rem;
    p.rem = NULL;
  }
  if(p.ans){
    delete[] p.ans;
    p.ans = NULL;
  }
  return 0;
}

int tkb32clFuncReqInit(){
  long pSize, size;
  int ret;
  tReq* req;
  char data;

  //!Request initialize
  pSize = strlen(confGetAllKeyVal())+1;
  size = sizeof(req->size) + sizeof(req->reqVer) + sizeof(req->reqFlg) + sizeof(long) + pSize;
  req = (tReq*)new unsigned char[size];
  memset(req, 0, size);
  req->size = size;
  req->reqVer = TKB32_REQ_VER;
  req->reqFlg = 1;//initialize
  ((tReqInit*)&(req->data))->paramSize = pSize;
  strcpy( &((tReqInit*) &(req->data))->param, confGetAllKeyVal());
  ret = tkb32clFuncReq(req);
  if(ret){
    uM1("tkb32clFuncReqInit(); tkb32clFuncReq(); error %d", ret);
    return ret;
  }

  //!Check
  ret = tkb32clFuncGetAns(&p.ans);
  if(ret){
    uM1("tkb32clFuncReqInit(); tkb32clFuncGetAns(); return %d", ret);
    return ret;
  }
  data = p.ans->data;
  if(data){
    uM1("init reject return=%d", (int)data);
  }
  else{
    uM("init accept");
  }
  delete[] req;
  return (int)data;
}

int tkb32clFuncReqStatus(){
  long size;
  int ret;
  tAnsState* s;

  //!Request status
  size = sizeof(p.req->size) + sizeof(p.req->reqVer) + sizeof(p.req->reqFlg) + sizeof(p.req->data);
  memset(p.req, 0, size);
  p.req->size = size;
  p.req->reqVer = TKB32_REQ_VER;
  p.req->reqFlg = 0;//Status
  ret = tkb32clFuncReq(p.req);
  if(ret){
    uM1("tkb32clFuncReqStatus(); tkb32clFuncReq(); return %d", ret);
    return ret;
  }

  //!Get status
  ret = tkb32clFuncGetAns(&p.ans);
  if(ret < 0){
    uM1("tkb32clFuncReqStatus(); tkb32clFuncGetAns(); return %d", ret);
    return ret;
  }
  memcpy(&p.stat, &p.ans->data, sizeof(tAnsState));
  s = &p.stat;
  char tmp[10240];
  sprintf(tmp,"status(); Status\nweath=%d trk=%d chop=%d fe=%d if1=%d if2=%d saacq=%d\n", s->weathState, s->trkState, s->chopState, s->feState, s->if1State, s->if2State, s->saacqState);
  sprintf(tmp + strlen(tmp), "Tamb=%6.2lf Pamb=%6.2lf Moi=%6.2lf WindD=%6.2lf WindAvg=%6.2lf WindMax=%6.2lf\n", s->weathData[0], s->weathData[1], s->weathData[2], s->weathData[3], s->weathData[4], s->weathData[5]);
  sprintf(tmp + strlen(tmp), "trkXY= %lf %lf\n", s->trkXY[0], s->trkXY[1]);
  sprintf(tmp + strlen(tmp), "trkP3AZEL= %lf %lf\n", s->trkP3AZEL[0], s->trkP3AZEL[1]);
  sprintf(tmp + strlen(tmp), "trkRAZEL= %lf %lf\n", s->trkRAZEL[0], s->trkRAZEL[1]);
  sprintf(tmp + strlen(tmp), "trkEAZEL= %lf %lf\n", s->trkEAZEL[0], s->trkEAZEL[1]);
  sprintf(tmp + strlen(tmp), "trkTracking %d\n", s->trkTracking);
  sprintf(tmp + strlen(tmp), "trkPof= %lf %lf\n", s->trkPof[0], s->trkPof[1]);
  sprintf(tmp + strlen(tmp), "chopStat=%d chopMoter=%d chopTemp1=%6.2lf chopTemp1=%6.2lf\n", s->chopStat, s->chopMoter, s->chopTemp1, s->chopTemp2);
  sprintf(tmp + strlen(tmp), "feK=%6.2lf fePa=%6.2e\n", s->feK, s->fePa);
  sprintf(tmp + strlen(tmp), "Cmd if1 Freq=%eHz Amp=%6.2lf Att=%6.2lf\n", s->if1PFreq, s->if1PAmp, s->if1PAtt);
  sprintf(tmp + strlen(tmp), "Cmd if2 Freq=%eHz Amp=%6.2lf\n", s->if2PFreq, s->if2PAmp);
  sprintf(tmp + strlen(tmp), "saExe=%d saErr=%d saTsys=%6.2e saTotPow=%6.2e\n", s->saacqExe, s->saacqErr, s->saacqTsys, s->saacqTotPow);
  //printf("%s", tmp);
  return 0;
}

/*! \fn tkb32clFuncReqPosoff(double* posoff)
\brief ポインティングオフセットの設定
\param[in] posoff ポインティングオフセット X Y [sec]
\return 0:Success other:失敗
*/
int tkb32clFuncReqPosoff(double* posoff){
  int ret;
  char data;

  //!Request Set Position Offset
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->trkReq = 2;//Position offset
  p.rem->trkPof[0] = posoff[0];
  p.rem->trkPof[1] = posoff[1];
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqTrkStop(){
  int ret;
  char data;

  //!Request Trcking stop
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->trkReq = 4;
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqUnStow(){
  int ret;
  char data;

  //!Request UnStow
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->trkReq = 8;//UnStow
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqStow(){
  int ret;
  char data;

  //!Request Stow
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->trkReq = 16;//Stow
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqProgramTracking(){
  int ret;
  char data;

  //!Request ProgramTracking
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->trkReq = 32;//Program tracking
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqACUStandby(){
  int ret;
  char data;

  //!Request ProgramTracking
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->trkReq = 64;//ACU Standby
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqDriveUnLock(){
  int ret;
  char data;

  //!Request Drive UnLock
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->trkReq = 128;//ACU Drive UnLock
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqDriveLock(){
  int ret;
  char data;

  //!Request Drive Lock
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->trkReq = 256;//ACU Drive Lock
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqTrkStart(){
  int ret;
  char data;

  //!Request Tracking Start
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->trkReq = 512;//Tracking Start
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqRclose(){
  int ret;
  char data;

  //!Request Set R close
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->chopReq = 1;//close
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqRopen(){
  int ret;
  char data;

  //!Request Set R open
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->chopReq = 2;//open
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqRorigine(){
  int ret;
  char data;
  
  //!Request Set R origine
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->chopReq = 3;//origine
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqRstop(){
  int ret;
  char data;

  //!Request Set R stop
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->chopReq = 4;//stop
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqSetLo1Freq(double freq){
  int ret;
  char data;

  //!Request Set Lo1 Freq
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->if1Req |= 1;//SetFreq
  p.rem->if1Freq = freq;
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqSetLo1Amp(double amp){
  int ret;
  char data;

  //!Request Set Lo1 Amp
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->if1Req |= 2;//SetAmp
  p.rem->if1Amp = amp;
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqSetLo1Att(double att){
  int ret;
  char data;

  //!Request Set Lo1 Att
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->if1Req |= 4;//SetAtt
  p.rem->if1Att = att;
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqSetLo2Freq(double freq){
  int ret;
  char data;

  //!Request Set Lo2 Freq
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->if2Req |= 1;//SetFreq
  p.rem->if2Freq = freq;
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqSetLo2Amp(double amp){
  int ret;
  char data;

  //!Request Set Lo2 Amp
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->if2Req |= 2;//SetAmp
  p.rem->if2Amp = amp;
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqSetSaIntegTime(double time){
  int ret;
  char data;

  //!Request Set Sa integ time
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->saacqReq |= 1;//Set integ time
  //p.rem->saacqInteg = time;
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqSaStart(int OnOffR){
  int ret;
  char data;
  if(OnOffR < 0 || OnOffR > 2){
    return -101;
  }
  //!Request Saacq Start
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->saacqReq |= 2;//Start integ
  p.rem->saacqOnOffR = OnOffR;
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqSaStop(){
  int ret;
  char data;

  //!Request Saacq Stop
  memset(p.rem, 0, sizeof(tReqRemote));
  p.rem->saacqReq |= 4;//Stop integ
  ret = tkb32clFuncReqRemote(p.rem, &p.ans, &data);
  return ret;
}

int tkb32clFuncReqStartObs(){
  int ret;
  char data;

  //!Request start observation
  ret = tkb32clFuncReqObs(p.req, 3, &p.ans, &data);//3:Start obs
  if(ret){
    uM1("tkb32clFuncReqStartObs(); tkb32clFuncReqObs(); error %d", ret);
  }
  return ret;
}

int tkb32clFuncReqStopObs(){
  int ret;
  char data;

  //!Request stop obs
  ret = tkb32clFuncReqObs(p.req, 4, &p.ans, &data);//4:Stop obs
  if(ret){
    uM1("tkb32clFuncReqStopObs(); tkb32clFuncReqObs(); error %d", ret);
  }
  return ret;
}

int tkb32clFuncReqEnd(){
  int ret;
  char data;

  //!Request end
  ret = tkb32clFuncReqObs(p.req, 5, &p.ans, &data);//5:End obs
  return ret;
}


const tAnsState* tkb32clFuncGetAnsData(){
  return (const tAnsState*)&p.stat;
}

int tkb32clFuncReq(tReq* req){
  int ret;
  
  //!Request
  req->reqVer = TKB32_REQ_VER;
  ret = netclWrite(p.net, (const unsigned char*)req, req->size);
  if(ret <= 0){
    uM1("tkb32clFuncReq(); netclWrite(); error %d", ret);
    return -101;
  }
  return 0;
}

int tkb32clFuncGetAns(tAns** ans){
  long ansSize;
  long size;
  int ret;
  unsigned char* buf;

  //!Get Answer
  ret = netclRead(p.net, (unsigned char*)&ansSize, sizeof(long));
  size = sizeof(long);
  if(ansSize <= 0){
    return -1;
  }
  buf = new unsigned char[ansSize];
  memset(buf, 0, ansSize);
  *((long*)buf) = ansSize;
  for(int i=0; size < ansSize && i < 10; i++){
    ret = netclRead(p.net, buf + size, ansSize - size);
    if(ret < 0){
      uM1("tkb32clFuncGetAns(); netclRead(); error %d", ret);
      delete[] buf;
      return -101;
    }
    size += ret;
    tmSleepMSec(1);//10msec待ってデータを更新する
  }
  if(size != ansSize){
    uM2("tkb32clFuncGetAns(); buf size error size=%ld ansSize=%ld", size, ansSize);
    delete[] buf;
    return -101;
  }

  int ver = ((tAns*)buf)->ansVer;
  if(ver != TKB32_ANS_VER){
    uM2("tkb32clFuncGetAns(); ansVer(%d) != TKB32_ANS_VER(%d) error!!", ver, TKB32_ANS_VER);
    delete[] buf;
    return -101;
  }
  
  if(*ans){
    delete[] *ans;
    *ans = NULL;
  }
  *ans = (tAns*)buf;

  return 0;
}

int tkb32clFuncReqObs(tReq* req, int reqFlg, tAns** ans, char* data){
  long size;
  int ret;

  size = sizeof(p.req->size) + sizeof(p.req->reqVer) + sizeof(p.req->reqFlg) + sizeof(p.req->data);
  memset(req, 0, size);
  req->size = size;
  req->reqFlg = reqFlg;
  ret = tkb32clFuncReq(req);
  if(ret){
    uM2("tkb32clFuncReqObs(); tkb32clFuncReq(); reqFlg = %d ret = %d", reqFlg, ret);
    return ret;
  }

  ret = tkb32clFuncGetAns(ans);
  if(ret){
    uM1("tkb32clFuncReqObs(); tkb32clFuncGetAns(); return %d", ret);
    return ret;
  }
  *data = (*ans)->data;
  if(*data){
    uM2("tkb32clFuncReqObs(); ansFlg = %d ret = %d reject!!", (*ans)->ansFlg, *data);
    return (int)(*data);
  }
  else{
    uM1("tkb32clFuncReqObs(); ansFlg = %d accept", (*ans)->ansFlg);
  }
  return 0;
}

int tkb32clFuncReqRemote(tReqRemote* rem, tAns** ans, char* data){
  long size;
  int ret;

  size = sizeof(p.req->size) + sizeof(p.req->reqVer) + sizeof(p.req->reqFlg) + sizeof(tReqRemote);
  memset(p.reqRem, 0, size);
  p.reqRem->size = size;
  p.reqRem->reqFlg = 2;//Remote
  memcpy(&p.reqRem->data, rem, sizeof(tReqRemote));
  ret = tkb32clFuncReq(p.reqRem);
  if(ret){
    uM1("tkb32clFuncReqRemote(); tkb32clFuncReq(); ret = %d", ret);
    return ret;
  }

  ret = tkb32clFuncGetAns(ans);
  if(ret){
    uM1("tkb32clFuncReqRemote(); tkb32clFuncGetAns(); return %d", ret);
    return ret;
  }
  *data = (*ans)->data;
  if(*data){
    uM2("tkb32clFuncReqRemote(); ansFlg = %d ret = %d reject!!", (*ans)->ansFlg, *data);
    return (int)(*data);
  }
  else{
    uM1("tkb32clFuncReqRemote(); ansFlg = %d accept", (*ans)->ansFlg);
  }
  return 0;
}

void setParam(){
  if(confSetKey("tkb32ip"))
    strcpy(p.ip, confGetVal());
  if(confSetKey("tkb32port"))
    p.port = atoi(confGetVal());
  return;
}
