/*!
¥file tkb32clPort.cpp
¥date 2009.06.16
¥author NAGAI Makoto
¥brief Adopted tkb32clFunc.cpp by  Y.Koide
¥brief tkb32/ant30 Central Program Client Port
*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "../libtkb/src/libtkb.h"
#include "tkb32clPort.h"

typedef struct sParamTkb32clPort{
/* 090616 out
  char ip[256];
  int port;
*/
  netClass_t* net;
}tParamTkb32clPort;

static tParamTkb32clPort p;

static void _netInit();
/* 090616 out
static void setParam();
*/

int tkb32clPortInit(){
  memset(&p, 0, sizeof(p));
	_netInit();//090616 in
/* 090616 out
  setParam();
  p.net = netclInit(p.ip, p.port);
*/
  if(!p.net){//This condition seems to be never saticefied, even if netclInit() failed.
    return -1;
  }
  return 0;
}

int tkb32clPortEnd(){
  if(p.net){
    netclEnd(p.net);
    p.net = NULL;
  }
  return 0;
}

int tkb32clPortReq(tReq* req){
  int ret;
  
  //!Request
  req->reqVer = TKB32_REQ_VER;
  ret = netclWrite(p.net, (const unsigned char*)req, req->size);
  if(ret <= 0){
    uM1("tkb32clPortReq(); netclWrite(); error %d", ret);
    return -101;
  }
  return 0;
}


int tkb32clPortGetAns(tAns** ans){
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
      uM1("tkb32clPortGetAns(); netclRead(); error %d", ret);
      delete[] buf;
      return -101;
    }
    size += ret;
    tmSleepMSec(1);//10msec待ってデータを更新する
  }
  if(size != ansSize){
    uM2("tkb32clPortGetAns(); buf size error size=%ld ansSize=%ld", size, ansSize);
    delete[] buf;
    return -101;
  }

  int ver = ((tAns*)buf)->ansVer;
  if(ver != TKB32_ANS_VER){
    uM2("tkb32clPortGetAns(); ansVer(%d) != TKB32_ANS_VER(%d) error!!", ver, TKB32_ANS_VER);
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

void _netInit(){
/*
void setParam(){
*/
	char ip[256];
	int port;
  if(confSetKey("tkb32ip"))
    strcpy(ip, confGetVal());
  if(confSetKey("tkb32port"))
    port = atoi(confGetVal());

	p.net = netclInit(ip, port);
	uM("?");
  return;
}
