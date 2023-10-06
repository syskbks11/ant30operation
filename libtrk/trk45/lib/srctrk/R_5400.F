      SUBROUTINE R_5400
C----------------------------------------------------------------------C
C     MODULE NAME : R_5400                                             C
C                                                                      C
C     FUNCTION    : CORRECTION FOR PRECESSION AND NUTATION             C
C                   OF THE EARTH                                       C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.27                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      PRECESSION MATRIX IS CALCULATED IN THE MODULE.                  C
C      NUTATION PARAMETERS ARE CALCULATED IN R_2200                    C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include     "__DFTF"
C                                       * COMMON AREA OF INSTRUCTION   *
#include     "XPRCNT"
#include     "XTMERT"
#include     "XVWORK"
C
C                                       * WORK VARIABLE DEFINITION     *
C
      REAL*8      DCZETA(3), DCZA(3), DCTHET(3),
     +            DPZETA,    DPZA,    DPTHET,
     +            DPRMT1(3,3), DPRMT2(3,3), DPRMT3(3,3),
     +            DNTMT1(3,3), DNTMT2(3,3), DNTMT3(3,3),
     +            DWRKM1(3,3), DWRKM2(3,3), DWK1, DWK2, DWK3, DVW(3)
      INTEGER*2   JX, JY, JZ, JECODE(3)
      INTEGER*4   ICON(3)
C                                       * CONSTANT DATA DEFINITION     *
      DATA   DCZETA /  1.1180860865024400D-02,  1.4635555405334672D-06,
     +                 8.7256766326094287D-08 /
      DATA   DCZA   /  1.1180860865024400D-02,  5.3071584043698686D-06,
     +                 8.8250634372368836D-08 /
      DATA   DCTHET /  9.7171734551696708D-03, -2.0684575704538353D-06,
     +                -2.0281210721855219D-07 /
      DATA   JX, JY, JZ / 1, 2, 3 /
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 歳差計算
      DPZETA =   DCZETA(1) * DCENTR(1)
     +         + DCZETA(2) * DCENTR(1)**2
     +         + DCZETA(3) * DCENTR(1)**3
      DPZA   =   DCZA(1)   * DCENTR(1)
     +         + DCZA(2)   * DCENTR(1)**2
     +         + DCZA(3)   * DCENTR(1)**3
      DPTHET =   DCTHET(1) * DCENTR(1)
     +         + DCTHET(2) * DCENTR(1)**2
     +         + DCTHET(3) * DCENTR(1)**3
C
C 歳差
C 回転行列作成
      DWK1 = - DPZA
      DWK2 =   DPTHET
      DWK3 = - DPZETA
      CALL R_5410( DWK1 , DPRMT1 , JZ , JECODE(1) )
      CALL R_5410( DWK2 , DPRMT2 , JY , JECODE(2) )
      CALL R_5410( DWK3 , DPRMT3 , JZ , JECODE(3) )
      CALL DMGGM( DPRMT2, 3, DPRMT3, 3, DWRKM1, 3,
     +                                  3, 3, 3, DVW, ICON(1) )
      CALL DMGGM( DPRMT1, 3, DWRKM1, 3, DWRKM2, 3,
     +                                  3, 3, 3, DVW, ICON(2) )
C
C 章動
C 回転行列作成
      DWK1 = - ( DMNOBL + DNTOBL )
      DWK2 = - DNTLON
      DWK3 =   DMNOBL
      CALL R_5410( DWK1 , DNTMT1 , JX , JECODE(1) )
      CALL R_5410( DWK2 , DNTMT2 , JZ , JECODE(2) )
      CALL R_5410( DWK3 , DNTMT3 , JX , JECODE(3) )
      CALL DMGGM( DNTMT3, 3, DWRKM2, 3, DWRKM1, 3,
     +                                  3, 3, 3, DVW, ICON(1) )
      CALL DMGGM( DNTMT2, 3, DWRKM1, 3, DWRKM2, 3,
     +                                  3, 3, 3, DVW, ICON(2) )
      CALL DMGGM( DNTMT1, 3, DWRKM2, 3, DPRNMT, 3,
     +                                  3, 3, 3, DVW, ICON(3) )
C
C                                      *-------------------------------*
C                                      * CORRECTION                    *
C                                      *-------------------------------*
C 天体の方向ベクトルへ歳差行列を作用させる
C 天体の方向ベクトルへ章動行列を作用させる
      CALL DMAV( DPRNMT , 3 , 3 , 3 , DANABR , DPRNUT , ICON(1) )
C
      RETURN
      END
