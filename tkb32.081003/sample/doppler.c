#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "track.h"

MAIN__(void)
{
  /* trk_10���ѿ� */
  int  ipflg, ioflg, iaoflg, ibjflg;
  double drdlv, dsourc[2];

  /* trk_20���ѿ� */
  char cstart[24];
  double dtime;
  XTRKI xin;
  XTRKO xout;

  /* trk_velo���ѿ� */
  static double dvrad;

  /* ������ѿ� */
  char cerr[256];
  int  i, ival;

  static char cenvdt[3][128] = {
    "ANTFILE=../ant_err.dat",
    "EPHFILE=../ephfile/19241221-20500102.ORG",
    "TIMEFILE=../time.dat"
  };

  /*--------------------------------------------------------------------------------*/
  /* �������                                                                       */
  /*--------------------------------------------------------------------------------*/
  memset(&xin, 0, sizeof(xin));
  memset(&xout, 0, sizeof(xout));

  /* �Ķ��ѿ������ */
  for (i = 0; i < 3; i++) {
    if (putenv(cenvdt[i]) != 0) {
      fprintf(stderr, "Cannot add value to environment (%s)\n", cenvdt[i]);
      exit(-1);
    }
  }

  /**********************************************************************************/
  /* (1) �����׻��ѥѥ�᥿���� (��Ū���� trk_10)                                   */
  /**********************************************************************************/
  ipflg = 3;                    /* �����ե饰 3:DOPPLER */
  ioflg = 0;                    /* ŷ�Υե饰 0:���۷ϳ� 1-11:���� 12:COMET 99:AZEL */
  iaoflg = 1;                   /* ���ֺ�ɸ�� 0:���� 1:RADEC 2:LB 3:AZEL */
  ibjflg = 2;                   /* ʬ���ե饰 1:B1950 2:J2000 */
  drdlv = 3000.0e0;             /* ŷ�Τλ���®�� [m/s] */
  dsourc[0] = 0.52359878e0;     /* ŷ�ΰ��� [rad] */
  dsourc[1] = 0.52359878e0;     /* ŷ�ΰ��� [rad] */

  /*
   * trk_00, trk_10��1�٤����Ƥ٤��ɤ�������������٤���Ϻ��ٸƤӽФ�
   */
  /*--------------------------------------------------------------------------------*/
  /* trk_00 (��¬���ϻ�������� YYYYMMDDhhmmss.0 (JST))                             */
  /*--------------------------------------------------------------------------------*/
  if (trk_00("20061018000000.0", "", cerr) != 0) {
    fprintf(stderr, "%s\n", cerr);
    exit(-1);
  }
  /*--------------------------------------------------------------------------------*/
  /* trk_10                                                                         */
  /*--------------------------------------------------------------------------------*/
  ival = trk_10(ioflg, ipflg, iaoflg, ibjflg, dsourc, drdlv);
  if (ival == 1) {
    fprintf(stderr, "trk_10: Argument check error\n");
    exit(-1);
  } else if (ival == 2) {
    fprintf(stderr, "trk_10: Common area initial error\n");
    exit(-1);
  }

  /**********************************************************************************/
  /* (2) �����׻��ѥѥ�᥿���� (ưŪ���� trk_20)                                   */
  /**********************************************************************************/
  xin.irpflg = 1;               /* �ݥ���ƥ��� 0:���� 1:�³� */
  xin.iapflg = 1;               /* �ݥ���ƥ��� 1:RADEC 2:LB 3:AZEL */
  xin.dpofst[0] = 0.0e0;        /* �ݥ���ƥ��󥰥��ե��å� [rad] RADEC or LB */
  xin.dpofst[1] = 0.0e0;
  xin.dpofaz[0] = 0.0e0;        /* �ݥ���ƥ��󥰥��ե��å� [rad] AZEL */
  xin.dpofaz[1] = 0.0e0;

  xin.irsflg = 1;               /* �������˥� 0:���� 1:�³� */
  xin.iasflg = 1;               /* �������˥� 1:RADEC 2:LB 3:AZEL */
  xin.dscnst[0] = 0.1e0;        /* �������˥󥰳��ϥ��ե��å� [rad] */
  xin.dscnst[1] = 0.2e0;
  xin.dscned[0] = 0.1e0;        /* �������˥󥰽�λ���ե��å� [rad] */
  xin.dscned[1] = 0.2e0;

  xin.itnscn = 1;               /* �������˥����� */
  xin.inoscn = 1;               /* �������˥��� */

  xin.ivdef = 1;                /* ����®�٤������ 1:RAD 2:OPT */
  xin.ivref = 1;                /* ����®�٤δ��� 1:LSR 2:HEL 3:SOLAR or COMET
                                   ������ or ������¬�Ǥδ���¬�����֤Ȥʤ� */
  xin.dobsfq = 4.3e10;          /* �Żߥȥ�å��󥰼��ȿ� [Hz] */

  /*
   * trk_20, trk_30�Ϲ�Ȼ��郎�Ѳ����뤿�ᡢ����롼�׽����ˤʤ�
   */
  /*--------------------------------------------------------------------------------*/
  /* trk_20 (��¬������������� YYYYMMDDhhmmss.0 (JST))                             */
  /*--------------------------------------------------------------------------------*/
  strcpy(cstart, "20061018000010.0");
  if (tjcd2m(cstart, &dtime) == 1) {  /* JST���Ϥ������դ���MJD�ؤ��Ѵ� */
    fprintf(stderr, "tjcd2m: Error\n");
    exit(-1);
  }
  ival = trk_20(dtime, xin, &xout);
  if (ival == -1) {
    fprintf(stderr, "trk_20: Argument check error\n");
    exit(-1);
  } else if (ival == -2) {
    fprintf(stderr, "trk_20: Local error end\n");
    exit(-1);
  }
  /*--------------------------------------------------------------------------------*/
  /* VRAD (����ƥʤ��鸫������®��) �λ���                                         */
  /*--------------------------------------------------------------------------------*/
  trk_velo_(&dvrad);

  /**********************************************************************************/
  /* ��̽���                                                                       */
  /**********************************************************************************/
  printf("START => %s\n", cstart);
  printf("Observing Frequency        [Hz]: %+10.9lf\n", xin.dobsfq);
  printf("Doppler Tracking Frequency [Hz]: %+10.9lf\n", xout.dflkfq);
  printf("VRAD                      [m/s]: %+10.9lf\n", dvrad);
}
