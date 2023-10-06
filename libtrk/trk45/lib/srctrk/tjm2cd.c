/*--------------------------------------------------------------------*/
/*                                                                    */
/*  関数名   : tjm2cd                                                 */
/*                                                                    */
/*  和名     : ＭＪＤからＪＳＴ時系の暦の日付への変換                 */
/*                                                                    */
/*  機能概要 : 修正ユリウス日（ＭＪＤ）[sec]からＪＳＴ時系における暦  */
/*             の日付（YYYYMMDDhhmmss.ss...）へ変換を行う             */
/*                                                                    */
/*  戻り値   : =  0: 正常終了                                         */
/*             =  1: 入力値の範囲がサポートする時間範囲を越えている   */
/*                                                                    */
/*  注意事項 : なし                                                   */
/*                                                                    */
/*  作成者   : FUJITSU LIMITED    1996.05.29                          */
/*                                                                    */
/*  メモ     : 1996.11.28 改修                                        */
/*                                                                    */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>
#include	"trkmod.h"	/* 追尾デフォルトパラメタ */
#include	"trkmsg.h"	/* エラーメッセージ */

tjm2cd( dtime, ctime )

double	dtime;			/* ＭＪＤ */
char	ctime[];		/* 日付 */

{

  double	djd;		/* ＪＤ */
  double	ddtime;		/* ＪＭＪＤ */
  double	ditime;		/* ＭＪＤの端数 */
  double	dsec;
  int		idate[6];	/* 日付 */
  int		iwrk[7];
  double	dwrk1;
  double	dwrk;

  /* ＭＪＤからＪＤへ変換 */
  ddtime = dtime + 9.0 * 3600.0;
  djd = 2400000.5 + ddtime / 86400.0;
  dwrk1 = ( djd - (int)djd ) + 0.5;
  /* ＭＪＤの日以上と未満へ分離 */
  ditime = ddtime -( (int)( ddtime / 86400.0 ) ) * 86400.0;

  /* 計算 */
  iwrk[0] = (int)( djd + 68569.5 );
  iwrk[1] = (int)( (double)iwrk[0] / 36524.25 );
  dwrk = (double)36524.25 * (double)iwrk[1] + 0.75;
  iwrk[2] = iwrk[0] - (int)( dwrk );
  iwrk[3] = (double)( iwrk[2] + 1 ) / 365.25025 ;
  dwrk = (double)365.25 * (double)iwrk[3];
  iwrk[4] = iwrk[2] - (int)dwrk + 31;
  iwrk[5] = iwrk[4] / 30.59;
  iwrk[6] = iwrk[5] / 11;

  /* 年 */
  idate[0] = 100 * ( iwrk[1] - 49 ) + iwrk[3] + iwrk[6];
  /* 月 */
  idate[1] = iwrk[5] - 12 * iwrk[6] + 2;
  /* 日 */
  idate[2] = iwrk[4] - (int)( 30.59 * iwrk[5] );
  if( idate[2] == 32 ){
    idate[2] = 1;
    idate[1] = 1;
    idate[0] = idate[0] + 1;
  }
  /* 時 */
  idate[3] = (int)( ditime / 3600 );
  /* 分 */
  idate[4] = (int)( ( ditime - idate[3] * 3600 ) / 60 );
  /* 秒 */
  dsec = ditime - idate[3] * 3600 - idate[4] * 60;

  /* 出力値 */
  sprintf( ctime, "%04d%02d%02d%02d%02d%016.13f\0",
           idate[0], idate[1], idate[2], idate[3], idate[4], dsec );

  return( 0 );

}
