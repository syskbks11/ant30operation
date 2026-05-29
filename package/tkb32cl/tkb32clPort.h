/*!
¥file tkb32clPort.h
¥date 2009.06.16
¥author NAGAI Makoto
¥brief Adopted tkb32clFunc.h by  Y.Koide
¥brief tkb32/ant30 Central Program Client Port
*/
#ifndef __TKB32CLPORT_H__
#define __TKB32CLPORT_H__
#include "tkb32Protocol.h"

#ifdef __cplusplus
extern "C"{
#endif

int tkb32clPortInit();int tkb32clPortEnd();

int tkb32clPortReq(tReq* req);
int tkb32clPortGetAns(tAns** ans);

#ifdef __cplusplus
}
#endif
#endif
