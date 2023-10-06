/*!
¥file penguin_tracking.cpp
¥author NAGAI Makoto
¥date 2009.02.20
¥brief Tracking Calculation Program for 32-m telescope & 30-cm telescope
*/
#include <stdlib.h>//time_t
#include <memory.h>//memset()
#include <math.h>//M_PI

#include "trk45Sub.h"
#include "../libtkb/export/libtkb.h"
#include "glacier_celestialObject.h"
#include "glacier_trackingInformation.h"
#include "penguin_tracking.h"

#define SPARCE_LOG_FREQ 200
#define DEBUG 0

typedef struct {
	XTRKI xin;            //!< 45m追尾ライブラリ入力パラメーター構造体
	XTRKO xout;           //!< 45m追尾ライブラリ計算結果出力用構造体
	char cstart[24];      //!< 観測時刻の設定 YYYYMMDDhhmmss.0 (JST)
	tmClass_t* vtm;		//!< trk_00()更新時間管理用

	int isToTrk00;		//!< 2: trk_00()を呼ぶべし。 1: JST 09時台だったらtrk_00()を呼ぶべし。0: 呼ばなくていい。

	int sparceLogCounter;	//!< To output results once per SPARCE_LOG_FREQ

	int currentLock;	//!< to be thread-safe
	int queueLast;	//!< to be thread-safe
}tPenguinTracking;

typedef struct{
	double localSideralTime;//[rad]
}tWhatWeWant;

static tPenguinTracking p;

static char envAnt[128];
static char envEph[128];
static char envTime[128];

//static const double sec2rad = M_PI / (180.0 * 3600.0);
//static const double rad2sec = 180.0 * 3600.0 / M_PI;

static void _calcSetXinDscn(int coord, double dscn0, double dscn1);
static void _calcSetXinDscn4(int coord, double dscnst0, double dscnst1, double dscned0, double dscned1);//090206 in

static void _calcSetXinIFlags(int irpflg, int iapflg, int irsflg, int iasflg);
static void _calcSetXinIScn(int itnscn, int inoscn);
static void _calcSetXinIVFlags(int ivdef, int ivref);

static void _calcSetXinDpof(int coord, double x, double y);


static int _setObject(glacier_CelestialObjectClass_t* object, int calculationMode);

static int _setTime(char* timeString);
static int _setEnvironmentForTrk45();

static void _standInLineOfQueue();
static void _unLock();

/* 090604 out
static int checkParam();
*/

/*
* int penguin_tracking_Init()
* Invoke this function at first.
*/
int penguin_tracking_Init(){

	memset(&p.xout, 0, sizeof(p.xout));
	memset(p.cstart, 0, sizeof(p.cstart[0]) * 24);
	memset(&p.xin, 0, sizeof(p.xin));
	p.sparceLogCounter = 0;//090604 in

	p.currentLock = 1;//090605 in
	p.queueLast = 0;//090605 in
	p.isToTrk00 = 2;//090619 in

	p.vtm = tmInit();//081117 in

	return 0;
}
/*
 * void penguin_tracking_end()
 * Invoke this function at last.
 */
void penguin_tracking_end(){
	if(p.vtm){
		tmEnd(p.vtm);
		p.vtm = NULL;
	}
}


/*
 * Load configuration with "conf" of libtkb.
 * This function will initialize "conf", and then load configuration file "../etc/penguin_tracking.conf". 
 */
int penguin_tracking_loadConfigWithConfInit(){
	confInit();
	confAddFile("../etc/penguin_tracking.conf");
	confPrint();
	return penguin_tracking_loadConfig();
}

/*
 * Load configuration with "conf" of libtkb.
 * "conf" must have been initialized. 
 */
int penguin_tracking_loadConfig(){
	return  _setEnvironmentForTrk45();
}

/*
* Set the time in JST.
*/
int penguin_tracking_setTime(int year, int month, int day, int hour, int minute, int second, double microSecond){
	char timeString[24];
	sprintf(timeString, "%04d%02d%02d%02d%02d%02d.%06.0lf", year, month, day, hour, minute, second, microSecond);
	if(_setTime(timeString)){
		return 1;
	}
	return 0;//normal end
}

