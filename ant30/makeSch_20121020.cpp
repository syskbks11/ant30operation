/*!
\file makeSch.cpp
\date 2008.01.10
\author Y.Koide
\brief Make Schedule file for tkb32
*/
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include "configuration.h"

const double PI = M_PI;

//const char dir[] = "../bin";

#define delPrm(x) {if((x)){delete[] (x); (x) = NULL; }}
#define setPrms(x, y) {if(confSetKey((x))){int len = strlen(confGetVal()); (y) = new char[len + 1]; strcpy((y), confGetVal()); }}
#define setPrmi(x, y) {if(confSetKey((x))){(y) = atoi(confGetVal()); }}
#define setPrmf(x, y) {if(confSetKey((x))){(y) = atof(confGetVal()); }}
#define setPrmf2(x, y1, y2) (confSetKey((x)) ? (sscanf(confGetVal(), "%lf %lf", &(y1), &(y2)) != 2) : (0))
#define chkPrm(a, str) {if((a)){uM((str)); return -1; }}
#define chkPrm1(a, str, x1) {if((a)){uM1((str), (x1)); return -1; }}
#define chkPrm2(a, str, x1, x2) {if((a)){uM2((str), (x1), (x2)); return -1; }}
#define chkPrm3(a, str, x1, x2, x3) {if((a)){uM3((str), (x1), (x2), (x3)); return -1; }}
#define chkPrm4(a, str, x1, x2, x3) {if((a)){uM4((str), (x1), (x2), (x3), (x4)); return -1; }}

//! device
static int BeUse01 = 0;
static double IfAtt = 0;
static double IfAttDecR = 0;
static double IfAttR = 0;
//! scan
static int ScanFlag = 0; 
//static int SchInc = 0;         //! 1:Include Schedule Data 0:Not Include
static int RSkyInterval = 0;
static int Coordinate = 0;
static double PosAngle = 0;
static int OnNumber = 0;
static int OnGridNoX = 0;
static int OnGridNoY = 0;
static double OnStartX = 0;
static double OnStartY = 0;
static double OnGridSpX = 0;
static double OnGridSpY = 0;
static int  OnPathNo = 0;
static double OnPathSp = 0;
static double TimeScan = 0;
static double TimeApp = 0;
static int OffInterval = 0;
static int OffNumber = 0; 
static int OffMode = 0;
static int OffCoord = 0;
static int sofNo = 0;
static int offNo = 0;
static int offFlg = 0;
//! For Raster



//! Function
static const char* OnPoint();
static const char* getOff();
static void calRadHorD(const int horD, const int min, const double sec, double* rad);
static void calRadDeg(const int deg, const int min, const double sec, double* rad);
//static void calPosAng(double* x, double* y);

