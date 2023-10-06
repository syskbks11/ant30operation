/*!
\file callbacksSub.c
\date 2007.01.27
\author Y.Koide
*/
#include <stdlib.h>
#include <gtk/gtk.h>
#include <time.h>
#include "../libtkb/export/libtkb.h"
#include "import/errno.h"
#include "import/tkb32clFunc.h"
#include "support.h"
#include "tkb32clGUIParam.h"
#include "callbacksSub.h"

const double ABS_ZERO= 273.15;

static void* vtm;
static void* vtm2;
static tParam* p = NULL;
//static GdkPixmap *pixmap = NULL;

void* callbacksSubInit(int argc, char* argv[]){
  unsigned int intval = 200;
  int i = 0;

  p = (tParam*)malloc(sizeof(tParam));
  memset(p, 0, sizeof(tParam));
  p->argc = argc;
  p->argv = argv;
  
  p->show = 0;
  for(i = 1; i < p->argc; i++){
    if(strcmp(p->argv[i], "full") == 0){
      p->show = 1;
    }
  }

  vtm = tmInit();
  vtm2 = tmInit();
  //gtk_idle_add(idle_func_winTkb32clGUI, (void*)&p);
  //gtk_idle_add(idle_func_winAllStatus, (void*)&p);
  gtk_timeout_add(intval, idle_func_winTkb32clGUI, (void*)&p);
  if(p->show){
    gtk_timeout_add(intval, idle_func_winAllStatus, (void*)&p);
  }
  return (void*)p;
}

void callbacksSubViewParam(void* _p){
  tParam* p = (tParam*) _p;
  char tmp[256];
  double xy[2][3] = {0};
  int e;
  int c;
  int d;
  int f;

  if(confGetStr("X_Sec"))
    xy[0][0] = atof(confGetStr("X_Sec"));
  if(confGetStr("X_Min"))
    xy[0][1] = atof(confGetStr("X_Min"));
  if(confGetStr("X_HorD"))
    xy[0][2] = atof(confGetStr("X_HorD"));
  if(confGetStr("Y_Sec"))
    xy[1][0] = atof(confGetStr("Y_Sec"));
  if(confGetStr("Y_Min"))
    xy[1][1] = atof(confGetStr("Y_Min"));
  if(confGetStr("Y_Deg"))
    xy[1][2] = atof(confGetStr("Y_Deg"));
  
  e = c = d = f = -1;
  if(confSetKey("Epoch"))
    e = atoi(confGetVal());
  if(confSetKey("Coordinate"))
    c = atoi(confGetVal());
  if(confSetKey("Definition"))
    d = atoi(confGetVal());
  if(confSetKey("Frame"))
    f = atoi(confGetVal());

  //gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtParameterList)), _(confGetAllKeyVal()), -1);
  sprintf(tmp, "%s\n%s\n%s\n%02.0lf:%02.0lf:%02.2lf %02.0lf:%02.0lf:%02.2lf\n%s\n%s\n%s",
    confGetStr("SourceName"), errnoEpoch(e), errnoCoord(c),
    xy[0][2], xy[0][1], xy[0][0], xy[1][2], xy[1][1], xy[1][0],
    errnoFrame(f), errnoDefinition(d), confGetStr("Velocity")
    );
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtSource)), _(tmp), -1);
  if(p->show){
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtParameterList)), _(confGetAllKeyVal()), -1);
  }
  return;
}

struct tParam* callbacksSubGetInstance(){
  return p;
}