/*
 * Set the time to the current machine time.
 */
int penguin_tracking_setCurrentTime(double microSecondOffset){
	tmReset(p.vtm);

	char timeString[24];

	strcpy(timeString, tmGetTimeStr(p.vtm, microSecondOffset*0.000001));
	if(_setTime(timeString)){

		return 1;
	}
	return 0;//normal end
}

int _setTime(char* timeString){
	char cerr[256];
//	printf("_setTime(): %s\n", timeString);fflush(stdout);
	if(p.sparceLogCounter % SPARCE_LOG_FREQ == 0){//090604 iin
		uM1("_setTime(): %s", timeString);
	}
	memcpy(p.cstart, timeString, 24);

	if(p.isToTrk00 == 2){//for the first time 
		uM("NOTICE: I'll invoke trk_00(), which costs about 90 ms.");
		if(trk_00(timeString, "", cerr) != 0) {
			uM2("_setTime(); trk_00(%s) %s", p.cstart, cerr);
			return 1;
		}
		p.isToTrk00 = 1;
		return 0;
	}
	// the second time or later
	if( p.cstart[8] != '0' || p.cstart[9] != '9' ){         //!< JST時刻との比較 JSTが09時でないの意
		p.isToTrk00 = 1;
		return 0;//normal end
	}
	if(!p.isToTrk00){
		return 0;//normal end
	}
	// isToTrk00 == 1
	uM("NOTICE: I'll invoke trk_00(), which costs about 90 ms.");
	if(trk_00(timeString, "", cerr) != 0) {
		uM2("_setTime(); trk_00(%s) %s", p.cstart, cerr);
		return 1;
	}
	p.isToTrk00 = 0;
	return 0;//normal end
}

/*
 * Set an object to calculate its position. 
 */
int penguin_tracking_setObject(glacier_CelestialObjectClass_t* object){
	return _setObject(object, 0);//mode 0 for position.
/*
	int ival = trk_10(glacier_CelestialObjectGetSource(object), 0, 
			glacier_CelestialObjectGetCoordinate(object), 
			glacier_CelestialObjectGetEpoch(object), 
			glacier_CelestialObjectGetXY(object), 
			glacier_CelestialObjectGetVelocity(object));
	if (ival == 1) {
		printf("Invalid value of object.\n");
		return 1;
	}
	else if (ival == 2){
		printf("trk45 library has some trouble.\n");
		return 2;
	}
	return 0;
*/
}
/*
 * Set an object to calculate its radial velocity. 
 */
int penguin_tracking_setObjectForVelocity(glacier_CelestialObjectClass_t* object){
	_calcSetXinIVFlags(glacier_CelestialObjectGetDefinition(object), glacier_CelestialObjectGetFrame(object));
	return _setObject(object, 3);//mode 3 for velocity.
/*
	int ival = trk_10(glacier_CelestialObjectGetSource(object), 0, 
			glacier_CelestialObjectGetCoordinate(object), 
			glacier_CelestialObjectGetEpoch(object), 
			glacier_CelestialObjectGetXY(object), 
			glacier_CelestialObjectGetVelocity(object));
	if (ival == 1) {
		printf("Invalid value of object.\n");
		return 1;
	}
	else if (ival == 2){
		printf("trk45 library has some trouble.\n");
		return 2;
	}
	return 0;
*/
}

int _setObject(glacier_CelestialObjectClass_t* object, int calculationMode){
	int ival = trk_10(glacier_CelestialObjectGetSource(object), calculationMode, 
			glacier_CelestialObjectGetCoordinate(object), 
			glacier_CelestialObjectGetEpoch(object), 
			glacier_CelestialObjectGetXY(object), 
			glacier_CelestialObjectGetVelocity(object));
	if (ival == 1) {
		printf("Invalid value of object.\n");
		return 1;
	}
	else if (ival == 2){
		printf("trk45 library has some trouble.\n");
		return 2;
	}
	return 0;
}

