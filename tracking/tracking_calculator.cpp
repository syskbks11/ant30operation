/*!
¥file tracking_calculator.cpp
¥author NAGAI Makoto
¥date 2009.02.20
¥brief Tracking Calculation Program for 32-m telescope & 30-cm telescope
*/
#include <stdlib.h>//strtol()
#include <stdio.h>//printf()
#include <math.h>//M_PI
#include <unistd.h>//usleep()

#include "../libtkb/src/libtkb.h"

#include "penguin_tracking.h"
#include "glacier_celestialObject.h"
#include "glacier_trackingInformation.h"

static double toDegree(double x){
	return x * 180.0 / M_PI;
}
static double toRadian(double x){
	return x * M_PI / 180.0;
}

int allInteractiveMode(){
	char tmpstr[256];    // データ受信バッファ

	int res;//sscanf用


	// The time to calculate
	//default values.
	int year = 2009;
	int month = 2;
	int day = 24;
	int hour = 22;
	int minute =13;
	int second = 0;
	double microsecond = 0.0;


	printf("Enter year\t[yyyy; (%d)] > ", year);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%d",&year);
//	year = strtol(tmpstr, (char**)NULL, 10);
	
	printf("Enter month\t[MM; (%d)] > ", month);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%d",&month);
//	month = strtol(tmpstr, (char**)NULL, 10);
	
	printf("Enter day\t[dd; (%d)] > ", day);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%d",&day);
//	day = strtol(tmpstr, (char**)NULL, 10);
	
	printf("Enter hour\t[hh; (%d)] > ", hour);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%d",&hour);
//	hour = strtol(tmpstr, (char**)NULL, 10);
	
	printf("Enter minute\t[mm; (%d)] > ", minute);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%d",&minute);
//	minute = strtol(tmpstr, (char**)NULL, 10);
	
	printf("Enter second\t[ss; (%d)] > ", second);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%d",&second);
//	second = strtol(tmpstr, (char**)NULL, 10);
	
	printf("Enter mircosecond\t[ss.sss; (%lf)] > ", microsecond);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&microsecond);
//	microsecond = strtod(tmpstr, (char**)NULL);
	
	printf("year\t: %d\n", year);
	printf("month\t: %d\n", month);
	printf("day\t: %d\n", day);
	printf("hour\t: %d\n", hour);
	printf("minute\t: %d\n", minute);
	printf("second\t: %d\n", second);
	printf("microsecond\t: %lf\n", microsecond);

	penguin_tracking_setTime(year, month, day, hour, minute, second, microsecond);

	// The object to calculate
	//default values.
	int coordinate = 0;
	int object = 10;
	int epoch = 2;
	printf("Choose what you want [0:planet,moon,sun, 1:R.A.,Dec., 2:l,b, 3:Az,El; (%d)] > ", coordinate);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%d",&coordinate);
//	coordinate = strtol(tmpstr, (char**)NULL, 10);

	if(coordinate == CELESTIAL_OBJECT_COORDINATE_PLANET){
		printf("Choose planet,moon,sun [1:Mercury, 2:Venus, 4:Mars, 5:Jupiter, 6:Saturn, 7:Uranus, 8:Neptune, 9:Pluto, 10:Moon, 11:Sun ; (%d)] > ", object);
		fflush(stdout);
		fgets(tmpstr, 256, stdin);
		res = sscanf(tmpstr, "%d",&object);
//		object = strtol(tmpstr, (char**)NULL, 10);
	}else if(coordinate == CELESTIAL_OBJECT_COORDINATE_RADEC){
		object = CELESTIAL_OBJECT_SOURCE_OUT;
	}else if(coordinate == CELESTIAL_OBJECT_COORDINATE_RADEC){
		object = CELESTIAL_OBJECT_SOURCE_OUT;
	}else if(coordinate == CELESTIAL_OBJECT_COORDINATE_AZEL){
		object = CELESTIAL_OBJECT_SOURCE_AZEL;
	}else{
		printf("Invalid value (%d), exit.\n", coordinate);
		exit(1);
	}

	if(object == CELESTIAL_OBJECT_SOURCE_OUT){
		printf("Choose epoch [1:B1950 2:J2000; (%d)] > ", epoch);
		fflush(stdout);
		fgets(tmpstr, 256, stdin);
		res = sscanf(tmpstr, "%d",&epoch);
//		epoch = strtol(tmpstr, (char**)NULL, 10);
	}
	double x = 0;
	double y = 0;
	int offsetCoordinate = 1;
	double xOffset = 0;
	double yOffset = 0;
	if(coordinate != CELESTIAL_OBJECT_COORDINATE_PLANET){
		printf("Enter X [xxx.xx; (%d deg)] > ", x);
		fflush(stdout);
		fgets(tmpstr, 256, stdin);
		res = sscanf(tmpstr, "%lf",&x);
//		x = strtod(tmpstr, (char**)NULL);

		printf("Enter Y [+-yy.yy; (%d deg)] > ", y);
		fflush(stdout);
		fgets(tmpstr, 256, stdin);
		res = sscanf(tmpstr, "%lf",&y);
//		y = strtod(tmpstr, (char**)NULL);
	}
	printf("Enter coordinate of Offset [1:R.A.,Dec., 2:l,b, 3:Az,El; (%d)] > ", offsetCoordinate);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%d",&offsetCoordinate);
