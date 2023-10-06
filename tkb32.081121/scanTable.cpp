/*!
¥file scanTable.cpp
¥author NAGAI Makoto
¥date 2008.11.15
¥brief Manages the scan table for 32-m telescope & 30-cm telescope
*/

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>



#include "configuration.h"
#include "scanTable.h"

#define PI M_PI


typedef struct {//values in scan table
  int OnCoord;          //!< On点の座標定義 1:RADEC 2:LB 3:AZEL
  int OffCoord;         //!< Off点の座標定義 1:RADEC 2:LB 3:AZEL
  int OffNumber;     //!< Off点を2点使うか 1:NoUse 2:Use
  int ScanFlag;         //!< スキャンの定義 1:OnOff 2:5Points 3:Grid 4:Random 5:9Point 6:Raster 1001:SourceTracking 1002:Lee Tracking
  
  //int OffInterval;      //!< One-Point、グリッド、ランダムマッピングでOn点何回に一回Off点を入れるか 081117 out since never used
  int OffMode;          //!< 1:OffsetFromCenter 2:AbsoluteValue
  char* SetPattern;     //!< マッピングの観測順序
  int OnNumber;         //!< マッピングの要素の数
  double posAngleRad;  //!< ポジションアングル [rad]
  double LineTime;      //!< ラスターのアプローチ時間を考慮した1Lineの時間 [sec]
  //double LinePathX_Rad;  //!< ラスターのアプローチ時間を考慮した1Lineの距離X [rad]
  //double LinePathY_Rad; //!< ラスターのアプローチ時間を考慮した1Lineの距離Y [rad]
}scanTable_t;

typedef struct {//calculated values from scan table
  double* off;          //!< Off点の位置 offX1 offY1 offX2 offY2 ... [sec]
  double* on;           //!< マッピングの際のOn点 onX1 onY1 onX2 onY2 ... [sec]
  double* linePath;     //!< ラスターの際のonからのパス
  int* mapPattern;      //!< マッピングの観測順序 0:R -:Off +:On 
  int mapNum;           //!< マッピングの点数
}scanSequence_t;


const double rad2sec = 180.0 * 3600.0 / PI;


static scanTable_t scan;
static scanSequence_t seq;