int main(int argc, char* argv[]){
  std::string SetPattern;
  int d1, d2, m1, m2;
  double s1, s2;
  double r1, r2;
  int len;
  char tmp[1024] = {'\0'};
  FILE* fp = NULL;

  if(argc != 3){
    printf("Usage:\n");
    printf("      %s OBSTABLE OUTPUT_SCHFILENAME\n", argv[0]);
    return 1;
  }

  d1 = m1 = s1 = d2 = m2 = s2 = 0;

  //! Get All Obs Table Data
  confInit();
  sprintf(tmp, "%s", argv[1]);
  if(confAddFile(tmp)){
    printf("Not found table(%s) error!!", tmp);
    return 2;
  }

  //! Set Parameter
  //! Common
  setPrmi("Hardware", BeUse01);   //!< $B8_49@-0];}$N$?$a$KEvLL;D$7$F$*$/(B
  setPrmi("BeUse01", BeUse01);
  setPrmf("Att_H", IfAtt);        //!< $B2<0L8_49(B
  setPrmf("IfAtt", IfAtt);
  setPrmf("IfAttDecR", IfAttDecR);

  setPrmi("ScanFlag", ScanFlag);
  setPrmi("RSkyInterval", RSkyInterval);
  setPrmf("PosAngle", PosAngle);
  setPrmi("OnNumber", OnNumber);

  //! Grid, Raster
  if(confSetKey("OnGridNoX"))
    OnGridNoX = atoi(confGetVal());
  if(confSetKey("OnGridNoY"))
    OnGridNoY = atoi(confGetVal());
  if(confSetKey("OnStartX"))
    OnStartX = atof(confGetVal()) * PI / (3600.0 * 180.0); //!< rad
  if(confSetKey("OnStartY"))
    OnStartY = atof(confGetVal()) * PI / (3600.0 * 180.0); //!< rad
  if(confSetKey("OnGridSpX"))
    OnGridSpX = atof(confGetVal()) * PI / (3600.0 * 180.0); //!< rad
  if(confSetKey("OnGridSpY"))
    OnGridSpY = atof(confGetVal()) * PI / (3600.0 * 180.0); //!< rad
  if(confSetKey("OnPathNo"))
    OnPathNo = atoi(confGetVal());
  if(confSetKey("OnPathSp"))
    OnPathSp = atof(confGetVal()) * PI / (3600.0 * 180.0); //!< rad
  if(confSetKey("TimeApp"))
    TimeApp = atof(confGetVal());
  if(confSetKey("TimeScan"))
    TimeScan = atof(confGetVal());
  if(confSetKey("OffInterval"))
    OffInterval = atoi(confGetVal());
  if(confSetKey("OnOffPattern"))                 //!< $B2<0L8_49(B
    OffNumber = atoi(confGetVal());
  if(confSetKey("OffNumber"))
    OffNumber = atoi(confGetVal());
  if(confSetKey("OffMode"))
    OffMode = atoi(confGetVal());
  if(confSetKey("OffCoord"))
    OffCoord = atoi(confGetVal());

  //! Source
  d1 = m1 = s1 = d2 = m2 = s2 = 0;
  if(confSetKey("Coordinate"))
    Coordinate = atoi(confGetVal());
  if(confSetKey("X_HorD"))
    d1 = atoi(confGetVal());
  if(confSetKey("X_Min"))
    m1 = atoi(confGetVal());
  if(confSetKey("X_Sec"))
    s1 = atof(confGetVal());
  if(confSetKey("Y_Deg"))
    d2 = atoi(confGetVal());
  if(confSetKey("Y_Min"))
    m2 = atoi(confGetVal());
  if(confSetKey("Y_Sec"))
    s2 = atof(confGetVal());
  if(Coordinate == 1){ //!< RADEC
    calRadHorD(d1, m1, s1, &r1);
  }
  else{ //!< LB,AZEL
    calRadDeg(d1, m1, s1, &r1);
  }
  calRadDeg(d2, m2, s2, &r2);
  sprintf(tmp, "X_Rad %.15lf",r1);
  confAddStr(tmp);
  sprintf(tmp, "Y_Rad %.15lf", r2);
  confAddStr(tmp);

  ////////////////////////////////////////////////////////////////
  //! Device
  sprintf(tmp, "BeUse01 %d", BeUse01);
  confAddStr(tmp);
  sprintf(tmp, "IfAtt %lf", IfAtt);
  confAddStr(tmp);
  IfAttR = IfAtt - IfAttDecR;
  sprintf(tmp, "IfAttR %lf", IfAttR);
  confAddStr(tmp);

  //! PosAngle_Rad$B$r%;%C%H(B
  r1 = PosAngle * PI / 180.0;
  sprintf(tmp, "PosAngle_Rad %.15lf", r1);
  confAddStr(tmp);

  //! OffNumber
  sprintf(tmp, "OffNumber %d", OffNumber);
  confAddStr(tmp);

  //! Off
  //! OffX?_Rad,OffY?_Rad$B$r%Q%i%a!<%?$KDI2C(B
  for(int i = 0; i < OffNumber; i++){    
    d1 = m1 = s1 = 0;
    d2 = m2 = s2 = 0;
    r1 = r2 = 0;

    sprintf(tmp, "OffX%d_HorD", i+1);
    if(confSetKey(tmp))
      d1 = atoi(confGetVal());
    sprintf(tmp, "OffX%d_Min", i+1);
    if(confSetKey(tmp))
      m1 = atoi(confGetVal());
    sprintf(tmp, "OffX%d_Sec", i+1);
    if(confSetKey(tmp))
      s1 = atof(confGetVal());
    
    sprintf(tmp, "OffY%d_Deg", i+1);
    if(confSetKey(tmp))
      d2 = atoi(confGetVal());;
    sprintf(tmp, "OffY%d_Min", i+1);
    if(confSetKey(tmp))
      m2 = atoi(confGetVal());
    sprintf(tmp, "OffY%d_Sec", i+1);
    if(confSetKey(tmp))
      s2 = atof(confGetVal());
    
    //if(OffMode == 2 && (OffCoord == 1 || OffCoord == 2)){
    if(OffMode == 2 && OffCoord == 1){//! debug 090424 //090608 from Ogino by Nagai
      //!< Absolute Value & (RADEC or LB)
      calRadHorD(d1, m1, s1, &r1);
    }
    else{
      //!< From Center
      calRadDeg(d1, m1, s1, &r1);
    }
    calRadDeg(d2, m2, s2, &r2);
    sprintf(tmp, "OffX%d_Rad %.15lf", i+1, r1);
    confAddStr(tmp);
    sprintf(tmp, "OffY%d_Rad %.15lf", i+1, r2);
    confAddStr(tmp);
  }

  if(1 == ScanFlag){
    //! On-Point
    //! On
    confAddStr("OnX1_Rad\t0");
    confAddStr("OnY1_Rad\t0");
    confAddStr("OnNumber\t1");

    //! SetPattern
    SetPattern = "SetPattern\t";
    SetPattern += "R,"; //!< $B:G=i$K(BR$B$rF~$l$k(B
    len = (OffInterval + 1) * RSkyInterval;
    if(2 == OffMode && RSkyInterval % 2){
      len = len * 2 + 1; //! Off1 Off2 Mode
    }
    for(int i = 0; i < len; i++){
      SetPattern += OnPoint();
      SetPattern += ',';
    }
  }
  else if(ScanFlag == 2){
    //! 5-Points    
    confAddStr("OffNumber\t2"); //!< Off-Point * 2
    confAddStr("OnNumber\t6");
    
    for(int i = 0; i < 3; i++){
      r1 = OnGridSpX * (i - 1);
      r2 = 0;
      //calPosAng(&r1, &r2);
      sprintf(tmp, "OnX%d_Rad\t%.15lf", i+1, r1);
      confAddStr(tmp);
      sprintf(tmp, "OnY%d_Rad\t%.15lf", i+1, r2);
      confAddStr(tmp);
    }
    for(int i = 0; i < 3; i++){
      r1 = 0;
      r2 = OnGridSpY * (1 - i);
      //calPosAng(&r1, &r2);
      sprintf(tmp, "OnX%d_Rad\t%.15lf", i+4, r1);
      confAddStr(tmp);
      sprintf(tmp, "OnY%d_Rad\t%.15lf", i+4, r2);
      confAddStr(tmp);
    }

    SetPattern = "SetPattern\t";
    SetPattern += "R,"; //!< $B:G=i$K(BR$B$rF~$l$k(B
    SetPattern += "A,1,2,3,B,4,5,6";
  }
  else if(ScanFlag == 3){
    //! Grid
    int num = OnGridNoX * OnGridNoY;
    sprintf(tmp, "OnNumber\t%d", num);
    confAddStr(tmp);
    
    SetPattern = "SetPattern\t";
    SetPattern += "R,"; //!< $B:G=i$K(BR$B$rF~$l$k(B
    for(int i = 0; i < OnGridNoY; i++){
      for(int j = 0; j < OnGridNoX; j++){
	int no = j + i * OnGridNoX;
	if(!(no % OffInterval)){
	  SetPattern += getOff();
	  SetPattern += ",";
	}

	r1 = j * OnGridSpX + OnStartX;
	r2 = i * OnGridSpY + OnStartY;
	//calPosAng(&r1, &r2);
	sprintf(tmp, "OnX%d_Rad\t%.15lf", no+1, r1);
	confAddStr(tmp);
	sprintf(tmp, "OnY%d_Rad\t%.15lf", no+1, r2);
	confAddStr(tmp);
	sprintf(tmp, "%d,", no + 1);
	SetPattern += tmp;


				//*********** 20110911 new! ***************
				// onInterval: Rを入れるまでのon点の観測回数
				//! onIntervalは実際の整数で置き換えてください
				if((no+1) % 5 == 0) {
					SetPattern += "R,";
				}
				//***************************************

      }
    }
  }
  else if(ScanFlag == 4){
    //! Random
    SetPattern = "SetPattern\t";
    //SetPattern += "R,"; //!< $B:G=i$K(BR$B$rF~$l$k(B
    if(confSetKey("SetPattern")){
      SetPattern += confGetVal();
    }
    else if(confSetKey("SeqPattern")){
      SetPattern += confGetVal();
    }
    else{
      printf("Not Found SetPattern\n");
      return 1;
    }
    
    //! On
    for(int i = 0; i < OnNumber; i++){
      d1 = m1 = s1 = 0;
      d2 = m2 = s2 = 0;
      r1 = r2 = 0;
      sprintf(tmp, "OnX%d_Deg", i+1);
      if(confSetKey(tmp))
	d1 = atoi(confGetVal());
      sprintf(tmp, "OnX%d_Min", i+1);
      if(confSetKey(tmp))
	m1 = atoi(confGetVal());
      sprintf(tmp, "OnX%d_Sec", i+1);
      if(confSetKey(tmp))
	s1 = atof(confGetVal());
    
      sprintf(tmp, "OnY%d_Deg", i+1);
      if(confSetKey(tmp))
	d2 = atoi(confGetVal());;
      sprintf(tmp, "OnY%d_Min", i+1);
      if(confSetKey(tmp))
	m2 = atoi(confGetVal());
      sprintf(tmp, "OnY%d_Sec", i+1);
      if(confSetKey(tmp))
	s2 = atof(confGetVal());

      calRadDeg(d1, m1, s1, &r1);
      calRadDeg(d2, m2, s2, &r2);
      //calPosAng(&r1, &r2);
      sprintf(tmp, "OnX%d_Rad %.15lf", i+1, r1);
      confAddStr(tmp);
      sprintf(tmp, "OnY%d_Rad %.15lf", i+1, r2);
      confAddStr(tmp);
    }
  }
  else if(ScanFlag == 5){
    //! 9-Points
    confAddStr("OffNumber\t2"); //!< Off-Point * 2
    confAddStr("OnNumber\t10");

    for(int i = 0; i < 5; i++){
      r1 = OnGridSpX * (i - 2);
      r2 = 0;
      sprintf(tmp, "OnX%d_Rad\t%.15lf", i+1, r1);
      confAddStr(tmp);
      sprintf(tmp, "OnY%d_Rad\t%.15lf", i+1, r2);
      confAddStr(tmp);
    }
    for(int i = 0; i < 5; i++){
      r1 = 0;
      r2 = OnGridSpY * (2 - i);
      sprintf(tmp, "OnX%d_Rad\t%.15lf", i+6, r1);
      confAddStr(tmp);
      sprintf(tmp, "OnY%d_Rad\t%.15lf", i+6, r2);
      confAddStr(tmp);
    }

    SetPattern = "SetPattern\t";
    SetPattern += "R,";
    SetPattern += "A,1,2,3,4,5,B,6,7,8,9,10";
  }
  else if(ScanFlag == 6){
    //! Raster
    double OnPathLn_Rad = 0;
    double r3, r4;

    //! SetPattern
    if(confSetKey("SetPattern")){
      SetPattern = confGetVal();
    }
    else if(confSetKey("SeqPattern")){ //!< $B2<0L8_49(B
      SetPattern = confGetVal();
    }
    else{
      printf("Not Found SetPattern\n");
      return 1;
    }

    //! OnPathLn
    d1 = m1 = s1 = 0;
    if(confSetKey("OnPathLn_Deg"))
      d1 = atoi(confGetVal());
    if(confSetKey("OnPathLn_Min"))
      m1 = atoi(confGetVal());
    if(confSetKey("OnPathLn_Sec"))
      s1 = atof(confGetVal());
    calRadDeg(d1, m1, s1, &OnPathLn_Rad);
    //sprintf(tmp, "OnPathLn_Rad\t%.15lf", r1);
    //confAddStr(tmp);
    //! 1Line$B$N%9%-%c%s5wN%$H;~4V$r=PNO(B
    double LineTime = TimeApp + TimeScan;
    double LinePath_Rad =  LineTime * OnPathLn_Rad / TimeScan;
    double AppPath_Rad = TimeApp * OnPathLn_Rad / TimeScan;
    
    sprintf(tmp, "OnNumber\t%d", OnPathNo);
    confAddStr(tmp);
    for(int i = 0; i < OnPathNo; i++){
      if(i % 2){
	r1 = OnStartX + LinePath_Rad;
	r3 = - LinePath_Rad;
      }
      else{
	r1 = OnStartX - AppPath_Rad;
	r3 = LinePath_Rad;
      }
      r2 = (double)i * OnPathSp + OnStartY;
      r4 = 0;
      sprintf(tmp, "OnX%d_Rad\t%.15lf", i+1, r1);
      confAddStr(tmp);
      sprintf(tmp, "OnY%d_Rad\t%.15lf", i+1, r2);
      confAddStr(tmp);
      
      sprintf(tmp, "LinePathX%d_Rad\t%.15lf", i+1, r3);
      confAddStr(tmp);
      sprintf(tmp, "LinePathY%d_Rad\t%.15lf", i+1, r4);
      confAddStr(tmp);
    }
    sprintf(tmp, "LineTime\t%.0lf", LineTime);
    confAddStr(tmp);
  }
  else if(ScanFlag == 7){
    //! Cross
    double OnPathLn_Rad = 0;
    double r3, r4;

    //! SetPattern
    if(confSetKey("SetPattern")){
      SetPattern = confGetVal();
    }
    else if(confSetKey("SeqPattern")){ //!< $B2<0L8_49(B
      SetPattern = confGetVal();
    }
    else{
      printf("Not Found SetPattern\n");
      return 1;
    }

    //! OnPathLn
    d1 = m1 = s1 = 0;
    if(confSetKey("OnPathLn_Deg"))
      d1 = atoi(confGetVal());
    if(confSetKey("OnPathLn_Min"))
      m1 = atoi(confGetVal());
    if(confSetKey("OnPathLn_Sec"))
      s1 = atof(confGetVal());
    calRadDeg(d1, m1, s1, &OnPathLn_Rad);
    //sprintf(tmp, "OnPathLn_Rad\t%.15lf", r1);
    //confAddStr(tmp);
    //! 1Line$B$N%9%-%c%s5wN%$H;~4V$r=PNO(B
    double LineTime = TimeApp + TimeScan;//[s]
    double LinePath_Rad =  LineTime * OnPathLn_Rad / TimeScan;//[rad]
    double AppPath_Rad = TimeApp * OnPathLn_Rad / TimeScan;//[rad]
    
	if(OnPathNo != 2){//090616 in
		printf("OnPathNo is not 2! (%d)\n", OnPathNo);
	}
    sprintf(tmp, "OnNumber\t%d", OnPathNo);
    confAddStr(tmp);

	// along X axis
	int i = 0;
	r1 = -OnPathLn_Rad/2 - AppPath_Rad;
	r2 = 0;
	r3 = LinePath_Rad;
	r4 = 0;
      sprintf(tmp, "OnX%d_Rad\t%.15lf", i+1, r1);
      confAddStr(tmp);
      sprintf(tmp, "OnY%d_Rad\t%.15lf", i+1, r2);
      confAddStr(tmp);
      
      sprintf(tmp, "LinePathX%d_Rad\t%.15lf", i+1, r3);
      confAddStr(tmp);
      sprintf(tmp, "LinePathY%d_Rad\t%.15lf", i+1, r4);
      confAddStr(tmp);

	// along Y axis
	i = 1;
	r1 = 0;
	r2 = -OnPathLn_Rad/2 - AppPath_Rad;
	r3 = 0;
	r4 = LinePath_Rad;
      sprintf(tmp, "OnX%d_Rad\t%.15lf", i+1, r1);
      confAddStr(tmp);
      sprintf(tmp, "OnY%d_Rad\t%.15lf", i+1, r2);
      confAddStr(tmp);
      
      sprintf(tmp, "LinePathX%d_Rad\t%.15lf", i+1, r3);
      confAddStr(tmp);
      sprintf(tmp, "LinePathY%d_Rad\t%.15lf", i+1, r4);
      confAddStr(tmp);

/*
    for(int i = 0; i < OnPathNo; i++){
	if(i % 2){//along X
		r1 = OnStartX + LinePath_Rad;
		r3 = - LinePath_Rad;
		r2 = (double)i * OnPathSp + OnStartY;
		r4 = 0;
	}
	else{//along Y
		r1 = (double)i * OnPathSp + OnStartY;
		r3 = 0;
		r2 = - OnStartX - LinePath_Rad;
		r4 = LinePath_Rad;
      }
      sprintf(tmp, "OnX%d_Rad\t%.15lf", i+1, r1);
      confAddStr(tmp);
      sprintf(tmp, "OnY%d_Rad\t%.15lf", i+1, r2);
      confAddStr(tmp);
      
      sprintf(tmp, "LinePathX%d_Rad\t%.15lf", i+1, r3);
      confAddStr(tmp);
      sprintf(tmp, "LinePathY%d_Rad\t%.15lf", i+1, r4);
      confAddStr(tmp);
    }
*/
    sprintf(tmp, "LineTime\t%.0lf", LineTime);
    confAddStr(tmp);
  }
  else if(ScanFlag == 1001){
    //! Source Tracking    
  }
  else if(ScanFlag == 1002){
    //! Antenna track leeward
  }
  else{
    //! ScanFlag Error
    return 4;
  }

  confAddStr(SetPattern.c_str());

  //! File out
  fp = fopen(argv[2], "w");
  if(!fp){
    return 5;
  }
  fprintf(fp, "%s", confGetAllKeyVal());
  fclose(fp);

  return 0;
}


