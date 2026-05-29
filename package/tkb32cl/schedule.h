/*!
\file schedule.h
\author NAGAI Makoto
\date 2009.08.12
\brief For Tsukuba 32-m telescope & Antarctic 30-cm telescope 
*/
#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__
#ifdef __cplusplus
extern "C"{
#endif


/* Each implementation is in schedule.cpp */
int schedule_Init(const char* dir, const char* grpName, const char* obsrName, const char* scheduleFile, char* msg);
void schedule_End();

int schedule_getNumberOfTables();
void schedule_getNext(char* dst);
int schedule_hasNext();
void schedule_printStatus(FILE* fd);


#ifdef __cplusplus
}
#endif
#endif
