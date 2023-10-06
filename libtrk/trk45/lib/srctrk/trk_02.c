/*--------------------------------------------------------------------*/
/*                                                                    */
/*  �ؿ�̾   : trk_02                                                 */
/*                                                                    */
/*  ��̾     : �ţУȡ��ԣɣͣťե�����Υ꡼��                       */
/*                                                                    */
/*  ��ǽ���� : �ţУȡ��ԣɣͣťե����뤫���¬�����ӤΥǡ������ɤ߹� */
/*             �ࡣ                                                   */
/*                                                                    */
/*  �����   : >  0: ���ｪλ                                         */
/*             = -1: �ţУȥե�����̾�μ������顼                     */
/*             = -2: �ԣɣͣťե�����̾�μ������顼                   */
/*             = -3: �ţУȥե�����Υ����ץ󥨥顼                   */
/*             = -4: �ԣɣͣťե�����Υ����ץ󥨥顼                 */
/*                                                                    */
/*  ��ջ��� : �ʤ�                                                   */
/*                                                                    */
/*  ������   : FUJITSU LIMITED  1996.06.24                            */
/*                                                                    */
/*  ���     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trkmsg.h"
#include "trkmod.h"

#define  BUFF 1024

trk_02( dmjd, deph, dtime )
double	dmjd;                   /* ��¬���ϣͣʣ�[day]                */
double	deph[248];              /* �ţУȥǡ���                       */
double	dtime[3];               /* �ԣɣͣťե�����                   */
{
  double        djd;            /* ��ꥦ����                         */
  double        dd;             /* �ե�����ǡ����λ���               */
  double        dd0, d0;
  int           i0;

  FILE          *fid;           /* �ե�����ݥ���                   */
  char          *cenv;          /* �Ķ��ѿ������ΰ�                   */
  char          *p;
  char          buff[128];
  char          cfile[2][80];   /* �ե�����̾                         */
  char          temp[4][40];    /* ����Хåե��ѿ�                   */
  char          dummy[BUFF];    /* �ɤߤ����ѥХåե�                 */
  char          buff1[BUFF];
  char          bedata[BUFF];   /* �����ǡ���                         */
  int           i, j, k;        /* �롼�������ѿ�                     */
  int           flag;           /* �����ǡ���̵ͭ�ե饰               */

  /*------------------------------------------------------------------*/
  /* �����                                                           */
  /*------------------------------------------------------------------*/
  bedata[0] = '\0';

  /*------------------------------------------------------------------*/
  /* �ե�����̾�μ���                                                 */
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
  /* �ԣɣͣťǡ����μ���                                             */
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

      /* ��Ū�λ��狼�ɤ����Υ����å� */
      if( dd > dmjd ){
        flag = 1;
        break;
      }
      strcpy( bedata, buff );
    }
  }
  /* �ե����륯���� */
  fclose( fid );

  /* �����������Υǡ����ʤ� */
  if( flag != 1 ){
    fprintf( stderr, "%s%s (flg:%d)\n", CMTRK0203, cfile[0], flag );
    return( -4 );
  }
  /* �����������Υǡ������ɤ� */
  if( sscanf( bedata, "%s%s%s%s", temp[0], temp[1], temp[2], temp[3] ) != 4 ){
    fprintf( stderr, "%s%s\n", CMTRK0203, cfile[0] );
    return( -4 );
  }
  dd0 = atof( temp[1] );
  i0  = atoi( temp[2] );
  d0  = atof( temp[3] ) - (double)i0;

  /* �ǡ��������ΰ�إ��ԡ� */
  dtime[0] = dd0;			/* �쥳���ɤλ���[MJD] */
  dtime[1] = d0;			/* UT1R-TAI */
  dtime[2] = i0;			/* TAI-UTC */

#ifdef DEBUG
  fprintf( stderr, "TIME data = %lf %lf %lf\n",dtime[0],dtime[1],dtime[2]);
#endif
 
  /*------------------------------------------------------------------*/
  /* �ţУȥǡ����μ���                                               */
  /*------------------------------------------------------------------*/
  /* ��ꥦ���� */
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

      /* ��Ū�λ��狼�ɤ����Υ����å� */
      if( ( dd > djd ) && ( strlen(bedata) != 0 ) ){
        p = (char *)strtok( bedata, "," );
        deph[j*31] = atof( p );
        /* �����ͣʣĤ��Ѵ����� */
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
  /* �ե����륯���� */
  fclose( fid );

  /* �����������Υǡ����ʤ� */
  if( j != 8 ){
    fprintf( stderr, "%s%s\n", CMTRK0203, cfile[1] );
    return( -5 );
  }

  return( 0 );
}