//	xOffset = strtod(tmpstr, (char**)NULL);

	printf("Enter X Offset [xxx.xx; (%d deg)] > ", xOffset);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&xOffset);
//	xOffset = strtod(tmpstr, (char**)NULL);

	printf("Enter Y Offset [+-yy.yy; (%d deg)] > ", yOffset);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&yOffset);
//	yOffset = strtod(tmpstr, (char**)NULL);


	printf("object\t: %d\n", object);
	printf("coordinate\t: %d\n", coordinate);
	printf("epoch\t: %d\n", epoch);
	printf("x\t: %lf\n", x);
	printf("y\t: %lf\n", y);
	printf("offset coordinate\t: %d\n", offsetCoordinate);
	printf("x offset\t: %lf\n", xOffset);
	printf("y offset\t: %lf\n", yOffset);

	double ddAz = 0;//[deg]
	double ddEl = 0;//[deg]
	printf("Enter ddAz of Pointing Offset [+-x.xxxx; (%d deg)] > ", ddAz);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&ddAz);

	printf("Enter ddEl of Pointing Offset [+-y.yyyy; (%d deg)] > ", ddEl);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&ddEl);

	glacier_CelestialObjectClass_t* celestialObject = glacier_CelestialObjectInit();
	glacier_CelestialObjectSetSource(celestialObject, object);
	glacier_CelestialObjectSetCoordinate(celestialObject, coordinate, x, y);
	glacier_CelestialObjectSetEpoch(celestialObject, epoch);

	penguin_tracking_setObject(celestialObject);

	glacier_ScanOffsetClass_t* scanOffset = glacier_ScanOffsetInit();
	glacier_ScanOffsetSetXY(scanOffset, offsetCoordinate, toRadian(xOffset), toRadian(yOffset));

	penguin_tracking_setScanOffset(scanOffset);

	glacier_ScanOffsetClass_t* pointingOffset = glacier_ScanOffsetInit();
	glacier_ScanOffsetSetXY(pointingOffset, CELESTIAL_SCAN_COORDINATE_AZEL, toRadian(ddAz), toRadian(ddEl));

	penguin_tracking_setPointingOffset(pointingOffset);



	glacier_TrackingInformationClass_t* information = penguin_tracking_work();

	uM1("LST: %lf", glacier_TrackingInformationGetLocalSideralTime(information));
	const double* r0 = glacier_TrackingInformationGet0(information);
	uM2("(longitude\t[deg], latitude\t[deg]): %lf, %lf", toDegree(r0[0]), toDegree(r0[1]));
	const double* r1 = glacier_TrackingInformationGet1(information);
	uM2("result1: %lf, %lf", toDegree(r1[0]), toDegree(r1[1]));
	const double* r2 = glacier_TrackingInformationGet2(information);
	uM2("result2: %lf, %lf", toDegree(r2[0]), toDegree(r2[1]));
	const double* r3 = glacier_TrackingInformationGet3(information);
	uM2("result3: %lf, %lf", toDegree(r3[0]), toDegree(r3[1]));
	const double* r4 = glacier_TrackingInformationGet4(information);
	uM2("(R.A.\t[deg], Dec.\t[deg]): %lf, %lf", toDegree(r4[0]), toDegree(r4[1]));
	const double* r5 = glacier_TrackingInformationGet5(information);
	uM2("(l\t[deg], b\t[deg]): %lf, %lf", toDegree(r5[0]), toDegree(r5[1]));
	const double* r6 = glacier_TrackingInformationGet6(information);
	uM2("(Az\t[deg], El\t[deg]): %lf, %lf", toDegree(r6[0]), toDegree(r6[1]));
	const double* r7 = glacier_TrackingInformationGet7(information);
	uM2("result7: %lf, %lf", toDegree(r7[0]), toDegree(r7[1]));
	const double* azel = glacier_TrackingInformationGetAzEl(information);
	uM2("(Az, El): %lf, %lf", toDegree(azel[0]), toDegree(azel[1]));

	glacier_CelestialObjectSetDefinition(celestialObject, CELESTIAL_OBJECT_DEEFINITION_RADIO);
	glacier_CelestialObjectSetFrame(celestialObject, CELESTIAL_OBJECT_FRAME_LSR);

	double rf = 1e9;
	printf("Enter tracking frequency\t[rf; (%e Hz)] > ", rf);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&rf);
