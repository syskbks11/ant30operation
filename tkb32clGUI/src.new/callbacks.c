#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>

#include "../../../import/libtkb.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "callbacksSub.h"
#include "tkb32clGUIParam.h"
const char dir[] = "../etc";

void
on_btnSetParameter_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  char* grpName;
  char* obsrName;
  char* obsFile;
  char tmp[1024];
  char msg[1024];
  int i;
  int ret;

  confInit();
  sprintf(tmp, "%s/tkb32cl.conf", dir);
  if(confAddFile(tmp)){
    /* throw "";*/
  }
  for(i = 1; i < p->argc; i++){
    if(confAddFile(p->argv[i])){
      /* throw "";*/
    }
  }
  grpName = gtk_entry_get_text(GTK_ENTRY(p->entGroupName));
  obsrName = gtk_entry_get_text(GTK_ENTRY(p->entProjectName));
  obsFile = gtk_entry_get_text(GTK_ENTRY(p->entObsFileName));
  if(grpName[0] == NULL){
    dialog_ok(p, "Please input Group Name.");
    return;
  }
  if(obsrName[0] == NULL){
    dialog_ok(p, "Please input Project Name.");
    return;
  }
  if(obsFile[0] == NULL){
    dialog_ok(p, "Please input obsFile Name.");
    return;
  }

  //sprintf(tmp, "%s/%s.obs", dir, obsFile);
  sprintf(tmp, "%s/%s/%s/%s.obs", dir, grpName, obsrName, obsFile);
  if(confAddFile(tmp)){
    sprintf(msg, "Not found obsTable(%s)", tmp);
    dialog_ok(p, msg);
    return;
  }
  sprintf(tmp, "%s/%s/%s/%s", dir, grpName, obsrName, confGetStr("DeviceTable"));
  if(confAddFile(tmp)){
    printf("init Not found DeviceTable(%s)", tmp);
    dialog_ok(p, "Device table not found.");
    return;
    /* throw "";*/
  }
  sprintf(tmp, "%s/%s/%s/%s", dir, grpName, obsrName, confGetStr("SourceTable"));
  if(confAddFile(tmp)){
    printf("init Not found SourceTable(%s)", tmp);
    dialog_ok(p, "Source table not found.");
    return;
    /* throw "";*/
  }
  sprintf(tmp, "%s/%s/%s/%s", dir, grpName, obsrName, confGetStr("ScanTable"));
  if(confAddFile(tmp)){
    printf("init Not found ScanTable(%s)", tmp);
    dialog_ok(p, "Scan table not found.");
    return;
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
    dialog_ok(p, "Reject init");
  }
  callbacksSubViewParam(p);
}


void
on_btnStartObs_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  callbacksSubDrawMap(NULL);
  ret = tkb32clFuncReqStartObs();
  if(ret){
    dialog_ok(p, "Reject startObs");
  }
}


void
on_btnStopObs_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = tkb32clFuncReqStopObs();
  if(ret){
    dialog_ok(p, "Reject stopObs");
  }
}

void
on_btnEnd_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = tkb32clFuncReqEnd();
  if(ret){
    dialog_ok(p, "Reject end");
  }
}

void
on_btnViewParameter_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  gtk_widget_show (p->winParameterList);
}

void
on_btnViewAllStatus_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  gtk_widget_show(p->winAllStatus);
}

void
on_btnChopperOpen_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqRopen();
  }
}


void
on_btnChopperClose_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqRclose();
  }
}


void
on_btnChopperOrigin_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqRorigine();
  }
}


void
on_btn2ndIFGHz_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*)user_data;
  char* tmp;
  double freq;
  int ret;

  tmp = gtk_entry_get_text(GTK_ENTRY(p->ent2ndIFGHz));
  if(tmp[0] == NULL){
    return;
  }
  freq = atof(tmp);
  
  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetLo2Freq(freq);
  }
}


void
on_btn2ndIFdBm_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*)user_data;
  char* tmp;
  double amp;
  int ret;

  tmp = gtk_entry_get_text(GTK_ENTRY(p->ent2ndIFdBm));
  if(tmp[0] == NULL){
    return;
  }
  amp = atof(tmp);

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetLo2Amp(amp);
  }
}


void
on_btnSASec_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*)user_data;
  char* tmp;
  double t;
  int ret;

  tmp = gtk_entry_get_text(GTK_ENTRY(p->entSASec));
  if(tmp[0] == NULL){
    return;
  }
  t = atof(tmp);

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetSaIntegTime(t);
  }
}


void
on_btnSAStartR_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*)user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSaStart(2);//2:R
  }
}

