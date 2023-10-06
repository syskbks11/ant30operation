/*--------------------------------------------------------------------*
 *
 * ´Ø¿ôÌ¾   : trk_20_cmt
 *
 * ÏÂÌ¾     : JPL ×ÂÀ±¥Ç¡¼¥¿¤ÎÀþ·ÁÊä´
 *
 * µ¡Ç½³µÍ× : ÆÉ¤ß¹þ¤ó¤À JPL ×ÂÀ±¥Ç¡¼¥¿¤ò²¼¤Ë³ºÅö»þ¹ï¤Î·ë²Ì¤ò»»½Ð¤¹¤ë
 *
 * Ìá¤êÃÍ   : ==  0: Àµ¾ï½ªÎ»
 *            !=  0: °Û¾ï½ªÎ»
 *
 * Ãí°Õ»ö¹à : ¤Ê¤·
 *
 * ºîÀ®¼Ô   : FUJITSU LIMITED  2005.11.07
 *
 * ¥á¥â     :
 *
 *--------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "trkmod.h"

static void f_4500(double *, double *, double);

trk_20_cmt(dmjd, dsrc, dist, delt)
  double dmjd;			/* (in ) ·×»»»þ¹ï (MJD) [sec] */
  double dsrc[2];		/* (out) Å·ÂÎºÂÉ¸ (RADEC) [2] [rad] */
  double *dist;			/* (out) ÃÏ¿´¤«¤é¤ÎÅ·ÂÎ¤Îµ÷Î¥ [m] */
  double delt[3];		/* (out) ¥Ç¥«¥ë¥ÈÀ®Ê¬¤Ç¤ÎÊÑ²½ÎÌ [m] */
{
  int  i, i1, i2, indx;
  double dwk0, dwk1[3], dwk2[3];

  if (t_cmt.index != -1) {
    indx = t_cmt.index;
  } else {
    indx = 1;
  }
  for (i = indx; i < t_cmt.imax; i++) {
    if (dmjd <= t_cmt.ldmjd[i]) {
      break;
    }
  }
  if (i == t_cmt.imax) {
    fprintf(stderr, "trk_20_cmt End of JPL data (%d)\n", t_cmt.imax);
    return -1;
  }
  if (i == t_cmt.imax - 1 && dmjd == t_cmt.ldmjd[i]) {
    fprintf(stderr, "trk_20_cmt End of JPL data (%d)\n", t_cmt.imax);
    return -1;
  }
  t_cmt.index = i;
  i1 = i - 1;
  i2 = i;

  /* JPL ·ë²Ì¤Î»þ¹ï¤È´ÑÂ¬»þ¹ï¤¬°ìÃ×¤·¤¿¾ì¹ç */
  if (dmjd == t_cmt.ldmjd[i1] || dmjd == t_cmt.ldmjd[i2]) {
    if (dmjd == t_cmt.ldmjd[i2]) {
      i1 = i;
      i2 = i + 1;
    }
    dsrc[0] = t_cmt.ldrad[i1][0];
    dsrc[1] = t_cmt.ldrad[i1][1];
    *dist = t_cmt.lddkm[i1];
  }
  /* ¶è´Ö»þ¹ï¤Î¾ì¹ç¤Ï¡¢Àþ·ÁÊä´Ö¤ÇÃÍ¤òµá¤á¤ë */
  else {
    if (t_cmt.ldmjd[i2] == t_cmt.ldmjd[i1]) {
      fprintf(stderr, "trk_20_cmt 0 dividing error (1)\n");
      return -1;
    }
    dwk0 = (dmjd - t_cmt.ldmjd[i1]) / (t_cmt.ldmjd[i2] - t_cmt.ldmjd[i1]);
    dsrc[0] = dwk0 * (t_cmt.ldrad[i2][0] - t_cmt.ldrad[i1][0]) + t_cmt.ldrad[i1][0];
    dsrc[1] = dwk0 * (t_cmt.ldrad[i2][1] - t_cmt.ldrad[i1][1]) + t_cmt.ldrad[i1][1];
    *dist = dwk0 * (t_cmt.lddkm[i2] - t_cmt.lddkm[i1]) + t_cmt.lddkm[i1];
  }
  /* ¥Ç¥«¥ë¥ÈÀ®Ê¬¤Ç¤ÎÊÑ²½ÎÌ¤Î»»½Ð */
  f_4500(&t_cmt.ldrad[i1][0], dwk1, t_cmt.lddkm[i1]);
  f_4500(&t_cmt.ldrad[i2][0], dwk2, t_cmt.lddkm[i2]);
  dwk0 = t_cmt.ldmjd[i2] - t_cmt.ldmjd[i1];
  if (t_cmt.ldmjd[i2] == t_cmt.ldmjd[i1]) {
    fprintf(stderr, "trk_20_cmt 0 dividing error (2)\n");
    return -1;
  }
  delt[0] = (dwk2[0] - dwk1[0]) / dwk0;
  delt[1] = (dwk2[1] - dwk1[1]) / dwk0;
  delt[2] = (dwk2[2] - dwk1[2]) / dwk0;

/*
  printf("%lf\n", dwk0);
  printf("%.10lf %.10lf %.10lf\n",
    t_cmt.ldrad[i1][0], t_cmt.ldrad[i1][1], t_cmt.lddkm[i1]);
  printf("%.10lf %.10lf %.10lf\n",
    t_cmt.ldrad[i2][0], t_cmt.ldrad[i2][1], t_cmt.lddkm[i2]);
*/
  return 0;
}

static void f_4500(DSPHRC, DCARTC, DNRM)
  double DSPHRC[2];
  double DCARTC[3];
  double DNRM;
{
  DCARTC[0] = DNRM * cos(DSPHRC[1]) * cos(DSPHRC[0]);
  DCARTC[1] = DNRM * cos(DSPHRC[1]) * sin(DSPHRC[0]);
  DCARTC[2] = DNRM * sin(DSPHRC[1]);
}
