      SUBROUTINE EI_350( JESTC, JERCOD )
C----------------------------------------------------------------------C
C     MODULE NAME : EI_350                                             C
C                                                                      C
C     FUNCTION    : DERIVATION OF THE GEOCENTRIC APPARENT POSITION OF  C
C                   AN OBSERVING OBJECT                                C
C                                                                      C
C     AUTHOR      : FUJTISU LIMITED    1990.09.26                      C
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
C                                       * ARGUMENT VARIABLE DEFINITION *
      INTEGER*2     JESTC               ! 天体識別コード
      INTEGER*2     JERCOD              ! リターンコード
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "EI_TTME"
#include      "EI_LMAI"
#include      "XTMERT"
#include      "XSTINF"
#include      "XVWORK"
#include      "XTRK00"
C                                       * WORK VARIABLE DEFINITION     *
      INTEGER*2     JN3, JRC, JN
      CHARACTER     CEL, CM*79, CT*12, CW*38, CW25*25
C                                       * CONSTANT DATA DEFINITION     *
      DATA    JN3  / 3 /
      DATA    CEL, CW / 'E', 'XEI306E EI_350  EPH CALCULATION ERROR:' /
C
C----------------------------------------------------------------------C
C PROCEDURE                                                            C
C----------------------------------------------------------------------C
C
C リターンコードの初期化
      JERCOD = 0
C                                                                       
C 方位角、仰角を直接入力の場合
      IF ( JESTC .EQ. 2 ) THEN
        GOTO 100
      ENDIF
C                                                                       
C 太陽系中心座標系に於ける地球の速度ベクトルを計算する
      CALL R_3100( DTSTJD, JN3 , JRC )
      IF ( JRC .GT. 0 ) GOTO 9000
C
C α、δからＸＹＺの値を求める
      CALL R_4500( DXOBPS ,DXSTPS )
c     write(*,*) 'EI_350  DXOBPS =', DXOBPS
c     write(*,*) 'EI_350  DXSTPS =', DXSTPS
C
C 太陽系外の天体について固有運動、年周視差を補正する
      JN = JESTC
c     write(*,*) 'EI_350  JESTC =', JESTC
      IF ( JESTC .EQ. 1 ) THEN
        CALL R_5100
      ENDIF
C If you want to RADEC scan at comet, comment out next 4 lines.
c     IF ( CESTNM .EQ. 'COMET' ) THEN
c       CALL R_5110
c       JN = 1
c     ENDIF
C If you want to RADEC scan at planet or comet,
C comment out next 4 lines.
      IF ( JESTC .EQ. 3 ) THEN
        CALL R_5110
        JN = 1
      ENDIF
C
C 太陽重力による幾何学的光線の湾曲を求める
      IF (CESTNM .EQ. 'SUN'  .AND.  JN .EQ. 1) THEN
        DLTDFL(1) =  DPRPMT(1)
        DLTDFL(2) =  DPRPMT(2)
        DLTDFL(3) =  DPRPMT(3)
      ELSE
        CALL R_5200( JN )
      ENDIF
c     write(*,*) 'EI_350  DLTDFL =', DLTDFL
C
C ローレンツ変換
      CALL R_5300
c     write(*,*) 'EI_350  DANABR =', DANABR
C
C 章動、歳差の補正
      CALL R_5400
c     write(*,*) 'EI_350  DPRNUT =', DPRNUT
C
C 計算結果をコモンへ設定する
      DO 1000 JC = 1 , 3 , 1
        DGCAPS( JC ) = DPRNUT( JC )
 1000 CONTINUE
c     write(*,*)'EI_350 DGCAPS= ', DGCAPS
C
  100 CONTINUE
C
      RETURN
C
C----------------------------------------------------------------------C
C ERROR ENTRY                                                          C
C----------------------------------------------------------------------C
 9000 CONTINUE
      JERCOD = 4
      WRITE(CT(1:12),'(F12.5)') DTDBJD
      CM = CW//' TDB ='//CT
      CALL O_MSG2( IMID00, CEL, CM, IRC )
      WRITE(CW25(1:12),'(F12.5)') DTSTJD(1)
      WRITE(CW25(14:25),'(F12.5)') DTSTJD(8)
      CW25(13:13) = ' '
      CM = 'XEI306Q EI_350  EPH START AND END = '//CW25
      CALL O_MSG2( IMID00, CEL, CM, IRC )
C
C----------------------------------------------------------------------C
C EXIT                                                                 C
C----------------------------------------------------------------------C
      RETURN
      END
