      SUBROUTINE EI_371 ( JERCOD )
C----------------------------------------------------------------------C
C     MODULE NAME : EI_371                                             C
C                                                                      C
C     FUNCTION    : 測心視位置（赤経赤緯）算出                         C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1997.09.30                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include      "__DFTF"                                                
C                                                                       
C                                       * ARGUMENT VARIABLE DEFINITION *
C                                                                       
      INTEGER*2     JERCOD                                      
C                                                                       
C                                       * COMMON VARIABLE DEFINITION   *
C                                                                       
#include      "EI_TTME"
#include      "XSTINF"
#include      "XOBSEI"
#include      "XVWORK"
#include      "XMATHC"
#include      "XTMERT"
#include      "XOBSIN"
#include      "XANTEN"
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8        DPST(3), DSPH(2)                                    
      REAL*8        DRMT1(3, 3), DRMT2(3, 3)                                    
      REAL*8        DVW(3), DWK1, DWK2
      INTEGER*4     ICN, ICON
      INTEGER*2     JRS, JY, JZ
C
      DATA          JY, JZ   / 2, 3 /
C
      REAL*8        DXTCRA, DXTCDE      ! 測心視位置（赤経赤緯） 
      INTEGER*2     UPTIME              ! 初回フラグ
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C リターンコードの初期化
      JERCOD    = 0
C
C 計算結果をコモンへ設定
      DARFWK(1) = DAPSAE(1, 2)
      DARFWK(2) = DAPSAE(2, 2)
C
      DARFAE(1, 1) = DARFWK(1)
      DARFAE(2, 1) = DARFWK(2)
C
C 球面座標値計算
      DSPH(1) = DPIVAL - DARFWK(1)
      DSPH(2) = DARFWK(2)
C
      CALL R_4500( DSPH, DPST )
C
C 計算結果をコモンへ設定
      DO 1100 ICN = 1, 3 
        DARFCR(ICN, 1) = DPST(ICN)
 1100 CONTINUE
C
      DWK1 = DLONAT(2) - DHLFPI
      CALL R_5410( DWK1, DRMT1, JY, JRS )
      IF ( JRS .NE. 0 ) THEN
        JERCOD = -2 
        GOTO 9000
      ENDIF
C
      DWK2 = - DXLAST
      CALL R_5410( DWK2, DRMT2, JZ, JRS )
      IF ( JRS .NE. 0 ) THEN
        JERCOD = -3
        GOTO 9000
      ENDIF
C
      CALL DMGGM( DRMT2, 3, DRMT1, 3, DRMT2, 3, 3, 3, 3, DVW, ICON )
      IF ( JRS .NE. 0 ) THEN
        JERCOD = -4
        GOTO 9000
      ENDIF
C
      CALL DMAV( DRMT2, 3, 3, 3, DPST, DVW, ICON )
      IF ( JRS .NE. 0 ) THEN
        JERCOD = -5
        GOTO 9000
      ENDIF
C
      CALL R_4400( DVW, DSPH, JRS )
      IF ( JRS .NE. 0 ) THEN
        JERCOD = -6
        GOTO 9000
      ENDIF
C
      IF( UPTIME .EQ. 1 ) THEN
        DXTCRA = DSPH(1)
        DXTCDE = DSPH(2)
      ENDIF
C
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
 9000 CONTINUE
C
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
      RETURN
      END