const char* OnPoint(){
  char R[] = "R";

  //! Off On Off On ... Off On R $B7+$jJV$7(B
  if(sofNo >= (OffInterval * RSkyInterval)){
    //! End ScanFlag. RSky
    sofNo = 0;
    offFlg = 0;
    return R;
  }
  else if((sofNo % OffInterval == 0) && offFlg == 0){
    //! OffPoint
    offFlg = 1;
    return getOff();
  }
  else{
    //! OnPoint
    offFlg = 0;
    return "1";
  }
}

const char* getOff(){
  static char off[2] = {'\0'};
  
  off[0] = offNo + 'A';
  if(off[0] == 'R'){
    off[0] = 'S';
  }
  offNo++;
  if(offNo >= OffNumber){
    offNo = 0;
  }

  return off;
}

/*! \fn void calRadHorD(const int horD, const int min, const double sec, double* rad)
\brief $B;~3Q$N;~!"J,!"IC$+$i%i%8%"%sC10L$N3QEY$r5a$a$k!#(B
\param[in] horD $B;~3Q$N;~(B
\param[in] min $BJ,(B
\param[in] sec $BIC(B
\param[out] rad radian$BC10L$N3QEY(B
*/
void calRadHorD(const int horD, const int min, const double sec, double* rad){
  double x;
  
  x = 0;
  if(horD != 0){
    if(horD > 0){
      x = (double)horD + min / 60.0 + sec / 3600.0;
    }
    else{
      x = (double)horD - min / 60.0 - sec / 3600.0;
    }
  }
  else if(min != 0){
    if(min > 0){
      x = min / 60.0 + sec / 3600.0;
    }
    else{
      x = min / 60.0 - sec / 3600.0;
    }
  }
  else if(sec != 0){
    x = sec / 3600.0;
  }

  x *= PI / 12.0;
  *rad = x;
}

