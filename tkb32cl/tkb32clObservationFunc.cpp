/*!
\file tkb32clObservationFunc.cpp
\date 2009.08.03
\author NAGAI Makoto
\brief tkb32 Central Program Client Function for the 32-m telescope & the 30-cm telescope. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <errno.h>//errno
#include "../libtkb/src/libtkb.h"
#include "tkb32clFunc.h"
#include "tkb32clObservationFunc.h"

int tkb32clObservationFuncSetTable(const char* dir, const char* grpName, const char* obsrName, const char* obsFile, char* msg){
  char tmp[1024];
//  char msg[1024];
  int i;
  int ret;

  confInit();
  sprintf(tmp, "%s/tkb32cl.conf", dir);
  if(confAddFile(tmp)){
    /* throw "";*/
  }
/*
  for(i = 1; i < p->argc; i++){
    if(confAddFile(p->argv[i])){
      /* throw "";
    }
  }
*/
/*
  grpName = gtk_entry_get_text(GTK_ENTRY(p->entGroupName));
  obsrName = gtk_entry_get_text(GTK_ENTRY(p->entProjectName));
  obsFile = gtk_entry_get_text(GTK_ENTRY(p->entObsFileName));
*/
  if(grpName[0] == '\0'){
	sprintf(msg, "Please input Group Name.");
//    dialog_ok("Please input Group Name.");
    return 1;
  }
  if(obsrName[0] == '\0'){
	sprintf(msg, "Please input Project Name.");
//    dialog_ok("Please input Project Name.");
    return 2;
  }
  if(obsFile[0] == '\0'){
	sprintf(msg, "Please input obsFile Name.");
//    dialog_ok("Please input obsFile Name.");
    return 3;
  }

  //sprintf(tmp, "%s/%s.obs", dir, obsFile);
  sprintf(tmp, "%s/%s/%s/%s.obs", dir, grpName, obsrName, obsFile);
  if(confAddFile(tmp)){    sprintf(msg, "Not found obsTable(%s)", tmp);
//    dialog_ok(msg);
    return 4;
  }
  sprintf(tmp, "%s/%s/%s/%s", dir, grpName, obsrName, confGetStr("DeviceTable"));
  if(confAddFile(tmp)){
	sprintf(msg, "Device table (%s) not found.", tmp);
//    printf("init Not found DeviceTable(%s)", tmp);
//    dialog_ok("Device table not found.");
    return 5;
    /* throw "";*/
  }
  sprintf(tmp, "%s/%s/%s/%s", dir, grpName, obsrName, confGetStr("SourceTable"));
  if(confAddFile(tmp)){
	sprintf(msg, "Source table (%s) not found.", tmp);
//    printf("init Not found SourceTable(%s)", tmp);
//    dialog_ok("Source table not found.");
    return 6;
    /* throw "";*/
  }
  sprintf(tmp, "%s/%s/%s/%s", dir, grpName, obsrName, confGetStr("ScanTable"));
  if(confAddFile(tmp)){
	sprintf(msg, "Scan table (%s) not found.", tmp);
//    printf("init Not found ScanTable(%s)", tmp);
//    dialog_ok("Scan table not found.");
    return 7;
    /* throw "";*/
  }
  //! obsTable名を追加
  sprintf(tmp, "ObsTable %s.obs", obsFile);
  confAddStr(tmp);

  //! パラメータを表示＆記録
  printf("Initialize Parameters.");
  confPrint();

  ret = tkb32clFuncReqInit();
  if(ret){
	sprintf(tmp, "Reject init");
//    dialog_ok("Reject init");
	return 8;
  }
