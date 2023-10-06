C----------------------------------------------------------------------C
C     MODULE NAME : R_6100                                             C
C                                                                      C
C     FUNCTION    : DERIVATION OF THE OBSERVER'S POSITION              C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.07                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
      SUBROUTINE R_6100 ( JERCOD )
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
C                                       * DEFAULT DEFINITION           *
#include     "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      INTEGER*2     JERCOD              ! リターンコード
C                                       * COMMON AREA OF INSTRUCTION   *
#include     "XTMERT"
#include     "XMATHC"
#include     "XOBSIN"
C
C                                       * WORK VARIABLE DEFINITION     *
C
      REAL*8        DRM1(3,3), DRM2(3,3), DRM3(3,3), DRM4(3,3),
     +              DWKV(3), DVW(3), DWK,
     +              DLAT, DSQE, DFLT, DRDI, DLC, DDR, DRTOL
      INTEGER*4     IRS
      INTEGER*2     JX, JY, JZ, JRS
C                                       * CONSTANT DATA DEFINITION     *
      DATA    JX, JY, JZ / 1, 2, 3 /
C                                               * 1 / 298.257 & RADIUS *
C     DATA    DFLT, DRDI / 3.3528131778969144D-3, 6378136D0 /           
      DATA    DRTOL      / 5D-8 /
C *** TEST S. 90.10.24 ***                      * BESSELIAN            *
C      DATA    DFLT, DRDI / 3.3428046130703660D-3, 6377397D0 /
C                                               * GRS-80 *
      DATA    DFLT, DRDI / 3.3528106811823189D-3, 6378137D0 /
C      DFLT = 1D0 / 298.257222101;
C *** TEST E. 90.10.24 ***
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C リターンコードの初期化
      JERCOD = 0
C
C 長周期極運動と地球の自転による補正を行なう行列作成
      DWKV(1) = DNPLXY(1)
      DWKV(2) = DNPLXY(2)
      DWKV(3) = - DXGAST
      CALL R_5410 ( DWKV(1), DRM1, JY, JRS )
      CALL R_5410 ( DWKV(2), DRM2, JX, JRS )
      CALL R_5410 ( DWKV(3), DRM3, JZ, JRS )
C debug by Y.Koide 2007.12.22
C#ifdef DEBUG
C      write(*,*)' R_6100: DWKV   =',DWKV
C#endif
C
C 長周期極運動の補正を行なう行列を観測局のベクトルへ適用する
      CALL DMGGM ( DRM2, 3, DRM1, 3, DRM4, 3, 3, 3, 3, DVW, IRS )
      CALL DMAV  ( DRM4, 3, 3, 3, DORTRU, DWKV , IRS )
C
C 球面座標値を求める(経度、緯度)
      CALL R_4400 ( DWKV, DLONAT, JRS )
      IF( JRS .NE. 0 ) JERCOD = 1
C
C 緯度をベッセル楕円体での値で求める
C 収束させる初期値
      DLAT = DLONAT(2)
      DSQE = DFLT * ( 2D0 - DFLT )
      DDR = DSQRT( DWKV(1)**2 + DWKV(2)**2 )
C 収束するまで繰り返す
      DO 1000 IC = 1, 6
        DLC = DSQRT( 1D0 - DSQE * DSIN( DLAT )**2 )
        DLONAT(2) = DATAN(( DWKV(3) + DRDI * DSQE / DLC * DSIN( DLAT ))
     +                    / DDR )
        DWK = DABS( ( DLONAT(2) - DLAT ) / DLAT )
        IF ( DWK . LT. DRTOL ) THEN
          GOTO 100
        ELSE
          DLAT = DLONAT(2)
       ENDIF
C debug by Y.Koide 2007.12.22
C#ifdef DEBUG
C      write(*,*)'  R_6100: DLONAT  =',DLONAT
C#endif
 1000 CONTINUE
      JERCOD = 4
C
  100 CONTINUE
C 求めた２次元ベクトルDLONATは（経度、緯度）
C
C 長周期極運動を補正したベクトルをｚ軸を軸として回転し慣性座標系での
C ベクトルDORTRUを求める
      CALL DMAV ( DRM3, 3, 3, 3, DWKV, DORTRU, IRS )
C
C ベクトルDORTRUの絶対値DGCDISを求める
      CALL R_3210( DORTRU, DGCDIS, DWKV )
C
C グリニッジ視恒星時角に経度を加え地方視恒星時角を求める
      DWK = DXGAST + DLONAT(1)
      DXLAST = DMOD( DWK, DTWOPI )
C
      RETURN
C
      END
