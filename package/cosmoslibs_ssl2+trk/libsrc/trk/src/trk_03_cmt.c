/*--------------------------------------------------------------------*
 *
 * 関数名   : trk_03_cmt
 *
 * 和名     : JPL 彗星ファイルのリード
 *
 * 機能概要 : JPL 彗星ファイルからデータを読み込む
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
#include <time.h>
#include "trkmod.h"
#include "trk.h"

#define  IBUFF 1024
#define  DAUNIT  (double)1.4959787066e11	/* 1AU 距離 [m] */

static int f_err(int, int, char *, char *, char *);
static int f_leng
     (char *, char *, char *, char *, char *, char *, char *, char *, char *, char *, int, char *);
static double f_date1(char *);
static double f_date2(char *, char *);
static double f_tsec(int, int, struct tm);
static int f_leap(int);

/*--------------------------------------------------------------------*/
/* 異常終了処理                                                       */
/*--------------------------------------------------------------------*/
static FILE *fid;
static int f_err(int iret, int iline, char *cfile, char *cmes, char *cerr)
#if 0
  int  iret;
  int  iline;
  char *cfile;
  char *cmes;
  char *cerr;
#endif
{
  fclose(fid);
  sprintf(cerr, "trk_03 %s (Line:%d, %s)", cmes, iline, cfile);
  return iret;
}

/*--------------------------------------------------------------------*/
/* メイン処理                                                         */
/*--------------------------------------------------------------------*/
int trk_03_cmt(char *cstart, char *cfile, int *imax, double *ldmjd, double ldrad[CMTMAX][2], double *lddkm, double *ldang, char *cerr)
#if 0
  char *cstart;			/* (in ) 観測開始時刻 YYYYMMDDhhmmss */
  char *cfile;			/* (in ) JPL 彗星ファイル */
  int *imax;			/* (out) 取得したデータ数 */
  double ldmjd[CMTMAX];		/* (out) MJD [sec] */
  double ldrad[CMTMAX][2];	/* (out) 天体座標 (RADEC) [2] [rad] */
  double lddkm[CMTMAX];		/* (out) 地心からの天体の距離 [m] */
  double ldang[CMTMAX];		/* (out) ポジションアングル [rad] */
  char *cerr;			/* (out) エラーメッセージ */
