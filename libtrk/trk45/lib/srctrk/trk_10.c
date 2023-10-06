/*--------------------------------------------------------------------*/
/*                                                                    */
/*  関数名   : trk_10                                                 */
/*                                                                    */
/*  和名     : 動作ローカル種別指定                                   */
/*                                                                    */
/*  機能概要 : 動作ローカル種別を指定し、指定した動作にローカルな静的 */
/*             なパラメータの初期値をの共通領域へ設定する。           */
/*                                                                    */
/*  戻り値   : =  0: 正常終了                                         */
/*             =  1: 入力フラグの設定でエラー終了                     */
/*             =  2: 共通領域の初期設定でエラー終了                   */
/*                                                                    */
/*  注意事項 :                                                        */
/*                                                                    */
/*  作成者   : FUJITSU LIMITED    1996.06.18                          */
/*                                                                    */
/*  メモ     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>
#include	"trkmod.h"	/* 追尾デフォルトパラメタ */
#include	"trkmsg.h"	/* エラーメッセージ */

trk_10( ioflg0, ipflg0, iaflg0, ibjflg0, dsourc0, drdlv0 )

int	ioflg0;		/* 天体フラグ */
int	ipflg0;		/* 動作ローカル種別フラグ */
int	iaflg0;		/* 天体位置角フラグ */
int	ibjflg0;	/* 分点フラグ */
double	dsourc0[2];	/* 観測天体位置 */
double	drdlv0;		/* 観測天体の視線方向の測度 */
{

  FILE          *fid;           /* ファイルポインタ */
  char          *cenv;          /* 環境変数取得領域 */
  char          cfile[128];     /* ファイル名 */
  char          buff[256];      /* バッファ変数 */

  static int    ioflg, ipflg, iaflg, ibjflg, iret;
  static double dsourc[2], drdlv, antcd[3];

  ipflg = ipflg0;
  if (t_cmt.imax == -1) {
    ioflg = ioflg0;
    iaflg = iaflg0;
    ibjflg = ibjflg0;
    dsourc[0] = dsourc0[0];
    dsourc[1] = dsourc0[1];
    drdlv = drdlv0;
  } else {
    ioflg = 1;
    iaflg = 0;
    ibjflg = 2;
    dsourc[0] = 0.0e0;
    dsourc[1] = 0.0e0;
    drdlv = 0.0e0;
  }

  /* 引数チェック */
  /* 天体フラグチェック */
  if( ioflg != 99 ) { /* not comet */
    if( ( ioflg < 0 ) || ( ioflg > 12 ) ){
      fprintf( stderr, "%s%d\n", CMTRK1000, ioflg );
      return( 1 );
    }
  }
  /* 動作ローカル種別フラグ */
  if( ( ipflg < 0 ) || ( ipflg > 4 ) ){
    fprintf( stderr, "%s%d\n", CMTRK1001, ipflg );
    return( 1 );
  }
  /* 天体位置角フラグ */
  if( ( iaflg < 0 ) || ( iaflg > 3 ) ){
    fprintf( stderr, "%s%d\n", CMTRK1002, iaflg );
    return( 1 );
  }
  /* 分点フラグ */
  if( ( ibjflg < 1 ) || ( ibjflg > 2 ) ){
    fprintf( stderr, "%s%d\n", CMTRK1003, ibjflg );
    return( 1 );
  }
  /* 惑星観測時の分点フラグ  Ｊ２０００固定 */
  if( ( ioflg < 12 ) && ( ioflg > 0 ) ){
    if( ibjflg == 1 ){
      fprintf( stderr, "trk: Illegal EPOCH data. (B1950)\n" );
      ibjflg = 2;
    }
  }

  /* ファイル名の取得 */
  if( ( cenv = getenv( "ANTFILE" ) ) == NULL ){
    fprintf( stderr, "Not set <ANTFILE> in environ.\n" );
    return( 2 );
  }
  sprintf( cfile, "%s\0", cenv );

  /* ＡＮＴファイルオープン */
  if( ( fid = fopen( cfile, "r" ) ) == NULL ){
    fprintf( stderr, "Not open <ANTFILE> error.\n" );
    return( 2 );
  }

  /* ＡＮＴファイルリード */
  while( fgets( buff, 256, fid ) != NULL ){
    if( buff[0] != '*' ){
      if( sscanf( buff, "%lf%lf%lf", &antcd[0], &antcd[1], &antcd[2] ) != 3 ){
        fprintf( stderr, "Not get ant-data error.\n" );
        return( 2 );
      }  
      break;
    }
  }
  fclose( fid );

  /* ＦＯＲＴＲＡＮの共通領域へ値を代入する */
  trk_11_( antcd,  &ioflg, &ipflg, &iaflg, &ibjflg, dsourc, 
           &drdlv, &iret );
  if( iret != 0 ){
    fprintf( stderr, "%s%d\n", CMTRK1004,iret );
    return( 2 );
  }

  return( 0 );

}
