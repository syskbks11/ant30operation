/*--------------------------------------------------------------------*/
/*                                                                    */
/*  関数名   : trk_02                                                 */
/*                                                                    */
/*  和名     : ＥＰＨ、ＴＩＭＥファイルのリード                       */
/*                                                                    */
/*  機能概要 : ＥＰＨ、ＴＩＭＥファイルから観測時間帯のデータを読み込 */
/*             む。                                                   */
/*                                                                    */
/*  戻り値   : >  0: 正常終了                                         */
/*             = -1: ＥＰＨファイル名の取得エラー                     */
/*             = -2: ＴＩＭＥファイル名の取得エラー                   */
/*             = -3: ＥＰＨファイルのオープンエラー                   */
/*             = -4: ＴＩＭＥファイルのオープンエラー                 */
/*                                                                    */
/*  注意事項 : なし                                                   */
/*                                                                    */
/*  作成者   : FUJITSU LIMITED  1996.06.24                            */
/*                                                                    */
/*  メモ     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trkmsg.h"
#include "trkmod.h"

#define  BUFF 1024

trk_02( dmjd, deph, dtime )
double	dmjd;                   /* 観測開始ＭＪＤ[day]                */
double	deph[248];              /* ＥＰＨデータ                       */
double	dtime[3];               /* ＴＩＭＥファイル                   */
{
  double        djd;            /* ユリウス日                         */
  double        dd;             /* ファイルデータの時刻               */
  double        dd0, d0;
  int           i0;

  FILE          *fid;           /* ファイルポインタ                   */
  char          *cenv;          /* 環境変数取得領域                   */
  char          *p;
  char          buff[128];
  char          cfile[2][80];   /* ファイル名                         */
  char          temp[4][40];    /* 一時バッファ変数                   */
  char          dummy[BUFF];    /* 読みだし用バッファ                 */
  char          buff1[BUFF];
  char          bedata[BUFF];   /* 該当データ                         */
  int           i, j, k;        /* ループ制御変数                     */
  int           flag;           /* 該当データ有無フラグ               */

  /*------------------------------------------------------------------*/
  /* 初期化                                                           */
  /*------------------------------------------------------------------*/
  bedata[0] = '\0';

  /*------------------------------------------------------------------*/
  /* ファイル名の取得                                                 */
  /*------------------------------------------------------------------*/
  if( ( cenv = getenv( "TIMEFILE" ) ) == NULL ){
    fprintf( stderr, "%s\n", CMTRK0200 );
    return( -1 );
  }
  sprintf( cfile[0], "%s\0", cenv );
  if( ( cenv = getenv( "EPHFILE" ) ) == NULL ){
    fprintf( stderr, "%s\n", CMTRK0201 );
    return( -2 );
  }
  sprintf( cfile[1], "%s\0", cenv );

  /*------------------------------------------------------------------*/
  /* ＴＩＭＥデータの取得                                             */
  /*------------------------------------------------------------------*/
  if( ( fid = fopen( cfile[0], "r" ) ) == NULL ){
    fprintf( stderr, "%s%s\n", CMTRK0202, cfile[0] );
    return( -3 );
  }

  flag = 0;
  while( fgets( buff, 128, fid ) != NULL ){
    if( buff[0] != '#' ){
      if( sscanf( buff, "%s%s%s", temp[0], temp[1], dummy ) != 3 ){
        fprintf( stderr, "%s%s\n", CMTRK0203, cfile[0] );
        fclose( fid );
        return( -4 );
      }
      temp[1][ strlen( temp[1] ) -1 ] = '\0';
      dd = atof( temp[1] );

      /* 目的の時刻かどうかのチェック */
      if( dd > dmjd ){
        flag = 1;
        break;
      }
      strcpy( bedata, buff );
    }
  }
  /* ファイルクローズ */
  fclose( fid );

  /* 該当する時刻のデータなし */
  if( flag != 1 ){
    fprintf( stderr, "%s%s (flg:%d)\n", CMTRK0203, cfile[0], flag );
    return( -4 );
  }
  /* 該当する時刻のデータを読む */
  if( sscanf( bedata, "%s%s%s%s", temp[0], temp[1], temp[2], temp[3] ) != 4 ){
    fprintf( stderr, "%s%s\n", CMTRK0203, cfile[0] );
    return( -4 );
  }
  dd0 = atof( temp[1] );
  i0  = atoi( temp[2] );
  d0  = atof( temp[3] ) - (double)i0;

  /* データ出力領域へコピー */
  dtime[0] = dd0;			/* レコードの時刻[MJD] */
  dtime[1] = d0;			/* UT1R-TAI */
  dtime[2] = i0;			/* TAI-UTC */

#ifdef DEBUG
  fprintf( stderr, "TIME data = %lf %lf %lf\n",dtime[0],dtime[1],dtime[2]);
#endif
 
  /*------------------------------------------------------------------*/
  /* ＥＰＨデータの取得                                               */
  /*------------------------------------------------------------------*/
  /* ユリウス日 */
  djd = dmjd + (double)2400000.5e00;
/* 19880110 ohta */
  djd = djd - 1.0;
/* 19880110 ohta */

  if( ( fid = fopen( cfile[1], "r" ) ) == NULL ){
    fprintf( stderr, "%s%s\n", CMTRK0202, cfile[1] );
    return( -5 );
  }

  j = 0;
  bedata[0] = '\0';
  while( fgets( buff1, BUFF, fid ) != NULL ){
    if( buff1[0] != '#' ){
      if( sscanf( buff1, "%s", dummy ) != 1 ){
        fprintf( stderr, "%s%s\n", CMTRK0203, cfile[1] );
        fclose( fid );
        return( -5 );
      }
      dd = atof( dummy );

      /* 目的の時刻かどうかのチェック */
      if( ( dd > djd ) && ( strlen(bedata) != 0 ) ){
        p = (char *)strtok( bedata, "," );
        deph[j*31] = atof( p );
        /* 時刻をＭＪＤへ変換する */
        deph[j*31] = deph[j*31] - (double)2400000.5e00;
 
        k = 1;
        while( ( p = (char *)strtok( NULL, "," ) ) != NULL ){
          deph[j*31+k] = atof( p );
          k++;
        } 
        if( k != 31 ){
          fprintf( stderr, "%s%s\n", CMTRK0203, cfile[1] );
          fclose( fid );
          return( -5 );
        }
        j++;
        if( j == 8 ) break;
      }
      strcpy( bedata, buff1 );
    }
  }
  /* ファイルクローズ */
  fclose( fid );

  /* 該当する時刻のデータなし */
  if( j != 8 ){
    fprintf( stderr, "%s%s\n", CMTRK0203, cfile[1] );
    return( -5 );
  }

  return( 0 );
}