/*! ¥fn int penguin_tracking_setScanOffset(glacier_ScanOffsetClass_t* offset)
¥param[in] offset
*/
int penguin_tracking_setScanOffset(glacier_ScanOffsetClass_t* offset){
	_calcSetXinDscn(glacier_ScanOffsetGetCoordinate(offset), glacier_ScanOffsetGetX(offset), glacier_ScanOffsetGetY(offset));
	return 0;//normal end
}

/*! ¥fn int penguin_tracking_setPointingOffset(glacier_ScanOffsetClass_t* offset)
¥param[in] offset
*/
int penguin_tracking_setPointingOffset(glacier_ScanOffsetClass_t* offset){
	_calcSetXinDpof(glacier_ScanOffsetGetCoordinate(offset), glacier_ScanOffsetGetX(offset), glacier_ScanOffsetGetY(offset));
	return 0;//normal end
}

/*! ¥fn void calcSetXinDpof(int coord, double dpofst0, double dpofst1, double dpofaz0, double dpofaz1)
¥brief xin 構造体のsetter. 
¥param[in] coord ポインティングオフセット座標の定義 1:RADEC 2:LB 3:AZEL
¥param[in] dscnst0 [rad]
¥param[in] dscnst1 [rad]
¥param[in] dscned0 [rad]
¥param[in] dscned1 [rad]
*/
void _calcSetXinDpof(int coord, double x, double y){
	if(DEBUG){//090605 in
		uM("_calcSetXinDpof() invoked");
	}
	_standInLineOfQueue();

	p.xin.iapflg = coord;
	/* I think the following is not expected usage of trk45. */
	/* But it's OK, since we use only coord == 3 */
	if(coord == 1 || coord == 2){
		p.xin.dpofst[0] = x;
		p.xin.dpofst[1] = y;
		p.xin.dpofaz[0] = 0;
		p.xin.dpofaz[1] = 0;
	}else if(coord == 3){
		p.xin.dpofst[0] = 0;
		p.xin.dpofst[1] = 0;
		p.xin.dpofaz[0] = x;
		p.xin.dpofaz[1] = y;
	}else{
		uM1("_calcSetXinDpof(); Invalid coord: %d", coord);
	}

	_unLock();
	if(DEBUG){//090605 in
		uM("_calcSetXinDpof() return");
	}
}


/*
 * Calculate.
 */
glacier_TrackingInformationClass_t* penguin_tracking_work(){
	if(DEBUG){//090605 in
		uM("penguin_tracking_work() invoked");
	}
	_standInLineOfQueue();

/* 090605 out
	_calcSetXinIFlags(1, 3, 1, 1);
	_calcSetXinIScn(1 ,1);
*/
	//! trk_20用変数
	double dtime;

	//! trk_20 (観測該当時刻の設定 YYYYMMDDhhmmss.0 (JST))
	if(tjcd2m(p.cstart, &dtime) == 1){  //!< JST時系の暦日付からMJDへの変換
		uM("tjcd2m: Error");
		return NULL;
	}
	int ival = trk_20(dtime, p.xin, &p.xout);
	if(ival == -1){
		uM("trk_20: Argument check error");
		return NULL;
	}
	else if (ival == -2) {
		uM("trk_20: Local error end");
		return NULL;
	}

	double result[14];
	double dgtloc;        //!< 瞬間の視恒星時 [rad]
	trk_ant_(result, result+2, result+4, result+6, result+8, result+10, result+12, result+14, &dgtloc);
	glacier_TrackingInformationClass_t* res = glacier_TrackingInformationInit(result, dgtloc, p.xout.dazel);

	if(p.sparceLogCounter % SPARCE_LOG_FREQ == 0){//090604 iin
		uM3("penguin_tracking_work(): LST=%lf, (Az, El)=(%lf, %lf)", dgtloc, p.xout.dazel[0], p.xout.dazel[1]);
		p.sparceLogCounter = 0;
	}
	p.sparceLogCounter++;//090604 in

	_unLock();
	if(DEBUG){//090605 in
		uM("penguin_tracking_work() return");
	}
	return res;
}
/*
 * velocity [m/s]
 */
