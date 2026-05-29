/*!
\file thrd.cpp
\date 2007.11.16
\author Y.Koide
\brief Thread Module
*/

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "thrd.h"

typedef struct thrd_s{
#ifdef WIN32
  HANDLE hth;          //!< Thread handle
  DWORD thrdState;     //!< Thread status
  unsigned int thrdId; //!< Thread ID
#else
  pthread_t thrdId;    //!< Thread ID
#endif
  void* func;
  void* arg;
  void* thrdRet;      //!< Thread return val
  int thrdRun;         //!< 0:Not thread running 1:Thread running
} thrd_t;

typedef void*(*thrdFunc_t)(void*);

//! Thread function
#ifdef WIN32
static unsigned __stdcall thrdFunc(thrdClass_t* p){
#else
static void* thrdFunc(thrdClass_t* p){
#endif
//  thrd_t* p = (thrd_t*)_p;
  p->thrdRet = ((thrdFunc_t)(p->func))(p->arg);

  p->thrdRun = 0;
  return NULL;
}

thrdClass_t* thrdInit(void* (*func)(void*), void* arg){
  thrd_t* p;
  p = new thrd_t();
  if(!p){
    return NULL;
  }
  memset(p, 0, sizeof(p[0]));
  
  p->func = (void*)func;
  p->arg = arg;
  return p;
}

int thrdEnd(thrdClass_t* p){
//  thrd_t* p = (thrd_t*)_p;

  if(p){
    if(p->thrdRun){
      thrdStop(p);
    }
    delete[] p;
    p = NULL;
  }

  return 0;
}

int thrdStart(thrdClass_t* p){
//  thrd_t* p = (thrd_t*)_p;
  int ret;
#ifdef WIN32
  p->hth = (HANDLE) _beginthreadex(NULL, 0, thrdFunc, p, 0, &p->thrdId);
  if(p->hth == 0){
    return 1;
  }
#else
  ret = pthread_create(&p->thrdId, NULL, (void* (*)(void*))thrdFunc, (void*)p);
  if(ret){
    return 1;
  }
#endif

  p->thrdRun = 1;  
  return 0;
}

int thrdStop(thrdClass_t* p){
//  thrd_t* p = (thrd_t*)_p;
  int ret;
  void* thrdRet = NULL;

#ifdef WIN32
  CloseHandle(p->hth);
#else
  ret = pthread_join(p->thrdId, &thrdRet);
  if(ret){
    p->thrdRet = thrdRet;
  }
  p->thrdId = 0;
#endif

  p->thrdRun = 0;
  return 0;
}