//	rf = strtol(tmpstr, (char**)NULL, 10);
	penguin_tracking_setRestFrequencyForVelocity(rf);
	printf("rf\t: %lf\n", rf);

	penguin_tracking_setObjectForVelocity(celestialObject);
	double velocity = penguin_tracking_workForVelocity();
	uM1("velocity [m/s]: %lf", velocity);
	uM1("doppler-shifted frequency [Hz]: %lf", penguin_tracking_getDopplerFrequency());
	return 0;
}

int realtimeMode(){
	char tmpstr[256];    // データ受信バッファ
	int res;//sscanf用

	// The object to calculate
	//default values.
	int coordinate = 0;
	int object = 10;
	int epoch = 2;
	printf("Choose what you want [0:planet,moon,sun, 1:R.A.,Dec., 2:l,b, 3:Az,El; (%d)] > ", coordinate);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	coordinate = strtol(tmpstr, (char**)NULL, 10);

	if(coordinate == CELESTIAL_OBJECT_COORDINATE_PLANET){
		printf("Choose planet,moon,sun [1:Mercury, 2:Venus, 4:Mars, 5:Jupiter, 6:Saturn, 7:Uranus, 8:Neptune, 9:Pluto, 10:Moon, 11:Sun ; (%d)] > ", object);
		fflush(stdout);
		fgets(tmpstr, 256, stdin);
		object = strtol(tmpstr, (char**)NULL, 10);
	}else if(coordinate == CELESTIAL_OBJECT_COORDINATE_RADEC){
		object = CELESTIAL_OBJECT_SOURCE_OUT;
	}else if(coordinate == CELESTIAL_OBJECT_COORDINATE_RADEC){
		object = CELESTIAL_OBJECT_SOURCE_OUT;
	}else if(coordinate == CELESTIAL_OBJECT_COORDINATE_AZEL){
		object = CELESTIAL_OBJECT_SOURCE_AZEL;
	}else{
		printf("Invalid value (%d), exit.\n", coordinate);
		exit(1);
	}

	if(object == CELESTIAL_OBJECT_SOURCE_OUT){
		printf("Choose epoch [1:B1950 2:J2000; (%d)] > ", epoch);
		fflush(stdout);
		fgets(tmpstr, 256, stdin);
		epoch = strtol(tmpstr, (char**)NULL, 10);
	}
	double x = 0;//[deg]
	double y = 0;//[deg]
	int offsetCoordinate = 1;
	double xOffset = 0;
	double yOffset = 0;
	if(coordinate != CELESTIAL_OBJECT_COORDINATE_PLANET){
		printf("Enter X [xxx.xx; (%d deg)] > ", x);
		fflush(stdout);
		fgets(tmpstr, 256, stdin);
		res = sscanf(tmpstr, "%lf",&x);

		printf("Enter Y [+-yy.yy; (%d deg)] > ", y);
		fflush(stdout);
		fgets(tmpstr, 256, stdin);
		res = sscanf(tmpstr, "%lf",&y);
	}

	printf("Enter coordinate of Offset [1:R.A.,Dec., 2:l,b, 3:Az,El; (%d)] > ", offsetCoordinate);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%d",&offsetCoordinate);

	printf("Enter X Offset [xxx.xx; (%d deg)] > ", xOffset);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&xOffset);

	printf("Enter Y Offset [+-yy.yy; (%d deg)] > ", yOffset);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&yOffset);

	double ddAz = 0;//[deg]
	double ddEl = 0;//[deg]
	printf("Enter ddAz of Pointing Offset [+-x.xxxx; (%d deg)] > ", ddAz);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&ddAz);

	printf("Enter ddEl of Pointing Offset [+-y.yyyy; (%d deg)] > ", ddEl);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&ddEl);


	printf("object\t: %d\n", object);
	printf("coordinate\t: %d\n", coordinate);
	printf("epoch\t: %d\n", epoch);
	printf("x\t: %lf\n", x);
	printf("y\t: %lf\n", y);
	printf("offset coordinate\t: %d\n", offsetCoordinate);
	printf("x offset\t: %lf\n", xOffset);
	printf("y offset\t: %lf\n", yOffset);
	printf("ddAz\t: %lf\n", ddAz);
	printf("ddEl\t: %lf\n", ddEl);

	glacier_CelestialObjectClass_t* celestialObject = glacier_CelestialObjectInit();
	glacier_CelestialObjectSetSource(celestialObject, object);
	glacier_CelestialObjectSetCoordinate(celestialObject, coordinate, x, y);
	glacier_CelestialObjectSetEpoch(celestialObject, epoch);

	double microsecondOffset = 0;
	printf("Enter offset\t[ss.sss[microsecond]; (%lf)] > ", microsecondOffset);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&microsecondOffset);