//  callbacksSubViewParam(p);
	return 0;//normal end

}
int tkb32clObservationFuncSetSource(const char* dir, const char* obsFlag, const char* raH, const char* raM, const char* raS, const char* decH, const char* decM, const char* decS, char* msg){
  int d1, m1, d2, m2;
  double s1, s2;
  int i;
  int ret;
  char tmp[256];
  int SourceFlag = 0;
  int Coordinate = 0;
  int Epoch = 0;
  char SourceName[256] = {'\0'};

  const char* const srctrk[] = {
    "ObsTable srctrk.obs",
    "SourceName	srctrk",
    "ScanFlag	1001",          //!< 1001:SourceTracking
    //"SourceFlag	0",         //!< 0:太陽系外 1~12:太陽系惑星 99:Commet
    //"Coordinate	1",         //!< 1:RADEC 2:LB 3:AZEL
    //"Epoch	2",             //!< 1:B1950 2:J2000
    NULL
  };

  confInit();
  sprintf(tmp, "%s/tkb32cl.conf", dir);
  if(confAddFile(tmp)){
    /* throw "";*/
  }
  //! ダミーのパラメータを登録
  for(i = 0; srctrk[i] != NULL; i++){
    confAddStr(srctrk[i]);
  }

  if(obsFlag[0] == '\0'){
	sprintf(msg, "Please input Flag");
/*
    dialog_ok("Please input Flag");
*/
    return 1;
  }
  else if(0 == strcmp(obsFlag, "RADEC (B1950)")){
    SourceFlag = 0;
    Coordinate = 1;
    Epoch = 1;
  }
  else if(0 == strcmp(obsFlag, "RADEC (J2000)")){
    SourceFlag = 0;
    Coordinate = 1;
    Epoch = 2;
  }
  else if(0 == strcmp(obsFlag, "LB (B1950)")){
    SourceFlag = 0;
    Coordinate = 2;
    Epoch = 1;
  }
  else if(0 == strcmp(obsFlag, "LB (J2000)")){
    SourceFlag = 0;
    Coordinate = 2;
    Epoch = 2;
  }
  else if(0 == strcmp(obsFlag, "AZEL")){
    SourceFlag = 99;
    Coordinate = 3;
    Epoch = 2;
  }
  else if(0 == strcmp(obsFlag, "Mercury")){
    SourceFlag = 1;
    Coordinate = 1;
    Epoch = 2;
    strcpy(SourceName, obsFlag);
  }
  else if(0 == strcmp(obsFlag, "Venus")){
    SourceFlag = 2;
    Coordinate = 1;
    Epoch = 2;
    strcpy(SourceName, obsFlag);
  }
  else if(0 == strcmp(obsFlag, "Mars")){
    SourceFlag = 4;
    Coordinate = 1;
    Epoch = 2;
    strcpy(SourceName, obsFlag);
  }
  else if(0 == strcmp(obsFlag, "Jupiter")){
    SourceFlag = 5;
    Coordinate = 1;
    Epoch = 2;
    strcpy(SourceName, obsFlag);
  }
  else if(0 == strcmp(obsFlag, "Saturn")){
    SourceFlag = 6;
    Coordinate = 1;
    Epoch = 2;
    strcpy(SourceName, obsFlag);
  }
  else if(0 == strcmp(obsFlag, "Uranus")){
    SourceFlag = 7;
    Coordinate = 1;
    Epoch = 2;
    strcpy(SourceName, obsFlag);
  }
  else if(0 == strcmp(obsFlag, "Neptune")){
    SourceFlag = 8;
    Coordinate = 1;
    Epoch = 2;
    strcpy(SourceName, obsFlag);
  }
  else if(0 == strcmp(obsFlag, "Pluto")){
    SourceFlag = 9;
    Coordinate = 1;
    Epoch = 2;
    strcpy(SourceName, obsFlag);
  }
  else if(0 == strcmp(obsFlag, "Moon")){
    SourceFlag = 10;
    Coordinate = 1;
    Epoch = 2;
    strcpy(SourceName, obsFlag);
  }
  else if(0 == strcmp(obsFlag, "Sun")){
    SourceFlag = 11;
    Coordinate = 1;
    Epoch = 2;
    strcpy(SourceName, obsFlag);
  }
  sprintf(tmp, "SourceName %s", SourceName);
  confAddStr(tmp);
  sprintf(tmp, "SourceFlag %d", SourceFlag);
  confAddStr(tmp);
  sprintf(tmp, "Coordinate %d", Coordinate);
  confAddStr(tmp);
  sprintf(tmp, "Epoch %d", Epoch);
  confAddStr(tmp);

  //! 座標取得
  d1 = m1 = s1 = 0;
  d2 = m2 = s2 = 0;
  if(SourceFlag == 0 || SourceFlag == 99){
/*
    raH = gtk_entry_get_text(GTK_ENTRY(p->entObsXDeg));
    raM = gtk_entry_get_text(GTK_ENTRY(p->entObsXMin));
    raS = gtk_entry_get_text(GTK_ENTRY(p->entObsXSec));
    decH = gtk_entry_get_text(GTK_ENTRY(p->entObsYDeg));
    decM = gtk_entry_get_text(GTK_ENTRY(p->entObsYMin));
    decS = gtk_entry_get_text(GTK_ENTRY(p->entObsYSec));
*/
    if(raH[0] == '\0' || raM[0] == '\0' || raS[0] == '\0'
      || decH[0] == '\0' || decM[0] == '\0' || decS[0] == '\0'){
	sprintf(msg, "Please input source X Y");
/*
        dialog_ok("Please input source X Y");
*/
        return 2;
    }
    d1 = atoi(raH);
    m1 = atoi(raM);
    s1 = atof(raS);
    d2 = atoi(decH);
    m2 = atoi(decM);
    s2 = atof(decS);
  }
  sprintf(tmp, "X_HorD %d", d1);
  confAddStr(tmp);
  sprintf(tmp, "X_Min %d", m1);
  confAddStr(tmp);
  sprintf(tmp, "X_Sec %lf", s1);
  confAddStr(tmp);
  sprintf(tmp, "Y_Deg %d", d2);
  confAddStr(tmp);
  sprintf(tmp, "Y_Min %d", m2);
  confAddStr(tmp);
  sprintf(tmp, "Y_Sec %lf", s2);
  confAddStr(tmp);

  //! パラメータを表示＆記録
  confPrint();

  ret = tkb32clFuncReqInit();
  if(ret){
	sprintf(msg, "Reject Source Tracking");
/*
    dialog_ok("Reject Source Tracking");
*/
	return 3;
  }
	return 0;//normal end
/*
  callbacksSubViewParam(p);
*/

}


