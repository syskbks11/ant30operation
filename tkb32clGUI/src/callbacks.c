#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../libtkb/export/libtkb.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "callbacksSub.h"
#include "tkb32clGUIParam.h"
const char dir[] = "../etc";

static int _setParameterCore(const char* grpName, const char* obsrName, const char* obsFile, char* msg);//090803 in
static int _sourceTrackingCore(const char* obsFlag, const char* raH, const char* raM, const char* raS, const char* decH, const char* decM, const char* decS, char* msg);//090803 in


void
on_btnSetParameter_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  const gchar* grpName;
  const gchar* obsrName;
  const gchar* obsFile;
  char tmp[1024];
  char msg[1024];
  int i;
  int ret;

	ret = _setParameterCore(gtk_entry_get_text(GTK_ENTRY(p->entGroupName)), gtk_entry_get_text(GTK_ENTRY(p->entProjectName)), gtk_entry_get_text(GTK_ENTRY(p->entObsFileName)), tmp);//090803 in
	if(ret){
		dialog_ok(tmp);
		if(ret != 8){
			return;
		}
	}
/* 090803 out
  confInit();
  sprintf(tmp, "%s/tkb32cl.conf", dir);
  if(confAddFile(tmp)){
    /* throw "";
  }
  for(i = 1; i < p->argc; i++){
    if(confAddFile(p->argv[i])){
      /* throw "";
    }
  }
  grpName = gtk_entry_get_text(GTK_ENTRY(p->entGroupName));
  obsrName = gtk_entry_get_text(GTK_ENTRY(p->entProjectName));
  obsFile = gtk_entry_get_text(GTK_ENTRY(p->entObsFileName));
  if(grpName[0] == '\0'){
    dialog_ok("Please input Group Name.");
    return;
  }
  if(obsrName[0] == '\0'){
    dialog_ok("Please input Project Name.");
    return;
  }
  if(obsFile[0] == '\0'){
    dialog_ok("Please input obsFile Name.");
    return;
  }

  //sprintf(tmp, "%s/%s.obs", dir, obsFile);
  sprintf(tmp, "%s/%s/%s/%s.obs", dir, grpName, obsrName, obsFile);
  if(confAddFile(tmp)){
    sprintf(msg, "Not found obsTable(%s)", tmp);
    dialog_ok(msg);
    return;
  }
  sprintf(tmp, "%s/%s/%s/%s", dir, grpName, obsrName, confGetStr("DeviceTable"));
  if(confAddFile(tmp)){
    printf("init Not found DeviceTable(%s)", tmp);
    dialog_ok("Device table not found.");
    return;
    /* throw "";
  }
  sprintf(tmp, "%s/%s/%s/%s", dir, grpName, obsrName, confGetStr("SourceTable"));
  if(confAddFile(tmp)){
    printf("init Not found SourceTable(%s)", tmp);
    dialog_ok("Source table not found.");
    return;
    /* throw "";
  }
  sprintf(tmp, "%s/%s/%s/%s", dir, grpName, obsrName, confGetStr("ScanTable"));
  if(confAddFile(tmp)){
    printf("init Not found ScanTable(%s)", tmp);
    dialog_ok("Scan table not found.");
    return;
    /* throw "";
  }
  //! obsTable����ǉ�
  sprintf(tmp, "ObsTable %s.obs", obsFile);
  confAddStr(tmp);

  //! �p�����[�^��\�����L�^
  printf("Initialize Parameters.");
  confPrint();

  ret = tkb32clFuncReqInit();
  if(ret){
    dialog_ok("Reject init");
  }
*/
  callbacksSubViewParam(p);
}

int _setParameterCore(const char* grpName, const char* obsrName, const char* obsFile, char* msg){
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
  if(confAddFile(tmp)){
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
  //! obsTable����ǉ�
  sprintf(tmp, "ObsTable %s.obs", obsFile);
  confAddStr(tmp);

  //! �p�����[�^��\�����L�^
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

void
on_btnStartObs_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  callbacksSubDrawMap(NULL);
  ret = tkb32clFuncReqStartObs();
  if(ret){
    dialog_ok("Reject startObs");
  }
}


void
on_btnStopObs_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = tkb32clFuncReqStopObs();
  if(ret){
    dialog_ok("Reject stopObs");
  }
}

