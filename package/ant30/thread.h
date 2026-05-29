#ifndef INCLUDE_GUARD_UUID_866a9b6e_f976_4821_bcba_aa0f6eae73ca
#define INCLUDE_GUARD_UUID_866a9b6e_f976_4821_bcba_aa0f6eae73ca
#ifndef __THREAD_H__
#define __THREAD_H__

#include<pthread.h>

//pthread_mutex_t rastEndFlag_mutex;
enum eRASTENDFLAG{RUN_RASTENDF = 0, END_RASTENDF = 1} g_rastEndFlag;

#endif//__THREAD_H__
#endif