int idle_func_winTkb32clGUI(void* _p){
  //tParam* p = (tParam*) _p; //!< なぜかデータが取得できない。
  const tAnsState* s;
  time_t ut, lst;
  struct tm* ptm;
  char tmp[256];
  char tmp2[256];
  int ret;

  //! 初回呼び出し時、もしくはエラーになったら1sec待ってから接続する。
  if(tmGetLag(vtm) < 1.0){
    tmSleepMSec(1);
    return TRUE;
  }

  ret = tkb32clFuncReqStatus();
  if(ret){
    tmReset(vtm);
    return TRUE;
  }
  s = tkb32clFuncGetAnsData();

  if(!p->winTkb32clGUI){
    return TRUE;
  }

  /* JST LST */
  ut = s->UT;
  lst = s->LST;
  ptm = localtime(&ut);
  sprintf(tmp,               "%04d/%02d/%02d  %02d:%02d:%02d\n",
    ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  //ptm = localtime(&lst);
  ptm = gmtime(&lst);
  sprintf(tmp + strlen(tmp), "                 %02d:%02d:%02d",
    ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtJST)), _(tmp), -1);

  /* Weather */
  sprintf(tmp, "%s\n%.2lf\n%.2lf\n%.2lf\n%.2lf\n%.2lf\n%.2lf",
    errnoStr(s->weathState), s->weathData[0] - ABS_ZERO, s->weathData[1], s->weathData[2], s->weathData[3], s->weathData[4], s->weathData[5]);
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtWeather)), _(tmp), -1);
  
  /* ACU */
  sprintf(tmp, "%s\n%s\n%d\n%.1lf %.1lf\n%.1lf %.1lf\n(%s) %.4lf %.4lf\n(%s) %.4lf %.4lf\n%.4lf %.4lf\n%s\n%s %s\n%s %s %s",
    errnoStr(s->trkState),
    errnoCoord(s->trkSofCoord),
		s->trkScanCnt,
    s->trkSof[0], s->trkSof[1],
    s->trkPof[0], s->trkPof[1],
    errnoTrkPZoneStr(s->trkPZONE), s->trkP3AZEL[0], s->trkP3AZEL[1],
    errnoTrkRZoneStr(s->trkRZONE), s->trkRAZEL[0], s->trkRAZEL[1],
    s->trkEAZEL[0], s->trkEAZEL[1], errnoTrkTrackingStr(s->trkTracking),
    errnoTrkACUStatusStr(s->trkACUStatus[0], 1),
    errnoTrkACUStatusStr(s->trkACUStatus[1], 2),
    errnoTrkACUStatusStr(s->trkACUStatus[2], 3),
    errnoTrkACUStatusStr(s->trkACUStatus[3], 4),
    errnoTrkACUStatusStr(s->trkACUStatus[4], 5)
    );
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtACU)), _(tmp), -1);

  /* Chopper */
  sprintf(tmp, "%s\n%s\n%s\n%.2lf (%.2lf)\n%.2lf (%.2lf)",
    errnoStr(s->chopState), errnoChopChopStr(s->chopStat), errnoChopMoterStr(s->chopMoter),
    s->chopTemp1 - ABS_ZERO, s->chopTemp1, s->chopTemp2 - ABS_ZERO, s->chopTemp2);
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtChop)), _(tmp), -1);

  /* Frontend */
  sprintf(tmp, "%s\n%.2lf\n%.2e", errnoStr(s->feState), s->feK, s->fePa);
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtFrontend)), _(tmp), -1);

  /* 1st IF */
  sprintf(tmp, "%s\n%s\n%s\n%s\n%.9lf  %.2lf\n  %.2lf\n%.9lf  %.2lf\n  %.2lf",
    errnoStr(s->if1State),
    confGetStr("LoInterval"), confGetStr("Ftrack"), confGetStr("FifL"),
    s->if1PFreq, s->if1PAmp, s->if1PAtt, s->if1RFreq, s->if1RAmp, s->if1RAtt);
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtIf1)), _(tmp), -1);

  /* 2nd IF */
  sprintf(tmp, "%s\n%s\n%.9lf  %.2lf\n%.9lf  %.2lf",
    errnoStr(s->if2State),
    confGetStr("FsaL_H"),
    s->if2PFreq, s->if2PAmp, s->if2RFreq, s->if2RAmp);
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtIf2)), _(tmp), -1);

  /* SA Hardware */
  sprintf(tmp, "%s\n%s %s\n%s\n%s\n%e\n%e",
    errnoStr(s->saacqState),
    confGetStr("RSkyTime"), confGetStr("OnOffTime"),
    errnoSaacqExe(s->saacqExe),
    errnoSaacqErr(s->saacqErr),
    s->saacqTsys, s->saacqTotPow);
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (p->txtSaacq)), _(tmp), -1);

  //if(p->show){
    callbacksSubDrawMap(p);
  //}

  return TRUE;
}

