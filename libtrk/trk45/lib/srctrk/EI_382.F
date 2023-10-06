      SUBROUTINE EI_382( IBAND, IDSBFG )
C----------------------------------------------------------------------C
C     MODULE NAME : EI_382                                             C
C                                                                      C
C     FUNCTION    : DELAY - FRINGE TRACKING ; FRINGE & FX              C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1991.05.31                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C 宣言
#include      "__DFTF"                                                
C
C 引数
      INTEGER*4     IBAND              ! フリンジ回転方向
      INTEGER*4     IDSBFG             ! ＤＳＢフラグ
C
C コモン領域
#include      "EI_LATR"
#include      "EI_TTME"
#include      "XDFWRK"
C
C 作業領域
      REAL*8        DFLO, DFIF
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C ＳＳＢの場合
      IF ( IDSBFG .EQ. 0 ) THEN
        DFRGFR(1, 1) = JTFRRD(1) * ( DFBETA(1) + RTFROF(1, 1) )
        DFRIPH(1, 1) = JTFRRD(1) * DALPHA(1)
        DFRGFR(1, 2) = 0.0D0
        DFRIPH(1, 2) = 0.0D0
C
C ＤＳＢの場合
      ELSE IF ( IDSBFG .GT. 0 ) THEN
C
C ローカル周波数
        DFLO = ( 1D0 - IBAND * DTMLPA ) * DTFRCR
     +       - IBAND * DTMLPB
C
C 中間周波数
        DFIF = DTMLPA * DTFRCR + DTMLPB
C
C フリンジローテータ１
        DFRGFR(1, 1) = ( DFBETA(1) * DFLO / DTFRCR                
     +               + RTFROF(1, 2) ) * JTFRRD(1)               
        DFRIPH(1, 1) = DALPHA(1) * DFLO / DTFRCR * JTFRRD(1)      
C
C フリンジローテータ２
        DFRGFR(1, 2) = ( DFBETA(1) * DFIF / DTFRCR                
     +               + RTFROF(1, 2) ) * JTFRRD(2)               
        DFRIPH(1, 2) = DALPHA(1) * DFIF / DTFRCR * JTFRRD(2)      
      ENDIF
C
C デバック用
      DFRGFR(1, 1) = JTFRRD(1) * ( DFBETA(1) + RTFROF(1, 1) )
      DFRIPH(1, 1) = JTFRRD(1) * DALPHA(1)
      DFRGFR(1, 2) = JTFRRD(1) * ( DFBETA(1) + RTFROF(1, 1) )
      DFRIPH(1, 2) = JTFRRD(1) * DALPHA(1)
C
C
      RETURN
      END
