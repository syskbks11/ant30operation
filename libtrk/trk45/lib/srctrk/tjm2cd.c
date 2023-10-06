/*--------------------------------------------------------------------*/
/*                                                                    */
/*  �ؿ�̾   : tjm2cd                                                 */
/*                                                                    */
/*  ��̾     : �ͣʣĤ���ʣӣԻ��Ϥ�������դؤ��Ѵ�                 */
/*                                                                    */
/*  ��ǽ���� : ������ꥦ�����ʣͣʣġ�[sec]����ʣӣԻ��Ϥˤ�������  */
/*             �����ա�YYYYMMDDhhmmss.ss...�ˤ��Ѵ���Ԥ�             */
/*                                                                    */
/*  �����   : =  0: ���ｪλ                                         */
/*             =  1: �����ͤ��ϰϤ����ݡ��Ȥ�������ϰϤ�ۤ��Ƥ���   */
/*                                                                    */
/*  ��ջ��� : �ʤ�                                                   */
/*                                                                    */
/*  ������   : FUJITSU LIMITED    1996.05.29                          */
/*                                                                    */
/*  ���     : 1996.11.28 ����                                        */
/*                                                                    */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>
#include	"trkmod.h"	/* �����ǥե���ȥѥ�᥿ */
#include	"trkmsg.h"	/* ���顼��å����� */

tjm2cd( dtime, ctime )

double	dtime;			/* �ͣʣ� */
char	ctime[];		/* ���� */

{

  double	djd;		/* �ʣ� */
  double	ddtime;		/* �ʣͣʣ� */
  double	ditime;		/* �ͣʣĤ�ü�� */
  double	dsec;
  int		idate[6];	/* ���� */
  int		iwrk[7];
  double	dwrk1;
  double	dwrk;

  /* �ͣʣĤ���ʣĤ��Ѵ� */
  ddtime = dtime + 9.0 * 3600.0;
  djd = 2400000.5 + ddtime / 86400.0;
  dwrk1 = ( djd - (int)djd ) + 0.5;
  /* �ͣʣĤ����ʾ��̤����ʬΥ */
  ditime = ddtime -( (int)( ddtime / 86400.0 ) ) * 86400.0;

  /* �׻� */
  iwrk[0] = (int)( djd + 68569.5 );
  iwrk[1] = (int)( (double)iwrk[0] / 36524.25 );
  dwrk = (double)36524.25 * (double)iwrk[1] + 0.75;
  iwrk[2] = iwrk[0] - (int)( dwrk );
  iwrk[3] = (double)( iwrk[2] + 1 ) / 365.25025 ;
  dwrk = (double)365.25 * (double)iwrk[3];
  iwrk[4] = iwrk[2] - (int)dwrk + 31;
  iwrk[5] = iwrk[4] / 30.59;
  iwrk[6] = iwrk[5] / 11;

  /* ǯ */
  idate[0] = 100 * ( iwrk[1] - 49 ) + iwrk[3] + iwrk[6];
  /* �� */
  idate[1] = iwrk[5] - 12 * iwrk[6] + 2;
  /* �� */
  idate[2] = iwrk[4] - (int)( 30.59 * iwrk[5] );
  if( idate[2] == 32 ){
    idate[2] = 1;
    idate[1] = 1;
    idate[0] = idate[0] + 1;
  }
  /* �� */
  idate[3] = (int)( ditime / 3600 );
  /* ʬ */
  idate[4] = (int)( ( ditime - idate[3] * 3600 ) / 60 );
  /* �� */
  dsec = ditime - idate[3] * 3600 - idate[4] * 60;

  /* ������ */
  sprintf( ctime, "%04d%02d%02d%02d%02d%016.13f\0",
           idate[0], idate[1], idate[2], idate[3], idate[4], dsec );

  return( 0 );

}