int idle_func_winAllStatus(void* _p)
{
  //tParam* p = (tParam*) _p;
  char tmp[10240];
  GtkWidget* txt = p->txtAllStatus;
  const tAnsState* s;
  time_t ut, lst;

  //if(tmGetLag(vtm2) < 1.0)
  //  return TRUE;
  //tmReset(vtm2);

  //tkb32clFuncReqStatus();
  s = tkb32clFuncGetAnsData();
  
  ut=s->UT;
  lst=s->LST;
  sprintf(tmp,               "Status\nJST = (%u) %s\n", s->UT, ctime(&ut));
  sprintf(tmp + strlen(tmp), "LST = (%u) %s\n",s->LST, ctime(&lst));
  sprintf(tmp + strlen(tmp), "weath=%d trk=%d chop=%d fe=%d if1=%d if2=%d saacq=%d\n", s->weathState, s->trkState, s->chopState, s->feState, s->if1State, s->if2State, s->saacqState);
  sprintf(tmp + strlen(tmp), "Tamb=%6.2lf Pamb=%6.2lf Moi=%6.2lf WindD=%6.2lf WindAvg=%6.2lf WindMax=%6.2lf\n", s->weathData[0], s->weathData[1], s->weathData[2], s->weathData[3], s->weathData[4], s->weathData[5]);
  sprintf(tmp + strlen(tmp), "(Coord) trkXY = (%d) %lf %lf\n", s->trkXYCoord, s->trkXY[0], s->trkXY[1]);
  sprintf(tmp + strlen(tmp), "(Coord) trkSof = (%d) %lf %lf\n", s->trkSofCoord, s->trkSof[0], s->trkSof[1]);
  sprintf(tmp + strlen(tmp), "(Coord) trkPof = (%d) %lf %lf\n", s->trkPofCoord, s->trkPof[0], s->trkPof[1]);
  sprintf(tmp + strlen(tmp), "Cmd  (Zone) AZEL= (%d) %lf %lf\n", s->trkPZONE, s->trkP3AZEL[0], s->trkP3AZEL[1]);
  sprintf(tmp + strlen(tmp), "Read (Zone) AZEL= (%d) %lf %lf\n", s->trkRZONE, s->trkRAZEL[0], s->trkRAZEL[1]);
  sprintf(tmp + strlen(tmp), "Err            AZEL= %lf %lf\n", s->trkEAZEL[0], s->trkEAZEL[1]);
  sprintf(tmp + strlen(tmp), "TrackingStatus %d\n", s->trkTracking);
  sprintf(tmp + strlen(tmp), "ACUStatus  S1 = %d, S2 = %d, S3 = %d, S4 = %d, S5 = %d\n", s->trkACUStatus[0], s->trkACUStatus[1], s->trkACUStatus[2], s->trkACUStatus[3], s->trkACUStatus[4]);
  sprintf(tmp + strlen(tmp), "chopStat=%d chopMoter=%d chopTemp1=%6.2lf chopTemp2=%6.2lf\n", s->chopStat, s->chopMoter, s->chopTemp1, s->chopTemp2);
  sprintf(tmp + strlen(tmp), "feK=%6.2lf fePa=%6.2e\n", s->feK, s->fePa);
  sprintf(tmp + strlen(tmp), "Vrad = %lf\n", s->ifVrad);
  sprintf(tmp + strlen(tmp), "Cmd  if1 Freq=%eHz Amp=%6.2lf Att=%6.2lf\n", s->if1PFreq, s->if1PAmp, s->if1PAtt);
  sprintf(tmp + strlen(tmp), "Cmd  if2 Freq=%eHz Amp=%6.2lf\n", s->if2PFreq, s->if2PAmp);
  sprintf(tmp + strlen(tmp), "Real if1 Freq=%eHz Amp=%6.2lf Att=%6.2lf\n", s->if1RFreq, s->if1RAmp, s->if1RAtt);
  sprintf(tmp + strlen(tmp), "Real if2 Freq=%eHz Amp=%6.2lf\n", s->if2RFreq, s->if2RAmp);
  sprintf(tmp + strlen(tmp), "saExe=%d saErr=%d saTsys=%6.2e saTotPow=%6.2e\n", s->saacqExe, s->saacqErr, s->saacqTsys, s->saacqTotPow);
  
  //sprintf(tmp, "test")
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (txt)), _(tmp), -1);
  return TRUE;
}