void
on_btnEnd_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = tkb32clFuncReqEnd();
  if(ret){
    dialog_ok("Reject end");
  }
}

void
on_btnViewParameter_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  gtk_widget_show (p->winParameterList);
}

void
on_btnViewAllStatus_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  gtk_widget_show(p->winAllStatus);
}

void
on_btnChopperOpen_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqRopen();
  }
}


void
on_btnChopperClose_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqRclose();
  }
}


void
on_btnChopperOrigin_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqRorigine();
  }
}


void
on_btn2ndIFGHz_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*)user_data;
  const gchar* tmp;
  double freq;
  int ret;

  tmp = gtk_entry_get_text(GTK_ENTRY(p->ent2ndIFGHz));
  if(tmp[0] == '\0'){
    return;
  }
  freq = atof(tmp);
  
  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetLo2Freq(freq);
  }
}


void
on_btn2ndIFdBm_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*)user_data;
  const gchar* tmp;
  double amp;
  int ret;

  tmp = gtk_entry_get_text(GTK_ENTRY(p->ent2ndIFdBm));
  if(tmp[0] == '\0'){
    return;
  }
  amp = atof(tmp);

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetLo2Amp(amp);
  }
}


void
on_btnSASec_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*)user_data;
  const gchar* tmp;
  double t;
  int ret;

  tmp = gtk_entry_get_text(GTK_ENTRY(p->entSASec));
  if(tmp[0] == '\0'){
    return;
  }
  t = atof(tmp);

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetSaIntegTime(t);
  }
}


void
on_btnSAStartR_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*)user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSaStart(2);//2:R
  }
}

void
on_btnSAStartOn_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*)user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSaStart(0);//0:On
  }
}

void
on_btnSAStartOff_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*)user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSaStart(1);//1:Off
  }
}

void
on_btnACUPosOff_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*)user_data;
  //tParamGUI* p = callbacksSubGetInstance();
  const gchar* az;
  const gchar* el;
  double posoff[2];

  az = gtk_entry_get_text(GTK_ENTRY(p->entObsPosOffAZ));
  el = gtk_entry_get_text(GTK_ENTRY(p->entObsPosOffEL));

  if(az[0] == '\0' || el[0] == '\0'){
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
\breif SourceTracking�w�߂��o�͂���
*/
void
on_btnObsSrc_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  const gchar* obsFlag;
  const gchar* raH;
  const gchar* raM;
  const gchar* raS;
  const gchar* decH;
  const gchar* decM;
  const gchar* decS;


  int ret;
  char tmp[256];

	ret = _sourceTrackingCore(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO (p->cmbObsFlag)->entry)), 
		gtk_entry_get_text(GTK_ENTRY(p->entObsXDeg)), 
		gtk_entry_get_text(GTK_ENTRY(p->entObsXMin)), 
		gtk_entry_get_text(GTK_ENTRY(p->entObsXSec)), 
		gtk_entry_get_text(GTK_ENTRY(p->entObsYDeg)), 
		gtk_entry_get_text(GTK_ENTRY(p->entObsYMin)), 
		gtk_entry_get_text(GTK_ENTRY(p->entObsYSec)), tmp);//090803 in
	if(ret){//090803 in
		dialog_ok(tmp);
		if(ret != 3){
			return;
		}
	}

