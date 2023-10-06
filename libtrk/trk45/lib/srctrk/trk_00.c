/*--------------------------------------------------------------------*/
/*                                                                    */
/*  �ؿ�̾   : trk_00                                                 */
/*                                                                    */
/*  ��̾     : �����׻��⥸�塼�붦���ΰ�ν����                     */
/*                                                                    */
/*  ��ǽ���� : �����׻��⥸�塼��ζ����ΰ�ؽ���ͤ����Ϥ��롣       */
/*                                                                    */
/*  �����   : =  0: ���ｪλ                                         */
/*             =  1: �������դν񼰤ǥ��顼��λ                       */
/*             =  2: �����ΰ�ν������ǥ��顼��λ                   */
/*             =  3: �ţУȡ��ԣɣͣťե�����Υ꡼�ɤǥ��顼��λ     */
/*                                                                    */
/*  ��ջ��� : �ܥ⥸�塼��ϣ���롲�����Υ⥸�塼�����Ѥ����硢 */
/*            ɬ���ץ�������Ƭ�ǰ��ٸƤФʤ���Фʤ�ʤ���        */
/*                                                                    */
/*  ������   : FUJITSU LIMITED    1996.06.10                          */
/*                                                                    */
/*  ���     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>
#include	"trkmod.h"	/* �����ǥե���ȥѥ�᥿ */
#include	"trkmsg.h"	/* ���顼��å����� */

trk_00( cjstdate, cjpl, cerr )

char	cjstdate[];		/* �׻����ϣʣӣ����� */
char	cjpl[];			/* �ʣУ������ե����� */
char	cerr[];			/* ���顼��å����� */

{

  static double	dmjdjst;	/* �׻��������ܣͣʣĻ���[day] */
  static double	dmjd;		/* �׻����ϣͣʣĻ���[day] */
  static double	deph[248];	/* �ţУȥǡ��� */
  static double	dtime[3];	/* �ԣɣͣťǡ��� */
  static int	iret;		/* �꥿���󥳡��� */

  /* ���դ�ͣʣĤ��Ѵ����� */
  if( tjcd2m( cjstdate, &dmjd ) ){
    sprintf( cerr, "%s%s", CMTRK0000, cjstdate );
    return( 1 );
  }
  dmjdjst = ( dmjd + MJD_JST ) / DAYSEC;	/* ���ܣͣʣ� */
  dmjd = dmjd / DAYSEC;				/* ����˥å��ͣʣ� */
#ifdef DEBUG
printf( "JMJD=%f\n", dmjdjst );
printf( "GMJD=%f\n", dmjd );
printf( "DMJD=%f\n", dmjdjst-dmjd );
#endif

  /* �ԣɣͣš��ţУȥե����뤫��ǡ������ɤ߹��� */
  if( trk_02( dmjd, deph, dtime ) < 0 ){
    sprintf( cerr, "%s", CMTRK0002 );
    return( 3 );
  }
  /* JPL �����ե����뤫��ǡ������ɤ߹��� */
  t_cmt.index = -1;
  t_cmt.imax = -1;
  if (strlen(cjpl) != 0) {
    if (trk_03_cmt(cjstdate, cjpl, &t_cmt.imax,
      &t_cmt.ldmjd[0], &t_cmt.ldrad[0][0], &t_cmt.lddkm[0], cerr) != 0) {
      return( 4 );
    }
  }

  /* �ƣϣңԣң��Τζ����ΰ���ͤ��������� */
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