double penguin_tracking_workForVelocity(){
	if(DEBUG){//090605 in
		uM("penguin_tracking_workForVelocity() invoked");
	}
	_standInLineOfQueue();

/* 090605 out
	_calcSetXinIFlags(1, 3, 1, 1);
	_calcSetXinIScn(1 ,1);
*/
	//! trk_20用変数
	double dtime;

	//! trk_20 (観測該当時刻の設定 YYYYMMDDhhmmss.0 (JST))
	if(tjcd2m(p.cstart, &dtime) == 1){  //!< JST時系の暦日付からMJDへの変換
		uM("tjcd2m: Error");
		return NULL;
	}
	int ival = trk_20(dtime, p.xin, &p.xout);
	if(ival == -1){
		uM("trk_20: Argument check error");
		return NULL;
	}
	else if (ival == -2) {
		uM("trk_20: Local error end");
		return NULL;
	}

	//! VRAD (アンテナから見た視線速度) の算出
	double res;
 	trk_velo_(&res);

	_unLock();
	if(DEBUG){//090605 in
		uM("penguin_tracking_workForVelocity() return");
	}
	return res;
}

/*
* [Hz]
*/
void penguin_tracking_setRestFrequencyForVelocity(double freq){
	if(DEBUG){//090605 in
		uM("penguin_tracking_setRestFrequencyForVelocity() invoked");
	}
	_standInLineOfQueue();

	p.xin.dobsfq = freq;

	_unLock();
	if(DEBUG){//090605 in
		uM("penguin_tracking_setRestFrequencyForVelocity() return");
	}
}

/*
* [Hz]
*/
double penguin_tracking_getDopplerFrequency(){
	return p.xout.dflkfq;
}

/*! ¥fn void _calcSetXinIFlags(int irpflg, int iapflg, int irsflg, int iasflg)
¥brief xin 構造体のsetter. 
¥param[in] irpflg ポインティング 0:虚角 1:実角
¥param[in] iapflg ポインティング座標定義初期値 1:RADEC 2:LB 3:AZEL
¥param[in] irsflg スキャンニング 0:虚角 1:実角
¥param[in] iasflg スキャンニング座標定義初期値 1:RADEC 2:LB 3:AZEL
*/
void _calcSetXinIFlags(int irpflg, int iapflg, int irsflg, int iasflg){
	if(DEBUG){//090605 in
		uM("_calcSetXinIFlags() invoked");
	}
	_standInLineOfQueue();

	p.xin.irpflg = irpflg;
	p.xin.iapflg = iapflg;
	p.xin.irsflg = irsflg;
	p.xin.iasflg = iasflg;

	_unLock();
	if(DEBUG){//090605 in
		uM("_calcSetXinIFlags() return");
	}
}

/*! ¥fn void _calcSetXinIScn(int itnscn, int inoscn)
¥brief xin 構造体のsetter. 
¥param[in] itnscn スキャンニング点数
¥param[in] inoscn スキャンニング点
*/
void _calcSetXinIScn(int itnscn, int inoscn){
	if(DEBUG){//090605 in
		uM("_calcSetXinIScn() invoked");
	}
	_standInLineOfQueue();

	p.xin.itnscn = itnscn;
	p.xin.inoscn = inoscn;

	_unLock();
	if(DEBUG){//090605 in
		uM("_calcSetXinIScn() return");
	}
}

void _calcSetXinIVFlags(int ivdef, int ivref){
	if(DEBUG){//090605 in
		uM("_calcSetXinIFlag() invoked");
	}
	_standInLineOfQueue();

	p.xin.ivdef = ivdef;
	p.xin.ivref = ivref;

	_unLock();
	if(DEBUG){//090605 in
		uM("_calcSetXinIFlag() return");
	}
}