int scanTableInit(){
	//0 fill
	scan.OnCoord = 0;
	scan.OffCoord = 0;
	scan.OffNumber = 0;
	scan.ScanFlag = 0;
	//scan.OffInterval = 0;
	scan.OffMode = 0;
	if(scan.SetPattern){
		free(scan.SetPattern);
		scan.SetPattern = NULL;
	}
	scan.OnNumber = 0;
	scan.posAngleRad = 0;
	scan.LineTime = 0;

	if(seq.mapPattern){
		free(seq.mapPattern);
		seq.mapPattern = NULL;
	}
	if(seq.off){
		free(seq.off);
		seq.off = NULL;
	}
	if(seq.on){
		free(seq.on);
		seq.on = NULL;
	}
	if(seq.linePath){
		free(seq.linePath);
		seq.linePath = NULL;
	}


	//load the configuration file
  //! On-Point
  if(confSetKey("ScanFlag"))
    scan.ScanFlag = atoi(confGetVal());
  if(confSetKey("OnCoord"))
    scan.OnCoord = atoi(confGetVal());
  if(confSetKey("PosAngle_Rad"))
    scan.posAngleRad = atof(confGetVal());
  if(confSetKey("LineTime"))
    scan.LineTime = atof(confGetVal());
  //if(confSetKey("LinePathX_Rad"))
  //  p.LinePathX_Rad = atof(confGetVal());
  //if(confSetKey("LinePathY_Rad"))
  //  p.LinePathY_Rad = atof(confGetVal());

  if(confSetKey("SeqPattern")){ //!< 互換性のため残しておく
    if(scan.SetPattern){
      free(scan.SetPattern);
    }
    scan.SetPattern = (char*) malloc(strlen(confGetVal()) + 1);
    strcpy(scan.SetPattern, confGetVal());
  }
  if(confSetKey("SetPattern")){
    if(scan.SetPattern){
      free(scan.SetPattern);
    }
    scan.SetPattern = (char*) malloc(strlen(confGetVal()) + 1);
    strcpy(scan.SetPattern, confGetVal());
  }

  int i;
  char tmp[16];

  if(confSetKey("OnNumber")){
    scan.OnNumber = atoi(confGetVal());
    if(scan.OnNumber < 0){
      uM1("trk setParam(); invalid  scan.OnNumber[%d] < 0 ans set 0", scan.OnNumber);
      scan.OnNumber = 0;
    }
    else{
      if(seq.on){
	free(seq.on);
      }
      if(seq.linePath){
	free(seq.linePath);
      }
      seq.on = (double*)malloc(sizeof(seq.on[0]) * scan.OnNumber * 2);
      seq.linePath = (double*)malloc(sizeof(seq.linePath[0]) * scan.OnNumber * 2);
      memset(seq.on, 0, sizeof(seq.on[0]) * scan.OnNumber * 2);
      memset(seq.linePath, 0, sizeof(seq.linePath[0]) * scan.OnNumber * 2);
      for(i = 0; i < scan.OnNumber; i++){
	sprintf(tmp, "OnX%d_Rad", i+1);
	if(confSetKey(tmp)){
	  seq.on[i*2] = atof(confGetVal()) * rad2sec;
	}
	sprintf(tmp, "OnY%d_Rad", i+1);
	if(confSetKey(tmp)){
	  seq.on[i*2+1] = atof(confGetVal()) * rad2sec;
	}
	sprintf(tmp, "LinePathX%d_Rad", i+1);
	if(confSetKey(tmp)){
	  seq.linePath[i*2] = atof(confGetVal());
	}
	sprintf(tmp, "LinePathY%d_Rad", i+1);
	if(confSetKey(tmp)){
	  seq.linePath[i*2+1] = atof(confGetVal());
	}
      }
    }
  }

  //! Off-Point
  if(confSetKey("OffCoord"))
    scan.OffCoord = atoi(confGetVal());
  if(confSetKey("OffNumber")){
    scan.OffNumber = atoi(confGetVal());
    if(scan.OffNumber < 0){
      uM1("trk setParam(); invalid scan.OffNumber[%d] < 0 ans set 0", scan.OffNumber);
      scan.OffNumber = 0;
    }
    else{
      if(seq.off){
	free(seq.off);
      }
      seq.off = (double*)malloc(sizeof(seq.off[0]) * scan.OffNumber * 2);
      memset(seq.off, 0, sizeof(seq.off[0]) * scan.OffNumber * 2);
      for(i = 0; i < scan.OffNumber; i++){
	sprintf(tmp, "OffX%d_Rad", i+1);
	if(confSetKey(tmp)){
	  seq.off[i*2] = atof(confGetVal()) * rad2sec;
	}
	sprintf(tmp, "OffY%d_Rad", i+1);
	if(confSetKey(tmp)){
	  seq.off[i*2+1] = atof(confGetVal()) * rad2sec;
	}
      }
    }
  }
	if(confSetKey("OffInterval")){
		uM("CAUTION! OffInterval is not supported. ")
		//scan.OffInterval = atoi(confGetVal());//081117 out
	}
  if(confSetKey("OffMode"))
    scan.OffMode = atoi(confGetVal());



	//check the parameters
    if(!((scan.ScanFlag >= 1 && scan.ScanFlag <= 6)
	 || (scan.ScanFlag >= 1001 && scan.ScanFlag <= 1002))){
      uM1("ScanFlag(%d) error!!", scan.ScanFlag);
      return -1;
    }

    if(scan.ScanFlag >= 1 && scan.ScanFlag <= 6){
      if(scan.OffNumber < 1 && scan.OffNumber > 2){
	uM1("checkParam(); OffNumber %d error!!.", scan.OffNumber);
	return -1;
      }
/* 081117 out since OffInterval is not supported. 
      if(scan.ScanFlag == 1 && scan.ScanFlag == 3 && scan.ScanFlag == 4){
	//! On,Grid,Random
	if(scan.OffInterval <= 0){
	  uM1("checkParam(); OffInterval[%d] error!!", scan.OffInterval);
	  return -1;
	}
      }
*/
      if(!scan.SetPattern){
	uM1("ScanFlag(%d) SetPattern error!!", scan.ScanFlag);
	return -1;
      }
      if(scan.OnNumber <= 0){
	uM2("ScanFlag(%d) OnNumber(%d) error!!", scan.ScanFlag, scan.OnNumber);
	return -1;
      }
      if(scan.OffCoord < 1 || scan.OffCoord > 3){
	uM1("OffCoord(%d) error!!", scan.OffCoord);
	return -1;
      }
    }
    else if(scan.ScanFlag == 1001){
      //! Source Tracking
    }
    if(scan.LineTime < 0){
      uM1("LineTime(%lf) error!!", scan.LineTime);
      return -1;
    }


  //! Initialize map pattern
  if(scan.ScanFlag >= 0 && scan.ScanFlag <= 6){
    //! On,5Point,Grid,Random,9Point,Raster
    int len = strlen(scan.SetPattern);
	uM1("trkInitParam(); scan.SetPattern length=%d", len)
    int i, j, k;
    
    uM1("trkInitParam(); debug SetPattern %s", scan.SetPattern);
    for(i = 0, j = 0, k = 0; i < len; i++){
      if(scan.SetPattern[i] == ' ' || scan.SetPattern[i] == '\t'){
	continue;
      }
      else if((scan.SetPattern[i] >= 'A' && scan.SetPattern[i] <= 'Z')
	      || (scan.SetPattern[i] >= '0' && scan.SetPattern[i] <= '9')){
	if(j == 0){
	  k++;
	}
	j++;
      }
      else if(scan.SetPattern[i] == ','){
	j = 0;
      }
    }

    //! マップパターンの領域を確保する。
    seq.mapNum = k;
    uM1("trkInitParam(); debug seq.mapNum=%d", seq.mapNum);
    //if(k != scan.OnNumber){
    //  uM3("trkStart(); SetPattern Num[%d] != OnNumber[%d] and use first [%d] pieces", k, scan.OnNumber, seq.mapNum);
    //}
    seq.mapPattern = (int*) malloc(sizeof(seq.mapPattern[0]) * seq.mapNum);
    memset(seq.mapPattern, 0, sizeof(seq.mapPattern[0]) * seq.mapNum);
    //! マップパターンのID記録 0:R +:On -:Off
    for(i = 0, k = 0; k < seq.mapNum;){
      while((scan.SetPattern[i] == ' ' || scan.SetPattern[i] == '\t') && scan.SetPattern[i] != '\0'){
	i++;
      }
      
      if(scan.SetPattern[i] == 'R'){
		//! R
		seq.mapPattern[k] = 0;
		k++;
      }
      else if(scan.SetPattern[i] >= 'A' && scan.SetPattern[i] <= 'Z'){//081114 in
//      else if(scan.SetPattern[i] >= 'A' && scan.SetPattern[0] <= 'Z'){//081114 out
		//! Off-Point
		seq.mapPattern[k] = -(scan.SetPattern[i] - 'A' + 1);
		//! 不正データの場合は無視
		if(seq.mapPattern[k] < - scan.OffNumber){
			uM3("trkInitParam(); invalid SetPattern[%d]=%c(%d): too much off points!", i, scan.SetPattern[i], seq.mapPattern[k]);
//		  char tmp = - seq.mapPattern[k] - 1 + 'A';
//		  uM2("trkInitParam(); invalid SetPattern[%d(%c)]: too much off points!", i, tmp);
		  seq.mapPattern[k] = 0;
		}
		else{
		  k++;
		}
      }
      else if(scan.SetPattern[i] >= '0' && scan.SetPattern[i] <= '9'){
		//! On-Point
		while(scan.SetPattern[i] >= '0' && scan.SetPattern[i] <= '9' && scan.SetPattern[i] != '\0'){//more than two digits
		  seq.mapPattern[k] = (scan.SetPattern[i] - '0') + seq.mapPattern[k] * 10;
		  i++;
		}
		i--;//081114 in
		//! 不正データの場合は無視
		if(seq.mapPattern[k] > scan.OnNumber){
			uM3("trkInitParam(); invalid SetPattern[%d]=%c(%d): too much on points!", i, scan.SetPattern[i], seq.mapPattern[k]);
		  //uM1("trkInitParam(); invalid SetPattern[%d]: too much on points!", scan.SetPattern[k]);
		  seq.mapPattern[k] = 0;
		}
		else{
		  k++;
		}
      }
     else if(scan.SetPattern[i] == ','){
	}
      else{
//     else if(scan.SetPattern[i] != ','){
		uM1("trkInitParam(); invalid SetPattern data [%c]", scan.SetPattern[i]);
      }

      while(scan.SetPattern[i] != ',' && scan.SetPattern[i] != '\0'){
		i++;
      }
	if(scan.SetPattern[i] == '\0'){//081114 in
		break;
	}

      i++;
    }
	if(seq.mapNum != k){
		uM("CAUTION!! This scan table seems inconsistent. Please check it.");
	}
    seq.mapNum = k; //!< 不正データを除去後の観測点数
    //uM1("trkInitParam(); debug seq.mapNum=%d", seq.mapNum);
    //uM1("trkInitParam(); debug scan.OnNumber=%d", scan.OnNumber);
    //uM1("trkInitParam(); debug scan.OffNumber=%d", scan.OffNumber);

    //! 確認
//     for(k = 0; k < seq.mapNum; k++){
//       if(seq.mapPattern[k] < - scan.OffNumber){
// 	char tmp = - seq.mapPattern[k] - 1 + 'A';
// 	uM1("trkInitParam(); invalid SetPattern[%c] ans set 'A'", tmp);
// 	seq.mapPattern[k] = -1;
//       }
//       else if(seq.mapPattern[k] > scan.OnNumber){
// 	uM1("trkInitParam(); invalid SetPattern[%d] ans set '1'", scan.SetPattern[k]);
// 	seq.mapPattern[k] = 1;
//       }
//     }
    //! debug
    //printf("trkInitParam(); debug mapPattern = [");
    for(k = 0; k < seq.mapNum; k++){
	uM1("map:%d", seq.mapPattern[k]);
    //  printf("%d,", seq.mapPattern[k]);
    }
    //printf("]¥n");
  }


	return 0;//normal end
}
void scanTableEnd(){
	if(scan.SetPattern){
		free(scan.SetPattern);
		scan.SetPattern = NULL;
	}
  if(seq.mapPattern){
    free(seq.mapPattern);
    seq.mapPattern = NULL;
  }
  if(seq.on){
    free(seq.on);
    seq.on = NULL;
  }
  if(seq.linePath){
    free(seq.linePath);
    seq.linePath = NULL;
  }
  if(seq.off){
    free(seq.off);
    seq.off = NULL;
  }

}

int scanTableGetOnCoord(){
	return scan.OnCoord;
}
int scanTableGetOffCoord(){
	return scan.OffCoord;
}
int scanTableGetOffNumber(){
	return scan.OffNumber;
}
int scanTableGetScanFlag(){
	return scan.ScanFlag;
}
//int scanTableGetOffInterval();
int scanTableGetOffMode(){
	return scan.OffMode;
}
//char* scanTableGetSetPattern();
int scanTableGetOnNumber(){
	return scan.OnNumber;
}
double scanTableGetPosAngleRad(){
	return scan.posAngleRad;
}
double scanTableGetLineTime(){
	return scan.LineTime;
}


const double* scanTableGetOn(int index){
	return &seq.on[index];
}
const double* scanTableGetOff(int index){
	return &seq.off[index];
}
double scanTableGetOnValue(int index){
	return seq.on[index];
}
double scanTableGetOffValue(int index){
	return seq.off[index];
}
double scanTableGetLinePath(int index){
	return seq.linePath[index];
}
int scanTableGetNumberOfPoints(){
	return seq.mapNum;
}
int scanTableGetPointNumber(int index){
	return seq.mapPattern[index];
}

