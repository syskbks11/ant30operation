/*--------------------------------------------------------------------*/
/*                                                                    */
/*  関数名   : tjcd2m                                                 */
/*                                                                    */
/*  和名     : ＪＳＴ時系の暦の日付からＭＪＤへの変換                 */
/*                                                                    */
/*  機能概要 : ＪＳＴ時系の暦の日付(YYYYMMDDhhmmss.ss.... または      */
/*             YYMMDDhhmmss.ss...)から修正ユリウス日[sec]へ変換を行う */
/*                                                                    */
/*  戻り値   : =  0: 正常終了                                         */
/*             =  1: 入力文字列のフォーマットエラーで終了             */
/*                                                                    */
/*  注意事項 : なし                                                   */
/*                                                                    */
/*  作成者   : FUJITSU LIMITED    1996.05.29                          */
/*                                                                    */
/*  メモ     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>
#include	"trkmod.h"	/* 追尾デフォルトパラメタ */
#include	"trkmsg.h"	/* エラーメッセージ */

tjcd2m( ctime, dtime )

char	ctime[];		/* 日付ＪＳＴ */
double	*dtime;			/* ＭＪＤ */

{
  int           i;
  int		ildate;		/* 日付の文字列長 */
  char		cwrk[150];
  double	ddate[6];	/* 日付 */
  double	dwrk, dm = 0.0;

  *dtime = 0.0e0;

  /* 入力文字列のチェック */
  ildate = strlen( ctime );
  if (ildate == 0) {
    fprintf( stderr, "%slength=0\n", CMTJCD2M00 );
    return( 1 );
  }
  /* 小数点以下の値を求める */
  for ( i = 0 ; i < ildate ; i++ ) if (ctime[i] == '.') break;
  if (i < ildate) {
    ildate = i;
    dm = atof( &ctime[ildate] );
  }

  /* 入力文字列の分解 */
  strncpy(cwrk, ctime, ildate);
  cwrk[ildate] = 0;
  dwrk = atof( cwrk );
  /* 年 */
  ddate[0] = (double)( (int)( dwrk / 10000000000.0 ) );
  /* 月 */
  dwrk = dwrk - ddate[0] * 10000000000.0;
  ddate[1] = (double)( (int)( dwrk / 100000000.0 ) );
  /* 日 */
  dwrk = dwrk - ddate[1] * 100000000.0;
  ddate[2] = (double)( (int)( dwrk / 1000000.0 ) );
  /* 時 */
  dwrk = dwrk - ddate[2] * 1000000.0;
  ddate[3] = (double)( (int)( dwrk / 10000.0 ) );
  /* 分 */
  dwrk = dwrk - ddate[3] * 10000.0;
  ddate[4] = (double)( (int)( dwrk / 100.0 ) );
  /* 秒 */
  ddate[5] = (int)(dwrk - ddate[4] * 100.0) + dm;

  /* 入力日付のチェック */
  if( ildate == 14 ) ddate[0] = ddate[0] - 1900.0;
  /* 年 */
  if( ddate[0] < 50.0 ) ddate[0] = ddate[0] + 100.0;
  if( ddate[0] < 1.0 || ddate[0] > 200.0 ){
    fprintf( stderr, "%syear=%f\n", CMTJCD2M00, ddate[0] );
    return( 1 );
  }
  /* 月 */
  if( ddate[1] < 1.0 || ddate[1] > 12.0 ){
    fprintf( stderr, "%smonth=%f\n", CMTJCD2M00, ddate[1] );
    return( 1 );
  }
  /* 日 */
  if( ddate[1] == 2.0 ){
    if( ddate[2] < 1.0 || ddate[2] > 29.0 ){
      fprintf( stderr, "%sday=%f\n", CMTJCD2M00, ddate[2] );
      return( 1 );
    }
  }
  else if( ddate[1] == 4.0 || ddate[1] == 6.0 ||
           ddate[1] == 9.0 || ddate[1] == 11.0 ){
    if( ddate[2] < 1.0 || ddate[2] > 30.0 ){
      fprintf( stderr, "%sday=%f\n", CMTJCD2M00, ddate[2] );
      return( 1 );
    }
  }
  else{
    if( ddate[2] < 1.0 || ddate[2] > 31.0 ){
      fprintf( stderr, "%sday=%f\n", CMTJCD2M00, ddate[2] );
      return( 1 );
    }
  }
  /* 月,年 */
  if( ddate[1] < 3.0 ){
    ddate[1] = ddate[1] + 12.0;
    ddate[0] = ddate[0] - 1.0;
  }
  /* 時 */
  if( ddate[3] < 0.0 || ddate[3] > 23.0 ){
    fprintf( stderr, "%shour=%f\n", CMTJCD2M00, ddate[3] );
    return( 1 );
  }
  /* 分 */
  if( ddate[4] < 0.0 || ddate[4] > 59.0 ){
    fprintf( stderr, "%sminute=%f\n", CMTJCD2M00, ddate[4] );
    return( 1 );
  }
  /* 秒 */
  if( ddate[5] < 0.0 || ddate[5] >= 60.0 ){
    fprintf( stderr, "%ssecnd=%f\n", CMTJCD2M00, ddate[5] );
    return( 1 );
  }

  /* Ｊ２０００．０年元期からの経過時間 */
  dwrk = 365.0 * ddate[0] + 30.0 * ddate[1] + ddate[2] - 33.5
       + (int)( 3.0 * ( ddate[1] + 1.0 ) / 5.0 )
       + (int)( ddate[0] / 4.0 ) - 36525.0;

  /* 日本標準時系でのＭＪＤへ変換 */
  dwrk = D_J2000 + dwrk;

  /* 日から秒への単位変換 */
  dwrk = dwrk * 86400.0
       + ( ddate[3] * 60.0 + ddate[4] ) * 60.0 + ddate[5];

  /* ＭＪＤ */
  *dtime = dwrk - MJD_JST;

  return( 0 );

}