void dialog_func_true(GtkWidget *widget, gpointer data){
  *((int*) data) = TRUE;
}

void dialog_func_false(GtkWidget *widget, gpointer data){
  *((int*) data) = FALSE;
}

int dialog_ok(void* _p, const char* message){
  tParam* p = (tParam*) _p;
  static GtkWidget* dialog = NULL;
  char title[] = "Message";
  //char message[] = "message";

  //if(dialog == NULL){
    gint flag = TRUE;

    {
      dialog = gtk_dialog_new();
      //gtk_signal_connect(GTK_OBJECT(dialog), "delete_event",
      //  GTK_SIGNAL_FUNC(gtk_false), NULL);
      //gtk_signal_connect(GTK_OBJECT(dialog), "destory",
      //  G_CALLBACK(gtk_main_quit), NULL);
      g_signal_connect ((gpointer) dialog, "destroy", G_CALLBACK(gtk_main_quit),
        NULL);

      gtk_window_set_title(GTK_WINDOW(dialog), title);
      gtk_container_set_border_width(GTK_CONTAINER(dialog), 0);
      //gtk_widget_set_usize(dialog, 250, 110);
    }

    {
      GtkWidget *label;
      label = gtk_label_new(message);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
      gtk_widget_show(label);
    }
    
    {
      GtkWidget* button;
      button = gtk_button_new_with_label("OK");
      //gtk_signal_connect(GTK_OBJECT(button), "clicked",
      //  GTK_SIGNAL_FUNC(gtk_false), &flag);
      //gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
      //  G_CALLBACK(gtk_main_quit),
      //  GTK_OBJECT(dialog));
      gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
        G_CALLBACK(gtk_widget_destroy),
        GTK_OBJECT(dialog));
      GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
        button, TRUE, FALSE, 0);
      gtk_widget_grab_default(button);
      gtk_widget_show(button);
    }

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog),
      GTK_WINDOW(p->winTkb32clGUI));
    {
      gint x,y;
      gdk_window_get_root_origin(p->winTkb32clGUI->window, &x, &y);
      gtk_widget_set_uposition(dialog, x + 100, y + 100);
    }
    gtk_widget_show(dialog);

    gtk_main();

    return 0;
  //}
  //return TRUE;
}

int dialog_check(void* _p, const char* message){
  tParam* p = (tParam*) _p;
  static GtkWidget* dialog = NULL;
  char title[] = "Message";
  gint flag = FALSE;

  {
    dialog = gtk_dialog_new();
    g_signal_connect ((gpointer) dialog, "destroy", G_CALLBACK(gtk_main_quit),
      NULL);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 0);
  }

  {
    GtkWidget *label;
    label = gtk_label_new(message);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
  }

  //! OKボタン
  {
    GtkWidget* button;
    button = gtk_button_new_with_label("OK");
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
      GTK_SIGNAL_FUNC(dialog_func_true), (gpointer)&flag); //!< OKを押したらflagにtrueをセット
    gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
      G_CALLBACK(gtk_widget_destroy),
      GTK_OBJECT(dialog));
    GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
      button, TRUE, TRUE, 0);
    gtk_widget_grab_default(button);
    gtk_widget_show(button);
  }

  //! Cancelボタン
  {
    GtkWidget* button;
    button = gtk_button_new_with_label("Cancel");
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
      GTK_SIGNAL_FUNC(dialog_func_false), (gpointer)&flag); //!< Cancelを押したらflagにfalseをセット
    gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
      G_CALLBACK(gtk_widget_destroy),
      GTK_OBJECT(dialog));
    GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
      button, TRUE, TRUE, 0);
    gtk_widget_grab_default(button);
    gtk_widget_show(button);
  }

  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_transient_for(GTK_WINDOW(dialog),
    GTK_WINDOW(p->winTkb32clGUI));
  {
    gint x,y;
    gdk_window_get_root_origin(p->winTkb32clGUI->window, &x, &y);
    gtk_widget_set_uposition(dialog, x + 200, y + 200);
  }
  gtk_widget_show(dialog);

  gtk_main();

  if(flag == TRUE){
    return 1;
  }
  else{
    return 0;
  }
}