/*! ¥fn void _calcSetXinDscn(int coord, double dscn0, double dscn1)
¥brief xin 構造体のsetter. 
¥param[in] coord スキャンオフセット座標の定義 1:RADEC 2:LB 3:AZEL
¥param[in] dscn0 [rad]
¥param[in] dscn1 [rad]
*/
void _calcSetXinDscn(int coord, double dscn0, double dscn1){
	_calcSetXinDscn4(coord, dscn0, dscn1, dscn0, dscn1);
}

/*! ¥fn void _calcSetXinDscn4(int coord, double dscnst0, double dscnst1, double dscned0, double dscned1)
¥brief xin 構造体のsetter. 
¥param[in] coord スキャンオフセット座標の定義 1:RADEC 2:LB 3:AZEL
¥param[in] dscnst0 [rad]
¥param[in] dscnst1 [rad]
¥param[in] dscned0 [rad]
¥param[in] dscned1 [rad]
*/
void _calcSetXinDscn4(int coord, double dscnst0, double dscnst1, double dscned0, double dscned1){
	if(DEBUG){//090605 in
		uM("_calcSetXinDscn4() invoked");
	}
	_standInLineOfQueue();

	p.xin.iasflg = coord;
	p.xin.dscnst[0] = dscnst0;
	p.xin.dscnst[1] = dscnst1;
	p.xin.dscned[0] = dscned0;
	p.xin.dscned[1] = dscned1;

	_unLock();
	if(DEBUG){//090605 in
		uM("_calcSetXinDscn4() return");
	}
}


/*! ¥fn int checkParam()
¥brief パラメーターのチェック
¥retval 0 成功
¥retval -1 失敗
*/
/* 090604 out
int checkParam(){
  return 0;
}
*/

/*
 * invoked by init()
 * thread 1, phase A0
 */
int _setEnvironmentForTrk45(){


	//check the parameters
	if(!confSetKey("EnvEph")){
		printf("_setEnvironmentForTrk45(); EnvEph error!!\n");
		return -1;
	}
	if(!confSetKey("EnvTime")){
		printf("_setEnvironmentForTrk45(); EnvTime error!!\n");
		return -1;
	}
	if(!confSetKey("EnvAnt")){
		printf("_setEnvironmentForTrk45(); EnvAnt error!!\n");
		return -1;
	}

	if(confSetKey("EnvAnt"))
		sprintf(envAnt, "ANTFILE=%s", confGetVal());
	if(confSetKey("EnvEph"))
		sprintf(envEph, "EPHFILE=%s", confGetVal());
	if(confSetKey("EnvTime"))
		sprintf(envTime, "TIMEFILE=%s", confGetVal());

	if(putenv(envAnt) != 0){
		printf("_setEnvironmentForTrk45(); Cannot add value to environnmet (%s)\n", envAnt);
		return 1;
	}
	if(putenv(envEph) != 0){
		printf("_setEnvironmentForTrk45(); Cannot add value to environnmet (%s)\n", envEph);
		return 1;
	}
	if(putenv(envTime) != 0){
		printf("_setEnvironmentForTrk45(); Cannot add value to environnmet (%s)\n", envTime);
		return 1;
	}

	//to initialize time string of trk45.
	//This has no relation with configure file, but this can be only here (after the putenv() and before setObject()).
	penguin_tracking_setCurrentTime(0);

	_calcSetXinIFlags(1, 3, 1, 1);//090605 in
	_calcSetXinIScn(1 ,1);//090605 in

	return 0;//normal end
}



//////////////// To be thread-safe... ////////////////

#define QUEUE_LOOP_NUMBER 4096
void _standInLineOfQueue(){
	if(p.queueLast == QUEUE_LOOP_NUMBER){
		p.queueLast = 1;
	}else{
		p.queueLast++;
	}
	const int queueID = p.queueLast;
	//printf("[%d]", queueID);

	//At the first invocation of _standInLineOfQueue(), p.currentLock is 1 and it goes immediately.
	while(p.currentLock != queueID){
		printf(".");
	}
	return;
}
void _unLock(){
	if(p.currentLock == QUEUE_LOOP_NUMBER){
		p.currentLock = 1;
	}else{
		p.currentLock++;
	}
}
