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
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include "../libtkb/src/libtkb.h"
#include "penguin_chopper.h"

#define DEV_NAME "/dev/tty.usbserial-FT2GX65L0"

const int srvPort = 10002;
static netServerClass_t* net = NULL;


//! 強度較正装置からの応答構造体
typedef struct aAnsChop{
  int ansState;      //!< 状態 0:none, 1:close, 2:open(origin), 3:No Signal
  int ansMotor;      //!<  モータの状態 0:none, 1:closeへ回転中, 2:open, 3:originへ回転中, 4:stop
  double ansTemp1;   //!< 温度計Ch1  -300:Error, other(>-273):℃
  double ansTemp2;   //!< 温度計Ch2(Chopper)  -300:Error, other(>-273):℃
  int ansInterlock;  //!< インターロック 0:OFF, 1:ON
  int ansError;      //!< エラーデータ(予備) 0:No Error
}tAnsChop;

static int latestMovement = 0;//0:no movement, 1: close, 2: open

void manageRequest(int req){//制御コマンド 0:none, 1:status, 2:close, 3:open, 4:origin, 5:stop
  switch(req){
  case 0:
    break;
  case 1:
    uM1("pulse=%d", penguin_chopper_getPulse());
    uM1("status=%c", penguin_chopper_getStatus());
    uM1("sensor=%d", penguin_chopper_getSensorStatus());
    break;
  case 2:
    latestMovement = 1;
    penguin_chopper_close();
    break;
  case 3:
    latestMovement = 2;
    penguin_chopper_open();
    break;
  case 4:
    latestMovement = 2;
    penguin_chopper_open();
    break;
  case 5:
    penguin_chopper_stop();
    break;
  default:
    uM1("WARNING invalid request (%d)\n", req);
    break;
  }
}

void makeAnswer(tAnsChop* ans){
  ans->ansState = latestMovement;
  if(penguin_chopper_isMoving()){
    if(latestMovement == 1){
      ans->ansMotor = 1;
    }else{
      ans->ansMotor = 3;
    }
  }else{
    if(latestMovement == 1){
      if(penguin_chopper_isAtClose()){
        //I think this should be a value for close, but tkb32Func.cpp do not work.
        ans->ansMotor = 4;
      }else{
        ans->ansMotor = 4;
      }
    }else{
      if(penguin_chopper_isAtOpen()){
        //I think this should be 2, but tkb32Func.cpp do not work.
        ans->ansMotor = 4;
      }else{
        ans->ansMotor = 4;
      }
    }
  }
}

int main(int argc, char* argv[]){
  int req[1];          //!<
  tAnsChop ans;          //!< LoFreq[GHz], LoAmp[dBm], StepAtt[dB]

  int ret;

  char *devName;
  if(argc == 2){
    devName = argv[1];
  }else{
    devName = DEV_NAME;
  }

  memset(&ans, 0, sizeof(tAnsChop));

  uInit("../log/srvChopper/srvChopper");//090813 in
  uM2("This program is Compiled at %s %s", __DATE__, __TIME__);

  ret = penguin_chopper_init(devName);

  if(ret){
    uM1("main(); penguin_chopper_init(); failed (%d)", ret);
    return -1;
  }
  if(penguin_chopper_configure_settings()){
    uM("main(); penguin_chopper_configure_settings(); failed.");
    return -1;
  }
  if(penguin_chopper_configure_origin()){
    exit(1);
  }

  net = netsvInit(srvPort);
  if(net==NULL)
    return -1;
  uEnd();
  while(1){
    if(netsvWaiting(net) < 0){
      continue;
    }
    uInit("../log/srvChopper/srvChopper");//090813 in
    while(1){
      memset(req, 0, sizeof(int)*1);
      ret = netsvRead(net, (unsigned char*)req, sizeof(int)*1);
      if(ret <= 0){
        uM1("main(); netsvRead(); return %d",ret);
        break;
      }
      netuCnvInt((unsigned char*)req);
      uM1("main(); req %d", req[0]);

      manageRequest(req[0]);

      makeAnswer(&ans);

      uM2("main(); ans %d, %d", ans.ansState, ans.ansMotor);

      netuCnvInt((unsigned char*)&ans.ansState);			netuCnvInt((unsigned char*)&ans.ansMotor);
      ret = netsvWrite(net, (const unsigned char*)(&ans), sizeof(tAnsChop));
      if(ret <= 0){
        uM1("main(); netsvWrite(); return %d", ret);
        break;
      }
    }
    netsvDisconnect(net);
    uEnd();
  }
  penguin_chopper_end();
  netsvEnd(net);
}
