/****************************************************************************/
/*                                                                          */
/*  File          :    int trk_antvl                                        */
/*                                                                          */
/*                     アンテナから見た視線速度（ＶＲＡＤ）値の算出         */
/*                                                                          */
/*  Function      :    trk_00()                                             */
/*                     trk_10()                                             */
/*                     trk_20()                                             */
/*                                                                          */
/*  Author        :    Fujitsu Nagano Systems Engineering Co Ltd.           */
/*                                                                          */
/*  Modify        :    1999/06/15                                           */
/*                     2005/11/10 (New comet method)                        */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/* 【引き数】                                                               */
/*     XANTVL  xvin      : (in ) ＶＲＡＤ計算入力構造体                     */
/*     double  *dantvl   : (out) アンテナから見た視線速度 [m/s]             */
/*     char    *cerror   : (out) エラーステータス情報                       */
/*                                                                          */
/* 【戻り値】                                                               */
/*     正常終了  :  0                                                       */
/*     異常終了  : -1                                                       */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <time.h>

#include "track.h"
#include "antvl.h"

#define  NORMAL    0
#define  ABNORMAL -1

int trk_antvl( xvin, dantvl, cerror )
XANTVL  xvin;
double  *dantvl;
char    *cerror;
{
    static XTRKI  xin;              /* 追尾入力パラメタ構造体               */
    static XTRKO  xout;             /* 追尾出力パラメタ構造体               */
    static double dtime;            /* ＵＴＣ時系でのＭＪＤ                 */
    static double davelo;           /* アンテナから見た視線速度 [m/s]       */
    static char   cstart[128];      /* 日付格納変数                         */
 
    static int    ioflg;            /* 天体フラグ                           */
    static int    ipflg;            /* 処理フラグ                           */
    static int    iaoflg;           /* 天体位置角フラグ                     */
    static int    ibjflg;           /* 分点フラグ                           */
    static double drdlv;            /* 観測天体の視線速度                   */
    static double dsourc[2];        /* 観測天体位置                         */

    int    i;                       /* ループ制御変数                       */
    int    ivalue;                  /* ライブラリ戻り値                     */
    char   *p;                      /* 作業用変数                           */
    char   cvfile[3][64];           /* 環境変数                             */
    char   cjpl[256];

                        /* 天体名テーブル                                   */
    static char cplname[12][12] = {
        "OUT",    "MERCURY", "VENUS",   "EARTH", "MARS", "JUPITER",
        "SATURN", "URANUS",  "NEPTUNE", "PLUTO", "MOON", "SUN"
    };

    /*----------------------------------------------------------------------*/
    /* 初期化                                                               */
    /*----------------------------------------------------------------------*/
    memset( &xin.dpofst[0], 0, sizeof(xin) );
    memset( &xout.dazel[0], 0, sizeof(xout) );
    cerror[0] = '\0';
    davelo  = 0.0e0;
    *dantvl = 0.0e0;

    /*----------------------------------------------------------------------*/
    /* 環境変数の登録                                                       */
    /*----------------------------------------------------------------------*/
    strcpy( cvfile[0], "EPHFILE=/cosmos3/45m/etc/eph.dat" );
    strcpy( cvfile[1], "ANTFILE=/cosmos3/45m/etc/ant_err.dat" );
    strcpy( cvfile[2], "TIMEFILE=/cosmos3/45m/etc/time.dat" );
    for( i=0; i<3; i++ ){
        if( putenv( cvfile[i] ) != 0 ){
            sprintf( cerror, "Not entry in environ.(%s)", cvfile[i] );
            return ABNORMAL;
        }
    }

    /*----------------------------------------------------------------------*/
    /* 追尾計算値パラメタ設定１                                             */
    /*----------------------------------------------------------------------*/
    /* 処理フラグ（固定） */
    ipflg = 3;

    /* 観測天体の視線速度 */
    drdlv = 0.0e0;

    /* 観測天体位置 */
    dsourc[0] = xvin.dsrc_pos[0] / 180.0e0 * M_PI;
    dsourc[1] = xvin.dsrc_pos[1] / 180.0e0 * M_PI;

    /* 分点  1:B1950, 2:J2000 */
    if( strcmp( xvin.cepoch, "B1950" ) == 0 )
        ibjflg = 1;
    else if( strcmp( xvin.cepoch, "J2000" ) == 0 )
        ibjflg = 2;
    else{
        sprintf( cerror, "Illegal <EPOCH> param error.(%s)", xvin.cepoch );
        return ABNORMAL;
    }

    /* 天体位置角フラグ  PLANET, RADEC, LB, AZEL, COMET */
    cjpl[0] = '\0';
    if( strcmp( xvin.ctrk_type, "RADEC" ) == 0 ){
        ioflg  = 0;
        iaoflg = 1;
    }else if( strcmp( xvin.ctrk_type, "LB" ) == 0 ){
        ioflg  = 0;
        iaoflg = 2;
    }else if( strcmp( xvin.ctrk_type, "AZEL" ) == 0 ||
              strcmp( xvin.ctrk_type, "HOLO" ) == 0 ){
        ioflg  = 99;
        iaoflg = 3;
    }else if( strcmp( xvin.ctrk_type, "SOLAR" ) == 0 ){
        for( i=1; i<12; i++ )
            if( strcmp( xvin.csrc_name, cplname[i] ) == 0 )
                break;
        if( i == 12 ){
            sprintf( cerror, "Illegal <SRC_NAME> param error.(%s)", xvin.csrc_name );
            return ABNORMAL;
        }
        ioflg  = i;
        iaoflg = 0;
    }else if( strcmp( xvin.ctrk_type, "COMET" ) == 0 ){
        strcpy( cjpl, xvin.csrc_name );
        ioflg  = 12;
        iaoflg = 0;
    }else{
        sprintf( cerror, "Illegal <TRK_TYPE> param error.(%s)", xvin.ctrk_type );
        return ABNORMAL;
    }

    /*----------------------------------------------------------------------*/
    /* 追尾計算値パラメタ設定２                                             */
    /*----------------------------------------------------------------------*/
    /* スキャン点数 */
    xin.inoscn = 1;
    xin.itnscn = 1;

    /* 実角フラグ */
    xin.irpflg = 1;
    xin.irsflg = 1;

    /* ポインティングオフセット（座標系） 1:RADEC, 2:LB, 3:AZEL */
    xin.iapflg = 1;
    xin.dpofst[0] = 0.0e0;
    xin.dpofst[1] = 0.0e0;
    xin.dpofaz[0] = 0.0e0;
    xin.dpofaz[1] = 0.0e0;

    /* スキャンニングオフセット（座標系） 1:RADEC, 2:LB, 3:AZEL */
    xin.iasflg = 1;
    xin.dscnst[0] = 0.0e0;
    xin.dscnst[1] = 0.0e0;
    xin.dscned[0] = 0.0e0;
    xin.dscned[1] = 0.0e0;
/*
    if( strcmp( xvin.cscan_cood, "RADEC" ) == 0 )
        xin.iasflg = 1;
    else if( strcmp( xvin.cscan_cood, "LB" ) == 0 )
        xin.iasflg = 2;
    else if( strcmp( xvin.cscan_cood, "AZEL" ) == 0 )
        xin.iasflg = 3;
    else{
        sprintf( cerror, "Illegal <SCAN_COOD> param error.(%s)", xvin.cscan_cood );
        return ABNORMAL;
    }
    xin.dscnst[0] = xvin.dscan_st[0] / 180.0e0 * M_PI;
    xin.dscnst[1] = xvin.dscan_st[1] / 180.0e0 * M_PI;
    xin.dscned[0] = xvin.dscan_ed[0] / 180.0e0 * M_PI;
    xin.dscned[1] = xvin.dscan_ed[1] / 180.0e0 * M_PI;
*/

    /* 静止トラッキング周波数 */
    xin.dobsfq = 0.0e0;

    /* 視線速度の基準系  1:RAD, 2:OPT */
    xin.ivdef  = 1;

    /* 視線速度の定義系  1:LSR, 2:HEL, 3:PLANET or COMET */
    if( strcmp( xvin.ctrk_type, "SOLAR" ) == 0 ||
        strcmp( xvin.ctrk_type, "COMET" ) == 0 )
        xin.ivref = 3;
    else if( strcmp( xvin.cvref, "LSR" ) == 0 ||
             strcmp( xvin.cvref, "GAL" ) == 0 )
        xin.ivref = 1;
    else if( strcmp( xvin.cvref, "HEL" ) == 0 )
        xin.ivref = 2;
    else{
        sprintf( cerror, "Illegal <VREF> param error.(%s)", xvin.cvref );
        return ABNORMAL;
    }

    /*----------------------------------------------------------------------*/
    /* 追尾計算ライブラリ  trk_00                                           */
    /*----------------------------------------------------------------------*/
    strcpy( cstart, xvin.cstime );
    if (trk_00( cstart, cjpl, cerror ) != 0) {
        return ABNORMAL;
    }

    /*----------------------------------------------------------------------*/
    /* 追尾計算ライブラリ  trk_10                                           */
    /*----------------------------------------------------------------------*/
    ivalue = trk_10( ioflg, ipflg, iaoflg, ibjflg, dsourc, drdlv );
    if( ivalue == 1 ){
        sprintf( cerror, "Argument check error.(trk_10)" );
        return ABNORMAL;
    }else if( ivalue == 2 ){
        sprintf( cerror, "Common area initial error.(trk_10)" );
        return ABNORMAL;
    }

    /*----------------------------------------------------------------------*/
    /* 追尾計算ライブラリ  trk_20                                           */
    /*----------------------------------------------------------------------*/
    /* MJD への変換 */
    if( tjcd2m( cstart, &dtime ) == 1 ){
        sprintf( cerror, "Input date-format error.(tjcd2m)" );
        return ABNORMAL;
    }
    ivalue = trk_20( dtime, xin, &xout );
    if( ivalue == -1 ){
        sprintf( cerror, "Argument check error.(trk_20)" );
        return ABNORMAL;
    }else if( ivalue == -2 ){
        sprintf( cerror, "Local error end.(trk_20)" );
        return ABNORMAL;
    }

    trk_velo_( &davelo );
    if( iaoflg == 3 )
        *dantvl = 0.0e0;
    else
        *dantvl = davelo;

#ifdef DEBUG
    fprintf( stderr, "EPHFILE    = %s\n", getenv( "EPHFILE" ) );
    fprintf( stderr, "ANTFILE    = %s\n", getenv( "ANTFILE" ) );
    fprintf( stderr, "TIMEFILE   = %s\n", getenv( "TIMEFILE" ) );
    fprintf( stderr,"\n" );
    fprintf( stderr, "ibjflg     = %d\n",  ibjflg );
    fprintf( stderr, "ioflg      = %d\n",  ioflg );
    fprintf( stderr, "iaoflg     = %d\n",  iaoflg );
    fprintf( stderr, "dsource1   = %lf\n", dsourc[0] );
    fprintf( stderr, "dsource2   = %lf\n", dsourc[1] );
    fprintf( stderr, "ipflg  (3) = %d\n",  ipflg );
    fprintf( stderr, "drdlv  (0) = %lf\n", drdlv );
    fprintf( stderr,"\n" );
    fprintf( stderr, "ivref      = %d\n",  xin.ivref  );
    fprintf( stderr, "ivdef  (1) = %d\n",  xin.ivdef  );
    fprintf( stderr, "inoscn (1) = %d\n",  xin.inoscn );
    fprintf( stderr, "itnscn (1) = %d\n",  xin.itnscn );
    fprintf( stderr, "irpflg (1) = %d\n",  xin.irpflg );
    fprintf( stderr, "irsflg (1) = %d\n",  xin.irsflg );
    fprintf( stderr, "iapflg (1) = %d\n",  xin.iapflg );
    fprintf( stderr, "iasflg (1) = %d\n",  xin.iasflg );
    fprintf( stderr, "dobsfq (0) = %lf\n", xin.dobsfq );
    fprintf( stderr, "dcomet[0]  = %lf\n", xin.dcomet[0] );
    fprintf( stderr, "dcomet[1]  = %lf\n", xin.dcomet[1] );
    fprintf( stderr, "dcomet[2]  = %lf\n", xin.dcomet[2] );
    fprintf( stderr, "dcomet[3]  = %lf\n", xin.dcomet[3] );
    fprintf( stderr, "dcomet[4]  = %lf\n", xin.dcomet[4] );
    fprintf( stderr, "dcomet[5]  = %lf\n", xin.dcomet[5] );
    fprintf( stderr, "cstart     = %s\n",  cstart );
    fprintf( stderr, "davelo     = %10.9lf\n", davelo );
#endif

    return NORMAL;
}
