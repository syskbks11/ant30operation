/*!
¥file fe.cpp
¥date 2006.12.31
¥author Y.Koide
¥brief フロントエンド制御プログラム
*/
#include <stdlib.h>
#include <memory.h>
#include "configuration.h"
#include "errno.h"
#include "fe.h"
#include "controlBoard.h"

//! フロントエンドのリクエスト構造体
typedef struct sReqFe{
  long size;   //!< 送信サイズ
  int reqFlg;  //!< リクエストフラグ 1:Status
}tReqFe;

//! フロントエンドの応答構造体
typedef struct sAnsFe{
  long size;   //!< 受信サイズ
  int tmp;     //!< 予備(ダミー)
  double K;    //!< フロントエンド温度 [K]
  double Pa;   //!< フロントエンド気圧 [Pa]
}tAnsFe;

typedef struct sParamFe{
/* 090611 out, since we use controlBoard.cpp
  int FeUse;
*/
  char FeIp[256];
  int FePort;
  tReqFe req;
  tAnsFe ans;
	netClass_t* net;
//  void* net;
}tParamFe;

static int init = 0;
static tParamFe p;

static int _feNet();
static void setParam();
static int checkParam();

/*! ¥fn int feInit()
¥brief 初期化
¥retval 0 成功
¥retval 0以外 エラーコード
* thread 1, phase A0
*/
int feInit(){

  if(init){
    feEnd();
    init=0;
  }
  memset(&p,0,sizeof(p));
  setParam();
  if(checkParam()){
    uM("feInit(); checkParam(); error");
    return FE_PARAM_ERR;
  }

  init = 1;
	if(controlBoardGetFrontEnd() == CONTROL_BOARD_USE_NO){//090611 in
/* 090611 out
  if(p.FeUse==0){
*/
		return FE_NOT_USE;
	}
	if(controlBoardGetFrontEnd() == CONTROL_BOARD_USE_LOGIC){//090611 in
/* 090611 out
	if(p.FeUse == 2){//081119 in
*/
		return 0;//do not connect
	}
/* 090611 out
	if(p.FeUse != 1){//never comes here
		uM1("feInit(); Fatal Error, FeUse: %d", p.FeUse);
	}
	p.net=netclInit(p.FeIp, p.FePort);
*/
/* 081219 out
	p.net=netclInitWithTimeOut(p.FeIp, p.FePort, 0);
*/
	if(p.net == NULL){
		uM("feInit(); netclInit(); error");
		return FE_NET_ERR;
	}
/* 081119 out
  if(p.FeUse==1){
    p.net=netclInitWithTimeOut(p.FeIp, p.FePort, 0);
    if(p.net == NULL){
      uM("feInit(); netclInit(); error");
      return FE_NET_ERR;
    }
  }
*/
  return 0;
}

/*! ¥fn int feEnd()
¥brief 終了時に呼び出す
¥retval 0 成功
* thread 1, phase A2
*/
int feEnd(){
	if(!init)
		return 0;

	if(p.net){
		netclEnd(p.net);
		p.net = NULL;
	}
	init = 0;
/* 081119 out
  if(init == 1){
    if(p.net){
      netclEnd(p.net);
      p.net = NULL;
    }
    init = 0;
  }
*/
  return 0;
}

/*! ¥fn int feRepeat()
¥brief フロントエンドのステータスを更新する
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int feRepeat(){
  int ret = 0;

  if(init == 0){
    return FE_NOT_INIT_ERR;
  }
  
	if(controlBoardGetFrontEnd() == CONTROL_BOARD_USE_NO){//090611 in
/* 090611 out
  if(p.FeUse==0)
*/
		return FE_NOT_USE;
	}

	if(controlBoardGetFrontEnd() == CONTROL_BOARD_USE_LOGIC){//090611 in
/* 090611 out
	if(p.FeUse==2)//081119 in
*/
		return 0;
	}
	ret = _feNet();
/* 081119 out
  if(p.FeUse==1){
    ret = feNet();
    //uM2("feRepeat(); anser K=%lf Pa=%e", p.ans.K, p.ans.Pa);
  }
*/
  return ret;
}

/*! ¥fn double feGetK()
¥return フロントエンドの温度[K]
*/
double feGetTemperatureK(){
  return p.ans.K;
}

/*! ¥fn double feGetPa()
¥return フロントエンドの気圧[Pa]
*/
double feGetPressurePa(){
  return p.ans.Pa;
}

/*! ¥fn int _feNet()
¥brief フロントエンドと通信する
¥retval 0 成功
¥retval 0以外 エラーコード
*/
int _feNet(){
  int ret;
  const int bufSize = 256;
  unsigned char buf[bufSize] = {0};
  int ansSize;
  int size;

  if(init == 0){
    return FE_NOT_INIT_ERR;
  }

  //! Request
  p.req.reqFlg = 1; //!< 1:Status
  p.req.size=sizeof(p.req);
  //uM2("debug feNet(); reqFlg[%d] size[%d]", p.req.reqFlg, p.req.size);
  ret = netclWrite(p.net, (const unsigned char*)&p.req, p.req.size);
  if(ret != p.req.size){
    //uM1("feNet(); netclWrite(); send size error = %d", ret);
    return FE_NET_DAT_ERR;
  }

  //! Get Answer
  //uM1("debug feNet(); ansSize[%d]", sizeof(p.ans));
  ansSize = sizeof(p.ans);
  for(int i = 0; i < 5; i++){
    ret = netclRead(p.net, (unsigned char*)buf + size, ansSize - size);
    if(ret < 0){
      //uM1("feNet(); netclRead(); [%d] error!!", ret);
      return FE_NET_ERR;
    }
    size += ret;
    if(size == ansSize){
      break;
    }
    tmSleepMSec(50);
  }
  if(size != ansSize || ((tAnsFe*)buf)->size != ansSize){
    //uM1("feNet(); Net Dat error!!");
    return FE_NET_DAT_ERR;
  }
  memcpy(&p.ans, buf, ansSize);
  return 0;
}

/*! ¥fn void setParam()
¥brief パラメーター情報を取得する
* thread 1, phase A0
*/
void setParam(){
/* 090611 out
  if(confSetKey("FeUse"))
    p.FeUse = atoi(confGetVal());
*/
  if(confSetKey("FeIp"))
    strcpy(p.FeIp, confGetVal());
  if(confSetKey("FePort"))
    p.FePort = atoi(confGetVal());
}

/*! ¥fn int checkParam()
¥brief パラメーターのチェック
¥retval 0 成功
¥retval -1 失敗
* thread 1, phase A0
*/
int checkParam(){
/* 090611 out
  if(p.FeUse < 0 || p.FeUse > 2){
    uM1("FeUse(%d) error!!¥n", p.FeUse);
    return -1;
  }
*/
	if(controlBoardGetFrontEnd() == CONTROL_BOARD_USE_FULL){//090611 in
/* 090611 out
  if(p.FeUse == 1){
*/
	if(!confSetKey("FeIp")){
		uM("checkParam(); ERROR: There is no FeIp key in conf.");//090611 in
/* 090611 out
      uM1("FeUse(%d) feIp() error!!¥n", p.FeUse); 
*/
		return -1;
	}
	if(p.FePort <= 0){
		uM1("checkParam(); ERROR: Invalid FePort value (%d).", p.FePort);//090611 in
/* 090611 out
      uM1("FePort(%d) error!!¥n", p.FePort);
*/
		return -1;
	}
  }
  return 0;
}