/*--------------------------------------------------------------------*/
/*                                                                    */
/*  �ؿ�̾   : trk_10                                                 */
/*                                                                    */
/*  ��̾     : ư���������̻���                                   */
/*                                                                    */
/*  ��ǽ���� : ư���������̤���ꤷ�����ꤷ��ư��˥��������Ū */
/*             �ʥѥ�᡼���ν���ͤ�ζ����ΰ�����ꤹ�롣           */
/*                                                                    */
/*  �����   : =  0: ���ｪλ                                         */
/*             =  1: ���ϥե饰������ǥ��顼��λ                     */
/*             =  2: �����ΰ�ν������ǥ��顼��λ                   */
/*                                                                    */
/*  ��ջ��� :                                                        */
/*                                                                    */
/*  ������   : FUJITSU LIMITED    1996.06.18                          */
/*                                                                    */
/*  ���     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>
#include	"trkmod.h"	/* �����ǥե���ȥѥ�᥿ */
#include	"trkmsg.h"	/* ���顼��å����� */

trk_10( ioflg0, ipflg0, iaflg0, ibjflg0, dsourc0, drdlv0 )

int	ioflg0;		/* ŷ�Υե饰 */
int	ipflg0;		/* ư���������̥ե饰 */
int	iaflg0;		/* ŷ�ΰ��ֳѥե饰 */
int	ibjflg0;	/* ʬ���ե饰 */
double	dsourc0[2];	/* ��¬ŷ�ΰ��� */
double	drdlv0;		/* ��¬ŷ�Τλ���������¬�� */
{

  FILE          *fid;           /* �ե�����ݥ��� */
  char          *cenv;          /* �Ķ��ѿ������ΰ� */
  char          cfile[128];     /* �ե�����̾ */
  char          buff[256];      /* �Хåե��ѿ� */

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

  /* ���������å� */
  /* ŷ�Υե饰�����å� */
  if( ioflg != 99 ) { /* not comet */
    if( ( ioflg < 0 ) || ( ioflg > 12 ) ){
      fprintf( stderr, "%s%d\n", CMTRK1000, ioflg );
      return( 1 );
    }
  }
  /* ư���������̥ե饰 */
  if( ( ipflg < 0 ) || ( ipflg > 4 ) ){
    fprintf( stderr, "%s%d\n", CMTRK1001, ipflg );
    return( 1 );
  }
  /* ŷ�ΰ��ֳѥե饰 */
  if( ( iaflg < 0 ) || ( iaflg > 3 ) ){
    fprintf( stderr, "%s%d\n", CMTRK1002, iaflg );
    return( 1 );
  }
  /* ʬ���ե饰 */
  if( ( ibjflg < 1 ) || ( ibjflg > 2 ) ){
    fprintf( stderr, "%s%d\n", CMTRK1003, ibjflg );
    return( 1 );
  }
  /* ������¬����ʬ���ե饰  �ʣ����������� */
  if( ( ioflg < 12 ) && ( ioflg > 0 ) ){
    if( ibjflg == 1 ){
      fprintf( stderr, "trk: Illegal EPOCH data. (B1950)\n" );
      ibjflg = 2;
    }
  }

  /* �ե�����̾�μ��� */
  if( ( cenv = getenv( "ANTFILE" ) ) == NULL ){
    fprintf( stderr, "Not set <ANTFILE> in environ.\n" );
    return( 2 );
  }
  sprintf( cfile, "%s\0", cenv );

  /* ���Σԥե����륪���ץ� */
  if( ( fid = fopen( cfile, "r" ) ) == NULL ){
    fprintf( stderr, "Not open <ANTFILE> error.\n" );
    return( 2 );
  }

  /* ���Σԥե�����꡼�� */
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

  /* �ƣϣңԣң��Τζ����ΰ���ͤ��������� */
  trk_11_( antcd,  &ioflg, &ipflg, &iaflg, &ibjflg, dsourc, 
           &drdlv, &iret );
  if( iret != 0 ){
    fprintf( stderr, "%s%d\n", CMTRK1004,iret );
    return( 2 );
  }

  return( 0 );

}
