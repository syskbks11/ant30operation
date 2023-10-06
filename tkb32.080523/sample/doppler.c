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

  /* trk_velo用変数 */
  static double dvrad;

  /* 作業用変数 */
  char cerr[256];
  int  i, ival;

  static char cenvdt[3][128] = {
    "ANTFILE=../ant_err.dat",
    "EPHFILE=../ephfile/19241221-20500102.ORG",
    "TIMEFILE=../time.dat"
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
  ipflg = 3;                    /* 処理フラグ 3:DOPPLER */
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
  if (trk_00("20061018000000.0", "", cerr) != 0) {
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

  xin.ivdef = 1;                /* 視線速度の定義系 1:RAD 2:OPT */
  xin.ivref = 1;                /* 視線速度の基準系 1:LSR 2:HEL 3:SOLAR or COMET
                                   ※惑星 or 彗星観測での基準は測心位置となる */
  xin.dobsfq = 4.3e10;          /* 静止トラッキング周波数 [Hz] */

  /*
   * trk_20, trk_30は刻々と時刻が変化するため、本来ループ処理になる
   */
  /*--------------------------------------------------------------------------------*/
  /* trk_20 (観測該当時刻の設定 YYYYMMDDhhmmss.0 (JST))                             */
  /*--------------------------------------------------------------------------------*/
  strcpy(cstart, "20061018000010.0");
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
  /* VRAD (アンテナから見た視線速度) の算出                                         */
  /*--------------------------------------------------------------------------------*/
  trk_velo_(&dvrad);

  /**********************************************************************************/
  /* 結果出力                                                                       */
  /**********************************************************************************/
  printf("START => %s\n", cstart);
  printf("Observing Frequency        [Hz]: %+10.9lf\n", xin.dobsfq);
  printf("Doppler Tracking Frequency [Hz]: %+10.9lf\n", xout.dflkfq);
  printf("VRAD                      [m/s]: %+10.9lf\n", dvrad);
}
