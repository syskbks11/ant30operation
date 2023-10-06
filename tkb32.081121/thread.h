#ifndef __THREAD_H__
#define __THREAD_H__

#include<pthread.h>

//pthread_mutex_t rastEndFlag_mutex;
enum eRASTENDFLAG{RUN_RASTENDF = 0, END_RASTENDF = 1} g_rastEndFlag;

#endif//__THREAD_H__