/*! \fn void calRadDeg(const int deg, const int min, const double sec double* rad)
\brief $BDL>o$N3QEYDj5A$NEY!"J,!"IC$+$i%i%8%"%sC10L$N3QEY$r5a$a$k!#(B
\param[in] deg $B3QEY$N@0?tItJ,(B
\param[in] min $BJ,(B
\param[in] sec $BIC(B
\param[out] rad radian$BC10L$N3QEY(B
*/
void calRadDeg(const int deg, const int min, const double sec, double* rad){
  double x;

  x = 0;
  if(deg != 0){
    if(deg > 0){
      x = (double)deg + min / 60.0 + sec / 3600.0;
    }
    else{
      x = (double)deg - min / 60.0 - sec / 3600.0;
    }
  }
  else if(min != 0){
    if(min > 0){
      x = min / 60.0 + sec / 3600.0;
    }
    else{
      x = min / 60.0 - sec / 3600.0;
    }
  }
  else{
    x = sec / 3600.0;
  }
  x *= PI / 180.0;
  *rad = x;
}

/*!
\brief $B:BI87O$rH?;~7W2s$j$K(BPositionAngle$B$@$12sE>$5$;$k(B
*/
void calPosAng(double* x, double* y){
  double arg = PosAngle * PI / 180.0;
  const double _x = *x;
  const double _y = *y;

  *x = cos(arg) * _x + sin(arg) * _y;
  *y = -sin(arg) * _x + cos(arg) * _y;
  return;
}
