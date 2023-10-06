      SUBROUTINE R_4100 ( DRADEC , DGLCLB , CESCFE )
C----------------------------------------------------------------------C
C     MODULE NAME : R_4100                                             C
C                                                                      C
C     FUNCTION    : COORDINATES TRANSFORMATION ; RA, DEC TO L, B       C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.04                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include     "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8       DRADEC(2)            ! 変換結果（銀河座標）
      REAL*8       DGLCLB(2)            ! 入力値  （赤道座標）
      CHARACTER*6  CESCFE               ! 分点名
C                                       * WORK VARIABLE DEFINITION     *
#include     "XGLCST"
#include     "XMATHC"
C
C                                       * WORK VARIABLE DEFINITION     * 
C
      REAL*8     DWK1
      REAL*8     DWK2
      REAL*8     DASEC
      INTEGER*2  JSEPC
C                                       * CONSTANT DATA DEFINITION     *
      DATA  DASEC   / 4.8481368110953599D-8 / 
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 分点の識別
      IF ( CESCFE .EQ. 'J2000' ) THEN
        JSEPC = 1
      ELSEIF ( CESCFE .EQ. 'B1950' ) THEN
        JSEPC = 2
      ENDIF
C
C
C----------------------------------------------------------------------C
C 銀緯を求める                                                         C
C----------------------------------------------------------------------C
      DGLCLB(2) = DASIN( DCOS( DRADEC(2) ) * DCOS( DGNPDE( JSEPC ) ) *
     +                   DCOS( DRADEC(1) - DGNPRA( JSEPC ) ) +
     +                   DSIN( DRADEC(2) ) * DSIN( DGNPDE( JSEPC ) )
     +                 )
C
C----------------------------------------------------------------------C
C 銀経を求める                                                         C
C----------------------------------------------------------------------C
C <<<<<< 分子 >>>>>>
C DSIN（赤緯）- DSIN（銀緯）* DSIN（銀河北極の赤緯）
C
      DWK1      = DSIN( DRADEC(2) ) - DSIN( DGLCLB(2) ) *
     +                                DSIN( DGNPDE( JSEPC ) )
C
C <<<<<< 分母 >>>>>>
C DCOS（赤緯）* DSIN（赤経 - 銀河北極の赤経）* DCOS（銀河北極の赤緯）
C
      DWK2      = DCOS( DRADEC(2) ) * DSIN( DRADEC(1) -
     +                                      DGNPRA( JSEPC ) ) *
     +                                DCOS( DGNPDE( JSEPC ) )
C
C <<<<<< 分母 >>>>>> が０の場合
C
      IF ( DWK2 .EQ. 0D0 ) GOTO 9000
C
C 銀経の算出
C
      DGLCLB(1) = DATAN( DWK1 / DWK2 )
C
C
C----------------------------------------------------------------------C
C アークタンジェント不定性の除去                                       C
C----------------------------------------------------------------------C
C 分子が負  かつ  分母が正 のとき
C
      IF ( ( DWK1 .LT. 0D0 ) .AND. ( DWK2 .GT. 0D0 ) ) THEN
        DGLCLB(1) = DGLCLB(1) + DTWOPI
C
C 分母が負 のとき
C
      ELSE IF ( DWK2 .LT. 0D0 ) THEN
        DGLCLB(1) = DGLCLB(1) + DPIVAL
      ENDIF
C
C 銀河赤道の対赤道昇交点を加える
C
      DGLCLB(1) = DGLCLB(1) + DGANGL( JSEPC )
      DGLCLB(1) = DMOD( DGLCLB(1) , DTWOPI )
C
      RETURN
C
C----------------------------------------------------------------------C
C <<<<<< 分母 >>>>>> が０の場合の処理                                  C
C----------------------------------------------------------------------C
 9000 CONTINUE
C
C 赤道座標が銀河の北極近傍の時
C
      IF( ( DABS( DRADEC(1) - DGNPRA(JSEPC) ) .LT. DASEC ) .AND.
     +    ( DABS( DRADEC(2) - DGNPDE(JSEPC) ) .LT. DASEC ) ) THEN
        DGLCLB(1) = 0D0
        DGLCLB(2) = DHLFPI
C
C 赤緯が π／２ の時
C
      ELSE IF ( DRADEC(2) .EQ. DHLFPI ) THEN
        DGLCLB(1) = DGANGL(JSEPC) + DHLFPI
        DGLCLB(2) = DGNPDE(JSEPC)
      ENDIF
C
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN
      END
