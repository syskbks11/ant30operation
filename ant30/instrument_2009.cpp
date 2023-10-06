/*!
¥file instrument.cpp
¥author NAGAI Makoto
¥date 2009.2.6
¥brief Manage Instrument parameter for 32-m telescope & 30-cm telescope
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "configuration.h"
#include "instrument.h"

#define PI M_PI
#define SPARCE_LOG_FREQ 1000

typedef struct {
	double P[30];         //!< アンテナ器差補正パラメータ used only in trkCalInst()
	int sparceLogCounter;	//!< To output results once per SPARCE_LOG_FREQ
	double rP[8];         //!< アンテナ器差補正パラメータ (radio pointing for 30-cm telescope)
	int rSparceLogCounter;	//!< To output results once per SPARCE_LOG_FREQ
}tParamInst;


static tParamInst p;



/* moved to header.
static void _calcCalInst(double X, double Y, double* dX, double* dY);
*/

static void setParam();


/*
* Invoked by calcInitParam()
* Thus, this function is excuted once for each initialization of observation tables. 
* thread 1, phase C0
*/
int instrumentInitParam(){

	memset(p.P, 0, sizeof(p.P[0]) * 30);

	setParam();

	return 0;
}



/*! ¥fn void trkCalInst(double X, double Y, double* dx, double* dy)
¥brief 器差補正を行うための関数。
¥param[in] X  raw Az deg
¥param[in] Y  raw El deg
¥param[out] dX (add to raw Az) deg
¥param[out] dY (add to raw El) deg
*/
void instrumentCalInst(double X, double Y, double* dX, double* dY){
	const double Phi = 90.0 * PI / 180.0;
  //double P[31] = {0};

  //memset(P, 0, 31*sizeof(P[0]));
  //! deg -> rad
	X *= PI / 180.0;
	Y *= PI / 180.0;

	const double cosX = cos(X);
	const double sinX = sin(X);
	const double cos2X = cos(2*X);
	const double sin2X = sin(2*X);
	const double tanY = tan(Y);
	const double cosY = cos(Y);
	const double sinY = sin(Y);
	const double Y39 = pow(Y,3.9);
	const double expY = exp(Y);
	printf(" Y = %lf, Y*180/PI = %lf, Y39 = %lf\n", Y, Y*180/PI, Y39);

//20100830 ishii 
	*dX = p.P[0] +p.P[1]*cosX*tanY + p.P[2]*sinX*tanY + p.P[3]*tanY
		+p.P[4]/cosY +p.P[7]*cosX +p.P[8]*sinX
		+p.P[11]*cosX*cosY +p.P[12]*cosX*sinY +p.P[13]*sinX*cosY +p.P[14]*Y;

	*dY = -p.P[1]*sinX +p.P[2]*cosX +p.P[5] +p.P[6]*cosY
		+p.P[9]*cosX + p.P[10]*sinX;

//20090225 in;  This is abnormal model!
//20100821 arai henkoushimashita!!
/*20100830 ishii out 
	*dX = (p.P[0] +p.P[1]*cosX*tanY + p.P[2]*sinX*tanY + p.P[3]*tanY
		+p.P[4]/cosY +p.P[7]*cosX +p.P[8]*sinX
		+p.P[11]*cosX*cosY +p.P[12]*cosX*sinY +p.P[13]*sinX*cosY +p.P[14]*Y +p.P[16]*Y39)/cosY +p.P[15];

	*dY = -p.P[1]*sinX +p.P[2]*cosX +p.P[5] +p.P[6]*cosY
		+p.P[9]*cosX + p.P[10]*sinX;
*/

/* 20090225 out
	*dX=p.P[0] - p.P[1]*cos(Phi)*tanY + p.P[2]*tanY - p.P[3]/cosY
	    + p.P[4]*sinX*tanY - p.P[5]*cosX*tanY
	    + p.P[11]*X + p.P[12]*cosX + p.P[13]*sinX + p.P[16]*cos2X + p.P[17]*sin2X;
	*dY=  p.P[4]*cosX + p.P[5]*sinX
	    + p.P[6] - p.P[7]*(cos(Phi)*sinY*cosX-sin(Phi)*cosY) + p.P[8]*Y
	    + p.P[9]*cosY + p.P[10]*sinY + p.P[14]*cos2X + p.P[15]*sin2X
	    + p.P[18]*cos(8*Y) + p.P[19]*sin(8*Y) + p.P[20]*cosX + p.P[21]*sinX;
*/
	if(p.sparceLogCounter % SPARCE_LOG_FREQ == 0){//090604 iin
		uM4("instrumentCalInst(); (Az, El)=(%lf, %lf) -> (dAz, dEl)=(%lf, %lf)", X, Y,  *dX, *dY);
		p.sparceLogCounter = 0;
	}
	p.sparceLogCounter++;//090604 in

	return;
}