#endif
{
  int  i, iline, ival, ichk, iwk;
  char cbuf[IBUFF], cmes[128], ctmp[10][128];
  double dsign, ddec, ddeg[2];
  double dmmm, d1st, d2nd, dwk;

  *imax = 0;

  /* MJD 算出 */
  if ((d1st = f_date1(cstart)) < 0) {
    sprintf(cerr, "trk_03 Illegal start-time (%s)", cstart);
    return -1;
  }
  /* JPL 彗星ファイルのオープン */
  if ((fid = fopen(cfile, "r")) == NULL) {
    sprintf(cerr, "trk_03 Can't open!! file name=%s", cfile);
    return -1;
  }
  /*---------------------------------------------------------------------------------
     Date__(UT)__HR:MN     R.A._(ICRF/J2000.0)_DEC              delta   PsAng   PsAMV
     2005-Oct-25 00:00     10 14 52.29 -05 16 36.5   7.34405166217695 280.985 115.297
    ---------------------------------------------------------------------------------*/
  /* JPL 彗星ファイルの読み込み */
  i = iline = 0;
  ichk = -1;
  d2nd = 0.0e0;
  while (fgets(cbuf, IBUFF, fid) != NULL) {
    iline++;
    if (cbuf[0] == '#' || cbuf[0] == '\n') {
      continue;
    }
    ival = sscanf(cbuf, "%s%s%s%s%s%s%s%s%s%s",
		  ctmp[0], ctmp[1], ctmp[2], ctmp[3], ctmp[4],
		  ctmp[5], ctmp[6], ctmp[7], ctmp[8], ctmp[9]);
    if (ival < 9) {
      sprintf(cmes, "Parameter shortage, ival:%d", ival);
      return f_err(-2, iline, cfile, cmes, cerr);
    }
    if (ichk == -1) {
      ichk = ival;
    }
    if (f_leng(ctmp[0], ctmp[1], ctmp[2], ctmp[3], ctmp[4],
	    ctmp[5], ctmp[6], ctmp[7], ctmp[8], ctmp[9], ival, cmes) != 0) {
      return f_err(-3, iline, cfile, cmes, cerr);
    }
    if ((dmmm = f_date2(ctmp[0], ctmp[1])) < 0) {
      sprintf(cmes, "Date format wrong");
      return f_err(-4, iline, cfile, cmes, cerr);
    }
    dmmm = dmmm + MJD_JST;

    if (i == 0 && dmmm > d1st) {
      sprintf(cmes, "No data in the past, start:%s", cstart);
      return f_err(-5, iline, cfile, cmes, cerr);
    }
    if (dmmm <= d2nd) {
      sprintf(cmes, "Date not an ascending order");
      return f_err(-6, iline, cfile, cmes, cerr);
    }
    if (dmmm < d1st) {
      i = 0;
    }
    d2nd = dmmm;

    dsign = 1.0e0;
    ddec = atof(ctmp[5]);
    if (strncmp(ctmp[5], "-", 1) == 0) {
      ddec = -ddec;
      dsign = -dsign;
    }
    ddeg[0] = atof(ctmp[2]) * 3600.0e0 + atof(ctmp[3]) * 60.0e0 + atof(ctmp[4]);
    ddeg[1] = (ddec * 3600.0e0 + atof(ctmp[6]) * 60.0e0 + atof(ctmp[7])) * dsign;
    ldrad[i][0] = ddeg[0] / 180.0e0 * M_PI / 240.0e0;
    ldrad[i][1] = ddeg[1] / 180.0e0 * M_PI / 3600.0e0;
    lddkm[i] = atof(ctmp[8]) * DAUNIT;
    if (ichk >= 10) {
      if (ival == 9) {
	sprintf(cmes, "Parameter shortage, PsAng data not exist");
	return f_err(-2, iline, cfile, cmes, cerr);
      }
      dwk = atof(ctmp[9]);
      if (fabs(dwk) >= 360.0e0) {
	iwk = (int) (fabs(dwk) / 360.0e0);
	if (dwk > 0.0e0) {
	  dwk = dwk - (360.0e0 * (double) iwk);
	} else {
	  dwk = dwk + (360.0e0 * (double) iwk) + 360.0e0;
	}
      }
      ldang[i] = dwk / 180.0e0 * M_PI;
    } else {
      ldang[i] = -9999.0e0;
    }
    ldmjd[i] = dmmm;
    i++;
    if (dmmm >= d1st + DAYSEC) {
      break;
    }
    if (i >= CMTMAX) {
      fprintf(stderr, "trk_03 Reached array max (%d)\n", CMTMAX);
      break;
    }
  }
  fclose(fid);
  if (i < 2) {
    sprintf(cerr, "trk_03 Number of data insufficient, start:%s", cstart);
    return -1;
  }
  *imax = i;
  return 0;
}

/*--------------------------------------------------------------------*/
/* 各パラメタ文字列 & 数値チェック                                    */
/*--------------------------------------------------------------------*/
static int f_leng(char *c1, char *c2, char *c3, char *c4, char *c5, char *c6, char *c7, char *c8, char *c9, char *c10, int ival, char *cmes)
#if 0
  char *c1;			/* 2004-May-19 */
  char *c2;			/* 00:00 */
  char *c3;			/* 04 */
  char *c4;			/* 24 */
  char *c5;			/* 29.5837 */
  char *c6;			/* -19 */
  char *c7;			/* 10 */
  char *c8;			/* 02.870 */
  char *c9;			/* 0.2658322437 */
  char *c10;			/* 280.985 */
  int  ival;
  char *cmes;
