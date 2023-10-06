C----------------------------------------------------------------------C
C     MODULE NAME : R_6300                                             C
C                                                                      C
C     FUNCTION    : CORRECTION OF DIURNAL ABERRATION                   C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.11                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
      SUBROUTINE R_6300
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
C                                       * DEFAULT DEFINITION           *
#include     "__DFTF"
C                                       * COMMON AREA OF INSTRUCTION   *
#include     "XTMERT"
#include     "XOBSIN"
#include     "XVWORK"
#include     "XIERST"
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8        DVLPL(3), DSP, DFC, DWKV(3), DNRM
      INTEGER*2     JC 
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * BEGIN                         *
C                                      *-------------------------------*
C アンテナ速度＝アンテナベクトルの絶対値＊自転角速度＊cos(緯度)
      DFC = DGCDIS * DVLERT * DCOS( DLONAT(2) )                         
c     for DB start
#ifdef DEBUG
      write(*,*)'ANTENNA veracity=',DFC
#endif
C
C アンテナの速度を地球慣性座標系でのベクトルを求める
      DOGCVL(1) = DFC * ( -DSIN( DXLAST ) )                             
      DOGCVL(2) = DFC * DCOS( DXLAST )                                  
      DOGCVL(3) = 0D0                                                   
C                                                                       
C 地心に対するアンテナの速度ベクトルを光速で割る
      DO 1000 JC = 1, 3, 1                                              
        DVLPL(JC) = DOGCVL(JC) / DVLLGT                                 
 1000 CONTINUE                                                          
C
C 地心視差を補正した天体の位置ベクトルと地心に対するアンテナの速度ベクト
C ルの内積を取る
      CALL DSUM( DGCPRL, DVLPL, 3, 1, 1, DSP )
C
C 自転による光行差を考慮した天体の位置ベクトルを計算する
      DO 2000 JC = 1, 3, 1
        DWKV(JC) = DGCPRL(JC) + DVLPL(JC) - DSP * DGCPRL(JC)            
 2000 CONTINUE
C
C 天体の位置ベクトルを正規化する
      CALL R_3210( DWKV, DNRM, DDIUAB )
C debug by Y.Koide 2007.12.22
#ifdef DEBUG
      write(*,*)'R_6300:DDIUAB  =',DDIUAB
#endif
C
      RETURN
C
      END