//	microsecondOffset = strtod(tmpstr, (char**)NULL);

	double secondInterval = 0;
	printf("Enter interval\t[ss.sss[second]; (%lf)] > ", secondInterval);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	res = sscanf(tmpstr, "%lf",&secondInterval);
//	secondInterval = strtod(tmpstr, (char**)NULL);

	//res = penguin_tracking_setCurrentTime(microsecondOffset);
	penguin_tracking_setObject(celestialObject);

	glacier_ScanOffsetClass_t* scanOffset = glacier_ScanOffsetInit();
	glacier_ScanOffsetSetXY(scanOffset, offsetCoordinate, toRadian(xOffset), toRadian(yOffset));

	penguin_tracking_setScanOffset(scanOffset);

	glacier_ScanOffsetClass_t* pointingOffset = glacier_ScanOffsetInit();
	glacier_ScanOffsetSetXY(pointingOffset, CELESTIAL_SCAN_COORDINATE_AZEL, toRadian(ddAz), toRadian(ddEl));

	penguin_tracking_setPointingOffset(pointingOffset);


	int i;
	for (i=0; i < 20; i++){
		res = penguin_tracking_setCurrentTime(microsecondOffset);
		if(res){
			uM1("penguin_tracking_setCurrentTime() returns %d.", res);
		}
		glacier_TrackingInformationClass_t* information = penguin_tracking_work();

		uM1("LST: %lf", glacier_TrackingInformationGetLocalSideralTime(information));
		const double* r0 = glacier_TrackingInformationGet0(information);
		uM2("(longitude\t[deg], latitude\t[deg]): %lf, %lf", toDegree(r0[0]), toDegree(r0[1]));
		const double* r1 = glacier_TrackingInformationGet1(information);
		uM2("result1: %lf, %lf", toDegree(r1[0]), toDegree(r1[1]));
		const double* r2 = glacier_TrackingInformationGet2(information);
		uM2("result2: %lf, %lf", toDegree(r2[0]), toDegree(r2[1]));
		const double* r3 = glacier_TrackingInformationGet3(information);
		uM2("result3: %lf, %lf", toDegree(r3[0]), toDegree(r3[1]));
		const double* r4 = glacier_TrackingInformationGet4(information);
		uM2("(R.A.\t[deg], Dec.\t[deg]): %lf, %lf", toDegree(r4[0]), toDegree(r4[1]));
		const double* r5 = glacier_TrackingInformationGet5(information);
		uM2("(l\t[deg], b\t[deg]): %lf, %lf", toDegree(r5[0]), toDegree(r5[1]));
		const double* r6 = glacier_TrackingInformationGet6(information);
		uM2("(Az\t[deg], El\t[deg]): %lf, %lf", toDegree(r6[0]), toDegree(r6[1]));
		const double* r7 = glacier_TrackingInformationGet7(information);
		uM2("result7: %lf, %lf", toDegree(r7[0]), toDegree(r7[1]));
		const double* azel = glacier_TrackingInformationGetAzEl(information);
		uM2("(Az, El): %lf, %lf", toDegree(azel[0]), toDegree(azel[1]));

		usleep(secondInterval*1e6);
	}
	return 0;
}

int MAIN__(int argc, char* argv[]){
	char tmpstr[256];    // データ受信バッファ


	uInit("../log/tracking_calculator/tracking_calculator");//090813 in
/* 090813 out
	uInit("../log/tracking_calculator");
*/
	int ret = penguin_tracking_Init();
	if(ret){
		printf("penguin_tracking_Init() failed, exit.\n");
		exit(1);
	}

	ret = penguin_tracking_loadConfigWithConfInit();
	if(ret){
		printf("penguin_tracking_loadConfigWithConfInit() failed, exit.\n");
		exit(1);
	}

	int mode =0;
	printf("Select mode\t[0:interactive, 1:realtime] > ", mode);
	fflush(stdout);
	fgets(tmpstr, 256, stdin);
	mode = strtol(tmpstr, (char**)NULL, 10);
	if(mode == 0){
		allInteractiveMode();
	}else{
		realtimeMode();
	}

	penguin_tracking_end();

	return 0;
}
