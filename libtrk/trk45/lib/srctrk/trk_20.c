/*--------------------------------------------------------------------*/
/*                                                                    */
/*  関数名   : trk_20                                                 */
/*                                                                    */
/*  和名     : 動作ローカル処理                                       */
/*                                                                    */
/*  機能概要 : 指定された動作ローカル処理を行う。                     */
/*                                                                    */
/*  戻り値   : =  0: 正常終了                                         */
/*             = -1: 引数チェックでエラー終了                         */
/*             = -2: ローカル処理中でエラー終了                       */
/*                                                                    */
/*  注意事項 :                                                        */
/*                                                                    */
/*  作成者   : FUJITSU LIMITED    1996.06.18                          */
/*                                                                    */
/*  メモ     :                                                        */
/*             MODIFY 1997.04.03 ( FLUKE_FREQ  FNS )                  */
/*             MODIFY 1997.09.30 ( WEATHER     FNS )                  */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>
#include	"track.h"	/* 追尾計算入出力データ構造体 */
#include	"trkmod.h"	/* 追尾デフォルトパラメタ */
#include	"trkmsg.h"	/* エラーメッセージ */

#define PI (double)3.1415926535897932e0

trk_20( dtime, xin, xout )

double	dtime;			/* 計算時刻(MJD) */
XTRKI	xin;			/* アンテナのＶＬＢＩ座標位置ベクトル */
XTRKO	*xout;			/* アンテナの機差補正値 */

