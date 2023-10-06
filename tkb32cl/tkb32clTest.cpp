/*!
\file tkb32clTest.cpp
\author NAGAI Makoto
\date 2009.06.16
\brief Tracking Calculation Program for 32-m telescope & 30-cm telescope
*/
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>//It's necessity of libtkb.h
#include <time.h>
//#include <curses.h>
#include <unistd.h>//usleep()

#include "../libtkb/export/libtkb.h"
#include "tkb32clFunc.h"
#include "tkb32clObservationFunc.h"
#include "errno.h"
#include "schedule.h"

//#define gotoxy(x,y) printf("\x1b[%d;%dH",(y+1),(x+1))

static const double ABS_ZERO= 273.15;
static const char dir[] = "../etc";



typedef struct tkb32clTest_s {
	char group[32];
	char project[32];
	char filename[32];
	char source[32];//source name (mode = 0), schedule name (mode = 2)
	int mode;//0: source tracking, 1: table, 2: schedule
	FILE* ttw;

	/* Auto */
	thrdClass_t* thrd;//090812 in
	int status;//0: not started, 1: the table initialized, 2: the table started, 3: the table end, 4: the table stopped abnormally
	int isRunning;//0: not running, 1: running
	const tAnsState* ansState;
}tkb32clTest_t;

static tkb32clTest_t p;

static void showStatus();
static int updateStatus();
static void showHelp();
static void showHelpMonitor();
static int manageSetCommand(char* cmd);
static void* scheduleMode(void* p);

void showHelp(){
	//gotoxy(0, 32);
	fprintf(p.ttw, "%s\r\n", "This is help of \"test\". ");
	fprintf(p.ttw, "%s\r\n", "Available commands: ");
	fprintf(p.ttw, "%s\r\n", "(program)");
	fprintf(p.ttw, "%s\r\n", "\thelp: ");
	fprintf(p.ttw, "%s\r\n", "\t\tShow this document. ");
	fprintf(p.ttw, "%s\r\n", "\tq: ");
	fprintf(p.ttw, "%s\r\n", "\t\tstop the program. . ");
	fprintf(p.ttw, "%s\r\n", "(monitor mode)");
	fprintf(p.ttw, "%s\r\n", "\tm: ");
	fprintf(p.ttw, "%s\r\n", "\t\tShift to monitor mode. ");
	fprintf(p.ttw, "%s\r\n", "(Observation Table)");
	fprintf(p.ttw, "%s\r\n", "\tset g[roup]: ");
	fprintf(p.ttw, "%s\r\n", "\t\tSet group. ");
	fprintf(p.ttw, "%s\r\n", "\tset o[bserver]: ");
	fprintf(p.ttw, "%s\r\n", "\t\tSet observer. ");
	fprintf(p.ttw, "%s\r\n", "\tset t[able]: ");
	fprintf(p.ttw, "%s\r\n", "\t\tSet table. ");
	fprintf(p.ttw, "%s\r\n", "\tset so[urce]: ");
	fprintf(p.ttw, "%s\r\n", "\t\tSet source. ");
	fprintf(p.ttw, "%s\r\n", "\tset sc[hedule]: ");
	fprintf(p.ttw, "%s\r\n", "\t\tSet schedule. ");
	fprintf(p.ttw, "%s\r\n", "(Excecute Observation)");
	fprintf(p.ttw, "%s\r\n", "\ti[nit]: ");
	fprintf(p.ttw, "%s\r\n", "\t\tInitialize the observation table. ");
	fprintf(p.ttw, "%s\r\n", "\tsta[rt]: ");
	fprintf(p.ttw, "%s\r\n", "\t\tStart observation. ");
	fprintf(p.ttw, "%s\r\n", "\tsto[p]: ");
	fprintf(p.ttw, "%s\r\n", "\t\tStop observation. ");
	fprintf(p.ttw, "%s\r\n", "\te[nd]: ");
	fprintf(p.ttw, "%s\r\n", "\t\tEnd observation. ");
	fflush(p.ttw);
}
void showHelpMonitor(){
//	gotoxy(0, 32);
	fprintf(p.ttw, "%s\r\n", "Available commands in monitor mode: ");
	fprintf(p.ttw, "%s\r\n", "c: command mode. ");
	fflush(p.ttw);
}

