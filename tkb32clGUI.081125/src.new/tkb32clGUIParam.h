#ifndef __TKB32CLGUIPARAM_H__
#define __TKB32CLGUIPARAM_H__

#include <gtk/gtk.h>

typedef struct sParam{
  int argc;
  char **argv;
  int show;

  /* winTkb32clGUI */
  GtkWidget* winTkb32clGUI;
  GtkWidget* txtJST;
  GtkWidget* txtSource;

  GtkWidget* txtWeather;
  GtkWidget* txtACU;
  GtkWidget* txtChop;
  GtkWidget* txtFrontend;
  GtkWidget* txtIf1;
  GtkWidget* txtIf2;
  GtkWidget* txtSaacq;

  GtkWidget* entObsFileName;
  GtkWidget* entGroupName;
  GtkWidget* entProjectName;
  GtkWidget* cmbObsFlag;
  GtkWidget* entObsPosOffAZ;
  GtkWidget* entObsPosOffEL;
  GtkWidget* entObsXDeg;
  GtkWidget* entObsXMin;
  GtkWidget* entObsXSec;
  GtkWidget* entObsYDeg;
  GtkWidget* entObsYMin;
  GtkWidget* entObsYSec;
  GtkWidget* ent1stIFGHz;
  GtkWidget* ent1stIFdBm;
  GtkWidget* ent1stIFdB;
  GtkWidget* ent2ndIFGHz;
  GtkWidget* ent2ndIFdBm;
  GtkWidget* entSASec;
  double posoff0;
  double posoff1;

  /* winAllParam */
  GtkWidget* winAllStatus;
  GtkWidget* txtAllStatus;

  /* winParameterList */
  GtkWidget* winParameterList;
  GtkWidget* txtParameterList;

  /* winMap */
  GtkWidget *winMap;
  GdkPixmap *pixmap;
  GtkWidget *drawing_area;
  GdkGC *gc;
}tParam;

#endif