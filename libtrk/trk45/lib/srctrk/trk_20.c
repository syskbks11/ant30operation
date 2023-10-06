/*--------------------------------------------------------------------*/
/*                                                                    */
/*  �ؿ�̾   : trk_20                                                 */
/*                                                                    */
/*  ��̾     : ư����������                                       */
/*                                                                    */
/*  ��ǽ���� : ���ꤵ�줿ư������������Ԥ���                     */
/*                                                                    */
/*  �����   : =  0: ���ｪλ                                         */
/*             = -1: ���������å��ǥ��顼��λ                         */
/*             = -2: �����������ǥ��顼��λ                       */
/*                                                                    */
/*  ��ջ��� :                                                        */
/*                                                                    */
/*  ������   : FUJITSU LIMITED    1996.06.18                          */
/*                                                                    */
/*  ���     :                                                        */
/*             MODIFY 1997.04.03 ( FLUKE_FREQ  FNS )                  */
/*             MODIFY 1997.09.30 ( WEATHER     FNS )                  */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>
#include	"track.h"	/* �����׻������ϥǡ�����¤�� */
#include	"trkmod.h"	/* �����ǥե���ȥѥ�᥿ */
#include	"trkmsg.h"	/* ���顼��å����� */

#define PI (double)3.1415926535897932e0

trk_20( dtime, xin, xout )

double	dtime;			/* �׻�����(MJD) */
XTRKI	xin;			/* ����ƥʤΣ֣̣£ɺ�ɸ���֥٥��ȥ� */
XTRKO	*xout;			/* ����ƥʤε��������� */

