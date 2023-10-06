      SUBROUTINE EI_381 ( DINTRL, DLW )                                
C----------------------------------------------------------------------C
C     MODULE NAME : EI_381                                             C
C                                                                      C
C     FUNCTION    : DELAY - FRINGE TRACKING                            C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1991.03.20                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE 1996.10.09 遅延追尾を２次で行う                            C
C           1996.12.27 遅延オフセットのバグ改修                        C
C                                                                      C
C----------------------------------------------------------------------C
C
C 宣言
#include      "__DFTF"
C                                                                       
C 引数
      REAL*8        DLW(3)              ! ジオメティカルディレイ
      REAL*8        DINTRL              ! フリンジローテータ制御間隔
C
C コモン領域
#include      "EI_LATR"
#include      "EI_TTME"
#include      "XDFWRK"
#include      "XSTINF"
#include      "XOBSEI"
#include      "XTRK00"
#include      "XMATHC"
C
C                                       * WORK VARIABLE DEFINITION     *
C
      REAL*8        DWK,  DRTMT(3,3), DBSL(3), DVW(3),
     +              DHAG, DA, DB, DC, DFHA, DLI, DLR
C     REAL*8        DWRK
      INTEGER*2     JZ
C
C                                       * CONSTANT DATA DEFINITION     *
C
      DATA  JZ / 3 /
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C 観測者の経度
      DWK = DGDINF(1,1)
C
C 極軸回りに回転する行列作成
      CALL R_5410( DWK, DRTMT, JZ, JRS )
C
C ベースラインベクトルへ適応
      DBSL(1) = DBASEL(1,1)
      DBSL(2) = DBASEL(2,1)
      DBSL(3) = DBASEL(3,1)
C 回転行列とベースラインベクトルの積
      CALL DMAV(DRTMT, 3, 3, 3, DBSL, DVW, ICON)
C
C 観測点子午線基準の地球固定座標系のベースラインベクトル
      DBASEV(1,1) = DVW(1)
      DBASEV(2,1) = DVW(2)
      DBASEV(3,1) = DVW(3)
C                                                                       
C 基準局におけるアワーアングル
      DFHA = DICTRD(3) - DTMPHA
      IF ( DFHA .LT. -4.0 ) THEN
        DFHA = ( DICTRD(3) + DTWOPI ) - DTMPHA
      ELSEIF ( DFHA .GT. DTWOPI ) THEN
        DFHA = ( DICTRD(3) - DTWOPI ) - DTMPHA
      ENDIF
      DHAG = DICTRD(3)
CCCC  write(*,*) 'EI_381  2 , 1 , DFHA, DINTRL =',
CCCC +DICTRD(3), DTMPHA, DFHA, DINTRL
C
C DA+DB は天体の方向ベクトルとベースラインの内積
C DFHA * DC は DA+DB の一回微分
      DA = DBASEV(3, 1) * DSIN(DICTRD(2))
      DB = (  DBASEV(1, 1) * DCOS(DHAG)
     +      - DBASEV(2, 1) * DSIN(DHAG) ) * DCOS(DICTRD(2))
      DC =-(  DBASEV(1, 1) * DSIN(DHAG)
     +      + DBASEV(2, 1) * DCOS(DHAG) ) * DCOS(DICTRD(2))
      DLI    = ( DA + DB*(1D0 + DFHA**2 / 12D0) )
      DLR    = ( DC - 0.5*DB*DFHA ) * DFHA / DINTRL
#ifdef DEBUG
      write(*,*)'EI_381:DA,DB,DC=',DA,DB,DC
      write(*,*)'EI_381:DLI,DLR =',DLI,DLR
      write(*,*)'EI_381:DTFRCR  =',DTFRCR
      write(*,*)'EI_381:DTMPHA  =',DTMPHA
      write(*,*)'EI_381:DFHA    =',DFHA
#endif
C
C 初期位相と位相変化量
      DALPHA(1) = DLI * DTFRCR
      DFBETA(1) = DLR * DTFRCR
#ifdef DEBUG
      write(*,*)'EI_381:DALPHA(1)=',DALPHA(1)
      write(*,*)'EI_381:DFBETA(1)=',DFBETA(1)
#endif
C
C 遅延時間
C     DLW(1) = DA + DB - DFHA**3 * DC / 120.0D0 - DTDLOF(1)
C     changed by H.ohta  1997.1.16
CC    DLW(1) = DA + DB - DFHA**3 * DC / 120.0D0 + DTDLOF(1)
CC    DLW(2) = DC * ( DFHA / DINTRL ) * ( 1.0D0 + DFHA**2 * 1.0D-1 )
CC    DLW(3) = -(DFHA/DINTRL)**2 * ( DFHA * DC + 2.0D0 * DB ) / 4.0D0
CCC   changed by H.ohta  1997.1.17
      DLW(1) =-DA - DB + DFHA**3 * DC / 120.0D0 + DTDLOF(1)
      DLW(2) =-DC * ( DFHA / DINTRL ) * ( 1.0D0 + DFHA**2 * 1.0D-1 )
      DLW(3) =  (DFHA/DINTRL)**2 * ( DFHA * DC + 2.0D0 * DB ) / 4.0D0
C      DFHA = DFHA / DINTRL
C      DLW(1) = DA + DB + ( DFHA * DINTRL )**3  * DC / 120.0D0
C      DLW(2) = - DC * DFHA * ( 1.0D0 + ( DFHA * DINTRL )**2 * 1.0D-1 )
C      DLW(3) = DFHA**2 * ( DFHA * DINTRL * DC - 2.0D0 * DB ) / 2.0D0
C
#ifdef DEBUG
      DWRK = DFHA**2
      write(*,*)'EI_381:DFHA**2  =',DWRK
      DWRK = DFHA**3
      write(*,*)'EI_381:DFHA**3  =',DWRK
      DWRK = DFHA**3 * DC / 120.0D0
      write(*,*)'EI_381:DFHA**3*DC/120=',DWRK
      write(*,*)'EI_381:DDELAY(1)=DA + DB + DFHA**3 * DC / 120'
      write(*,*)'EI_381:DDELAY(1)=',DLW(1)
      DWRK = DC * DFHA / DINTRL
      write(*,*)'EI_381:DC * DFHA / DINTRL=',DWRK
      DWRK = 1.0D0 + DFHA**2 * 1.0D-1
      write(*,*)'EI_381:1.0D0 + DFHA**2 * 1.0D-1=',DWRK
      write(*,*)'EI_381:DDELAY(2)=-DC*(DFHA/DINTRL)*(1+DFHA**2/10)'
      write(*,*)'EI_381:DDELAY(2)=',DLW(2)
      write(*,*)'EI_381:DDELAY(3)=',DLW(3)
#endif
      RETURN
      END
