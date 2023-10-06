      SUBROUTINE R_5100
C----------------------------------------------------------------------C
C     MODULE NAME : R_5100                                             C
C                                                                      C
C     FUNCTION    : CORRECTION OF THE PROPER MOTION & ANNUAL PARALLAX  C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.06                      C
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
C                                       * COMMON AREA OF INSTRUCTION   *
#include     "XIERST"
#include     "XSTINF"
#include     "XTMERT"
#include     "XEPHEM"
#include     "XVWORK"
#include     "XDUMMY"
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8      DAUSC, DWVC1(3), DWVC2(3), DWVC3(3), DNM, DMOV(3)
      INTEGER*2   JC
C                                       * CONSTANT DATA DEFINITION     *
      PARAMETER  ( DAUSC = 2.1094502D-2 )
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 作業用ベクトルの初期化
      DMOV(1) = 0D0
      DMOV(2) = 0D0
      DMOV(3) = 0D0
C
C 観測天体の、α、δから速度ベクトルを計算する
      DWVC1(1) = - DCOS( DXOBPS(2) ) * DSIN( DXOBPS(1) )
      DWVC1(2) =   DCOS( DXOBPS(2) ) * DCOS( DXOBPS(1) )
      DWVC1(3) =   0D0                                 
      DWVC2(1) = - DSIN( DXOBPS(2) ) * DCOS( DXOBPS(1) )
      DWVC2(2) = - DSIN( DXOBPS(2) ) * DSIN( DXOBPS(1) )
      DWVC2(3) =   DCOS( DXOBPS(2) )
      DWVC3(1) =   DXSTPS(1)
      DWVC3(2) =   DXSTPS(2)
      DWVC3(3) =   DXSTPS(3)
C
C DPRPCR : 固有運動
C DVELCR : 視線速度
C DANCRC : 年周視差
      DO 1000 JC = 1 , 3 , 1
        DMOV( JC ) =  DPRPCR(1) * DWVC1( JC ) + DPRPCR(2) * DWVC2( JC )
     +              + DAUSC * DVELCR * DANCRC * DWVC3( JC )
 1000 CONTINUE
C
C 補正値適応
      DO 2000 JC = 1 , 3 , 1
        DWVC1( JC ) = DXSTPS( JC ) + DCENTR(1) * DMOV( JC ) -
     +                DANCRC * DOPBAR( JC ) / DASUNT
 2000 CONTINUE 
C
C 天体の位置ベクトルの規格化    
      CALL R_3210( DWVC1, DNM, DPRPMT )
C
      RETURN
      END
