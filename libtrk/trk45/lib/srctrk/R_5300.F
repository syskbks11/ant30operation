      SUBROUTINE R_5300
C----------------------------------------------------------------------C
C     MODULE NAME : R_5300                                             C
C                                                                      C
C     FUNCTION    : LORENTZ TRANSFORMATION FROM THE NATURAL FRAME      C
C                   TO THE PROPER FRAME                                C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.06                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      ANNUAL ABERRATION IS CORRECTED BY THE MODULE.                   C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include     "__DFTF"
C
C                                       * COMMON AREA OF INSTRUCTION   *
#include     "XIERST"
#include     "XEPHEM"
#include     "XVWORK"
C
C                                       * WORK VARIABLE DEFINITION     *
C
      REAL*8      DGAMMA, DSCPR, DSPVD1
      REAL*8      DWRK
      REAL*8      DWRK1
      INTEGER*2   JC
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 特殊相対論におけるγ算出
      CALL DSUM( DOPBVL , DOPBVL , 3 , 1 , 1 , DSCPR )
C      DGAMMA = DSCPR / DVLLGT / DVLLGT
C      DGAMMA = 1D0 - DGAMMA
C      DGAMMA = DSQRT( DGAMMA )
C 1-γの平方根の精度が足りないのでテーラー展開して求める
      DWRK = - DSCPR / DVLLGT / DVLLGT
      DGAMMA = - 5.0D0 * DWRK / 128.0D0
      DGAMMA = DGAMMA * DWRK + 1.0D0 / 16.0D0
      DGAMMA = DGAMMA * DWRK - 1.0D0 / 8.0D0
      DGAMMA = DGAMMA * DWRK + 1.0D0 / 2.0D0
      DGAMMA = DGAMMA * DWRK + 1.0D0
C
C ローレンツ変換
      CALL DSUM( DOPBVL , DLTDFL , 3 , 1 , 1 , DSPVD1 )
      DWRK1 = DVLLGT + DSPVD1
      DO 1000 JC = 1 , 3 , 1
        DWRK = ( 1.0D0 + DSPVD1 / DVLLGT / ( 1.0D0 + DGAMMA ) )
        DWRK =  DWRK * DOPBVL( JC ) / DWRK1
        DANABR( JC ) =  DVLLGT * DGAMMA * DLTDFL( JC ) / DWRK1
        DANABR( JC ) =  DANABR( JC ) + DWRK
 1000 CONTINUE
C
      RETURN
      END