{

  double dantintvl = 0.2;       /* sec */
  int	iret;			/* リターンコード */
  static double	dpofst[2];	/* ポインティングオフセット [RADEC,LB] */
  static double	dpofaz[2];	/* ポインティングオフセット [AZEL]     */
  static double	dscnst[2];	/* スキャン開始位置 */
  static double	dscned[2];	/* スキャン終了位置 */
  static double	dintrl;		/* フリンジローテータ制御間隔[sec] */
  static double	ddofst;		/* 遅延時間オフセット */
  static double	dobsfq;		/* 観測周波数(=静止周波数) */
  static double	dtrnfq[2];	/* 周波数変換係数 */
  static double	dfofst;		/* 周波数オフセット */
  static double	dlclfq;		/* 中間周波数 */
  static int	itnscn;		/* スキャン点数 */
  static int	inoscn;		/* スキャン点 */
  static int	irpflg;		/* ポインティング実角フラグ */
  static int	iapflg;		/* ポインティング角フラグ */
  static int	irsflg;		/* スキャン実角フラグ */
  static int	iasflg;		/* スキャン角フラグ */
  static int	idlflg;		/* 遅延追尾フラグ */
  static int	idsbfg;		/* ＤＳＢフラグ */
  static int	iulsel;		/* Ｕ／Ｌフラグ */
  static int	irotdr;		/* フリンジ回転方向 */
  static int	inlocl;		/* 第一ローカル発振器逓倍次数 */
  static int	inflck;		/* フルークシンセサイザ逓倍次数 */
  static int	itrkfq;		/* トラッキング周波数種別 */
  static int	ivref; 		/* 視線速度の基準系フラグ */
  static int	ivdef; 		/* 視線速度の定義系フラグ */
  static double dcomet[6];      /* 彗星６要素 */
  static double dsrc[2];	/* 天体座標 (RADEC) [2] [rad] */
  static double dist;		/* 地心からの天体の距離 [m] */
  static double delt[3];	/* デカルト成分での変化量 [m] */

  static double	dazel[2];	/* アンテナ指令値 */
  static double	dazel1[2];	/* アンテナ指令値 */
  static double	ddazel[2];	/* アンテナ速度 */
  static double	dtaug[3];	/* 遅延時間 */
  static double	dphas[2];	/* 初期位相 */
  static double	ddphas[2];	/* 位相変化量 */
  static double	dflkfq;		/* 受信周波数 */
  static double	duvw[3];	/* ＵＶＷ */
  static double	dhoura;		/* アワーアングル */
  static double	dtime1;
  static int    frstfg;
  char		cwrk[32];

  /* 入力値 */
  dpofst[0] = xin.dpofst[0];	/* ポインティングオフセット [RADEC,LB] */
  dpofst[1] = xin.dpofst[1];	/* ポインティングオフセット [RADEC,LB] */
  dpofaz[0] = xin.dpofaz[0];	/* ポインティングオフセット [AZEL] */
  dpofaz[1] = xin.dpofaz[1];	/* ポインティングオフセット [AZEL] */
  dscnst[0] = xin.dscnst[0];	/* スキャン開始位置 */
  dscnst[1] = xin.dscnst[1];	/* スキャン開始位置 */
  dscned[0] = xin.dscned[0];	/* スキャン終了位置 */
  dscned[1] = xin.dscned[1];	/* スキャン終了位置 */
  dintrl = xin.dintrl;		/* 制御間隔 */
  ddofst = xin.ddofst;		/* 遅延時間オフセット */
  dobsfq = xin.dobsfq;		/* 観測周波数(=静止周波数) */
  dtrnfq[0] = xin.dtrnfq[0];	/* 周波数変換係数-------未使用 */
  dtrnfq[1] = xin.dtrnfq[1];    /* 周波数変換係数-------未使用 */
  dfofst = xin.dfofst;		/* 初期位相オフセット */
  dlclfq = xin.dlclfq;		/* 中間周波数-----------未使用 */
  itnscn = xin.itnscn;		/* スキャン総点数 */
  inoscn = xin.inoscn;		/* スキャン点 */
  irpflg = xin.irpflg;		/* ポインティング実角フラグ */
  iapflg = xin.iapflg;		/* ポインティング角フラグ */
  irsflg = xin.irsflg;		/* スキャン実角フラグ */
  iasflg = xin.iasflg;		/* スキャン角フラグ */
  idsbfg = xin.idsbfg;		/* ＤＳＢフラグ---------未使用 */
  idlflg = xin.idlflg;		/* 遅延追尾フラグ */
  iulsel = xin.iulsel;		/* Ｕ／Ｌフラグ */
  irotdr = xin.irotdr;		/* フリンジ回転方向 */
  inlocl = xin.inlocl;		/* 発振器逓倍次数-------未使用 */
  inflck = xin.inflck;		/* シンセ逓倍次数-------未使用 */
  itrkfq = xin.itrkfq;		/* トラッキング周波数種別 */
  ivref  = xin.ivref; 		/* 視線速度の基準系フラグ */
  ivdef  = xin.ivdef; 		/* 視線速度の定義系フラグ */

  dcomet[0] = xin.dcomet[0] / 86400.0; /* 近日点通過時刻 ＭＪＤ[DAY]   */
  dcomet[1] = xin.dcomet[1];    /* 近日点距離     0.0 〜 99.999999[AU] */
  dcomet[2] = xin.dcomet[2];    /* 離心率         0.0 〜 99.999999     */
  dcomet[3] = xin.dcomet[3];    /* 近日点引数     [rad] */
  dcomet[4] = xin.dcomet[4];    /* 昇降点黄経     [rad] */
  dcomet[5] = xin.dcomet[5];    /* 軌道傾斜角     [rad] */
  
  /* 引数チェック */
  /* スキャン点数 */
  if( ( itnscn < 1 ) ){
    fprintf( stderr, "%s%d\n", CMTRK2000, itnscn );
    return( -1 );
  }
  /* スキャン点 */
  if( ( inoscn < 1 ) || ( inoscn > itnscn ) ){
    fprintf( stderr, "%s%d\n", CMTRK2001, inoscn );
    return( -1 );
  }
  /* ポインティング実角フラグ */
  if( irpflg != 1 ){
    fprintf( stderr, "%s%d\n", CMTRK2002, irpflg );
    return( -1 );
  }
  /* ポインティング角フラグ */
  if( ( iapflg < 1 ) || ( iapflg > 3 ) ){
    fprintf( stderr, "%s%d\n", CMTRK2003, iapflg );
    return( -1 );
  }
  /* スキャン実角フラグ */
  if( irsflg != 1 ){
    fprintf( stderr, "%s%d\n", CMTRK2004, irsflg );
    return( -1 );
  }
  /* スキャン角フラグ */
  if( ( iasflg < 1 ) || ( iasflg > 3 ) ){
    fprintf( stderr, "%s%d\n", CMTRK2005, iasflg );
    return( -1 );
  }

  /* 彗星観測の場合、線形補間によるデータ算出 */
  if (t_cmt.imax != -1) {
    if (trk_20_cmt(dtime, &dsrc[0], &dist, &delt[0]) != 0) {
      return( -2 );
    }
  } else {
    dsrc[0] = 0.0e0;
    dsrc[1] = 0.0e0;
    dist = -1.0e0;
    delt[0] = 0.0e0;
    delt[1] = 0.0e0;
    delt[2] = 0.0e0;
  }

  /* 速度成分の初期化 */
  ddazel[0] = 0.0;
  ddazel[1] = 0.0;
  dtime1 = dtime;
  iret = 0;

  /* ローカル処理へ値を渡す */
  frstfg = 1;
  trk_21_( &dtime1,  dpofst,  dscnst,  dscned, &dobsfq,
            dtrnfq, &dintrl, &ddofst, &dfofst, &dlclfq, &itnscn,
           &inoscn, &irpflg, &iapflg, &irsflg, &iasflg, &idsbfg,
           &idlflg, &iulsel, &irotdr, &inlocl, &inflck, &itrkfq,
             dazel,   dtaug,   dphas,  ddphas, &dflkfq,    duvw,
           &dhoura,   &iret,  &ivref,  &ivdef,  dpofaz, &frstfg,
            dcomet, dsrc, &dist, delt );
  if( iret >= 100 ){
    ;
  }
  else if( iret < 0 ){
    fprintf( stderr, "%s %d\n", CMTRK2012, iret );
    return( -2 );
  }

  xout->dazel[0] = dazel[0];		/* アンテナの方向 */
  xout->dazel[1] = dazel[1];
  xout->ddazel[0] = ddazel[0];		/* アンテナの速度 */
  xout->ddazel[1] = ddazel[1];
  xout->dtaug[0] = dtaug[0];		/* 遅延時間 */
  xout->dtaug[1] = dtaug[1];
  xout->dtaug[2] = dtaug[2];
  dphas[0] = dphas[0] - (int)dphas[0];	/* 初期位相 */
  dphas[1] = dphas[1] - (int)dphas[1];
  xout->dphas[0] = dphas[0];
  xout->dphas[1] = dphas[1];
  xout->ddphas[0] = ddphas[0];		/* フリンジ周波数 */
  xout->ddphas[1] = ddphas[1];
  xout->dflkfq = dflkfq;		/* 受信周波数 */
  xout->duvw[0] = duvw[0];		/* ＵＶＷ */
  xout->duvw[1] = duvw[1];
  xout->duvw[2] = duvw[2];
  xout->dhoura = dhoura;		/* アワーアングル */

  return( iret );

}
