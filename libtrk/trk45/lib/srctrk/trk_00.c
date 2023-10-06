/*--------------------------------------------------------------------*/
/*                                                                    */
/*  関数名   : trk_00                                                 */
/*                                                                    */
/*  和名     : 追尾計算モジュール共通領域の初期化                     */
/*                                                                    */
/*  機能概要 : 追尾計算モジュールの共通領域へ初期値を入力する。       */
/*                                                                    */
/*  戻り値   : =  0: 正常終了                                         */
/*             =  1: 入力日付の書式でエラー終了                       */
/*             =  2: 共通領域の初期設定でエラー終了                   */
/*             =  3: ＥＰＨ、ＴＩＭＥファイルのリードでエラー終了     */
/*                                                                    */
/*  注意事項 : 本モジュールはｔｒｋ＿ｘｘのモジュールを使用する場合、 */
/*            必ずプログラムの先頭で一度呼ばなければならない。        */
/*                                                                    */
/*  作成者   : FUJITSU LIMITED    1996.06.10                          */
/*                                                                    */
/*  メモ     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>
#include	"trkmod.h"	/* 追尾デフォルトパラメタ */
#include	"trkmsg.h"	/* エラーメッセージ */

trk_00( cjstdate, cjpl, cerr )

char	cjstdate[];		/* 計算開始ＪＳＴ日付 */
char	cjpl[];			/* ＪＰＬ彗星ファイル */
char	cerr[];			/* エラーメッセージ */

{

  static double	dmjdjst;	/* 計算開始日本ＭＪＤ時刻[day] */
  static double	dmjd;		/* 計算開始ＭＪＤ時刻[day] */
  static double	deph[248];	/* ＥＰＨデータ */
  static double	dtime[3];	/* ＴＩＭＥデータ */
  static int	iret;		/* リターンコード */

  /* 日付をＭＪＤへ変換する */
  if( tjcd2m( cjstdate, &dmjd ) ){
    sprintf( cerr, "%s%s", CMTRK0000, cjstdate );
    return( 1 );
  }
  dmjdjst = ( dmjd + MJD_JST ) / DAYSEC;	/* 日本ＭＪＤ */
  dmjd = dmjd / DAYSEC;				/* グリニッジＭＪＤ */
#ifdef DEBUG
printf( "JMJD=%f\n", dmjdjst );
printf( "GMJD=%f\n", dmjd );
printf( "DMJD=%f\n", dmjdjst-dmjd );
#endif

  /* ＴＩＭＥ／ＥＰＨファイルからデータを読み込む */
  if( trk_02( dmjd, deph, dtime ) < 0 ){
    sprintf( cerr, "%s", CMTRK0002 );
    return( 3 );
  }
  /* JPL 彗星ファイルからデータを読み込む */
  t_cmt.index = -1;
  t_cmt.imax = -1;
  if (strlen(cjpl) != 0) {
    if (trk_03_cmt(cjstdate, cjpl, &t_cmt.imax,
      &t_cmt.ldmjd[0], &t_cmt.ldrad[0][0], &t_cmt.lddkm[0], cerr) != 0) {
      return( 4 );
    }
  }

  /* ＦＯＲＴＲＡＮの共通領域へ値を代入する */
/* 980106 yanaka */
  dmjdjst = dmjdjst * DAYSEC;
/* 980106 yanaka */
  trk_01_( &dmjdjst, deph, dtime, &iret );
  if( iret != 0 ){
    sprintf( cerr, "%s", CMTRK0001 );
    return( 2 );
  }

  return( 0 );

}