int manageCommand(char* cmd){
	long pos;
	char *token;
	int ret;
	char msg[256];

	/* 入力文字列を表示 */
	//gotoxy(5, 6);
//	gotoxy(0, 31);
	uM1("cmd: %s", cmd);
	switch(cmd[0]){
	case 'h':
		showHelp();
		break;
	case 'm':
		return 2;//monitor mode.
	case 'q':
		return 0;//stop the program.
	case 's':
		switch(cmd[1]){
		case 't':
			switch(cmd[2]){
			case 'a'://start
				if(p.mode == 2){
					if(p.isRunning){
						printf("AUTO observation is running. \n", ret);
						break;
					}
					printf("Schedule mode. AUTO observation. \n", ret);
					p.thrd = thrdInit(scheduleMode, (void*)&p);
					if(!p.thrd){
						uM("thrdInit(); error");
						break;
					}
					p.isRunning = 1;
					thrdStart(p.thrd);
				}else{
					ret=tkb32clFuncReqStartObs();
					if(ret){
						printf("tkb32clFuncReqStartObs() = %d\n", ret);
					}
				}
				break;
			case 'o'://stop
				if(p.mode == 2){
					if(!p.isRunning){
						printf("AUTO observation is not running. \n", ret);
						break;
					}
					printf("Schedule mode. AUTO observation is stopped. \n", ret);
					p.isRunning = 0;
					thrdStop(p.thrd);
				}else{
					ret=tkb32clFuncReqStopObs();
					if(ret){
						printf("tkb32clFuncReqStopObs() = %d\n", ret);
					}
				}
				break;
			default:
				break;	
			}
			break;
		case 'e':
			if(cmd[2] == 't' && cmd[3] == ' '){//set
				manageSetCommand(cmd);
			}
			break;
		default:
			break;	
		}
		break;
	case 'i':
		if(p.mode == 1){//table
			ret = tkb32clObservationFuncSetTable(dir, p.group, p.project, p.filename, msg);
			if(ret){
				printf("tkb32clObservationFuncSetTable() = %d: %s\n", ret, msg);
			}
		}else if(p.mode == 2){//schedule
		}else{//source tracking
			ret = tkb32clObservationFuncSetSource(dir, p.source, NULL, NULL, NULL, NULL, NULL, NULL, msg);
			if(ret){
				printf("tkb32clObservationFuncSetSource() = %d: %s\n", ret, msg);
			}
		}
/*
		ret=tkb32clFuncReqInit();
		if(ret){
			printf("tkb32clFuncReqInit() = %d\n", ret);
		}
*/
		break;
	case 'e':
		if(p.mode == 2){
			if(p.isRunning){
				printf("Stop AUTO observation. \n", ret);
				p.isRunning = 0;
				thrdStop(p.thrd);
			}
			if(p.thrd){
				thrdEnd(p.thrd);
			}
			printf("Exit schedule mode. MANUAL observation. \n", ret);
			p.mode = 1;
		}else{
			ret=tkb32clFuncReqEnd();
			if(ret){
				printf("tkb32clFuncReqEnd() = %d\n", ret);
			}
		}
		break;
	default:
		updateStatus();
		showStatus();
		break;	
	}
	return 1;//continue the  program in command mode.
}
int manageSetCommand(char* cmd){
	char tmp1[32];
	char tmp2[32];
	char msg[256];
	int ret;
	//cmd starts with "set ".
	switch(cmd[4]){
	case 't'://set table
		sscanf(cmd, "set %s %s", tmp1, tmp2);
		sprintf(p.filename, "%s", tmp2);
		p.mode = 1;
		break;
	case 'o'://set observer
		sscanf(cmd, "set %s %s", tmp1, tmp2);
		sprintf(p.project, "%s", tmp2);
		p.mode = 1;
		break;
	case 'g'://set group
		sscanf(cmd, "set %s %s", tmp1, tmp2);
		sprintf(p.group, "%s", tmp2);
		p.mode = 1;
		break;
	case 's':
		switch(cmd[5]){
		case 'o'://set source
			sscanf(cmd, "set %s %s", tmp1, tmp2);
			sprintf(p.source, "%s", tmp2);
			p.mode = 0;
			break;
		case 'c'://set schedule
			sscanf(cmd, "set %s %s", tmp1, tmp2);
			sprintf(p.source, "%s", tmp2);
			memset(msg, 0, sizeof(msg));
			ret = schedule_Init(dir, p.group, p.project, p.source, msg);
			if(ret){
				printf("schedule_Init() = %d: %s\n", ret, msg);
			}else{
				p.mode = 2;
			}
			break;
		default:
			break;	
		}
	default:
		break;	
	}
	return 0;
}

