C----------------------------------------------------------------------C
C     MODULE NAME : R_6400                                             C
C                                                                      C
C     FUNCTION    : CORRECTION OF EARTH ROTATION                       C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.11                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
      SUBROUTINE R_6400
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
C                                       * DEFAULT DEFINITION           *
#include     "__DFTF"
C                                       * COMMON AREA OF INSTRUCTION   *
#include     "XMATHC"
#include     "XTMERT"
#include     "XOBSIN"
#include     "XVWORK"
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8        DRM1(3,3), DRM2(3,3), DVW(3), DWK1, DWK2, DSP(2)    
      INTEGER*4     ICON                                                
      INTEGER*2     JX, JY, JZ, JRS                                     
C                                       * CONSTANT DATA DEFINITION     *
      DATA   JX, JY, JZ / 1, 2, 3 /                                     
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C                                      *-------------------------------*
C                                      * BEGIN                         *
C                                      *-------------------------------*
C ９０度ー観測局の緯度だけＹ軸回りに回転する行列作成
      DWK1 = DHLFPI - DLONAT(2)                                         
      CALL R_5410 ( DWK1, DRM1, JY, JRS )                               
C
C 地方視恒星時角Ｚ軸回りに回転する行列作成
      DWK2 = DXLAST                                                     
      CALL R_5410 ( DWK2, DRM2, JZ, JRS )                               
C                                      * CORRECTION                    *
C Ｚ軸を地球の自転軸、Ｘ軸を春分点方向とする座標系から、観測アンテナを中
C 心とし、天頂をＺ軸、南をＸ軸とする座標系へ変換する行列DRM1を求める
      ICON = JX -1
      CALL DMGGM  ( DRM1, 3, DRM2, 3, DRM1, 3, 3, 3, 3, DVW, ICON )     
C
C 自転による光行差まで補正した天体の位置ベクトルDDIUABを上で作成した行列
C に作用させ、アンテナ中心の座標系で天体の位置ベクトルDHOLAPを求める
      CALL DMAV   ( DRM1, 3, 3, 3, DDIUAB, DHOLAP, ICON)                
C debug by Y.Koide 2007.12.22
#ifdef DEBUG
      write(*,*)'R_6400:DHOLAP  =',DHOLAP
#endif
C
C 天体の位置ベクトル(３次元)を球面座標系(２次元)へ変換する
      CALL R_4400 ( DHOLAP, DSP, JRS )
C
C DSP(1)は南を０とした方位角であるから１８０度シフトし、北を０度とする
C 方位角を求める
      DSP(1) = DPIVAL - DSP(1)                                          
      IF ( DSP(1) .LT. 0D0 ) THEN                                       
        DAZELW(1) = DSP(1) + DTWOPI                                     
      ELSE                                                              
        DAZELW(1) = DSP(1)                                              
      ENDIF                                                             
C 
C 仰角
      DAZELW(2) = DSP(2)                                                
C                                                                       
      RETURN
C
      END
