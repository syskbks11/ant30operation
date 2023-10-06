      SUBROUTINE EI_375()
C----------------------------------------------------------------------C
C     MODULE NAME : EI_375                                             C
C                                                                      C
C     FUNCTION    : CORRECTION OF INSTRUMENTAL POINTING ERROR          C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.27                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      IGNORE THE CORRECTION FOR (AZ,EL) OBSERVATION.                  C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
C                                       * DEFAULT DEFINITION           *
#include      "__DFTF"                                                
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "EI_TTME"                                               
#include      "XSTINF"                                                
#include      "XUNTCV"                                                
#include      "XOBSEI"                                                
#include      "XMATHC"                                                
C                                       * ARGUMENT VARIABLE DEFENITION *
C     INTEGER*2     JESTCR              ! 天体識別子
C
C                                       * WORK VARIABLE DEFINITION     *
C
C     REAL*8        DU, DSIAZ, DCOAZ, DSIEL, DCOEL,
C    +              DSISU, DCOSU, DCRAZ, DCREL
      REAL*8        DCRAZ, DCREL
C                    
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 観測局についてのみアンテナの器差補正を行なう
C 通常の天体についてのみ補正する
C     IF ( JESTCR .NE. 2 ) THEN
C 補正値計算
C       DSIAZ = DSIN ( DARFAE( 1, 1 ) )
C       DCOAZ = DCOS ( DARFAE( 1, 1 ) )
C       DSIEL = DSIN ( DARFAE( 2, 1 ) )
C       DCOEL = DCOS ( DARFAE( 2, 1 ) )
C       DU    = DARFAE( 1, 1 ) + DARFAE( 2, 1 )
C       DSISU = DSIN ( DU )
C       DCOSU = DCOS ( DU )
C
C       DCRAZ = ( DTIZPC(1,1) * DSIEL + DTIZPC(2,1)
C    +          + DTIZPC(3,1) * DCOEL
C    +          + DTIZPC(4,1) * DSIAZ * DSIEL
C    +          - DTIZPC(5,1) * DCOAZ * DSIEL
C    +          + DTIZPC(8,1) * DSISU
C    +          + DTIZPC(9,1) * DCOSU ) / DCOEL
C       DCREL =   DTIZPC(4,1) * DCOAZ + DTIZPC(5,1) * DSIAZ
C    +          + DTIZPC(6,1) * DCOEL + DTIZPC(7,1)
C    +          + DTIZPC(8,1) * DCOSU - DTIZPC(9,1) * DSISU
C
C 天体以外は補正を行なわない
C     ELSE
C 補正値初期化
      DCRAZ = 0D0
      DCREL = 0D0
C
C     ENDIF
C
C 補正値適用
      DISTCR( 1, 1 ) = DARFAE( 1, 1 ) * DRNDEG + DCRAZ
      DISTCR( 2, 1 ) = DARFAE( 2, 1 ) * DRNDEG + DCREL
      DISTCR( 1, 1 ) = DISTCR( 1, 1 ) * DPIVAL / 180.0D0
      DISTCR( 2, 1 ) = DISTCR( 2, 1 ) * DPIVAL / 180.0D0
C
      RETURN
      END