/* 090803 moved to _sourceTrackingCore()
  int d1, m1, d2, m2;
  double s1, s2;
  int i;
  int SourceFlag = 0;
  int Coordinate = 0;
  int Epoch = 0;
  char SourceName[256] = {'\0'};

  char* srctrk[] = {
    "ObsTable srctrk.obs",
    "SourceName	srctrk",
    "ScanFlag	1001",          //!< 1001:SourceTracking
    //"SourceFlag	0",         //!< 0:���z�n�O 1~12:���z�n�f�� 99:Commet
    //"Coordinate	1",         //!< 1:RADEC 2:LB 3:AZEL
    //"Epoch	2",             //!< 1:B1950 2:J2000
    NULL
  };

  confInit();
  sprintf(tmp, "%s/tkb32cl.conf", dir);
  if(confAddFile(tmp)){
    /* throw "";
  }
  //! �_�~�[�̃p�����[�^��o�^
  for(i = 0; srctrk[i] != NULL; i++){
    confAddStr(srctrk[i]);
  }

  //! �V�̃t���O�擾
  obsFlag = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO (p->cmbObsFlag)->entry));
  if(obsFlag[0] == '\0'){
    dialog_ok("Please input Flag");
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

  //! ���W�擾
  d1 = m1 = s1 = 0;
  d2 = m2 = s2 = 0;
  if(SourceFlag == 0 || SourceFlag == 99){
    raH = gtk_entry_get_text(GTK_ENTRY(p->entObsXDeg));
    raM = gtk_entry_get_text(GTK_ENTRY(p->entObsXMin));
    raS = gtk_entry_get_text(GTK_ENTRY(p->entObsXSec));
    decH = gtk_entry_get_text(GTK_ENTRY(p->entObsYDeg));
    decM = gtk_entry_get_text(GTK_ENTRY(p->entObsYMin));
    decS = gtk_entry_get_text(GTK_ENTRY(p->entObsYSec));
    if(raH[0] == '\0' || raM[0] == '\0' || raS[0] == '\0'
      || decH[0] == '\0' || decM[0] == '\0' || decS[0] == '\0'){
        dialog_ok("Please input source X Y");
        return;
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

  //! �p�����[�^��\�����L�^
  confPrint();

  ret = tkb32clFuncReqInit();
  if(ret){
    dialog_ok("Reject Source Tracking");
  }
*/
  callbacksSubViewParam(p);
}


int _sourceTrackingCore(const char* obsFlag, const char* raH, const char* raM, const char* raS, const char* decH, const char* decM, const char* decS, char* msg){
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
    //"SourceFlag	0",         //!< 0:���z�n�O 1~12:���z�n�f�� 99:Commet
    //"Coordinate	1",         //!< 1:RADEC 2:LB 3:AZEL
    //"Epoch	2",             //!< 1:B1950 2:J2000
    NULL
  };

  confInit();
  sprintf(tmp, "%s/tkb32cl.conf", dir);
  if(confAddFile(tmp)){
    /* throw "";*/
  }
  //! �_�~�[�̃p�����[�^��o�^
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

  //! ���W�擾
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

  //! �p�����[�^��\�����L�^
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


void
on_btnACUDriveUnlock_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqDriveUnLock();
  }
}


void
on_btnACUPrgTrk_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqProgramTracking();
  }
}


void
on_btnACUStowUnlock_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqUnStow();
  }
}


void
on_btnACUStowLock_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqStow();
  }
}


void
on_btnACUStandby_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqACUStandby();
  }
}


void
on_btnACUDriveLock_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*) user_data;
  int ret;

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqDriveLock();
  }
}


void
on_btn1stIFGHz_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*)user_data;
  const gchar* tmp;
  double freq;
  int ret;

  tmp = gtk_entry_get_text(GTK_ENTRY(p->ent1stIFGHz));
  if(tmp[0] == '\0'){
    return;
  }
  freq = atof(tmp);

  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetLo1Freq(freq);
  }
}


void
on_btn1stIFdBm_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*)user_data;
  const gchar* tmp;
  double amp;
  int ret;

  tmp = gtk_entry_get_text(GTK_ENTRY(p->ent1stIFdBm));
  if(tmp[0] == '\0'){
    return;
  }
  amp = atof(tmp);
  
  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetLo1Amp(amp);
  }
}

void
on_btn1stIFdB_clicked (GtkButton * button, gpointer user_data)
{
  tParamGUI* p = (tParamGUI*)user_data;
  const gchar* tmp;
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
    dialog_ok(msg);
    return;
  }
  
  ret = dialog_check("Do you really send the request ?");
  if(ret){
    tkb32clFuncReqSetLo1Att(att);
  }
}