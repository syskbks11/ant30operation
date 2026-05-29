/*!
\file callbacksSub.h
\date 2007.01.27
\author Y.Koide
*/
#ifndef __CALLBACKSSUB_H__
#define __CALLBACKSSUB_H__

#include "tkb32clGUIParam.h"

tParamGUI* callbacksSubInit(int, char**);
void callbacksSubViewParam();
/* 090616 moved to callbacksSub.c
int idle_func_winTkb32clGUI();
int idle_func_winAllStatus();
*/
int dialog_ok(const char* message);
int dialog_check(const char* message);
int callbacksSubDrawMap();
/* 090616 out
void* callbacksSubInit(int, char**);
void callbacksSubViewParam(void*);
int idle_func_winTkb32clGUI(void*);
int idle_func_winAllStatus(void*);
int dialog_ok(void* _p, const char* message);
int dialog_check(void* _p, const char* message);
int callbacksSubDrawMap(void* _p);
*/tParamGUI* callbacksSubGetInstance();
gint configure_event (GtkWidget * widget, GdkEventConfigure * event);
GdkGC *set_color(gushort r, gushort g, gushort b);
gint expose_event (GtkWidget * widget, GdkEventExpose * event);

#endif