void
on_btnSAStartOn_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*)user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSaStart(0);//0:On
  }
}

void
on_btnSAStartOff_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*)user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSaStart(1);//1:Off
  }
}

void
on_btnACUPosOff_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*)user_data;
  //tParam* p = callbacksSubGetInstance();
  const char* az;
  const char* el;
  double posoff[2];

  az = gtk_entry_get_text(GTK_ENTRY(p->entObsPosOffAZ));
  el = gtk_entry_get_text(GTK_ENTRY(p->entObsPosOffEL));

  if(az[0] == NULL || el[0] == NULL){
    return;
  }
  posoff[0] = atof(az);
  posoff[1] = atof(el);
  p->posoff0 = posoff[0];
  p->posoff1 = posoff[1];

  tkb32clFuncReqPosoff(posoff);
}


void
on_btnACUAZEL_clicked (GtkButton * button, gpointer user_data)
{
  
}


/*!
\breif SourceTracking指令を出力する
*/
void
on_btnObsSrc_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  char* obsFlag;
  char* raH;
  char* raM;
  char* raS;
  char* decH;
  char* decM;
  char* decS;
  int d1, m1, d2, m2;
  double s1, s2;
  int i;
  int ret;
  char tmp[256];
  int SourceFlag = 0;
  int Coordinate = 0;
  int Epoch = 0;
  char SourceName[256] = {'\0'};

  char* srctrk[] = {
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

  //! 天体フラグ取得
  obsFlag = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO (p->cmbObsFlag)->entry));
  if(obsFlag[0] == NULL){
    dialog_ok(p, "Please input Flag");
    return;
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
    raH = gtk_entry_get_text(GTK_ENTRY(p->entObsXDeg));
    raM = gtk_entry_get_text(GTK_ENTRY(p->entObsXMin));
    raS = gtk_entry_get_text(GTK_ENTRY(p->entObsXSec));
    decH = gtk_entry_get_text(GTK_ENTRY(p->entObsYDeg));
    decM = gtk_entry_get_text(GTK_ENTRY(p->entObsYMin));
    decS = gtk_entry_get_text(GTK_ENTRY(p->entObsYSec));
    if(raH[0] == NULL || raM[0] == NULL || raS[0] == NULL
      || decH[0] == NULL || decM[0] == NULL || decS[0] == NULL){
        dialog_ok(p, "Please input source X Y");
        return;
    }
    if(raH[0] != NULL){
      d1 = atoi(raH);
    }
    if(raM[0] != NULL){
      m1 = atoi(raM);
    }
    if(raS[0] != NULL){
      s1 = atof(raS);
    }
    if(decH[0] != NULL){
      d2 = atoi(decH);
    }
    if(decM[0] != NULL){
      m2 = atoi(decM);
    }
    if(decS[0] != NULL){
      s2 = atof(decS);
    }
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
    dialog_ok(p, "Reject Source Tracking");
  }
  callbacksSubViewParam(p);
}


void
on_btnACUDriveUnlock_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqDriveUnLock();
  }
}


void
on_btnACUPrgTrk_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqProgramTracking();
  }
}


void
on_btnACUStowUnlock_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqUnStow();
  }
}


void
on_btnACUStowLock_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqStow();
  }
}


void
on_btnACUStandby_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqACUStandby();
  }
}


void
on_btnACUDriveLock_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*) user_data;
  int ret;

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqDriveLock();
  }
}


void
on_btn1stIFGHz_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*)user_data;
  char* tmp;
  double freq;
  int ret;

  tmp = gtk_entry_get_text(GTK_ENTRY(p->ent1stIFGHz));
  if(tmp[0] == '\0'){
    return;
  }
  freq = atof(tmp);

  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetLo1Freq(freq);
  }
}


void
on_btn1stIFdBm_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*)user_data;
  char* tmp;
  double amp;
  int ret;

  tmp = gtk_entry_get_text(GTK_ENTRY(p->ent1stIFdBm));
  if(tmp[0] == '\0'){
    return;
  }
  amp = atof(tmp);
  
  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetLo1Amp(amp);
  }
}

void
on_btn1stIFdB_clicked (GtkButton * button, gpointer user_data)
{
  tParam* p = (tParam*)user_data;
  char* tmp;
  double att;
  int ret;
  char msg[256];

  tmp = gtk_entry_get_text(GTK_ENTRY(p->ent1stIFdB));
  if(tmp[0] == '\0'){
    return;
  }
  att = atof(tmp);

  if(att > 0){
    sprintf(msg, "Invalid value ([%d] >0)", att);
    dialog_ok(p, msg);
    return;
  }
  
  ret = dialog_check(p, "Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetLo1Att(att);
  }
}