/*************************************************************************************
* 以下winMap用のグラフィック関連
*************************************************************************************/

GdkGC *set_color(gushort r, gushort g, gushort b)
{
  GdkColor color;

  color.red = r;
  color.green = g;
  color.blue = b;
  gdk_color_alloc(gdk_colormap_get_system(), &color);
  gdk_gc_set_foreground(p->gc, &color);
  return p->gc;
}

gint configure_event (GtkWidget * widget, GdkEventConfigure * event)
{
  if (p->pixmap)
    gdk_pixmap_unref (p->pixmap);

  /*
   * GdkPixmap *
   * gdk_pixmap_new (GdkWindow *window, # gdk window
   *                 gint      width,   # 
   *                 gint      height,  # 
   *                 gint      depth);  # 
   * 第4引数 (depth):
   *   depth を指定します。
   *   通常は、-1 を指定します。
   */
  p->pixmap = gdk_pixmap_new (widget->window,
                           widget->allocation.width,
                           widget->allocation.height,
                           -1);
  return TRUE;
}

/* Redraw the screen from the backing pixmap */
gint expose_event (GtkWidget * widget, GdkEventExpose * event)
{
  //gdk_window_clear_area(widget->window,
  //                      event->area.x,
  //                      event->area.y,
  //                      event->area.width,
  //                      event->area.height);
  //gdk_gc_set_clip_rectangle(widget->style->fg_gc[widget->state],
  //                          &event->area);
  /*
   *
   * void
   * gdk_draw_pixmap (GdkDrawable *drawable,
   *                  GdkGC       *gc,
   *                  GdkPixmap   *src,
   *                  gint        xsrc,
   *                  gint        ysrc,
   *                  gint        xdest,
   *                  gint        ydest,
   *                  gint        width,
   *                  gint        height);
   * drawable  コピー先
   * gc        グラフィック属性
   * src       コピー元
   * xsrc      コピー元の x 座標
   * ysrc      コピー元の y 座標
   * xdest     コピー先の x 座標
   * ydest     コピー先の y 座標
   * width     コピーを行う横幅
   *           -1 の時は、コピー元の横幅
   * height    コピーを行う縦幅
   *           -1 の時は、コピー元の縦幅
   */
  gdk_draw_pixmap (widget->window,
                   widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                   p->pixmap,
                   event->area.x, event->area.y,
                   event->area.x, event->area.y,
                   event->area.width, event->area.height);
  return FALSE;
}

int callbacksSubDrawMap(void* _p){
  int ret;
  int w, h;
  const int circ = 5;
  const tAnsState* s;

  w = p->drawing_area->allocation.width;
  h = p->drawing_area->allocation.height;
  s = tkb32clFuncGetAnsData();

  gdk_draw_rectangle(p->pixmap,
             set_color(0xeeee, 0xeeee, 0xeeee),
             TRUE,
             0, 0,
             p->drawing_area->allocation.width, 
             p->drawing_area->allocation.height);

  /* X axis */
  gdk_draw_line(p->pixmap,
             set_color(0x00000, 0x00000, 0x00000),
             0, h/2,
             w, h/2);
  /* Y axis */
  gdk_draw_line(p->pixmap,
             set_color(0x00000, 0x00000, 0x00000),
             w/2, 0,
             w/2, h);

  /*
  円や円弧を描く
  void gdk_draw_arc       (GdkDrawable      *drawable,
                         GdkGC            *gc,
                         gint              filled,
                         gint              x,
                         gint              y,
                         gint              width,
                         gint              height,
                         gint              angle1,
                         gint              angle2);
  */

  gdk_draw_arc(p->pixmap,
    set_color(0x00000, 0x00000, 0x00000),
    TRUE,
    w/2 - s->trkSof[0] / 10 - circ/2, w/2 - s->trkSof[1]/10 - circ/2,
    circ, circ,
    0, 360*64);

  gtk_widget_draw(p->drawing_area, NULL);
  
  return 0;
}
