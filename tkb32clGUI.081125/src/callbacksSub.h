/*!
\file callbacksSub.h
\date 2007.01.27
\author Y.Koide
*/
#ifndef __CALLBACKSSUB_H__
#define __CALLBACKSSUB_H__


void* callbacksSubInit(int, char**);
void callbacksSubViewParam(void*);
struct tParam* callbacksSubGetInstance();
int idle_func_winTkb32clGUI(void*);
int idle_func_winAllStatus(void*);
int dialog_ok(void* _p, const char* message);
int dialog_check(void* _p, const char* message);
gint configure_event (GtkWidget * widget, GdkEventConfigure * event);
GdkGC *set_color(gushort r, gushort g, gushort b);
gint expose_event (GtkWidget * widget, GdkEventExpose * event);
int callbacksSubDrawMap(void* _p);

#endif