/*! ¥fn void instrumentRadioModel(double X, double Y, double* dx, double* dy)
¥brief 器差補正を行うための関数。
¥param[in] X  raw Az deg
¥param[in] Y  raw El deg
¥param[out] dX (add to raw Az) deg
¥param[out] dY (add to raw El) deg
*/
void instrumentRadioModel(double X, double Y, double* dX, double* dY){//090918 in
	const double Phi = 90.0 * PI / 180.0;
	X *= PI / 180.0;
	Y *= PI / 180.0;

	const double dXY = X+Y;//091114 in, for 30-cm type offset-Casegren
/* 091114 out, for 18-cm type offset-Casegren
	const double dXY = X-Y;
*/
	const double cosDXY = cos(dXY);
	const double sinDXY = sin(dXY);

	//090918 in; Iwata master thesis.
	*dX = p.rP[0]*sinDXY + p.rP[1]*cosDXY + p.rP[2];

	*dY = p.rP[0]*cosDXY + p.rP[1]*sinDXY + p.rP[3];

	if(p.rSparceLogCounter % SPARCE_LOG_FREQ == 0){//090604 iin
		uM4("instrumentRadioModel(); (Az, El)=(%lf, %lf) -> (dAz, dEl)=(%lf, %lf)", X, Y,  *dX, *dY);
		p.rSparceLogCounter = 0;
	}
	p.rSparceLogCounter++;
	return;
}


/*! ¥fn void setParam()
¥brief パラメーター情報を取得する
* thread 1, phase C0
*/
void setParam(){


	char tmp[16];
	const char *antInst[6];

	memset(antInst, 0, sizeof(antInst[0]) * 6);
	for(int i = 0; i < 6; i++){
		sprintf(tmp, "AntInst%d", i+1);
		if(confSetKey(tmp))
			antInst[i] = confGetVal();
	}
	for(int i = 0; i < 6; i++){
		int j = i * 5;
		if(antInst[i] == NULL){
			uM1("setParam(); I found NO AntInst%d in ant30.conf, are you sure?? ", i+1);
			continue;
		}
		if(sscanf(antInst[i], "%lf %lf %lf %lf %lf",
			&p.P[j], &p.P[j+1], &p.P[j+2], &p.P[j+3], &p.P[j+4]) != 5){
			uM2("setParam(); antInst[%d] (%s) error!!", i+1, antInst[i+1]);
		}
	}
	//090918 in, for radio model.
	memset(antInst, 0, sizeof(antInst[0]) * 1);
	sprintf(tmp, "AntRadioInst");
	if(confSetKey(tmp)){
		antInst[0] = confGetVal();
		if(sscanf(antInst[0], "%lf %lf %lf %lf",
			&p.rP[0], &p.rP[1], &p.rP[2], &p.rP[3]) != 4){
			uM1("setParam(); AntRadioInst (%s) error!!", antInst[0]);
		}
	}else{
		uM("setParam(); I found NO AntRadioInst in ant30.conf, are you sure?? ");
	}
/* 090920 out
	for(int i = 0; i < 7; i++){
		sprintf(tmp, "AntInst%d", i);
		if(confSetKey(tmp))
			antInst[i] = confGetVal();
	}
	for(int i = 0; i < 6; i++){
		int j = i * 5;
		if(sscanf(antInst[i+1], "%lf %lf %lf %lf %lf",
			&p.P[j], &p.P[j+1], &p.P[j+2], &p.P[j+3], &p.P[j+4]) != 5){
			uM2("setParam(); antInst[%d] (%s) error!!", i+1, antInst[i+1]);
		}
	}
*/
}