#endif
{
  char cchk[128];
  int  ichk;
  double dchk;

  if (strlen(c1) != 11) {
    sprintf(cmes, "Date format wrong");
    return -1;
  }
  if (strlen(c2) != 5 && strlen(c2) != 12) {
    sprintf(cmes, "Date format wrong");
    return -1;
  }
  if (sscanf(c9, "%le%s", &dchk, cchk) != 1) {
    sprintf(cmes, "delta format wrong");
    return -1;
  }
  if (dchk <= 0.0e0) {
    sprintf(cmes, "delta out of range");
    return -1;
  }
  if (ival >= 10) {
    if (sscanf(c10, "%le%s", &dchk, cchk) != 1) {
      sprintf(cmes, "PsAng format wrong");
      return -1;
    }
/*
    if (dchk < 0.0e0 || dchk > 360.0e0) {
      sprintf(cmes, "PsAng out of range");
      return -1;
    }
*/
  }
  /* RA 時 */
  if (strncmp(c3, "-", 1) == 0) {
    sprintf(cmes, "RA:HMS(H) format wrong");
    return -1;
  }
  if (sscanf(c3, "%d%s", &ichk, cchk) != 1) {
    sprintf(cmes, "RA:HMS(H) format wrong");
    return -1;
  }
  if (ichk < 0 || ichk >= 24) {
    sprintf(cmes, "RA:HMS(H) out of range");
    return -1;
  }
  /* RA 分 */
  if (strncmp(c4, "-", 1) == 0) {
    sprintf(cmes, "RA:HMS(M) format wrong");
    return -1;
  }
  if (sscanf(c4, "%d%s", &ichk, cchk) != 1) {
    sprintf(cmes, "RA:HMS(M) format wrong");
    return -1;
  }
  if (ichk < 0 || ichk >= 60) {
    sprintf(cmes, "RA:HMS(M) out of range");
    return -1;
  }
  /* RA 秒 */
  if (strncmp(c5, "-", 1) == 0) {
    sprintf(cmes, "RA:HMS(S) format wrong");
    return -1;
  }
  if (sscanf(c5, "%le%s", &dchk, cchk) != 1) {
    sprintf(cmes, "RA:HMS(S) format wrong");
    return -1;
  }
  if (dchk < 0.0e0 || dchk > 60.0e0) {
    sprintf(cmes, "RA:HMS(S) out of range");
    return -1;
  }
  /* DEC 度 */
  if (sscanf(c6, "%d%s", &ichk, cchk) != 1) {
    sprintf(cmes, "DEC:DMS(D) format wrong");
    return -1;
  }
  if (ichk <= -90 || ichk >= 90) {
    sprintf(cmes, "DEC:DMS(D) out of range");
    return -1;
  }
  /* DEC 分 */
  if (strncmp(c7, "-", 1) == 0) {
    sprintf(cmes, "DEC:DMS(M) format wrong");
    return -1;
  }
  if (sscanf(c7, "%d%s", &ichk, cchk) != 1) {
    sprintf(cmes, "DEC:DMS(M) format wrong");
    return -1;
  }
  if (ichk < 0 || ichk >= 60) {
    sprintf(cmes, "DEC:DMS(M) out of range");
    return -1;
  }
  /* DEC 秒 */
  if (strncmp(c8, "-", 1) == 0) {
    sprintf(cmes, "DEC:DMS(S) format wrong");
    return -1;
  }
  if (sscanf(c8, "%le%s", &dchk, cchk) != 1) {
    sprintf(cmes, "DEC:DMS(S) format wrong");
    return -1;
  }
  if (dchk < 0.0e0 || dchk > 60.0e0) {
    sprintf(cmes, "DEC:DMS(S) out of range");
    return -1;
  }
  return 0;
}

/*--------------------------------------------------------------------*/
/* 日付データの取得                                                   */
/*--------------------------------------------------------------------*/
static double f_date1(char *cstart)
#if 0
  char *cstart;
