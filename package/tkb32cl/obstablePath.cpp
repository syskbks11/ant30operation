/*!
¥file obstablePath.cpp
¥author NAGAI Makoto
¥date 2009.8.12
¥brief For Tsukuba 32-m telescope & Antarctic 30-cm telescope 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obstablePath.h"


typedef struct sParamObstablePath{
	char directory[32];
	char group[32];
	char observer[32];
}tParamObstablePath;

static tParamObstablePath p;


void obstablePath_Init(){
	memset(&p, 0, sizeof(p));
}
void obstablePath_End(){
}

void obstablePath_setDirectory(const char* dir){
	sprintf(p.directory, "%s", dir);
}
const char* obstablePath_getDirectory(){
	return p.directory;
}
void obstablePath_setGruop(const char* dir){
	sprintf(p.group, "%s", dir);
}
const char* obstablePath_getGroup(){
	return p.group;
}
void obstablePath_setObserver(const char* dir){
	sprintf(p.observer, "%s", dir);
}
const char* obstablePath_getObserver(){
	return p.observer;
}

