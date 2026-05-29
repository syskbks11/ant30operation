/*--------------------------------------------------------------------*/
/*                                                                    */
/*  関数名   : trk_30                                                 */
/*                                                                    */
/*  和名     : 大気差補正処理                                         */
/*                                                                    */
/*  戻り値   : =  0: 正常終了                                         */
/*             = -1: 異常終了                                         */
/*                                                                    */
/*  引数     : (in ) double idazel[2]  ＡＺＥＬ値                     */
/*                                     [0]: 方位角   [rad]            */
/*                                     [1]: 仰角     [rad]            */
/*             (in ) double idweat[3]  気象パラメタ                   */
/*                                     [0]: 気温     [度]             */
/*                                     [1]: 気圧     [hp]             */
/*                                     [2]: 水蒸気圧 [hp]             */
/*             (in ) double idgdin[2]  経度、緯度                     */
/*                                     [0]: 経度                      */
/*                                     [1]: 緯度                      */
/*             (in ) double idlast     瞬時の視恒星時                 */
/*                                                                    */
/*                                                                    */
/*             (out) double odazel[2]  大気差補正後のＡＺＥＬ値       */
/*                                     [0]: 方位角   [rad]            */
/*                                     [1]: 仰角     [rad]            */
/*             (out) double odrade[2]  大気差補正後の測心視位置       */
/*                                     [0]: 赤経     [rad]            */
/*                                     [1]: 赤緯     [rad]            */
/*                                                                    */
/*  注意事項 :                                                        */
/*                                                                    */
/*  作成者   : FUJITSU LIMITED    1997.09.30                          */
/*                                                                    */
/*  メモ     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  NORMAL     0
#define  ABNORMAL  -1

extern void trk_31_ (double *, double *, double *, double *, double *, double *, int *);

int     trk_30( double *idazel, double *idweat, double *idgdin, double idlast, double *odazel, double *odrade )
#if 0
double  idazel[2];          /* (in ) ＡＺＥＬ値                       */
double  idweat[3];          /* (in ) 気象パラメタ                     */
double  idgdin[2];          /* (in ) 経度、緯度                       */
double  idlast;             /* (in ) 瞬時の視恒星時                   */
double  odazel[2];          /* (out) 大気差補正後のＡＺＥＬ値         */
double  odrade[2];          /* (out) 大気差補正後の測心視位置         */
#endif
{
  int           i;

  static double ciazel[2];
  static double ciweat[3];
  static double cigdin[2];
  static double cilast;

  static double coazel[2];
  static double corade[2];
  static int	iret;

  /*------------------------------------------------------------------*/
  /* 初期化及び静的変数への格納                                       */
  /*------------------------------------------------------------------*/
  for( i=0; i<2; i++ ){
    ciazel[i] = idazel[i];
    ciweat[i] = idweat[i];
    cigdin[i] = idgdin[i];

    coazel[i] = 0.0;
    corade[i] = 0.0;
  }
  cilast    = idlast;    
  ciweat[2] = idweat[2];

  /*------------------------------------------------------------------*/
  /* ＦＯＲＴＲＡＮの共通領域へ値を代入する                           */
  /*------------------------------------------------------------------*/
  trk_31_( ciazel, ciweat, cigdin, &cilast, coazel, corade, &iret );
  if( iret != 0 ){
    fprintf( stderr, "trk_30: Failed to local calculate!! %d\n", iret );
    return ABNORMAL;
  }
  for( i=0; i<2; i++ ){
    odazel[i] = coazel[i];
    odrade[i] = corade[i];
  }

  return NORMAL;
}
