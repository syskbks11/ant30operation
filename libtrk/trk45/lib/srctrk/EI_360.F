C----------------------------------------------------------------------C
C     MODULE NAME : EI_360                                             C
C                                                                      C
C     FUNCTION    : DERIVATION OF THE TOPOCENTRIC POSITION OF AN       C
C                   OBSERVING OBJECT FROM ITS GEOCENTRIC POSITION      C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.27                      C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      NONE                                                            C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
      SUBROUTINE EI_360 ( DELAST, JESTCR, JERCOD )
#include      "__DFTF"
C                                       * ARGUMENT VARIABLE DEFINITION *
      REAL*8      DELAST                ! 地方視恒星時
      INTEGER*2   JESTCR                ! 天体識別フラグ
      INTEGER*2   JERCOD(2)             ! リターンコード
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "XOBSIN"
#include      "XTMERT"
#include      "XSTINF"
#include      "XOBSEI"
#include      "XUNTCV"
#include      "XVWORK"
#include      "XANTEN"
C 
C                                       * WORK VARIABLE DEFINITION     *
C                                                                       
      REAL*8      DSS , DRD(2)
      INTEGER*2   JC, JHH, JMM, JRS
C
      REAL*8      DPINT1                ! 観測者の経度
      REAL*8      DPINT2                ! 観測者の緯度
      INTEGER*2   UPTIME                ! 初回フラグ
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C リターンコードの初期化
      JERCOD(1) = 0
      JERCOD(2) = 0
C 
C 方位角、仰角を直接入力する場合はスキャンは行なわない
      IF ( JESTCR. EQ. 2 ) THEN
        GOTO 100
      ENDIF
C
      DO 1000 JC = 2, 1, -1
C
C 観測アンテナのＶＬＢＩ座標系での位置
        DORTRU(1) = DVLBIP( 1, JC )
        DORTRU(2) = DVLBIP( 2, JC )
        DORTRU(3) = DVLBIP( 3, JC )
C
C 観測時の赤道座標（慣性座標）での位置を求める
C 極運動補正後の観測アンテナの経度緯度を求める
        CALL R_6100( JERCOD(1) )
C
C 求めた経度緯度をコモンへ設定する
        DGDINF( 1, JC ) = DLONAT(1)
        DGDINF( 2, JC ) = DLONAT(2)
C debug by Y.Koide 2007.12.22
#ifdef DEBUG
        write(*,*)'EI_360:DLONAT =',DLONAT
#endif
C
        IF( UPTIME .EQ. 1 ) THEN
          DPINT1 = DLONAT(1)
          DPINT2 = DLONAT(2)
        ENDIF
C
C 惑星の地心歳差を求める
c       write(*,*)'EI_360 DPRNUT= ', DPRNUT
        IF ( JESTCR .EQ. 3 ) THEN
          CALL R_6200( JERCOD(2) )
        ELSE
          DGCPRL(1) = DPRNUT(1)
          DGCPRL(2) = DPRNUT(2)
          DGCPRL(3) = DPRNUT(3)
        ENDIF
C debug by Y.Koide 2007.12.22
#ifdef DEBUG
        write(*,*)'EI_360 DGCPRL= ', DGCPRL
#endif
c       write(*,*)'EI_360 DGCPRL= ', DGCPRL
C
C 日周光行差を補正する
        CALL R_6300
C
C 地球の自転を補正する
        CALL R_6400
C
C 各アンテナ毎の測心における観測天体の位置ベクトル
        DXTCPS( 1, JC ) = DDIUAB(1)
        DXTCPS( 2, JC ) = DDIUAB(2)
        DXTCPS( 3, JC ) = DDIUAB(3)
C
C 瞬時における地平座標系での観測アンテナの位置ベクトル
        DAPSAE( 1 ,JC ) = DAZELW(1)
        DAPSAE( 2 ,JC ) = DAZELW(2)
C
 1000 CONTINUE
C
C 基準局の測心観測天体位置ベクトルを球面座標系に変換する
      CALL R_4400( DDIUAB ,DRD ,JRS )
      DICTRD(1) = DRD(1)               ! α
      DICTRD(2) = DRD(2)               ! δ
      DICTRD(3) = DXLAST - DRD(1)      ! Ｈ
#ifdef DEBUG
      write(*,*)'EI_360:DDIUAB   =',DDIUAB
      write(*,*)'EI_360:DRD      =',DRD
      write(*,*)'EI_360:DICTRD   =',DICTRD
      write(*,*)'EI_360:DXLAST   =',DXLAST
#endif
C
C 瞬時の地方恒星時
      JHH = IDINT( DXLAST * DRADDH )
      JMM = IDINT( DXLAST * DRNHMN ) - JHH * 60
      DSS = DXLAST * DRNHSC - JHH * 3600 - JMM * 60
      DELAST = DFLOAT( JHH ) * 1D4 + DFLOAT( JMM ) * 1D2 + DSS
C debug by Y.Koide 2007.12.22
#ifdef DEBUG
      write(*,*)'EI_360:DELAST   =',DELAST
#endif
C----------------------------------------------------------------------C
C END PROCEDURE                                                        C
C----------------------------------------------------------------------C
C
  100 CONTINUE
C
      RETURN
      END
