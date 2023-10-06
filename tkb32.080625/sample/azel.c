#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "track.h"

MAIN__(void)
{
  /* trk_10用変数 */
  int  ipflg, ioflg, iaoflg, ibjflg;
  double drdlv, dsourc[2];

  /* trk_20用変数 */
  char cstart[24];
  double dtime;
  XTRKI xin;
  XTRKO xout;

  /* trk_30用変数 */
  double diazel[2], doazel[2], dorade[2], dweath[3];
  static double dgtloc, dgtdat[8][2];

  /* 作業用変数 */
  char cerr[256];
  int  i, ival;

  static char cenvdt[3][128] = {
    "ANTFILE=../../../etc/ant_err.dat",
    "EPHFILE=../../../etc/ephfile/19241221-20500102.ORG",
    "TIMEFILE=../../../etc/time.dat"
  };

  /*--------------------------------------------------------------------------------*/
  /* 初期設定                                                                       */
  /*--------------------------------------------------------------------------------*/
  memset(&xin, 0, sizeof(xin));
  memset(&xout, 0, sizeof(xout));

  /* 環境変数の宣言 */
  for (i = 0; i < 3; i++) {
    if (putenv(cenvdt[i]) != 0) {
      fprintf(stderr, "Cannot add value to environment (%s)\n", cenvdt[i]);
      exit(-1);
    }
  }

  /**********************************************************************************/
  /* (1) 追尾計算用パラメタ設定 (静的情報 trk_10)                                   */
  /**********************************************************************************/
  ipflg = 0;                    /* 処理フラグ 0:AZEL */
  ioflg = 0;                    /* 天体フラグ 0:太陽系外 1-11:惑星 12:COMET 99:AZEL */
  iaoflg = 1;                   /* 位置座標系 0:惑星 1:RADEC 2:LB 3:AZEL */
  ibjflg = 2;                   /* 分点フラグ 1:B1950 2:J2000 */
  drdlv = 3000.0e0;             /* 天体の視線速度 [m/s] */
  dsourc[0] = 0.52359878e0;     /* 天体位置 [rad] */
  dsourc[1] = 0.52359878e0;     /* 天体位置 [rad] */

  /*
   * trk_00, trk_10は1度だけ呼べば良い、ただし日を跨る場合は再度呼び出す
   */
  /*--------------------------------------------------------------------------------*/
  /* trk_00 (観測開始時刻の設定 YYYYMMDDhhmmss.0 (JST))                             */
  /*--------------------------------------------------------------------------------*/
  if (trk_00("20061118000000.0", "", cerr) != 0) {
    fprintf(stderr, "%s\n", cerr);
    exit(-1);
  }
  /*--------------------------------------------------------------------------------*/
  /* trk_10                                                                         */
  /*--------------------------------------------------------------------------------*/
  ival = trk_10(ioflg, ipflg, iaoflg, ibjflg, dsourc, drdlv);
  if (ival == 1) {
    fprintf(stderr, "trk_10: Argument check error\n");
    exit(-1);
  } else if (ival == 2) {
    fprintf(stderr, "trk_10: Common area initial error\n");
    exit(-1);
  }

  /**********************************************************************************/
  /* (2) 追尾計算用パラメタ設定 (動的情報 trk_20)                                   */
  /**********************************************************************************/
  xin.irpflg = 1;               /* ポインティング 0:虚角 1:実角 */
  xin.iapflg = 1;               /* ポインティング 1:RADEC 2:LB 3:AZEL */
  xin.dpofst[0] = 0.0e0;        /* ポインティングオフセット [rad] RADEC or LB */
  xin.dpofst[1] = 0.0e0;
  xin.dpofaz[0] = 0.0e0;        /* ポインティングオフセット [rad] AZEL */
  xin.dpofaz[1] = 0.0e0;

  xin.irsflg = 1;               /* スキャンニング 0:虚角 1:実角 */
  xin.iasflg = 1;               /* スキャンニング 1:RADEC 2:LB 3:AZEL */
  xin.dscnst[0] = 0.1e0;        /* スキャンニング開始オフセット [rad] */
  xin.dscnst[1] = 0.2e0;
  xin.dscned[0] = 0.1e0;        /* スキャンニング終了オフセット [rad] */
  xin.dscned[1] = 0.2e0;

  xin.itnscn = 1;               /* スキャンニング点数 */
  xin.inoscn = 1;               /* スキャンニング点 */

  /*
   * trk_20, trk_30は刻々と時刻が変化するため、本来ループ処理になる
   */
  /*--------------------------------------------------------------------------------*/
  /* trk_20 (観測該当時刻の設定 YYYYMMDDhhmmss.0 (JST))                             */
  /*--------------------------------------------------------------------------------*/
  strcpy(cstart, "20061118000110.0");
  if (tjcd2m(cstart, &dtime) == 1) {  /* JST時系の暦日付からMJDへの変換 */
    fprintf(stderr, "tjcd2m: Error\n");
    exit(-1);
  }
  ival = trk_20(dtime, xin, &xout);
  if (ival == -1) {
    fprintf(stderr, "trk_20: Argument check error\n");
    exit(-1);
  } else if (ival == -2) {
    fprintf(stderr, "trk_20: Local error end\n");
    exit(-1);
  }
  /*--------------------------------------------------------------------------------*/
  /* trk_30 (大気差補正)                                                            */
  /*--------------------------------------------------------------------------------*/
  diazel[0] = xout.dazel[0];
  diazel[1] = xout.dazel[1];
  trk_ant_(dgtdat[0], dgtdat[1], dgtdat[2], dgtdat[3], dgtdat[4],
           dgtdat[5], dgtdat[6], dgtdat[7], &dgtloc);

  dweath[0] = 23.3e0;           /* 気温 [℃] */
  dweath[1] = 865.0e0;          /* 気圧 [hPa] */
  dweath[2] = 18.25e0;          /* 水蒸気圧 [hPa] */

  printf("%.9lf %.9lf\n", dgtdat[0][0], dgtdat[0][1]);

  if (trk_30(diazel, dweath, dgtdat[0], dgtloc, doazel, dorade) != 0) {
    fprintf(stderr, "trk_30: Local error end\n");
    exit(-1);
  }

  /**********************************************************************************/
  /* 結果出力                                                                       */
  /**********************************************************************************/
  printf("START => %s\n", cstart);
  printf("AZ  [rad]    : %+10.9lf\n", doazel[0]);
  printf("EL  [rad]    : %+10.9lf\n", doazel[1]);
  printf("DAZ [rad/sec]: %+10.9lf\n", xout.ddazel[0]);
  printf("DEL [rad/sec]: %+10.9lf\n", xout.ddazel[1]);

  /* マップセンター座標 */
  printf("MAP [rad]    : %+10.9lf %+10.9lf (RADEC)\n", dgtdat[1][0], dgtdat[1][1]);
  printf("MAP [rad]    : %+10.9lf %+10.9lf (LB)\n",    dgtdat[2][0], dgtdat[2][1]);
  /* 観測位置座標 */
  printf("OBS [rad]    : %+10.9lf %+10.9lf (RADEC)\n", dgtdat[4][0], dgtdat[4][1]);
  printf("OBS [rad]    : %+10.9lf %+10.9lf (LB)\n",    dgtdat[5][0], dgtdat[5][1]);
}
