/*--------------------------------------------------------------------*
 *
 * 関数名   : trk_20_cmt
 *
 * 和名     : JPL 彗星データの線形補完
 *
 * 機能概要 : 読み込んだ JPL 彗星データを下に該当時刻の結果を算出する
 *
 * 戻り値   : ==  0: 正常終了
 *            !=  0: 異常終了
 *
 * 注意事項 : なし
 *
 * 作成者   : FUJITSU LIMITED  2005.11.07
 *
 * メモ     :
 *
 *--------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "trkmod.h"

#define PI2 (double)(M_PI*2.0e0)

extern LDINFO t_cmt;

static void f_4500(double *, double *, double);

int trk_20_cmt(double dmjd, double *dsrc, double *dist, double *dang, double *delt)
#if 0
  double dmjd;			/* (in ) 計算時刻 (MJD) [sec] */
  double dsrc[2];		/* (out) 天体座標 (RADEC) [2] [rad] */
  double *dist;			/* (out) 地心からの天体の距離 [m] */
  double *dang;			/* (out) ポジションアングル [rad] */
  double delt[3];		/* (out) デカルト成分での変化量 [m] */
#endif
{
  int  i, i1, i2, indx;
  double dwk0, dwk1[3], dwk2[3], dval;

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

  /* JPL 結果の時刻と観測時刻が一致した場合 */
  if (dmjd == t_cmt.ldmjd[i1] || dmjd == t_cmt.ldmjd[i2]) {
    if (dmjd == t_cmt.ldmjd[i2]) {
      i1 = i;
      i2 = i + 1;
    }
    dsrc[0] = t_cmt.ldrad[i1][0];
    dsrc[1] = t_cmt.ldrad[i1][1];
    *dist = t_cmt.lddkm[i1];
    *dang = t_cmt.ldang[i1];
  }
  /* 区間時刻の場合は、線形補間で値を求める */
  else {
    if (t_cmt.ldmjd[i2] == t_cmt.ldmjd[i1]) {
      fprintf(stderr, "trk_20_cmt 0 dividing error (1)\n");
      return -1;
    }
    dwk0 = (dmjd - t_cmt.ldmjd[i1]) / (t_cmt.ldmjd[i2] - t_cmt.ldmjd[i1]);
    dsrc[0] = dwk0 * (t_cmt.ldrad[i2][0] - t_cmt.ldrad[i1][0]) + t_cmt.ldrad[i1][0];
    dsrc[1] = dwk0 * (t_cmt.ldrad[i2][1] - t_cmt.ldrad[i1][1]) + t_cmt.ldrad[i1][1];
    *dist = dwk0 * (t_cmt.lddkm[i2] - t_cmt.lddkm[i1]) + t_cmt.lddkm[i1];

    dval = t_cmt.ldang[i2] - t_cmt.ldang[i1];
    if (fabs(dval) > M_PI) {
      if (dval > 0.0e0) {
	dval = t_cmt.ldang[i2] - (t_cmt.ldang[i1] + PI2);
      } else {
	dval = (t_cmt.ldang[i2] + PI2) - t_cmt.ldang[i1];
      }
    }
    *dang = dwk0 * dval + t_cmt.ldang[i1];
    if (*dang >= PI2) {
      *dang = *dang - PI2;
    } else if (*dang < 0.0e0) {
      *dang = *dang + PI2;
    }
  }
  /* デカルト成分での変化量の算出 */
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
  printf("%.10lf %.10lf %.10lf %.10lf\n",
    t_cmt.ldrad[i1][0], t_cmt.ldrad[i1][1], t_cmt.lddkm[i1], t_cmt.ldang[i1]);
  printf("%.10lf %.10lf %.10lf %.10lf\n",
    t_cmt.ldrad[i2][0], t_cmt.ldrad[i2][1], t_cmt.lddkm[i2], t_cmt.ldang[i2]);
*/
  return 0;
}

static void f_4500(double *DSPHRC, double *DCARTC, double DNRM)
#if 0
  double DSPHRC[2];
  double DCARTC[3];
  double DNRM;
#endif
{
  DCARTC[0] = DNRM * cos(DSPHRC[1]) * cos(DSPHRC[0]);
  DCARTC[1] = DNRM * cos(DSPHRC[1]) * sin(DSPHRC[0]);
  DCARTC[2] = DNRM * sin(DSPHRC[1]);
}
