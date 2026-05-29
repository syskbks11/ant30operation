/*!
\file obstablePath.h
\author NAGAI Makoto
\date 2009.08.12
\brief For Tsukuba 32-m telescope & Antarctic 30-cm telescope 
*/
#ifndef __OBSTABLEPATH_H__
#define __OBSTABLEPATH_H__
#ifdef __cplusplus
extern "C"{
#endif


/* Each implementation is in obstablePath.cpp */
void obstablePath_Init();
void obstablePath_End();

void obstablePath_setDirectory(const char* dir);
const char* obstablePath_getDirectory();
void obstablePath_setGruop(const char* dir);
const char* obstablePath_getGroup();
void obstablePath_setObserver(const char* dir);
const char* obstablePath_getObserver();


#ifdef __cplusplus
}
#endif
#endif
