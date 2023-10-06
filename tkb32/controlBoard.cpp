/*!
¥file controlBoard.cpp
¥author NAGAI Makoto
¥date 2009.6.10
¥brief For Tsukuba 32-m telescope & Antarctic 30-cm telescope 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>//usleep()

#include "configuration.h"
#include "controlBoard.h"


typedef struct sParamControlBoard{
	//! 各制御の起動フラグ 0:起動無し, 1:起動, 2:起動(通信部以外)
	int useWeath;
	int useTrk;
	int useChop;
	int useFe;
	int useIf1;
	int useIf2;
	int useBe;
}tParamControlBoard;

static tParamControlBoard p;


int controlBoardInit(){
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


  if(p.useWeath < 0 || p.useWeath > 2){
    uM1("checkParam(); ERROR: invalid value of useWeath[%d]!!", p.useWeath);
    return -1;
  }
  if(p.useTrk < 0 || p.useTrk > 2){
    uM1("checkParam(); ERROR: invalid value of useTrk[%d]!!", p.useTrk);
    return -1;
  }
  if(p.useChop < 0 || p.useChop > 2){
    uM1("checkParam(); ERROR: invalid value of useChop[%d]!!", p.useChop);
    return -1;
  }
  if(p.useFe < 0 || p.useFe > 2){
    uM1("checkParam(); ERROR: invalid value of useFe[%d]!!", p.useFe);
    return -1;
  }
  if(p.useIf1 < 0 || p.useIf1 > 2){
    uM1("checkParam(); ERROR: invalid value of useIf1[%d]!!", p.useIf1);
    return -1;
  }
  if(p.useIf2 < 0 || p.useIf2 > 2){
    uM1("checkParam(); ERROR: invalid value of useIf2[%d]!!", p.useIf2);
    return -1;
  }

	if(confSetKey("BeUse"))
		p.useBe = atoi(confGetVal());
	if(p.useBe < 0 || p.useBe > 2){
		uM1("checkParam(); ERROR: invalid value of useBe[%d]!!", p.useBe);
		return -1;
	}
//	p.useBe = 1;//for test


	return 0;//normal end
}








int controlBoardGetWeather(){
	return p.useWeath;
}
int controlBoardGetAntenna(){
	return p.useTrk;
}
int controlBoardGetChopper(){
	return p.useChop;
}
int controlBoardGetFrontEnd(){
	return p.useFe;
}
int controlBoardGet1stIF(){
	return p.useIf1;
}
int controlBoardGet2ndIF(){
	return p.useIf2;
}
int controlBoardGetBackEnd(){
	return p.useBe;
}