int manageCommandMonitor(char* cmd){
	long pos;
	char *token;
	int pact;

	/* 入力文字列を表示 */
	//gotoxy(5, 6);
//	gotoxy(0, 31);
	uM1("cmd: %s", cmd);
	switch(cmd[0]){
	case 'h':
		showHelpMonitor();
		break;
	case 'c':
		return 1;//command mode.
	default:break;	
	}
	return 2;//monitor mode
}

int updateStatus(){
	//uM("main(); I'll show status.");
	int ret = tkb32clFuncReqStatus();
	if(ret){
		//tmReset(vtm);
		return 1;
	}
	p.ansState = tkb32clFuncGetAnsData();
//	const tAnsState* ansState = tkb32clFuncGetAnsData();
	return 0;
}
void showStatus(){
	/* JST LST */
	time_t ut, lst;
	struct tm* ptm;
	char tmp[256];
	ut = p.ansState->UT;
	lst = p.ansState->LST;
	ptm = localtime(&ut);
	sprintf(tmp,               "%04d/%02d/%02d  %02d:%02d:%02d\r\n",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	//ptm = localtime(&lst);
	ptm = gmtime(&lst);
	sprintf(tmp + strlen(tmp), "                 %02d:%02d:%02d",
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

	fprintf(p.ttw, "** JST / LST**\r\n");
	fprintf(p.ttw, "%s\r\n", tmp);

	/* ACU */
  sprintf(tmp, "%s\r\n%s\r\n%d\r\n%.1lf %.1lf\r\n%.1lf %.1lf\r\n(%s) %.4lf %.4lf\r\n(%s) %.4lf %.4lf\r\n%.4lf %.4lf\r\n%s\r\n%s %s\r\n%s %s %s",
    errnoStr(p.ansState->trkState),
    errnoCoord(p.ansState->trkSofCoord),
		p.ansState->trkScanCnt,
    p.ansState->trkSof[0], p.ansState->trkSof[1],
    p.ansState->trkPof[0], p.ansState->trkPof[1],
    errnoTrkPZoneStr(p.ansState->trkPZONE), p.ansState->trkP3AZEL[0], p.ansState->trkP3AZEL[1],
    errnoTrkRZoneStr(p.ansState->trkRZONE), p.ansState->trkRAZEL[0], p.ansState->trkRAZEL[1],
    p.ansState->trkEAZEL[0], p.ansState->trkEAZEL[1], errnoTrkTrackingStr(p.ansState->trkTracking),
    errnoTrkACUStatusStr(p.ansState->trkACUStatus[0], 1),
    errnoTrkACUStatusStr(p.ansState->trkACUStatus[1], 2),
    errnoTrkACUStatusStr(p.ansState->trkACUStatus[2], 3),
    errnoTrkACUStatusStr(p.ansState->trkACUStatus[3], 4),
    errnoTrkACUStatusStr(p.ansState->trkACUStatus[4], 5)
    );
	fprintf(p.ttw, "** ACU **\r\n");
	fprintf(p.ttw, "%s\r\n", tmp);

	/* Chopper */
  sprintf(tmp, "%s\r\n%s\r\n%s\r\n%.2lf (%.2lf)\r\n%.2lf (%.2lf)",
    errnoStr(p.ansState->chopState), errnoChopChopStr(p.ansState->chopStat), errnoChopMoterStr(p.ansState->chopMoter),
    p.ansState->chopTemp1 - ABS_ZERO, p.ansState->chopTemp1, p.ansState->chopTemp2 - ABS_ZERO, p.ansState->chopTemp2);

	fprintf(p.ttw, "** Chopper **\r\n");
	fprintf(p.ttw, "%s\r\n", tmp);

  /* 1st IF */
  sprintf(tmp, "%s\r\n%s\r\n%s\r\n%s\r\n%.9lf  %.2lf\r\n  %.2lf\r\n%.9lf  %.2lf\r\n  %.2lf",
    errnoStr(p.ansState->if1State),
    confGetStr("LoInterval"), confGetStr("Ftrack"), confGetStr("FifL"),
    p.ansState->if1PFreq, p.ansState->if1PAmp, p.ansState->if1PAtt, p.ansState->if1RFreq, p.ansState->if1RAmp, p.ansState->if1RAtt);

	fprintf(p.ttw, "** 1st IF **\r\n");
	fprintf(p.ttw, "%s\r\n", tmp);

	/* observation table */
//	gotoxy(0, 30);
	if(p.mode == 1){
		fprintf(p.ttw, "Obstable: %s/%s/%s.obs\r\n", p.group, p.project, p.filename);
	}else if(p.mode == 2){
		schedule_printStatus(p.ttw);
//		fprintf(p.ttw, "Obstable: %s/%s/%s.obs in Schedule: %s\r\n", p.group, p.project, p.filename, p.source);
		switch(p.status){
		case 0:
			fprintf(p.ttw, "NOT STARTED.\n");
			break;
		case 1:
			fprintf(p.ttw, "Initializing...\n");
			break;
		case 2:
			fprintf(p.ttw, "Running...\n");
			break;
		case 3:
			fprintf(p.ttw, "End.\n");
			break;
		case 4:
			fprintf(p.ttw, "STOPPED ABNORMALLY.\n");
			break;
		default:
			break;
		}
	}else{
		fprintf(p.ttw, "Source: %s\r\n", p.source);
	}
	fflush(p.ttw);
}

/**
 * 
 */
int _killCIWProcess(){
	FILE* fp;
	char tmp[128];
	pid_t pid;

	//! Check
	pid = 0;

	if((fp = popen("pgrep -f -x '/usr/bin/java -jar /usr/local/inubuyo/current/inubuyo-ciw-0.0.jar'", "r")) != NULL){
		fgets(tmp, 127, fp);
		pid = atoi(tmp);
		pclose(fp);
	}
	else{
		uM("command pgrep error");
		return 1;
	}
	if(pid != 0){
		sprintf(tmp, "kill -s KILL %d", pid);
		system(tmp);
		uM(tmp);
	}
	else{
		uM("No CIW proccess was found. ");
		return 1;
	}
	return 0;
}


int
main (int argc, char *argv[])
{
  const char dir[] = "../etc";
  int ret;
  long size;
  char tmp[256];
  int i;

  confInit();
  sprintf(tmp, "%s/tkb32cl.conf", dir);
  if(confAddFile(tmp)){
    uM1("main(); confAddFile(%s); error!!", tmp);
    return 1;
    /* throw "";*/
  }
  uInit("../log/tkb32clTest");
/* 090702 out; It works but we don't need it, since tkb32clFuncInit() logs similar information. 
  uM2("Compile at %s %s\n", __DATE__, __TIME__); //!< なぜか実行時に落ちる
*/
  ret = tkb32clFuncInit();
  if(ret){
    uM("main(); tkb32clFuncInit(); error!!");
    return 1;
  }

	sprintf(p.group, "antarctic\0");
	sprintf(p.project, "test\0");
	sprintf(p.filename, "test\0");
/*
	sprintf(p.filename, "test.obs\0");
*/
	p.mode = 1;
	p.thrd = NULL;
	p.status = 0;
	p.isRunning = 0;
	/* check stdin, while the program moniters the status of tkb32. */
	fd_set rfds;
	struct timeval tv;
	char tmpstr[256];    // データ受信バッファ
	int cont = 1;//0: end, 1: command mode, 2: moniter mode
	int n = 1;

	int isOpen = 1;

/*
	if(initscr() == NULL){//090731 in
		fprintf(stderr, "initscr failure\n");
		exit(EXIT_FAILURE);
	}

	//nocbreak();//090731 in
	//echo();
*/

	FILE* ciw = popen("/usr/bin/java -jar /usr/local/inubuyo/current/inubuyo-ciw-0.0.jar", "r");
	p.ttw = popen("/usr/bin/java -jar /usr/local/inubuyo/current/inubuyo-ttw-0.1.jar", "w");
/*
	p.ttw = popen("/usr/bin/java -jar /usr/local/inubuyo/current/inubuyo-ttw-0.0.jar", "w");
*/
	char* check;
//	FILE* xterm = popen("/bin/echo", "w");
//	FILE* xterm = popen("/usr/bin/screen -X cat", "w");
//	FILE* xterm = popen("/usr/bin/xterm -e cat", "w");
	const int fd = fileno(ciw);
	do{

		if(cont == 2){//monitor used
		
			/* Watch stdin (fd 0) to see when it has input. */
			FD_ZERO(&rfds);
			FD_SET(fd, &rfds);
/*
//			FD_SET(fileno(xterm), &rfds);
			FD_SET(0, &rfds);
*/	
			/* Wait up to one seconds. */
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			/* print prompt */
			//printf("[tkb32clTest: %2d] (monitor mode) > ", n);
			//fflush(stdout);
	
			ret = select(fd+1, &rfds, NULL, NULL, &tv);
/*
			ret = select(1, &rfds, NULL, NULL, &tv);
*/	
			if(ret == -1){
				perror("select()");
			}else if(ret){
/*
				fgets(tmpstr, 256, stdin);
*/
				check = fgets(tmpstr, 256, ciw);
				if(check == NULL){
					uM("CIW terminated. ");
					break;
				}
				cont = manageCommandMonitor(tmpstr);
				n++;
				if(n >= 100){
					n -= 100;
				}
			}else{
//				if(!p.isRunning){//If it's running, the status is updated. 
					ret = updateStatus();
					if(ret){
						return 1;
					}
//				}
				showStatus();
			}
		}else{
			/* print prompt */
			//printf("[tkb32clTest: %2d] > ", n);
			//fflush(stdout);
	
			//ret = select(1, &rfds, NULL, NULL, &tv);
	
			check = fgets(tmpstr, 256, ciw);
			if(check == NULL){
				uM("CIW terminated. ");
				break;
			}
/*
			fgets(tmpstr, 256, stdin);
*/
			cont = manageCommand(tmpstr);
			n++;
		}
		//gotoxy(0, 32);
	}while(cont);
	uM("* End phase 1 *");
	
	ret = tkb32clFuncEnd();//090702 in
	if(ret){//090702 in
		uM("main(); tkb32clFuncEnd(); error!!");
		return 1;
	}
	uM("* End phase 2 *");
	_killCIWProcess();

	uM("* End phase 3 *");
	pclose(ciw);
	pclose(p.ttw);

	uM("* End phase 4 *");
	uEnd();
//	endwin();

	exit(EXIT_SUCCESS);
/* 090731 ouot
	return 0;
*/
}

void* scheduleMode(void* _p){
	tkb32clTest_t* p = (tkb32clTest_t*)_p;
	int i;
	int nFile = schedule_getNumberOfTables();
	int ret = 1;
	char msg[256];
	for(i = 0; i < nFile && p->isRunning; i++){
		schedule_getNext(p->filename);
		//initialize
		ret = 1;
		while(ret && p->isRunning){
			ret = tkb32clObservationFuncSetTable(dir, p->group, p->project, p->filename, msg);
			if(ret){
				printf("tkb32clObservationFuncSetTable() = %d: %s\n", ret, msg);
				usleep(1000*1000*5);
			}
		}
		//start
		p->status = 1;
		ret = 1;
		while(ret && p->isRunning){
			ret=tkb32clFuncReqStartObs();
			if(ret){
				printf("tkb32clFuncReqStartObs() = %d\n", ret);
			}
			usleep(1000*1000*5);
		}
		p->status = 2;
	}
}
