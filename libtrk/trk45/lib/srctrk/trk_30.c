/*--------------------------------------------------------------------*/
/*                                                                    */
/*  �ؿ�̾   : trk_30                                                 */
/*                                                                    */
/*  ��̾     : �絤����������                                         */
/*                                                                    */
/*  �����   : =  0: ���ｪλ                                         */
/*             = -1: �۾ｪλ                                         */
/*                                                                    */
/*  ����     : (in ) double idazel[2]  ���ڣţ���                     */
/*                                     [0]: ���̳�   [rad]            */
/*                                     [1]: �ĳ�     [rad]            */
/*             (in ) double idweat[3]  ���ݥѥ�᥿                   */
/*                                     [0]: ����     [��]             */
/*                                     [1]: ����     [hp]             */
/*                                     [2]: ������� [hp]             */
/*             (in ) double idgdin[2]  ���١�����                     */
/*                                     [0]: ����                      */
/*                                     [1]: ����                      */
/*             (in ) double idlast     �ֻ��λ빱����                 */
/*                                                                    */
/*                                                                    */
/*             (out) double odazel[2]  �絤��������Σ��ڣţ���       */
/*                                     [0]: ���̳�   [rad]            */
/*                                     [1]: �ĳ�     [rad]            */
/*             (out) double odrade[2]  �絤���������¬�������       */
/*                                     [0]: �ַ�     [rad]            */
/*                                     [1]: �ְ�     [rad]            */
/*                                                                    */
/*  ��ջ��� :                                                        */
/*                                                                    */
/*  ������   : FUJITSU LIMITED    1997.09.30                          */
/*                                                                    */
/*  ���     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  NORMAL     0
#define  ABNORMAL  -1

int     trk_30( idazel, idweat, idgdin, idlast, odazel, odrade )
double  idazel[2];          /* (in ) ���ڣţ���                       */
double  idweat[3];          /* (in ) ���ݥѥ�᥿                     */
double  idgdin[2];          /* (in ) ���١�����                       */
double  idlast;             /* (in ) �ֻ��λ빱����                   */
double  odazel[2];          /* (out) �絤��������Σ��ڣţ���         */
double  odrade[2];          /* (out) �絤���������¬�������         */
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
  /* ������ڤ���Ū�ѿ��ؤγ�Ǽ                                       */
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
  /* �ƣϣңԣң��Τζ����ΰ���ͤ���������                           */
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
