/*--------------------------------------------------------------------*/
/*                                                                    */
/*  ´Ø¿ôÌ¾   : trk_30                                                 */
/*                                                                    */
/*  ÏÂÌ¾     : Âçµ¤º¹ÊäÀµ½èÍý                                         */
/*                                                                    */
/*  Ìá¤êÃÍ   : =  0: Àµ¾ï½ªÎ»                                         */
/*             = -1: °Û¾ï½ªÎ»                                         */
/*                                                                    */
/*  °ú¿ô     : (in ) double idazel[2]  £Á£Ú£Å£ÌÃÍ                     */
/*                                     [0]: Êý°Ì³Ñ   [rad]            */
/*                                     [1]: ¶Ä³Ñ     [rad]            */
/*             (in ) double idweat[3]  µ¤¾Ý¥Ñ¥é¥á¥¿                   */
/*                                     [0]: µ¤²¹     [ÅÙ]             */
/*                                     [1]: µ¤°µ     [hp]             */
/*                                     [2]: ¿å¾øµ¤°µ [hp]             */
/*             (in ) double idgdin[2]  ·ÐÅÙ¡¢°ÞÅÙ                     */
/*                                     [0]: ·ÐÅÙ                      */
/*                                     [1]: °ÞÅÙ                      */
/*             (in ) double idlast     ½Ö»þ¤Î»ë¹±À±»þ                 */
/*                                                                    */
/*                                                                    */
/*             (out) double odazel[2]  Âçµ¤º¹ÊäÀµ¸å¤Î£Á£Ú£Å£ÌÃÍ       */
/*                                     [0]: Êý°Ì³Ñ   [rad]            */
/*                                     [1]: ¶Ä³Ñ     [rad]            */
/*             (out) double odrade[2]  Âçµ¤º¹ÊäÀµ¸å¤ÎÂ¬¿´»ë°ÌÃÖ       */
/*                                     [0]: ÀÖ·Ð     [rad]            */
/*                                     [1]: ÀÖ°Þ     [rad]            */
/*                                                                    */
/*  Ãí°Õ»ö¹à :                                                        */
/*                                                                    */
/*  ºîÀ®¼Ô   : FUJITSU LIMITED    1997.09.30                          */
/*                                                                    */
/*  ¥á¥â     :                                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  NORMAL     0
#define  ABNORMAL  -1

int     trk_30( idazel, idweat, idgdin, idlast, odazel, odrade )
double  idazel[2];          /* (in ) £Á£Ú£Å£ÌÃÍ                       */
double  idweat[3];          /* (in ) µ¤¾Ý¥Ñ¥é¥á¥¿                     */
double  idgdin[2];          /* (in ) ·ÐÅÙ¡¢°ÞÅÙ                       */
double  idlast;             /* (in ) ½Ö»þ¤Î»ë¹±À±»þ                   */
double  odazel[2];          /* (out) Âçµ¤º¹ÊäÀµ¸å¤Î£Á£Ú£Å£ÌÃÍ         */
double  odrade[2];          /* (out) Âçµ¤º¹ÊäÀµ¸å¤ÎÂ¬¿´»ë°ÌÃÖ         */
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
  /* ½é´ü²½µÚ¤ÓÀÅÅªÊÑ¿ô¤Ø¤Î³ÊÇ¼                                       */
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
  /* £Æ£Ï£Ò£Ô£Ò£Á£Î¤Î¶¦ÄÌÎÎ°è¤ØÃÍ¤òÂåÆþ¤¹¤ë                           */
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