{

  double dantintvl = 0.2;       /* sec */
  int	iret;			/* �꥿���󥳡��� */
  static double	dpofst[2];	/* �ݥ���ƥ��󥰥��ե��å� [RADEC,LB] */
  static double	dpofaz[2];	/* �ݥ���ƥ��󥰥��ե��å� [AZEL]     */
  static double	dscnst[2];	/* ������󳫻ϰ��� */
  static double	dscned[2];	/* �������λ���� */
  static double	dintrl;		/* �ե�󥸥��ơ�������ֳ�[sec] */
  static double	ddofst;		/* �ٱ���֥��ե��å� */
  static double	dobsfq;		/* ��¬���ȿ�(=�Ż߼��ȿ�) */
  static double	dtrnfq[2];	/* ���ȿ��Ѵ����� */
  static double	dfofst;		/* ���ȿ����ե��å� */
  static double	dlclfq;		/* ��ּ��ȿ� */
  static int	itnscn;		/* ����������� */
  static int	inoscn;		/* ��������� */
  static int	irpflg;		/* �ݥ���ƥ��󥰼³ѥե饰 */
  static int	iapflg;		/* �ݥ���ƥ��󥰳ѥե饰 */
  static int	irsflg;		/* �������³ѥե饰 */
  static int	iasflg;		/* �������ѥե饰 */
  static int	idlflg;		/* �ٱ������ե饰 */
  static int	idsbfg;		/* �ģӣ¥ե饰 */
  static int	iulsel;		/* �ա��̥ե饰 */
  static int	irotdr;		/* �ե�󥸲�ž���� */
  static int	inlocl;		/* ��������ȯ�������ܼ��� */
  static int	inflck;		/* �ե롼�����󥻥��������ܼ��� */
  static int	itrkfq;		/* �ȥ�å��󥰼��ȿ����� */
  static int	ivref; 		/* ����®�٤δ��ϥե饰 */
  static int	ivdef; 		/* ����®�٤�����ϥե饰 */
  static double dcomet[6];      /* ���������� */
  static double dsrc[2];	/* ŷ�κ�ɸ (RADEC) [2] [rad] */
  static double dist;		/* �Ͽ������ŷ�Τε�Υ [m] */
  static double delt[3];	/* �ǥ������ʬ�Ǥ��Ѳ��� [m] */

  static double	dazel[2];	/* ����ƥʻ����� */
  static double	dazel1[2];	/* ����ƥʻ����� */
  static double	ddazel[2];	/* ����ƥ�®�� */
  static double	dtaug[3];	/* �ٱ���� */
  static double	dphas[2];	/* ������� */
  static double	ddphas[2];	/* �����Ѳ��� */
  static double	dflkfq;		/* �������ȿ� */
  static double	duvw[3];	/* �գ֣� */
  static double	dhoura;		/* ������󥰥� */
  static double	dtime1;
  static int    frstfg;
  char		cwrk[32];

  /* ������ */
  dpofst[0] = xin.dpofst[0];	/* �ݥ���ƥ��󥰥��ե��å� [RADEC,LB] */
  dpofst[1] = xin.dpofst[1];	/* �ݥ���ƥ��󥰥��ե��å� [RADEC,LB] */
  dpofaz[0] = xin.dpofaz[0];	/* �ݥ���ƥ��󥰥��ե��å� [AZEL] */
  dpofaz[1] = xin.dpofaz[1];	/* �ݥ���ƥ��󥰥��ե��å� [AZEL] */
  dscnst[0] = xin.dscnst[0];	/* ������󳫻ϰ��� */
  dscnst[1] = xin.dscnst[1];	/* ������󳫻ϰ��� */
  dscned[0] = xin.dscned[0];	/* �������λ���� */
  dscned[1] = xin.dscned[1];	/* �������λ���� */
  dintrl = xin.dintrl;		/* ����ֳ� */
  ddofst = xin.ddofst;		/* �ٱ���֥��ե��å� */
  dobsfq = xin.dobsfq;		/* ��¬���ȿ�(=�Ż߼��ȿ�) */
  dtrnfq[0] = xin.dtrnfq[0];	/* ���ȿ��Ѵ�����-------̤���� */
  dtrnfq[1] = xin.dtrnfq[1];    /* ���ȿ��Ѵ�����-------̤���� */
  dfofst = xin.dfofst;		/* ������ꥪ�ե��å� */
  dlclfq = xin.dlclfq;		/* ��ּ��ȿ�-----------̤���� */
  itnscn = xin.itnscn;		/* ������������� */
  inoscn = xin.inoscn;		/* ��������� */
  irpflg = xin.irpflg;		/* �ݥ���ƥ��󥰼³ѥե饰 */
  iapflg = xin.iapflg;		/* �ݥ���ƥ��󥰳ѥե饰 */
  irsflg = xin.irsflg;		/* �������³ѥե饰 */
  iasflg = xin.iasflg;		/* �������ѥե饰 */
  idsbfg = xin.idsbfg;		/* �ģӣ¥ե饰---------̤���� */
  idlflg = xin.idlflg;		/* �ٱ������ե饰 */
  iulsel = xin.iulsel;		/* �ա��̥ե饰 */
  irotdr = xin.irotdr;		/* �ե�󥸲�ž���� */
  inlocl = xin.inlocl;		/* ȯ�������ܼ���-------̤���� */
  inflck = xin.inflck;		/* �������ܼ���-------̤���� */
  itrkfq = xin.itrkfq;		/* �ȥ�å��󥰼��ȿ����� */
  ivref  = xin.ivref; 		/* ����®�٤δ��ϥե饰 */
  ivdef  = xin.ivdef; 		/* ����®�٤�����ϥե饰 */

  dcomet[0] = xin.dcomet[0] / 86400.0; /* �������̲���� �ͣʣ�[DAY]   */
  dcomet[1] = xin.dcomet[1];    /* ��������Υ     0.0 �� 99.999999[AU] */
  dcomet[2] = xin.dcomet[2];    /* Υ��Ψ         0.0 �� 99.999999     */
  dcomet[3] = xin.dcomet[3];    /* ����������     [rad] */
  dcomet[4] = xin.dcomet[4];    /* ����������     [rad] */
  dcomet[5] = xin.dcomet[5];    /* ��ƻ���г�     [rad] */
  
  /* ���������å� */
  /* ����������� */
  if( ( itnscn < 1 ) ){
    fprintf( stderr, "%s%d\n", CMTRK2000, itnscn );
    return( -1 );
  }
  /* ��������� */
  if( ( inoscn < 1 ) || ( inoscn > itnscn ) ){
    fprintf( stderr, "%s%d\n", CMTRK2001, inoscn );
    return( -1 );
  }
  /* �ݥ���ƥ��󥰼³ѥե饰 */
  if( irpflg != 1 ){
    fprintf( stderr, "%s%d\n", CMTRK2002, irpflg );
    return( -1 );
  }
  /* �ݥ���ƥ��󥰳ѥե饰 */
  if( ( iapflg < 1 ) || ( iapflg > 3 ) ){
    fprintf( stderr, "%s%d\n", CMTRK2003, iapflg );
    return( -1 );
  }
  /* �������³ѥե饰 */
  if( irsflg != 1 ){
    fprintf( stderr, "%s%d\n", CMTRK2004, irsflg );
    return( -1 );
  }
  /* �������ѥե饰 */
  if( ( iasflg < 1 ) || ( iasflg > 3 ) ){
    fprintf( stderr, "%s%d\n", CMTRK2005, iasflg );
    return( -1 );
  }

  /* ������¬�ξ�硢������֤ˤ��ǡ������� */
  if (t_cmt.imax != -1) {
    if (trk_20_cmt(dtime, &dsrc[0], &dist, &delt[0]) != 0) {
      return( -2 );
    }
  } else {
    dsrc[0] = 0.0e0;
    dsrc[1] = 0.0e0;
    dist = -1.0e0;
    delt[0] = 0.0e0;
    delt[1] = 0.0e0;
    delt[2] = 0.0e0;
  }

  /* ®����ʬ�ν���� */
  ddazel[0] = 0.0;
  ddazel[1] = 0.0;
  dtime1 = dtime;
  iret = 0;

  /* ������������ͤ��Ϥ� */
  frstfg = 1;
  trk_21_( &dtime1,  dpofst,  dscnst,  dscned, &dobsfq,
            dtrnfq, &dintrl, &ddofst, &dfofst, &dlclfq, &itnscn,
           &inoscn, &irpflg, &iapflg, &irsflg, &iasflg, &idsbfg,
           &idlflg, &iulsel, &irotdr, &inlocl, &inflck, &itrkfq,
             dazel,   dtaug,   dphas,  ddphas, &dflkfq,    duvw,
           &dhoura,   &iret,  &ivref,  &ivdef,  dpofaz, &frstfg,
            dcomet, dsrc, &dist, delt );
  if( iret >= 100 ){
    ;
  }
  else if( iret < 0 ){
    fprintf( stderr, "%s %d\n", CMTRK2012, iret );
    return( -2 );
  }

  xout->dazel[0] = dazel[0];		/* ����ƥʤ����� */
  xout->dazel[1] = dazel[1];
  xout->ddazel[0] = ddazel[0];		/* ����ƥʤ�®�� */
  xout->ddazel[1] = ddazel[1];
  xout->dtaug[0] = dtaug[0];		/* �ٱ���� */
  xout->dtaug[1] = dtaug[1];
  xout->dtaug[2] = dtaug[2];
  dphas[0] = dphas[0] - (int)dphas[0];	/* ������� */
  dphas[1] = dphas[1] - (int)dphas[1];
  xout->dphas[0] = dphas[0];
  xout->dphas[1] = dphas[1];
  xout->ddphas[0] = ddphas[0];		/* �ե�󥸼��ȿ� */
  xout->ddphas[1] = ddphas[1];
  xout->dflkfq = dflkfq;		/* �������ȿ� */
  xout->duvw[0] = duvw[0];		/* �գ֣� */
  xout->duvw[1] = duvw[1];
  xout->duvw[2] = duvw[2];
  xout->dhoura = dhoura;		/* ������󥰥� */

  return( iret );

}