#endif
{
  int  iyear, imon;
  char ctmp[8];
  struct tm tsec;

  strncpy(ctmp, &cstart[0], 4);
  ctmp[4] = '\0';
  iyear = atoi(ctmp);
  strncpy(ctmp, &cstart[4], 2);
  ctmp[2] = '\0';
  imon = atoi(ctmp);
  strncpy(ctmp, &cstart[6], 2);
  ctmp[2] = '\0';
  tsec.tm_mday = atoi(ctmp);

  strncpy(ctmp, &cstart[8], 2);
  ctmp[2] = '\0';
  tsec.tm_hour = atoi(ctmp);
  strncpy(ctmp, &cstart[10], 2);
  ctmp[2] = '\0';
  tsec.tm_min = atoi(ctmp);
  strncpy(ctmp, &cstart[12], 2);
  ctmp[2] = '\0';
  tsec.tm_sec = atoi(ctmp);

  return f_tsec(iyear, imon, tsec);
}

static double f_date2(char *cday, char *cmin)
#if 0
  char *cday;
  char *cmin;
#endif
{
  int  i, iyear, imon;
  char ctmp[8];
  struct tm tsec;
  static char MON[12][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  strncpy(ctmp, &cday[0], 4);
  ctmp[4] = '\0';
  iyear = atoi(ctmp);

  strncpy(ctmp, &cday[5], 3);
  ctmp[3] = '\0';
  for (i = 0; i < 12; i++) {
    if (strcmp(ctmp, MON[i]) == 0) {
      break;
    }
  }
  if (i == 12) {
    return -1;
  }
  imon = i + 1;

  strncpy(ctmp, &cday[9], 2);
  ctmp[2] = '\0';
  tsec.tm_mday = atoi(ctmp);

  strncpy(ctmp, &cmin[0], 2);
  ctmp[2] = '\0';
  tsec.tm_hour = atoi(ctmp);
  strncpy(ctmp, &cmin[3], 2);
  ctmp[2] = '\0';
  tsec.tm_min = atoi(ctmp);
  if (strlen(cmin) == 5) {
    tsec.tm_sec = 0;
  } else {
    strncpy(ctmp, &cmin[6], 2);
    ctmp[2] = '\0';
    tsec.tm_sec = atoi(ctmp);
  }

  return f_tsec(iyear, imon, tsec);
}

/*--------------------------------------------------------------------*/
/* MJD 算出                                                           */
/*--------------------------------------------------------------------*/
static double f_tsec(int iyear, int imon, struct tm tsec)
#if 0
  int  iyear;
  int  imon;
  struct tm tsec;
#endif
{
  char ccal[16];
  double dmjd;
  int  iflg;
  static int DAY[12][2] = {
    {31, 31}, {28, 29}, {31, 31}, {30, 30}, {31, 31}, {30, 30},
    {31, 31}, {31, 31}, {30, 30}, {31, 31}, {30, 30}, {31, 31}
  };

  if ((iyear > 2038 || iyear < 1970) ||
      (imon > 12 || imon < 1) ||
      (tsec.tm_mday > 31 || tsec.tm_mday < 1) ||
      (tsec.tm_hour > 23 || tsec.tm_hour < 0) ||
      (tsec.tm_min > 59 || tsec.tm_min < 0) ||
      (tsec.tm_sec > 59 || tsec.tm_sec < 0)) {
    return -1;
  }
  iflg = f_leap(iyear);
  if (tsec.tm_mday > DAY[imon - 1][iflg]) {
    return -1;
  }
  sprintf(ccal, "%04d%02d%02d%02d%02d%02d",
	  iyear, imon, tsec.tm_mday, tsec.tm_hour, tsec.tm_min, tsec.tm_sec);
  if (tjcd2m(ccal, &dmjd) != 0) {
    return -1;
  }
  return dmjd;
}

/*--------------------------------------------------------------------*/
/* 閏年の判別                                                         */
/*--------------------------------------------------------------------*/
static int f_leap(int YY)
#if 0
  int  YY;
#endif
{
  if (YY < 100) {
    if (YY > 90) {
      YY = 1900 + YY;
    } else {
      YY = 2000 + YY;
    }
  }
  if ((YY - (YY / 4) * 4) != 0) {
    return 0;
  } else if ((YY - (YY / 100) * 100) != 0) {
    return 1;
  } else if ((YY - (YY / 400) * 400) != 0) {
    return 0;
  } else {
    return 1;
  }
}
