      SUBROUTINE R_8100 ( DTSTJD , DRSLMT , JCRF , JERCOD )             
C----------------------------------------------------------------------C
C     MODULE NAME : R_8100                                             C
C                                                                      C
C     FUNCTION    : DERIVE RADIAL VELOCITY OF LSR(OR HELIOCENTER)      C
C                   AND A PLANET                                       C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.26                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C
C 宣言
#include     "__DFTF"
C
C 引数
      REAL*8        DTSTJD(8)           ! 惑星データ読みだし時刻
      REAL*8        DRSLMT              ! 瞬時の太陽運動
      INTEGER*2     JCRF                ! 天体の識別
      INTEGER*2     JERCOD              ! リターンコード
C
C コモン領域
#include     "XGLCST"
#include     "XSTINF"
#include     "XTMERT"
#include     "XPRCNT"
#include     "XEPHEM"
#include     "XOBSIN"
#include     "XANTEN"
#include     "XCOMET"
C                                                                       
C                                       * WORK VARIABLE DEFINITION     *
      common /dopvel/ dopant
      real*8 dopant
C                                                                       
      REAL*8        DDIR(3)             ! 位相中心の測心視位置ベクトル
      REAL*8        DSV                 ! 太陽の向点方向の速度
      REAL*8        DSLMT(3)            ! 太陽速度ベクトル
      REAL*8        DAY                 ! １日の秒
      REAL*8        DOTH(3), DROTH, DCF(8), DXT(6), DVW(8)
      REAL*8        DPV, DWK1(3), DWK2(3)
      REAL*8        DWRK
      INTEGER*4     IC, IC1, IC2, ICON                                  
      INTEGER*2     UPTIME
C                                                                       
C                                       * INITIALIZE DATA              *
      DATA     IDPOS  / 2 /                                             
C                                       * CONSTANT DATA DEFINITION     *
      PARAMETER ( DSV = 2.0D4 , DAY = 86400D0 )                         
C                                                                       
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C リターンコードの初期化
      JERCOD = 0
C
C 位相中心の測心視位置ベクトル
      DO 1500 IC = 1, 3                                                 
        DDIR(IC) = DXTCPS( IC, 1 )                                      
 1500 CONTINUE                                                          
C
C 太陽の向点の方向ベクトル計算
      CALL R_4500( DSMTJ2, DWK1 )
      DO 2000 IC = 1, 3
        DWK2(IC) = DSV * DWK1(IC)
 2000 CONTINUE
C
C 歳差、章動の回転行列を作用させ瞬時の太陽運動のベクトルを求める
      CALL DMAV( DPRNMT, 3, 3, 3, DWK2, DSLMT, ICON)
C
C 太陽の向点運動ベクトルの視線方向成分
      CALL DSUM( DSLMT, DDIR, 3, 1, 1, DPV )
C
C 観測者の視線方向の速度成分
      DRSLMT = - DPV
#ifdef DEBUG
      WRITE(*,*)'veracity of SUNrad =',DRSLMT
#endif
C
C 惑星以外の場合
      IF ( JCRF .NE. 3 ) THEN
C
C 地球の速度ベクトル
        DO 1000 IC1 = 1, 3
C スプライン補間係数のコピー
          DO 1100 IC2 = 1, 8
            DCF(IC2) = DINTHL( IC2, IC1 , 1 )
            IF ( IC2 .LE. 6 ) DXT(IC2) = DHLNOD( IC2, IC1, 1 )
 1100     CONTINUE
C スプライン補間
          CALL DBIF3( DTSTJD, 8, 3, DCF, DXT, 1, DTDBJD, IDPOS, DPV,
     +                DVW, ICON )
          IF ( ICON .EQ. 30000 ) JERCOD = 1
C 単位変換 [m/day]->[m/sec]
          DOTH(IC1) = DPV / DAY
 1000   CONTINUE
C
C 歳差、章動回転行列を作用させ瞬時の速度ベクトルを求める
        CALL DMAV( DPRNMT, 3, 3, 3, DOTH, DWK1, ICON )                  
C for DB start 96.08.13
      CALL DSUM(DWK1, DDIR, 3, 1, 1, DPV )
      DPV = - DPV
#ifdef DEBUG
      WRITE(*,*)'veracity of EARTHrad =',DPV
#endif
      CALL DSUM(DOGCVL, DDIR, 3, 1, 1, DWRK )
      DPV = - DWRK
#ifdef DEBUG
      WRITE(*,*)'veracity of ANTENNAread =',DPV
      WRITE(*,*)'veracity of ANTENNA veracity=',DOGCVL
#endif
C for DB end
C
C 観測者の地心速度ベクトルを加える
        DWK2(1) = DWK1(1) + DOGCVL(1)                                   
        DWK2(2) = DWK1(2) + DOGCVL(2)                                   
        DWK2(3) = DWK1(3) + DOGCVL(3)                                   
C                                                                       
C 観測者の視線方向の速度成分を求める（測心）
        CALL DSUM(DWK2, DDIR, 3, 1, 1, DPV )
        DROTH = - DPV
#ifdef DEBUG
      WRITE(*,*)'veracity of (EARTH+ANTENNA)rad =',DROTH
#endif
C
C  * 視線速度の基準系が”銀河系中心（ＧＡＬ）”に対し未対応状態
C  * 現状、”ＬＳＲ”と同処理を行う。
C  銀経緯の場合
        IF ( JCRF .EQ. 1 ) THEN
          DSTRVL = DROTH + DRSLMT
#ifdef DEBUG
      WRITE(*,*)'veracity of (SUN+EARTH+ANTENNA)rad =',DSTRVL
#endif
C
C 赤経緯の場合
        ELSEIF ( JCRF .EQ. 2 ) THEN
          DSTRVL = DROTH
        ENDIF
C
C 惑星の場合
      ELSEIF (JCRF .EQ. 3 ) THEN
C
C 惑星の地心速度
        DO 3000 IC = 1, 3
          IF ( DCCDST .NE. -1.0D0 ) THEN
            DWK1(IC) = DCCDLT(IC)
          ELSE
            DWK1(IC) = DPLBVL(IC) - DOPBVL(IC)
          ENDIF
 3000   CONTINUE
C
C 歳差、章動の回転行列を作用させ瞬時の速度ベクトルを求める
        CALL DMAV( DPRNMT, 3, 3, 3, DWK1, DWK2, ICON )
C
C 観測者の地心速度ベクトルを加える
        DWK1(1) = DWK2(1) - DOGCVL(1)
        DWK1(2) = DWK2(2) - DOGCVL(2)
        DWK1(3) = DWK2(3) - DOGCVL(3)
C                                                                       
C 観測者の視線方向の速度成分を求める
        CALL DSUM( DWK1, DDIR, 3, 1, 1, DPV )
        DSTRVL = DPV
C
      ENDIF
C
      IF( UPTIME .EQ. 1 ) THEN
        dopant = DSTRVL
      ENDIF
      RETURN
      END
