/*--------------------------------------------------------------------*/
/*                                                                    */
/*  �ؿ�̾   : tjcd2m                                                 */
/*                                                                    */
/*  ��̾     : �ʣӣԻ��Ϥ�������դ���ͣʣĤؤ��Ѵ�                 */
/*                                                                    */
/*  ��ǽ���� : �ʣӣԻ��Ϥ��������(YYYYMMDDhhmmss.ss.... �ޤ���      */
/*             YYMMDDhhmmss.ss...)���齤����ꥦ����[sec]���Ѵ���Ԥ� */
/*                                                                    */
/*  �����   : =  0: ���ｪλ                                         */
/*             =  1: ����ʸ����Υե����ޥåȥ��顼�ǽ�λ             */
/*                                                                    */
/*  ��ջ��� : �ʤ�                                                   */
/*                                                                    */
/*  ������   : FUJITSU LIMITED    1996.05.29                          */
/*                                                                    */
/*  ���     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>
#include	"trkmod.h"	/* �����ǥե���ȥѥ�᥿ */
#include	"trkmsg.h"	/* ���顼��å����� */

tjcd2m( ctime, dtime )

char	ctime[];		/* ���գʣӣ� */
double	*dtime;			/* �ͣʣ� */

{
  int           i;
  int		ildate;		/* ���դ�ʸ����Ĺ */
  char		cwrk[150];
  double	ddate[6];	/* ���� */
  double	dwrk, dm = 0.0;

  *dtime = 0.0e0;

  /* ����ʸ����Υ����å� */
  ildate = strlen( ctime );
  if (ildate == 0) {
    fprintf( stderr, "%slength=0\n", CMTJCD2M00 );
    return( 1 );
  }
  /* �������ʲ����ͤ���� */
  for ( i = 0 ; i < ildate ; i++ ) if (ctime[i] == '.') break;
  if (i < ildate) {
    ildate = i;
    dm = atof( &ctime[ildate] );
  }

  /* ����ʸ�����ʬ�� */
  strncpy(cwrk, ctime, ildate);
  cwrk[ildate] = 0;
  dwrk = atof( cwrk );
  /* ǯ */
  ddate[0] = (double)( (int)( dwrk / 10000000000.0 ) );
  /* �� */
  dwrk = dwrk - ddate[0] * 10000000000.0;
  ddate[1] = (double)( (int)( dwrk / 100000000.0 ) );
  /* �� */
  dwrk = dwrk - ddate[1] * 100000000.0;
  ddate[2] = (double)( (int)( dwrk / 1000000.0 ) );
  /* �� */
  dwrk = dwrk - ddate[2] * 1000000.0;
  ddate[3] = (double)( (int)( dwrk / 10000.0 ) );
  /* ʬ */
  dwrk = dwrk - ddate[3] * 10000.0;
  ddate[4] = (double)( (int)( dwrk / 100.0 ) );
  /* �� */
  ddate[5] = (int)(dwrk - ddate[4] * 100.0) + dm;

  /* �������դΥ����å� */
  if( ildate == 14 ) ddate[0] = ddate[0] - 1900.0;
  /* ǯ */
  if( ddate[0] < 50.0 ) ddate[0] = ddate[0] + 100.0;
  if( ddate[0] < 1.0 || ddate[0] > 200.0 ){
    fprintf( stderr, "%syear=%f\n", CMTJCD2M00, ddate[0] );
    return( 1 );
  }
  /* �� */
  if( ddate[1] < 1.0 || ddate[1] > 12.0 ){
    fprintf( stderr, "%smonth=%f\n", CMTJCD2M00, ddate[1] );
    return( 1 );
  }
  /* �� */
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
  /* ��,ǯ */
  if( ddate[1] < 3.0 ){
    ddate[1] = ddate[1] + 12.0;
    ddate[0] = ddate[0] - 1.0;
  }
  /* �� */
  if( ddate[3] < 0.0 || ddate[3] > 23.0 ){
    fprintf( stderr, "%shour=%f\n", CMTJCD2M00, ddate[3] );
    return( 1 );
  }
  /* ʬ */
  if( ddate[4] < 0.0 || ddate[4] > 59.0 ){
    fprintf( stderr, "%sminute=%f\n", CMTJCD2M00, ddate[4] );
    return( 1 );
  }
  /* �� */
  if( ddate[5] < 0.0 || ddate[5] >= 60.0 ){
    fprintf( stderr, "%ssecnd=%f\n", CMTJCD2M00, ddate[5] );
    return( 1 );
  }

  /* �ʣ�����������ǯ��������ηв���� */
  dwrk = 365.0 * ddate[0] + 30.0 * ddate[1] + ddate[2] - 33.5
       + (int)( 3.0 * ( ddate[1] + 1.0 ) / 5.0 )
       + (int)( ddate[0] / 4.0 ) - 36525.0;

  /* ����ɸ����ϤǤΣͣʣĤ��Ѵ� */
  dwrk = D_J2000 + dwrk;

  /* �������äؤ�ñ���Ѵ� */
  dwrk = dwrk * 86400.0
       + ( ddate[3] * 60.0 + ddate[4] ) * 60.0 + ddate[5];

  /* �ͣʣ� */
  *dtime = dwrk - MJD_JST;

  return( 0 );

}