int tkb32clObservationFuncCheckTable(const char* dir, const char* grpName, const char* obsrName, const char* obsFile, char* msg){
	char tmp[1024];
	sprintf(tmp, "%s/%s/%s/%s.obs", dir, grpName, obsrName, obsFile);
	FILE* fp = fopen(tmp, "r");
	if(fp == NULL){
		sprintf(msg, "%s cannot opened; %s", tmp, strerror(errno));
		return 1;
	}
	fclose(fp);
	return 0;//normal end
}
/*
int tkb32clObservationFuncSetSchedule(const char* grpName, const char* obsrName, const char* schFile, char* msg){
	char tmp[1024];
	sprintf(tmp, "%s/%s/%s/%s.sch", dir, grpName, obsrName, schFile);
	FILE* fp = fopen(tmp, "r");
	if(fp == NULL){
		sprintf(msg, "%s cannot opened; %s", tmp, strerror(errno));
//		sprintf(msg, "%s", strerror_r(errno));
		return 1;
	}
	char line[256];
	char* check;
	int ret;
	int nLine = 0;
	char filename[64];
	while(1){
		nLine++;
		check = fgets(line, 256, fp);
		if(check == NULL){//EOF or error
			break;
		}
		if(line[0] == '#' || line[0] == '\n'){
			continue;
		}
		ret = sscanf(line, "%s", &filename);
		if(ret == 0){
			continue;
		}
		ret = tkb32clObservationFuncCheckTable(grpName, obsrName, filename, msg);
		if(ret){
			fclose(fp);
			return 1;
		}
		printf("obsfile %s is readable.\n", filename);
	}
	fclose(fp);
	printf("%d lines were read from %s.\n", nLine, schFile);
	return 0;//normal end
}
*/
