/*!
¥file schedule.cpp
¥author NAGAI Makoto
¥date 2009.8.12
¥brief For Tsukuba 32-m telescope & Antarctic 30-cm telescope 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>//usleep()

#include "tkb32clObservationFunc.h"

#include "schedule.h"
#include "obstablePath.h"


typedef struct sParamSchedule{
	char scheduleFile[64];
	int nTables;
	char tables[64][16];//max 64 files, filename max 16 char.
	int current;
}tParamSchedule;

static tParamSchedule p;


int schedule_Init(const char* dir, const char* grpName, const char* obsrName, const char* scheduleFile, char* msg){
	char tmp[1024];
	sprintf(tmp, "%s/%s/%s/%s.sch", dir, grpName, obsrName, scheduleFile);
	FILE* fp = fopen(tmp, "r");
	if(fp == NULL){
		sprintf(msg, "%s cannot opened; %s", tmp, strerror(errno));
//		sprintf(msg, "%s", strerror_r(errno));
		return 1;
	}

	obstablePath_Init();
	obstablePath_setDirectory(dir);
	obstablePath_setGruop(grpName);
	obstablePath_setObserver(obsrName);
	sprintf(p.scheduleFile, "%s", scheduleFile);

	char line[256];
	char* check;
	int ret;
	int nFile = 0;
	char filename[64];
	while(1){
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
		ret = tkb32clObservationFuncCheckTable(dir, grpName, obsrName, filename, msg);
		if(ret){
			fclose(fp);
			return 1;
		}
		sprintf(p.tables[nFile], filename);
		nFile++;
		printf("obsfile %s is readable.\n", filename);
	}
	fclose(fp);
	p.nTables = nFile;
	p.current = 0;
	printf("%d files were read from %s.\n", nFile, scheduleFile);
//	printf("%d lines were read from %s.\n", nLine, schFile);
	return 0;//normal end

}
void schedule_End(){
	obstablePath_End();
}

int schedule_getNumberOfTables(){
	return p.nTables;
}
void schedule_getNext(char* dst){
	sprintf(dst, "%s", p.tables[p.current]);
//	sprintf(dst, "%s/%s/%s/%s", obstablePath_getDirectory(), obstablePath_getGroup(), obstablePath_getObserver(), p.tables[p.current]);
	p.current++;
}
/*
 * 0: No table remains, 1: Some tables remain. 
 */
int schedule_hasNext(){
	if(p.current == p.nTables){
		return 1;
	}else{
		return 0;
	}
}
void schedule_printStatus(FILE* fd){
	fprintf(fd, "%s/%s/%s/%s:\r\n", obstablePath_getDirectory(), obstablePath_getGroup(), obstablePath_getObserver(), p.scheduleFile);
	int i;
	for(i = 0; i < p.current-1; i++){
		fprintf(fd, "\t+ %s\r\n", p.tables[i]);
	}
	if(p.current != 0){
		fprintf(fd, "\t* %s\r\n", p.tables[i]);
	}
	for(i = p.current; i < p.nTables; i++){
		fprintf(fd, "\t  %s\r\n", p.tables[i]);
	}
}

