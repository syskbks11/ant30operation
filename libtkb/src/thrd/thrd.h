/*!
\file thrd.h
\date 2007.11.16
\author Y.Koide
\brief Thread Module
*/
#ifndef __THRD_H__
#define __THRD_H__


#ifdef __cplusplus
extern "C"{
#endif

typedef struct thrd_s thrdClass_t;

  thrdClass_t* thrdInit(void* (*func)(void*), void* arg);
  int thrdEnd(thrdClass_t* _p);
  int thrdStart(thrdClass_t* _p);
  int thrdStop(thrdClass_t* _p);

#ifdef __cplusplus
}
#endif

#endif //!< __THRD_H__


